/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File
 * Description:
 */

#ifndef UX_BT_CALL_HF_H
#define UX_BT_CALL_HF_H
#include "ux_bt_hal_sco.h"
#include "ux_api_bt_audio.h"
#include "ux_bt_hal_hfp_hf.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    uint16_t dev_id;
    ux_api_bt_addr_t addr;
    ux_bt_hal_hfp_role_t role;
    ux_api_bt_stream_state_t sco_state;
    ux_bt_hal_hfp_connect_state_t hfp_state;
    ux_bt_hal_call_cat1_info_t cat1_info;
    uint8_t mic_volume;
    uint8_t speaker_volume;
    bool mic_is_mute;
    bool speaker_is_mute;
} ux_bt_call_info_t;

errcode_t ux_bt_hf_init(void);
errcode_t ux_bt_hf_deinit(void);
ux_bt_call_info_t* ux_bt_call_get_info(void);
ux_api_bt_audio_info_t* ux_bt_call_get_audio_info(const ux_api_bt_addr_t *addr);
ux_api_bt_control_info_t* ux_bt_call_get_control_info(const ux_api_bt_addr_t *addr);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif