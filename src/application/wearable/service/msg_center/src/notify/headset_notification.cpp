/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: headset notification implement
 * Author: CompanyName
 * Create: 2025-05-13
 */

#include <unistd.h>
#include "notification_manager.h"
#include "wearable_log.h"
#include "UiConfig.h"
#include "components/ui_image_view.h"
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_image_animator.h"
#include "NativeRegisterManager.h"
#include "graphic_timer.h"
#include "AppViewIDs.h"
#include "samgr_lite.h"
#include "common/image_cache_manager.h"
#include "cmsis_os2.h"
#include "graphic_service.h"
#include "common_def.h"

namespace OHOS {
UIImageAnimatorView *imageAnimator = nullptr;
UIViewGroup *g_headsetConnView = nullptr;
UIViewGroup *g_HeadsetPopView = nullptr;
UIViewGroup *g_HeadsetConnFailView = nullptr;
UILabelButton *g_buttonNo = nullptr;
UILabelButton *g_buttonYes = nullptr;
UILabelButton *g_connFailConfirm = nullptr;
UILabel *g_headsetMainLable = nullptr;
UILabelButton *g_buttonCancel = nullptr;
UILabelButton *g_buttonDetails = nullptr;
UILabel *g_headsetPopMainLable = nullptr;
UILabel *g_headsetConnectingLable = nullptr;
UILabel *g_headsetConnFailLable = nullptr;
UIImageView *g_headsetImg = nullptr;

UIImageView *g_headsetBatteryLeft = nullptr;
UIImageView *g_headsetBatteryRight = nullptr;

constexpr int32_t TWS_NAME_LENGTH = 64;
char g_twsName[TWS_NAME_LENGTH];
uint32_t TWS_BATTERY;
uint32_t TWS_VOLUME;

constexpr int16_t MAX_SET_SRC_LEN = 90;
constexpr int16_t BUTTON_WIDTH = 100;
constexpr int16_t BUTTON_HEIGHT = 70;
// container
constexpr int16_t CONTAINER_X = 0;
constexpr int16_t CONTAINER_Y = 0;
// label
constexpr int16_t LABEL_X = 185;
constexpr int16_t LABEL_Y = 60;
constexpr int16_t LABEL_WIDTH = 250;
constexpr int16_t LABEL_HEIGHT = 45;
constexpr int16_t LABEL_FONT = 40;
constexpr int16_t CONNECTING_LABEL_FONT = 35;
// buttons
constexpr int16_t NO_BUTTON_X = 85;
constexpr int16_t NO_BUTTON_Y = 320;
constexpr int16_t YES_BUTTON_X = 280;
constexpr int16_t YES_BUTTON_Y = 320;
constexpr int16_t CONNECTING_LABEL_Y = 340;
constexpr int16_t BUTTON_FONT = 30;

static constexpr uint16_t IMAGE_SIZE = 200;
static constexpr uint16_t IMAGE_POSITION_X = 127;
static constexpr uint16_t IMAGE_POSITION_Y = 110;
static constexpr uint16_t IMAGEANIMATORVIEW_SIZE = 48;
static constexpr uint16_t IMAGEANIMATORVIEW_SPEED = 50;

#define TEST_TASK_STACK_SIZE    0x1000

#define BLUETOOTH_SEARCH_IMAGE_NUM 8
#define BLUETOOTH_IMAGE_X 175
#define BLUETOOTH_IMAGE_Y 175
#define BLUETOOTH_IMAGE_WIDTH 104
#define BLUETOOTH_IMAGE_HEIGHT 104
#define BLUETOOTH_DELAY 200

static const ImageAnimatorInfo g_imageAnimatorInfo_new[48] = {
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_01, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_02, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_03, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_04, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_05, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_06, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_07, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_08, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_09, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_10, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_11, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_12, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_13, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_14, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_15, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_16, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_17, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_18, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_19, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_20, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_21, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_22, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_23, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_24, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_25, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_26, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_27, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_28, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_29, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_30, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_31, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_32, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_33, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_34, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_35, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_36, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH},
    {HEADSET_IMAGE_37, {IMAGE_POSITION_X, IMAGE_POSITION_Y}, IMAGE_SIZE, IMAGE_SIZE, IMG_SRC_FILE_PATH}};

static bool CreateHeadsetConnViewImage(void)
{
    g_buttonNo = new UILabelButton();
    if (g_buttonNo == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonNo null!");
        return false;
    }
    g_buttonNo->SetPosition(NO_BUTTON_X, NO_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_buttonNo->SetText("取消");
    g_buttonNo->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT);
    g_buttonNo->SetViewId(HEADSET_IMAGE_NO);
    g_buttonNo->SetTouchable(true);
    UIView::OnClickListener *closeListener =
        static_cast<UIView::OnClickListener *>(HeadsetViewEventListener::GetInstance());
    g_buttonNo->SetOnClickListener(closeListener);

    g_buttonYes = new UILabelButton();
    if (g_buttonYes == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonYes null!");
        delete g_buttonYes;
        return false;
    }
    g_buttonYes->SetPosition(YES_BUTTON_X, YES_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_buttonYes->SetText("连接");
    g_buttonYes->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT);
    g_buttonYes->SetViewId(HEADSET_IMAGE_YES);
    g_buttonYes->SetTouchable(true);
    UIView::OnClickListener *headsetListener =
        static_cast<UIView::OnClickListener *>(OHOS::HeadsetViewEventListener::GetInstance());
    g_buttonYes->SetOnClickListener(headsetListener);

    g_headsetConnectingLable = new UILabel();
    if (g_headsetConnectingLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_headsetConnectingLable is nullptr");
        delete g_buttonNo;
        delete g_buttonYes;
        g_buttonNo = nullptr;
        g_buttonYes = nullptr;
        return false;
    }
    g_headsetConnectingLable->SetPosition(0, CONNECTING_LABEL_Y, HORIZONTAL_RESOLUTION, LABEL_HEIGHT);
    g_headsetConnectingLable->SetText("连接中...");
    g_headsetConnectingLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, CONNECTING_LABEL_FONT);
    g_headsetConnectingLable->SetAlign(TEXT_ALIGNMENT_CENTER);
    return true;
}

static bool CreateHeadsetConnFailButton(void)
{
    g_connFailConfirm = new UILabelButton();
    if (g_connFailConfirm == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_connFailConfirm null!");
        return false;
    }
    g_connFailConfirm->SetPosition(177, NO_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_connFailConfirm->SetText("确认");
    g_connFailConfirm->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT);
    g_connFailConfirm->SetViewId(HEADSET_CONN_FAIL_BUTTON);
    g_connFailConfirm->SetTouchable(true);
    UIView::OnClickListener *closeListener =
        static_cast<UIView::OnClickListener *>(HeadsetViewEventListener::GetInstance());
    g_connFailConfirm->SetOnClickListener(closeListener);

    return true;
}

static void FreeHeadsetPopViewImage(void)
{
    if (g_buttonCancel != nullptr) {
        delete g_buttonCancel;
        g_buttonCancel = nullptr;
    }

    if (g_buttonDetails != nullptr) {
        delete g_buttonDetails;
        g_buttonDetails = nullptr;
    }

    if (g_headsetBatteryLeft != nullptr) {
        delete g_headsetBatteryLeft;
        g_headsetBatteryLeft = nullptr;
    }

    if (g_headsetBatteryRight != nullptr) {
        delete g_headsetBatteryRight;
        g_headsetBatteryRight = nullptr;
    }
}

static bool CreateHeadsetPopViewImage(void)
{
    g_buttonCancel = new UILabelButton();
    if (g_buttonCancel == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonCancel null!");
        return false;
    }
    g_buttonCancel->SetPosition(NO_BUTTON_X, NO_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_buttonCancel->SetText("取消");
    g_buttonCancel->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT);
    g_buttonCancel->SetViewId(HEADSET_CANCEL);
    g_buttonCancel->SetTouchable(true);
    UIView::OnClickListener *closeListener =
        static_cast<UIView::OnClickListener *>(HeadsetViewEventListener::GetInstance());
    g_buttonCancel->SetOnClickListener(closeListener);

    g_buttonDetails = new UILabelButton();
    if (g_buttonDetails == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonDetails null!");
        delete g_buttonDetails;
        delete g_buttonCancel;
        g_buttonCancel = nullptr;
        return false;
    }
    g_buttonDetails->SetPosition(YES_BUTTON_X, YES_BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT);
    g_buttonDetails->SetText("设置");

    g_buttonDetails->SetFont(DEFAULT_VECTOR_FONT_FILENAME, BUTTON_FONT);
    g_buttonDetails->SetViewId(HEADSET_DETAILS);
    g_buttonDetails->SetTouchable(true);
    UIView::OnClickListener *headsetListener =
        static_cast<UIView::OnClickListener *>(OHOS::HeadsetViewEventListener::GetInstance());
    g_buttonDetails->SetOnClickListener(headsetListener);

    g_headsetBatteryLeft = new UIImageView();
    if (g_headsetBatteryLeft == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonYes null!");
        FreeHeadsetPopViewImage();
        return false;
    }
    g_headsetBatteryLeft->SetPosition(50, 220);
    g_headsetBatteryLeft->SetWidth(75);
    g_headsetBatteryLeft->SetHeight(38);

    g_headsetBatteryRight = new UIImageView();
    if (g_headsetBatteryRight == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "showHeadsetNotify g_buttonYes null!");
        FreeHeadsetPopViewImage();
        return false;
    }
    g_headsetBatteryRight->SetPosition(329, 220);
    g_headsetBatteryRight->SetWidth(75);
    g_headsetBatteryRight->SetHeight(38);
    if (TWS_BATTERY < 10) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_01);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_01);
    } else if (10 <= TWS_BATTERY && TWS_BATTERY < 20) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_02);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_02);
    } else if (20 <= TWS_BATTERY && TWS_BATTERY < 30) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_03);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_03);
    } else if (30 <= TWS_BATTERY && TWS_BATTERY < 40) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_04);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_04);
    } else if (40 <= TWS_BATTERY && TWS_BATTERY < 50) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_05);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_05);
    } else if (50 <= TWS_BATTERY && TWS_BATTERY < 60) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_06);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_06);
    } else if (60 <= TWS_BATTERY && TWS_BATTERY < 70) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_07);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_07);
    } else if (70 <= TWS_BATTERY && TWS_BATTERY < 80) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_08);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_08);
    } else if (80 <= TWS_BATTERY && TWS_BATTERY < 90) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_09);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_09);
    } else if (90 <= TWS_BATTERY && TWS_BATTERY <= 100) {
        g_headsetBatteryRight->SetSrc(HEADSET_BATTERY_IMAGE_10);
        g_headsetBatteryLeft->SetSrc(HEADSET_BATTERY_IMAGE_10);
    }

    return true;
}

static void FreeHeadsetConnViewImage(void)
{
    delete g_buttonNo;
    g_buttonNo = nullptr;
    delete g_buttonYes;
    g_buttonYes = nullptr;
    delete g_headsetConnectingLable;
    g_headsetConnectingLable = nullptr;
}

static bool CreateHeadsetConnViewLabel(void)
{
    g_headsetMainLable = new UILabel();
    if (g_headsetMainLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetViewLabel g_headsetMainLable is nullptr");
        return false;
    }
    g_headsetMainLable->SetPosition(0, LABEL_Y, HORIZONTAL_RESOLUTION, LABEL_HEIGHT);
    g_headsetMainLable->SetText(g_twsName);
    g_headsetMainLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    g_headsetMainLable->SetAlign(TEXT_ALIGNMENT_CENTER);

    imageAnimator = new UIImageAnimatorView();
    if (imageAnimator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BloodOxygenView::new UIImageAnimatorView fail");
        delete g_headsetMainLable;
        g_headsetMainLable = nullptr;
        return false;
    }
    imageAnimator->SetImageAnimatorSrc(g_imageAnimatorInfo_new, IMAGEANIMATORVIEW_SIZE, IMAGEANIMATORVIEW_SPEED);
    imageAnimator->Start();
    imageAnimator->SetRepeat(false);

    return true;
}
// 连接失败页Label
static bool CreateHeadsetConnFailLabel(void)
{
    g_headsetConnFailLable = new UILabel();
    if (g_headsetConnFailLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetViewLabel g_headsetMainLable is nullptr");
        return false;
    }
    g_headsetConnFailLable->SetPosition(0, 150, HORIZONTAL_RESOLUTION, LABEL_HEIGHT);
    g_headsetConnFailLable->SetText("连接失败");
    g_headsetConnFailLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    g_headsetConnFailLable->SetAlign(TEXT_ALIGNMENT_CENTER);

    return true;
}

static bool CreateHeadsetPopViewLabel(void)
{
    g_headsetPopMainLable = new UILabel();
    if (g_headsetPopMainLable == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetViewLabel g_headsetMainLable is nullptr");
        return false;
    }
    g_headsetPopMainLable->SetPosition(0, LABEL_Y, HORIZONTAL_RESOLUTION, LABEL_HEIGHT);
    g_headsetPopMainLable->SetText(g_twsName);
    g_headsetPopMainLable->SetFont(DEFAULT_VECTOR_FONT_FILENAME, LABEL_FONT);
    g_headsetPopMainLable->SetAlign(TEXT_ALIGNMENT_CENTER);
    imageAnimator = new UIImageAnimatorView();
    if (imageAnimator == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "BloodOxygenView::new UIImageAnimatorView fail");
        return false;
    }
    imageAnimator->SetImageAnimatorSrc(g_imageAnimatorInfo_new, IMAGEANIMATORVIEW_SIZE, IMAGEANIMATORVIEW_SPEED);
    imageAnimator->Start();
    imageAnimator->SetRepeat(false);
    return true;
}

static void FreeHeadsetConnViewLabel(void)
{
    delete g_headsetMainLable;
    g_headsetMainLable = nullptr;
    delete imageAnimator;
    imageAnimator = nullptr;
}

static void FreeHeadsetPopViewLabel(void)
{
    delete g_headsetPopMainLable;
    g_headsetPopMainLable = nullptr;
    delete imageAnimator;
    imageAnimator = nullptr;
}

static void FreeHeadsetConnFailLabel(void)
{
    delete g_headsetConnFailLable;
    g_headsetConnFailLable = nullptr;
}

UIViewGroup *CreateHeadsetConnView(char *twsName, uint32_t twsBattery, uint32_t twsVolume)
{
    if (twsName != nullptr) {
        errno_t ret1 = memset_s(g_twsName, TWS_NAME_LENGTH, 0, TWS_NAME_LENGTH);
        if (ret1 != EOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "memset_s failed");
        }

        errno_t ret2 = memcpy_s(g_twsName, TWS_NAME_LENGTH, twsName, strlen(twsName) + 1);
        if (ret2 != EOK) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "memcpy_s failed");
        }
    }
    g_headsetConnView = new UIViewGroup();
    if (g_headsetConnView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetView g_headsetView null!");
        return nullptr;
    }
    g_headsetConnView->SetPosition(
        CONTAINER_X, CONTAINER_Y, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    g_headsetConnView->SetViewId(HEADSET_CONN_VIEW);

    if (!CreateHeadsetConnViewLabel()) {
        delete g_headsetConnView;
        g_headsetConnView = nullptr;
        return nullptr;
    }

    if (!CreateHeadsetConnViewImage()) {
        delete g_headsetConnView;
        g_headsetConnView = nullptr;
        FreeHeadsetConnViewLabel();
        return nullptr;
    }
    g_headsetConnView->Add(imageAnimator);
    g_headsetConnView->Add(g_headsetMainLable);
    g_headsetConnView->Add(g_buttonNo);
    g_headsetConnView->Add(g_buttonYes);
    g_headsetConnView->SetDraggable(true);
    g_headsetConnView->SetTouchable(true);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener *>(HeadsetViewEventListener::GetInstance());
    g_headsetConnView->SetOnDragListener(listener);

    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeHeadsetConnView);
    return g_headsetConnView;
}

UIViewGroup *CreateHeadsetPopView(char *twsName, uint32_t twsBattery, uint32_t twsVolume)
{
    if (twsName != nullptr) {
        memset_s(g_twsName, TWS_NAME_LENGTH, 0, TWS_NAME_LENGTH);
        int ret = memcpy_s(g_twsName, TWS_NAME_LENGTH, twsName, strlen(twsName));
        if (ret != EOK) {
            return nullptr;
        }
    }

    TWS_BATTERY = twsBattery;
    TWS_VOLUME = twsVolume;
    g_HeadsetPopView = new UIViewGroup();
    if (g_HeadsetPopView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetConnView g_headsetConnView null!");
        return nullptr;
    }
    g_HeadsetPopView->SetPosition(
        CONTAINER_X, CONTAINER_Y, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    g_HeadsetPopView->SetViewId(HEADSET_POP_VIEW);
    if (!CreateHeadsetPopViewLabel()) {
        delete g_HeadsetPopView;
        g_HeadsetPopView = nullptr;
        FreeHeadsetPopViewLabel();
        return nullptr;
    }
    if (!CreateHeadsetPopViewImage()) {
        delete g_HeadsetPopView;
        g_HeadsetPopView = nullptr;
        FreeHeadsetPopViewLabel();
        return nullptr;
    }
    g_HeadsetPopView->Add(imageAnimator);
    g_HeadsetPopView->Add(g_headsetPopMainLable);
    g_HeadsetPopView->Add(g_buttonCancel);
    g_HeadsetPopView->Add(g_buttonDetails);
    g_HeadsetPopView->Add(g_headsetBatteryLeft);
    g_HeadsetPopView->Add(g_headsetBatteryRight);

    g_HeadsetPopView->SetDraggable(true);
    g_HeadsetPopView->SetTouchable(true);
    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener *>(HeadsetViewEventListener::GetInstance());
    g_HeadsetPopView->SetOnDragListener(listener);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeHeadsetPopView);

    return g_HeadsetPopView;
}

UIViewGroup *CreateConnectFailView()
{
    g_HeadsetConnFailView = new UIViewGroup();
    if (g_HeadsetConnFailView == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CreateHeadsetConnView g_HeadsetConnFailView null!");
        return nullptr;
    }
    g_HeadsetConnFailView->SetPosition(
        CONTAINER_X, CONTAINER_Y, Screen::GetInstance().GetWidth(), Screen::GetInstance().GetHeight());
    g_HeadsetConnFailView->SetViewId(HEADSET_CONN_FAIL_VIEW);

    if (!CreateHeadsetConnFailLabel()) {
        delete g_HeadsetConnFailView;
        g_HeadsetConnFailView = nullptr;
        return nullptr;
    }

    if (!CreateHeadsetConnFailButton()) {
        delete g_HeadsetConnFailView;
        g_HeadsetConnFailView = nullptr;
        FreeHeadsetConnFailLabel();
        return nullptr;
    }
    g_HeadsetConnFailView->Add(g_headsetConnFailLable);
    g_HeadsetConnFailView->Add(g_connFailConfirm);

    UIView::OnDragListener *listener = static_cast<UIView::OnDragListener *>(HeadsetViewEventListener::GetInstance());
    g_HeadsetConnFailView->SetOnDragListener(listener);
    NotificationManager::GetInstance()->RegisterNotifyCleanupFunction(FreeHeadsetConnFailView);

    return g_HeadsetConnFailView;
}

void FreeHeadsetConnFailView(void)
{
    if (g_HeadsetConnFailView != nullptr) {
        g_HeadsetConnFailView->RemoveAll();
        FreeHeadsetConnFailLabel();
        delete g_connFailConfirm;
        RootView::GetInstance()->Remove(g_HeadsetConnFailView);
        delete g_HeadsetConnFailView;
        g_HeadsetConnFailView = nullptr;
    }
}

void FreeHeadsetConnView(void)
{
    if (g_headsetConnView != nullptr) {
        g_headsetConnView->RemoveAll();
        FreeHeadsetConnViewLabel();
        FreeHeadsetConnViewImage();
        RootView::GetInstance()->Remove(g_headsetConnView);
        delete g_headsetConnView;
        g_headsetConnView = nullptr;
    }
}

void FreeHeadsetPopView(void)
{
    if (g_HeadsetPopView != nullptr) {
        g_HeadsetPopView->RemoveAll();
        FreeHeadsetPopViewLabel();
        FreeHeadsetPopViewImage();
        RootView::GetInstance()->Remove(g_HeadsetPopView);
        delete g_HeadsetPopView;
        g_HeadsetPopView = nullptr;
    }
}

HeadsetViewEventListener::HeadsetViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "HeadsetViewEventListener :%p", this);
}

HeadsetViewEventListener::~HeadsetViewEventListener()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "~HeadsetViewEventListener");
}

HeadsetViewEventListener *HeadsetViewEventListener::GetInstance(void)
{
    static HeadsetViewEventListener headsetViewEventListener;
    return &headsetViewEventListener;
}

bool HeadsetViewEventListener::OnDrag(UIView &view, const DragEvent &event)
{
    return true;
}

bool HeadsetViewEventListener::OnDragEnd(UIView &view, const DragEvent &event)
{
    if (event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) {
        if (strcmp(view.GetViewId(), HEADSET_CONN_VIEW) == 0) {
            NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
            NotificationManager::GetInstance()->StopNotify();
        } else if (strcmp(view.GetViewId(), HEADSET_POP_VIEW) == 0) {
            NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
            NotificationManager::GetInstance()->StopNotify();
        }
    }
    return true;
}

static void CmsisConnectFunc(void const *argument) {
}

bool HeadsetViewEventListener::OnClick(UIView &view, const ClickEvent &event)
{
    unused(event);
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "AlamNotify ClickListener %s", view.GetViewId());
    if (strcmp(view.GetViewId(), HEADSET_IMAGE_NO) == 0) {
        NotificationManager::GetInstance()->StopNotify();
    } else if (strcmp(view.GetViewId(), HEADSET_CONN_FAIL_BUTTON) == 0) {
        NotificationManager::GetInstance()->StopNotify();
    } else if (strcmp(view.GetViewId(), HEADSET_IMAGE_YES) == 0) {
        g_headsetConnView->Remove(g_buttonNo);
        g_headsetConnView->Remove(g_buttonYes);
        g_headsetConnView->Add(g_headsetConnectingLable);

        osThreadId_t id;
        osThreadAttr_t attr;
        attr.name = "CmsisConnectFunc";
        attr.attr_bits = 0U;
        attr.cb_mem = NULL;
        attr.cb_size = 0U;
        attr.stack_mem = NULL;
        attr.stack_size = TEST_TASK_STACK_SIZE;
        attr.priority = osPriorityAboveNormal;
        id = osThreadNew((osThreadFunc_t)CmsisConnectFunc, NULL, &attr);
        if (id == NULL) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "CmsisConnectFunc thread create failed");
            return false;
        }
    } else if (strcmp(view.GetViewId(), HEADSET_CANCEL) == 0) {
        NotificationManager::GetInstance()->StopNotify();
    } else if (strcmp(view.GetViewId(), HEADSET_DETAILS) == 0) {
        NotificationManager::GetInstance()->screenContainer_->SetVisible(false);
        NotificationManager::GetInstance()->StopNotify();
        NativeAbility::GetInstance().ChangeSlice(VIEW_SETTING, TransitionType::TRANSITION_ZOOM);
    }
    return true;
}
}  // namespace OHOS
