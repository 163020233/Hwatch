/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestColorize
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef UI_TEST_COLORIZE_H
#define UI_TEST_COLORIZE_H

#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_post_effect_image_view.h"

namespace OHOS {
class UITestColorize : public UITest, public UIView::OnClickListener {
public:
    UITestColorize() {}
    ~UITestColorize() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text);
    UIScrollView* container_ = nullptr;
    UILabelButton* white_ = nullptr;
    UILabelButton* black_ = nullptr;
    UILabelButton* gray_ = nullptr;
    UILabelButton* silver_ = nullptr;
    UILabelButton* red_ = nullptr;
    UILabelButton* yellow_ = nullptr;
    UILabelButton* green_ = nullptr;
    UILabelButton* blue_ = nullptr;
    UILabelButton* orange_ = nullptr;
    UILabelButton* enable_ = nullptr;
    UIPostEffectImageView* postEffectImageView_ = nullptr;
};
}
#endif // UI_TEST_COLORIZE_H
