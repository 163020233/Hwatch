/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingDisplaySteadyOnPage
 * Create: 2025-06-01
 */
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "main/LoadImg.h"
#include "NativeAbility.h"
#include "ui_view_ids_setting.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/page/SettingDisplaySteadyOnPage.h"

namespace OHOS {
REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::DISPLAY_STEADY_ON_PAGE,
    SettingDisplaySteadyOnPage, false);

static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_X = 100;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_Y = 39;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_WIDTH = 254;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_HEIGHT = 53;
static constexpr int16_t SETTING_DISPLAY_STEADY_TITLE_FSIZE = 38;
static const char* SETTING_DISPLAY_STEADY_TITLE_TEXT = "长亮时刻";
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_X = 64;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_Y = 118;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_WIDTH = 321;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_HEIGHT = 218;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_FSIZE = 44;
static constexpr int16_t SETTING_DISPLAY_STEADY_PICKER_ITEM_HEIGHT = 84;

void SettingDisplaySteadyOnPage::OnStart(void* data)
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplaySteadyOnTimePage::OnStart title_ new fail");
        return;
    }
    title_->SetPosition(SETTING_DISPLAY_STEADY_TITLE_X, SETTING_DISPLAY_STEADY_TITLE_Y);
    title_->Resize(SETTING_DISPLAY_STEADY_TITLE_WIDTH, SETTING_DISPLAY_STEADY_TITLE_HEIGHT);
    title_->SetFont(BOLD_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_TITLE_FSIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetTextColor(Color::White());
    title_->SetText(SETTING_DISPLAY_STEADY_TITLE_TEXT);

    picker_ = new UIPicker();
    if (picker_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "SettingDisplaySteadyOnTimePage::OnStart picker_ new fail");
        return;
    }
    picker_->SetPosition(SETTING_DISPLAY_STEADY_PICKER_X, SETTING_DISPLAY_STEADY_PICKER_Y);
    picker_->Resize(SETTING_DISPLAY_STEADY_PICKER_WIDTH, SETTING_DISPLAY_STEADY_PICKER_HEIGHT);
    picker_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
    picker_->SetBackgroundFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_PICKER_FSIZE);
    picker_->SetHighlightFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_DISPLAY_STEADY_PICKER_FSIZE);
    picker_->SetItemHeight(SETTING_DISPLAY_STEADY_PICKER_ITEM_HEIGHT);
    picker_->SetTextColor(SETTING_LIST_DEFAULT_BG_COLOR, Color::White());
    picker_->SetIntercept(true);
    const char** timeAll = SettingDisplayModel::GetInstance().GetSteadyOnTimeAllStr();
    uint8_t size = SettingDisplayModel::GetInstance().GetSteadyOnTimeAllStrSize();
    uint8_t selectIndex = SettingDisplayModel::GetInstance().GetSteadyOnTimeIndex();
    picker_->SetValues(timeAll, size);
    picker_->SetSelected(selectIndex);

    button_ = new UIImageView();
    if (button_ == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "new button_ ERROR");
        return;
    }
    LOADIMG::LoadImageViewImg(button_, SETTING_IMAGE, IMAGE_OK_DOWN);
    int posX = (HORIZONTAL_RESOLUTION - button_->GetWidth()) / 2;
    int posY = HORIZONTAL_RESOLUTION - button_->GetHeight();
    button_->SetPosition(posX, posY);
    button_->SetViewId(UIVIEW_SETTING_UIBUTTON_DISPLAY_STEADY_ON_CONFIRM_BUTTON);
    button_->SetOnClickListener(this);
    button_->SetTouchable(true);

    AddViewToPageContainer(title_);
    AddViewToPageContainer(picker_);
    AddViewToPageContainer(button_);
}

void SettingDisplaySteadyOnPage::OnStop()
{
    if (title_ != nullptr) {
        delete title_;
        title_ = nullptr;
    }
    if (picker_ != nullptr) {
        delete picker_;
        picker_ = nullptr;
    }
    if (button_ != nullptr) {
        delete button_;
        button_ = nullptr;
    }
    ImageCacheManager::GetInstance().UnloadOneInMultiRes(IMAGE_OK_DOWN, SETTING_IMAGE);
}

bool SettingDisplaySteadyOnPage::OnClick(UIView &view, const ClickEvent &event)
{
    if (strcmp(view.GetViewId(), UIVIEW_SETTING_UIBUTTON_DISPLAY_STEADY_ON_CONFIRM_BUTTON) == 0) {
        uint8_t index = picker_->GetSelected();
        SettingDisplayModel::GetInstance().SetSteadyOnTimeIndex(index);
        NativeAbility::GetInstance().BackToPrevSlicePage();
    }
    return true;
}
}
