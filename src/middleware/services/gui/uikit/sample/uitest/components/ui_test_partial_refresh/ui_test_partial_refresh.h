/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: UITestPartialRefresh
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#ifndef UI_TEST_PARTIAL_REFRESH_H
#define UI_TEST_PARTIAL_REFRESH_H

#include "ui_test.h"
#include "animator/animator.h"
#include "components/ui_view_group.h"
#include "components/ui_image_view.h"

namespace OHOS {
class RefreshAnimatorCallback : public AnimatorCallback {
public:
    explicit RefreshAnimatorCallback(UIView* partial1, UIView* partial2, UIView* partial3)
        : partial1_(partial1), partial2_(partial2), partial3_(partial3) {}
    ~RefreshAnimatorCallback() override {}
    void Callback(UIView *view) override;

    void SetAnimator(Animator* animator)
    {
        animator_ = animator;
    }
private:
    UIView* partial1_ = nullptr;
    UIView* partial2_ = nullptr;
    UIView* partial3_ = nullptr;
    Animator* animator_ = nullptr;
};

class UITestPartialRefresh : public UITest {
public:
    explicit UITestPartialRefresh() {}
    ~UITestPartialRefresh() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

private:
    RefreshAnimatorCallback *callBack_ = nullptr;
    Animator *animator_ = nullptr;
    UIViewGroup* container_ = nullptr;
    UIImageView* partial1_ = nullptr;
    UIImageView* partial2_ = nullptr;
    UIImageView* partial3_ = nullptr;
};
}
#endif
