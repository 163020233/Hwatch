/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: SlideOutRight Transition
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#ifndef SLIDE_OUT_RIGHT_H
#define SLIDE_OUT_RIGHT_H
#include "TransitionCallback.h"

namespace OHOS {
class SlideOutRight : public TransitionCallback {
public:
    SlideOutRight(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot)
        : TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~SlideOutRight() override {}
    void OnTransitionStart(UIViewGroup* current, UIViewGroup* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;

private:
    int16_t curStartX_ = 0;
    UIViewGroup* current_ = nullptr;
    UIViewGroup* target_ = nullptr;
};
} // namespace OHOS
#endif // SLIDE_OUT_RIGHT_H