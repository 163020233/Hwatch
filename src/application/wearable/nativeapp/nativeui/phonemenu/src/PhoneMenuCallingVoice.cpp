/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PhoneMenuCallingVoice
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "UiConfig.h"
#include "AppViewIDs.h"
#include "ui_resource_settings.h"
#include "main/LoadImg.h"
#include "ui_resource_phonemenu.h"
#include "ui_view_ids_phonemenu.h"
#include "ui_resource_pulldown.h"
#include "dock/focus_manager.h"
#include "phoneservice/PhoneAudio.h"
#include "phoneservice/PhoneService.h"
#include "phonemenu/PhoneMenuCallingPage.h"
#include "phonemenu/PhoneMenuCallingVoice.h"
namespace OHOS {

static constexpr uint16_t TITLE_LABEL_Y = 30;
static constexpr uint16_t TITLE_LABEL_HEIGHT = 36;
static constexpr uint16_t TITLE_LABEL_FRONT_SIZE = 30;
static constexpr uint16_t VOICE_LABEL_Y = 70;
static constexpr uint16_t VOICE_LABEL_HEIGHT = 30;
static constexpr uint16_t VOICE_LABEL_FRONT_SIZE = 24;
static constexpr uint16_t SUBTRACTION_IMG_SIZE = 64;
static constexpr uint16_t SUBTRACTION_IMG_X = 26;
static constexpr uint16_t ADDITION_IMG_OFFSET = 20;
static constexpr uint16_t SLIDER_WIDTH = 264;
static constexpr uint16_t SLIDER_HEIGHT = 124;
static constexpr uint16_t HALF = 2;
static constexpr uint16_t VOICE_MIN = 10;
static constexpr uint16_t VOICE_MAX = 150;
static constexpr uint16_t VOICE_STEP = 10;
static constexpr uint16_t NINETY_NINE = 99;
static constexpr uint16_t FOURTEEN = 14;
static constexpr uint16_t HFP_VOLUME_MAX = 15;
static constexpr uint16_t HFP_VOLUME_MIN = 1;
static constexpr const char *SLIDER = "slider";

void PhoneMenuCallingVoice::OnCreateView(void* data)
{
    InitVoiceFragment();
}

void PhoneMenuCallingVoice::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (titleLabel_ != nullptr) {
        delete titleLabel_;
        titleLabel_ = nullptr;
    }
    if (voiceLabel_ != nullptr) {
        delete voiceLabel_;
        voiceLabel_ = nullptr;
    }
    if (additionImg_ != nullptr) {
        delete additionImg_;
        additionImg_ = nullptr;
    }
    if (subtractionImg_ != nullptr) {
        delete subtractionImg_;
        subtractionImg_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
    if (slider_ != nullptr) {
        delete slider_;
        slider_ = nullptr;
    }
}

void PhoneMenuCallingVoice::InitVoiceFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);

    titleLabel_ = new UILabel();
    if (titleLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitVoiceFragment titleLabel_ new fail");
        return;
    }
    titleLabel_->SetPosition(0, TITLE_LABEL_Y, HORIZONTAL_RESOLUTION, TITLE_LABEL_HEIGHT);
    titleLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    titleLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TITLE_LABEL_FRONT_SIZE);
    titleLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    titleLabel_->SetText("通话音量");

    voiceLabel_ = new UILabel();
    if (voiceLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitVoiceFragment voiceLabel_ new fail");
        return;
    }
    voiceLabel_->SetPosition(0, VOICE_LABEL_Y, HORIZONTAL_RESOLUTION, VOICE_LABEL_HEIGHT);
    voiceLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    voiceLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, VOICE_LABEL_FRONT_SIZE);
    voiceLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);

    subtractionImg_ = new UIImageView();
    if (subtractionImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitVoiceFragment subtractionImg_ new fail");
        return;
    }

    int posY = (VERTICAL_RESOLUTION  - SUBTRACTION_IMG_SIZE) / HALF;
    subtractionImg_->SetPosition(SUBTRACTION_IMG_X, posY);
    subtractionImg_->SetViewId(UIVIEW_PHONEMENU_UIIMAGEVIEW_SUBTRACTION_BUTTON);
    subtractionImg_->SetTouchable(true);
    subtractionImg_->SetOnClickListener(this);
    LOADIMG::LoadImageViewImg(subtractionImg_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_VOICE_SUBTRACTION);

    additionImg_ = new UIImageView();
    if (additionImg_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitVoiceFragment additionImg_ new fail");
        return;
    }
    int posX = HORIZONTAL_RESOLUTION - ADDITION_IMG_OFFSET - SUBTRACTION_IMG_SIZE;
    additionImg_->SetPosition(posX, posY);
    additionImg_->SetViewId(UIVIEW_PHONEMENU_UIIMAGEVIEW_ADDITION_BUTTON);
    additionImg_->SetTouchable(true);
    additionImg_->SetOnClickListener(this);
    LOADIMG::LoadImageViewImg(additionImg_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_VOICE_ADDITION);

    confirmButton_ = new UIImageView();
    if (confirmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitVoiceFragment confirmButton_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(confirmButton_, PHONEMENU_IMAGE, IMAGE_PHONEMENU_OK);
    posX = (HORIZONTAL_RESOLUTION - confirmButton_->GetWidth()) / HALF;
    posY = VERTICAL_RESOLUTION - confirmButton_->GetHeight();
    confirmButton_->SetPosition(posX, posY);
    confirmButton_->SetViewId(UIVIEW_PHONEMENU_UIIMAGEVIEW_CONFIRM_BUTTON);
    confirmButton_->SetTouchable(true);
    confirmButton_->SetOnClickListener(this);

    slider_ = new UISlider();
    if (slider_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownSlider slider_ new fail");
        return;
    }
    FocusManager::GetInstance()->RequestFocus(slider_);
    posX = (HORIZONTAL_RESOLUTION - SLIDER_WIDTH) / HALF;
    posY = (VERTICAL_RESOLUTION - SLIDER_HEIGHT) / HALF;
    slider_->SetPosition(posX, posY, SLIDER_WIDTH, SLIDER_HEIGHT);
    slider_->SetRange(VOICE_MAX, 0);

    slider_->SetSliderColor(Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0x66),
                            Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0xFF));
    slider_->SetSliderRadius(SLIDER_HEIGHT / HALF, SLIDER_HEIGHT / HALF);
    slider_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    slider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
    slider_->SetSliderEventListener(this);
    slider_->SetOnClickListener(this);
    slider_->SetOnDragListener(this);
    slider_->LayoutCenterOfParent();
    slider_->SetViewId(SLIDER);
    RefreshFragment();

    fragmentView_.Add(titleLabel_);
    fragmentView_.Add(voiceLabel_);
    fragmentView_.Add(slider_);
    fragmentView_.Add(additionImg_);
    fragmentView_.Add(subtractionImg_);
    fragmentView_.Add(confirmButton_);
}

void PhoneMenuCallingVoice::RefreshFragment()
{
    int volume = static_cast<int>(GetCallCurrVolume());
    slider_->SetValue(volume * VOICE_STEP);
    if (volume < HFP_VOLUME_MIN) {
        volume = HFP_VOLUME_MIN;
    } else if (volume > HFP_VOLUME_MAX) {
        volume = HFP_VOLUME_MAX;
    }
    volume = (NINETY_NINE * (volume - HFP_VOLUME_MIN)) / FOURTEEN + 1;
    std::string voiceStr = std::to_string(volume);
    voiceLabel_->SetText(voiceStr.c_str());
}

bool PhoneMenuCallingVoice::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), UIVIEW_PHONEMENU_UIIMAGEVIEW_CONFIRM_BUTTON) == 0) {
        PhoneMenuCallingPage::GetInstance()->HideVoiceFragment();
    } else if (strcmp(view.GetViewId(), UIVIEW_PHONEMENU_UIIMAGEVIEW_ADDITION_BUTTON) == 0) {
        int value = slider_->GetValue();
        value = value + VOICE_STEP;
        if (value > VOICE_MAX) {
            value = VOICE_MAX;
        }
        VolumeSet(value);
    } else if (strcmp(view.GetViewId(), UIVIEW_PHONEMENU_UIIMAGEVIEW_SUBTRACTION_BUTTON)== 0) {
        int value = slider_->GetValue();
        value = value - VOICE_STEP;
        // 通话声音无法完全关闭
        if (value < VOICE_STEP) {
            value = VOICE_STEP;
        }
        VolumeSet(value);
    }
    return true;
}

void PhoneMenuCallingVoice::VolumeSet(int16_t volume)
{
    if (volume < VOICE_STEP) {
        volume = VOICE_MIN;
    } else if (volume > VOICE_MAX) {
        volume = VOICE_MAX;
    }
    volume = volume / VOICE_STEP;
    slider_->SetValue(volume * VOICE_STEP);
    int valueText = (NINETY_NINE * (volume - HFP_VOLUME_MIN)) / FOURTEEN + 1;
    std::string voiceStr = std::to_string(valueText);
    voiceLabel_->SetText(voiceStr.c_str());
    voiceLabel_->Invalidate();
    char charVolume = volume;
    SetSpeakerVolume(charVolume);
}

void PhoneMenuCallingVoice::OnChange(int32_t progress)
{
    VolumeSet(progress);
}

void PhoneMenuCallingVoice::OnRelease(int32_t progress)
{
    VolumeSet(progress);
}


bool PhoneMenuCallingVoice::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool PhoneMenuCallingVoice::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool PhoneMenuCallingVoice::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}

}
