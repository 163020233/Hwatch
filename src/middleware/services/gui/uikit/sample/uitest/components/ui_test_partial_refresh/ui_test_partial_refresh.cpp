/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description: UITestPartialRefresh
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#include "ui_test_partial_refresh.h"
#include "animator/easing_equation.h"
#include "gfx_utils/graphic_math.h"

namespace OHOS {
namespace {
    static const int16_t IMAGE_WIDTH = 200;
    static const int16_t IMAGE_SMALL_WIDTH = 50;
    static const int16_t IMAGE_HEIGHT = 10;
    static const float ROTATE_CENTER_Y = 5.0f;
    static uint8_t g_cbCount = 0;
}
void RefreshAnimatorCallback::Callback(UIView *view)
{
    int16_t rotate = EasingEquation::LinearEaseNone(0, CIRCLE_IN_DEGREE,
        animator_->GetRunTime() % animator_->GetTime(), animator_->GetTime());
    if (partial1_ != nullptr) {
        partial1_->Rotate(static_cast<float>(rotate), {0, ROTATE_CENTER_Y});
        partial1_->Invalidate();
    }

    if (partial3_ != nullptr) {
        partial3_->Rotate(static_cast<float>(rotate), {0, ROTATE_CENTER_Y});
        partial3_->Invalidate();
    }

    g_cbCount++;
    if (g_cbCount >= 30 && partial2_ != nullptr) { // 30:invocation frequency
        partial2_->Rotate(static_cast<float>(CIRCLE_IN_DEGREE - rotate), {0, ROTATE_CENTER_Y});
        partial2_->Invalidate();
        g_cbCount = 0;
    }
}

void UITestPartialRefresh::SetUp()
{
    if (container_ != nullptr) {
        return;
    }

    container_ = new(std::nothrow) UIViewGroup();
    if (container_ == nullptr) {
        return;
    }
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());

    partial1_ = new(std::nothrow) UIImageView();
    if (partial1_ == nullptr) {
        TearDown();
        return;
    }
    partial1_->SetPosition(116, 116, IMAGE_WIDTH, IMAGE_HEIGHT); // 116: x 116:y
    partial1_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Red().full);
    partial1_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(partial1_);

    partial2_ = new(std::nothrow) UIImageView();
    if (partial2_ == nullptr) {
        TearDown();
        return;
    }
    partial2_->SetPosition(350, 350, IMAGE_WIDTH, IMAGE_HEIGHT); // 350:x 350:y
    partial2_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Blue().full);
    partial2_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(partial2_);

    partial3_ = new(std::nothrow) UIImageView();
    if (partial3_ == nullptr) {
        TearDown();
        return;
    }
    partial3_->SetPosition(20, 300, IMAGE_SMALL_WIDTH, IMAGE_HEIGHT); // 20:x 300:y
    partial3_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);
    partial3_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    container_->Add(partial3_);

    callBack_ = new(std::nothrow) RefreshAnimatorCallback(partial1_, partial2_, partial3_);
    if (callBack_ == nullptr) {
        TearDown();
        return;
    }

    animator_ = new(std::nothrow) Animator(callBack_, nullptr, 1000, true); // 1000: animati period
    if (animator_ == nullptr) {
        TearDown();
        return;
    }
    callBack_->SetAnimator(animator_);
}

void UITestPartialRefresh::TearDown()
{
    if (container_ == nullptr) {
        return;
    }
    container_->RemoveAll();
    if (partial1_ != nullptr) {
        delete partial1_;
        partial1_ = nullptr;
    }
    if (partial2_ != nullptr) {
        delete partial2_;
        partial2_ = nullptr;
    }
    if (partial3_ != nullptr) {
        delete partial3_;
        partial3_ = nullptr;
    }
    delete container_;
    container_ = nullptr;

    if (animator_ != nullptr) {
        animator_->Stop();
        delete animator_;
        animator_ = nullptr;
    }
    if (callBack_ != nullptr) {
        delete callBack_;
        callBack_ = nullptr;
    }
}

const UIView* UITestPartialRefresh::GetTestView()
{
    if (animator_ != nullptr) {
        animator_->Start();
    }
    return container_;
}
}
