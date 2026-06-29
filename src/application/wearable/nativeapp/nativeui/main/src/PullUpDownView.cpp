/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: PullUpDownView
 * Create: 2025-06-29
 */

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "wearable_log.h"
#include "UiConfig.h"
#include "common/screen.h"
#include "main/MainPresenterSample.h"
#include "ui_resource_image.h"
#include "ui_resource_pulldown.h"
#include "settings/model/SettingPowerModel.h"
#include "main/PullUpDownModel.h"
#include "main/PullUpDownView.h"

namespace OHOS {

static PullUpDownView *g_pPullUpDownView = nullptr;

static constexpr uint16_t BUTTON_X1 = 41;
static constexpr uint16_t BUTTON_X2 = 177;
static constexpr uint16_t BUTTON_X3 = 313;
static constexpr uint16_t BUTTON_Y1 = 176;
static constexpr uint16_t BUTTON_Y2 = 341;
static constexpr uint16_t BUTTON_Y3 = 506;
static constexpr uint16_t BUTTON_Y4 = 671;
static constexpr uint16_t TEXT_SIZE = 24;
static constexpr uint16_t LIGHT_LABEL_Y = 288;
static constexpr uint16_t LIGHT_LABEL_WIDTH = 264;
static constexpr uint16_t LIGHT_LABEL_HEIGHT = 40;
static constexpr uint16_t VOLUME_LABEL_Y = 618;
static constexpr uint16_t VOLUME_LABEL_WIDTH = 264;
static constexpr uint16_t VOLUME_LABEL_HEIGHT = 40;
static constexpr uint16_t BATTERY_IMG_X = 145;
static constexpr uint16_t BATTERY_IMG_Y = 37;
static constexpr uint16_t BATTERY_LABEL_X = 210;
static constexpr uint16_t BATTERY_LABEL_Y = 45;
static constexpr uint16_t DATE_LABEL_X = 145;
static constexpr uint16_t DATE_LABEL_Y = 95;
static constexpr uint16_t TEXT_WIDTH = 192;
static constexpr uint16_t TEXT_HEIGHT = 30;
static constexpr uint16_t VERSION_LABEL_Y = 883;
static constexpr uint16_t VERSION_LABEL_HEIGHT = 30;
static constexpr uint16_t ONLY_ONE_CONNECT_X = 270;
static constexpr uint16_t HEADPHONE_CONNECT_X = 260;
static constexpr uint16_t PHONE_CONNECT_X = 296;
static constexpr uint16_t HEADPHONE_IMG_Y = 45;
static constexpr uint16_t BLANK_SIZE_TOP = 30;
static constexpr uint16_t BLANK_SIZE_BOTTOM = 30;
static constexpr uint16_t BATTERY_LEVEL1 = 10;
static constexpr uint16_t BATTERY_LEVEL2 = 20;
static constexpr uint16_t BATTERY_LEVEL3 = 40;
static constexpr uint16_t BATTERY_LEVEL4 = 70;
static constexpr uint16_t BATTERY_LEVEL5 = 100;
static constexpr int DESK_IMAGE[] = {IMAGE_PULLDOWN_DESK_LIST, IMAGE_PULLDOWN_DESK_HONEYCOMB,
                                     IMAGE_PULLDOWN_DESK_BALL, IMAGE_PULLDOWN_DESK_WATERFALLS,
                                     IMAGE_PULLDOWN_DESK_PLENT, IMAGE_PULLDOWN_DESK_CIRCLE};

PullUpDownView::PullUpDownView()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PullUpDownView::PullUpDownView");
    g_pPullUpDownView = this;
}

PullUpDownView::~PullUpDownView()
{
    RemoveAll();
    if (scroll_ != nullptr) {
        scroll_->RemoveAll();
        delete scroll_;
        scroll_ = nullptr;
    }
    if (headPhone_ != nullptr) {
        delete headPhone_;
        headPhone_ = nullptr;
    }
    if (phone_ != nullptr) {
        delete phone_;
        phone_ = nullptr;
    }
    if (battery_ != nullptr) {
        delete battery_;
        battery_ = nullptr;
    }
    if (batteryLabel_ != nullptr) {
        delete batteryLabel_;
        batteryLabel_ = nullptr;
    }
    if (dateLabel_ != nullptr) {
        delete dateLabel_;
        dateLabel_ = nullptr;
    }
    if (screenButton_ != nullptr) {
        delete screenButton_;
        screenButton_ = nullptr;
    }
    if (dnotDisturButton_ != nullptr) {
        delete dnotDisturButton_;
        dnotDisturButton_ = nullptr;
    }
    if (alarmButton_ != nullptr) {
        delete alarmButton_;
        alarmButton_ = nullptr;
    }
    if (findPhoneButton_ != nullptr) {
        delete findPhoneButton_;
        findPhoneButton_ = nullptr;
    }
    if (lightSlider_ != nullptr) {
        delete lightSlider_;
        lightSlider_ = nullptr;
    }
    if (lightLabel_ != nullptr) {
        delete lightLabel_;
        lightLabel_ = nullptr;
    }
    if (volumeSlider_ != nullptr) {
        delete volumeSlider_;
        volumeSlider_ = nullptr;
    }
    if (volumeLabel_ != nullptr) {
        delete volumeLabel_;
        volumeLabel_ = nullptr;
    }
    if (setButton_ != nullptr) {
        delete setButton_;
        setButton_ = nullptr;
    }
    if (muteButton_ != nullptr) {
        delete muteButton_;
        muteButton_ = nullptr;
    }
    if (flashLightButton_ != nullptr) {
        delete flashLightButton_;
        flashLightButton_ = nullptr;
    }
    if (deskStyleButton_ != nullptr) {
        delete deskStyleButton_;
        deskStyleButton_ = nullptr;
    }
    if (versionLabel_ != nullptr) {
        delete versionLabel_;
        versionLabel_ = nullptr;
    }
    g_pPullUpDownView = nullptr;
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "PullUpDownView::~PullUpDownView");
}

PullUpDownView *PullUpDownView::GetInstance(void)
{
    return g_pPullUpDownView;
}

void PullUpDownView::InitViewInfo(void)
{
    battery_ = new UIImageView(); // 电量状态显示
    if (battery_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView battery_ new fail");
        return;
    }
    battery_->SetPosition(BATTERY_IMG_X, BATTERY_IMG_Y);
    uint8_t battery = SettingPowerModel::GetInstance().GetBatteryPowerLevel();
    if (battery < BATTERY_LEVEL1) {
        LOADIMG::LoadImageViewImg(battery_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_BATTERY_0);
    } else if (battery < BATTERY_LEVEL2) {
        LOADIMG::LoadImageViewImg(battery_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_BATTERY_20);
    } else if (battery < BATTERY_LEVEL3) {
        LOADIMG::LoadImageViewImg(battery_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_BATTERY_40);
    } else if (battery < BATTERY_LEVEL4) {
        LOADIMG::LoadImageViewImg(battery_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_BATTERY_70);
    } else if (battery <= BATTERY_LEVEL5) {
        LOADIMG::LoadImageViewImg(battery_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_BATTERY_100);
    }

    batteryLabel_ = new UILabel();
    if (batteryLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView batteryLabel_ new fail");
        return;
    }
    batteryLabel_->SetPosition(BATTERY_LABEL_X, BATTERY_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    std::string batteryStr = std::to_string(battery) + "%";
    batteryLabel_->SetText(batteryStr.c_str());
    batteryLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    batteryLabel_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);

    phone_ = new UIImageView();
    if (phone_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView phone_ new fail");
        return;
    }
    phone_->SetPosition(PHONE_CONNECT_X, HEADPHONE_IMG_Y);
    LOADIMG::LoadImageViewImg(phone_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_PHONE_CONNECT);

    headPhone_ = new UIImageView();
    if (headPhone_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView headPhone_ new fail");
        return;
    }
    headPhone_->SetPosition(HEADPHONE_CONNECT_X, HEADPHONE_IMG_Y);
    LOADIMG::LoadImageViewImg(headPhone_, PULLDOWN_IAMGES, IMAGE_PULLDOWN_EARPHONE_CONNECT);
    UpdatePhoneAndHeadPhoneConnectStatus();

    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    if (localTime == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "nullptr, %s, %d\r\n", __func__, __LINE__);
        return;
    }
    std::ostringstream dateStream;
    dateStream << std::put_time(localTime, "%m/%d");
    const char* weekdays[] = {"日", "一", "二", "三", "四", "五", "六"};
    int weekday = localTime->tm_wday;
    std::string date = dateStream.str() + " 星期" + weekdays[weekday];

    dateLabel_ = new UILabel();
    if (dateLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView dateLabel_ new fail");
        return;
    }
    dateLabel_->SetPosition(DATE_LABEL_X, DATE_LABEL_Y, TEXT_WIDTH, TEXT_HEIGHT);
    dateLabel_->SetText(date.c_str());
    dateLabel_->SetAlign(TEXT_ALIGNMENT_CENTER);
    dateLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    dateLabel_->SetStyle(STYLE_BACKGROUND_COLOR, OPA_TRANSPARENT);
    int8_t half = 2;
    int16_t posX = (HORIZONTAL_RESOLUTION - TEXT_WIDTH) / half;
    dateLabel_->SetX(posX);

    versionLabel_ = new UILabel();
    if (versionLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView versionLabel_ new fail");
        return;
    }
    versionLabel_->SetPosition(0, VERSION_LABEL_Y, HORIZONTAL_RESOLUTION, VERSION_LABEL_HEIGHT);
    versionLabel_->SetText("版本：" VERSION_STRING);
    versionLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    versionLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
}

void PullUpDownView::InitButton(void)
{
    setButton_ = new PullupDownButton(IMAGE_PULLDOWN_SET, "设置", false);
    if (setButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView setButton_ new fail");
        return;
    }
    setButton_->SetPosition(BUTTON_X1, BUTTON_Y1);
    setButton_->SetViewId(DROPDOWN_CLICK_SETTING);

    muteButton_ = new PullupDownButton(IMAGE_PULLDOWN_MUTE_DARK, "静音", false);
    if (muteButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView muteButton_ new fail");
        return;
    }
    muteButton_->SetPosition(BUTTON_X1, BUTTON_Y2);
    muteButton_->SetViewId(DROPDOWN_CLICK_MUTE);

    alarmButton_ = new PullupDownButton(IMAGE_PULLDOWN_ALARM, "闹钟", false);
    if (alarmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView alarmButton_ new fail");
        return;
    }
    alarmButton_->SetPosition(BUTTON_X2, BUTTON_Y2);
    alarmButton_->SetViewId(DROPDOWN_CLICK_ALARM);

    int imageSource = 0;
    bool highLight = PullUpDownModel::GetInstance()->GetBriScrStatus();
    if (highLight == 0) {
        imageSource = IMAGE_PULLDOWN_SCREEN_DARK;
        highLight = false;
    } else {
        imageSource = IMAGE_PULLDOWN_SCREEN_BRIGHT;
        highLight = true;
    }
    screenButton_ = new PullupDownButton(imageSource, "亮屏", highLight);
    if (screenButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView screenButton_ new fail");
        return;
    }
    screenButton_->SetPosition(BUTTON_X3, BUTTON_Y2);
    screenButton_->SetViewId(DROPDOWN_CLICK_BRIGHTNESS);

    findPhoneButton_ = new PullupDownButton(IMAGE_PULLDOWN_FINDPHONE, "找手机", false);
    if (findPhoneButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView findPhoneButton_ new fail");
        return;
    }
    findPhoneButton_->SetPosition(BUTTON_X1, BUTTON_Y3);
    findPhoneButton_->SetViewId(DROPDOWN_CLICK_FINDMYPHONE);

    highLight = PullUpDownModel::GetInstance()->GetDntDistStatus();
    if (highLight == 0) {
        imageSource = IMAGE_PULLDOWN_DONTDIST_DARK;
        highLight = false;
    } else {
        imageSource = IMAGE_PULLDOWN_DONTDIST_BRIGHT;
        highLight = true;
    }
    dnotDisturButton_ = new PullupDownButton(imageSource, "勿扰", highLight);
    if (dnotDisturButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView dnotDisturButton_ new fail");
        return;
    }
    dnotDisturButton_->SetPosition(BUTTON_X1, BUTTON_Y4);
    dnotDisturButton_->SetViewId(DROPDOWN_CLICK_DONOTDISTURB);

    flashLightButton_ = new PullupDownButton(IMAGE_PULLDOWN_FLASHLIGHT, "手电筒", false);
    if (flashLightButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView flashLightButton_ new fail");
        return;
    }
    flashLightButton_->SetPosition(BUTTON_X2, BUTTON_Y4);
    flashLightButton_->SetViewId(DROPDOWN_CLICK_FLASHLIGHT);

    DesktopStyle style = SettingDesktopModel::GetInstance().GetDesktopStyle();
    deskStyleButton_ = new PullupDownButton(DESK_IMAGE[style], "桌面风格", false);
    if (deskStyleButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView deskStyleButton_ new fail");
        return;
    }
    deskStyleButton_->SetPosition(BUTTON_X3, BUTTON_Y4);
    deskStyleButton_->SetViewId(DROPDOWN_CLICK_SETTING_LIST_MODE);
}

void PullUpDownView::InitViewSlider(void)
{
    lightSlider_ = new PullUpDownSlider(SliderType::LIGHT);
    if (lightSlider_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView lightSlider_ new fail");
        return;
    }
    lightSlider_->SetPosition(BUTTON_X2, BUTTON_Y1);

    lightLabel_ = new UILabel();
    if (lightLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView lightLabel_ new fail");
        return;
    }
    lightLabel_->SetPosition(BUTTON_X2, LIGHT_LABEL_Y, LIGHT_LABEL_WIDTH, LIGHT_LABEL_HEIGHT);
    lightLabel_->SetText("亮度");
    lightLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    lightLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);

    volumeSlider_ = new PullUpDownSlider(SliderType::VOLUME);
    if (volumeSlider_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView volumeSlider_ new fail");
        return;
    }
    volumeSlider_->SetPosition(BUTTON_X2, BUTTON_Y3);

    volumeLabel_ = new UILabel();
    if (volumeLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView volumeLabel_ new fail");
        return;
    }
    volumeLabel_->SetPosition(BUTTON_X2, VOLUME_LABEL_Y, VOLUME_LABEL_WIDTH, VOLUME_LABEL_HEIGHT);
    volumeLabel_->SetText("音量");
    volumeLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, TEXT_SIZE);
    volumeLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
}

bool PullUpDownView::InitView()
{
    scroll_ = new PullupDownScrollView();
    if (scroll_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "PullUpDownView scroll_ new fail");
        return false;
    }
    scroll_->SetPosition(0, 0, HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    scroll_->SetStyle(STYLE_BACKGROUND_OPA, OPA_TRANSPARENT);
    scroll_->SetScrollBlankSize(BLANK_SIZE_TOP, UIAbstractScroll::Direction::TOP);
    scroll_->SetScrollBlankSize(BLANK_SIZE_BOTTOM, UIAbstractScroll::Direction::BOTTOM);
    InitViewInfo();
    InitButton();
    InitViewSlider();
    scroll_->Add(battery_);
    scroll_->Add(batteryLabel_);
    scroll_->Add(headPhone_);
    scroll_->Add(phone_);
    scroll_->Add(dateLabel_);
    scroll_->Add(screenButton_);
    scroll_->Add(dnotDisturButton_);
    scroll_->Add(alarmButton_);
    scroll_->Add(findPhoneButton_);
    scroll_->Add(setButton_);
    scroll_->Add(muteButton_);
    scroll_->Add(lightSlider_);
    scroll_->Add(lightLabel_);
    scroll_->Add(volumeSlider_);
    scroll_->Add(volumeLabel_);
    scroll_->Add(flashLightButton_);
    scroll_->Add(deskStyleButton_);
    scroll_->Add(versionLabel_);
    Add(scroll_);
    return true;
}

void PullUpDownView::Refresh()
{
    uint16_t y = HEADPHONE_IMG_Y - headPhone_->GetY();
    scroll_->ScrollBy(0, y);
    scroll_->Invalidate();
}

void PullUpDownView::PreLoad(void)
{
    if (!viewInitStatus_) {
        InitView();
        viewInitStatus_ = true;
    }
}

void PullUpDownView::UpdatePhoneAndHeadPhoneConnectStatus()
{
    if (headPhone_ == nullptr || phone_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "headPhone_ phone_ nullptr");
        return;
    }
    if (SettingBluetoothModel::GetInstance() == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "headPhone_ model nullptr");
        return;
    }
    headPhone_->SetVisible(false);
    phone_->SetVisible(false);

    bool headPhoneConnectStatus = SettingBluetoothModel::GetInstance()->GetHeadPhoneConnectStatus();
    bool phoneConnectStatus = SettingBluetoothModel::GetInstance()->GetPhoneConnected();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "headPhoneConnectStatus:%d phoneConnectStatus:%d",
                  headPhoneConnectStatus, phoneConnectStatus);
    if (headPhoneConnectStatus && phoneConnectStatus) {
        headPhone_->SetX(HEADPHONE_CONNECT_X);
        headPhone_->SetVisible(true);
        phone_->SetX(PHONE_CONNECT_X);
        phone_->SetVisible(true);
    } else if (headPhoneConnectStatus) {
        headPhone_->SetX(ONLY_ONE_CONNECT_X);
        headPhone_->SetVisible(true);
    } else if (phoneConnectStatus) {
        phone_->SetX(ONLY_ONE_CONNECT_X);
        phone_->SetVisible(true);
    }
    headPhone_->Invalidate();
    phone_->Invalidate();
    return;
}
}
