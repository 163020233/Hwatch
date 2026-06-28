/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_config_common.h
 * Description:  Audio ux common config
 */
#ifndef __UX_CONFIG_TWS_H__
#define __UX_CONFIG_TWS_H__

#include "ux_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ux_bd_addr coupled_bt_addr;             // bt coupled addr
    ux_bd_addr coupled_sle_addr;            // sle coupled addr
    ux_bd_addr coupled_ble_addr;            // ble coupled addr
} ux_config_tws;

ux_config_tws* ux_get_tws_config(void);

int32_t ux_config_product_form_init(void);
void ux_config_read_factory_peer_addr(ux_work_protocol_t protocol);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
