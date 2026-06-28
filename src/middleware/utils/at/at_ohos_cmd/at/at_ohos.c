/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At ohos function \n
 *
 */

#include "at_ohos.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "at_cmd.h"
#include "at_ohos_cmd_register.h"
#include "at_ohos_cmd_table.h"
#include "debug_print.h"
#include "dfx/dfx_cmd.h"
#include "sample_ui.h"
#include "service/graphic_service_wrapper.h"
#include "soc_osal.h"
#include "location_sample.h"
#include "sensor_sample.h"
#include "ohosfwk_at_process.h"
#include "ohosfwk_at_netstack_dfx.h"

static bool g_sampleInitialized = false;

#define AT_OHSO_FUNC_NUM (sizeof(g_at_ohos_cmd_parse_table) / sizeof(g_at_ohos_cmd_parse_table[0]))

void los_at_ohos_cmd_register(void)
{
    print_str("los_at_ohos_cmd_register EXCUTE\r\n");
    uapi_at_ohos_register_cmd(g_at_ohos_cmd_parse_table, AT_OHSO_FUNC_NUM);
}

static void space_to_comma(char *cmd)
{
    for (uint32_t i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ') cmd[i] = ',';
    }
}

static const at_ohos_cmd g_at_ohos_cmd[] = {
    {"OHOSFWK_AM_EXIT_JSAPP",         OhosfwkATCmdAmExitJsApp},
    {"OHOSFWK_BM_INSTALL",            OhosfwkATCmdBmInstall},
    {"OHOSFWK_BM_SET",                OhosfwkATCmdBmSet},
    {"OHOSFWK_BM_UNINSTALL",          OhosfwkATCmdBmUninstall},
    {"OHOSFWK_AM_CHANGENATIVESLICE",  OhosfwkATCmdAmChangeNativeSlice},
    {"OHOSFWK_AM_START_JS_APP",       OhosfwkATCmdAmStartJsApp},
    {"OHOSFWK_DFX_JS_MEMORY_INFO",    OhosfwkATCmdDfxJsMemoryInfo},
    {"OHOSFWK_CHANGE_LANGUAGE",       OhosfwkATCmdChangeLanguage},
    {"OHOSFWK_BM_GET_APPINFO",        OhosfwkATCmdBmGetAppInfo},
    {"OHOSFWK_BM_GET_APPLIST",        OhosfwkATCmdBmGetAppList},
    {"OHOSFWK_AM_DFX",                OhosfwkATCmdAmDfx},
    {"OHOSFWK_LOCATION",              LocationSample},
    {"OHOSFWK_SENSOR",                SensorSample},
    {"OHOSFWK_MSG_START_NOTIFY",      OhosfwkATStartNotify},
#ifdef SUPPORT_AT_PERMISSION
    {"OHOSFWK_QUERY_APP_PERMISSION",  OhosfwkATCmdQueryAppPermission},
    {"OHOSFWK_CHECK_APP_PERMISSION",  OhosfwkATCmdCheckAppPermission},
    {"OHOSFWK_GRANT_APP_PERMISSION",  OhosfwkATCmdGrantAppPermission},
#endif
    {"OHOSFWK_GLOBAL_TIMEZONE_SET",   OhosfwkATCmdTimezoneSet},
    {"OHOSFWK_GLOBAL_TIMEZONE_GET",   OhosfwkATCmdTimezoneGet},
#ifdef CONFIG_NATIVEAPP_TEST
    {"OHOSFWK_SWITCH_SLICE_PAGE",     OhosfwkATSwitchSlicePage},
    {"OHOSFWK_MONKEY_CONTROL",        OhosATMonkeyControl},
#endif
#ifdef CONFIG_NETSTACK_TEST
    {"OHOSFWK_NETSTACK_DFX",     OhosfwkATNetstackDfx},
#endif
};

td_void at_ohos_process(int argc, char *argv[])
{
    if (argc < 1) {
        return;
    }

    const size_t numCommands = sizeof(g_at_ohos_cmd) / sizeof(g_at_ohos_cmd[0]);
    for (size_t i = 0; i < numCommands; ++i) {
        if (strcmp(argv[0], g_at_ohos_cmd[i].cmd) == 0) {
            g_at_ohos_cmd[i].handler(argc - 1, &argv[1]);
            return;
        }
    }

    PRINT("Unknown command: %s\n", argv[0]);
    return;
}

static td_s32 at_ohos_convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;
    td_s32 temp = pbin;

    while (temp != 0) {
        result += temp % 2; // 2 div
        temp /= 2; // 2 div
    }

    return result;
}

at_ret_t at_ohos_sample_process(const ohos_args_t *arg)
{
    PRINT("ohos_at enter \r\n");
    td_s32 argc = at_ohos_convert_bin_to_dec(arg->para_map);
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6, arg->para7, arg->para8,
        arg->para9, arg->para10, arg->para11, arg->para12, arg->para13, arg->para14, arg->para15, arg->para16,
        arg->para17, arg->para18, arg->para19, arg->para20};
    PRINT("AT+OHOS=");
    for (td_s32 i = 0; i < argc; i++) {
        printf("%s,", argv[i]);
        if (strcmp(argv[i], " ") == 0) {
            argc = i;
            break;
        }
        space_to_comma(argv[i]);
    }
    printf("\n");

    at_ohos_process(argc, argv);

    return AT_RET_OK;
}
