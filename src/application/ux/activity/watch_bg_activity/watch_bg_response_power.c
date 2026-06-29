/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-27， Create file. \n
 */
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "errcode.h"
#include "common_def.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "ux_errcode.h"
#include "ux_api_bt.h"
#include "ux_api_fwk.h"
#include "ux_module_init.h"
#include "watch_bg_response_power.h"

typedef void (*sub_event_func)(uint8_t *param, uint16_t param_len);
typedef struct {
    ux_event_power_t sub_event;
    sub_event_func process;
} bg_power_event_process_t;

void bg_power_service_bt_power_on(uint8_t *param, uint16_t param_len)
{
    errcode_t err = UX_ERR_SUCCESS;
    err = ux_api_bt_enable(UX_API_BT_PROTOCOL_BLE);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to enable ble stack 0x%x.", err);
    }
    err = ux_api_bt_enable(UX_API_BT_PROTOCOL_BR);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to enable br stack 0x%x.", err);
    }
}

void bg_power_service_bt_power_off(uint8_t *param, uint16_t param_len)
{
    errcode_t err = UX_ERR_SUCCESS;
    /* b核主动下电时, 去初始化bt */
    ux_api_bt_disable(UX_API_BT_PROTOCOL_BR);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to disable bt br stack 0x%x.", err);
    }
    ux_api_bt_disable(UX_API_BT_PROTOCOL_BLE);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to disable bt ble stack 0x%x.", err);
    }
}

const bg_power_event_process_t g_power_event_func_map[] = {
    { .sub_event = UX_POWER_BT_POWER_ON, .process = bg_power_service_bt_power_on },
    { .sub_event = UX_POWER_BT_POWER_OFF, .process = bg_power_service_bt_power_off },
};

errcode_t bg_process_power_msg(uint8_t event, uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    for (uint8_t i = 0; i < arry_num(g_power_event_func_map); i++) {
        if (g_power_event_func_map[i].sub_event == event) {
            g_power_event_func_map[i].process(param, param_len);
        }
    }
    return ERRCODE_SUCC;
}