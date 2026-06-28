/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
 
#ifndef BG_HFP_HF_SERVICE_H
#define BG_HFP_HF_SERVICE_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt.h"
#include "ux_device_manager.h"
#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

void bg_hfp_hf_service_sco_state_change(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_sco_codec_change(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_cat1_state_change(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_volume_change(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_answer(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_hangup(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_reject(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_dailing(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_volume_up(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_volume_down(uint8_t *param, uint16_t param_len);
void bg_hfp_hf_service_mute(uint8_t *param, uint16_t param_len);

#if defined(__cplusplus) && __cplusplus
}
#endif  /* __cplusplus */
#endif