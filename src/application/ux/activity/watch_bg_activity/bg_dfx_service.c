/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-27， Create file. \n
 */
#include "ux_api_bt.h"
#include "common_def.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_power.h"
#include "bg_dfx_service.h"

void bg_dfx_service_bt_core_crash(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = ux_api_bt_disable(UX_API_BT_PROTOCOL_BR);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_dfx_service_bt_core_crash] Fail to disable bt br stack 0x%x.", err);
    }
    err = ux_api_bt_disable(UX_API_BT_PROTOCOL_BLE);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_dfx_service_bt_core_crash] Fail to disable bt ble stack 0x%x.", err);
    }
    ux_power_b_core_t state = UX_POWER_BT_MAX;
    err = ux_power_get_bt_core_state(&state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[bg_dfx_service_bt_core_crash] ux_power_get_bt_core_state fail err:%x", err);
        return;
    }
    if (state == UX_POWER_BT_OFF) {
        ux_log_debug("[bg_dfx_service_bt_core_crash] man-made b core power off err:%x", err);
        return;
    }
    err = ux_power_bt_off(0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_dfx_service_bt_core_crash] Fail to power off b core 0x%x.", err);
        return;
    }
    err = ux_power_bt_on();
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_dfx_service_bt_core_crash] Fail to power on b core 0x%x.", err);
        return;
    }
}