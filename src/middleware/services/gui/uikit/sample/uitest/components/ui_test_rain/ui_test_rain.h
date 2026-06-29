/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestRAIN
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */
 
#ifndef UI_TEST_RAIN_H
#define UI_TEST_RAIN_H
 
#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_rain_view.h"
 
namespace OHOS {
class UITestRain : public UITest, public UIView::OnClickListener {
public:
    UITestRain() {}
    ~UITestRain() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;
 
private:
    void InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text);
    void LoadImg(void);
    void ReleaseImg(void);
    UIScrollView* container_ = nullptr;
    UIRainView* rainView_ = nullptr;
    UIImageView* background_ = nullptr;
    UILabelButton* Start_ = nullptr;
    UILabelButton* pause_ = nullptr;
    UILabelButton* stop_ = nullptr;
};
}
#endif // UI_TEST_RAIN_H