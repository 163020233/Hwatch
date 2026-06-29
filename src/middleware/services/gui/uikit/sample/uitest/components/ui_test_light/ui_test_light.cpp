/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: TestLight
 * Author: Hisi Graphic Team
 * Created: 2025-6
 */

#include "ui_test_light.h"
#include "common/screen.h"
#include "components/root_view.h"

#if ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
namespace OHOS {
const uint16_t CENTER_X = 233;
const uint16_t CENTER_Y = 183;
const uint16_t RADIUS = 133;
const float SPE_AMP = 1.5;
constexpr int16_t DEG_0 = 0;
constexpr int16_t DEG_30 = 30;

void UITestLight::InitButton(UILabelButton*& button, int16_t x, int16_t y, const char* text)
{
    button = new UILabelButton();
    button->SetPosition(x, y, 90, 50); // 90: width, 50: height
    button->SetText(text);
    button->SetOnClickListener(this);
    container_->Add(button);
}

void UITestLight::SetUp()
{
    container_ = new UIScrollView();
    container_->Resize(Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    container_->SetScrollBlankSize(200); // 200: blank size
    container_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Yellow().full);

    lightTestView_ = new UIImageView();
    lightTestView_->SetPosition(100, 50); // 100, 50: x, y
    lightTestView_->SetSrc(RES_PATH"planet.bin");
    lightTestView_->SetWidth(266); // 266: light view width value
    lightTestView_->SetHeight(266); // 266: light view height value
    lightTestView_->SetResizeMode(UIImageView::ImageResizeMode::SCALE_DOWN);
    lightTestView_->EnableLight(true);
    LightSpeStyle speStyle = {LightType::LIGHT_CIRCLE_RADIAL,
        CENTER_X, CENTER_Y, RADIUS, SPE_AMP, DEG_0, DEG_30, false};
    lightStyle_.speStyle = speStyle;
    lightTestView_->SetSpeLightStyle(lightStyle_.speStyle);
    lightStyle_.ambAmp = 1.0;
    lightTestView_->SetAmbLightStyle(lightStyle_.ambAmp);
    callBack_ = new LightAnimatorCallback();
    if (callBack_ == nullptr) {
        GRAPHIC_LOGE("callBack_ new fail\n");
    }
    lightAnimator_ = new Animator(callBack_, lightTestView_, 0, true);
    if (lightAnimator_ == nullptr) {
        GRAPHIC_LOGE("lightAnimator_ new fail\n");
    }
    container_->Add(lightTestView_);

    InitButton(ambAdd_, 47, 330, "Amb+"); // 47: x, 330 : y
    InitButton(ambSub_, 142, 330, "Amb-"); // 142: x, 330 : y
    InitButton(speAdd_, 237, 330, "Spe+"); // 237: x, 330 : y
    InitButton(speSub_, 332, 330, "Spe-"); // 332: x, 330 : y
    InitButton(enable_, 152, 390, "Enable"); // 152: x, 390 : y
    InitButton(lightMode_, 257, 390, "CircleR"); // 257: x, 390 : y
}

void UITestLight::TearDown()
{
    if (lightAnimator_ != nullptr) {
        lightAnimator_->Stop();
        delete lightAnimator_;
        lightAnimator_ = nullptr;
    }
    delete callBack_;
    callBack_ = nullptr;
    DeleteChildren(container_);
    container_ = nullptr;
    ambAdd_ = nullptr;
    ambSub_ = nullptr;
    speAdd_ = nullptr;
    speSub_ = nullptr;
    lightMode_ = nullptr;
    enable_ = nullptr;
    lightTestView_ = nullptr;
}

const UIView* UITestLight::GetTestView()
{
    return container_;
}

bool UITestLight::OnClick(UIView& view, const ClickEvent& event)
{
    if (&view == ambAdd_) {
        lightStyle_.ambAmp += 0.1; // 0.1: add step value
    } else if (&view == ambSub_) {
        lightStyle_.ambAmp -= 0.1; // 0.1: sub step value
    } else if (&view == speAdd_) {
        lightStyle_.speStyle.speAmp += 0.1;  // 0.1: add step value
    } else if (&view == speSub_) {
        lightStyle_.speStyle.speAmp -= 0.1; // 0.1: sub step value
    } else if (&view == enable_) {
        if (lightTestView_->IsEnableLight()) {
            if (lightStyle_.speStyle.lightMode > LightType::LIGHT_CIRCLE_CONST) {
                lightAnimator_->Stop();
            }
            lightTestView_->EnableLight(false);
            enable_->SetText("Disable");
        } else {
            lightTestView_->EnableLight(true);
            if (lightStyle_.speStyle.lightMode > LightType::LIGHT_CIRCLE_CONST) {
                lightAnimator_->Start();
            }
            enable_->SetText("Enable");
        }
    } else if (&view == lightMode_) {
        switch (lightStyle_.speStyle.lightMode) {
            case LightType::LIGHT_CIRCLE_RADIAL : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_CIRCLE_CONST;
                lightMode_->SetText("CircleC");
                break;
            }
            case LightType::LIGHT_CIRCLE_CONST : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_FAN_RADIAL;
                lightMode_->SetText("FanR");
                callBack_->SetSpeLightStyle(lightStyle_.speStyle);
                lightAnimator_->Start();
                break;
            }
            case LightType::LIGHT_FAN_RADIAL : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_FAN_CONST;
                lightMode_->SetText("FanC");
                callBack_->SetSpeLightStyle(lightStyle_.speStyle);
                break;
            }
            case LightType::LIGHT_FAN_CONST : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_FAN_SWEEP_RADIAL;
                lightMode_->SetText("FanSweepR");
                callBack_->SetSpeLightStyle(lightStyle_.speStyle);
                break;
            }
            case LightType::LIGHT_FAN_SWEEP_RADIAL : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_FAN_SWEEP_CONST;
                lightMode_->SetText("FanSweepC");
                callBack_->SetSpeLightStyle(lightStyle_.speStyle);
                break;
            }
            case LightType::LIGHT_FAN_SWEEP_CONST : {
                lightStyle_.speStyle.lightMode = LightType::LIGHT_CIRCLE_RADIAL;
                lightMode_->SetText("CircleR");
                lightAnimator_->Stop();
                break;
            }
            default :
                break;
        }
    }
    if (&view == ambAdd_ || &view == ambSub_) {
        lightTestView_->SetAmbLightStyle(lightStyle_.ambAmp);
    } else {
        lightTestView_->SetSpeLightStyle(lightStyle_.speStyle);
    }
    lightTestView_->Invalidate();
    return true;
}
}
#endif // ENABLE_VGU_ENGINE && CAPABILITY_HW_LIGHT
