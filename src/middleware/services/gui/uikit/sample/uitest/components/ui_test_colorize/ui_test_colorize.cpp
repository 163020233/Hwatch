/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestColorize
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */

#include "ui_test_colorize.h"
#include "common/screen.h"
#include "components/root_view.h"

namespace OHOS {

void UITestColorize::InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text)
{
    button = new UILabelButton();
    button->SetPosition(x, y, 100, 50); // 100: width, 50: height
    button->SetText(text);
    button->SetOnClickListener(this);
    container_->Add(button);
}

void UITestColorize::SetUp()
{
    container_ = new UIScrollView();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetScrollBlankSize(200); // 200: blank size
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);

    postEffectImageView_ = new UIPostEffectImageView();
    postEffectImageView_->SetPosition(100, 50, Screen::GetInstance().GetWidth(), // 100, 50: x, y
                                      Screen::GetInstance().GetHeight());
    postEffectImageView_->SetSrc(RES_PATH"A094_051_8888.bin");
    postEffectImageView_->EnableColorize(true);
    container_->Add(postEffectImageView_);

    InitButton(white_, 57, 340, "White"); // 57: x, 340 : y
    InitButton(black_, 177, 340, "Black"); // 177: x, 340 : y
    InitButton(gray_, 297, 340, "Gray"); // 297: x, 340 : y
    InitButton(silver_, 57, 285, "Silver"); // 57: x, 285 : y
    InitButton(red_, 177, 285, "Red"); // 177: x, 285 : y
    InitButton(yellow_, 297, 285, "Yellow"); // 297: x, 285 : y
    InitButton(green_, 57, 230, "Green"); // 57: x, 230 : y
    InitButton(blue_, 177, 230, "Blue"); // 177: x, 230 : y
    InitButton(orange_, 297, 230, "Orange"); // 297: x, 230 : y
    InitButton(enable_, 177, 395, "Enable"); // 177: x, 395 : y
}

void UITestColorize::TearDown()
{
    DeleteChildren(container_);
    container_ = nullptr;
    white_ = nullptr;
    black_ = nullptr;
    gray_ = nullptr;
    silver_ = nullptr;
    red_ = nullptr;
    yellow_ = nullptr;
    green_ = nullptr;
    blue_ = nullptr;
    orange_ = nullptr;
    enable_ = nullptr;
    postEffectImageView_ = nullptr;
}

const UIView* UITestColorize::GetTestView()
{
    return container_;
}

bool UITestColorize::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == white_) {
        postEffectImageView_->SetColorize(Color::White());
    } else if (&view == black_) {
        postEffectImageView_->SetColorize(Color::Black());
    } else if (&view == gray_) {
        postEffectImageView_->SetColorize(Color::Gray());
    } else if (&view == silver_) {
        postEffectImageView_->SetColorize(Color::Silver());
    } else if (&view == red_) {
        postEffectImageView_->SetColorize(Color::Red());
    } else if (&view == yellow_) {
        postEffectImageView_->SetColorize(Color::Yellow());
    } else if (&view == green_) {
        postEffectImageView_->SetColorize(Color::Green());
    } else if (&view == blue_) {
        postEffectImageView_->SetColorize(Color::Blue());
    } else if (&view == orange_) {
        postEffectImageView_->SetColorize(Color::Orange());
    } else if (&view == enable_) {
        if (postEffectImageView_->IsEnableColorize()) {
            postEffectImageView_->EnableColorize(false);
            enable_->SetText("Disable");
        } else {
            postEffectImageView_->EnableColorize(true);
            enable_->SetText("Enable");
        }
    }
    postEffectImageView_->Invalidate();
    return true;
}
}
