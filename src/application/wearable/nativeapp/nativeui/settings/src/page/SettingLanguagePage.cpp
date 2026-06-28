/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguagePage.cpp
 * Create: 2025-09-20
 */
 
#include "wearable_log.h"
#include "SlicePageFactory.h"
#include "PageTransitionMgr.h"
#include "NativeAbility.h"
#include "AppViewIDs.h"
#include "uiservice/ui_service.h"
#include "main/LoadImg.h"
#include "ui_resource_settings.h"
#include "settings/page/SettingLanguagePage.h"
#include "settings/model/SettingLanguageModel.h"
namespace OHOS {

REGIST_SLICE_PAGE(VIEW_SETTING, SettingPageId::SET_LANGUAGE_PAGE, SettingLanguagePage, false);
static constexpr uint8_t SETTINGS_ITEM_FONT_SIZE = 40;
static constexpr int16_t SETTINGS_RESIZE_SIZE_WIDTH = 100;
static constexpr int16_t SETTINGS_RESIZE_SIZE_HEIGHT = 60;
static constexpr int16_t SETTING_TITLE_POSITION_X = 187;
static constexpr int16_t SETTING_LIST_POSITION_X = 32;
static constexpr int16_t SETTING_LIST_POSITION_Y = 90;
static constexpr int16_t SETTING_SCROLLBLANKSIZE = 50;
static constexpr int16 MAX_SCR_DIS = 1000;
static constexpr int16 LIST_POS = 20;
static constexpr int16 SNUM_2 = 2;
SettingLanguagePage::SettingLanguagePage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::SettingLanguagePage");
}
 
SettingLanguagePage::~SettingLanguagePage()
{
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::~SettingLanguagePage");
}
 
void SettingLanguagePage::OnStart(void* data)
{
    title_ = new UILabel();
    if (title_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::OnStart::title_ is nullptr");
        return;
    }
    title_->SetPosition(SETTING_TITLE_POSITION_X, 0);
    title_->SetText(FontGlobalManager::GetInstance()->GetText(LANGUAGE_LANGUAGE));
    title_->Resize(SETTINGS_RESIZE_SIZE_WIDTH, SETTINGS_RESIZE_SIZE_HEIGHT);
    title_->SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTINGS_ITEM_FONT_SIZE);
    title_->SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    title_->SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    title_->SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    title_->SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    title_->SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
 
    contentList_ = new UIListNested();
    if (contentList_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::OnStart::contentList_ is nullptr");
        return;
    }
    contentList_->SetPosition(SETTING_LIST_POSITION_X, SETTING_LIST_POSITION_Y);
    contentList_->Resize(HORIZONTAL_RESOLUTION, 364);  // 364: height
    contentList_->SetScrollBlankSize(SETTING_SCROLLBLANKSIZE);
    contentList_->SetSelectPosition(HORIZONTAL_RESOLUTION / SNUM_2);  // middle position
    contentList_->SetDraggable(true);
    contentList_->SetThrowDrag(true);
    contentList_->EnableAutoAlign(true);
    contentList_->SetMaxScrollDistance(MAX_SCR_DIS);
    listAdapter_ = new SettingLanguageAdapterGroup();
    if (listAdapter_ == nullptr) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::OnStart::listAdapter_ is nullptr");
        return;
    }
    const SetLanguages *itemInfo = SettingLanguageModel::GetInstance().GetSetLanguagelistItems();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SettingLanguagePage::RefreshSetLanguageList");
    for (int i = 0; i < SettingLanguageModel::GetInstance().GetSettingListNum(); i++) {
        listAdapter_->AddListItem(itemInfo[i]);
    }
    listAdapter_->SetItemClickListener(this);
    contentList_->SetAdapter(listAdapter_);
    AddViewToPageContainer(title_);
    AddViewToPageContainer(contentList_);
}
 
bool SettingLanguagePage::OnDrag(UIView &view, const DragEvent &event)
{
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "SetLanguagePresenter::OnDrag");
    UNUSED(view);
    if ((event.GetDragDirection() == DragEvent::DIRECTION_LEFT_TO_RIGHT) && (event.GetDeltaX() > X_DRAG_OFFSET) &&
        (event.GetDeltaX() > (DOUBLES * MATH_ABS(event.GetDeltaY())))) {
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetLanguagePresenter::OnDrag ChangeSlice");
        uint16_t preSlice = NativeAbility::GetInstance().GetPreSliceId();
        NativeAbility::GetInstance().ChangeSlice(preSlice);
    }
    return false;
}
 
bool SettingLanguagePage::OnClick(UIView &view, const ClickEvent &event)
{
    SettingLanguageItem *settingItemView = static_cast<SettingLanguageItem *>(&view);
    LanguageId viewId = settingItemView->GetViewId();
    WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "SetLanguagePresenter::OnClick");
    if (viewId == CHINESE_LANGUAGE) {
        SettingLanguageModel::GetInstance().SetLanguageStyle(LanguageStyle::CHINESE);
    } else if (viewId == ENGLISH_LANGUAGE) {
        SettingLanguageModel::GetInstance().SetLanguageStyle(LanguageStyle::ENGLISH);
    }
    uint16_t preSlice = NativeAbility::GetInstance().GetPreSliceId();
    NativeAbility::GetInstance().ChangeSlice(preSlice);
    return true;
}
}  // namespace OHOS
