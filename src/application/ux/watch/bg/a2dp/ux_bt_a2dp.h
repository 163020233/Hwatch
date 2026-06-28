/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */

#ifndef UX_BT_A2DP_H
#define UX_BT_A2DP_H
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_audio_a2dp_sink.h"
#include "ux_errcode.h"
#include "ux_api_bt.h"
#include "ux_api_bt_audio.h"
#include "errcode.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_BT_A2DP_SNK = 0x01,
    UX_BT_A2DP_SRC = 0x02,
    UX_BT_A2DP_MAX,
} ux_bt_a2dp_role_t;

typedef struct {
    uint16_t device_id;
    ux_bt_a2dp_role_t a2dp_role;
    AudioFormatConfig codec;
    ux_api_bt_stream_state_t stream_state;
    ux_api_bt_profile_state_t profile_state;
} ux_bt_a2dp_info_t;

errcode_t ux_bt_a2dp_init(void);
errcode_t ux_bt_a2dp_deinit(void);
errcode_t ux_bt_hal_a2dp_req_reply(bool is_accept);
errcode_t ux_bt_a2dp_start_stream(const ux_api_bt_addr_t *addr);
errcode_t ux_bt_a2dp_stop_stream(const ux_api_bt_addr_t *addr);
errcode_t ux_bt_a2dp_connect(ux_api_bt_addr_t *addr, uint8_t len);
errcode_t ux_bt_a2dp_disconnect(ux_api_bt_addr_t *addr, uint8_t len);
ux_api_bt_audio_info_t* ux_bt_a2dp_get_audio_info(const ux_api_bt_addr_t *addr);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif