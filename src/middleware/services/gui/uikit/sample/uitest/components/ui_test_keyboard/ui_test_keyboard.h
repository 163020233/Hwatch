/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 */

#ifndef UI_TEST_KEYBOARD_H
#define UI_TEST_KEYBOARD_H

#include "components/ui_edit_text.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "graphic_config.h"
#include "ui_test.h"

namespace OHOS {
#if ENABLE_SOFT_KEYBOARD
class UITestKeyboard : public UITest, public UIView::OnClickListener {
public:
    UITestKeyboard() {}
    ~UITestKeyboard() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;

    void UIKitUIEditTextTestDisplay001();
    bool OnClick(UIView& view, const ClickEvent& event) override;
private:
    UIEditText* SetupEditText(const char* value, const char* placeholder, const char* viewId);

    UIScrollView* container_ = nullptr;
    UIEditText* editText2_ = nullptr;
    UILabelButton* showsBtn_ = nullptr;
};
#endif
} // namespace OHOS
#endif // UI_TEST_KEYBOARD_H