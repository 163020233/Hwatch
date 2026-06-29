/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PullUpDownControls
 * Created: 2025-11-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "main/MainModel.h"
#include "ui_resource_image.h"
#include "main/MainPresenterSample.h"
#include "ui_resource_pulldown.h"
#include "dock/focus_manager.h"
#include "main/PullUpDownControls.h"

namespace OHOS {

#ifdef __cplusplus
extern "C" {
#endif

static constexpr uint16_t SCROLLVIEW_EXIT_DISTANCE = 400;
static constexpr uint16_t SLIDER_WIDTH = 248;
static constexpr uint16_t SLIDER_HEIGHT = 112;
static constexpr uint16_t SLIDER_IMG_POSITION_X = 24;
static constexpr uint16_t SLIDER_IMG_POSITION_Y = 30;
static constexpr uint16_t SLIDER_VALUE_MAX = 100;
static constexpr uint16_t VIEW_RADIUS = 64;
static constexpr uint16_t BUTTON_WIDTH = 112;
static constexpr uint16_t BUTTON_HEIGHT = 146;
static constexpr uint16_t BUTTON_VIEW_IMG_POSITION = 30;
static constexpr uint16_t BUTTON_VIEW_TEXT_POSITION_Y = 112;
static constexpr uint16_t BUTTON_VIEW_TEXT_HEIGHT = 40;
static constexpr uint16_t TEXT_SIZE = 24;
static constexpr uint16_t BIG_BUTTON_VIEW_WIDTH = 264;
static constexpr uint16_t BIG_BUTTON_VIEW_HEIGHT = 170;
static constexpr uint16_t BIG_BUTTON_VIEW_BG_BUTTON_HEIGHT = 112;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_DES_POSITION_X = 124;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_DES_POSITION_Y = 50;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_DES_WIDTH = 140;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_DES_HEIGHT = 30;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_POSITION_X = 124;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_POSITION_Y = 138;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_WIDTH = 264;
static constexpr uint16_t BIG_BUTTON_VIEW_TEXT_HEIGHT = 30;

bool PullupDownScrollView::OnDragStartEvent(const DragEvent& event)
{
    uint16_t posY = event.GetStartPoint().y;
    if (posY < SCROLLVIEW_EXIT_DISTANCE) {
        UIScrollView::OnDragStartEvent(event);
        return true;
    } else {
        return false;
    }
}

bool PullupDownScrollView::OnDragEvent(const DragEvent& event)
{
    uint16_t posY = event.GetStartPoint().y;
    if (posY < SCROLLVIEW_EXIT_DISTANCE) {
        UIScrollView::OnDragEvent(event);
        return true;
    } else {
        return false;
    }
}

bool PullupDownScrollView::OnDragEndEvent(const DragEvent& event)
{
    uint16_t posY = event.GetStartPoint().y;
    if (posY < SCROLLVIEW_EXIT_DISTANCE) {
        UIScrollView::OnDragEndEvent(event);
        return true;
    } else {
        return false;
    }
}

PullUpDownSlider::~PullUpDownSlider()
{
    RemoveAll();
    if (img_ != nullptr) {
        delete img_;
        img_ = nullptr;
    }
    if (slider_ != nullptr) {
        delete slider_;
        slider_ = nullptr;
    }
}

PullUpDownSlider::PullUpDownSlider(SliderType type)
{
    type_ = type;
    Resize(SLIDER_WIDTH, SLIDER_HEIGHT);
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    SetDraggable(true);
    SetTouchable(true);
    SetOnClickListener(this);
    SetOnDragListener(this);
    img_ = new UIImageView();
    if (img_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownSlider img_ new fail");
        return;
    }
    img_->SetPosition(SLIDER_IMG_POSITION_X, SLIDER_IMG_POSITION_Y);

    slider_ = new UISlider();
    if (slider_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownSlider slider_ new fail");
        return;
    }
    FocusManager::GetInstance()->RequestFocus(slider_);
    slider_->SetPosition(0, 0, SLIDER_WIDTH, SLIDER_HEIGHT);
    slider_->SetRange(0, SLIDER_VALUE_MAX);
    if (type_ == SliderType::LIGHT) {
        int curBrightness = SettingDisplayModel::GetInstance().GetBrightness();
        slider_->SetValue(curBrightness);
        slider_->SetSliderColor(Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0x66),
                                Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0xFF));
    } else if (type_ == SliderType::VOLUME) {
        int curSound = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_MUSIC);
        slider_->SetValue(curSound);
        slider_->SetSliderColor(Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0x66),
                                Color::GetColorFromRGBA(0xFF, 0xFF, 0xFF, 0xFF));
    }
    slider_->SetSliderRadius(VIEW_RADIUS, VIEW_RADIUS);
    slider_->SetBackgroundStyle(STYLE_BACKGROUND_OPA, SLIDER_VALUE_MAX);
    slider_->SetDirection(UISlider::Direction::DIR_LEFT_TO_RIGHT);
    slider_->SetSliderEventListener(this);
    slider_->SetOnClickListener(this);
    slider_->SetOnDragListener(this);
    slider_->LayoutCenterOfParent();
    RefreshFragment();

    Add(slider_);
    Add(img_);
}

void PullUpDownSlider::RefreshFragment()
{
    if (type_ == SliderType::LIGHT) {
        int curBrightness = SettingDisplayModel::GetInstance().GetBrightness();
        slider_->SetValue(curBrightness);
        if (curBrightness > 0) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_LIGHT_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_LIGHT_DARK);
        }
    } else if (type_ == SliderType::VOLUME) {
        int curSound = AudioManager::GetInstance().GetVolume(AUDIO_STREAM_MUSIC);
        slider_->SetValue(curSound);
        if (curSound > 0) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_VOICE_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_VOICE_DARK);
        }
    }
    img_->Invalidate();
}

void PullUpDownSlider::VolumeSet(int16_t sounding)
{
    BtAvrcpTgInf *bt_avrcp_srv_inf = BtAvrcpSrvGetInfo();
    if (bt_avrcp_srv_inf->avrcp_srv_conn_stat == PROFILE_STATE_CONNECTED) {
        uint8_t volume = sounding * AVRCP_ABSOLUTE_VOLUME_PERCENTAGE_100 / 100; // 100：本地音量最大值
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "avrcp volume = %d", volume);
        (void)avrcp_ct_set_absolute_volume(&(bt_avrcp_srv_inf->bd_addr), volume); // 手表音量设置同步远端BT音量
    }
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "audio volume = %d", sounding);
    AudioManager::GetInstance().SetVolume(AUDIO_STREAM_MUSIC, sounding); // 本地音量设置
}

void PullUpDownSlider::OnChange(int32_t progress)
{
    if (type_ == SliderType::LIGHT) {
        SettingDisplayModel::GetInstance().SetBrightness(progress);
        if (progress > 0) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_LIGHT_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_LIGHT_DARK);
        }
    } else if (type_ == SliderType::VOLUME) {
        VolumeSet(progress);
        if (progress > 0) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_VOICE_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_VOICE_DARK);
        }
    }
}

void PullUpDownSlider::OnRelease(int32_t progress)
{
    if (type_ == SliderType::LIGHT) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnRelease LIGHT progress:%d", progress);
        SettingDisplayModel::GetInstance().SetBrightness(progress);
    } else if (type_ == SliderType::VOLUME) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "OnRelease VOLUME progress:%d", progress);
        VolumeSet(progress);
    }
}

PullupDownButton::PullupDownButton(int imgSource, const char* text, bool highLight)
{
    Resize(BUTTON_WIDTH, BUTTON_HEIGHT);
    SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    SetTouchable(true);
    SetOnClickListener(OHOS::MainPresenterSample::GetInstance());

    img_ = new UIImageView();
    if (img_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullupDownButton img_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, imgSource);
    img_->SetPosition(0, 0);

    textLabel_ = new UILabel();
    if (textLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullupDownButton textLabel_ new fail");
        return;
    }
    textLabel_->SetPosition(0, BUTTON_VIEW_TEXT_POSITION_Y, BUTTON_WIDTH, BUTTON_VIEW_TEXT_HEIGHT);
    textLabel_->SetText(text);
    textLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    textLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    
    Add(img_);
    Add(textLabel_);
}

PullupDownButton::~PullupDownButton()
{
    RemoveAll();
    if (img_ != nullptr) {
        delete img_;
        img_ = nullptr;
    }
    if (textLabel_ != nullptr) {
        delete textLabel_;
        textLabel_ = nullptr;
    }
}

void PullupDownButton::ChangeState(bool flag)
{
    const char* text = textLabel_->GetText();
    if (text != nullptr && strcmp(text, "亮屏") == 0) {
        if (flag) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_SCREEN_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_SCREEN_DARK);
        }
    } else if (text != nullptr && strcmp(text, "勿扰") == 0) {
        if (flag) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_DONTDIST_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_DONTDIST_DARK);
        }
    } else if (text != nullptr && strcmp(text, "静音") == 0) {
        if (flag) {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_MUTE_BRIGHT);
        } else {
            LOADIMG::LoadImageViewImg(img_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_MUTE_DARK);
        }
    }
    return;
}
#ifdef __cplusplus
}
#endif
}
