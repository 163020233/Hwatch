/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File         ux_bt_hfp_hf_basic_event.h
 * Description: ux bt basic event.
 */
#ifndef UX_BT_HFP_HF_BASIC_EVENT_H
#define UX_BT_HFP_HF_BASIC_EVENT_H

#include <stdint.h>
#include "errcode.h"
#include "ux_bt_hal_sco.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void ux_bt_common_sco_start(ux_api_bt_stream_state_t sco_state);
void ux_bt_common_sco_stop(ux_api_bt_stream_state_t sco_state);

void ux_bt_main_state_handle(uint8_t old_state, uint8_t new_state);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

