/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_ble_adapt.h
 * Description:  ux ble event register callback interface
 */

#ifndef UX_BLE_ADAPT_H
#define UX_BLE_ADAPT_H
#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_bs_ble_register_event_callback(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

