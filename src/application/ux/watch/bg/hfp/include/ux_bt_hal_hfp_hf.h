/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File
 * Description:
 */
#ifndef UX_BT_HAL_HFP_HF_H
#define UX_BT_HAL_HFP_HF_H

#include "ux_errcode.h"
#include "errcode.h"
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt_hal_hfp.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */
/* 铃声回调 */
typedef void (*ux_bt_hal_hf_ring_cbk)(const ux_api_bt_addr_t *addr, bool is_band_in, uint8_t ring_status);
/* ag端通话状态回调 */
typedef void (*ux_bt_call_cat1_status_cbk)(const ux_api_bt_addr_t *addr,
    const ux_bt_hal_call_cat1_info_t *cat1_info);

typedef struct {
    ux_bt_hal_hfp_conn_state_cbk hfp_connect_state_cb;
    ux_bt_hal_call_set_volume_request_cbk set_volume_request_cb;
    ux_bt_hal_hf_ring_cbk call_ring_cb;
    ux_bt_call_cat1_status_cbk cat1_state_from_ag_cb;
} ux_bt_hal_hf_callbacks_t;

errcode_t ux_bt_hfp_register(ux_bt_hal_hfp_role_t hfp_role);
errcode_t ux_bt_hfp_unregister(ux_bt_hal_hfp_role_t hfp_role);
errcode_t ux_bt_hal_call_operate_request(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_call_info_t *call_control);
errcode_t ux_bt_call_hfp_connect(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t role);
errcode_t ux_bt_call_hfp_disconnect(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t role);
// volume 音量值，值范围0-15
errcode_t ux_bt_call_set_volume_request(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info);

errcode_t ux_bt_hal_hfp_hf_register_callbacks(uint8_t app_id, ux_bt_hal_hf_callbacks_t *func);
errcode_t ux_bt_hal_hfp_hf_unregister_callbacks(uint8_t app_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif