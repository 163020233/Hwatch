/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide api functions for at \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-04， Create file. \n
 */
#ifndef UX_AT_LED_H
#define UX_AT_LED_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#include "ux_cust_led_config.h"
#include "ux_api_led.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t at_ux_led_set_process(const ux_at_para3_args_t *args);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
