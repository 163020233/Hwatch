/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: Provides ohos sample. \n
 * Author: CompanyName \n
 * History: \n
 * 2022-11-17， Create file. \n
 */
#include <unistd.h>
#include <cstdlib>
#include "sys/time.h"
#include "time.h"
#include "common_def.h"
#include "wearable_log.h"
#include "at_ohos_process_cpp/ohosfwk_at_process.h"
#include "bundlems_slite_client.h"
#include "abilityms_slite_client.h"
#include "gt_bundle_manager_service.h"
#include "uiservice/ui_service.h"
#include "sign_mode.h"
#include "NativeAbility.h"
#include "ability_manager.h"
#include "js_fwk_common.h"
#include "js_ability.h"
#include "global.h"
#include "gt_bundle_manager_service.h"
#include "utils_file.h"
#include <securec.h>
#include "hal_file.h"
#include "applist/ApplistModel.h"
#include "bundle_manager.h"
#include "msg_center.h"
#ifdef SUPPORT_OHOSFWK
#ifndef CFG_NORFLASH_ONLY
#include "alarmclocktimerconfig/AlarmClockConfig.h"
#endif
#endif
#ifdef SUPPORT_AT_LOCATION
#include "location_sample.h"
#include <cmsis_os.h>
#endif
#ifdef SUPPORT_AT_SENSOR
#include "sensor_sample.h"
#endif
#ifdef SUPPORT_AT_PERMISSION
#include "hal_pms.h"
#endif
#ifdef CONFIG_NATIVEAPP_TEST
#include "monkey/MonkeyModel.h"
#endif

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define OHOSFWK_MAX_ARGC            8
#define OHOSFWK_MAX_ARGV_LEN        64

#define OHOSFWK_SUCCESS             0
#define OHOSFWK_FAILED              (-1)
constexpr uint32_t MAX_OUTTIMES = 60000;
constexpr uint32_t DEFAULT_TIME = 500;
constexpr uint8_t ARGC_ONE = 1;
constexpr uint8_t ARGC_TOW = 2;
constexpr uint8_t ARGC_THREE = 3;
constexpr uint32_t DEFAULT_SECOND = 1000;
#define ARG_NUM_QUERY_APP_PERMISSION    1
#define ARG_NUM_CHECK_APP_PERMISSION    2
#define ARG_NUM_GRANT_APP_PERMISSION    2
#define MAX_PAGE 100

constexpr int16_t DEFAULT_TIME_ZONE = 8 * 60;
constexpr int8_t MINUTES_PER_HOUR = 60;
constexpr int8_t SECONDS_PER_MINUTE = 60;
constexpr int16_t USEC_TO_MILLI = 1000;
constexpr int8_t TIME_MAX = 64;
constexpr int8_t DECIMAL = 10;
constexpr int8_t TIME_YEAR_OFFSET = 70;
constexpr int16_t MIN_TIMEZONE_OFFSET = -12 * 60;
constexpr int16_t MAX_TIMEZONE_OFFSET = 12 * 60;

enum {
    UNDEFINED_STATE = -1,
    INIT_STATE, // 0
    READY_STATE,
    SHOW_STATE,
    BACKGROUND_STATE,
    DESTROY_STATE,
    END_STATE
};

static void ReceiveCallback(const uint8_t resultCode, const void *resultMessage)
{
    BundleInstallMsg *installMsg = (BundleInstallMsg *)resultMessage;
    if (resultCode == 0) {
        BundleData bundleData;
        memset_s(&bundleData, sizeof(BundleData), 0, sizeof(BundleData));
        strncpy_s(bundleData.bundleName, BUNDLE_NAME_LEN, installMsg->bundleName, BUNDLE_NAME_LEN);

        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bms resultCode = 0x%x", resultCode);
        WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bms bundleName = %s", bundleData.bundleName);

        if (installMsg->installState == BUNDLE_INSTALL_OK) {
            strncpy_s(bundleData.label, BUNDLE_PATH_LEN, installMsg->label, BUNDLE_PATH_LEN);
            strncpy_s(bundleData.smallIconPath, BUNDLE_PATH_LEN, installMsg->smallIconPath, BUNDLE_PATH_LEN);
            strncpy_s(bundleData.bigIconPath, BUNDLE_PATH_LEN, installMsg->bigIconPath, BUNDLE_PATH_LEN);
            SendMsgToUIService(JS_INSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
        } else if (installMsg->installState == BUNDLE_UNINSTALL_OK) {
            SendMsgToUIService(JS_UNINSTALL_SUCESS_TO_UI, 0, &bundleData, sizeof(BundleData));
        }
    } else {
        if(installMsg->installState == BUNDLE_INSTALL_FAIL) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bms install resultCode = 0x%x", resultCode);
        } else if (installMsg->installState == BUNDLE_UNINSTALL_FAIL) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "bms uninstall resultCode = 0x%x", resultCode);
        } else {
            WEARABLE_LOGD(WEARABLE_LOG_MODULE_APP, "bms resultCode = 0x%x", resultCode);
        }
    }
    AdapterFree(installMsg);
}

uint32_t OhosfwkATCmdAmExitJsApp(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
    ElementName *elementName = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Stop %s\n", elementName->bundleName);
    OHOS::AbilitySlite::AbilityMsClient::GetInstance().ForceStop(elementName->bundleName);
    FreeElement(elementName);
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdBmInstall(uint8_t argc, char *argv[])
{
    if (argc < 1 || argc > 2 || argv[0] == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Installing Package Failed\n");
        return ERRCODE_FAIL;
    }
    InstallParam installParam = { .installLocation = 1, .keepData = true };
    if (argc == 2 && argv[1] != nullptr && strcmp("rmdata", (char*)argv[1]) == 0) {
        installParam.keepData = false;
    }
    OHOS::BundleMsClient::GetInstance().Install((char*)argv[0], &installParam, ReceiveCallback);

    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdBmUninstall(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Uninstalling Package Failed\n");
        return ERRCODE_FAIL;
    }

    char *bundleName = (char *)argv[0];
    ElementName* topAbility = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();

    if ((topAbility != nullptr) && (strcmp(topAbility->bundleName, bundleName) == 0)) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "js app is active, first need terminate");
        int token = 0;
        OHOS::AbilitySlite::AbilityMsClient::GetInstance().TerminateAbility(token);
    }

    InstallParam installParam = { .installLocation = 1, .keepData = false };
    OHOS::BundleMsClient::GetInstance().Uninstall(bundleName, &installParam, ReceiveCallback);
    FreeElement(topAbility);
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdBmSet(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }

    char *enable = (char *)argv[0];
    if (strcmp(enable, "enable") == 0) {
        OHOS::SignMode::GetInstance().SetSignMode(true);
    } else if (strcmp(enable, "disable") == 0) {
        OHOS::SignMode::GetInstance().SetSignMode(false);
    } else {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "input failed!");
    }
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdAmChangeNativeSlice(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    char *stringAppViewId = (char *)argv[0];
    uint16_t AppViewId = static_cast<uint16_t>(std::stoi(stringAppViewId));
    OHOS::NativeAbility::GetInstance().ChangeSlice(AppViewId);
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdAmStartJsApp(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Set Parameter Failed\n");
        return ERRCODE_FAIL;
    }
    Want *want = new Want();
    memset_s(want, sizeof(Want), 0, sizeof(Want));

    ElementName startElement = {};
    startElement.abilityName = (char *)"default";
    startElement.bundleName = (char *)argv[0];
    startElement.deviceId = nullptr;
    SetWantElement(want, startElement);
    StartAbility(want);
    ClearWant(want);
    delete want;
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdDfxJsMemoryInfo(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
    OHOS::ACELite::DumpPrintMemoryInfo();
    return ERRCODE_SUCC;
}

void ScanPackagesResultCallback(const uint8_t resultCode, const void* resultMessage)
{
    if (resultCode == 0) {
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

uint32_t OhosfwkATCmdChangeLanguage(uint8_t argc, char *argv[])
{
    if (argc != 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Language Setting Failed\n");
        return ERRCODE_FAIL;
    }

    char *languageRegion = (char *)argv[0];
    if (strcmp(languageRegion, "get") == 0) {
        char language[4] = {0};
        GLOBAL_GetLanguage(language, MAX_LANGUAGE_LENGTH);
        printf("language: %s\n", language);
        return ERRCODE_SUCC;
    }
    
    GLOBAL_ConfigLanguage(languageRegion);

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "Language changed to %s\n", languageRegion);

    OHOS::BundleMsClient::GetInstance().ScanPackages(ScanPackagesResultCallback);
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdBmGetAppInfo(uint8_t argc, char *argv[])
{
    char *bundleName = argv[0];
    if (bundleName == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleName!\n");
        return ERRCODE_FAIL;
    }

    BundleInfo bundleInfo;
    int ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(bundleName, 0, &bundleInfo);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleInfo!");
    } else {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
        "-----------------AppInfo----------------\n"
        "bundleName: %s\n"
        "appId: %s\n"
        "label: %s\n"
        "versionCode: %d\n"
        "targetApi: %d\n"
        "isSystemApp: %s\n"
        "compatibleApi: %d\n"
        "versionName: %s\n"
        "vendor: %s\n"
        "numOfModule: %d",
            bundleInfo.bundleName,
            bundleInfo.appId,
            bundleInfo.label,
            bundleInfo.versionCode,
            bundleInfo.targetApi,
            bundleInfo.isSystemApp ? "true" : "false",
            bundleInfo.compatibleApi,
            bundleInfo.versionName,
            bundleInfo.vendor,
            bundleInfo.numOfModule);

        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
        "bigIconPath: %s\n"
        "codePath: %s\n"
        "dataPath: %s\n"
        "--------------AppInfo End---------------\n",
            bundleInfo.bigIconPath,
            bundleInfo.codePath,
            bundleInfo.dataPath);
    }
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdBmGetAppList(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
    BundleInfo *bundleInfos = nullptr;
    int32_t count = 0;
    int ret = OHOS::BundleMsClient::GetInstance().GetBundleInfos(0, &bundleInfos, &count);
    if (ret != OHOSFWK_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "Get bundleInfos Failed\n");
    } else {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList Start --------------------\n");
        for (int32_t i = 0; i < count; i++) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                "App:%d-----------------\n"
                "label:            %s\n"
                "bundleName:       %s\n",
                i+1,
                bundleInfos[i].label,
                bundleInfos[i].bundleName);
        }
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "\n"
            "----------------- AppList End -----------------------\n");
    }
    if (bundleInfos != nullptr) {
        for (int32_t i = 0; i < count; i++) {
            ClearBundleInfo(bundleInfos + i);
        }

        if (bundleInfos != nullptr) {
            AdapterFree(bundleInfos);
            bundleInfos = nullptr;
        }
    }
    return ERRCODE_SUCC;
}

static const char *StateInfoToString(int8_t pageInfo)
{
    switch (pageInfo) {
        case UNDEFINED_STATE:
            return "-1 - UNDEFINED_STATE";
        case INIT_STATE:
            return "0 - INIT_STATE";
        case READY_STATE:
            return "1 - READY_STATE";
        case SHOW_STATE:
            return "2 - SHOW_STATE";
        case BACKGROUND_STATE:
            return "3 - BACKGROUND_STATE";
        case DESTROY_STATE:
            return "4 - DESTROY_STATE";
        case END_STATE:
            return "5 - END_STATE";
        default:
            return "Can not get state!";
    }
}

uint32_t OhosfwkATCmdAmDfx(uint8_t argc, char *argv[])
{
    if (argc < 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD AmDfx Setting Failed\n");
        return ERRCODE_FAIL;
    }
    char *info = argv[0];
    if (info == nullptr) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get info!\n");
        return ERRCODE_FAIL;
    }

    if (strcmp(info, "dump") == 0) {
        ElementName *elementName = OHOS::AbilitySlite::AbilityMsClient::GetInstance().GetTopAbility();
        if (elementName == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get elementName!\n");
            return ERRCODE_FAIL;
        }
        if (elementName->bundleName == nullptr) {
            WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "failed to get bundleName!\n");
            return ERRCODE_FAIL;
        }
        char *currentBundleName = elementName->bundleName;

        BundleInfo bundleInfo;
        int ret = OHOS::BundleMsClient::GetInstance().GetBundleInfo(currentBundleName, 0, &bundleInfo);
        if (ret != OHOSFWK_SUCCESS) {
            PRINT("Native App: %s \r\n", currentBundleName);
            PRINT("Can not provide BundleInfo \r\n");
            return ERRCODE_SUCC;
        }
        int8_t pageInfo = OHOS::ACELite::GetJsPageStateInfo();
        const char *pageInfoLog = StateInfoToString(pageInfo);
        PRINT("-------------PageState------------- \r\n");
        PRINT("label: %s \r\n", bundleInfo.label);
        PRINT("bundleName: %s \r\n", bundleInfo.bundleName);
        PRINT("pageState: %s \r\n", pageInfoLog);
        PRINT("-----------PageState End----------- \r\n");
    }
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATStartNotify(uint8_t argc, char *argv[])
{
    if (argc < 1) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD Language Setting Failed\n");
        return ERRCODE_FAIL;
    }
    uint32_t times = 1; // 1 times
    uint32_t msleepTime = DEFAULT_TIME; // 500ms
    if (argc == ARGC_TOW) {
        char *strtimes = argv[1];
        times = static_cast<uint32_t>(std::atoi(strtimes));
    } else if (argc == ARGC_THREE) {
        char *strtimes = argv[1];
        times = static_cast<uint32_t>(std::atoi(strtimes));
        char *strMsleepTime = argv[2];
        msleepTime = static_cast<uint32_t>(std::atoi(strMsleepTime));
    } else if (argc < 1) {
        return ERRCODE_FAIL;
    }
    char *topicstr = argv[0];
    uint16_t topicId = static_cast<uint16_t>(std::atoi(topicstr));
    times = ((msleepTime * times) > MAX_OUTTIMES) ? 1 : times; // if time is too long times == 1
    for (uint32_t i = 0; i < times; i++) {
        int ret = MsgCenterNotifyProc(topicId, nullptr);
        if (ret != true) {
            WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "times:%d faild\n", i);
            return ERRCODE_FAIL;
        }
        if (i != times - 1) {
            usleep(DEFAULT_SECOND * msleepTime);
        }
    }
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_AT_PERMISSION
uint32_t OhosfwkATCmdQueryAppPermission(uint8_t argc, char *argv[])
{
    if (argc != ARG_NUM_QUERY_APP_PERMISSION) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "QueryPerm: invalid argc (%u)\n", argc);
        return ERRCODE_FAIL;
    }

    const char *bundleName = argv[0];
    PermissionSaved *permList = nullptr;
    int permNum = 0;

    int ret = QueryPermission(bundleName, &permList, &permNum);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "QueryPerm: QueryPermission failed (%d)\n", ret);
        return ERRCODE_FAIL;
    }

    for (int i = 0; i < permNum; ++i) {
        WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                      "[%s] perm[%d]=%s\n", bundleName, i, permList[i].name);
    }
    if (permList != nullptr) {
        HalFree(permList);
    }
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdCheckAppPermission(uint8_t argc, char *argv[])
{
    const int TEST_TASKID = 10003;          // 内部虚拟 taskId

    if (argc != ARG_NUM_CHECK_APP_PERMISSION) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "CheckPerm: invalid argc (%u)\n", argc);
        return ERRCODE_FAIL;
    }

    const char *bundleName    = argv[0];
    const char *permission    = argv[1];

    int ret = LoadPermissions(bundleName, TEST_TASKID);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "CheckPerm: LoadPermissions failed (%d)\n", ret);
        return ERRCODE_FAIL;
    }

    ret = CheckPermissionStat(TEST_TASKID, permission);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                  "[%s] %s permission [%s]\n",
                  bundleName, (ret == GRANTED) ? "HAS" : "NO", permission);
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdGrantAppPermission(uint8_t argc, char *argv[])
{
    if (argc != ARG_NUM_GRANT_APP_PERMISSION) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "GrantPerm: invalid argc (%u)\n", argc);
        return ERRCODE_FAIL;
    }

    const char *bundleName = argv[0];
    const char *permission = argv[1];

    int ret = GrantPermission(bundleName, permission);
    if (ret != PERM_ERRORCODE_SUCCESS) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP,
                      "GrantPerm: failed, ret=%d\n", ret);
        return ERRCODE_FAIL;
    }

    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP,
                  "GrantPerm: [%s] granted to [%s] OK\n",
                  permission, bundleName);
    return ERRCODE_SUCC;
}
#endif  // SUPPORT_AT_PERMISSION

static int32_t GetLocalTimezone(void)
{
    struct tm localTime;
    struct tm gmtTime;
    struct timeval tv = { 0, 0 };

    if (gettimeofday(&tv, nullptr)) {
        return 0;
    }

    localtime_r(&tv.tv_sec, &localTime);
    gmtime_r(&tv.tv_sec, &gmtTime);

    return (localTime.tm_hour - gmtTime.tm_hour) * MINUTES_PER_HOUR + (localTime.tm_min - gmtTime.tm_min);
}

static uint32_t GetTimeZone(char *time)
{
    char *ptr = time;
    char *tmp;
    const char* utcStr = "UTC";
    size_t utcStrLen = strlen(utcStr);

    // 判断是否配置时区
    tmp = strchr(ptr, ',');
    if (tmp == nullptr) {
        return DEFAULT_TIME_ZONE; // 未配置时区默认为UTC+8:00
    }
    *tmp = '\0';
    ptr = tmp + 1;
    if (strncmp(ptr, utcStr, utcStrLen) != 0) {
        return DEFAULT_TIME_ZONE;
    }
    ptr += utcStrLen;

    // 时
    tmp = strchr(ptr, ':');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    int hour = strtol(ptr, nullptr, DECIMAL);
    ptr = tmp + 1;

    // 分
    int min = strtol(ptr, nullptr, DECIMAL);
    int offsetMin = hour * MINUTES_PER_HOUR + min;
    if (offsetMin < MIN_TIMEZONE_OFFSET || offsetMin > MAX_TIMEZONE_OFFSET) {
        return DEFAULT_TIME_ZONE;
    }
    return offsetMin;
}

static uint32_t GetSecTime(char *time, struct timezone *tz)
{
    uint32_t sec;
    struct tm tm = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr};
    char *ptr = time;
    char *tmp;

    // 年
    tmp = strchr(ptr, '-');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    // 年的计算需要减去1900
    tm.tm_year = strtol(ptr, nullptr, DECIMAL) - 1900;
    if (tm.tm_year < TIME_YEAR_OFFSET) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosfwkATCmdTimezoneSet year num must bigger than 1970!");
        return 0;
    }
    ptr = tmp + 1;

    // 月
    tmp = strchr(ptr, '-');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    // 月的计算需要减1
    tm.tm_mon = strtol(ptr, nullptr, DECIMAL) - 1;
    ptr = tmp + 1;

    // 日
    tmp = strchr(ptr, ',');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    tm.tm_mday = strtol(ptr, nullptr, DECIMAL);
    ptr = tmp + 1;

    // 时
    tmp = strchr(ptr, ':');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    tm.tm_hour = strtol(ptr, nullptr, DECIMAL);
    ptr = tmp + 1;

    // 分
    tmp = strchr(ptr, ':');
    if (tmp == nullptr) { return 0; }
    *tmp = '\0';
    tm.tm_min = strtol(ptr, nullptr, DECIMAL);
    ptr = tmp + 1;

    // 秒
    tm.tm_sec = strtol(ptr, nullptr, DECIMAL);
    sec = (uint32_t)mktime(&tm);

    // 时区
    tz->tz_minuteswest = GetTimeZone(ptr);

    return sec;
}

uint32_t OhosfwkATCmdTimezoneSet(uint8_t argc, char *argv[])
{
    struct timeval tv = { 0, 0 };
    struct timezone tz = { 0, 0 };
    tv.tv_sec = GetSecTime(argv[0], &tz);
    int32_t localTimeZone = GetLocalTimezone();

    tv.tv_sec = tv.tv_sec + localTimeZone * SECONDS_PER_MINUTE - (tz.tz_minuteswest * SECONDS_PER_MINUTE);
    if (tv.tv_sec == 0) {
        return ERRCODE_FAIL;
    } else if (tv.tv_sec <= (tz.tz_minuteswest * SECONDS_PER_MINUTE)) {
        tv.tv_sec = 0;
    }

    settimeofday(&tv, &tz);
#ifdef SUPPORT_OHOSFWK
#ifndef CFG_NORFLASH_ONLY
    AlarmClockReset();
#endif
#endif
    return ERRCODE_SUCC;
}

uint32_t OhosfwkATCmdTimezoneGet(uint8_t argc, char *argv[])
{
    (void)argc;
    (void)argv;
    char timebuf[TIME_MAX] = { 0 };
    struct tm tm;
    struct timeval tv = { 0, 0 };
    const char* formatStr = " %F %T";

    if (gettimeofday(&tv, nullptr)) {
        return ERRCODE_FAIL;
    }

    // 计算本地时间
    localtime_r(&tv.tv_sec, &tm);
    uint32_t millisec = tv.tv_usec / USEC_TO_MILLI;
    strftime(timebuf, TIME_MAX - 1, formatStr + 1, &tm);

    // 计算时区
    int32_t localTimeZone = GetLocalTimezone();
    int32_t offsetMin = localTimeZone % MINUTES_PER_HOUR;
    offsetMin = offsetMin < 0 ? offsetMin + MINUTES_PER_HOUR : offsetMin;
    int32_t offsetHour = localTimeZone / MINUTES_PER_HOUR;
    offsetHour = localTimeZone < 0 && offsetMin != 0 ? offsetHour - 1 : offsetHour;

    if (localTimeZone >= 0) {
        PRINT("%s:%03d UTC+%d:%02d \r\n", timebuf, millisec, offsetHour, offsetMin);
    } else {
        PRINT("%s:%03d UTC%d:%02d \r\n", timebuf, millisec, offsetHour, offsetMin);
    }

    return ERRCODE_SUCC;
}

#ifdef CONFIG_NATIVEAPP_TEST
uint32_t OhosfwkATSwitchSlicePage(uint8_t argc, char *argv[])
{
    if (argc < 2 || argv[0] == nullptr || argv[1] == nullptr) { // 2: argv num
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OHOSFWK ATCMD switch slice page Failed\n");
        return ERRCODE_FAIL;
    }
    char *sliceStr = (char *)argv[0];
    char *pageStr = (char *)argv[1];
    uint32_t sliceIndex = atoi(sliceStr);
    uint32_t pageIndex = atoi(pageStr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd(), sliceIndex = %[d], pageIndex = %[d]", sliceIndex,
                  pageIndex);
    if (sliceIndex > VIEW_INVALIDE_APP || pageIndex > MAX_PAGE) {
        WEARABLE_LOGE(WEARABLE_LOG_MODULE_APP, "OhosATTestPageCmd sliceIndex/pageIndex too larget!", sliceIndex,
                      pageIndex);
        return ERRCODE_FAIL;
    }
    OHOS::NativeAbility::GetInstance().SwitchSlice(sliceIndex, pageIndex);
    return ERRCODE_SUCC;
}

uint32_t OhosATMonkeyControl(uint8_t argc, char *argv[])
{
    char *monkeyStr = (char *)argv[0];
    uint32_t monkeyControl = atoi(monkeyStr);
    WEARABLE_LOGI(WEARABLE_LOG_MODULE_APP, "OhosATMonkeyControl(),monkeyControl = %[d]", monkeyControl);
    OHOS::MonkeyModel::GetInstance().SetStart(monkeyControl);
    return ERRCODE_SUCC;
}
#endif
