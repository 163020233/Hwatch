/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "NativeRegisterManager.h"
#include "NativeLauncher.h"
#include "SliceProxy.h"
#include "AbilitySliceProxy.h"
#include "wearable_log.h"
#include "phonemenu/PhoneMenuCallerLogModel.h"
#include "activity/ActivityModel.h"
#include "player/PlayersModel.h"
#include "offlinevoice/VocassistModel.h"
#include "screensetting/ScreenModels.h"
#include "settings/model/SettingBtHeadset.h"
#include "graphic_hardware_config.h"
#include "graphic_service.h"
#include "common/graphic_startup.h"
#ifdef JS_ENABLE
#include "product_adapter.h"
#ifdef ENABLE_MODULE_CALENDAR
#include "event_id_manager.h"
#endif
#endif
#include "RecentManager.h"
#include "PageTransitionMgr.h"
#include "settings/model/SettingDisplayModel.h"
#include "settings/model/SettingBluetoothModel.h"
#include "UiConfig.h"
#include "kv_store.h"
#include "settings/model/SettingLanguageModel.h"
#include "RotationManager.h"
#include "font/ui_font.h"

namespace OHOS {
#if !defined(_WIN32)
static const char *BIN_LANG_EN =  "/user/res/lang/en.lang";
static const char *BIN_LANG_ZH =  "/user/res/lang/zh.lang";
#else
static const char *BIN_LANG_EN =  "res/res/lang/en.lang";
static const char *BIN_LANG_ZH =  "res/res/lang/zh.lang";
#endif
static const char* resFiles[] = {BIN_LANG_ZH, BIN_LANG_EN};
static const int KV_STORE_LEN = 10;
NativeLauncher::NativeLauncher() {}
NativeLauncher::~NativeLauncher() {}

NativeLauncher &NativeLauncher::GetInstance(void)
{
    static NativeLauncher instance;
    return instance;
}

void NativeLauncher::InitNativeLauncher()
{
#ifdef JS_ENABLE
    ACELite::ProductAdapter::SetScreenSize(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION);
    ACELite::ProductAdapter::SetDefaultFontStyle(
        DEFAULT_VECTOR_FONT_FILENAME, ACELite::ProductAdapter::GetDefaultFontSize());
#endif
    GraphicStartUp::InitFontEngine(MEM_POOL_UI_FONT, MEM_POOL_UI_FONT_SIZE,
        VECTOR_FONT_DIR, DEFAULT_VECTOR_FONT_FILENAME);
    UIFont::GetInstance()->RegisterFontInfo(BOLD_VECTOR_FONT_FILENAME);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_LAUNCHER_FWK, "NativeLauncher::InitNativeLauncher");
    // 初始化所有预加载slice
    NativeRegisterManager::GetInstance().InitAllNativesSlices();
    PhoneMenuCallerLogModel::GetInstance()->InitPhoneService();
    PlayersModel::GetInstance()->AudioInit();
    ActivityModel::GetInstance()->Init();
    ScreenModels::GetInstance()->InitKvStorage();
#ifdef ENABLE_MODULE_CALENDAR
    ACELite::InitEventMaps();
#endif
    char str[KV_STORE_LEN];
    UtilsGetValue("current_language", str, KV_STORE_LEN);
    int defaultLanguage = strtol(str, nullptr, 0);
    FontGlobalManager::GetInstance()->
    RegisterLanguageInfo(resFiles, (sizeof(resFiles) / sizeof(char*)), defaultLanguage);
    SettingLanguageModel::GetInstance().InitSetLanguages();
    SettingBluetoothModel::GetInstance()->Init();
    // 初始化默认首页
    ScreenModels::GetInstance()->PreLoadAppimages();
    RotationManager::GetInstance().RegisterRotationListener(&NativeAbility::GetInstance());
    NativeAbility::GetInstance().SetDefaultSliceId(VIEW_MAIN_SAMPLE);
    RecentManager::GetInstance()->SetDirPath(APP_RECENT_PATH);
    RecentManager::GetInstance()->AddToBlackList(VIEW_AOD);
    RecentManager::GetInstance()->AddToBlackList(VIEW_RECENT_APP);
    RecentManager::GetInstance()->AddToBlackList(VIEW_APPLIST);
    RecentManager::GetInstance()->AddToBlackList(VIEW_MAIN_SAMPLE);
    RecentManager::GetInstance()->SetMaxSize(5); // 5: max number of recent apps.
    PageTransitionMgr::GetInstance().SetRecentMinPeriod(1000); // 1000: min period
    SettingDisplayModel::GetInstance().InitDisplayModel();
}

}
