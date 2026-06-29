/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux bt ble \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */

#ifndef UX_BT_BR_H
#define UX_BT_BR_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt.h"
#include "bs_br_service_api.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif

errcode_t ux_bt_br_init(void);
errcode_t ux_bt_br_deinit(void);
ux_bt_stack_info_t *ux_bt_br_get_stack_info(void);
errcode_t ux_bt_br_switch_discovery(ux_api_bt_role_t role, ux_bt_discovery_state_t discovery_state);
ux_api_bt_local_info_t* ux_bt_br_get_local_info(void);
ux_api_bt_device_info_t* ux_bt_br_get_cur_central_device_info(void);
ux_api_bt_device_info_t* ux_bt_br_get_cur_peripheral_device_info(void);
#if defined(__cplusplus) && __cplusplus
}
#endif
#endif
