/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Lottie
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_LOTT_VIEW_H
#define UI_LOTT_VIEW_H

#include <string>
#include "lottie/lott_comp.h"
#include "animator/animator.h"
#include "gfx_utils/vector.h"
#include "gfx_utils/rect.h"
#include "gfx_utils/graphic_buffer.h"

namespace OHOS {

class UILottView : public UIView {
public:
    /**
     * @brief  Constructor of UILottView.
     */
    UILottView();

    /**
     * @brief  Destructor of UILottView.
     */
    virtual ~UILottView();

    /**
     * @brief Copy constructor of UILottView.
     */
    UILottView(const UILottView&) = delete;

    /**
     * @brief Assignment function of UILottView.
     */
    UILottView& operator=(const UILottView&) = delete;

    /**
     * @brief Set lottie source file.
     * @param file lottie source file.
     * @return Returns <b>true</b> if set file successfully; returns <b> false</b> otherwise.
     */
    bool SetSrc(std::string file, FILE *fp = nullptr, uint32_t offset = 0);

    /**
     * @brief  Start lottie animation.
     */
    void Start();

    /**
     * @brief  Pause lottie animation.
     */
    void Pause();

    /**
     * @brief  Resume lottie animation.
     */
    void Resume();

    /**
     * @brief  Stop lottie animation.
     */
    void Stop();

    /**
     * @brief Set whether lottie animation is played repeatedly.
     */
    void SetRepeat(bool repeat);

    /**
     * @brief Get whether lottie animation is played repeatedly.
     */
    bool IsRepeat() const;

    /**
     * @brief  Get lottie animation state.
     */
    uint8_t GetState() const;

    void OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea) override;

    UIViewType GetViewType() const override
    {
        return UI_LOTT_VIEW;
    }

    /**
    * @brief Set the animation play count.
    * @param count Play count, 0 indicates infinite loop.
    * @note This setting takes effect only when repeat is false.
    */
    void SetPlayCount(uint16_t count);

    /**
    * @brief Get the current animation play count.
    * @return Current play count.
    */
    uint16_t GetPlayCount() const;

    /**
    * @brief Get the current animation frame number.
    * @return Current frame number.
    */
    float GetCurrentFrame() const;

    /**
    * @brief Set reverse play.
    * @param reverse Whether to play in reverse.
    */
    void SetReverse(bool reverse);

    /**
    * @brief Get whether the current play is in reverse.
    * @return Whether to play in reverse.
    */
    bool IsReverse() const;

    class OnLottAnimStopListener : public HeapBase {
    public:
        /**
        * @brief Callback function, triggered when the animation play is stopped.
        * @param view Animation view.
        */
        virtual void OnLottAnimStop(UILottView& view) {}

        /**
        * @brief Destructor.
        */
        virtual ~OnLottAnimStopListener() {}
    };

    /**
    * @brief Set the animation stop listener.
    * @param listener Animation stop listener.
    */
    void SetOnLottAnimStopListener(OnLottAnimStopListener* listener);

    /**
    * @brief Get the current animation stop listener.
    * @return Current listener.
    */
    UILottView::OnLottAnimStopListener* GetOnLottAnimStopListener() const;

private:

    class LottAnimCb : public AnimatorCallback {
    public:
        LottAnimCb() {}
        virtual ~LottAnimCb() {}
        void Callback(UIView* view) override;
        void OnStop(UIView& view) override;
        void SetEndValue(float value)
        {
            endValue_ = value;
        }

        void BindAnimator(Animator* anim)
        {
            anim_ = anim;
        }

        void Reset()
        {
            endValue_ = 0;
            anim_ = nullptr;
        }

        void SetUpdateInterval(uint32_t interval)
        {
            updateInterval_ = interval;
        }

    private:
        float endValue_ = 0;
        uint32_t updateInterval_ = 0;
        uint32_t time_ = 0;
        Animator* anim_ = nullptr;
    };

    friend class LottAnimCb;

    bool repeat_ = false;
    LottAnimCb lottAnimCb_;
    Animator animator_;
    LottComp* lottComp_ = nullptr;
    std::string filePath_;
    LottInterpolatorFactory factory_;
    uint16_t playCount_ = 1;
    uint16_t currentPlayCount_ = 0;
    OnLottAnimStopListener* onLottAnimStopListener_ = nullptr;
    float currentFrame_ = 0.0f;
    bool reverse_ = false;
};
}
#endif
