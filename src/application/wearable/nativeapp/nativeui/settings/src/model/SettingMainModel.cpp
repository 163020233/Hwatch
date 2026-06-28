/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingMainModel
 * Create: 2025-06-01
 */
#include "Slice.h"
#include "UiConfig.h"
#include "settings/common/SettingCommon.h"
#include "settings/model/SettingMainModel.h"
#include "font/font_global_manager.h"

namespace OHOS {
SettingMainModel::SettingMainModel()
{
    std::vector<SettingUnionItemData> tempUnionData = {{
        PlainTextItemData{"设置"},
        IconTextNavigationItemData{SettingPageId::BLUETOOTH_PAGE, IMAGE_MAIN_LIST_BLUETOOTH, "蓝牙",
            nullptr, true, SETTING_LIST_BLUETOOTH_BG_COLOR, SETTING_LIST_BLUETOOTH_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DISPLAY_PAGE, IMAGE_MAIN_LIST_DISPLAY, "显示与亮度",
            nullptr, true, SETTING_LIST_DISPLAY_BG_COLOR, SETTING_LIST_DISPLAY_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DESKTOP_PAGE, IMAGE_MAIN_LIST_DESKTOP, "表盘与桌面",
            nullptr, true, SETTING_LIST_DESKTOP_BG_COLOR, SETTING_LIST_DESKTOP_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::POWER_PAGE, IMAGE_MAIN_LIST_POWER, "电量",
            nullptr, true, SETTING_LIST_POWER_BG_COLOR, SETTING_LIST_POWER_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::CARD_PAGE, IMAGE_MAIN_LIST_CARD, "卡片",
            nullptr, true, SETTING_LIST_CARD_BG_COLOR, SETTING_LIST_CARD_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SYSTEM_PAGE, IMAGE_MAIN_LIST_SYSTEM, "系统和升级",
            nullptr, true, SETTING_LIST_SYSTEM_BG_COLOR, SETTING_LIST_SYSTEM_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SET_LANGUAGE_PAGE, IMAGE_MAIN_LIST_ABOUT, "语言设置",
            nullptr, true, SETTING_LIST_ABOUT_BG_COLOR, SETTING_LIST_ABOUT_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::ABOUT_PAGE, IMAGE_MAIN_LIST_ABOUT, "关于",
            nullptr, true, SETTING_LIST_ABOUT_BG_COLOR, SETTING_LIST_ABOUT_BG_COLOR_OPA},
    }};
    mainUnionData_.swap(tempUnionData);
}

void SettingMainModel::UpdateMainUnionData()
{
    std::vector<SettingUnionItemData> tempUnionData = {{
        PlainTextItemData{FontGlobalManager::GetInstance()->GetText(SETTING_LANGUAGE)},
        IconTextNavigationItemData{SettingPageId::BLUETOOTH_PAGE, IMAGE_MAIN_LIST_BLUETOOTH,
            FontGlobalManager::GetInstance()->GetText(BLUETOOTH_LANGUAGE),
            nullptr, true, SETTING_LIST_BLUETOOTH_BG_COLOR, SETTING_LIST_BLUETOOTH_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DISPLAY_PAGE, IMAGE_MAIN_LIST_DISPLAY,
            FontGlobalManager::GetInstance()->GetText(DISPLAY_LANGUAGE),
            nullptr, true, SETTING_LIST_DISPLAY_BG_COLOR, SETTING_LIST_DISPLAY_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::DESKTOP_PAGE,  IMAGE_MAIN_LIST_DESKTOP,
            FontGlobalManager::GetInstance()->GetText(DESKTOP_LANGUAGE),
            nullptr, true, SETTING_LIST_DESKTOP_BG_COLOR, SETTING_LIST_DESKTOP_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::POWER_PAGE, IMAGE_MAIN_LIST_POWER,
            FontGlobalManager::GetInstance()->GetText(POWER_LANGUAGE),
            nullptr, true, SETTING_LIST_POWER_BG_COLOR, SETTING_LIST_POWER_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::CARD_PAGE, IMAGE_MAIN_LIST_CARD,
            FontGlobalManager::GetInstance()->GetText(CARD_LANGUAGE),
            nullptr, true, SETTING_LIST_CARD_BG_COLOR, SETTING_LIST_CARD_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SYSTEM_PAGE, IMAGE_MAIN_LIST_SYSTEM,
            FontGlobalManager::GetInstance()->GetText(SYSTEM_LANGUAGE),
            nullptr, true, SETTING_LIST_SYSTEM_BG_COLOR, SETTING_LIST_SYSTEM_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::SET_LANGUAGE_PAGE, IMAGE_MAIN_LIST_LANGUAGE,
            FontGlobalManager::GetInstance()->GetText(SET_LANGUAGE_LANGUAGE),
            nullptr, true, SETTING_LIST_LANGUAGE_BG_COLOR, SETTING_LIST_LANGUAGE_BG_COLOR_OPA},
        IconTextNavigationItemData{SettingPageId::ABOUT_PAGE, IMAGE_MAIN_LIST_ABOUT,
            FontGlobalManager::GetInstance()->GetText(SABOUT_LANGUAGE),
            nullptr, true, SETTING_LIST_ABOUT_BG_COLOR, SETTING_LIST_ABOUT_BG_COLOR_OPA},
    }};
    mainUnionData_.swap(tempUnionData);
}

std::vector<SettingUnionItemData>& SettingMainModel::GetMainUnionData()
{
    return mainUnionData_;
}

UIView* SettingMainModel::OnSimpleListCreate(uint8_t type)
{
    if (type == SettingUnionItemType::PLAIN_TEXT) {
        return new PlainTextItemView();
    } else if (type == SettingUnionItemType::ICON_TEXT_TOGGLE) {
        return new IconTextToggleItemView();
    } else if (type == SettingUnionItemType::ICON_TEXT_NAVIGATION) {
        return new IconTextNavigationItemView();
    }
    return nullptr;
}
}
