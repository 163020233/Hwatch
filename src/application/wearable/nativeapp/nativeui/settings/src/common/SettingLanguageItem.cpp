/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageItem.cpp
 * Create: 2025-09-20
 */
 
#include "wearable_log.h"
#include "settings/common/SettingLanguageItem.h"
 
namespace OHOS {
static constexpr int16 SETTING_ICON_X = 20;
static constexpr int16 NUM_22 = 22;
static constexpr int16_t SETTING_ITEM_BUTTON_HEIGHT = 100;
static constexpr int16_t SETTING_ITEM_LABEL_WIDTH = 155;
static constexpr int16_t SETTING_ITEM_LABEL_HEIGHT = 100;
static constexpr int16 SETTING_ITEM_FONT_SIZE = 35;
static constexpr int16_t SETTINGS_RESIZE_SIZE = 100;
static constexpr int16_t SETTING_SICON_Y = 15;
static constexpr int16 NUMS_2 = 2;
static constexpr int16 NUMS_50 = 50;
 
SettingLanguageItem::SettingLanguageItem()
{
    SetPosition(0, 0, HORIZONTAL_RESOLUTION, SETTING_ITEM_BUTTON_HEIGHT);
    SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    SetStyle(STYLE_BACKGROUND_OPA, UN_OPACITY);
    SetTouchable(true);
    SetUpChild();
    Add(&sLabel);
}
 
SettingLanguageItem::~SettingLanguageItem()
{
    Remove(&sLabel);
}
 
void SettingLanguageItem::SetLanguagesInfo(const SetLanguages itemInfo)
{
    sItemInfo = itemInfo;
    sLabel.SetText(itemInfo.label);
}
 
void SettingLanguageItem::SetUpChild()
{
    sLabel.SetPosition(SETTING_ITEM_LABEL_WIDTH, 0);
    sLabel.Resize(SETTING_ITEM_LABEL_WIDTH, SETTING_ITEM_LABEL_HEIGHT);
    sLabel.SetFont(DEFAULT_VECTOR_FONT_FILENAME, SETTING_ITEM_FONT_SIZE);
    sLabel.SetAlign(TEXT_ALIGNMENT_CENTER, TEXT_ALIGNMENT_CENTER);
    sLabel.SetStyle(STYLE_TEXT_OPA, OPA_OPAQUE);
    sLabel.SetStyle(STYLE_TEXT_COLOR, Color::White().full);
    sLabel.SetStyle(STYLE_BACKGROUND_COLOR, Color::Black().full);
    sLabel.SetStyle(STYLE_BACKGROUND_OPA, OPA_OPAQUE);
}
 
LanguageId SettingLanguageItem::GetViewId()
{
    return sItemInfo.viewId;
}
 
}  // namespace OHOS