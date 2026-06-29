/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 */

#ifndef UI_TEST_RUNNING_BALL_H
#define UI_TEST_RUNNING_BALL_H

#include "ui_test.h"
#include "components/ui_scroll_view.h"
#include "components/ui_canvas_ext.h"
#include "components/ui_label_button.h"
#include "components/root_view.h"
#include "hal_tick.h"

namespace OHOS {
#if ENABLE_VGU_ENGINE && CAPABILITY_HWDRAW_PATH
class BallAnimatorCallback : public AnimatorCallback {
public:
    explicit BallAnimatorCallback(UICanvasExt *canvas) : canvas_(canvas) {}
    ~BallAnimatorCallback() override {}
    void Callback(UIView *view) override;
    void NotifyStart();

private:
    void UpdateBall(uint16_t curTime);

    UICanvasExt *canvas_ = nullptr;
    bool runningFlag_ = false;
    int16_t startPos_ = 0;
    int16_t endPos_ = 0;
    uint16_t curTime_ = 0;
    uint16_t durationTime_ = 2000; // 2000: means 2000ms
    uint32_t startTimestamp_ = 0;
};

class UITestRunningBall : public UITest, public UIView::OnClickListener {
public:
    UITestRunningBall() {}
    ~UITestRunningBall() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void CaseInit();
    BallAnimatorCallback *callBack_ = nullptr;
    Animator *ballAnimator_ = nullptr;
    UICanvasExt* canvas_ = nullptr;
    UILabelButton* restart_ = nullptr;
    UIViewGroup* container_ = nullptr;
};
#endif
}
#endif
