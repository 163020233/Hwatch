/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File
 * Description:
 */
#ifndef UX_BT_HAL_SCO_H
#define UX_BT_HAL_SCO_H
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt_hal_hfp.h"
#include "ux_api_bt_audio.h"
#include "ux_audio_hfp_hf.h"
#include "ux_errcode.h"
#include "errcode.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*ux_bt_hal_sco_state_cb)(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    ux_api_bt_stream_state_t sco_state);

typedef void (*ux_bt_hal_sco_codec_report_cb)(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    const ux_audio_sco_codec *codec_cfg);

typedef struct {
    ux_bt_hal_sco_state_cb ux_bt_hal_sco_state_cbk;
    ux_bt_hal_sco_codec_report_cb ux_bt_hal_sco_codec_cbk;
} ux_bt_hal_sco_callbacks_t;

errcode_t ux_bt_hal_sco_register_callbacks(uint8_t app_id, const ux_bt_hal_sco_callbacks_t *func);
errcode_t ux_bt_hal_sco_unregister_callbacks(uint8_t app_id);
errcode_t ux_bt_hal_sco_create_connection(const ux_api_bt_addr_t *addr);
errcode_t ux_bt_hal_sco_release_connection(const ux_api_bt_addr_t *addr);
errcode_t ux_bt_hal_sco_reply_request(const ux_api_bt_addr_t *addr, bool is_accept);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif