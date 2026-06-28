/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 */

#ifndef UI_TEST_VIDEO_PLAY_H
#define UI_TEST_VIDEO_PLAY_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/root_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_lite_surface_view.h"
#include "animator/animator.h"
#include "video_player_service.h"

namespace OHOS {
class VideoAnimatorCallback : public AnimatorCallback {
public:
    explicit VideoAnimatorCallback(UILabel *label) : label_(label) {}
    ~VideoAnimatorCallback() override {}
    void Callback(UIView *view) override;

private:
    UILabel* label_ = nullptr;
};

class UITestVideoPlay : public UITest, public UIView::OnClickListener {
public:
    UITestVideoPlay() {}
    ~UITestVideoPlay() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    ColorType colorKey_;
    UIScrollView *container_ = nullptr;
    UILabel *label_ = nullptr;
    UILabelButton *button1_ = nullptr; // Refer to start and stop
    UILabelButton *button2_ = nullptr; // Refer to pause and resume
#if ENABLE_VIDEO_RGB
    UILabelButton *buttonRotate_ = nullptr; // Refer to rotate
    UILabelButton *buttonResizeMode_ = nullptr; // Refer to switch scale mode
    UILabelButton *buttonResize_ = nullptr; // Refer to reszie surfaceView
    UILiteSurfaceView::VideoResizeMode resizeMode_ = UILiteSurfaceView::VideoResizeMode::FILL;
    UILiteSurfaceView::VideoRotation rotation_ = UILiteSurfaceView::VideoRotation::ROTATE_0;
#endif
    UILiteSurfaceView *surfaceView_ = nullptr;
    VideoAnimatorCallback *callBack_ = nullptr;
    Animator *animator_ = nullptr;
    VideoPlayerService *videoPlay_ = nullptr;
    void InitViews();
    void AddToContainer();
    void CreateSurfaceView(int index);
};
}
#endif // UI_TEST_VIDEO_PLAY_H
