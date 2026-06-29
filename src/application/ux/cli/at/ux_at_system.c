/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide at cmd service for 3322 watch \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-09， Create file. \n
 */
#include <dut.h>
#include <at_product.h>
#include <app_at_uart.h>
#include "feature_config.h"
#include "ux_api_system_config.h"
#include "ux_at_manager.h"
#include "debug_print.h"
#include "ux_hal_memory.h"
#include "securec.h"
#include "ux_bg_config.h"
#include "ux_api_fwk.h"
#include "ux_at_api.h"
#include "at.h"
#include "ux_event_define.h"
#include "ux_errcode.h"
#include "ux_api_msg.h"

#ifdef DEMO_WATCH
#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */
#endif

/* 获取sdk版本号的at命令 */
at_ret_t at_ux_get_sdk_version(void)
{
    char sdk_version[SDK_VERSION_LEN_MAX] = {0, };

    errcode_t ret = ux_api_get_sdk_version(sdk_version, sizeof(sdk_version));
    if (ret != EOK) {
        at_uart_printf("at_ux_atcmd_get_sdk_version  fail! ret = %d\r\n", ret);
        return AT_RET_SYNTAX_ERROR;
    }
    at_uart_printf("^SDKVERSION: %s\r\n", sdk_version);

    return AT_RET_OK;
}

/* 获取系统所有版本信息的at命令 */
at_ret_t at_ux_get_all_version(void)
{
    ux_api_version_info_t *version_info = ux_hal_memory_acquire(UX_MEM_AT, sizeof(ux_api_version_info_t));
    if (version_info == NULL) {
        at_uart_printf("at_ux_get_all_version  ux_hal_memory_acquire fail! \r\n");
        return AT_RET_SYNTAX_ERROR;
    }

    /* 通过ux_api_get_all_version获取所有版本信息 */
    errcode_t ret = ux_api_get_all_version(version_info, sizeof(ux_api_version_info_t));
    if (ret != ERRCODE_SUCC) {
        at_uart_printf("ux_api_get_all_version  fail! ret = %d \r\n", ret);
        ux_hal_memory_release(UX_MEM_AT, version_info);
        version_info = NULL;
        return AT_RET_SYNTAX_ERROR;
    }

    /* 根据掩码返回结果判断当前是否支持SDK版本查询。 */
    if (version_info->version_mask & VERSION_MASK_SDK) {
        at_uart_printf("sdk version is: %s \r\n", version_info->sdk_version);
    }
    if (version_info->version_mask & VERSION_MASK_FLASHBOOT) {
        at_uart_printf("flash boot key ver: 0x%x, code ver : 0x%x \r\n",
            version_info->flashboot_image.key_version, version_info->flashboot_image.image_version);
    }
    if (version_info->version_mask & VERSION_MASK_RECOVERY) {
        at_uart_printf("recovery image key ver: 0x%x, code ver : 0x%x \r\n",
            version_info->recovery_image.key_version, version_info->recovery_image.image_version);
    }
    if (version_info->version_mask & VERSION_MASK_APPLICATION) {
        at_uart_printf("application image key ver: 0x%x, code ver : 0x%x \r\n",
            version_info->application_image.key_version, version_info->application_image.image_version);
    }
    if (version_info->version_mask & VERSION_MASK_BT) {
        at_uart_printf("bt image key ver: 0x%x, code ver : 0x%x \r\n",
            version_info->bt_image.key_version, version_info->bt_image.image_version);
    }
    if (version_info->version_mask & VERSION_MASK_CHIP) {
        at_uart_printf("chip version is: %s \r\n", version_info->chip_version);
    }

    ux_hal_memory_release(UX_MEM_AT, version_info);
    version_info = NULL;
    return AT_RET_OK;
}

#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
static at_ret_t set_transmit_mode_process(const char *mode_str)
{
    if (mode_str == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    ux_api_sys_transmit_mode_t mode;
    errcode_t ret;
    if (strcmp(mode_str, "1R2T") == 0) {
        mode = UX_API_SYS_TRANSMIT_MODE_1R2T;
    } else if (strcmp(mode_str, "1R4T") == 0) {
        mode = UX_API_SYS_TRANSMIT_MODE_1R4T;
    } else if (strcmp(mode_str, "1R8T") == 0) {
        mode = UX_API_SYS_TRANSMIT_MODE_1R8T;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }

    ret = ux_api_sys_set_transmit_mode(mode);
    if (ret != ERRCODE_SUCC) {
        at_uart_printf("Failed to set transmit mode:%s\n", mode_str);
        return AT_RET_SYNTAX_ERROR;
    }
    at_uart_printf("Successfully set transmit mode:%s\n", mode_str);
    return AT_RET_OK;
}

static at_ret_t get_transmit_mode_process(void)
{
    ux_api_sys_transmit_mode_t mode;
    mode = ux_api_sys_get_transmit_mode();
    switch (mode) {
        case UX_API_SYS_TRANSMIT_MODE_1R2T:
            at_uart_printf("Transmit Mode:1R2T\n");
            break;
        case UX_API_SYS_TRANSMIT_MODE_1R4T:
            at_uart_printf("Transmit Mode:1R4T\n");
            break;
        case UX_API_SYS_TRANSMIT_MODE_1R8T:
            at_uart_printf("Transmit Mode:1R8T\n");
            break;
        default:
            at_uart_printf("Transmit Mode:UNKNOWN\n");
    }
    return AT_RET_OK;
}
#endif

at_ret_t at_ux_sys_set_process(const ux_at_para2_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    static ux_cmd_t sys_cfg_cmd[] = {
#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
        { "transmit_mode", set_transmit_mode_process },
#endif
    };

    return at_cmd_proc_ux_cmd(sys_cfg_cmd, arry_num(sys_cfg_cmd), args->para1, args->para2);
}

at_ret_t at_ux_sys_get_process(const ux_at_para2_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    static cmd_query_t sys_cfg_query[] = {
#ifdef CONFIG_SUPPORT_SYSTEM_TRANSMIT_MODE
        { "transmit_mode", get_transmit_mode_process },
#endif
    };

    return at_cmd_proc_ux_query(sys_cfg_query, arry_num(sys_cfg_query), args->para1);
}