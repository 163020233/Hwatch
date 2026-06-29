/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * Description: Breath Ease
 * Author: wangle
 * Create: 2025-4-23
 */
#include "uiservice/ui_language.h"
#include "ui_resource_string.h"
#include "font/font_global_manager.h"
#include "wearable_log.h"

namespace OHOS {
static constexpr const char *BIN_LANG_EN = "/music/font/en.lang";
static constexpr const char *BIN_LANG_ZH = "/music/font/zh.lang";

const char* UILanguage::GetText(uint16_t textId)
{
    return nullptr;
}
void UILanguage::SetLanguage(uint8_t id)
{
}
void UILanguage::UpdateLanguage()
{
}
bool UILanguage::RegisterGlobalManager()
{
    return false;
}
}