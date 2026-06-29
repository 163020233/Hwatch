/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: CoverRight Transition
 * Author: Hisi Graphic Team
 * Created: 2025-12
 */

#include "CoverRight.h"
#include "TransitionManager.h"
#include "animator/easing_equation.h"
#include "color.h"
#include "common/screen.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr uint16_t DURATION = 500;

REGIST_TRANSITION(TransitionType::TRANSITION_COVER_RIGHT, CoverRight, DURATION, false, false);

void CoverRight::OnTransitionStart(UIViewGroup* current, UIViewGroup* target)
{
    if (current == nullptr || target == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "CoverRight current or target is null!");
        return;
    }
    current_ = current;
    target_ = target;
    target_->SetVisible(true);
    target_->SetOpaScale(OPA_OPAQUE);
    target_->SetX(Screen::GetInstance().GetWidth());
}

void CoverRight::TransitionAlg(uint32_t time)
{
    if (target_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "CoverRight target is null!");
        return;
    }
    int16_t targetX = EasingEquation::CubicEaseInOut(
        Screen::GetInstance().GetWidth(), 0, time, duration_);
    target_->SetX(targetX);
    target_->Invalidate();
}

void CoverRight::OnTransitionEnd()
{
    if (target_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "CoverRight target is null!");
        return;
    }
    // 结束时将target置为最终状态，防止动效被打断显示异常
    target_->SetX(0);
    target_->Invalidate();
    target_ = nullptr;
}
} // namespace OHOS