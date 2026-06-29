/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialRainView
 * Author:
 * Create: 2025-09-16
 */

#include "hal_tick.h"
#include "main/dial/DialRainView.h"

namespace OHOS {
DialRainView::DialRainView()
{
}

DialRainView::~DialRainView()
{
}

void DialRainView::SetBlurDuration(int16_t duration)
{
    if (duration < 0) {
        GRAPHIC_LOGE("duration is wrong, it should be more than 0, set to default 0");
        duration_ = 0;
        return;
    }
    duration_ = duration;
}

int16_t DialRainView::GetBlurDuration() const
{
    return duration_;
}

void DialRainView::ResetBlur()
{
    // reset blur parameters
    hasStarted_ = false;
    hasEnded_ = false;
    SetDraggable(true);
    SetOnDragListener(this);
    if (FloatEqual(GetBlur(), 0)) {
        SetBlur(saveBlurRadius_);
    }
}

void DialRainView::ReMeasure()
{
    // After the drawing time exceeds duration_, the blur should disappear.
    if (hasEnded_) {
        return;
    }
    if (!hasStarted_) {
        blurStartTime_ = HALTick::GetInstance().GetTime();
        hasStarted_ = true;
        Start();
    } else if (HALTick::GetInstance().GetElapseTime(blurStartTime_) > uint32_t(duration_)) {
        saveBlurRadius_ = GetBlur();
        SetBlur(0);
        SetDraggable(false);
        SetOnDragListener(nullptr);
        ClearWipeArea();
        hasEnded_ = true;
        Stop();
        SetVisible(false);
    }
}
} // namespace OHOS