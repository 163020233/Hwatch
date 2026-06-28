/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: DialRainView
 * Author:
 * Create: 2025-09-16
 */

#ifndef DIAL_RAIN_VIEW_H
#define DIAL_RAIN_VIEW_H
#include "components/ui_rain_view.h"
namespace OHOS {
class DialRainView : public UIRainView {
public:
    DialRainView();
    ~DialRainView() override;
    void SetBlurDuration(int16_t duration);
    int16_t GetBlurDuration() const;
    void ResetBlur();
    void ReMeasure() override;

private:
    int16_t duration_ = 5000; // 5000: ms
    uint32_t blurStartTime_ = 0;
    bool hasStarted_ = false;
    bool hasEnded_ = false;
    float saveBlurRadius_ = 0;
};
} // namespace OHOS
#endif // DIAL_RAIN_VIEW_H
