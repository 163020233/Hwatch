/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At gui function \n
 *
 */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "at_cmd.h"
#include "at_gui_cmd_register.h"
#include "at_gui_cmd_table.h"
#include "debug_print.h"
#include "dfx/dfx_cmd.h"
#include "sample_ui.h"
#include "service/graphic_service_wrapper.h"
#include "soc_osal.h"
#include "at_gui.h"

static bool g_sampleInitialized = false;

#define AT_GUI_FUNC_NUM (sizeof(g_at_gui_cmd_parse_table) / sizeof(g_at_gui_cmd_parse_table[0]))


static td_void wstp_uikit_sample(td_void)
{
    if (g_sampleInitialized) {
        return;
    }
#ifndef SUPPORT_OHOSFWK
    SampleUi();
#endif
    g_sampleInitialized = true;
    return;
}


static td_void wstp_uikit_sample_run(int argc, char *argv[])
{
    wstp_uikit_sample();
#ifndef SUPPORT_OHOSFWK
    if (argc < 1) {
        printf("Invalid Input, argc must > 1, argc: %d\n", argc);
        return;
    }
    char* testcase = argv[0];
    print_str("SAMPLE [%s] START RUN. \n", testcase);
    StartCaseUi(testcase);
    return;
#endif
}

void los_at_gui_cmd_register(void)
{
    print_str("los_at_gui_cmd_register EXCUTE\r\n");
    uapi_at_gui_register_cmd(g_at_gui_cmd_parse_table, AT_GUI_FUNC_NUM);
}

void space_to_comma(char *cmd)
{
    for (uint32_t i = 0; i < strlen(cmd); i++) {
        if (cmd[i] == ' ') cmd[i] = ',';
    }
}

td_void at_gui_process(int argc, char *argv[])
{
    if (argc < 1) {
        return;
    }

    if (strcmp(argv[0], "UIKIT_SAMPLE") == 0) {
        InitGraphicService();
        wstp_uikit_sample();
    } else if (strcmp(argv[0], "UIKIT_DFX") == 0) {
        ExecUikitDfxByCmdType(argc - 1, &argv[1]);
    } else if (strcmp(argv[0], "UIKIT_SAMPLE_RUN") == 0) {
        InitGraphicService();
        wstp_uikit_sample_run(argc - 1, &argv[1]);
    }
    return;
}

static td_s32 at_gui_convert_bin_to_dec(td_s32 pbin)
{
    td_s32 result = 0;
    td_s32 temp = pbin;

    while (temp != 0) {
        result += temp % 2; // 2 div
        temp /= 2; // 2 div
    }

    return result;
}

at_ret_t at_gui_sample_process(const gui_args_t *arg)
{
    PRINT("gui_at enter \r\n");
    td_s32 argc = at_gui_convert_bin_to_dec(arg->para_map);
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6, arg->para7, arg->para8,
        arg->para9, arg->para10, arg->para11, arg->para12, arg->para13, arg->para14, arg->para15, arg->para16,
        arg->para17, arg->para18, arg->para19, arg->para20};
    PRINT("AT+GUI=");
    for (td_s32 i = 0; i < argc; i++) {
        printf("%s,", argv[i]);
        if (strcmp(argv[i], " ") == 0) {
            argc = i;
            break;
        }
        space_to_comma(argv[i]);
    }
    printf("\n");

    at_gui_process(argc, argv);

    return AT_RET_OK;
}
