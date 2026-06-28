/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestLight
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#ifndef UI_TEST_LIGHT_H
#define UI_TEST_LIGHT_H

#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_image_view.h"
#include "sys/time.h"

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
namespace OHOS {
class LightAnimatorCallback : public AnimatorCallback {
public:
    explicit LightAnimatorCallback()
        : lightStyle_{LightType::LIGHT_CIRCLE_RADIAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false}
    {}
    ~LightAnimatorCallback() override {}

    void Callback(UIView *view) override
    {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        uint64_t second = static_cast<uint64_t>(tv.tv_sec);
        uint64_t uSecond = static_cast<uint64_t>(tv.tv_usec);
        uint16_t realMilliSecond = static_cast<uint16_t>((second % MIN_TO_HOUR) *
            SEC_TO_MILLI + uSecond / USEC_TO_MILLI);
        float lightWidth = 30;
        float startAngle = 360 * (static_cast<float>(realMilliSecond) / 60000);
        float endAngle = 360 * (static_cast<float>(realMilliSecond) / 60000) + lightWidth;
        lightStyle_.startAngle = startAngle;
        lightStyle_.endAngle = endAngle;
        view->SetSpeLightStyle(lightStyle_);
        view->Invalidate();
    }

    void SetSpeLightStyle(LightSpeStyle style)
    {
        lightStyle_ = style;
    }

private:
    static constexpr int16_t SEC_TO_MILLI = 1000;
    static constexpr int16_t USEC_TO_MILLI = 1000;
    static constexpr int16_t MIN_TO_HOUR = 60;
    static constexpr int16_t HOURLY_SYSTEM = 24;
    static constexpr int16_t INTERVAL = 30;
    LightSpeStyle lightStyle_;
};

class UITestLight : public UITest, public UIView::OnClickListener {
public:
    UITestLight() : lightStyle_{ {LightType::LIGHT_CIRCLE_RADIAL, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false}, 0.0f}
    {}
    ~UITestLight() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text);
    UIScrollView* container_ = nullptr;
    UILabelButton* ambAdd_ = nullptr;
    UILabelButton* ambSub_ = nullptr;
    UILabelButton* speSub_ = nullptr;
    UILabelButton* speAdd_ = nullptr;
    UILabelButton* lightMode_ = nullptr;
    UILabelButton* enable_ = nullptr;
    UIImageView* lightTestView_ = nullptr;
    LightStyle lightStyle_;
    LightAnimatorCallback *callBack_{nullptr};
    Animator *lightAnimator_{nullptr};
};
}
#endif // ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
#endif // UI_TEST_LIGHT_H
