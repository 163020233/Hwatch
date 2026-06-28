/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies Co., Ltd. All rights reserved
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cmath>
#include "engines/gfx/lite_m_gfx_engine.h"
#include "components/root_view.h"
#include "cmsis_os2.h"
#include "draw/draw_utils.h"
#include "draw/hw_draw_utils.h"
#include "gfx_utils/graphic_log.h"
#include "hals/display_dev.h"
#include "hals/gralloc_engines.h"
#include "imgdecode/image_load.h"
#include "imgdecode/image_load.h"
#include "imgdecode/cache_manager.h"
#include "securec.h"
#include "dfx/dfx_record.h"
#include "dfx/dfx_frame_trace.h"
#include "graphic_service.h"
#include "common/screen.h"
#ifdef __LITEOS__
#include "los_exc.h"
#endif

namespace OHOS {
#if ENABLE_VGU_ENGINE
static const uint8_t AGRB_ALPHA_SHIFT = 24;
static const uint16_t HW_PATH_PATTERN_NODE_CNT = 7;
static const uint16_t HW_PATH_STROKE_MAX_CNT = 2; // dash or arc is 2.
static const uint16_t DEGRESS_180 = 180;
static const uint16_t DEGRESS_720 = 720;
static const uint16_t VAU_CLIPRECT_WIDTH_MAX = 640;
static const uint16_t VAU_CLIPRECT_HEIGHT_MAX = 600;
static const uint16_t SRC_COMP_WIDTH_MAX = 640; // with compression
static const uint16_t SRC_UNCOMP_WIDTH_MAX = 1024; // without compression
static const uint16_t SRC_HEIGHT_MAX = 1024;
static const uint16_t SRC_WIDTH_MIN = 1;
static const uint16_t SRC_HEIGHT_MIN = 1;
#define UNUSED_COLOR 0
#define INVALID_HANDLE 0

static bool CheckSrcDimension(const BufferInfo& buffer)
{
    uint16_t widthMax = SRC_UNCOMP_WIDTH_MAX;
#ifdef VERSION_IOT
    if (buffer.compressMode != COMPRESS_MODE_NONE) {
        widthMax = SRC_COMP_WIDTH_MAX;
    }
#endif
    if ((buffer.width < SRC_WIDTH_MIN) || (buffer.width > widthMax) ||
        (buffer.height < SRC_HEIGHT_MIN) || (buffer.height > SRC_HEIGHT_MAX)) {
        GRAPHIC_LOGE("buffer's width and height[%u, %d] is not in valid range: Width[%d, %d] Height[%d, %d]",
            buffer.width, buffer.height, SRC_WIDTH_MIN, widthMax,
            SRC_HEIGHT_MIN, SRC_HEIGHT_MAX);
        return false;
    }
    return true;
}

static uint64_t GetThreadId()
{
#ifdef _WIN32
    return GetCurrentThreadId();
#else
    return reinterpret_cast<uint64_t>(osThreadGetId());
#endif
}

static VGUBlendType Convert2VGUBlendType(const BlendMode& mode)
{
    VGUBlendType type = BLEND_BUTT;
    switch (mode) {
        case BlendMode::BLEND_SRC:
            type = BlendType::BLEND_SRC;
            break;
        case BlendMode::BLEND_DST:
            type = BlendType::BLEND_DST;
            break;
        case BlendMode::BLEND_SRC_OVER:
            type = BlendType::BLEND_SRCOVER;
            break;
        case BlendMode::BLEND_DST_OVER:
            type = BlendType::BLEND_DSTOVER;
            break;
        case BlendMode::BLEND_SRC_IN:
            type = BlendType::BLEND_SRCIN;
            break;
        case BlendMode::BLEND_DST_IN:
            type = BlendType::BLEND_DSTIN;
            break;
        case BlendMode::BLEND_SRC_OUT:
            type = BlendType::BLEND_SRCOUT;
            break;
        case BlendMode::BLEND_DST_OUT:
            type = BlendType::BLEND_DSTOUT;
            break;
        case BlendMode::BLEND_ADDITIVE:
            type = BlendType::BLEND_ADD;
            break;
        case BlendMode::BLEND_SRC_ATOP:
            type = BlendType::BLEND_SRCATOP;
            break;
        case BlendMode::BLEND_DST_ATOP:
            type = BlendType::BLEND_DSTATOP;
            break;
        default:
            GRAPHIC_LOGE("unsupported BlendMode: %u.", static_cast<uint8_t>(mode));
            break;
    }
    return type;
}

static float Deg2Rad(float degrees)
{
    return (degrees * M_PI / 180.0f);
}

bool LiteMGfxEngine::Convert2VGUCmpMode(const uint8_t& cmpMode, VGUCompressMode& dstcmpMode) const
{
    switch (cmpMode) {
        case COMPRESS_MODE_NONE:
            dstcmpMode = VGU_COMPRESS_NONE;
            break;
        case COMPRESS_MODE_HFBC:
            dstcmpMode = VGU_COMPRESS_HFBC;
            break;
        case COMPRESS_MODE_HFBC_ABYPASS:
            dstcmpMode = VGU_COMPRESS_HFBC_ABYPASS;
            break;
        case COMPRESS_MODE_HFBC_2X:
            dstcmpMode = VGU_COMPRESS_HFBC_2X;
            break;
        case COMPRESS_MODE_HFBC_3X:
            dstcmpMode = VGU_COMPRESS_HFBC_3X;
            break;
        case COMPRESS_MODE_HIMC:
            dstcmpMode = VGU_COMPRESS_HIMC;
            break;
        default:
            GRAPHIC_LOGE("unsupported compress mode: %u", cmpMode);
            return false;
    }
    return true;
}

static uint32_t GetColorWithAlpha(uint32_t color, uint8_t alpha)
{
    uint8_t mixedAlpha = ((color >> AGRB_ALPHA_SHIFT) * alpha) / 0Xff;
    uint32_t mixedColor = (mixedAlpha << AGRB_ALPHA_SHIFT) | (color & 0x00FFFFFF);
    return mixedColor;
}

static Matrix4<float> GetRotateMatrix(uint8_t rotateType, int16_t w, int16_t h)
{
    Matrix4<float> mat; // mat[col][row]
    switch (rotateType) {
        case GRAPHIC_BUFFER_ROTATE_90:
            mat[0][0] = 0; mat[1][0] = -1; mat[3][0] = h; // 3: index
            mat[0][1] = 1; mat[1][1] = 0; mat[3][1] = 0; // 3: index
            break;
        case GRAPHIC_BUFFER_ROTATE_180:
            mat[0][0] = -1; mat[1][0] = 0; mat[3][0] = w; // 3: index
            mat[0][1] = 0; mat[1][1] = -1; mat[3][1] = h; // 3: index
            break;
        case GRAPHIC_BUFFER_ROTATE_270:
            mat[0][0] = 0; mat[1][0] = 1; mat[3][0] = 0; // 3: index
            mat[0][1] = -1; mat[1][1] = 0; mat[3][1] = w; // 3: index
            break;
        default:
            if (rotateType != 0) {
                GRAPHIC_LOGE("error rotType! %u", rotateType);
            }
    }
    return mat;
}

static Matrix4<float> GetRotateMatrixReverse(uint8_t rotateType, int16_t w, int16_t h)
{
    Matrix4<float> mat; // mat[col][row]
    switch (rotateType) {
        case GRAPHIC_BUFFER_ROTATE_90:
            mat[0][0] = 0; mat[1][0] = 1; mat[3][0] = 0; // 3: index
            mat[0][1] = -1; mat[1][1] = 0; mat[3][1] = h; // 3: index
            break;
        case GRAPHIC_BUFFER_ROTATE_180:
            mat[0][0] = -1; mat[1][0] = 0; mat[3][0] = w; // 3: index
            mat[0][1] = 0; mat[1][1] = -1; mat[3][1] = h; // 3: index
            break;
        case GRAPHIC_BUFFER_ROTATE_270:
            mat[0][0] = 0; mat[1][0] = -1; mat[3][0] = w; // 3: index
            mat[0][1] = 1; mat[1][1] = 0; mat[3][1] = 0; // 3: index
            break;
        default:
            if (rotateType != 0) {
                GRAPHIC_LOGE("error rotType! %u", rotateType);
            }
    }
    return mat;
}

static bool IsTranslateMatrix(Matrix4<float> matrix)
{
    if (FloatEqual(matrix[0][0], 1) && FloatEqual(matrix[0][1], 0) && FloatEqual(matrix[0][3], 0) &&  // 3: index
        FloatEqual(matrix[1][0], 0) && FloatEqual(matrix[1][1], 1) && FloatEqual(matrix[1][3], 0) &&  // 3: index
        FloatEqual(matrix[3][3], 1)) { // 3: index
        return true;
    }
    return false;
}

static Rect GetRotateRect(uint8_t rotateType, int16_t w, int16_t h, const Rect &rect)
{
    int16_t x = rect.GetX();
    int16_t y = rect.GetY();
    int16_t x1 = rect.GetX();
    int16_t y1 = rect.GetY();
    int16_t w1 = rect.GetWidth();
    int16_t h1 = rect.GetHeight();
    Rect ret = rect;
    switch (rotateType) {
        case GRAPHIC_BUFFER_ROTATE_90:
            x1 = h - y - h1;
            y1 = x;
            ret.SetPosition(x1, y1);
            ret.SetWidth(h1);
            ret.SetHeight(w1);
            break;
        case GRAPHIC_BUFFER_ROTATE_180:
            x1 = w - x - w1;
            y1 = h - y - h1;
            ret.SetPosition(x1, y1);
            ret.SetWidth(w1);
            ret.SetHeight(h1);
            break;
        case GRAPHIC_BUFFER_ROTATE_270:
            x1 = y;
            y1 = w - x - w1;
            ret.SetPosition(x1, y1);
            ret.SetWidth(h1);
            ret.SetHeight(w1);
            break;
        default:
            if (rotateType != 0) {
                GRAPHIC_LOGE("error rotType! %u", rotateType);
            }
    }

    return ret;
}

static bool UpdateRectWithTranslateMat(const Rect &boxRect, Rect &srcRect, Rect &dstRect)
{
    Rect intercet;
    if (!intercet.Intersect(boxRect, dstRect)) {
        return false;
    }
    int16_t x = srcRect.GetX() + intercet.GetX() - boxRect.GetX();
    int16_t y = srcRect.GetY() + intercet.GetY() - boxRect.GetY();
    int16_t w = intercet.GetWidth();
    int16_t h = intercet.GetHeight();
    srcRect.SetPosition(x, y);
    srcRect.SetWidth(w);
    srcRect.SetHeight(h);
    dstRect = intercet;
    return true;
}

static void VGUMatrix3Rotate(VGUMatrix3* matrix, float angle, int16_t translateX, int16_t translateY)
{
    if (matrix == nullptr) {
        return;
    }

    float a = angle * UI_PI / 180.0f; /* * M_PI / 180: deg to rad */
    float n[3][3] = {{cos(a), -sin(a), translateX}, {sin(a), cos(a), translateY}, {0, 0, 1}};

    float t[3][3];
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            t[i][j] = n[i][0] * matrix->m[0][j] +
                      n[i][1] * matrix->m[1][j] +
                      n[i][2] * matrix->m[2][j]; /* 2 is index */
        }
    }
    for (int i = 0; i < 3; i++) { /* 3 * 3 matrix */
        for (int j = 0; j < 3; j++) { /* 3 * 3 matrix */
            matrix->m[i][j] = t[i][j];
        }
    }
}

static void RecalculateFillArea(BufferInfo& dst, VGURect& fillArea, const Rect& mask)
{
    if ((DisplayDev::GetInstance()->GetRotateMethod() != RotateMethod::ROTATE_METHOD_PRE_ROTATE) ||
        dst.rotType == GRAPHIC_BUFFER_ROTATE_0) {
        fillArea = {static_cast<float>(mask.GetLeft()), static_cast<float>(mask.GetTop()),
            static_cast<float>(mask.GetWidth()), static_cast<float>(mask.GetHeight())};
        return;
    }
    Rect boxRect = GetRotateRect(dst.rotType, dst.width, dst.height, mask);
    fillArea = {static_cast<float>(boxRect.GetLeft()), static_cast<float>(boxRect.GetTop()),
        static_cast<float>(boxRect.GetWidth()), static_cast<float>(boxRect.GetHeight())};
    return;
}

static void RecalculateFillAreaAndMatrix(BufferInfo& dst, VGURect& fillArea, const Rect& mask, VGUMatrix3* matrix)
{
    RecalculateFillArea(dst, fillArea, mask);
#if ENABLE_VGU_ENGINE
    if ((DisplayDev::GetInstance()->GetRotateMethod() != RotateMethod::ROTATE_METHOD_PRE_ROTATE) ||
        dst.rotType == GRAPHIC_BUFFER_ROTATE_0) {
        return;
    }
    int16_t xTranslate = 0;
    int16_t yTranslate = 0;
    if (dst.rotType == GRAPHIC_BUFFER_ROTATE_90) {
        xTranslate = dst.height;
    } else if (dst.rotType == GRAPHIC_BUFFER_ROTATE_180) {
        xTranslate = dst.width;
        yTranslate = dst.height;
    } else if (dst.rotType == GRAPHIC_BUFFER_ROTATE_270) {
        yTranslate = dst.width;
    }
    float rotateAngle = static_cast<float>((QUARTER_IN_DEGREE * dst.rotType) % CIRCLE_IN_DEGREE);
    VGUMatrix3Rotate(matrix, rotateAngle, xTranslate, yTranslate);
#endif
}

bool LiteMGfxEngine::HandleImageSrc(const Image* imgSrc, VGUImage& image, const Style& style)
{
    ImageInfo imgInfo;
    if (imgSrc->GetSrcType() == IMG_SRC_FILE) {
        CacheEntry entry;
        if (CacheManager::GetInstance().Open(imgSrc->GetPath(), style, entry) != RetCode::OK) {
            return false;
        }
        imgInfo = entry.GetImageInfo();
    } else {
        memcpy_s(&imgInfo, sizeof(ImageInfo), imgSrc->GetImageInfo(), sizeof(ImageInfo));
    }

    image.buffer.width = imgInfo.header.width;
    image.buffer.height = imgInfo.header.height;
    image.buffer.memHandle = static_cast<uint64_t>(uintptr_t(imgInfo.phyAddr));
    Convert2VGUFormat(static_cast<ColorMode>(imgInfo.header.colorMode), image.buffer.pixelFormat);

    if (imgInfo.header.compressMode == false) {
        image.buffer.stride = DisplayDev::GetInstance()->CalcStride(image.buffer.width, image.buffer.pixelFormat);
    } else {
        image.enCompressed = true;
    }
    return true;
}

void LiteMGfxEngine::AddHwDraw(uint16_t nodeCnt, uint16_t strokeCnt)
{
    isHwDrawPending_ = true;
    hwDrawPendingCnt_ += nodeCnt;
    hwStrokePendingCnt_ += strokeCnt;
}

void LiteMGfxEngine::CheckHwDrawLimit(uint16_t nodeCnt, uint16_t strokeCnt)
{
    if ((hwStrokePendingCnt_ + strokeCnt >= STROKE_MAX_CNT) ||
        (hwDrawPendingCnt_ + nodeCnt >= DRAW_MAX_CNT)) {
        SyncHwDraw();
    }
}

void LiteMGfxEngine::Fill(BufferInfo& dst, const Rect& fillArea, const ColorType color, const OpacityType opacity)
{
#if !CAPABILITY_HWDRAW_SMALL_SIZE
    if (fillArea.GetSize() < HARDWARE_ACC_SIZE_LIMIT) {
        SyncHwDraw();
        FRAME_TRACE_START(FrameTraceType::SW_DRAW_RECT);
        SoftEngine::Fill(dst, fillArea, color, opacity);
        FRAME_TRACE_END(FrameTraceType::SW_DRAW_RECT, &fillArea);
        return;
    }
#endif

    BlitImages();

    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    surface_.clipNum = 0;
    surface_.clipRects[0] = {0, 0, 0, 0};
    uint32_t clearColor = Color::ColorTo32(color);
    RecalculateFillArea(dst, surface_.clipRects[0], fillArea);
    CheckHwDrawLimit(1);

    int ret;
    if (opacity == 0xff) {
        VGUSurface subSurface = CreateSubSurface(surface_, nullptr);
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_RECT);
        ret = vguFuncs_->RenderClearRect(&subSurface, &subSurface.clipRects[0], clearColor);
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_RECT, &fillArea);
    } else {
        VGUImage image;
        memset_s(&image, sizeof(VGUImage), 0, sizeof(VGUImage));
        image.buffer.isSolid = true;
        image.buffer.color = clearColor;
        image.blend = BLEND_SRCOVER;
        image.globalAlpha = opacity;
        image.rect.w = surface_.clipRects[0].w;
        image.rect.h = surface_.clipRects[0].h;
        image.dstRect = surface_.clipRects[0];
        image.buffer.pixelFormat = PIXEL_FMT_RGBA_8888;
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_IMGBLIT);
        ret = vguFuncs_->RenderBlit(&surface_, &image, 1);
        Rect num = {1, 0, 0, 0};
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_IMGBLIT, &num);
    }
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("Fill operation failed.");
    } else {
        AddHwDraw(1);
    }
}

bool LiteMGfxEngine::Convert2VGUBuffer(const BufferInfo& bufferInfo, VGUBuffer& buffer) const
{
    if (!Convert2VGUFormat(bufferInfo.mode, buffer.pixelFormat)) {
        return false;
    }
    if (!Convert2VGUCmpMode(bufferInfo.compressMode, buffer.compressMode)) {
        return false;
    }

    if (bufferInfo.rotType == GRAPHIC_BUFFER_ROTATE_0 || bufferInfo.rotType == GRAPHIC_BUFFER_ROTATE_180) {
        buffer.memHandle = static_cast<uint64_t>((uintptr_t)bufferInfo.phyAddr);
        buffer.width = bufferInfo.width;
        buffer.height = bufferInfo.height;
        buffer.stride = bufferInfo.stride;
        buffer.uvOffset = bufferInfo.uvOffset;
    } else {
        buffer.memHandle = static_cast<uint64_t>((uintptr_t)bufferInfo.phyAddr);
        buffer.width = bufferInfo.height;
        buffer.height = bufferInfo.width;
        buffer.stride =
            DisplayDev::GetInstance()->CalcStride(buffer.width, buffer.pixelFormat, bufferInfo.compressMode);
        buffer.uvOffset = bufferInfo.uvOffset;
    }
    buffer.color = bufferInfo.color;

    return true;
}

void LiteMGfxEngine::DrawRect(BufferInfo& dst, const Rect& rect,
    const Rect& dirtyRect, const Style& style, OpacityType opacity)
{
    DrawRect(dst, rect, dirtyRect, style, opacity, nullptr);
}

static void Convert2Matrix(VGUMatrix3& matrix, TransformMap* transMap)
{
    if (transMap != nullptr) {
        Matrix3<float> viewMatrix;
        const float* data = transMap->GetOrigTransformMatrix().GetData();
        // 0, 1, 3, 4, 5, 7, 12, 13: matrix data index
        viewMatrix = Matrix3<float>(data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], 1);
        HWDrawUtils::GetInstance()->Matrix3ToVguMatrix3(viewMatrix, matrix);
    }
}

void LiteMGfxEngine::DrawRect(BufferInfo& dst, const Rect& rect,
    const Rect& dirtyRect, const Style& style, OpacityType opacity, TransformMap* transMap)
{
    VGUMatrix3 matrix;
    Convert2Matrix(matrix, transMap);
    VGUMatrix3* matrixPtr = (transMap == nullptr) ? nullptr : &matrix;
    uint16_t bgOpa = DrawUtils::GetMixOpacity(opacity, style.bgOpa_);
    uint16_t borderOpa = DrawUtils::GetMixOpacity(opacity, style.borderOpa_);
    if ((bgOpa == OPA_TRANSPARENT) && ((style.borderWidth_ == 0) || (borderOpa == OPA_TRANSPARENT))) {
        return;
    }

    Rect fillArea;
    Rect rectIn = transMap != nullptr ? transMap->GetBoxRect() : rect;
    if (!fillArea.Intersect(rectIn, dirtyRect)) {
        return;
    }

#if !CAPABILITY_HWDRAW_RECT
    if ((borderOpa != OPA_TRANSPARENT) && (style.borderWidth_ != 0)) {
        SyncHwDraw();
        FRAME_TRACE_START(FrameTraceType::SW_DRAW_RECT);
        SoftEngine::DrawRect(dst, rect, dirtyRect, style, opacity);
        FRAME_TRACE_END(FrameTraceType::SW_DRAW_RECT, &rect);
        return;
    }
#endif
    Rect rectHw{rect};
    Style roundStyle(style);
    int16_t bottomRadius = style.GetStyle(STYLE_BOTTOM_BORDER_RADIUS);
    int16_t topRadius = style.GetStyle(STYLE_TOP_BORDER_RADIUS);
    if (style.borderWidth_ != 0) {
        int16_t widthAdjust = (style.borderWidth_ + 1) / 2; // 2: divide by 2
        rectHw.SetLeft(rect.GetLeft() + widthAdjust);
        rectHw.SetTop(rect.GetTop() + widthAdjust);
        rectHw.SetRight(rect.GetRight() - widthAdjust);
        rectHw.SetBottom(rect.GetBottom() - widthAdjust);

        bottomRadius = (bottomRadius == 0 ? 0 : (bottomRadius > widthAdjust ? (bottomRadius - widthAdjust) : 1));
        topRadius = (topRadius == 0 ? 0 : (topRadius > widthAdjust ? (topRadius - widthAdjust) : 1));
    }

    if ((bottomRadius == 0 && topRadius == 0) || (rectHw.GetWidth() <= 1 || rectHw.GetHeight() <= 1)) {
        if (matrixPtr != nullptr || IsCurrentThreadCaching() || style.borderWidth_ != 0) {
            DrawRectWithoutRound(dst, rectHw, style, dirtyRect, opacity, matrixPtr);
        } else {
            Fill(dst, fillArea, style.bgColor_, bgOpa);
        }
    } else {
        uint16_t shortSide = MATH_MIN(rectHw.GetWidth(), rectHw.GetHeight());
        int16_t minRadius = shortSide / 2; // 2: half
        bottomRadius = MATH_MIN(minRadius, bottomRadius);
        topRadius = MATH_MIN(minRadius, topRadius);
        roundStyle.SetStyle(STYLE_BOTTOM_BORDER_RADIUS, bottomRadius);
        roundStyle.SetStyle(STYLE_TOP_BORDER_RADIUS, topRadius);
        DrawRectWithRound(dst, rectHw, roundStyle, dirtyRect, opacity, matrixPtr);
    }
}

void LiteMGfxEngine::DrawArcInner(BufferInfo& dst, const ArcInfo& arcInfo, const Rect& mask,
    const Style& style, OpacityType opacity, uint8_t cap, VGUImage& image)
{
    ArcInfo arcInfoHw{arcInfo};
    while (FloatLess(arcInfoHw.endAngle, arcInfoHw.startAngle)) {
        arcInfoHw.endAngle += CIRCLE_IN_DEGREE;
    }

    Path path = {0};
    arcInfoHw.radius = arcInfo.radius - (style.lineWidth_ + 1) / 2; // 2: divide by 2
    if (!HWDrawUtils::GetInstance()->ObtainArcPath(arcInfoHw, path.cmdNum, path.cmds, path.dataNum, path.data)) {
        return;
    }
    uintptr_t vguPath = InitPath(true);
    SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);

    SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_ARC, FrameTracePathAttr::STROKE);
    if (arcInfoHw.imgSrc != nullptr && arcInfoHw.imgSrc->GetSrcType() != IMG_SRC_UNKNOWN) {
        image.matrix.m[0][0] = 1;
        image.matrix.m[1][1] = 1;
        image.matrix.m[2][2] = 1; // 2: index
        image.matrix.m[0][2] = arcInfo.imgPos.x; // 0, 2: index
        image.matrix.m[1][2] = arcInfo.imgPos.y; // 1, 2: index
        image.buffer.color = opacity << AGRB_ALPHA_SHIFT;
        VGUStrokeAttr strokeAttr = {static_cast<VGULineCap>(cap)};
        VGUPaintStyle strokeStyle = {0};
        VGUPattern pattern = {0};
        pattern.image = &image;
        strokeStyle.type = VGU_PAINT_PATTERN;
        strokeStyle.pattern = &pattern;
        strokeAttr.width = style.lineWidth_;
        strokeAttr.style = &strokeStyle;
        RenderPathInternal(dst, vguPath, mask, &strokeAttr, nullptr, nullptr);
    } else {
        VGUStrokeAttr strokeAttr = {static_cast<VGULineCap>(cap)};
        VGUPaintStyle strokeStyle = {0};
        strokeStyle.type = VGU_PAINT_COLOR;
        strokeStyle.color = GetColorWithAlpha(Color::ColorTo32(style.lineColor_), opacity);
        strokeAttr.width = style.lineWidth_;
        strokeAttr.style = &strokeStyle;
        RenderPathInternal(dst, vguPath, mask, &strokeAttr, nullptr, nullptr);
    }
    pathRender_.PushBack(path);
    DeinitPath(vguPath);
}

void LiteMGfxEngine::DrawArc(BufferInfo& dst, ArcInfo& arcInfo, const Rect& mask,
    const Style& style, OpacityType opacity, uint8_t cap)
{
    OpacityType opa = DrawUtils::GetMixOpacity(opacity, style.lineOpa_);
    if ((opa == OPA_TRANSPARENT) || (style.lineWidth_ < 1)) {
        return;
    }

    Rect coordsTmp;
    coordsTmp.SetPosition(arcInfo.center.x - arcInfo.radius, arcInfo.center.y - arcInfo.radius);
    coordsTmp.SetHeight(arcInfo.radius * 2); // d = 2 * r
    coordsTmp.SetWidth(arcInfo.radius * 2); // d = 2 * r
    if (!coordsTmp.Intersect(mask, coordsTmp)) {
        return;
    }

#if !CAPABILITY_HWDRAW_ARC
    SyncHwDraw();
    FRAME_TRACE_START(FrameTraceType::SW_DRAW_ARC);
    SoftEngine::DrawArc(dst, arcInfo, mask, style, opacity, cap);
    FRAME_TRACE_END(FrameTraceType::SW_DRAW_ARC, &mask);
    return;
#endif

    if (FloatEqual(arcInfo.startAngle, arcInfo.endAngle)) {
        return;
    }

    VGUImage image;
    (void)memset_s(&image, sizeof(VGUImage), 0, sizeof(VGUImage));
    if (arcInfo.imgSrc != nullptr && arcInfo.imgSrc->GetSrcType() != IMG_SRC_UNKNOWN) {
        if (HandleImageSrc(arcInfo.imgSrc, image, style) == false) {
            GRAPHIC_LOGE("handle image src failed.");
            return;
        }
    }

    BlitImages();

    DrawArcInner(dst, arcInfo, mask, style, opa, cap, image);
}

void LiteMGfxEngine::DrawLine(BufferInfo& dst, const Point& start, const Point& end,
    const Rect& mask, int16_t width, ColorType color, OpacityType opacity)
{
    if ((width == 0) || (opacity == OPA_TRANSPARENT)) {
        return;
    }

#if !CAPABILITY_HWDRAW_LINE
    SyncHwDraw();
    FRAME_TRACE_START(FrameTraceType::SW_DRAW_LINE);
    SoftEngine::DrawLine(dst, start, end, mask, width, color, opacity);
    FRAME_TRACE_END(FrameTraceType::SW_DRAW_LINE, &mask);
    return;
#endif

    BlitImages();

    Path path = {0};
    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(&path, 2, 4)) { // 2, 4: path cmd and data num
        GRAPHIC_LOGE("Alloc buffer for path failed.");
        return;
    }

    path.data[0] = start.x;
    path.data[1] = start.y;
    path.data[2] = end.x; // 2: index
    path.data[3] = end.y; // 3: index

    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_LINE_TO;

    uintptr_t vguPath = InitPath(true);
    SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT};
    VGUPaintStyle strokeStyle = {0};
    strokeStyle.type = VGU_PAINT_COLOR;
    strokeStyle.color = GetColorWithAlpha(Color::ColorTo32(color), opacity);
    strokeAttr.width = width;
    strokeAttr.style = &strokeStyle;

    SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_LINE, FrameTracePathAttr::STROKE);
    RenderPathInternal(dst, vguPath, mask, &strokeAttr, nullptr, nullptr);

    pathRender_.PushBack(path);
    DeinitPath(vguPath);
}

void LiteMGfxEngine::DrawEllipse(BufferInfo& dst, const Rect& mask, const Point& center, uint16_t rx, uint16_t ry,
    VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, VGUMatrix3* matrix)
{
    if (rx == 0 || ry == 0) {
        return;
    }

    BlitImages();

    uint16_t w = rx;
    uint16_t h = ry;
    if (strokeAttr != nullptr) {
        w -= (strokeAttr->width + 1) / 2; // 2: divide by 2
        h -= (strokeAttr->width + 1) / 2; // 2: divide by 2
    }

    Path path = {0};
    if (!HWDrawUtils::GetInstance()->ObtainEllipsePath(center, w, h, path)) {
        GRAPHIC_LOGE("Obtain Ellipse Path failed.");
        return;
    }

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);

    SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_ELLIPSE,
        static_cast<FrameTracePathAttr>(((strokeAttr != nullptr) << 1) | (fillAttr != nullptr)));
    RenderPathInternal(dst, vguPath, mask, strokeAttr, fillAttr, matrix);

    pathRender_.PushBack(path);
    DeinitPath(vguPath);
}

void LiteMGfxEngine::DrawLetter(BufferInfo& gfxDstBuffer, const uint8_t* fontMap,
    const Rect& fontRect, const Rect& subRect, const uint8_t fontWeight,
    const ColorType& color, const OpacityType opa)
{
    // avoid to draw space letter, or other logcial error of native code.
    if ((subRect.GetWidth() <= 0) || (subRect.GetHeight() <= 0)) {
        return;
    }

#if CAPABILITY_HWDRAW_ALPHA_BITMAP
    BufferInfo src;
    src.rect = {subRect.GetX() - fontRect.GetX(),
                subRect.GetY() - fontRect.GetY(),
                subRect.GetRight() - fontRect.GetX(),
                subRect.GetBottom() - fontRect.GetY()};
    src.virAddr = static_cast<void*>(const_cast<uint8_t*>(fontMap));
    src.phyAddr = static_cast<void*>(const_cast<uint8_t*>(fontMap));
    switch (fontWeight) {
        case FONT_WEIGHT_1:
            src.mode = A1;
            break;
        case FONT_WEIGHT_2:
            src.mode  = A2;
            break;
        case FONT_WEIGHT_4:
            src.mode  = A4;
            break;
        case FONT_WEIGHT_8:
            src.mode  = A8;
            break;
        default:
            GRAPHIC_LOGE("font weight %u was not support", fontWeight);
            return;
    }
    src.width = fontRect.GetWidth();
    src.stride = ALIGN_BYTE(src.width, BYTE_ALIGNMENT);
    src.height = fontRect.GetHeight();
    src.compressMode = COMPRESS_MODE_NONE;
    src.color = color.full;
    src.rotType = 0;
    BlendOption blendOption;
    blendOption.opacity = opa;
    blendOption.transMap.Translate(Vector2<int16_t>(subRect.GetX(), subRect.GetY()));
    blendOption.mode = BLEND_SRC_OVER;
    Blit(gfxDstBuffer, {0, 0}, src, subRect, blendOption);
#else
    SyncHwDraw();
    FRAME_TRACE_START(FrameTraceType::SW_DRAW_LETTER);
    SoftEngine::DrawLetter(gfxDstBuffer, fontMap, fontRect, subRect, fontWeight, color, opa);
    FRAME_TRACE_END(FrameTraceType::SW_DRAW_LETTER, &fontRect);
#endif
}

#if CAPABILITY_HWDRAW_PATH
uintptr_t LiteMGfxEngine::InitPath(bool enAlias)
{
    if ((hwDrawPendingCnt_ + HW_PATH_PATTERN_NODE_CNT * 2 >= DRAW_MAX_CNT) || // 2: both fill and stroke use pattern
        (hwStrokePendingCnt_ + HW_PATH_STROKE_MAX_CNT >= STROKE_MAX_CNT)) {
        SyncHwDraw();
    }
    uintptr_t path = vguFuncs_->VGUPathInit(VGU_DATA_TYPE_F32, enAlias);
    return path;
}

void LiteMGfxEngine::DeinitPath(uintptr_t path) const
{
    vguFuncs_->VGUPathClear(path);
}

bool LiteMGfxEngine::AppendPathData(uintptr_t path, uint32_t num, const uint8_t* cmd, uint32_t dataNum,
    const float* data) const
{
    VGUResult ret = vguFuncs_->VGUPathAppendData(path, num, cmd, dataNum, data);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("VGUPathAppendData failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetPathData(uintptr_t path, uint32_t num, const uint8_t* cmd, uint32_t dataNum,
    const float* data) const
{
    VGUResult ret = vguFuncs_->VGUPathSetData(path, num, cmd, dataNum, data);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("VGUPathSetData failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::IsCacheRenderSupport(void)
{
    return vguFuncs_->IsCacheRenderSupport();
}

uint32_t LiteMGfxEngine::InitRenderCache(int32_t renderNum)
{
    uint32_t cacheKey = vguFuncs_->InitRenderCache(renderNum);
    if (cacheKey == 0) {
        GRAPHIC_LOGE("InitPathCache failed.");
        return 0;
    }
    return cacheKey;
}

void LiteMGfxEngine::DeinitRenderCache(uint32_t cacheKey)
{
    for (auto it = cacheKeyByThreadId_.begin(); it != cacheKeyByThreadId_.end();) {
        if (it->second == cacheKey) {
            it = cacheKeyByThreadId_.erase(it);
        } else {
            ++it;
        }
    }
    vguFuncs_->DeinitRenderCache(cacheKey);
}

bool LiteMGfxEngine::IsCacheKeyValidForCurrentThread(uint32_t cacheKey) const
{
    auto it = cacheKeyByThreadId_.find(GetThreadId());
    return (it != cacheKeyByThreadId_.end() && it->second == cacheKey);
}

bool LiteMGfxEngine::IsCurrentThreadCaching() const
{
    return cacheKeyByThreadId_.find(GetThreadId()) != cacheKeyByThreadId_.end();
}

bool LiteMGfxEngine::CacheRenderGroupBegin(uint32_t cacheKey,
    VGURenderGroupShareType preShareType, VGURenderGroupShareType subShareType)
{
    VGUResult ret = vguFuncs_->CacheRenderGroupBegin(cacheKey, preShareType, subShareType);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("CacheRenderGroupBegin failed.");
        return false;
    }
    cacheKeyByThreadId_[GetThreadId()] = cacheKey;
    return true;
}

uint16_t LiteMGfxEngine::CacheRenderGroupEnd(uint32_t cacheKey)
{
    if (!IsCacheKeyValidForCurrentThread(cacheKey)) {
        GRAPHIC_LOGE("CacheRenderGroupEnd called with invalid cacheKey.");
        return 0;
    }
    cacheKeyByThreadId_.erase(GetThreadId());
    return vguFuncs_->CacheRenderGroupEnd(cacheKey);
}

bool LiteMGfxEngine::CacheOnePath(BufferInfo& dst, const Rect& mask,
    VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, VGUMatrix3* matrix)
{
    BlitImages();
    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return false;
    }

    surface_.clipNum = 1;
    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface_.clipRects[0], mask, &tempMatrix);
    VGUSurface subSurface = CreateSubSurface(surface_, &tempMatrix);

    uint32_t cacheKey = cacheKeyByThreadId_[GetThreadId()];
    VGUResult ret = vguFuncs_->CacheOnePath(cacheKey, &subSurface, &tempMatrix, fillAttr, strokeAttr);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("CacheOnePath failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetGroupRenderBufferAddr(uint32_t cacheKey, uint32_t nodeId, void* phyAddr) const
{
    uint64_t memHandle = static_cast<uint64_t>(uintptr_t(phyAddr));

    VGUResult ret = vguFuncs_->SetGroupRenderBufferAddr(cacheKey, nodeId, memHandle);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetGroupRenderBufferAddr failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetGroupRenderRect(uint32_t cacheKey, uint32_t nodeId, const Rect& mask) const
{
    VGURect clipRect = {static_cast<float>(mask.GetLeft()), static_cast<float>(mask.GetTop()),
        static_cast<float>(mask.GetWidth()), static_cast<float>(mask.GetHeight())};

    VGUResult ret = vguFuncs_->SetGroupRenderRect(cacheKey, nodeId, &clipRect);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetGroupRenderRect failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetGroupRenderBuffer(uint32_t cacheKey, uint32_t nodeId,
    BufferInfo& dst, const Rect& mask, VGUMatrix3 *matrix) const
{
    VGUSurface surface;
    if (!Convert2VGUBuffer(dst, surface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return false;
    }

    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface.clipRects[0], mask, &tempMatrix);
    VGUSurface subSurface = CreateSubSurface(surface, &tempMatrix);
    VGUResult ret = vguFuncs_->SetGroupRenderBuffer(cacheKey, nodeId, &subSurface.buffer, &subSurface.clipRects[0]);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetGroupRenderBufferAddr failed.");
        return false;
    }
    ret = vguFuncs_->SetGroupRenderMatrix(cacheKey, nodeId, &tempMatrix);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetGroupRenderMatrix failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetGroupRenderMatrix(uint32_t cacheKey, uint32_t nodeId, const VGUMatrix3 *matrix) const
{
    VGUResult ret = vguFuncs_->SetGroupRenderMatrix(cacheKey, nodeId, matrix);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetGroupRenderMatrix failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::TranslateRenderNode(uint32_t cacheKey, uint32_t nodeId, int16_t offsetX, int16_t offsetY)
{
    VGUResult ret = vguFuncs_->TranslateRenderNode(cacheKey, nodeId, offsetX, offsetY);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("TranslateRenderCache failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::SetRenderNodeOpa(uint32_t cacheKey, uint32_t nodeId, uint8_t opa)
{
    VGUResult ret = vguFuncs_->SetRenderNodeOpa(cacheKey, nodeId, opa);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("SetRenderNodeOpa failed.");
        return false;
    }
    return true;
}

bool LiteMGfxEngine::DoRenderByCache(uint32_t cacheKey)
{
    SyncHwDraw(true);
    VGUResult ret = vguFuncs_->DoRenderByCache(cacheKey);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("DoRenderByCache failed.");
        return false;
    }
    return true;
}
#endif

void LiteMGfxEngine::FillPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
    ColorType color, OpacityType opacity, VGUMatrix3 *matrix)
{
    BlitImages();

    VGUFillAttr fillAttr = {VGU_RULE_WINDING};
    VGUPaintStyle style = {{0}};
    fillAttr.style = &style;
    fillAttr.style->color = GetColorWithAlpha(Color::ColorTo32(color), opacity);
    fillAttr.style->type = VGU_PAINT_COLOR;
    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }
    surface_.clipNum = 1;
    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface_.clipRects[0], mask, &tempMatrix);
    VGUSurface subSurface = CreateSubSurface(surface_, &tempMatrix);
    if (IsCurrentThreadCaching()) {
        uint32_t cacheKey = cacheKeyByThreadId_[GetThreadId()];
        VGUResult ret = vguFuncs_->CacheOnePath(cacheKey, &subSurface, &tempMatrix, &fillAttr, nullptr);
        if (ret != VGU_SUCCESS) {
            GRAPHIC_LOGE("CacheOnePath failed.");
        }
        return;
    }
    CheckHwDrawLimit(1);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_PATH);
    VGUResult ret = vguFuncs_->RenderPath(&subSurface, path, &tempMatrix, &fillAttr, nullptr);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_PATH, &mask);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("RenderPath failed. Sync previous task.");
    } else {
        AddHwDraw(1);
    }
}

void LiteMGfxEngine::StrokePath(BufferInfo& dst, uintptr_t path, const Rect& mask,
    int16_t width, ColorType color, OpacityType opacity, VGUMatrix3* matrix)
{
    BlitImages();

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT};
    VGUPaintStyle style = {{0}};
    strokeAttr.miterLimit = 4; // 4: Miter Limit
    strokeAttr.width = width;
    strokeAttr.style = &style;
    strokeAttr.style->color = GetColorWithAlpha(Color::ColorTo32(color), opacity);
    strokeAttr.style->type = VGU_PAINT_COLOR;
    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }
    surface_.clipNum = 1;
    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface_.clipRects[0], mask, &tempMatrix);
    VGUSurface subSurface = CreateSubSurface(surface_, &tempMatrix);
    CheckHwDrawLimit(1, HW_PATH_STROKE_MAX_CNT);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_PATH);
    VGUResult ret = vguFuncs_->RenderPath(&subSurface, path, &tempMatrix, nullptr, &strokeAttr);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_PATH, &mask);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("RenderPath failed. Sync previous task.");
    } else {
        AddHwDraw(1, HW_PATH_STROKE_MAX_CNT);
    }
}

void LiteMGfxEngine::RenderPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
    int16_t strokeWidth, ColorType strokeColor, ColorType fillColor,
    OpacityType opacity, VGUMatrix3* matrix)
{
    BlitImages();

    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    surface_.clipNum = 1;
    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface_.clipRects[0], mask, &tempMatrix);
    VGUSurface subSurface = CreateSubSurface(surface_, &tempMatrix);

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT};
    VGUPaintStyle strokeStyle = {{0}};
    strokeAttr.miterLimit = 4; // 4: Miter Limit
    strokeAttr.width = strokeWidth;
    strokeAttr.style = &strokeStyle;
    strokeAttr.style->color = GetColorWithAlpha(Color::ColorTo32(strokeColor), opacity);
    strokeAttr.style->type = VGU_PAINT_COLOR;

    VGUFillAttr fillAttr = {VGU_RULE_WINDING};
    VGUPaintStyle fillStyle = {{0}};
    fillAttr.style = &fillStyle;
    fillAttr.style->color = GetColorWithAlpha(Color::ColorTo32(fillColor), opacity);
    fillAttr.style->type = VGU_PAINT_COLOR;

    CheckHwDrawLimit(2, HW_PATH_STROKE_MAX_CNT); // 2: create 2 nodes

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_PATH);
    VGUResult ret = vguFuncs_->RenderPath(&subSurface, path, &tempMatrix, &fillAttr, &strokeAttr);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_PATH, &mask);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("RenderPath failed. Sync previous task.");
    } else {
        AddHwDraw(2, HW_PATH_STROKE_MAX_CNT); // 2: create 2 nodes
    }
}

void LiteMGfxEngine::RenderPathInternal(BufferInfo& dst, uintptr_t path, const Rect& mask,
    VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, VGUMatrix3* matrix)
{
    BlitImages();

    if (!Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    surface_.clipNum = 1;
    VGUMatrix3 tempMatrix;
    if (matrix == nullptr) {
        TransformMap transf;
        Convert2Matrix(tempMatrix, &transf);
    } else {
        tempMatrix = *matrix;
    }
    RecalculateFillAreaAndMatrix(dst, surface_.clipRects[0], mask, &tempMatrix);

    uint16_t nodeCnt = 0;
    uint16_t strokeCnt = 0;
    if (fillAttr != nullptr) {
        nodeCnt = (fillAttr->style->type == VGU_PAINT_PATTERN) ? nodeCnt + HW_PATH_PATTERN_NODE_CNT : nodeCnt + 1;
    }
    if (strokeAttr != nullptr) {
        nodeCnt = (strokeAttr->style->type == VGU_PAINT_PATTERN) ? nodeCnt + HW_PATH_PATTERN_NODE_CNT : nodeCnt + 1;
        strokeCnt = HW_PATH_STROKE_MAX_CNT;
    }

    VGUSurface subSurface = CreateSubSurface(surface_, &tempMatrix);
    CheckHwDrawLimit(nodeCnt, strokeCnt);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_PATH);
    VGUResult ret = vguFuncs_->RenderPath(&subSurface, path, &tempMatrix, fillAttr, strokeAttr);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_PATH, &mask);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("RenderPath failed. Sync previous task.");
    } else {
        AddHwDraw(nodeCnt, strokeCnt);
    }
}

void LiteMGfxEngine::RenderPath(BufferInfo& dst, uintptr_t path, const Rect& mask,
    VGUStrokeAttr* strokeAttr, VGUFillAttr* fillAttr, VGUMatrix3* matrix)
{
    if (IsCurrentThreadCaching()) {
        CacheOnePath(dst, mask, strokeAttr, fillAttr, matrix);
    } else {
        RenderPathInternal(dst, path, mask, strokeAttr, fillAttr, matrix);
    }
}

void LiteMGfxEngine::RenderMask(BufferInfo& dst, uintptr_t path, const Rect& mask)
{
    VGUSurface maskSurface;
    if (!Convert2VGUBuffer(dst, maskSurface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    VGURect rect;
    RecalculateFillArea(dst, rect, {0, 0, maskSurface.buffer.width - 1, maskSurface.buffer.height - 1});
    FRAME_TRACE_START(FrameTraceType::HW_DRAW_RECT);
    VGUResult ret = vguFuncs_->RenderClearRect(&maskSurface, &rect, 0x0);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_RECT, &mask);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("Clear mask buffer failed.");
        return;
    }

    VGUFillAttr fillAttr = {VGU_RULE_WINDING, nullptr};
    VGUPaintStyle fillStyle = {{0}};
    fillAttr.style = &fillStyle;
    fillAttr.style->type = VGU_PAINT_COLOR;
    fillAttr.style->color = 0xFFFFFFFF;

    maskSurface.clipNum = 1;
    VGUMatrix3 matrix;
    TransformMap transf;
    Convert2Matrix(matrix, &transf);
    RecalculateFillAreaAndMatrix(dst, maskSurface.clipRects[0], mask, &matrix);
    VGUSurface subSurface = CreateSubSurface(maskSurface, &matrix);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_PATH);
    ret = vguFuncs_->RenderPath(&subSurface, path, &matrix, &fillAttr, nullptr);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_PATH, &mask);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("RenderPath failed.");
        return;
    }
}

void LiteMGfxEngine::DrawBlurMaskByRect(BufferInfo& dst, const Rect& cropRect, const Rect &rect, const Style& style)
{
    if (!cropRect.IsValid()) {
        GRAPHIC_LOGE("mask is invalid!");
        return;
    }
    Rect rectHw(rect);
    Style roundStyle(style);
    int16_t widthAdjust = (style.borderWidth_ + 1) / 2; // 2: divide by 2
    rectHw.SetLeft(rectHw.GetLeft() + widthAdjust);
    rectHw.SetTop(rectHw.GetTop() + widthAdjust);
    rectHw.SetRight(rectHw.GetRight() - widthAdjust);
    rectHw.SetBottom(rectHw.GetBottom() - widthAdjust);
    roundStyle.borderRadius_ = (style.borderRadius_ > widthAdjust) ? (style.borderRadius_ - widthAdjust) : 1;
    roundStyle.borderRadius_ = (style.borderRadius_ == 0) ? 0 : roundStyle.borderRadius_;

    uint16_t shortSide = MATH_MIN(rectHw.GetWidth(), rectHw.GetHeight());
    if (style.borderRadius_ > (shortSide / 2)) { // 2: divide by 2
        roundStyle.borderRadius_ = shortSide / 2; // 2: divide by 2
    }
    Path blurPath = {0};
    if (!HWDrawUtils::GetInstance()->ObtainRectPathWithRadius(rectHw, blurPath, roundStyle.borderRadius_)) {
        GRAPHIC_LOGE("Obtain RoundRect Path failed.");
        return;
    }

    uintptr_t handle = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(handle, blurPath.cmdNum, blurPath.cmds,
                                               blurPath.dataNum, blurPath.data);
    LiteMGfxEngine::GetInstance()->RenderMask(dst, handle, cropRect);

    pathRender_.PushBack(blurPath);
    DeinitPath(handle);
}

void LiteMGfxEngine::DrawBlurMaskByPath(BufferInfo& dst, const Rect& cropRect, const Path& maskPath)
{
    Rect rectHw(cropRect);
    Path blurPath = {0};
    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(&blurPath, maskPath.cmdNum, maskPath.dataNum)) {
        return;
    }
    errno_t ret = memcpy_s((void *)blurPath.cmds, sizeof(uint8_t) * blurPath.cmdNum, maskPath.cmds,
        sizeof(uint8_t) * maskPath.cmdNum);
    if (ret != EOK) {
        GRAPHIC_LOGE("call memcpy_s failed!, ret = %d", ret);
        HWDrawUtils::GetInstance()->FreeBufferForPath(&blurPath);
        return;
    }
    ret = memcpy_s((void *)blurPath.data, sizeof(float) * blurPath.dataNum, maskPath.data,
        sizeof(float) * maskPath.dataNum);
    if (ret != EOK) {
        GRAPHIC_LOGE("call memcpy_s failed!, ret = %d", ret);
        HWDrawUtils::GetInstance()->FreeBufferForPath(&blurPath);
        return;
    }

    uintptr_t handle = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(handle, blurPath.cmdNum, blurPath.cmds,
                                               blurPath.dataNum, blurPath.data);
    LiteMGfxEngine::GetInstance()->RenderMask(dst, handle, rectHw);

    pathRender_.PushBack(blurPath);
    DeinitPath(handle);
}

void LiteMGfxEngine::ConvertImgInfoToVGUImage(const ImageInfo* info, VGUImage* img)
{
    memset_s(img, sizeof(VGUImage), 0, sizeof(VGUImage));
    img->buffer.isSolid = false;
    img->buffer.memHandle = static_cast<uint64_t>(uintptr_t(info->data));
    img->buffer.width = info->header.width;
    img->buffer.height = info->header.height;
    img->globalAlpha = 0xff;
    img->enCompressed = (info->header.compressMode != COMPRESS_MODE_NONE);
    Convert2VGUFormat(static_cast<ColorMode>(info->header.colorMode), img->buffer.pixelFormat);
    Convert2VGUCmpMode(info->header.compressMode, img->buffer.compressMode);
    img->buffer.stride = DisplayDev::GetInstance()->CalcStride(img->buffer.width,
        img->buffer.pixelFormat, info->header.compressMode);
    img->blend = BLEND_SRCOVER;
}

void LiteMGfxEngine::ClipImgByPath(ImageInfo& dstInfo, ImageInfo& srcInfo, uintptr_t path, Rect* srcImgArea,
    const Matrix4<float>* pathMatrix, const Matrix4<float>* imgMatrix, bool isAlphaFF)
{
    BlitImages();

    VGUSurface dstSurface;
    dstSurface.clipNum = 1;
    dstSurface.clipRects[0].x = 0;
    dstSurface.clipRects[0].y = 0;
    dstSurface.clipRects[0].w = dstInfo.header.width;
    dstSurface.clipRects[0].h = dstInfo.header.height;
    dstSurface.buffer.memHandle = static_cast<uint64_t>(uintptr_t(dstInfo.data));
    dstSurface.buffer.width = static_cast<uint32_t>(uintptr_t(dstInfo.header.width));
    dstSurface.buffer.height = static_cast<uint32_t>(uintptr_t(dstInfo.header.height));

    Convert2VGUFormat(static_cast<ColorMode>(dstInfo.header.colorMode), dstSurface.buffer.pixelFormat);
    Convert2VGUCmpMode(dstInfo.header.compressMode, dstSurface.buffer.compressMode);
    dstSurface.buffer.stride = DisplayDev::GetInstance()->CalcStride(dstSurface.buffer.width,
        dstSurface.buffer.pixelFormat, dstInfo.header.compressMode);

    VGUImage img;
    ConvertImgInfoToVGUImage(&srcInfo, &img);

    if (srcImgArea != nullptr) {
        img.rect = {static_cast<float>(srcImgArea->GetLeft()), static_cast<float>(srcImgArea->GetTop()),
            static_cast<float>(srcImgArea->GetWidth()), static_cast<float>(srcImgArea->GetHeight())};
    } else {
        img.rect = {0, 0, static_cast<float>(srcInfo.header.width), static_cast<float>(srcInfo.header.height)};
    }

    VGUFillAttr fillAttr;
    memset_s(&fillAttr, sizeof(fillAttr), 0, sizeof(fillAttr));
    VGUPaintStyle style = {{0}};
    style.color = Color::White().full;
    if (isAlphaFF) {
        style.type = VGU_PAINT_COLOR_ALPHA_FF;
    } else {
        style.type = VGU_PAINT_COLOR;
    }
    fillAttr.style = &style;

    VGUMatrix3 vguMat = {{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
    if (imgMatrix != nullptr) {
        HWDrawUtils::GetInstance()->Matrix4ToVguMatrix3(*imgMatrix, img.matrix);
    } else {
        memcpy_s(img.matrix.m, sizeof(img.matrix.m), vguMat.m, sizeof(img.matrix.m));
    }

    CheckHwDrawLimit(4); // 4: create 4 nodes

    if (pathMatrix != nullptr) {
        HWDrawUtils::GetInstance()->Matrix4ToVguMatrix3(*pathMatrix, vguMat);
        FRAME_TRACE_START(FrameTraceType::HW_CLIP_IMGBYPATH);
        vguFuncs_->ClipImgByPath(&dstSurface, &img, path, &vguMat, &fillAttr);
        FRAME_TRACE_END(FrameTraceType::HW_CLIP_IMGBYPATH, srcImgArea);
    } else {
        FRAME_TRACE_START(FrameTraceType::HW_CLIP_IMGBYPATH);
        vguFuncs_->ClipImgByPath(&dstSurface, &img, path, nullptr, &fillAttr);
        FRAME_TRACE_END(FrameTraceType::HW_CLIP_IMGBYPATH, srcImgArea);
    }

    AddHwDraw(4); // 4: create 4 nodes
}

static bool RecomputeRectAndMatrix(
    const BufferInfo &dst, const BufferInfo &src, Rect &srcRect, Rect &dstRect, Matrix4<float> &matrix)
{
    if (dst.rotType != 0 || src.rotType != 0) {
        if (dst.rotType == src.rotType && IsTranslateMatrix(matrix)) {  // only translate and same angle
            // get real rect with pre rotation
            srcRect = GetRotateRect(src.rotType, src.width, src.height, srcRect);
            dstRect = GetRotateRect(dst.rotType, dst.width, dst.height, dstRect);
            return true;
        }

        if (src.rotType == 0) {
            dstRect = GetRotateRect(dst.rotType, dst.width, dst.height, dstRect);
            matrix = GetRotateMatrix(dst.rotType, dst.width, dst.height) * matrix;
        } else {
            Matrix4<float> toOrigin;           // buffer的部分区域平移，需要将起始位置移动到(0,0)
            toOrigin[3][0] = -srcRect.GetX();  // 3: index
            toOrigin[3][1] = -srcRect.GetY();  // 3: index
            srcRect = GetRotateRect(src.rotType, src.width, src.height, srcRect);
            dstRect = GetRotateRect(dst.rotType, dst.width, dst.height, dstRect);
            matrix = GetRotateMatrix(dst.rotType, dst.width, dst.height) * matrix * toOrigin *
                     GetRotateMatrixReverse(src.rotType, src.width, src.height);
        }
        if (!IsTranslateMatrix(matrix)) {  // 矩阵经过处理，有可能变为平移矩阵
            return true;
        }
        TransformMap trans;
        trans.SetTransMapRect(srcRect);
        trans.SetMatrix(matrix);
        Rect boxRect = trans.GetBoxRect();
        if (!UpdateRectWithTranslateMat(boxRect, srcRect, dstRect)) {
            return false;
        }
    }
    return true;
}

void LiteMGfxEngine::Blit(BufferInfo& dst, const Point& dstPos,
    const BufferInfo& src, const Rect& subRect, const BlendOption& blendOption)
{
#if !CAPABILITY_HWDRAW_ALPHA_BITMAP
    if ((src.mode == A1) || (src.mode  == A2) || (src.mode  == A4) || (src.mode  == A8)) {
        SyncHwDraw();
        FRAME_TRACE_START(FrameTraceType::SW_DRAW_IMGBLIT);
        SoftEngine::Blit(dst, dstPos, src, subRect, blendOption);
        FRAME_TRACE_END(FrameTraceType::SW_DRAW_IMGBLIT, &subRect);
        return;
    }
#endif

    if (!CheckSrcDimension(src)) {
        return;
    }

    uint64_t currentPhyAddr = static_cast<uint64_t>(uintptr_t(dst.phyAddr));
    if (needColorize_ || (surface_.buffer.memHandle != currentPhyAddr) ||
        (Convert2VGUBlendType(blendOption.mode) != images_[0].blend)) {
        BlitImages();
    }

    VGUImage& image = images_[blitNum_];
    if (!Convert2VGUBuffer(src, image.buffer) ||
        !Convert2VGUBuffer(dst, surface_.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    Matrix4<float> matrix = blendOption.transMap.GetTransformMatrix();
    Rect srcRect = src.rect;
    Rect dstRect = subRect;

    if (DisplayDev::GetInstance()->GetRotateMethod() == RotateMethod::ROTATE_METHOD_PRE_ROTATE) {
        if (!RecomputeRectAndMatrix(dst, src, srcRect, dstRect, matrix)) {
            return;
        }
    }

    image.enCompressed = (src.compressMode != COMPRESS_MODE_NONE);
    image.globalAlpha = blendOption.opacity;
    image.blend = Convert2VGUBlendType(blendOption.mode);
    image.rect = {static_cast<float>(srcRect.GetLeft()), static_cast<float>(srcRect.GetTop()),
        static_cast<float>(srcRect.GetWidth()), static_cast<float>(srcRect.GetHeight())};
    image.dstRect = {static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
        static_cast<float>(dstRect.GetWidth()), static_cast<float>(dstRect.GetHeight())};
    image.enColorize = needColorize_;
    image.buffer.color = needColorize_ ? colorizeValue_ : src.color;

    /* convert Transmap Matrix into VGUMatrix */
    HWDrawUtils::GetInstance()->Matrix4ToVguMatrix3(matrix, image.matrix);

    if (needHSVAdjust_) {
        CheckHwDrawLimit(2); // 2: create 2 nodes
        VGUResult ret = vguFuncs_->RenderHSV(&surface_, &image, &hsvMask_, &hsvInfo_);
        if (ret != VGU_SUCCESS) {
            GRAPHIC_LOGE("RenderHSV failed!");
        } else {
            AddHwDraw(2); // 2: create 2 nodes
        }
        return;
    }

    blitNum_++;
    /* colorize need to be handled separately */
    if (needColorize_ || blitNum_ >= BLIT_COUNT_MAX) {
        BlitImages();
    }
}

void LiteMGfxEngine::Render3DCylinder(BufferInfo& dst, ImageInfo* texture,
    VGUModelSurface* model, VGUCameraAttr* cam, const Rect& subRect)
{
    VGUSurface target;
    if (!Convert2VGUBuffer(dst, target.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    BlitImages();

    VGUImage img;
    ConvertImgInfoToVGUImage(texture, &img);

    img.rect = {static_cast<float>(subRect.GetX()), static_cast<float>(subRect.GetY()),
        static_cast<float>(subRect.GetWidth()), static_cast<float>(subRect.GetHeight())};
    img.dstRect = {0, 0, static_cast<float>(dst.width), static_cast<float>(dst.height)};

    CheckHwDrawLimit(1);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_3D_CYLINDER);
    VGUResult ret = vguFuncs_->Render3DCylinder(&target, &img, model, cam);
    Rect clinderRect = {0, 0, static_cast<int16_t>(img.dstRect.w - 1), static_cast<int16_t>(img.dstRect.h - 1)};
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_3D_CYLINDER, &clinderRect);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("Render3DCylinder failed. Sync previous task.");
    } else {
        AddHwDraw(1);
    }
}

void LiteMGfxEngine::Render3DMesh(BufferInfo& dst, ImageInfo* texture,
    VGUModelSurface* model, VGUMeshAttr* mesh)
{
    VGUSurface target;
    if (!Convert2VGUBuffer(dst, target.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    BlitImages();

    VGUImage img;
    ConvertImgInfoToVGUImage(texture, &img);

    img.rect = {0, 0, static_cast<float>(texture->header.width), static_cast<float>(texture->header.height)};
    img.dstRect = {0, 0, static_cast<float>(dst.width), static_cast<float>(dst.height)};

    CheckHwDrawLimit(1);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_3D_MESH);
    VGUResult ret = vguFuncs_->Render3DMesh(&target, &img, model, mesh);
    Rect meshRect = {0, 0, static_cast<int16_t>(img.dstRect.w - 1), static_cast<int16_t>(img.dstRect.h - 1)};
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_3D_MESH, &meshRect);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("Render3DMesh failed. Sync previous task.");
    } else {
        AddHwDraw(1);
    }
}

void LiteMGfxEngine::Render3DSphere(BufferInfo& dst, ImageInfo* texture,
    VGUModelSurface* model, VGUSphereAttr* sphere, ImageInfo* mask)
{
    VGUSurface target;
    if (!Convert2VGUBuffer(dst, target.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    BlitImages();

    VGUImage img;
    ConvertImgInfoToVGUImage(texture, &img);
    img.rect = {0, 0, static_cast<float>(texture->header.width), static_cast<float>(texture->header.height)};
    img.dstRect = {0, 0, static_cast<float>(dst.width), static_cast<float>(dst.height)};

    VGUImage maskImg;
    ConvertImgInfoToVGUImage(mask, &maskImg);
    maskImg.rect = {0, 0, static_cast<float>(mask->header.width), static_cast<float>(mask->header.height)};
    maskImg.dstRect = {0, 0, static_cast<float>(dst.width), static_cast<float>(dst.height)};

    CheckHwDrawLimit(2); // 2: create 2 nodes

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_3D_SPHERE);
    VGUResult ret = vguFuncs_->Render3DSphere(&target, &img, model, sphere, &maskImg);
    Rect sphereRect = {0, 0, static_cast<int16_t>(img.dstRect.w - 1), static_cast<int16_t>(img.dstRect.h - 1)};
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_3D_SPHERE, &sphereRect);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("Render3DSphere failed. Sync previous task.");
    } else {
        AddHwDraw(2); // 2: create 2 nodes
    }
}

void LiteMGfxEngine::Lighting(BufferInfo& dst, LightStyle style)
{
    VGUSurface dstSurface;
    if (!Convert2VGUBuffer(dst, dstSurface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    BlitImages();

    Rect mask = dst.rect;
    VGULightStyle lightStyle;
    lightStyle.lightMode = static_cast<VGULightType>(style.speStyle.lightMode);
    lightStyle.cx = style.speStyle.cx;
    lightStyle.cy = style.speStyle.cy;
    lightStyle.radius = style.speStyle.radius;
    lightStyle.speAmp = style.speStyle.speAmp;
    lightStyle.startAngle = Deg2Rad(style.speStyle.startAngle - DEGRESS_720 *
                                    static_cast<uint32_t>(style.speStyle.startAngle / DEGRESS_720));
    lightStyle.endAngle = Deg2Rad(style.speStyle.endAngle - DEGRESS_720 *
                                  static_cast<uint32_t>(style.speStyle.startAngle / DEGRESS_720));
    lightStyle.isSymmetrical = style.speStyle.isSymmetrical;
    lightStyle.ambAmp = style.ambAmp;

    dstSurface.clipRects[0] = {static_cast<float>(mask.GetLeft()), static_cast<float>(mask.GetTop()),
                               static_cast<float>(mask.GetWidth()), static_cast<float>(mask.GetHeight())};
    dstSurface.clipNum = 1;

    CheckHwDrawLimit(1);

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_LIGHT);
    VGUResult ret = vguFuncs_->RenderLighting(&dstSurface, lightStyle);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_LIGHT, &mask);
    if (ret != VGU_SUCCESS) {
        SyncHwDraw(true);
        GRAPHIC_LOGE("RenderLighting failed. Sync previous task.");
    } else {
        AddHwDraw(1);
    }
}

void LiteMGfxEngine::Blur(BufferInfo& dst, float radius)
{
    Blur(dst, radius, 0, nullptr);
}

void LiteMGfxEngine::Blur(BufferInfo& dst, float radius, uint32_t quality, BufferInfo* maskinfo)
{
    VGUSurface dstSurface;
    if (!Convert2VGUBuffer(dst, dstSurface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }
    VGUSurface* maskSurface = nullptr;
    VGUSurface maskSurfaceInstance;
    if (maskinfo != nullptr) {
        maskSurface = &maskSurfaceInstance;
        if (!Convert2VGUBuffer(*maskinfo, maskSurface->buffer)) {
            GRAPHIC_LOGE("Convert2VGUBuffer failed.");
            return;
        }
    }

    BlitImages();

    Rect mask = dst.rect;
    dstSurface.clipRects[0] = {static_cast<float>(mask.GetLeft()), static_cast<float>(mask.GetTop()),
                               static_cast<float>(mask.GetWidth()), static_cast<float>(mask.GetHeight())};
    dstSurface.clipNum = 1;

    CheckHwDrawLimit(2); // 2: create 2 nodes

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_BLUR);
    VGUResult ret = vguFuncs_->RenderBlurExt(&dstSurface, radius, quality, maskSurface);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_BLUR, &mask);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("RenderBlur failed!");
        return;
    }

    AddHwDraw(2); // 2: create 2 nodes
}

void LiteMGfxEngine::SetColorize(uint32_t color)
{
    needColorize_ = true;
    colorizeValue_ = color;
}

void LiteMGfxEngine::ClearColorize(void)
{
    needColorize_ = false;
}

void LiteMGfxEngine::SetHSVInfo(int16_t hAdjust, int8_t sAdjust, int8_t vAdjust,
    const ImageInfo& mask, uint8_t threshold)
{
    if (mask.header.colorMode != A8) {
        GRAPHIC_LOGE("Hsv mask mode need to be A8.");
        return;
    }

    memset_s(&hsvMask_, sizeof(VGUImage), 0, sizeof(VGUImage));
    memset_s(&hsvInfo_, sizeof(VGUHSVInfo), 0, sizeof(VGUHSVInfo));
    hsvMask_.buffer.memHandle = static_cast<uint64_t>(uintptr_t(mask.data));
    hsvMask_.buffer.width = mask.header.width;
    hsvMask_.buffer.height = mask.header.height;
    hsvMask_.enCompressed = (mask.header.compressMode != COMPRESS_MODE_NONE);
    hsvMask_.rect = {0, 0, static_cast<float>(mask.header.width), static_cast<float>(mask.header.height)};
    hsvMask_.blend = Convert2VGUBlendType(BLEND_SRC_OVER);

    Convert2VGUFormat(static_cast<ColorMode>(mask.header.colorMode), hsvMask_.buffer.pixelFormat);
    Convert2VGUCmpMode(mask.header.compressMode, hsvMask_.buffer.compressMode);
    hsvMask_.buffer.stride = DisplayDev::GetInstance()->CalcStride(hsvMask_.buffer.width,
        hsvMask_.buffer.pixelFormat, mask.header.compressMode);

    hsvInfo_.hAdj = hAdjust;
    hsvInfo_.sAdj = sAdjust;
    hsvInfo_.vAdj = vAdjust;
    hsvInfo_.threshold = threshold;

    needHSVAdjust_ = true;
    return;
}

void LiteMGfxEngine::ClearHSVInfo(void)
{
    needHSVAdjust_ = false;
    memset_s(&hsvMask_, sizeof(hsvMask_), 0, sizeof(hsvMask_));
    memset_s(&hsvInfo_, sizeof(VGUHSVInfo), 0, sizeof(VGUHSVInfo));
}

void LiteMGfxEngine::DrawRain(BufferInfo& dstInfo, const ImageInfo& rainInfo)
{
    VGUSurface dstSurface;
    if (!Convert2VGUBuffer(dstInfo, dstSurface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    BlitImages();

    VGUImage rain;
    memset_s(&rain, sizeof(rain), 0, sizeof(rain));
    rain.rect = {0, 0, dstInfo.rect.GetWidth() - 1, dstInfo.rect.GetHeight() - 1};
    rain.dstRect = rain.rect;
    rain.buffer.isSolid = false;
    rain.buffer.memHandle = static_cast<uint64_t>(uintptr_t(rainInfo.data));
    rain.buffer.width = rainInfo.header.width;
    rain.buffer.height = rainInfo.header.height;
    rain.globalAlpha = 0xff;
    rain.enCompressed = (rainInfo.header.compressMode != COMPRESS_MODE_NONE);

    Convert2VGUFormat(static_cast<ColorMode>(rainInfo.header.colorMode), rain.buffer.pixelFormat);
    Convert2VGUCmpMode(rainInfo.header.compressMode, rain.buffer.compressMode);
    rain.buffer.stride = DisplayDev::GetInstance()->CalcStride(rain.buffer.width,
        rain.buffer.pixelFormat, rainInfo.header.compressMode);

    CheckHwDrawLimit(2); // 2: create 2 nodes

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_RAIN);
    VGUResult ret = vguFuncs_->RenderRain(&dstSurface, &rain);
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_RAIN, &dstInfo.rect);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("RenderRain failed!");
    } else {
        AddHwDraw(2); // 2: create 2 nodes
    }
}

void LiteMGfxEngine::RenderAYUV(BufferInfo& dst, const BufferInfo& alpha, const BufferInfo& yuv,
    const Rect& dstRect, const BlendOption& blendOption)
{
    BlitImages();

    VGUSurface dstSurface;
    VGUImage alphaImg;
    VGUImage yuvImg;
    memset_s(&dstSurface, sizeof(VGUSurface), 0, sizeof(VGUSurface));
    memset_s(&alphaImg, sizeof(VGUImage), 0, sizeof(VGUImage));
    memset_s(&yuvImg, sizeof(VGUImage), 0, sizeof(VGUImage));

    if (!Convert2VGUBuffer(alpha, alphaImg.buffer) || !Convert2VGUBuffer(yuv, yuvImg.buffer) ||
        !Convert2VGUBuffer(dst, dstSurface.buffer)) {
        GRAPHIC_LOGE("Convert2VGUBuffer failed.");
        return;
    }

    dstSurface.clipNum = 1;
    dstSurface.clipRects[0] = {static_cast<float>(dstRect.GetLeft()), static_cast<float>(dstRect.GetTop()),
        static_cast<float>(dstRect.GetWidth()), static_cast<float>(dstRect.GetHeight())};

    alphaImg.enCompressed = (alpha.compressMode != COMPRESS_MODE_NONE);
    alphaImg.globalAlpha = blendOption.opacity;
    alphaImg.blend = Convert2VGUBlendType(blendOption.mode);
    alphaImg.rect = {static_cast<float>(alpha.rect.GetLeft()), static_cast<float>(alpha.rect.GetTop()),
        static_cast<float>(alpha.rect.GetWidth()), static_cast<float>(alpha.rect.GetHeight())};

    yuvImg.enCompressed = (yuv.compressMode != COMPRESS_MODE_NONE);
    yuvImg.globalAlpha = blendOption.opacity;
    yuvImg.blend = Convert2VGUBlendType(blendOption.mode);
    yuvImg.rect = {static_cast<float>(yuv.rect.GetLeft()), static_cast<float>(yuv.rect.GetTop()),
        static_cast<float>(yuv.rect.GetWidth()), static_cast<float>(yuv.rect.GetHeight())};

    CheckHwDrawLimit(4); // 4: create 4 nodes
    VGUResult ret = vguFuncs_->RenderBlitAYUV(&dstSurface, &alphaImg, &yuvImg);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("RenderBlitAYUV failed!");
    } else {
        AddHwDraw(4); // 4: create 4 nodes
    }
}

void LiteMGfxEngine::SyncHwDraw(bool isNeedWait)
{
    GraphicService::GetInstance()->switchMutex_.Lock();
    if (GraphicService::GetInstance()->IsNativeRunning() && !GraphicService::IsNativeUITask()) {
        GraphicService::GetInstance()->switchMutex_.Unlock();
        return;
    }

    if (!GraphicService::GetInstance()->IsNativeRunning() && GraphicService::IsNativeUITask()) {
        GraphicService::GetInstance()->switchMutex_.Unlock();
        return;
    }

    BlitImages(true);

    RenderSubmitInner(isNeedWait);
    GraphicService::GetInstance()->switchMutex_.Unlock();
}

void LiteMGfxEngine::RenderSubmitInner(bool isNeedWait)
{
    AsyncFlush();

    if (isHwDrawPending_) {
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_SUBMIT);
        vguFuncs_->RenderSubmit(isNeedWait, nullptr);
        const Rect *needWait = isNeedWait ? &lastAsyncInfo_.rect : nullptr;
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_SUBMIT, needWait);
        isHwDrawPending_ = false;
        hwDrawPendingCnt_ = 0;
        hwStrokePendingCnt_ = 0;

        while (pathRender_.Size() != 0) {
            pathSubmit_.PushBack(pathRender_.Back());
            pathRender_.PopBack();
        }
    }

    if (isNeedWait) {
#if CAPABILITY_HWDRAW_PATH
        ReleasePath(pathSubmit_);
#endif
    }
}

void LiteMGfxEngine::StartDraw()
{
    vguFuncs_->StartRender();
    if (DisplayDev::GetInstance()->IsDoubleBuffer() && enableAsyncSubmit_ && !lastAsyncInfo_.hasPendingFrame) {
        return; // don't exit ulps for first RTC/TE period in async mode to avoid exit too early
    }
    DisplayDev::GetInstance()->TryExitUlps(false);
}

void LiteMGfxEngine::BlitImages(bool calledBySync)
{
    if (blitNum_ < 1) {
        return;
    }

    surface_.clipNum = 0;
    surface_.clipRects[0] = {0, 0, 0, 0};
    uint8_t blitNum = blitNum_;
    blitNum_ = 0;

    if (hwDrawPendingCnt_ + blitNum >= DRAW_MAX_CNT) {
        if (calledBySync) {
            RenderSubmitInner(); // called by SyncHwDraw, switchMutex_ is already acquired.
        } else {
            SyncHwDraw(); // called by other render method, need to call SyncHwDraw to aquire switchMutex_.
        }
    }

    FRAME_TRACE_START(FrameTraceType::HW_DRAW_IMGBLIT);
    VGUResult ret = vguFuncs_->RenderBlit(&surface_, images_, blitNum);
    Rect num = {blitNum, 0, 0, 0};
    FRAME_TRACE_END(FrameTraceType::HW_DRAW_IMGBLIT, &num);
    if (ret != VGU_SUCCESS) {
        GRAPHIC_LOGE("RenderBlit failed!");
    } else {
        AddHwDraw(blitNum);
    }
}

void LiteMGfxEngine::DrawTransform(BufferInfo& dst, const Rect& mask, const Point& position, ColorType color,
    OpacityType opacity, const TransformMap& transMap, const TransformDataInfo& dataInfo)
{
    if (opacity == OPA_TRANSPARENT) {
        return;
    }
    Rect boxRect = transMap.GetBoxRect();
    Rect trans(boxRect);
    trans.SetX(trans.GetX() + position.x);
    trans.SetY(trans.GetY() + position.y);

    Rect dstRect(mask);
    if (!dstRect.Intersect(trans, dstRect)) {
        return;
    }
#if !CAPABILITY_HWDRAW_ALPHA_BITMAP
    if ((dataInfo.header.colorMode == A1) || (dataInfo.header.colorMode == A2) || (dataInfo.header.colorMode == A4) ||
        (dataInfo.header.colorMode == A8)) {
        SyncHwDraw();
        FRAME_TRACE_START(FrameTraceType::SW_DRAW_TRANSFORM);
        SoftEngine::DrawTransform(dst, dstRect, position, color, opacity, transMap, dataInfo);
        FRAME_TRACE_END(FrameTraceType::SW_DRAW_TRANSFORM, &dstRect);
        return;
    }
#endif
#if !CAPABILITY_HW_ROTATE_AND_SCALE
    Vector3<float> scaleCoef = transMap.GetScaleCoef();
    if ((transMap.GetRotateAngle() != 0) && ((scaleCoef.x_ != 1) || (scaleCoef.y_ != 1))) {
        if (dataInfo.header.compressMode != COMPRESS_MODE_NONE)  {
            GRAPHIC_LOGE("DrawTransform failed. Software can only process uncompressed data.");
            return;
        }
        SyncHwDraw();
        FRAME_TRACE_START(FrameTraceType::SW_DRAW_TRANSFORM);
        SoftEngine::DrawTransform(dst, dstRect, position, color, opacity, transMap, dataInfo);
        FRAME_TRACE_END(FrameTraceType::SW_DRAW_TRANSFORM, &dstRect);
        return;
    }
#endif

    BufferInfo src = {{0, 0, dataInfo.header.width - 1, dataInfo.header.height - 1}};
    src.color = color.full;
    src.width = dataInfo.header.width;
    src.height = dataInfo.header.height;
    src.compressMode = dataInfo.header.compressMode;
    src.mode = static_cast<ColorMode>(dataInfo.header.colorMode);
    src.virAddr = reinterpret_cast<void*>(const_cast<uint8_t*>(dataInfo.data));
    src.phyAddr = reinterpret_cast<void*>(const_cast<uint8_t*>(dataInfo.data));
    src.rotType = HWDrawUtils::GetInstance()->GetBufferInfoRotType(dataInfo.header.reserved);

    PixelFormat pixelFmt;
    Convert2VGUFormat(src.mode, pixelFmt);
    src.stride = DisplayDev::GetInstance()->CalcStride(src.width, pixelFmt, dataInfo.header.compressMode);

    BlendOption opt;
    opt.mode = BLEND_SRC_OVER;
    opt.opacity = opacity;

    Matrix4<float> matrix = transMap.GetTransformMatrix();
    // adjust src rect using dstRect for only translate matrix
    if (FloatEqual(matrix[0][0], 1) && FloatEqual(matrix[0][1], 0) && FloatEqual(matrix[0][3], 0) && // 3: index
        FloatEqual(matrix[1][0], 0) && FloatEqual(matrix[1][1], 1) && FloatEqual(matrix[1][3], 0) && // 3: index
        FloatEqual(matrix[3][3], 1)) { // 3: index
        int16_t imgX = dstRect.GetLeft() - trans.GetLeft();
        int16_t imgY = dstRect.GetTop() - trans.GetTop();
        src.rect = {imgX, imgY, imgX + dstRect.GetWidth() - 1, imgY + dstRect.GetHeight() - 1};
    }
    opt.transMap.SetMatrix(matrix); // only matrix of transMap is useful for function Blit
    Blit(dst, {0, 0}, src, dstRect, opt);
}

void LiteMGfxEngine::DrawTransform(BufferInfo& dst, const Rect& mask, const Point& position, ColorType color,
    OpacityType opacity, const TransformMap& transMap, BufferInfo src)
{
    if (opacity == OPA_TRANSPARENT) {
        return;
    }
    Rect boxRect = transMap.GetBoxRect();
    Rect trans(boxRect);
    trans.SetX(trans.GetX() + position.x);
    trans.SetY(trans.GetY() + position.y);

    Rect dstRect(mask);
    if (!dstRect.Intersect(trans, dstRect)) {
        return;
    }

    BlendOption opt;
    opt.mode = BLEND_SRC_OVER;
    opt.opacity = opacity;

    Matrix4<float> matrix = transMap.GetTransformMatrix();
    // adjust src rect using dstRect for only translate matrix
    if (FloatEqual(matrix[0][0], 1) && FloatEqual(matrix[0][1], 0) && FloatEqual(matrix[0][3], 0) && // 3: index
        FloatEqual(matrix[1][0], 0) && FloatEqual(matrix[1][1], 1) && FloatEqual(matrix[1][3], 0) && // 3: index
        FloatEqual(matrix[3][3], 1)) { // 3: index
        int16_t imgX = dstRect.GetLeft() - trans.GetLeft();
        int16_t imgY = dstRect.GetTop() - trans.GetTop();
        src.rect = {imgX, imgY, imgX + dstRect.GetWidth() - 1, imgY + dstRect.GetHeight() - 1};
    }
    opt.transMap.SetMatrix(matrix); // only matrix of transMap is useful for function Blit
    Blit(dst, {0, 0}, src, dstRect, opt);
}

void LiteMGfxEngine::DrawCubicBezier(BufferInfo& dst, const Point& start,
    const Point& control1, const Point& control2, const Point& end,
    const Rect& mask, int16_t width, ColorType color, OpacityType opacity)
{
    if ((width == 0) || (opacity == OPA_TRANSPARENT)) {
        return;
    }

#if !CAPABILITY_HWDRAW_CURVE
    SyncHwDraw();
    FRAME_TRACE_START(FrameTraceType::SW_DRAW_CUBICBEZIER);
    SoftEngine::DrawCubicBezier(dst, start, control1, control2, end, mask, width, color, opacity);
    FRAME_TRACE_END(FrameTraceType::SW_DRAW_CUBICBEZIER, nullptr);
    return;
#endif

    BlitImages();

    Path path = {0};
    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(&path, 2, 8)) { // 2, 8: path cmd and data num
        GRAPHIC_LOGE("Alloc buffer for path failed.");
        return;
    }

    path.data[0] = start.x;
    path.data[1] = start.y;
    path.data[2] = control1.x; // 2: index
    path.data[3] = control1.y; // 3: index
    path.data[4] = control2.x; // 4: index
    path.data[5] = control2.y; // 5: index
    path.data[6] = end.x; // 6: index
    path.data[7] = end.y; // 7: index

    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_CUBIC_TO;

    uintptr_t vguPath = InitPath(true);
    SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT};
    VGUPaintStyle strokeStyle = {0};
    strokeStyle.type = VGU_PAINT_COLOR;
    strokeStyle.color = GetColorWithAlpha(Color::ColorTo32(color), opacity);
    strokeAttr.width = width;
    strokeAttr.style = &strokeStyle;

    SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_CUBICBEZIER, FrameTracePathAttr::STROKE);
    RenderPathInternal(dst, vguPath, mask, &strokeAttr, nullptr, nullptr);

    pathRender_.PushBack(path);
    DeinitPath(vguPath);
}

bool LiteMGfxEngine::InitDriver()
{
    if (VGUInitialize(&vguFuncs_) == VGU_SUCCESS) {
        if (vguFuncs_ == nullptr) {
            GRAPHIC_LOGE("vguFuncs_ is null!");
            return false;
        }
        if ((vguFuncs_->InitVgu == nullptr) || (vguFuncs_->RenderClearRect == nullptr)) {
            GRAPHIC_LOGE("InitVgu or RenderClearRect is null!");
            return false;
        }
#if CAPABILITY_HWDRAW_PATH || CAPABILITY_HWDRAW_CURVE || CAPABILITY_HWDRAW_LINE || \
    CAPABILITY_HWDRAW_ARC || CAPABILITY_HWDRAW_ELLIPSE
        if ((vguFuncs_->VGUPathInit == nullptr) || (vguFuncs_->VGUPathClear == nullptr) ||
            (vguFuncs_->RenderPath == nullptr)) {
            GRAPHIC_LOGE("VGUPathInit, VGUPathAppendData, VGUPathClear or RenderPath is nullptr!");
            return false;
        }
#endif
#if CAPABILITY_HWDRAW_PATH || CAPABILITY_HWDRAW_CURVE
        if ((vguFuncs_->VGUPathAppendData == nullptr) || (vguFuncs_->VGUPathSetData == nullptr)) {
            GRAPHIC_LOGE("VGUPathInit, VGUPathAppendData, VGUPathClear or RenderPath is nullptr!");
            return false;
        }
#endif
        if (vguFuncs_->InitVgu() != VGU_SUCCESS) {
            GRAPHIC_LOGE("InitVgu failed!");
            return false;
        }
        return true;
    }
    return false;
}

void LiteMGfxEngine::CloseDriver()
{
    if (vguFuncs_ == nullptr) {
        GRAPHIC_LOGI("vguFuncs_ is null!");
        return;
    }

    if (vguFuncs_->DeinitVgu != nullptr) {
        if (vguFuncs_->DeinitVgu() != VGU_SUCCESS) {
            GRAPHIC_LOGE("DeinitVgu failed!");
        }
    }
    VGUUninitialize(vguFuncs_);
    vguFuncs_ = nullptr;
}

VGUSurface LiteMGfxEngine::CreateSubSurface(const VGUSurface& dstSurface, VGUMatrix3* matrix) const
{
    auto displayDev = DisplayDev::GetInstance();
    if (FloatMore(dstSurface.clipRects[0].w, VAU_CLIPRECT_WIDTH_MAX) ||
        FloatMore(dstSurface.clipRects[0].h, VAU_CLIPRECT_HEIGHT_MAX) ||
        displayDev->GetRotateMethod() != RotateMethod::ROTATE_METHOD_PRE_ROTATE) {
        return dstSurface;
    }

    if (FloatMore((dstSurface.clipRects[0].x + dstSurface.clipRects[0].w), VAU_CLIPRECT_WIDTH_MAX) ||
        FloatMore((dstSurface.clipRects[0].y + dstSurface.clipRects[0].h), VAU_CLIPRECT_HEIGHT_MAX)) {
        VGUSurface subSurface = dstSurface;
        uint16_t offsetX = displayDev->CalcAlignCoordinate(static_cast<uint16_t>(dstSurface.clipRects[0].x),
            dstSurface.buffer.compressMode);
        uint16_t offsetStride =
            displayDev->CalcStride(offsetX, dstSurface.buffer.pixelFormat, dstSurface.buffer.compressMode);
        uint64_t offsetAddr =
            static_cast<uint64_t>((dstSurface.clipRects[0].y * dstSurface.buffer.stride) + offsetStride);
        subSurface.buffer.memHandle = dstSurface.buffer.memHandle + offsetAddr;

        subSurface.buffer.width = dstSurface.buffer.width;
        subSurface.buffer.height = static_cast<uint32_t>(dstSurface.clipRects[0].h);

        subSurface.clipRects[0].x = dstSurface.clipRects[0].x - offsetX;
        subSurface.clipRects[0].y = 0;
        if (FloatMore((subSurface.clipRects[0].x + dstSurface.clipRects[0].w), VAU_CLIPRECT_WIDTH_MAX)) {
            subSurface.clipRects[0].w = VAU_CLIPRECT_WIDTH_MAX - subSurface.clipRects[0].x;
            GRAPHIC_LOGW("Out of the area and truncated. adjust ths x or reduce the width");
        } else {
            subSurface.clipRects[0].w = dstSurface.clipRects[0].w;
        }
        subSurface.clipRects[0].h = dstSurface.clipRects[0].h;

        if (matrix != nullptr) {
            VGUMatrix3Rotate(matrix, 0, -offsetX, static_cast<int16_t>(-dstSurface.clipRects[0].y));
        }
        return subSurface;
    }
    return dstSurface;
}
#endif

bool LiteMGfxEngine::Convert2VGUFormat(const ColorMode& colorMode, VGUPixelFormat& dstColorFmt) const
{
    switch (colorMode) {
        case RGB565:
            dstColorFmt = PIXEL_FMT_RGB_565;
            break;
        case RGB888:
            dstColorFmt = PIXEL_FMT_RGB_888;
            break;
        case ARGB8888:
            dstColorFmt = PIXEL_FMT_RGBA_8888;
            break;
        case A1:
            dstColorFmt = PIXEL_FMT_A1;
            break;
        case A2:
            dstColorFmt = PIXEL_FMT_A2;
            break;
        case A4:
            dstColorFmt = PIXEL_FMT_A4;
            break;
        case A8:
            dstColorFmt = PIXEL_FMT_A8;
            break;
        case YCBCR420:
            dstColorFmt = PIXEL_FMT_YCBCR_420_SP;
            break;
        default:
            GRAPHIC_LOGE("unsupported color format: %d", colorMode);
            return false;
    }
    return true;
}

uint8_t* LiteMGfxEngine::AllocBuffer(uint32_t size, uint32_t usage)
{
    uint64_t grallocUsage = HBM_USE_ASSIGN_SIZE;
    if (usage == BUFFER_MAP_SURFACE) {
        grallocUsage |= HBM_USE_MEM_BUFMAP;
    }
    return GrallocEngines::GetInstance()->AllocBufferBySize(size, grallocUsage);
}

void LiteMGfxEngine::FreeBuffer(uint8_t* buffer, uint32_t usage)
{
    GrallocEngines::GetInstance()->FreeBuffer(buffer);
}

BufferInfo* LiteMGfxEngine::GetFBBufferInfo()
{
    if (bufferInfo_ != nullptr) {
        return bufferInfo_;
    }

    bufferInfo_ = static_cast<BufferInfo*>(UIMalloc(sizeof(BufferInfo)));
    if (bufferInfo_ == nullptr) {
        return nullptr;
    }

    bool ret = DisplayDev::GetInstance()->GetCurrentBufferInfo(LAYER_0, *bufferInfo_);
    if (!ret) {
        UIFree(bufferInfo_);
        bufferInfo_ = nullptr;
        return nullptr;
    }
    return bufferInfo_;
}

BufferInfo* LiteMGfxEngine::GetNextFBBufferInfo()
{
    if (nextBufferInfo_ != nullptr) {
        return nextBufferInfo_;
    }

    nextBufferInfo_ = static_cast<BufferInfo*>(UIMalloc(sizeof(BufferInfo)));
    if (nextBufferInfo_ == nullptr) {
        return nullptr;
    }

    bool ret = DisplayDev::GetInstance()->GetCurrentBufferInfo(LAYER_0, *nextBufferInfo_);
    if (!ret) {
        UIFree(nextBufferInfo_);
        nextBufferInfo_ = nullptr;
        return nullptr;
    }
    return nextBufferInfo_;
}

void LiteMGfxEngine::Flush(const Rect &rect)
{
    // flush on async mode
    auto* displayDev = DisplayDev::GetInstance();
    if (enableAsyncSubmit_ && displayDev->IsDoubleBuffer()) {
#if ENABLE_VGU_ENGINE
        BlitImages();
        // flush pending frame
        AsyncFlush();

        // Submit hardware draw task asynchronously, and set pending frame.
        if (rect.IsValid()) {
            if (displayDev->GetRotateMethod() == RotateMethod::ROTATE_METHOD_USE_ROTATE_BUF) {
                displayDev->RotateBufferIfNecessary();
            }
            SyncHwDraw(false);
            lastAsyncInfo_.rect = rect;
            lastAsyncInfo_.hasPendingFrame = true;
            lastAsyncInfo_.needSwapToPendingFrame = false;
        }
#endif
        return;
    }

    // flush on sync mode
    if (rect.IsValid()) {
        if (displayDev->GetRotateMethod() == RotateMethod::ROTATE_METHOD_USE_ROTATE_BUF) {
            displayDev->RotateBufferIfNecessary();
        }
#if ENABLE_VGU_ENGINE
        SyncHwDraw();
#endif
        displayDev->SetLayerDirtyRegion(LAYER_0, rect);
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_FLUSH);
        displayDev->LcdFlush(LAYER_0);
#if ENABLE_VGU_ENGINE
        if (hardwareView_ != nullptr) {
            hardwareView_->Flush();
        }
#endif
        displayDev->LcdFlush(LAYER_MAX);
#if ENABLE_VGU_ENGINE
        if (hardwareView_ != nullptr) {
            hardwareView_->PostFlush();
        }
#endif
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_FLUSH, &rect);
        lastAsyncInfo_.hasPendingFrame = false;
        lastAsyncInfo_.needSwapToPendingFrame = false;
    }
}

static void CopyBufferInfo(const BufferInfo &src, BufferInfo &dst)
{
    dst.rect = src.rect;
    dst.stride = src.stride;
    dst.phyAddr = src.phyAddr;
    dst.virAddr = src.virAddr;
    dst.width = src.width;
    dst.height = src.height;
    dst.mode = src.mode;
    dst.color = src.color;
#ifdef VERSION_IOT
    dst.compressMode = src.compressMode;
    dst.rotType = src.rotType;
#endif
}

bool LiteMGfxEngine::SwapBuffer(const Rect* rect)
{
    if (bufferInfo_ == nullptr) {
        GRAPHIC_LOGE("bufferInfo_ is not inited.");
        return false;
    }

    DisplayDev::GetInstance()->SwapBuffer(LAYER_0);
    if (lastAsyncInfo_.hasPendingFrame) {
        lastAsyncInfo_.needSwapToPendingFrame = true;
    }
    BufferInfo tempBuffer;
    CopyBufferInfo(*bufferInfo_, tempBuffer);
    if (!DisplayDev::GetInstance()->GetCurrentBufferInfo(LAYER_0, *bufferInfo_)) {
        GRAPHIC_LOGD("Get current buffer failed.");
        return false;
    }

    if (rect != nullptr) {
        tempBuffer.rect = *rect;
        Point dstPos = {rect->GetX(), rect->GetY()};
        BlendOption blendOption;
        blendOption.opacity = OPA_OPAQUE;
        blendOption.mode = BLEND_SRC_OVER;
        blendOption.transMap.Translate(Vector2<int16_t>(rect->GetX(), rect->GetY()));
        Blit(*bufferInfo_, dstPos, tempBuffer, *rect, blendOption);
    }
    RootView::GetInstance()->UpdateBufferInfo(bufferInfo_);
    return true;
}

void LiteMGfxEngine::AsyncFlush()
{
#if ENABLE_VGU_ENGINE
    if (DisplayDev::GetInstance()->IsDoubleBuffer() && lastAsyncInfo_.hasPendingFrame) {
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_WAIT);
        vguFuncs_->RenderWaitDone(0);
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_WAIT, nullptr);
#if CAPABILITY_HWDRAW_PATH
        ReleasePath(pathSubmit_);
#endif
        if (lastAsyncInfo_.needSwapToPendingFrame) {
            DisplayDev::GetInstance()->SwapBuffer(LAYER_0);
        }
        DisplayDev::GetInstance()->SetLayerDirtyRegion(LAYER_0, lastAsyncInfo_.rect);
        FRAME_TRACE_START(FrameTraceType::HW_DRAW_FLUSH);
        DisplayDev::GetInstance()->LcdFlush(LAYER_0);
        DisplayDev::GetInstance()->LcdFlush(LAYER_MAX);
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_FLUSH, &lastAsyncInfo_.rect);
        if (lastAsyncInfo_.needSwapToPendingFrame) {
            DisplayDev::GetInstance()->SwapBuffer(LAYER_0);
        }
        lastAsyncInfo_.hasPendingFrame = false;
        lastAsyncInfo_.needSwapToPendingFrame = false;
    }
#endif
}

void LiteMGfxEngine::EnableAsyncMode(bool enable)
{
    GRAPHIC_LOGD("set value: %d", enable);
    AsyncFlush();
    enableAsyncSubmit_ = enable;
}

bool LiteMGfxEngine::IsAsyncModeEnabled()
{
    return enableAsyncSubmit_;
}

void LiteMGfxEngine::DrawRectWithoutRound(BufferInfo& dst, const Rect& rect, const Style& style, const Rect& dirtyRect,
    OpacityType opacity, VGUMatrix3* matrix)
{
#if CAPABILITY_HWDRAW_PATH
    uint16_t bgOpa = DrawUtils::GetMixOpacity(opacity, style.bgOpa_);
    uint16_t borderOpa = DrawUtils::GetMixOpacity(opacity, style.borderOpa_);

    BlitImages();

    Path path = {0};
    if (!HWDrawUtils::GetInstance()->AllocBufferForPath(&path, 5, 5)) { // 5: path cmd and data num
        GRAPHIC_LOGE("Alloc buffer for path failed.");
        return;
    }

    path.data[0] = rect.GetLeft();
    path.data[1] = rect.GetTop();
    path.data[2] = rect.GetRight() + 1; // 2: index
    path.data[3] = rect.GetBottom() + 1; // 3: index
    path.data[4] = rect.GetLeft(); // 4: index

    path.cmds[0] = CMD_MOVE_TO;
    path.cmds[1] = CMD_HLINE_TO;
    path.cmds[2] = CMD_VLINE_TO; // 2: index
    path.cmds[3] = CMD_HLINE_TO; // 3: index
    path.cmds[4] = CMD_CLOSE; // 4: index

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT, VGU_LINE_JOIN_MITER, 4}; // 4 : exp value
    VGUPaintStyle strokeStyle = {0};
    strokeStyle.type = VGU_PAINT_COLOR;
    strokeStyle.color = GetColorWithAlpha(Color::ColorTo32(style.borderColor_), borderOpa);
    strokeAttr.width = style.borderWidth_;
    strokeAttr.style = &strokeStyle;

    VGUFillAttr fillAttr = {VGU_RULE_WINDING};
    VGUPaintStyle fillStyle = {0};
    fillStyle.type = VGU_PAINT_COLOR;
    fillStyle.color =  GetColorWithAlpha(Color::ColorTo32(style.bgColor_), bgOpa);
    fillAttr.style = &fillStyle;

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);
    if (borderOpa == OPA_TRANSPARENT || style.borderWidth_ == 0) {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_RECT, FrameTracePathAttr::FILL);
        RenderPath(dst, vguPath, dirtyRect, nullptr, &fillAttr, matrix);
    } else if (bgOpa == OPA_TRANSPARENT) {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_RECT, FrameTracePathAttr::STROKE);
        RenderPath(dst, vguPath, dirtyRect, &strokeAttr, nullptr, matrix);
    } else {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_RECT, FrameTracePathAttr::BOTH);
        RenderPath(dst, vguPath, dirtyRect, &strokeAttr, &fillAttr, matrix);
    }

    pathRender_.PushBack(path);
    DeinitPath(vguPath);
#endif
}

void LiteMGfxEngine::DrawRectWithRound(BufferInfo& dst, const Rect& rect, const Style& style, const Rect& dirtyRect,
    OpacityType opacity, VGUMatrix3* matrix)
{
#if CAPABILITY_HWDRAW_PATH
    uint16_t bgOpa = DrawUtils::GetMixOpacity(opacity, style.bgOpa_);
    uint16_t borderOpa = DrawUtils::GetMixOpacity(opacity, style.borderOpa_);

    BlitImages();

    Path path = {0};
    if (!HWDrawUtils::GetInstance()->ObtainRectPathWithRadius(rect, path,
        style.GetStyle(STYLE_TOP_BORDER_RADIUS), style.GetStyle(STYLE_BOTTOM_BORDER_RADIUS))) {
        GRAPHIC_LOGE("Obtain RoundRect Path failed.");
        return;
    }

    VGUFillAttr fillAttr = {VGU_RULE_WINDING};
    VGUPaintStyle fillStyle = {{0}};
    fillStyle.type = VGU_PAINT_COLOR;
    fillStyle.color =  GetColorWithAlpha(Color::ColorTo32(style.bgColor_), bgOpa);
    fillAttr.style = &fillStyle;

    VGUStrokeAttr strokeAttr = {VGU_LINECAP_BUTT};
    VGUPaintStyle strokeStyle = {0};
    strokeStyle.type = VGU_PAINT_COLOR;
    strokeStyle.color = GetColorWithAlpha(Color::ColorTo32(style.borderColor_), borderOpa);
    strokeAttr.width = style.borderWidth_;
    strokeAttr.style = &strokeStyle;

    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(true);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path.cmdNum, path.cmds, path.dataNum, path.data);

    if (borderOpa == OPA_TRANSPARENT || style.borderWidth_ == 0) {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_ARC_RECT, FrameTracePathAttr::FILL);
        RenderPath(dst, vguPath, dirtyRect, nullptr, &fillAttr, matrix);
    } else if (bgOpa == OPA_TRANSPARENT) {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_ARC_RECT, FrameTracePathAttr::STROKE);
        RenderPath(dst, vguPath, dirtyRect, &strokeAttr, nullptr, matrix);
    } else {
        SET_TRACE_PATH_TYPE(FrameTraceType::HW_DRAW_ARC_RECT, FrameTracePathAttr::BOTH);
        RenderPath(dst, vguPath, dirtyRect, &strokeAttr, &fillAttr, matrix);
    }

    pathRender_.PushBack(path);
    DeinitPath(vguPath);
#endif
}

#if CAPABILITY_HWDRAW_PATH
void LiteMGfxEngine::ReleasePath(Graphic::Vector<Path> &pathVector)
{
    for (Path* path = pathVector.Begin(); path != pathVector.End(); path++) {
        HWDrawUtils::GetInstance()->FreeBufferForPath(path);
    }
    pathVector.Clear();
}
#endif
}
