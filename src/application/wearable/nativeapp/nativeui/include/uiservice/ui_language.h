/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ui_language
 * Created: 2025-12-15
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H
#include <cstdint>
#include "main/AppGroupView.h"

namespace OHOS {
class UILanguage {
public:
    static const char* GetText(uint16_t textId);
    static void SetLanguage(uint8_t id);
    static void UpdateLanguage();
private:
    static bool RegisterGlobalManager();
};
}
#endif