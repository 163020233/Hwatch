/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "errcode.h"
#include "common_def.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "ux_api_fwk.h"
#include "ux_module_init.h"
#include "bg_gap_service.h"
#include "power_mgmt.h"
#include "watch_bg_response_master.h"

static errcode_t bg_response_power_on(void)
{
    errcode_t ret = ERRCODE_SUCC;
#ifdef CONFIG_SUPPORT_ENABLE_BG_WHEN_POWER_ON
    errcode_t err = UX_ERR_SUCCESS;
    err = ux_api_bt_enable(UX_API_BT_PROTOCOL_BLE);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to enable ble stack 0x%x.", err);
    }
    err = ux_api_bt_enable(UX_API_BT_PROTOCOL_BR);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_power_service_bt_power_on] Fail to enable br stack 0x%x.", err);
    }
#endif
    return ret;
}

static errcode_t bg_response_reset(void)
{
    errcode_t ret = ERRCODE_SUCC;
    ret = ux_adapter_power_reset();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[bg_response_reset] Fail to reset 0x%x!\n", ret);
        return ret;
    }
    return ret;
}
errcode_t bg_process_master_msg(uint8_t event, uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    switch (event) {
        case UX_MASTER_POWER_ON:
            return bg_response_power_on();
        case UX_MASTER_RESET_INFO:
            return bg_response_reset();
        default:
            return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
