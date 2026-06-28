/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestHSV
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#ifndef UI_TEST_HSV_H
#define UI_TEST_HSV_H

#include "ui_test.h"
#include "components/ui_label_button.h"
#include "components/ui_scroll_view.h"
#include "components/ui_post_effect_image_view.h"

namespace OHOS {
class UITestHSV : public UITest, public UIView::OnClickListener {
public:
    UITestHSV() {}
    ~UITestHSV() override {}
    void SetUp() override;
    void TearDown() override;
    const UIView* GetTestView() override;
    bool OnClick(UIView& view, const ClickEvent& event) override;

private:
    void InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text);
    UIScrollView* container_ = nullptr;
    UILabelButton* addHue_ = nullptr;
    UILabelButton* addSaturation_ = nullptr;
    UILabelButton* addValue_ = nullptr;
    UILabelButton* reduceHue_ = nullptr;
    UILabelButton* reduceSaturation_ = nullptr;
    UILabelButton* reduceValue_ = nullptr;
    UILabelButton* enable_ = nullptr;
    UILabelButton* rotate_ = nullptr;
    UIPostEffectImageView* postEffectImageView_ = nullptr;

    int16_t hueAdj_ = 0;
    int8_t saturationAdj_ = 0;
    int8_t valueAdj_ = 0;
};
}
#endif // UI_TEST_HSV_H