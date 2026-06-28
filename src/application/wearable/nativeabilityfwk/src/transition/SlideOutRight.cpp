/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: SlideOutRight Transition
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#include "SlideOutRight.h"
#include "TransitionManager.h"
#include "animator/easing_equation.h"
#include "color.h"
#include "common/screen.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr uint16_t DURATION = 500;

REGIST_TRANSITION(TransitionType::TRANSITION_SLIDE_OUT_RIGHT, SlideOutRight, DURATION, false, false);

void SlideOutRight::OnTransitionStart(UIViewGroup* current, UIViewGroup* target)
{
    if (current == nullptr || target == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "SlideOutRight current or target is null!");
        return;
    }
    current_ = current;
    target_ = target;

    curStartX_ = current_->GetX();
    target_->SetVisible(true);
    target_->SetOpaScale(OPA_OPAQUE);
    target_->SetX(0);
    target_->Invalidate();
}

void SlideOutRight::TransitionAlg(uint32_t time)
{
    if (current_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "SlideOutRight current is null!");
        return;
    }
    int16_t curX = EasingEquation::CubicEaseInOut(
        0, Screen::GetInstance().GetWidth(), time, duration_);
    Rect rect = current_->GetRect();
    current_->SetX(curX);
    rect.Join(rect, current_->GetRect());
    current_->InvalidateRect(rect);
}

void SlideOutRight::OnTransitionEnd()
{
    if (current_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "SlideOutRight current is null!");
        return;
    }
    Rect rect = current_->GetRect();
    current_->SetX(curStartX_);
    rect.Join(rect, current_->GetRect());
    current_->InvalidateRect(rect);
    current_ = nullptr;
}
} // namespace OHOS