/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_ble_hal.h
 * Description:  ux ble hal interface
 */

#ifndef UX_BLE_HAL_H
#define UX_BLE_HAL_H
#include <stdint.h>
#include "errcode.h"
#include "bs_api_stru.h"
#include "ux_bs_generic.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BS_BLE_CONNECT_SHORT_INTERVAL    36  /* 36*1.25 = 45ms */
#define UX_BS_BLE_CONNECT_SHORT_TIMEOUT     600 /* 600*10  = 6000ms */
#define UX_BS_BLE_CONNECT_LONG_INTERVAL     52  /* 52*1.25 = 52ms */
#define UX_BS_BLE_CONNECT_LONG_TIMEOUT      600 /* 600*10  = 6000ms */

typedef enum {
    UX_BS_BLE_CONNECT_MODE_LONG_CYCLE = 0,  /* 长周期模式 */
    UX_BS_BLE_CONNECT_MODE_SHORT_CYCLE,     /* 短周期模式 */
    UX_BS_BLE_CONNECT_MODE_MAX,             /* 无效枚举 */
} ux_bs_ble_connect_mode_t;

errcode_t ux_bs_ble_init(void);
errcode_t ux_bs_ble_set_config(void);
errcode_t ux_bs_ble_config_connect_mode(ux_bs_ble_connect_mode_t mode);
errcode_t ux_bs_ble_start_discovery(void);
errcode_t ux_bs_ble_stop_discovery(void);
errcode_t ux_bs_ble_disconnect(void);
errcode_t ux_bs_ble_power_off(void);

void ux_bt_set_ble_adv_data(bool is_need_add_ble_end_val); /* 设置BLE广播数据 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

