/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestVideoAYUV
 * Author: Hisi Graphic Team
 * Created: 2025-11
 */

#ifndef UI_TEST_VIDEO_AYUV_H
#define UI_TEST_VIDEO_AYUV_H

#include "ui_test.h"
#include "common/screen.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_lite_surface_view.h"
#include "video_player_service.h"

namespace OHOS {
class UITestVideoAYUV : public UITest, public UIView::OnClickListener {
public:
    UITestVideoAYUV() {}
    ~UITestVideoAYUV() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    bool InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text);
    UIScrollView *container_ = nullptr;
    UILabelButton *button1_ = nullptr; // Refer to start and stop
    UILabelButton *button2_ = nullptr; // Refer to pause and resume
    UILiteSurfaceView *surfaceView_ = nullptr;
    VideoPlayerService *videoPlay_ = nullptr;
};
}
#endif // UI_TEST_VIDEO_AYUV_H
