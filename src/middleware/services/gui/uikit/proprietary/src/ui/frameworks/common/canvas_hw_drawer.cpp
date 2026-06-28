/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: canvas hardware draw
 * Author: Hisi Graphic Team
 * Created: 2025-9
 */
#include "common/canvas_hw_drawer.h"
#include "common/image_cache_manager.h"
#include "common/paint_ext.h"
#include "display_vgu.h"
#include "gfx_utils/diagram/common/paint.h"
#include "draw/hw_draw_utils.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "hals/display_dev.h"
#include "components/ui_canvas.h"
#include "securec.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
static const uint8_t ALPHA_SHIFT = 24;
static const uint32_t COLOR_MASK = 0x00FFFFFF;

static bool UpdataStopColor(Paint* dstPaint, Paint* srcPaint)
{
    if (dstPaint == nullptr || srcPaint == nullptr) {
        return false;
    }
    // VGU需要连续存储的stopColors, 拷贝过来到新申请的Paint，paintExt是连续内存, paint是List的链表。
    Paint::StopAndColor* srtStopColors = srcPaint->GetStopAndColor();
    uint16_t srcStopCount = srcPaint->GetStopCount();
    Paint::StopAndColor* stopColors = nullptr;
    if (srtStopColors != nullptr && srcStopCount != 0) {
        stopColors = static_cast<Paint::StopAndColor*>(UIMalloc(sizeof(Paint::StopAndColor) * srcStopCount));
        if (stopColors == nullptr) {
            return false;
        }

        if (memcpy_s(stopColors, sizeof(Paint::StopAndColor) * srcStopCount,
            srtStopColors, sizeof(Paint::StopAndColor) * srcStopCount) != EOK) {
            UIFree(stopColors);
            GRAPHIC_LOGE("memcpy path error!");
            return false;
        }
    } else {
        auto stopAndColorList = srcPaint->getStopAndColor();
        srcStopCount = stopAndColorList.Size();
        if (srcStopCount == 0) {
            return true;
        }
        stopColors = static_cast<Paint::StopAndColor*>(UIMalloc(sizeof(Paint::StopAndColor) * srcStopCount));
        if (stopColors == nullptr) {
            return false;
        }

        uint16_t i = 0;
        ListNode<Paint::StopAndColor>* node = stopAndColorList.Begin();
        while (node != stopAndColorList.End() && node != nullptr && i < srcStopCount) {
            stopColors[i++] = node->data_;
            node = node->next_;
        }
    }
    Paint::StopAndColor* stopColorsInner =
            static_cast<Paint::StopAndColor*>(UIMalloc(sizeof(Paint::StopAndColor) * srcStopCount));
    if (stopColorsInner == nullptr) {
        UIFree(stopColors);
        return false;
    }
    dstPaint->SetStopAndColor(stopColors, srcStopCount);
    dstPaint->SetStopAndColorInner(stopColorsInner);
    return true;
}

static void ConvertMatrix(UIView* view, const Matrix3<float>* src, VGUMatrix3& dst)
{
    if (src == nullptr) {
        memset_s(&dst, sizeof(VGUMatrix3), 0, sizeof(VGUMatrix3));
        dst.m[0][0] = 1;
        dst.m[1][1] = 1;
        dst.m[0][2] = view->GetOrigRect().GetLeft() +   // 0, 2: index
            view->GetStyleConst().paddingLeft_ + view->GetStyleConst().borderWidth_;
        dst.m[1][2] = view->GetOrigRect().GetTop() +   // 1, 2: index
            view->GetStyleConst().paddingTop_ + view->GetStyleConst().borderWidth_;
        dst.m[2][2] = 1;  // 2: index
        return;
    }

    HWDrawUtils::GetInstance()->Matrix3ToVguMatrix3(*src, dst);
    return;
}

static bool GetRenderRect(const Rect invalidatedArea, const PaintExt* paint, VGUMatrix3 matrix, Rect& renderRect)
{
    if (paint == nullptr || !paint->hasClipRect_) {
        renderRect = invalidatedArea;
        return true;
    }

    Rect clipRect = paint->clipRect_;
    clipRect.SetX(static_cast<int16_t>(clipRect.GetX() + matrix.m[0][2])); // 2: row & col
    clipRect.SetY(static_cast<int16_t>(clipRect.GetY() + matrix.m[1][2])); // 2: row & col
    if (!clipRect.Intersect(invalidatedArea, clipRect)) {
        return false;
    }
    renderRect = clipRect;
    return true;
}

static bool HandlePaintGradient(VGUGradient& gradient, uint8_t opa, Paint* paint)
{
    if (paint->GetStopAndColor() == nullptr) {
        return false;
    }

    uint16_t stopCount = paint->GetStopCount();
    if (memcpy_s(paint->GetStopAndColorInner(), sizeof(ColorStop) * stopCount,
        paint->GetStopAndColor(), sizeof(ColorStop) * stopCount) == EOK) {
        gradient.stopCount = stopCount;
        gradient.colorStops = reinterpret_cast<VGUColorStop*>(paint->GetStopAndColorInner());
        for (uint16_t i = 0; i < stopCount; i++) {
            uint8_t alpha = ((gradient.colorStops[i].color >> ALPHA_SHIFT) * opa) / OPA_OPAQUE;
            uint32_t mixedColor = (alpha << ALPHA_SHIFT) | (gradient.colorStops[i].color & COLOR_MASK);
            gradient.colorStops[i].color = mixedColor;
        }
    } else {
        GRAPHIC_LOGE("memcpy gradient color stops error!");
        return false;
    }

    Paint::Gradient flag = paint->GetGradient();
    gradient.type = static_cast<VGUGradientType>(flag);
    gradient.spread = static_cast<VGUFillSpread>(0);
    const PaintExt* paintExt = dynamic_cast<const PaintExt*>(paint);
    if (paintExt != nullptr) {
        gradient.spread = static_cast<VGUFillSpread>(paintExt->gradientSpread_);
    }
    if (flag == Paint::Gradient::Linear) {
        Paint::LinearGradientPoint linearGradient = paint->GetLinearGradientPoint();
        gradient.linear.x1 = linearGradient.x0;
        gradient.linear.y1 = linearGradient.y0;
        gradient.linear.x2 = linearGradient.x1;
        gradient.linear.y2 = linearGradient.y1;
    } else if (flag == Paint::Gradient::Sweep) {
        if (paintExt != nullptr) {
            gradient.sweep.cx = paintExt->sweepGradient_.centerX;
            gradient.sweep.cy = paintExt->sweepGradient_.centerY;
            int16_t startAngle = paintExt->sweepGradient_.startAngle - QUARTER_IN_DEGREE;
            gradient.sweep.startAngle = (startAngle % CIRCLE_IN_DEGREE + CIRCLE_IN_DEGREE) % CIRCLE_IN_DEGREE;
        }
    } else if (flag == Paint::Gradient::Radial) {
        Paint::RadialGradientPoint radialGradient = paint->GetRadialGradientPoint();
        gradient.radial.x0 = radialGradient.x0;
        gradient.radial.y0 = radialGradient.y0;
        gradient.radial.r0 = radialGradient.r0;
        gradient.radial.x1 = radialGradient.x1;
        gradient.radial.y1 = radialGradient.y1;
        gradient.radial.r1 = radialGradient.r1;
    }
    return true;
}

static void HandlePaintColor(VGUPaintStyle& style, uint8_t opa, Paint* paint, uint32_t color)
{
    uint8_t alpha = ((color >> ALPHA_SHIFT) * opa) / OPA_OPAQUE;
    uint32_t mixedColor = (alpha << ALPHA_SHIFT) | (color & COLOR_MASK);
    style.color = mixedColor;
    style.type = VGU_PAINT_COLOR;
}

static void HandleStyleAndGradient(VGUPaintStyle& fillStyle, VGUPaintStyle& strokeStyle,
    VGUGradient &gradient, uint8_t opa, Paint* paint)
{
    if (paint->GetStyle() == Paint::PaintStyle::STROKE_FILL_STYLE) {
        if (HandlePaintGradient(gradient, opa, paint)) {
            fillStyle.gradient = &gradient;
            strokeStyle.gradient = &gradient;
        } else {
            HandlePaintColor(fillStyle, opa, paint, paint->GetFillColor().full);
            HandlePaintColor(strokeStyle, opa, paint, paint->GetStrokeColor().full);
        }
    } else {
        const PaintExt* paintExt = dynamic_cast<const PaintExt *>(paint);
        if (paintExt != nullptr && paintExt->paintType_ == PaintType::PAINT_PATTERN) {
            return;
        }
        if (paint->GetStyle() == Paint::PaintStyle::STROKE_STYLE) {
            if (HandlePaintGradient(gradient, opa, paint)) {
                strokeStyle.gradient = &gradient;
            } else {
                HandlePaintColor(strokeStyle, opa, paint, paint->GetStrokeColor().full);
            }
        } else {
            if (HandlePaintGradient(gradient, opa, paint)) {
                fillStyle.gradient = &gradient;
            } else {
                HandlePaintColor(fillStyle, opa, paint, paint->GetFillColor().full);
            }
        }
    }
}

static bool HandleStyleAndPattern(UICanvas* view, VGUPaintStyle &style, VGUPattern &pattern, uint8_t opa,
    Matrix3<float> *matrix, PaintExt *paint)
{
    if (paint->pattern_.img.phyAddr == nullptr) {
        GRAPHIC_LOGE("Image is invalid!");
        return false;
    }

    ImageCacheManager::GetInstance().UpdateImageInfoIfNecessary(paint->pattern_.img);
    if (paint->pattern_.img.data == nullptr) {
        GRAPHIC_LOGE("HandleStyleAndPattern failed. UpdateImageInfoIfNecessary failed.");
        return false;
    }

    style.pattern = &pattern;
    pattern.wrapx = static_cast<VGUWrapType>(static_cast<uint8_t>(paint->pattern_.wrap));
    pattern.image->buffer.width = paint->pattern_.img.header.width;
    pattern.image->buffer.height = paint->pattern_.img.header.height;
    uint8_t alpha = ((paint->pattern_.color >> ALPHA_SHIFT) * opa) / OPA_OPAQUE;
    uint32_t mixedColor = (alpha << ALPHA_SHIFT) | (paint->pattern_.color & COLOR_MASK);
    pattern.image->buffer.color = mixedColor;
    pattern.image->buffer.memHandle = static_cast<uint64_t>(uintptr_t(paint->pattern_.img.phyAddr));

    LiteMGfxEngine::GetInstance()->Convert2VGUFormat(static_cast<ColorMode>(paint->pattern_.img.header.colorMode),
        pattern.image->buffer.pixelFormat);
    if (paint->pattern_.img.header.compressMode == 0) {
        pattern.image->buffer.stride =
            DisplayDev::GetInstance()->CalcStride(pattern.image->buffer.width, pattern.image->buffer.pixelFormat);
    } else {
        pattern.image->enCompressed = true;
    }

    Matrix3<float> mat = *matrix * paint->pattern_.mat3;
    ConvertMatrix(view, &mat, pattern.image->matrix);

    style.type = VGU_PAINT_PATTERN;
    return true;
}

static VGULineCap VGULineCapConvert(LineCap lineCap)
{
    if (lineCap == LineCap::SQUARE_CAP) {
        return VGULineCap::VGU_LINECAP_SQUARE;
    } else if (lineCap == LineCap::ROUND_CAP) {
        return VGULineCap::VGU_LINECAP_ROUND;
    }
    return VGULineCap::VGU_LINECAP_BUTT;
}

static VGUJointType VGULineJoinConvert(LineJoin lineJoin)
{
    if (lineJoin == LineJoin::MITER_JOIN) {
        return VGUJointType::VGU_LINE_JOIN_MITER;
    } else if (lineJoin == LineJoin::ROUND_JOIN) {
        return VGUJointType::VGU_LINE_JOIN_ROUND;
    } else if (lineJoin == LineJoin::BEVEL_JOIN) {
        return VGUJointType::VGU_LINE_JOIN_BEVEL;
    }
    return VGUJointType::VGU_LINE_JOIN_BUTT;
}

static void DrawPathInner(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea,
    CanvasDrawParam* pathParam, UICanvas* canvas, Paint* paint)
{
    Path *path = pathParam->object.path;
    Matrix3<float> *matrix = &(pathParam->matrix);

    VGUMatrix3 mat;
    ConvertMatrix(canvas, matrix, mat);
    PaintExt* paintExt = dynamic_cast<PaintExt *>(paint);
    bool antialiased = (paintExt == nullptr) ? true : paintExt->antialiased_;
    uintptr_t vguPath = LiteMGfxEngine::GetInstance()->InitPath(antialiased);
    LiteMGfxEngine::GetInstance()->SetPathData(vguPath, path->cmdNum, path->cmds, path->dataNum, path->data);
    Rect renderRect = invalidatedArea;
    if (paintExt != nullptr && !GetRenderRect(invalidatedArea, paintExt, mat, renderRect)) {
        return;
    }
    VGUPaintStyle fillStyle = {0};
    VGUFillAttr fillAttr = {VGU_RULE_WINDING, &fillStyle};
    if (paintExt != nullptr) {
        fillAttr.rule = static_cast<VGUFillRule>(paintExt->fillRule_);
    }
    VGUPaintStyle strokeStyle = {0};
    VGUDashAttr dashAttr = {paint->GetLineDash(), paint->GetLineDashCount(), paint->GetLineDashOffset(), true};
    VGUStrokeAttr strokeAttr = {VGULineCapConvert(paint->GetLineCap()), VGULineJoinConvert(paint->GetLineJoin()),
        paint->GetMiterLimit(), paint->GetStrokeWidth(), &strokeStyle, &dashAttr};
    VGUGradient gradient = {0};
    uint8_t mixOpa = pathParam->mixOpa * paint->GetOpacity() / OPA_OPAQUE;
    HandleStyleAndGradient(fillStyle, strokeStyle, gradient, mixOpa, paint);
    if (paint->GetStyle() == Paint::PaintStyle::STROKE_FILL_STYLE) {
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, renderRect, &strokeAttr, &fillAttr, &mat);
        return;
    }
    VGUImage image;
    memset_s(&image, sizeof(VGUImage), 0, sizeof(VGUImage));
    VGUPattern pattern = {&image};
    if (paint->GetStyle() == Paint::PaintStyle::STROKE_STYLE) {
        if ((paintExt != nullptr) && (paintExt->paintType_ == PaintType::PAINT_PATTERN)) {
            if (!HandleStyleAndPattern(canvas, strokeStyle, pattern, mixOpa, matrix, paintExt)) {
                return;
            }
        }
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, renderRect, &strokeAttr, nullptr, &mat);
    } else {
        if ((paintExt != nullptr) && (paintExt->paintType_ == PaintType::PAINT_PATTERN)) {
            if (!HandleStyleAndPattern(canvas, fillStyle, pattern, mixOpa, matrix, paintExt)) {
                return;
            }
        }
        LiteMGfxEngine::GetInstance()->RenderPath(gfxDstBuffer, vguPath, renderRect, nullptr, &fillAttr, &mat);
    }
}

CanvasDrawParam* CanvasHwDrawer::CreateCanvasDrawParam(CanvasHwDrawType type, const Paint* paint, UIView* canvas)
{
    if (paint == nullptr || canvas == nullptr) {
        return nullptr;
    }

    CanvasDrawParam* param = new CanvasDrawParam();
    if (param == nullptr) {
        GRAPHIC_LOGE("Alloc CanvasDrawParam failed.");
        return nullptr;
    }

    param->type = type;
    if (type == CanvasHwDrawType::PARAM_TYPE_PATH) {
        Path* path = new Path();
        (void)memset_s(path, sizeof(Path), 0, sizeof(Path));
        param->object.path = path;
    } else if (type == CanvasHwDrawType::PARAM_TYPE_IMAGE) {
        param->object.imageView = new UIImageView();
    } else if (type == CanvasHwDrawType::PARAM_TYPE_LABEL) {
        param->object.label = new UILabel();
    } else if (type == PARAM_TYPE_BITMAP) {
        param->object.bitmap = new BufferInfo();
    } else if (type == PARAM_TYPE_RENDER_CACHE) {
        param->object.cacheKey = 0;
    } else {
        GRAPHIC_LOGE("type not support, %d\n", type);
        DestoryCanvasDrawParam(param);
        return nullptr;
    }

    const PaintExt* paintExt = dynamic_cast<const PaintExt*>(paint);
    param->paint = (paintExt == nullptr) ? new Paint() : new PaintExt();
    if (param->paint == nullptr) {
        DestoryCanvasDrawParam(param);
        return nullptr;
    }
    if (paintExt == nullptr) {
        *param->paint = *paint;
    } else {
        *static_cast<PaintExt*>(param->paint) = *paintExt;
    }

    if (!UpdataStopColor(param->paint, const_cast<Paint*>(paint))) {
        DestoryCanvasDrawParam(param);
        GRAPHIC_LOGE("UpdataStopColor faild\n");
        return nullptr;
    }
    param->index = -1;
    param->canvasView = canvas;
    return param;
}

void CanvasHwDrawer::DestoryCanvasDrawParam(CanvasDrawParam* param)
{
    if (param == nullptr) {
        return;
    }

    LiteMGfxEngine::GetInstance()->SyncHwDraw();
    if (param->type == CanvasHwDrawType::PARAM_TYPE_PATH && param->object.path != nullptr) {
        HWDrawUtils::GetInstance()->FreeBufferForPath(param->object.path);
        delete param->object.path;
    } else if (param->type == CanvasHwDrawType::PARAM_TYPE_IMAGE && param->object.imageView != nullptr) {
        delete param->object.imageView;
    } else if (param->type == CanvasHwDrawType::PARAM_TYPE_LABEL && param->object.label != nullptr) {
        delete param->object.label;
    } else if (param->type == CanvasHwDrawType::PARAM_TYPE_BITMAP && param->object.bitmap != nullptr) {
        delete param->object.bitmap;
    }

    if (param->paint != nullptr) {
        // paint 析构会清理stopColor
        delete param->paint;
    }
    delete param;
}

static void DoHwDrawPath(BufferInfo& gfxDstBuffer, void* param, Paint* paint, const Rect& rect,
    const Rect& invalidatedArea, const Style& style)
{
    CanvasDrawParam* pathParam = static_cast<CanvasDrawParam*>(param);

    Path* path = pathParam->object.path;
    if (path->cmdNum == 0 || path->dataNum == 0 || path->cmds == nullptr || path->data == nullptr) {
        return;
    }
    UICanvas* canvas = static_cast<UICanvas*>(pathParam->canvasView);
    Matrix3<float> viewMatrix;
    if (!canvas->IsTransInvalid()) {
        const float* data = canvas->GetTransformMap().GetOrigTransformMatrix().GetData();
        // 0, 1, 3, 4, 5, 7, 12, 13: matrix data index
        viewMatrix = Matrix3<float>(data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], 1);
    }

    auto translateMatrix = Matrix3<float>::Translate(
        Vector2<float>(rect.GetX() + canvas->GetStyle(STYLE_PADDING_LEFT) + canvas->GetStyle(STYLE_BORDER_WIDTH),
            rect.GetY() + canvas->GetStyle(STYLE_PADDING_TOP) + canvas->GetStyle(STYLE_BORDER_WIDTH)));
    if (paint!= nullptr && !paint->GetTransAffine().IsIdentity()) {
        const float* data = paint->GetTransAffine().GetData();
        Matrix3<float> paintMatrix;
        // 0, 1, 2, 3, 4, 5, 6, 7, 8: matrix data index
        paintMatrix = Matrix3<float>(data[0], data[3], data[6], data[1], data[4], data[7], data[2], data[5], data[8]);
        if (canvas->IsTransInvalid()) {
            pathParam->matrix = translateMatrix * paintMatrix * pathParam->matrix;
        } else {
            pathParam->matrix = viewMatrix * translateMatrix * paintMatrix * pathParam->matrix;
        }
    } else {
        if (canvas->IsTransInvalid()) {
            pathParam->matrix = translateMatrix * pathParam->matrix;
        } else {
            pathParam->matrix = viewMatrix * translateMatrix * pathParam->matrix;
        }
    }

    DrawPathInner(gfxDstBuffer, invalidatedArea, pathParam, canvas, paint);
}

static void DoHwDrawImage(BufferInfo& gfxDstBuffer, void* param, Paint* paint, const Rect& rect,
    const Rect& invalidatedArea, const Style& style)
{
    CanvasDrawParam* imageParam = static_cast<CanvasDrawParam*>(param);
    UIImageView* imageView = imageParam->object.imageView;
    if (!paint->GetTransAffine().IsIdentity()) {
        const float* data = paint->GetTransAffine().GetData();
        Matrix4<float> paintMatrix = Matrix4<float>(data[0], data[3], 0, data[6], data[1], data[4], 0, data[7],
            0, 0, 1, 0, data[2], data[5], 0, data[8]);

        TransformMap& imageMap = imageView->GetTransformMap();
        imageMap.SetTransMapRect(imageView->GetOrigRect());
        Rect rect = imageView->GetOrigRect();
        Matrix4<float> transToOrigin = Matrix4<float>::Translate(Vector3<float>(-rect.GetX(), -rect.GetY(), 0));
        Matrix4<float> transToPivot = Matrix4<float>::Translate(Vector3<float>(rect.GetX(), rect.GetY(), 0));
        Matrix4<float> imageMatrix = imageMap.GetOrigTransformMatrix() * transToPivot * paintMatrix * transToOrigin;
        imageMap.SetMatrix(imageMatrix, false);
    }

    TransformMap& imageMap = imageView->GetTransformMap();
    Matrix4<float> paintMatrix = imageMap.GetOrigTransformMatrix();
    const float* data = imageParam->matrix.GetData();
    Matrix4<float> dynamicMatrix = Matrix4<float>(data[0], data[1], 0, data[2], data[3], data[4], 0, data[5],
        0, 0, 1, 0, data[6], data[7], 0, data[8]);
    UICanvas* canvas = static_cast<UICanvas*>(imageParam->canvasView);
    if (canvas->IsTransInvalid()) {
        Matrix4<float> transMatrix = Matrix4<float>::Translate({rect.GetX(), rect.GetY(), 0});
        Matrix4<float> imageMatrix = transMatrix * dynamicMatrix * paintMatrix;
        imageMap.SetMatrix(imageMatrix, false);
    } else {
        Matrix4<float> canvasMatrix = canvas->GetTransformMap().GetTransformMatrix();
        Matrix4<float> imageMatrix = canvasMatrix * dynamicMatrix * paintMatrix;
        imageMap.SetMatrix(imageMatrix, false);
    }

    imageView->SetOpaScale(imageParam->mixOpa * paint->GetOpacity() / OPA_OPAQUE);
    imageView->OnDraw(gfxDstBuffer, invalidatedArea);
    imageMap.SetMatrix(paintMatrix, true);
}

static void DoHwDrawLabel(BufferInfo& gfxDstBuffer, void* param, Paint* paint, const Rect& rect,
    const Rect& invalidatedArea, const Style& style)
{
    CanvasDrawParam* labelParam = static_cast<CanvasDrawParam*>(param);
    UILabel* label = labelParam->object.label;

    TransformMap& labelMap = label->GetTransformMap();
    Matrix4<float> ortMatrix = labelMap.GetOrigTransformMatrix();
    const float* data = labelParam->matrix.GetData();
    Matrix4<float> dynamicMatrix = Matrix4<float>(data[0], data[1], 0, data[2], data[3], data[4], 0, data[5],
        0, 0, 1, 0, data[6], data[7], 0, data[8]);

    Matrix4<float> labelMatrix;
    UICanvas* canvas = static_cast<UICanvas*>(labelParam->canvasView);
    if (!paint->GetTransAffine().IsIdentity()) {
        const float* data = paint->GetTransAffine().GetData();
        // 0, 1, 2, 3, 4, 5, 6, 7, 8: matrix data index
        Matrix4<float> paintMatrix = Matrix4<float>(data[0], data[3], 0, data[6], data[1], data[4], 0, data[7],
            0, 0, 1, 0, data[2], data[5], 0, data[8]);
        if (canvas->IsTransInvalid()) {
            Matrix4<float> transMatrix = Matrix4<float>::Translate({rect.GetX(), rect.GetY(), 0});
            labelMatrix = transMatrix * paintMatrix * dynamicMatrix;
        } else {
            Matrix4<float> canvasMatrix = canvas->GetTransformMap().GetTransformMatrix();
            labelMatrix = canvasMatrix * paintMatrix * dynamicMatrix;
        }
    } else {
        if (canvas->IsTransInvalid()) {
            Matrix4<float> transMatrix = Matrix4<float>::Translate({rect.GetX(), rect.GetY(), 0});
            labelMatrix = transMatrix * dynamicMatrix;
        } else {
            Matrix4<float> canvasMatrix = canvas->GetTransformMap().GetTransformMatrix();
            labelMatrix = canvasMatrix * dynamicMatrix;
        }
    }
    labelMap.SetTransMapRect(label->GetRect());
    labelMap.SetMatrix(labelMatrix, false);
    if (!label->GetRect().IsIntersect(invalidatedArea)) {
        labelMap.SetMatrix(ortMatrix, true);
        return;
    }
    label->SetStyle(STYLE_TEXT_OPA, labelParam->mixOpa * paint->GetOpacity() / OPA_OPAQUE);
    label->OnDraw(gfxDstBuffer, invalidatedArea);
    labelMap.SetMatrix(ortMatrix, true);
}

static void DoHwDrawBitmap(BufferInfo& gfxDstBuffer, void* param, Paint* paint, const Rect& rect,
    const Rect& invalidatedArea, const Style& style)
{
    CanvasDrawParam* bitmapParam = static_cast<CanvasDrawParam*>(param);
    BufferInfo* bitmap = bitmapParam->object.bitmap;
    if (bitmap->virAddr == nullptr || bitmap->width == 0 || bitmap->height == 0 || bitmap->stride == 0) {
        return;
    }
    Matrix4<float> bitmapMatrix;
    UICanvas* canvas = static_cast<UICanvas*>(bitmapParam->canvasView);
    if (paint!= nullptr && !paint->GetTransAffine().IsIdentity()) {
        const float* data = paint->GetTransAffine().GetData();
        Matrix4<float> paintMatrix = Matrix4<float>(data[0], data[3], 0, data[6], data[1], data[4], 0, data[7],
            0, 0, 1, 0, data[2], data[5], 0, data[8]);
        if (canvas->IsTransInvalid()) {
            Matrix4<float> transMatrix = Matrix4<float>::Translate({rect.GetX(), rect.GetY(), 0});
            bitmapMatrix = transMatrix * paintMatrix;
        } else {
            Matrix4<float> canvasMatrix = canvas->GetTransformMap().GetTransformMatrix();
            bitmapMatrix = canvasMatrix * paintMatrix;
        }
    } else {
        if (canvas->IsTransInvalid()) {
            bitmapMatrix = Matrix4<float>::Translate({rect.GetX(), rect.GetY(), 0});
        } else {
            bitmapMatrix = canvas->GetTransformMap().GetTransformMatrix();
        }
    }

    TransformMap transMap;
    transMap.SetTransMapRect(bitmap->rect);
    transMap.SetMatrix(bitmapMatrix, false);
    Rect invalidRect = invalidatedArea;
    if (invalidRect.Intersect(invalidRect, transMap.GetBoxRect())) {
        ColorType inputColor;
        inputColor.full = bitmap->color;
        uint8_t opa = bitmapParam->mixOpa * paint->GetOpacity() / OPA_OPAQUE;
        LiteMGfxEngine::GetInstance()->DrawTransform(gfxDstBuffer, invalidRect,
            {0, 0}, inputColor, opa, transMap, *bitmap);
    }
}

static void DoHwDrawRenderCache(BufferInfo& gfxDstBuffer, void* param, Paint* paint, const Rect& rect,
    const Rect& invalidatedArea, const Style& style)
{
    CanvasDrawParam* renderCacheParam = static_cast<CanvasDrawParam*>(param);
    UICanvas* canvas = static_cast<UICanvas*>(renderCacheParam->canvasView);
    uint32_t cacheKey = renderCacheParam->object.cacheKey;
    if (cacheKey == 0) {
        return;
    }
    Matrix3<float> viewMatrix;
    if (!canvas->IsTransInvalid()) {
        const float* data = canvas->GetTransformMap().GetOrigTransformMatrix().GetData();
        // 0, 1, 3, 4, 5, 7, 12, 13: matrix data index
        viewMatrix = Matrix3<float>(data[0], data[1], data[3], data[4], data[5], data[7], data[12], data[13], 1);
    }

    auto translateMatrix = Matrix3<float>::Translate(
        Vector2<float>(rect.GetX() + canvas->GetStyle(STYLE_PADDING_LEFT) + canvas->GetStyle(STYLE_BORDER_WIDTH),
            rect.GetY() + canvas->GetStyle(STYLE_PADDING_TOP) + canvas->GetStyle(STYLE_BORDER_WIDTH)));
    if (paint!= nullptr && !paint->GetTransAffine().IsIdentity()) {
        const float* data = paint->GetTransAffine().GetData();
        Matrix3<float> paintMatrix;
        // 0, 1, 2, 3, 4, 5, 6, 7, 8: matrix data index
        paintMatrix = Matrix3<float>(data[0], data[3], data[6], data[1], data[4], data[7], data[2], data[5], data[8]);
        if (canvas->IsTransInvalid()) {
            renderCacheParam->matrix = translateMatrix * paintMatrix * renderCacheParam->matrix;
        } else {
            renderCacheParam->matrix = viewMatrix * translateMatrix * paintMatrix * renderCacheParam->matrix;
        }
    } else {
        if (canvas->IsTransInvalid()) {
            renderCacheParam->matrix = translateMatrix * renderCacheParam->matrix;
        } else {
            renderCacheParam->matrix = viewMatrix * translateMatrix * renderCacheParam->matrix;
        }
    }
    VGUMatrix3 mat;
    ConvertMatrix(canvas, &renderCacheParam->matrix, mat);
    PaintExt* paintExt = dynamic_cast<PaintExt *>(paint);
    Rect renderRect = invalidatedArea;
    if (paintExt != nullptr && !GetRenderRect(invalidatedArea, paintExt, mat, renderRect)) {
        return;
    }
    LiteMGfxEngine::GetInstance()->SetGroupRenderBuffer(cacheKey, 0, gfxDstBuffer, renderRect, &mat);
    LiteMGfxEngine::GetInstance()->DoRenderByCache(cacheKey);
}

void CanvasHwDrawer::DoDraw(BufferInfo& gfxDstBuffer, void* param, const Paint& paint,
    const Rect& rect, const Rect& invalidatedArea, const Style& style)
{
    if (param == nullptr) {
        return;
    }
    CanvasDrawParam* canvasParam = static_cast<CanvasDrawParam*>(param);
    if (canvasParam->paint == nullptr) {
        return;
    }
    if (canvasParam->type == CanvasHwDrawType::PARAM_TYPE_PATH) {
        DoHwDrawPath(gfxDstBuffer, param, canvasParam->paint, rect, invalidatedArea, style);
    } else if (canvasParam->type == CanvasHwDrawType::PARAM_TYPE_IMAGE) {
        DoHwDrawImage(gfxDstBuffer, param, canvasParam->paint, rect, invalidatedArea, style);
    } else if (canvasParam->type == CanvasHwDrawType::PARAM_TYPE_LABEL) {
        DoHwDrawLabel(gfxDstBuffer, param, canvasParam->paint, rect, invalidatedArea, style);
    } else if (canvasParam->type == CanvasHwDrawType::PARAM_TYPE_BITMAP) {
        DoHwDrawBitmap(gfxDstBuffer, param, canvasParam->paint, rect, invalidatedArea, style);
    } else if (canvasParam->type == CanvasHwDrawType::PARAM_TYPE_RENDER_CACHE) {
        DoHwDrawRenderCache(gfxDstBuffer, param, canvasParam->paint, rect, invalidatedArea, style);
    }
}

void CanvasHwDrawer::DeleteParam(void* param)
{
    DestoryCanvasDrawParam(static_cast<CanvasDrawParam*>(param));
}
#endif
}
