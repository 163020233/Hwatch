/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */

#ifndef WATCH_BG_RESPONSE_BG_H
#define WATCH_BG_RESPONSE_BG_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_device_manager.h"
#include "ux_types.h"
#include "bg_gap_service.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t bg_process_bg_msg(uint8_t event, uint8_t *param, uint16_t param_len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
