/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: CoverRight Transition
 * Author: Hisi Graphic Team
 * Created: 2025-12
 */

#ifndef COVER_RIGHT_H
#define COVER_RIGHT_H
#include "TransitionCallback.h"

namespace OHOS {
class CoverRight : public TransitionCallback {
public:
    CoverRight(uint32_t time, bool enableCurrentSnapshot, bool enableTargetSnapshot)
        : TransitionCallback(time, enableCurrentSnapshot, enableTargetSnapshot) {}
    ~CoverRight() override {}
    void OnTransitionStart(UIViewGroup* current, UIViewGroup* target) override;
    void TransitionAlg(uint32_t time) override;
    void OnTransitionEnd() override;
private:
    UIViewGroup* current_ = nullptr;
    UIViewGroup* target_ = nullptr;
};
} // namespace OHOS
#endif // COVER_RIGHT_H