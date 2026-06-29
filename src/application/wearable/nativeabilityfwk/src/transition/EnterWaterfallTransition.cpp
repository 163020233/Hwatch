/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: EnterWaterfallTransition
 * Author:
 * Create: 2024-04
 */


#include "EnterWaterfallTransition.h"
#include "animator/easing_equation.h"
#include "components/root_view.h"
#include "TransitionManager.h"
#include "wearable_log.h"
#include "TransitionUtils.h"
#include "common/screen.h"

namespace OHOS {
REGIST_TRANSITION(TransitionType::TRANSITION_ENTER_WATERFALL, EnterWaterfallTransition, 1000, true, false);
static const float SCALE_INIT = 1.5f;
static const float SCALE_END = 1.0f;
static const float CUR_ANIM_RATIO = 0.2f;
static const float TAR_ANIM_RATIO = 1.0f - CUR_ANIM_RATIO;
static const float BLUR_ANIM_RATIO = 0.5f;
static const int16_t EASE_END = 1000;
static bool enableBlurEffect = true;

void EnterWaterfallTransition::OnTransitionStart(UIImageView* current, UIViewGroup* target)
{
    current_ = current;
    target_ = dynamic_cast<UIWaterfallList*>(TransitionUtils::GetViewAndTransparentizePath(target, UI_WATERFALL_LIST));
    if ((target_ == nullptr) || (current_ == nullptr)) {
        return;
    }

    target_->SetEnterAnimFlag(true);

    /* 处理高斯效果 */
    if (!enableBlurEffect) {
        return;
    }
    UIView* blurView = new UIView();
    if (blurView == nullptr) {
        GRAPHIC_LOGE("new blurView failed!");
        return;
    }
    blurView->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    RootView::GetInstance()->GetAppView()->Add(blurView);
    blurView->SetFullScreenBlur(true);
    blurView->SetStyle(STYLE_BACKGROUND_OPA, 0);
    blurView->SetOpaScale(0);
    blurView->SetViewId("blurView");
}

void EnterWaterfallTransition::TransitionAlg(uint32_t time)
{
    if ((current_ == nullptr) || (target_ == nullptr)) {
        return;
    }
    uint32_t curAnimEnd = static_cast<uint32_t>(duration_ * CUR_ANIM_RATIO);
    uint32_t tarDuration = static_cast<uint32_t>(duration_ * TAR_ANIM_RATIO);
    float curOpaRatio = 0.0f;
    float tarOpaRatio = 0.0f;
    if (time < curAnimEnd) {
        int16_t curValue = EasingEquation::SineEaseOut(EASE_END, 0, time, curAnimEnd);
        curOpaRatio = (curValue == 0) ? 0.0f : curValue / static_cast<float>(EASE_END);
    } else {
        int16_t tarValue = EasingEquation::QuadEaseOut(0, EASE_END, time - curAnimEnd, tarDuration);
        tarOpaRatio = (tarValue == 0) ? 0.0f : tarValue / static_cast<float>(EASE_END);
    }
    uint8_t curOpa = static_cast<uint8_t>(curOpaRatio * OPA_OPAQUE);
    uint8_t tarOpa = static_cast<uint8_t>(tarOpaRatio * OPA_OPAQUE);
    current_->SetOpaScale(curOpa);
    target_->SetOpaScale(tarOpa);

    float timeRatio = (2 * time > duration_) ? 1.0f : 2.0f * time / duration_;
    float curScale = timeRatio * (SCALE_END - SCALE_INIT) + SCALE_INIT;

    target_->EnterMove(time - curAnimEnd, tarDuration);

    /* 处理高斯效果 */
    if (!enableBlurEffect) {
        return;
    }
    if (RootView::GetInstance() == nullptr || RootView::GetInstance()->GetAppView() == nullptr) {
        return;
    }
    UIView* blurView = RootView::GetInstance()->GetAppView()->GetChildrenTail();
    if (blurView == nullptr || blurView->GetViewId() == nullptr ||
        strcmp(blurView->GetViewId(), "blurView") != 0) {
        return;
    }
    int16_t blurValue = 0;
    uint32_t blurAnimEnd = static_cast<uint32_t>(duration_ * BLUR_ANIM_RATIO);
    if (time < curAnimEnd) {
        blurValue = EasingEquation::CubicEaseOut(0, EASE_END, time, curAnimEnd);
    } else if (time < blurAnimEnd) {
        blurValue = EasingEquation::SineEaseOut(EASE_END, 0, time - curAnimEnd, blurAnimEnd - curAnimEnd);
    }

    if (blurValue > 0) {
        float blur = 32.0f * blurValue / EASE_END; // 32: blur max
        blurView->SetBackgroundBlur(blur);
    } else {
        blurView->ClearBackgroundBlur();
    }
}

void EnterWaterfallTransition::OnTransitionEnd()
{
    if (current_ != nullptr) {
        current_->SetOpaScale(0); // 0, transparency
        current_ = nullptr;
    }
    if (target_ != nullptr) {
        target_->Scale(SCALE_END);
        target_->EnterMove(duration_, duration_);
        target_->SetEnterAnimFlag(false);
        target_ = nullptr;
    }

    /* 处理高斯效果 */
    if (!enableBlurEffect) {
        return;
    }
    if (RootView::GetInstance() == nullptr || RootView::GetInstance()->GetAppView() == nullptr) {
        return;
    }
    UIView* blurView = RootView::GetInstance()->GetAppView()->GetChildrenTail();
    if (blurView == nullptr || blurView->GetViewId() == nullptr ||
        strcmp(blurView->GetViewId(), "blurView") != 0) {
        return;
    }

    blurView->ClearBackgroundBlur();
    RootView::GetInstance()->GetAppView()->Remove(blurView);
    delete blurView;
}
}