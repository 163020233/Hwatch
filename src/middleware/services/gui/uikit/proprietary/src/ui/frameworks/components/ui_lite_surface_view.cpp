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

#include "imgdecode/image_load.h"
#include "components/root_view.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "graphic_service.h"
#include "draw/draw_rect.h"
#include "draw/draw_utils.h"
#include "draw/hw_draw_utils.h"
#include "dfx/dfx_frame_trace.h"
#include "gfx_utils/graphic_log.h"
#include "hals/display_dev.h"
#include "hals/gralloc_engines.h"
#include "securec.h"
#include "yuv_to_rgb.h"
#include "common/low_power_manager.h"
#include "common/task.h"
#include "components/ui_lite_surface_view.h"

#ifndef UIKIT_SKIP_LINE
#define UIKIT_SKIP_LINE 1
#endif

namespace OHOS {
static constexpr int16_t YUV_WIDTH_ALIGNMENT = 128;
#if ENABLE_VIDEO_RGB
class UILiteSurfaceView::SurfaceTask : public Task {
public:
    explicit SurfaceTask(UILiteSurfaceView &surface) : surface_(surface)
    {
        period_ = 0;
        Task::Init();
    }

    ~SurfaceTask() override
    {
        Task::Deinit();
    }

    void Callback() override
    {
        if (enable_) {
            surface_.Invalidate();
            enable_ = false;
        }
    }

    void EnableInvalidate()
    {
        enable_ = true;
    }

private:
    bool enable_ = false;
    UILiteSurfaceView &surface_;
};
#endif

UILiteSurfaceView::UILiteSurfaceView() : isLayerInit_(false), flushCount_(0)
{
    surface_ = std::make_shared<LiteSurface>();
    if (surface_ == nullptr) {
        GRAPHIC_LOGE("new LiteSurface failed\n");
        return;
    }
    surface_->RegisterConsumerListener(*this);
#if ENABLE_VIDEO_RGB
    surfaceTask_ = new (std::nothrow) SurfaceTask(*this);
#endif
}

UILiteSurfaceView::~UILiteSurfaceView()
{
    surface_->UnregisterConsumerListener();
#if !ENABLE_VIDEO_RGB
    if (isLayerInit_) {
        DisplayDev::GetInstance()->CloseLayer(LAYER_1);
        DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, false);
    }

    LiteMGfxEngine::GetInstance()->ClearHardwareLayer(this);
    GraphicService::GetInstance()->PostGraphicEvent([] {
            if (LiteMGfxEngine::GetInstance()->GetHardwareLayer() == nullptr) {
                GraphicService::GetInstance()->EnableAsyncMode(true);
            }
        });
#else
    ClearTempBufferInfo();
#endif

    if (preView_ != nullptr) {
        delete preView_;
        preView_ = nullptr;
    }

#if ENABLE_VIDEO_RGB
    if (surfaceTask_ != nullptr) {
        delete surfaceTask_;
        surfaceTask_ = nullptr;
    }
#endif
}

Surface* UILiteSurfaceView::GetSurface() const
{
    return surface_.get();
}

void UILiteSurfaceView::SetPosition(int16_t x, int16_t y, int16_t width, int16_t height)
{
    if (GetWidth() > 1 || GetHeight() > 1) {
        GRAPHIC_LOGE("SetPosition failed. width or height is already set.");
    } else {
        UIView::SetPosition(x, y, width, height);
    }
}

void UILiteSurfaceView::Resize(int16_t width, int16_t height)
{
    if (GetWidth() > 1 || GetHeight() > 1) {
        GRAPHIC_LOGE("Resize failed. width or height is already set.");
    } else {
        UIView::Resize(width, height);
    }
}

void UILiteSurfaceView::SetWidth(int16_t width)
{
    if (GetWidth() > 1) {
        GRAPHIC_LOGE("SetWidth failed. width is already set.");
    } else {
        UIView::SetWidth(width);
    }
}

void UILiteSurfaceView::SetHeight(int16_t height)
{
    if (GetHeight() > 1) {
        GRAPHIC_LOGE("SetHeight failed. height is already set.");
    } else {
        UIView::SetHeight(height);
    }
}

void UILiteSurfaceView::Flush()
{
    if (flushCount_ > 0) {
        lock_.Lock();
        --flushCount_;
        lock_.Unlock();

        if (!LowPowerManager::GetInstance()->IsLiteSurfaceInRootView()) {
            return;
        }

#if !ENABLE_VIDEO_RGB
        currentBuffer_ = surface_->AcquireBuffer();
        if (currentBuffer_ == nullptr) {
            return;
        }
        LayerBuffer layer;
        (void)memset_s(&layer, sizeof(LayerBuffer), 0, sizeof(LayerBuffer));
        layer.width = static_cast<int32_t>(currentBuffer_->width);
        layer.height = static_cast<int32_t>(currentBuffer_->height);
        layer.pitch = static_cast<int32_t>(currentBuffer_->stride);
        layer.pixFormat = currentBuffer_->format;
        layer.data.phyAddr = currentBuffer_->addr;
        layer.data.virAddr = reinterpret_cast<void*>(currentBuffer_->addr);

        FRAME_TRACE_START(FrameTraceType::HW_DRAW_FLUSH);
        DisplayDev::GetInstance()->SetHardwareLayerBuffer(LAYER_1, BUFFER_0, layer);
        DisplayDev::GetInstance()->LcdFlush(LAYER_1);
        FRAME_TRACE_END(FrameTraceType::HW_DRAW_FLUSH, nullptr);
#endif
    }

    if (flushCount_ > 0) {
        Invalidate();
    }
}

void UILiteSurfaceView::PostFlush()
{
    if (currentBuffer_ != nullptr) {
        surface_->ReleaseBuffer(lastFlushedBuffer_);
        lastFlushedBuffer_ = currentBuffer_;
        currentBuffer_ = nullptr;
    }
}

#if !ENABLE_VIDEO_RGB
void UILiteSurfaceView::SetSurfaceColorkey(ColorType color)
{
    colorKey_ = color;
}

void UILiteSurfaceView::InitLayer()
{
    Rect rect(GetRect());
    int16_t width = rect.GetWidth();
    int16_t height = rect.GetHeight();
    uint32_t fmt = surface_->GetFormat();
    uint32_t size = surface_->GetSize();
    uint8_t queueSize = surface_->GetQueueSize();
    if (width > 1 && height > 1 && size != 0 && queueSize != 0 && fmt != PIXEL_FMT_BUTT) {
        LayerInfo layerInfo;
        layerInfo.width = width;
        layerInfo.height = height;
        layerInfo.type = LAYER_TYPE_GRAPHIC;
        layerInfo.pixFormat = static_cast<PixelFormat>(surface_->GetFormat());
        DisplayDev::GetInstance()->OpenLayer(LAYER_1, layerInfo, 1);
        Rect dirtyRect(0, 0, width - 1, height - 1);
        DisplayDev::GetInstance()->SetLayerPositon(LAYER_1, rect.GetX(), rect.GetY());
        DisplayDev::GetInstance()->SetLayerDirtyRegion(LAYER_1, dirtyRect);
        DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, true, colorKey_.full);
        isLayerInit_ = true;
        layerRect_ = rect;
    }
}

#ifdef VERSION_IOT
bool UILiteSurfaceView::GetSurfaceBufferBitmap(ImageInfo& bitmap)
{
    CscYuvBuf yuv = {0};
    yuv.yAddr = reinterpret_cast<uint8_t*>(lastFlushedBuffer_->addr);
    yuv.uvAddr = static_cast<uint8_t*>(yuv.yAddr) + ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT) *
        lastFlushedBuffer_->height;
    yuv.width = lastFlushedBuffer_->width;
    yuv.height = lastFlushedBuffer_->height;
    yuv.yStride = ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT);
    yuv.uvStride = ALIGN_BYTE(lastFlushedBuffer_->width, YUV_WIDTH_ALIGNMENT);

    CscRgbBuf rgb = {0};
#if UIKIT_SKIP_LINE
    rgb.width = lastFlushedBuffer_->width / 2; // 2: half
    rgb.height = lastFlushedBuffer_->height / 2; // 2: half
#else
    rgb.width = lastFlushedBuffer_->width;
    rgb.height = lastFlushedBuffer_->height;
#endif
    rgb.stride = ALIGN_BYTE(rgb.width, BYTE_ALIGNMENT) * 3; // 3: 3 bytes for rgb888

    bitmap.header.colorMode = RGB888;
    bitmap.dataSize = rgb.stride * rgb.height;
    bitmap.header.width = rgb.width;
    bitmap.header.height = rgb.height;
    bitmap.header.compressMode = 0;
    bitmap.header.reserved = 0;
    rgb.addr = reinterpret_cast<uint8_t*>(ImageCacheMalloc(bitmap));
    if (rgb.addr == nullptr) {
        GRAPHIC_LOGE("UILiteSurfaceView::GetBitmap img alloc failed! size: 0x%x\n", bitmap.dataSize);
        return false;
    }

#if UIKIT_SKIP_LINE
    CscConvertYuvToRgbWithSkipLP(&rgb, &yuv);
#else
    CscConvertYuvToRgb(&rgb, &yuv);
#endif
    return true;
}

void UILiteSurfaceView::BlitBitmapToFb(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, ImageInfo& bitmap)
{
    BufferInfo src;
    src.rect = {0, 0, bitmap.header.width - 1, bitmap.header.height - 1};
    src.virAddr = static_cast<void*>(const_cast<uint8_t*>(bitmap.data));
    src.phyAddr = static_cast<void*>(const_cast<uint8_t*>(bitmap.phyAddr));
    src.stride = ALIGN_BYTE(bitmap.header.width, BYTE_ALIGNMENT) * 3; // 3: 3 bytes for rgb888;
    src.mode = RGB888;
    src.color = 0;
    src.width = bitmap.header.width;
    src.height = bitmap.header.height;
    src.compressMode = COMPRESS_MODE_NONE;
    src.rotType = HWDrawUtils::GetInstance()->GetBufferInfoRotType(bitmap.header.reserved);

    Rect rect = GetRect();
    rect.Intersect(rect, invalidatedArea);
    BlendOption blendOption;
#if UIKIT_SKIP_LINE
    blendOption.transMap.Scale(Vector2<float>{2.0, 2.0}, Vector2<float>{0, 0}); // 2.0: scale 2
    blendOption.transMap.Translate(Vector2<int16_t>(GetRect().GetX(), GetRect().GetY()));
#endif
    blendOption.opacity = OPA_OPAQUE;
    blendOption.mode = BLEND_SRC_OVER;
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, {0, 0}, src, rect, blendOption);
}
#endif

void UILiteSurfaceView::AdjustArea(Rect& rect)
{
    if (rect.GetX() % 2 != 0) { // 2: mod
        if (rect.GetX() < 0) {
            rect.SetX(rect.GetX() + 1);
        } else {
            rect.SetX(rect.GetX() - 1);
        }
    }

    if (rect.GetY() % 2 != 0) { // 2: mod
        if (rect.GetY() < 0) {
            rect.SetY(rect.GetY() + 1);
        } else {
            rect.SetY(rect.GetY() - 1);
        }
    }
}
#else

bool UILiteSurfaceView::Convert2ColorMode(const PixelFormat& colorFmt, ColorMode& dstColorMode)
{
    switch (colorFmt) {
        case PIXEL_FMT_YCBCR_420_SP:
            dstColorMode = YCBCR420;
            break;
        default:
            GRAPHIC_LOGE("unsupported color format: %d\n", colorFmt);
            return false;
    }
    return true;
}

void UILiteSurfaceView::DrawImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa)
{
    if (flushCount_ > 0) {
        lock_.Lock();
        --flushCount_;
        lock_.Unlock();
        LiteMGfxEngine::GetInstance()->AsyncFlush();
        SurfaceBuffer* surfaceBuffer = surface_->AcquireBuffer();
        if (surfaceBuffer != nullptr) {
            if (lastFlushedBuffer_ != nullptr) {
                surface_->ReleaseBuffer(lastFlushedBuffer_);
            }
            lastFlushedBuffer_ = surfaceBuffer;
        }
    }

    if (surfaceTask_ != nullptr && flushCount_ > 0) {
        surfaceTask_->EnableInvalidate();
    }

    if (lastFlushedBuffer_ == nullptr) {
        return;
    }

    if (lastFlushedBuffer_->format == PIXEL_FMT_YCBCR_420_SP) {
        DrawYUVImage(gfxDstBuffer, invalidatedArea, opa);
    } else if (lastFlushedBuffer_->format == PIXEL_FMT_AYCBCR_420_SP) {
        DrawAYUVImage(gfxDstBuffer, invalidatedArea, opa);
    } else {
        GRAPHIC_LOGE("Unsupported format: %d\n", lastFlushedBuffer_->format);
    }
}

static void CalcTransMap(TransformMap& map, UILiteSurfaceView::VideoRotation rotation, Rect center)
{
    if (rotation == UILiteSurfaceView::VideoRotation::ROTATE_90) {
        map.Rotate(90.0f, {0, 0});
        map.Translate(Vector2<int16_t>(center.GetX() + center.GetBottom(), center.GetY()));
    } else if (rotation == UILiteSurfaceView::VideoRotation::ROTATE_180) {
        map.Rotate(180.0f, {0, 0});
        map.Translate(Vector2<int16_t>(center.GetX() + center.GetRight(),
                                       center.GetY() + center.GetBottom()));
    } else if (rotation == UILiteSurfaceView::VideoRotation::ROTATE_270) {
        map.Rotate(270.0f, {0, 0});
        map.Translate(Vector2<int16_t>(center.GetX(), center.GetY() + center.GetRight()));
    }
}

void UILiteSurfaceView::DrawYUVImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa)
{
    ColorMode colorMode;
    if (!Convert2ColorMode(lastFlushedBuffer_->format, colorMode)) {
        GRAPHIC_LOGE("convert color mode fail, format : %d\n", lastFlushedBuffer_->format);
        return;
    }

    Rect frameRect(0, 0, static_cast<int32_t>(lastFlushedBuffer_->width) - 1,
                static_cast<int32_t>(lastFlushedBuffer_->height) - 1);
    int16_t scaleWidth = frameRect.GetWidth();
    int16_t scaleHeight = frameRect.GetHeight();
    if (resizeMode_ == VideoResizeMode::FILL) {
        scaleWidth = GetWidth();
        scaleHeight = GetHeight();
        if (rotation_ == VideoRotation::ROTATE_90 || rotation_ == VideoRotation::ROTATE_270) {
            scaleWidth = GetHeight();
            scaleHeight = GetWidth();
        }
    } else if (resizeMode_ == VideoResizeMode::CONTAIN) {
        float scaleFactor = MATH_MIN(GetWidth() * 1.0 / frameRect.GetWidth(), GetHeight() * 1.0 / frameRect.GetHeight());
        scaleWidth = static_cast<int16_t>(scaleWidth * scaleFactor);
        scaleHeight = static_cast<int16_t>(scaleHeight * scaleFactor);
    }

    int16_t displayWidth = scaleWidth;
    int16_t displayHeight = scaleHeight;
    // 旋转交换宽高
    if (rotation_ == VideoRotation::ROTATE_90 || rotation_ == VideoRotation::ROTATE_270) {
        displayHeight = scaleWidth;
        displayWidth = scaleHeight;
    }

    int16_t xStart = GetRect().GetX() + (GetWidth() - displayWidth) / 2;    // 2: half
    int16_t yStart = GetRect().GetY() + (GetHeight() - displayHeight) / 2;  // 2: half
    Rect displayArea(xStart, yStart, xStart + displayWidth - 1, yStart + displayHeight - 1);

    Rect mask;
    if (!mask.Intersect(displayArea, invalidatedArea)) {
        return;
    }
    int16_t imgX = mask.GetX() - displayArea.GetX();
    int16_t imgY = mask.GetY() - displayArea.GetY();
    float xRelRatio = 0;
    float yRelRatio = 0;
    float wScale = 1;
    float hScale = 1;
    if (rotation_ == VideoRotation::ROTATE_0) {
        xRelRatio = float(imgX) / float(displayArea.GetWidth());
        yRelRatio = float(imgY) / float(displayArea.GetHeight());
        wScale = float(mask.GetWidth()) / float(displayArea.GetWidth());
        hScale = float(mask.GetHeight()) / float(displayArea.GetHeight());
    }
    int16_t frameX = static_cast<int16_t>(frameRect.GetWidth() * xRelRatio);
    int16_t frameY = static_cast<int16_t>(frameRect.GetHeight() * yRelRatio);
    int16_t frameW = static_cast<int16_t>(frameRect.GetWidth() * wScale);
    int16_t frameH = static_cast<int16_t>(frameRect.GetHeight() * hScale);
    BufferInfo src;
    src.rect = {frameX, frameY, static_cast<int16_t>(frameX + frameW - 1),
        static_cast<int16_t>(frameY + frameH - 1)};
    src.virAddr = reinterpret_cast<void*>(uintptr_t(lastFlushedBuffer_->addr));
#if ENABLE_GFX_ENGINES || defined(VERSION_IOT)
    src.phyAddr = reinterpret_cast<void*>(uintptr_t(lastFlushedBuffer_->addr));
#endif
    src.stride = lastFlushedBuffer_->stride;
    src.mode = colorMode;
    src.color = 0;
#ifdef VERSION_IOT
    src.width = frameRect.GetWidth();
    src.height = frameRect.GetHeight();
    src.compressMode = COMPRESS_MODE_NONE;
    src.uvOffset = surface_->GetUVOffset();
    src.rotType = strtol(surface_->GetUserData(SURFACE_USER_DATA_VIDEO_ROTATION).c_str(), nullptr, 0);
    if (src.uvOffset == 0) {
        GRAPHIC_LOGE("UILiteSurfaceView::DrawImage set uvOffset failed! uvOffset is 0!\n");
    }
#endif

    Point dstPos = {xStart, yStart};
    BlendOption blendOption;
    blendOption.opacity = opa;
#ifdef VERSION_IOT
    blendOption.mode = BLEND_SRC_OVER;
#endif
    if (rotation_ == VideoRotation::ROTATE_0) {
        BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, dstPos, src, mask, blendOption);
        return;
    }

    BufferInfo* tempDst = GetTempBufferInfo(scaleWidth, scaleHeight);
    if (tempDst == nullptr) {
        GRAPHIC_LOGE("Get temp buffer failed.");
        return;
    }

    Rect tempOutArea(0, 0, scaleWidth - 1, scaleHeight - 1);
    // 叠加缩放后的视频：配置显示区域小于或大于源区域，由VAU进行实现缩放
    BaseGfxEngine::GetInstance()->Blit(*tempDst, dstPos, src, tempOutArea, blendOption);

    Rect centerRect(xStart, yStart, tempDst->width, tempDst->height);
    CalcTransMap(blendOption.transMap, rotation_, centerRect);
    BaseGfxEngine::GetInstance()->Blit(gfxDstBuffer, dstPos, *tempDst, displayArea, blendOption);
    return;
}

void UILiteSurfaceView::DrawAYUVImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa)
{
    uint32_t width = lastFlushedBuffer_->width;
    uint32_t height = lastFlushedBuffer_->height / 2; // 2: ayuv's height should be divided by 2.

    Rect coords(GetX(), GetY(), GetX() + static_cast<int32_t>(width) - 1,
                GetY() + static_cast<int32_t>(height) - 1);
    Rect maskedArea;
    if (!maskedArea.Intersect(coords, invalidatedArea)) {
        return;
    }
    int16_t imageX = maskedArea.GetLeft() - coords.GetLeft();
    int16_t imageY = maskedArea.GetTop() - coords.GetTop();

    BufferInfo alpha;
    BufferInfo yuv;

    uint32_t uvOffset = surface_->GetUVOffset();
    uint32_t stride = lastFlushedBuffer_->stride;
    uint32_t bufferStart = lastFlushedBuffer_->addr;
    uint32_t aSize = stride * height;
    uint32_t yStart = bufferStart + aSize;
    uint32_t ySize = aSize;
    uint32_t uvStart = bufferStart + uvOffset;
    uint32_t uvRealStart = uvStart + ySize / 2;
    uint32_t uvRealOffset = uvRealStart - yStart;

    Rect imgRect = {imageX, imageY, static_cast<int16_t>(imageX + maskedArea.GetWidth() - 1),
        static_cast<int16_t>(imageY + maskedArea.GetHeight() - 1)};

    alpha.rect = imgRect;
    alpha.virAddr = reinterpret_cast<void*>(uintptr_t(bufferStart));
    alpha.phyAddr = reinterpret_cast<void*>(uintptr_t(alpha.virAddr));
    alpha.stride = stride;
    alpha.mode = A8;
    alpha.color = 0;
    alpha.width = width;
    alpha.height = height;
#ifdef VERSION_IOT
    alpha.compressMode = COMPRESS_MODE_NONE;
    alpha.rotType = 0;
#endif

    yuv.rect = imgRect;
    yuv.virAddr = reinterpret_cast<void*>((uintptr_t)yStart);
    yuv.phyAddr = reinterpret_cast<void*>(yuv.virAddr);
    yuv.stride = stride;
    yuv.mode = YCBCR420;
    yuv.color = 0;
    yuv.width = width;
    yuv.height = height;
#ifdef VERSION_IOT
    yuv.compressMode = COMPRESS_MODE_NONE;
    yuv.rotType = 0;
    yuv.uvOffset = uvRealOffset;
#endif

    BlendOption blendOption;
    blendOption.opacity = opa;
    blendOption.mode = BLEND_SRC_OVER;

    LiteMGfxEngine::GetInstance()->RenderAYUV(gfxDstBuffer, alpha, yuv, maskedArea, blendOption);
    LiteMGfxEngine::GetInstance()->SyncHwDraw();
}

void UILiteSurfaceView::SetVideoResizeMode(VideoResizeMode resizeMode)
{
    resizeMode_ = resizeMode;
}

UILiteSurfaceView::VideoResizeMode UILiteSurfaceView::GetVideoResizeMode()
{
    return resizeMode_;
}

void UILiteSurfaceView::SetRotation(VideoRotation rotation)
{
    rotation_ = rotation;
}

UILiteSurfaceView::VideoRotation UILiteSurfaceView::GetRotation()
{
    return rotation_;
}

BufferInfo* UILiteSurfaceView::GetTempBufferInfo(uint16_t width, uint16_t height)
{
    if (tempDst_ == nullptr || width != tempDst_->width || height != tempDst_->height) {
        if (tempDst_ != nullptr) {
            ClearTempBufferInfo();
        }
#ifdef VERSION_IOT
#if ENABLE_FB_COMPRESS
        uint8_t compressMode = COMPRESS_MODE_HFBC_3X;
#else
        uint8_t compressMode = COMPRESS_MODE_NONE;
#endif
        uint32_t stride = DisplayDev::GetInstance()->CalcStride(width,
            PIXEL_FMT_RGB_888, compressMode);
        uint32_t dataSize = DisplayDev::GetInstance()->CalcSize(stride, height, compressMode);
        ColorMode colorMode = RGB888;
#else
        uint32_t stride = width * 4; // 4: bpp
        uint32_t dataSize = width *  height * 4; // 4: bpp
        ColorMode colorMode = ARGB8888;
#endif
        uint8_t* addr = GrallocEngines::GetInstance()->AllocBufferBySize(dataSize);
        if (addr == nullptr) {
            GRAPHIC_LOGE("AllocBuffer failed! size = %u", dataSize);
            return nullptr;
        }
        Rect dstRect = {0, 0, width - 1, height - 1};
        tempDst_ = static_cast<BufferInfo*>(UIMalloc(sizeof(BufferInfo)));
        if (tempDst_ == nullptr) {
            GRAPHIC_LOGE("UIMalloc failed! size = %u", sizeof(BufferInfo));
            GrallocEngines::GetInstance()->FreeBuffer(addr);
            return nullptr;
        }
        memset_s(tempDst_, sizeof(BufferInfo), 0, sizeof(BufferInfo));
        tempDst_->width = width;
        tempDst_->height = height;
        tempDst_->compressMode = compressMode;
        tempDst_->mode = colorMode;
        tempDst_->stride = stride;
        tempDst_->phyAddr = static_cast<void*>(addr);
        tempDst_->virAddr = static_cast<void*>(addr);
        tempDst_->rect = dstRect;
    }
    return tempDst_;
}

void UILiteSurfaceView::ClearTempBufferInfo()
{
    if (tempDst_ != nullptr) {
        LiteMGfxEngine::GetInstance()->SyncHwDraw();
        GrallocEngines::GetInstance()->FreeBuffer(static_cast<uint8_t*>(tempDst_->phyAddr));
        UIFree(tempDst_);
        tempDst_ = nullptr;
    }
}
#endif

void UILiteSurfaceView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (surface_ == nullptr) {
        GRAPHIC_LOGE("surface is null\n");
        return;
    }

    uint8_t opa = GetMixOpaScale();
    if (opa == OPA_TRANSPARENT) {
        return;
    }

#if ENABLE_VIDEO_RGB
    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, *style_, opa);
    DrawImage(gfxDstBuffer, invalidatedArea, opa);
#endif
    if ((isStopped_ || lastFlushedBuffer_ == nullptr) && preView_ != nullptr) {
        const ImageInfo* info = preView_->GetImageInfo();
        Rect coords(GetX(), GetY(), GetX() + info->header.width - 1, GetY() + info->header.height - 1);
        preView_->DrawImage(gfxDstBuffer, coords, invalidatedArea, *style_, opaScale_);
        return;
    }

#if !ENABLE_VIDEO_RGB
#ifdef VERSION_IOT
    if (lastFlushedBuffer_ != nullptr && RootView::GetInstance()->GetSnapshotFlag()) {
        ImageInfo bitmap;
        if (GetSurfaceBufferBitmap(bitmap)) {
            BlitBitmapToFb(gfxDstBuffer, invalidatedArea, bitmap);
            ImageCacheFree(bitmap);
            return;
        }
    }
#endif

    Style style = *style_;
    if (isLayerInit_ && !isStopped_) {
        style.bgColor_ = colorKey_;
    }

    layerRect_ = GetRect();
    AdjustArea(layerRect_);
    Rect dirtyRect(0, 0, layerRect_.GetWidth() - 1, layerRect_.GetHeight() - 1);
    DisplayDev::GetInstance()->SetLayerPositon(LAYER_1, layerRect_.GetX(), layerRect_.GetY());
    DisplayDev::GetInstance()->SetLayerDirtyRegion(LAYER_1, dirtyRect);

    BaseGfxEngine::GetInstance()->DrawRect(gfxDstBuffer, GetRect(), invalidatedArea, style, OPA_OPAQUE);
#endif
}

void UILiteSurfaceView::OnBufferAvailable()
{
    lock_.Lock();
    SurfaceBuffer* back = surface_->GetBackBuf();
    bool isLastFrame = (back->format == PIXEL_FMT_BUTT);
    if (isLastFrame) {
        back->format = PIXEL_FMT_YCBCR_420_SP;
        flushCount_ = 0;
    } else {
        ++flushCount_;
    }
    lock_.Unlock();
    std::weak_ptr<LiteSurface> ws = surface_;
    if (isLastFrame) {
        isStopped_ = true;
        GraphicService::GetInstance()->PostGraphicEvent(
            [&, ws] {
                if (!ws.expired()) {
#if !ENABLE_VIDEO_RGB
                    DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, false, 0);
                    LiteMGfxEngine::GetInstance()->ClearHardwareLayer(this);
                    if (LiteMGfxEngine::GetInstance()->GetHardwareLayer() == nullptr) {
                        GraphicService::GetInstance()->EnableAsyncMode(true);
                    }
#else
                    ClearTempBufferInfo();
                    lastFlushedBuffer_ = nullptr;
#endif
                    surface_->ClearBuffers();
                    currentBuffer_ = nullptr;
                    lastFlushedBuffer_ = nullptr;
                }
            });
    } else if (isStopped_) {
        isStopped_ = false;
        LowPowerManager::GetInstance()->ExitLowPower();
        GraphicService::GetInstance()->PostGraphicEvent(
            [&, ws] {
                if (!ws.expired()) {
                    if (!GraphicService::GetInstance()->IsScreenOn()) {
                        isStopped_ = true; // Do not process video frame, when screen off and dpu suspended.
                        return;
                    }
#if !ENABLE_VIDEO_RGB
                    LiteMGfxEngine::GetInstance()->EnableAsyncMode(false);
                    LiteMGfxEngine::GetInstance()->AddHardwareLayer(this);
                    DisplayDev::GetInstance()->SetLayerColorKey(LAYER_0, true, colorKey_.full);
                    InitLayer();
#endif
                }
            });
    }

    std::weak_ptr<LiteSurface> weak = surface_;
    GraphicService::GetInstance()->PostGraphicEvent(
        [weak, this] {
            if (!weak.expired()) {
                Invalidate();
            }
        });
}

void UILiteSurfaceView::SetPreview(ImageInfo* info)
{
    if (info == nullptr) {
        return;
    }
    if (preView_ == nullptr) {
        preView_ = new Image();
    }
    if (preView_ != nullptr) {
        preView_->SetSrc(info);
    }
}

void UILiteSurfaceView::DumpImgInfo()
{
    if (preView_ != nullptr) {
        printf("view[%p]: viewType = %d\n", this, GetViewType());
        const ImageInfo* info = preView_->GetImageInfo();
        PrintImgInfo(info);
    }
}

std::string  UILiteSurfaceView::GetGuiInfo() const
{
    const int len = 256;
    char *buf = new (std::nothrow) char[len];
    if (buf == nullptr) {
        return std::string("");
    }
    memset_s(buf, len, 0, len);
    sprintf_s(buf, len, "stopped %d layerinit %d flushCnt %u lastbuf %p curbuf %p colorKey %x preView %p",
        isStopped_, isLayerInit_, flushCount_, lastFlushedBuffer_, currentBuffer_, colorKey_.full, preView_);
    std::string str(buf);
    delete []buf;
    return str;
}
}
