/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: UITestA8ImgWithColor
 * Author: Hisi Graphic Team
 * Created: 2026-01
 */

#ifndef UI_TEST_A8_IMG_WITH_COLOR_H
#define UI_TEST_A8_IMG_WITH_COLOR_H
#include "animator/easing_equation.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "gfx_utils/sys_info.h"
#include "ui_test.h"
#include "common/task.h"

namespace OHOS {
class UITestA8ImgWithColor : public UITest, public Task {
public:
    UITestA8ImgWithColor() {}
    ~UITestA8ImgWithColor() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    void Callback() override;

private:
    bool InitRes();
    void DeInitRes();
    void InitViews();
    UIScrollView *container_ = nullptr;
    UIImageView **imgs_ = nullptr;
    uint32_t imgNum_ = 0;
    uint32_t colorIndex_ = 0;
}; // class UITestA8
} // namespace OHOS
#endif // UI_TEST_A8_IMG_WITH_COLOR_H
