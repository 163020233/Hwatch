/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestHSV
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#include "ui_test_hsv.h"
#include "common/screen.h"
#include "components/root_view.h"

namespace OHOS {
const static uint8_t ADJUST_STEP = 10;

void UITestHSV::InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text)
{
    button = new UILabelButton();
    button->SetPosition(x, y, 100, 50); // 100: width, 50: height
    button->SetText(text);
    button->SetOnClickListener(this);
    container_->Add(button);
}

void UITestHSV::SetUp()
{
    container_ = new UIScrollView();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetScrollBlankSize(200); // 200: blank size
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);

    postEffectImageView_ = new UIPostEffectImageView();
    postEffectImageView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(),
                                      Screen::GetInstance().GetHeight());
    postEffectImageView_->SetSrc(RES_PATH"/hsv/car.bin");
    postEffectImageView_->SetHSVMask(RES_PATH"/hsv/car_mask.bin");
    container_->Add(postEffectImageView_);

    InitButton(addHue_, 57, 340, "Hue+"); // 57: x, 340 : y
    InitButton(addSaturation_, 177, 340, "Saturation+"); // 177: x, 340 : y
    InitButton(addValue_, 297, 340, "Value+"); // 297: x, 340 : y
    InitButton(reduceHue_, 57, 285, "Hue-"); // 57: x, 285 : y
    InitButton(reduceSaturation_, 177, 285, "Saturation-"); // 177: x, 285 : y
    InitButton(reduceValue_, 297, 285, "Value-"); // 297: x, 285 : y
    InitButton(enable_, 177, 395, "Enable"); // 177: x, 395 : y
    InitButton(rotate_, 297, 395, "Rotate"); // 297: x, 395 : y
}

void UITestHSV::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    addHue_ = nullptr;
    addSaturation_ = nullptr;
    addValue_ = nullptr;
    reduceHue_ = nullptr;
    reduceSaturation_ = nullptr;
    reduceValue_ = nullptr;
    enable_ = nullptr;
    rotate_ = nullptr;
    postEffectImageView_ = nullptr;
    hueAdj_ = 0;
    saturationAdj_ = 0;
    valueAdj_ = 0;
}

const UIView* UITestHSV::GetTestView()
{
    return container_;
}

bool UITestHSV::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == addHue_) {
        hueAdj_ += ADJUST_STEP;
    } else if (&view == addSaturation_) {
        saturationAdj_ += ADJUST_STEP;
    } else if (&view == addValue_) {
        valueAdj_ += ADJUST_STEP;
    } else if (&view == reduceHue_) {
        hueAdj_ -= ADJUST_STEP;
    } else if (&view == reduceSaturation_) {
        saturationAdj_ -= ADJUST_STEP;
    } else if (&view == reduceValue_) {
        valueAdj_ -= ADJUST_STEP;
    } else if (&view == rotate_) {
        postEffectImageView_->Rotate(30, Vector2<float>(0, 0)); // 30: rotate angle
    } else if (&view == enable_) {
        if (postEffectImageView_->IsEnableHSVAdjust()) {
            postEffectImageView_->EnableHSVAdjust(false);
            enable_->SetText("Disable");
        } else {
            postEffectImageView_->EnableHSVAdjust(true);
            enable_->SetText("Enable");
        };
    }

    postEffectImageView_->SetHSVInfo(hueAdj_, saturationAdj_, valueAdj_, 90); // 90: car mask threshold
    postEffectImageView_->Invalidate();
    return true;
}
}