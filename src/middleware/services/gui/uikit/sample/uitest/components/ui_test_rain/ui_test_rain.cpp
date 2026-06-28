/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestRAIN
 * Author: Hisi Graphic Team
 * Created: 2025-5
 */
 
#include "ui_test_rain.h"
#include "common/screen.h"
#include "common/image_cache_manager.h"
#include "components/root_view.h"
 
namespace OHOS {

constexpr uint8_t RAIN_NUM = 12;
static const char* rainPath[RAIN_NUM] = {
    RES_PATH"rain/rain1.bin",
    RES_PATH"rain/rain2.bin",
    RES_PATH"rain/rain3.bin",
    RES_PATH"rain/rain4.bin",
    RES_PATH"rain/rain5.bin",
    RES_PATH"rain/rain6.bin",
    RES_PATH"rain/rain7.bin",
    RES_PATH"rain/rain8.bin",
    RES_PATH"rain/rain9.bin",
    RES_PATH"rain/rain10.bin",
    RES_PATH"rain/rain11.bin",
    RES_PATH"rain/rain12.bin",
};

static const ImageInfo* g_rainInfo[RAIN_NUM] = {0};

void UITestRain::LoadImg(void)
{
    for (int i = 0; i < RAIN_NUM; i++) {
        ImageInfo* img = ImageCacheManager::GetInstance().LoadSingleRes(rainPath[i]);
        if (img == nullptr) {
            GRAPHIC_LOGE("Load image failed: %s", rainPath[i]);
            ReleaseImg();
            return;
        }
        g_rainInfo[i] = img;
    }
}

void UITestRain::ReleaseImg(void)
{
    for (int i = 0; i < RAIN_NUM; i++) {
        if (g_rainInfo[i] != nullptr) {
            ImageCacheManager::GetInstance().UnloadSingleRes(rainPath[i]);
            g_rainInfo[i] = nullptr;
        }
    }
}

void UITestRain::InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text)
{
    button = new UILabelButton();
    button->SetPosition(x, y, 100, 50); // 100: width, 50: height
    button->SetText(text);
    button->SetOnClickListener(this);
    container_->Add(button);
}
 
void UITestRain::SetUp()
{
    container_ = new UIScrollView();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::White().full);

    background_ = new UIImageView();
    background_->SetSrc(RES_PATH"/rain/watch466.bin");
    background_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->Add(background_);

    LoadImg();
    rainView_ = new UIRainView();
    rainView_->SetPosition(0, 0, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    rainView_->SetBlur(5); // 5: blurRadius
    rainView_->SetRain(g_rainInfo, RAIN_NUM);
    rainView_->SetTimeOfUpdate(100); // 100: pause 100ms between two rain images
    rainView_->SetTimeOfPause(1000); // 1000: pause 1000ms between two playbacks
    rainView_->SetTouchable(true);
    rainView_->SetDraggable(true);
    rainView_->SetDragParentInstead(false);
    rainView_->Start();
    container_->Add(rainView_);

    container_->SetOnDragListener(rainView_);
    InitButton(Start_, 90, 370, "Start"); // 90: x, 370 : y
    InitButton(pause_, 200, 370, "Pause"); // 200: x, 370 : y
    InitButton(stop_, 310, 370, "Stop"); // 310: x, 370 : y
}
 
void UITestRain::TearDown()
{
    rainView_->ClearBackgroundBlur();
    DeleteChildren(container_);
    container_ = nullptr;
    pause_ = nullptr;
    Start_ = nullptr;
    stop_ = nullptr;
    rainView_ = nullptr;
    background_ = nullptr;

    ReleaseImg();
}
 
const UIView* UITestRain::GetTestView()
{
    return container_;
}
 
bool UITestRain::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == pause_) {
        if (rainView_->GetState() == Animator::START) {
            rainView_->Pause();
        } else {
            rainView_->Resume();
        }
    } else if (&view == stop_) {
        rainView_->Stop();
        rainView_->Clear();
        rainView_->SetBlur(0);
    } else if (&view == Start_) {
        rainView_->SetRain(g_rainInfo, RAIN_NUM);
        rainView_->SetBlur(5); // 5: blurRadius
        rainView_->Start();
    }

    return true;
}
}