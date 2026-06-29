/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: SettingLanguageModel.cpp
 * Create: 2025-09-20
 */

#include <cstdio>
#include "kv_store.h"
#include "kvstore_env.h"
#include "securec.h"
#include "global.h"
#include "bundlems_slite_client.h"
#include "font/font_global_manager.h"
#include "cstdlib"
#include "utils_list.h"
#include "wearable_log.h"
#include "settings/model/SettingLanguageModel.h"
#include "applist/ApplistModel.h"
#include "components/root_view.h"
#include "gt_bundle_manager_service.h"
namespace OHOS {
static const int KV_STORE_LEN = 10;
static const int STYLE_VALUE_LEN = 32;

SettingLanguageModel::SettingLanguageModel()
{}

SettingLanguageModel::~ SettingLanguageModel()
{}

SettingLanguageModel& SettingLanguageModel::GetInstance()
{
    static  SettingLanguageModel instance;
    return instance;
}

void  SettingLanguageModel::SetLanguageStyle(LanguageStyle style)
{
#if !defined(_WIN32)
    if (style == CHINESE) {
        GLOBAL_ConfigLanguage("zh-CN");
        nativeLanguage = 0;
    } else if (style == ENGLISH) {
        GLOBAL_ConfigLanguage("en-US");
        nativeLanguage = 1;
    }
    // page not support update name
    FontGlobalManager::GetInstance()->SetCurrentLangId(nativeLanguage);
    char str[KV_STORE_LEN];
    memset_s(str, KV_STORE_LEN, 0, KV_STORE_LEN);
    sprintf_s(str, KV_STORE_LEN, "%d", nativeLanguage);
    UtilsSetValue("current_language", str);
    OHOS::GtManagerService::GetInstance().ScanPackages(ScanPackagesResultCallback);
#endif
}

void  SettingLanguageModel::ScanPackagesResultCallback(const uint8_t isSuccess, const void *resultMessage)
{
    if (isSuccess == 0) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Scan Packages Succeeded.\n");
        OHOS::ApplistModel::GetInstance().SetInitFlag(false);
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Set InitFlag false Succeeded\n");
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Scan Packages Failed.\n");
        if (resultMessage != nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "[ERROR] %s\n", reinterpret_cast<const char *>(resultMessage));
        }
    }
}


void  SettingLanguageModel::InitSetLanguages()
{
    int index = 0;
    languageSettings[index++] = {CHINESE_LANGUAGE, "简体中文"};
    languageSettings[index++] = {ENGLISH_LANGUAGE, "English"};
    usedLen = index;
}

const SetLanguages* SettingLanguageModel::GetSetLanguagelistItems() const
{
    return languageSettings;
}

uint8_t  SettingLanguageModel::GetSettingListNum() const
{
    return usedLen;
}

}  // namespace OHOS