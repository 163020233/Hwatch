/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingConnectionOpen
 * Created: 2025-06-05
 */

#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "main/LoadImg.h"
#include "ui_resource_settings.h"
#include "ui_view_ids_setting.h"
#include "settings/model/SettingBluetoothModel.h"
#include "settings/page/SettingBluetoothPage.h"
#include "settings/page/SettingConnectionOpen.h"

namespace OHOS {
static constexpr int16_t SETTING_IMG_X = 85;
static constexpr int16_t SETTING_IMG_Y = -50;
static constexpr int16_t SETTING_WATCH_Y = 180;
static constexpr int16_t SETTING_WATCH_HEIGHT = 45;
static constexpr int16_t SETTING_WATCH_FONT = 36;
static constexpr int16_t SETTING_CONTENT_Y = 230;
static constexpr int16_t SETTING_CONYENT_HEIGHT = 40;
static SettingConnectionOpen *g_pSettingConnectionOpen{nullptr};

void SettingConnectionOpen::OnCreateView(void* data)
{
    InitConnectionOpenFragment();
    g_pSettingConnectionOpen = this;
}

void SettingConnectionOpen::OnDestroyView()
{
    fragmentView_.RemoveAll();
    if (img_ != nullptr) {
        delete img_;
        img_ = nullptr;
    }
    if (watchNameLabel_ != nullptr) {
        delete watchNameLabel_;
        watchNameLabel_ = nullptr;
    }
    if (connectStatusLabel_ != nullptr) {
        delete connectStatusLabel_;
        connectStatusLabel_ = nullptr;
    }
    if (confirmButton_ != nullptr) {
        delete confirmButton_;
        confirmButton_ = nullptr;
    }
}

void SettingConnectionOpen::InitConnectionOpenFragment()
{
    fragmentView_.SetTouchable(true);
    fragmentView_.SetDraggable(true);
    fragmentView_.SetOnDragListener(this);
    img_ = new UIImageView();
    if (img_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitConnectionOpenFragment img_ new fail");
        return;
    }
    LOADIMG::LoadImageViewImg(img_, SETTING_IMAGE, IMAGE_OK);
    img_->SetPosition(SETTING_IMG_X, SETTING_IMG_Y);

    watchNameLabel_ = new UILabel();
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitConnectionOpenFragment watchNameLabel_ new fail");
        return;
    }
    watchNameLabel_->SetPosition(0, SETTING_WATCH_Y, RESOLUTION_WIDTH, SETTING_WATCH_HEIGHT);
    watchNameLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    watchNameLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_WATCH_FONT);
    watchNameLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    watchNameLabel_->SetText(SettingBluetoothModel::GetInstance()->GetWatchName());

    connectStatusLabel_ = new UILabel();
    if (connectStatusLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "InitConnectionOpenFragment connectStatusLabel_ new fail");
        return;
    }
    connectStatusLabel_->SetPosition(0, SETTING_CONTENT_Y, RESOLUTION_WIDTH, SETTING_WATCH_HEIGHT);
    connectStatusLabel_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    connectStatusLabel_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_CONYENT_HEIGHT);
    connectStatusLabel_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    connectStatusLabel_->SetText("连接已打开");

    confirmButton_ = new UIImageView();
    if (confirmButton_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new confirmButton_ ERROR");
        return;
    }
    LOADIMG::LoadImageViewImg(confirmButton_, SETTING_IMAGE, IMAGE_OK_DOWN);
    int posX = (HORIZONTAL_RESOLUTION - confirmButton_->GetWidth()) / 2;
    int posY = HORIZONTAL_RESOLUTION - confirmButton_->GetHeight();
    confirmButton_->SetPosition(posX, posY);
    confirmButton_->SetViewId(UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_OPEN_CONFIRM_BUTTON);
    confirmButton_->SetOnClickListener(this);
    confirmButton_->SetTouchable(true);

    fragmentView_.Add(img_);
    fragmentView_.Add(watchNameLabel_);
    fragmentView_.Add(connectStatusLabel_);
    fragmentView_.Add(confirmButton_);
}

void SettingConnectionOpen::RefreshFragment()
{
    if (watchNameLabel_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "RefreshFragment fail");
        return;
    }
    watchNameLabel_->SetText(SettingBluetoothModel::GetInstance()->GetWatchName());
}

bool SettingConnectionOpen::OnClick(UIView &view, const ClickEvent &event)
{
    UNUSED(event);
    if (strcmp(view.GetViewId(), UIVIEW_SETTING_UIIMAGEVIEW_CONNECT_OPEN_CONFIRM_BUTTON) == 0) {
        fragmentView_.SetVisible(false);
        SettingBluetoothPage::GetInstance()->ChangeView(BT_MAIN_PAGE);
        SettingBluetoothModel::GetInstance()->SetConnectFragmentStatus(false);
        SettingBluetoothPage::GetInstance()->StartBtScan();
    }
    return true;
}

bool SettingConnectionOpen::OnDragStart(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectionOpen::OnDrag(UIView& view, const DragEvent& event)
{
    return true;
}

bool SettingConnectionOpen::OnDragEnd(UIView& view, const DragEvent& event)
{
    return true;
}
}
