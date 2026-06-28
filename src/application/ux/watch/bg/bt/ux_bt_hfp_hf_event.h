/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bt_hfp_hf_event.h
 * Description:  ux bt event register callback interface
 */

#ifndef UX_BT_HFP_HF_EVENT_H__
#define UX_BT_HFP_HF_EVENT_H__
#include <stdint.h>
#include "errcode.h"
#include "ux_api_bt_audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    ux_api_bt_addr_t addr;
    ux_bt_hal_hfp_role_t role;
    ux_api_bt_stream_state_t sco_state;
} ux_bt_call_sco_state_cb_packet_t;
 
typedef struct {
    ux_api_bt_addr_t addr;
    ux_bt_hal_hfp_role_t role;
    ux_audio_sco_codec sco_codec;
} ux_bt_call_sco_codec_cb_packet_t;
 
typedef struct {
    ux_api_bt_addr_t addr;
    ux_bt_hal_call_cat1_info_t cat1_info;
} ux_bt_call_cat1_state_packet_t;
 
typedef struct {
    ux_api_bt_addr_t addr;
    bool is_band_in;
} ux_bt_call_ring_cb_packet_t;

typedef struct {
    ux_api_bt_addr_t addr;
    ux_bt_hal_volume_info_t volume_info;
} ux_bt_call_volume_packet_t;

void ux_bt_hfp_hf_sco_codec_cbk(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    const ux_audio_sco_codec *codec_cfg);
 
void ux_bt_hfp_hf_sco_state_cbk(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    ux_api_bt_stream_state_t sco_state);
/* 铃声回调 */
void ux_bt_hf_ring_cbk(const ux_api_bt_addr_t *addr, bool is_band_in);
/* ag端通话状态回调 */
void ux_bt_hfp_hf_cat1_state_cbk(const ux_api_bt_addr_t *addr, const ux_bt_hal_call_cat1_info_t *cat1_info);
/* ag端音量回调 */
void ux_bt_hfp_hf_set_volume_request_cbk(const ux_api_bt_addr_t *addr, const ux_bt_hal_volume_info_t *volume_info);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

