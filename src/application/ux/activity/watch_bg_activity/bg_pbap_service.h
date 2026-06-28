/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */

#ifndef BG_PBAP_SERVICE_H
#define BG_PBAP_SERVICE_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

void bg_pbap_service_profile_state_change(uint8_t *param, uint16_t param_len);
void bg_pbap_service_finish_receive_file(uint8_t *param, uint16_t param_len);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif