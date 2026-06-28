/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
 
#ifndef BG_GAP_SERVICE_H
#define BG_GAP_SERVICE_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_bt.h"
#include "ux_device_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void bg_gap_service_enable(uint8_t *param, uint16_t param_len);
void bg_gap_service_disable(uint8_t *param, uint16_t param_len);
void bg_gap_service_disable_cb(uint8_t *param, uint16_t param_len);
void bg_gap_service_start_discovery(uint8_t *param, uint16_t param_len);
void bg_gap_service_stop_discovery(uint8_t *param, uint16_t param_len);
void bg_gap_service_start_pair(uint8_t *param, uint16_t param_len);
void bg_gap_service_disconnect(uint8_t *param, uint16_t param_len);
void bg_gap_service_remove_device(uint8_t *param, uint16_t param_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
