/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */

#ifndef BG_A2DP_AVRCP_SERVICE_H
#define BG_A2DP_AVRCP_SERVICE_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

void bg_avrcp_ct_service_fast_forward_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_media_attribute_report(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_media_play_state_change(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_media_volume_down(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_media_volume_mute(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_media_volume_up(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_pause_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_play_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_play_next_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_play_pre_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_stop_media(uint8_t *param, uint16_t param_len);
void bg_avrcp_ct_service_volume_sync_report(uint8_t *param, uint16_t param_len);
void bg_avrcp_service_profile_state_change(uint8_t *param, uint16_t param_len);
void bg_avrcp_tg_service_media_control_request(uint8_t *param, uint16_t param_len);
void bg_avrcp_tg_service_vol_control_request(uint8_t *param, uint16_t param_len);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif