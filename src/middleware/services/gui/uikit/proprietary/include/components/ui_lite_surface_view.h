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

#ifndef GRAPHIC_LITE_UI_LITE_SURFACE_VIEW_H
#define GRAPHIC_LITE_UI_LITE_SURFACE_VIEW_H

/**
 * @file ui_lite_surface_view.h
 * @brief Declares the surface view that interacts with the multimedia to achieve camera preview and video playback.
 * @since 1.0
 * @version 1.0
 */

#include <memory>
#include "common/ihardware_view.h"
#include "common/lite_surface.h"
#include "common/image.h"
#include "components/ui_view.h"
#include "ibuffer_consumer_listener.h"
#include "graphic_mutex.h"

namespace OHOS {
/**
 * @brief Represents a surface view that interacts with the multimedia to achieve camera preview and video playback.
 *
 * @since 1.0
 * @version 1.0
 */

class UILiteSurfaceView : public UIView, public IHardwareView, public IBufferConsumerListener {
public:
    /**
     * @brief Called to notify a consumer that a buffer is available for consumption.
     *
     * @since 1.0
     * @version 1.0
     */
    void OnBufferAvailable() override;

    /**
     * @brief Flush buffer to display HAL layer.
     *
     * @since 1.0
     * @version 1.0
     */
    void Flush() override;

    /**
     * @brief Called after a frame flushed.
     *
     * @since 1.0
     * @version 1.0
     */
    void PostFlush() override;

    /**
     * @brief A constructor used to create a <b>UILiteSurfaceView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    UILiteSurfaceView();

    /**
     * @brief A destructor used to delete the <b>UILiteSurfaceView</b> instance.
     * @since 1.0
     * @version 1.0
     */
    ~UILiteSurfaceView() override;

    /**
     * @brief Obtains the surface, which should be used together with the camera and video modules.
     * @return Returns the surface.
     * @since 1.0
     * @version 1.0
     */
    Surface* GetSurface() const;

    /**
     * @brief Sets the position and size for this view.
     * @param x Indicates the x-coordinate to set.
     * @param y Indicates the y-coordinate to set.
     * @param width Indicates the width to set.
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetPosition(int16_t x, int16_t y, int16_t width, int16_t height) override;

    /**
     * @brief Adjusts the size of this view.
     * @param width Indicates the new width.
     * @param height Indicates the new height.
     * @since 1.0
     * @version 1.0
     */
    void Resize(int16_t width, int16_t height) override;

    /**
     * @brief Sets the width for this view.
     * @param width Indicates the width to set.
     * @since 1.0
     * @version 1.0
     */
    void SetWidth(int16_t width) override;

    /**
     * @brief Sets the height for this view.
     * @param height Indicates the height to set.
     * @since 1.0
     * @version 1.0
     */
    void SetHeight(int16_t height) override;

#if !ENABLE_VIDEO_RGB
    /**
     * @brief Set the colorkey for this view.
     * @param color Indicates the colorkey.
     * @since 1.0
     * @version 1.0
     */
    void SetSurfaceColorkey(ColorType color);
#else
    enum class VideoResizeMode {
        FILL,           // 视频的内容完全填充容器，不考虑宽高比。内容将被拉伸以适应容器的尺寸，可能会导致内容变形。
        CONTAIN,        // 视频的内容在保持其宽高比的情况下完全适应容器。内容将被缩放以适应容器的尺寸，但不会被裁剪。
        MODE_MAX
    };

    enum class VideoRotation {
        ROTATE_0,
        ROTATE_90,
        ROTATE_180,
        ROTATE_270,
        ROTATE_MAX
    };

    /**
     * @brief Sets the video scaling and aspect ratio handling mode.
     * @param resizeMode The desired scaling strategy from the VideoResizeMode enum.
     */
    void SetVideoResizeMode(VideoResizeMode resizeMode);

    /**
     * @brief Gets the video scaling and aspect ratio handling mode.
     */
    VideoResizeMode GetVideoResizeMode();

    /**
     * @brief Sets the video rotation.
     * @param rotation The rotation of the video.
     */
    void SetRotation(VideoRotation rotation);

    /**
     * @brief Gets the video rotation.
     */
    VideoRotation GetRotation();
#endif

    /**
     * @brief Called when this view is drawn.
     * @param invalidatedArea Indicates the area to draw.
     * @since 1.0
     * @version 1.0
     */
    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    /**
     * @brief Obtains the view type.
     * @return Returns the view type.
     * @since 1.0
     * @version 1.0
     */
    UIViewType GetViewType() const override
    {
        return UI_LITE_SURFACE_VIEW;
    }

    /**
     * @brief Set preview image info.
     * @since 1.0
     * @version 1.0
     */
    void SetPreview(ImageInfo* info);

    void DumpImgInfo() override;

    std::string GetGuiInfo() const override;

private:
    UILiteSurfaceView(const UILiteSurfaceView&) = delete;
    UILiteSurfaceView &operator=(const UILiteSurfaceView&) = delete;
    void InitLayer();
    bool isStopped_ = true;
    bool isLayerInit_ : 1;
    uint8_t flushCount_;
    std::shared_ptr<LiteSurface> surface_ = nullptr;
    SurfaceBuffer* lastFlushedBuffer_ = nullptr;
    SurfaceBuffer* currentBuffer_ = nullptr;
    GraphicMutex lock_;
    ColorType colorKey_ = Color::White();
    Rect layerRect_;
    Image* preView_ = nullptr;
#if !ENABLE_VIDEO_RGB
#ifdef VERSION_IOT
    bool GetSurfaceBufferBitmap(ImageInfo& bitmap);
    void BlitBitmapToFb(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, ImageInfo& bitmap);
#endif
    void AdjustArea(Rect& rect);
#else
    VideoRotation rotation_ = VideoRotation::ROTATE_0;
    VideoResizeMode resizeMode_ = VideoResizeMode::FILL;
    BufferInfo* tempDst_ = nullptr;
    BufferInfo* GetTempBufferInfo(uint16_t width, uint16_t height);
    void ClearTempBufferInfo();
    bool Convert2ColorMode(const PixelFormat& colorFmt, ColorMode& dstColorMode);
    void DrawImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa);
    void DrawYUVImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa);
    void DrawAYUVImage(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea, OpacityType opa);
    class SurfaceTask;
    SurfaceTask* surfaceTask_;
#endif
};
} // namespace OHOS
#endif // GRAPHIC_LITE_UI_LITE_SURFACE_VIEW_H
/**
 * @}
 */
