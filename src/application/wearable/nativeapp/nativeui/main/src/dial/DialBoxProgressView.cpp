/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialBoxProgressView.cpp
 * Author: Hisi Graphic Team
 * Created: 2025-10
 */

#include "main/dial/DialBoxProgressView.h"

namespace OHOS {
DialBoxProgressView::DialBoxProgressView(void)
{
}

DialBoxProgressView::~DialBoxProgressView(void)
{
}

void DialBoxProgressView::HandleFloatData(float data)
{
    float ratio = data;
    if (ratio > 1.0f) {
        ratio = 1.0f;
    } else if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    int32_t value = static_cast<int32_t>(ratio * (GetRangeMax() - GetRangeMin()));
    if (value_ == value) {
        return;
    }
    value_ = value;
    SetValue(value_);
}

void DialBoxProgressView::OnDraw(BufferInfo& gfxDstBuffer, const Rect& invalidatedArea)
{
    if (foregroundStopCount_ != 0) {
        Rect rect = GetOrigRect();
        foregroundGradient_.startX = rect.GetX();
        foregroundGradient_.startY = rect.GetY();
        foregroundGradient_.endX = rect.GetRight();
        foregroundGradient_.endY = rect.GetBottom();
    }
    UIBoxProgress::OnDraw(gfxDstBuffer, invalidatedArea);
}
}