/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#include "at.h"
#include "errcode.h"
#include "ux_api_fwk.h"
#include "ux_event_define.h"
#include "ux_hal_memory.h"
#include "ux_errcode.h"
#include "app_at_uart.h"
#include "ux_api_pbap.h"
#include "ux_at_manager.h"
#include "ux_bt.h"
#include "debug_print.h"
#include "ux_bt_pbap.h"
#include "ux_at_pbap.h"

#define PBAP_MAX_LINE_LEN 1024
#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */

static at_ret_t parse_pbap_open_para_cmd(const ux_at_para5_args_t *args)
{
    ux_api_bt_pbap_file_type_t file_type = UX_API_BT_PBAP_FILE_MAX;
    FILE *vcf_file = NULL;
    char *buff = ux_hal_memory_acquire(UX_MEM_BT, PBAP_MAX_LINE_LEN);
    if (buff == NULL) {
        return AT_RET_MALLOC_ERROR;
    }
    errcode_t err = AT_RET_OK;

    if (strcasecmp(args->para2, "phone_book") == 0) {
        file_type = UX_API_BT_PBAP_FILE_PHONE_BOOK;
    } else if (strcasecmp(args->para2, "call_log") == 0) {
        file_type = UX_API_BT_PBAP_FILE_CALL_LOG;
    } else {
        err =  AT_RET_SYNTAX_ERROR;
        goto out;
    }

    err = ux_api_bt_pbap_open_file(&vcf_file, file_type);
    if (err != UX_ERR_SUCCESS) {
        err = AT_RET_SYNTAX_ERROR;
        goto out;
    }
    // 逐行读取电话簿内容并打印
    while (fgets(buff, PBAP_MAX_LINE_LEN, vcf_file) != NULL) {
        at_uart_printf("%s", buff);
    }
    err = ux_api_bt_pbap_close_file(vcf_file);
    if (err != UX_ERR_SUCCESS) {
        err = AT_RET_SYNTAX_ERROR;
        goto out;
    }
    goto out;
out:
    ux_hal_memory_release(UX_MEM_BT, buff);
    return err;
}

static at_ret_t parse_pbap_pull_para_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL || args->para3 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char *endptr = NULL;
    long dev_id = strtol(args->para2, &endptr, 0);

    ux_api_bt_pbap_file_type_t file_type = UX_API_BT_PBAP_FILE_MAX;
    if (strcasecmp(args->para3, "phone_book") == 0) {
        file_type = UX_API_BT_PBAP_FILE_PHONE_BOOK;
    } else if (strcasecmp(args->para3, "call_log") == 0) {
        file_type = UX_API_BT_PBAP_FILE_CALL_LOG;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_api_bt_addr_t addr = {0};
    errcode_t err = ux_bt_get_addr_by_device_id((uint16_t)dev_id, &addr);
    if (err != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    err = ux_api_bt_pbap_pull_remote_file(&addr, file_type);
    if (err != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

at_ret_t at_ux_pbap_set_process(const ux_at_para5_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_at_para5_args_t *pbap_args = (ux_at_para5_args_t *)args;
    ux_at_5para_cmd_pair_t pbap_cmds[] = {
        {"open", parse_pbap_open_para_cmd},
        {"pull", parse_pbap_pull_para_cmd},
    };
    for (uint8_t i = 0; i < (sizeof(pbap_cmds) / sizeof(ux_at_5para_cmd_pair_t)); i++) {
        if (strcasecmp(pbap_args->para1, pbap_cmds[i].cmd) == 0) {
            return pbap_cmds[i].cmd_handle(args);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

at_ret_t at_ux_pbap_read_process(void)
{
    ux_bt_pbap_info_t* pbap_info = ux_bt_pbap_get_info();
    at_uart_printf("===========pbap info===========\n");
    at_uart_printf("^phone book dev_id: %d\n", pbap_info->dev_id);
    if (pbap_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTING) {
        at_uart_printf("^profile state: connecting\n");
    } else if (pbap_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        at_uart_printf("^profile state: connected\n");
    } else if (pbap_info->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTING) {
        at_uart_printf("^profile state: disconnecting\n");
    } else if (pbap_info->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        at_uart_printf("^profile state: disconnected\n");
    } else {
        at_uart_printf("^unknow\n");
    }
    at_uart_printf("^phone book state: %d\n", pbap_info->phone_book_recevice_state);
    at_uart_printf("^call log state: %d\n", pbap_info->call_log_recevice_state);
    return AT_RET_OK;
}