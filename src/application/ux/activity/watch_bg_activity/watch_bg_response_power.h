/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */

#ifndef WATCH_BG_RESPONSE_POWER_H
#define WATCH_BG_RESPONSE_POWER_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"


#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif
errcode_t bg_process_power_msg(uint8_t event, uint8_t *param, uint16_t param_len);

#if defined(__cplusplus) && __cplusplus
}
#endif
#endif
