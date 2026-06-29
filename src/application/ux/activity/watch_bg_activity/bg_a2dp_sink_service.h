/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#ifndef BG_A2DP_SINK_SERVICE_H
#define BG_A2DP_SINK_SERVICE_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt_a2dp.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

void bg_a2dp_sink_service_codec_change(uint8_t *param, uint16_t param_len);
void bg_a2dp_sink_service_profile_state_change(uint8_t *param, uint16_t param_len);
void bg_a2dp_sink_service_stream_state_change(uint8_t *param, uint16_t param_len);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif