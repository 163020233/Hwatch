/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: UIRainView
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

/**
 * @file ui_rain_view.h
 *
 * @brief Declares a UIRainView.
 *
 */

#ifndef UI_RAIN_VIEW_H
#define UI_RAIN_VIEW_H

#include "components/ui_image_view.h"
#include "imgdecode/cache_manager.h"
#include "engines/gfx/lite_m_gfx_engine.h"
#include "animator/animator.h"

namespace OHOS {
/**
 * @brief It provide rain view.
 *
 */
class UIRainView : public UIView, public UIView::OnDragListener {
public:
    /**
     * @brief A default constructor used to create a <b>UIRainView</b> instance.
     *
     */
    UIRainView(): animator_(&rainAnimCb_, this, 0, true)
    {
        SetOpaScale(0);
        (void)memset_s(&frameImage_, sizeof(VGUImage), 0, sizeof(VGUImage));
    }

    /**
     * @brief A destructor used to delete the <b>UIRainView</b> instance.
     */
    ~UIRainView() override;
    /**
     * @brief Copy constructor of UIRainView.
     */
    UIRainView(const UIRainView&) = delete;

    /**
     * @brief Assignment function of UIRainView.
     */
    UIRainView& operator=(const UIRainView&) = delete;

    /**
    * @brief  Set rain images for rainview.
    * @param  [in]  rainInfo Indicates the pointer to the ImageInfo* array for storing the rain
    *                        image information. The memory cannot be released before this animator stops.
    * @param  [in]  rainNum    images number.
    */
    void SetRain(const ImageInfo* rainInfo[], uint8_t rainNum);

    /**
     * @brief Sets the speed for this animator.
     *
     * @param [in] timeOfUpdate Indicates the interval for updating rain images, in milliseconds.
                                The default value is <b>DEFAULT_TASK_PERIOD</b>.
     */
    void SetTimeOfUpdate(uint16_t timeOfUpdate);

    /**
     * @brief Obtains the speed of this animator.
     */
    uint16_t GetTimeOfUpdate() const;

    /**
     * @brief Sets the interval between two playbacks of an infinitely repeated animator.
     *
     * @param [in] timeOfPause Indicates the interval between two playbacks of an infinitely repeated animator, in
     *                    milliseconds. The default value is <b>0</b>, indicating that there is no interval.
     */
    void SetTimeOfPause(uint16_t timeOfPause);

    /**
     * @brief Obtains the interval between two playbacks of an infinitely repeated animator.
     */
    uint16_t GetTimeOfPause() const;

    /**
     * @brief  Start rain animation.
     */
    void Start();

    /**
     * @brief  Pause rain animation.
     */
    void Pause();

    /**
     * @brief  Resume rain animation.
     */
    void Resume();

    /**
     * @brief  Stop rain animation.
     */
    void Stop();

    /**
     * @brief  Manually clear wipe area.
     */
    void ClearWipeArea();

    /**
     * @brief  Manually clear rain images cache and wipe area, should be called after this animator stops.
     */
    void Clear();

    /**
     * @brief Get state of rain animator.
     */
    uint8_t GetState() const
    {
        return animator_.GetState();
    }

    /**
     * @brief Set the Blur radius.
     */
    void SetBlur(float radius)
    {
        blurRadius_ = radius;
    }

    /**
     * @brief Get the Blur radius.
     */
    float GetBlur() const
    {
        return blurRadius_;
    }

    /**
     * @brief Set the wipe width.
     */
    void SetWipeWidth(uint32_t width)
    {
        wipeWidth_ = width;
    }

    /**
     * @brief Get the wipe width.
     */
    uint32_t GetWipeWidth() const
    {
        return wipeWidth_;
    }

    bool OnDragStart(UIView& view, const DragEvent& event) override;
    bool OnDrag(UIView& view, const DragEvent& event) override;
    bool OnDragEnd(UIView& view, const DragEvent& event) override;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_RAIN_VIEW;
    }

private:
    class RainAnimCb : public AnimatorCallback {
    public:
        RainAnimCb() {}
        virtual ~RainAnimCb() {}
        void Callback(UIView* view) override;

        void Reset()
        {
            tickNum_ = 0;
        }
    protected:
        uint16_t tickNum_ = 0;
    };

    friend class RainAnimCb;

    RainAnimCb rainAnimCb_;
    Animator animator_;
    ImageInfo* rainInfo_ = nullptr;
    uint8_t rainNum_ = 0;
    int index_ = -1;
    uint8_t tickOfUpdate_ = 1;
    uint16_t timeOfUpdate_ = DEFAULT_TASK_PERIOD;
    uint16_t timeOfPause_ = 0;
    uint8_t tickOfPause_ = 0;

    static constexpr uint32_t DEFAULT_WIPE_WIDTH = 60;
    float blurRadius_ = 0;
    BufferInfo frameInfo_ = {{0, 0, 0, 0}};
    VGUImage frameImage_;
    Graphic::Vector<float> wipeData_;
    Graphic::Vector<uint8_t> wipeCmd_;
    bool needWipe_ = false;
    Rect wipeArea_;
    uint32_t wipeWidth_ = DEFAULT_WIPE_WIDTH;
    Path wipePath_ = {0};
    bool pathExceedLimit_ = false;   // 路径是否超出限制，超出限制时清空蒙版，停止雨滴动画

    void DrawRain(BufferInfo& dst);
    void DrawWipeArea(BufferInfo& dst);
    void UpdateWipeArea(Point point);
    bool InitFrameBuffer(const BufferInfo& srcInfo);
    void ClearRainCache();
    uint8_t GetTickByTime(uint16_t time) const;
    void HandlePathExceedLimit();
};
} // namespace OHOS
#endif // UI_RAIN_VIEW_H
/**
 * @}
 */