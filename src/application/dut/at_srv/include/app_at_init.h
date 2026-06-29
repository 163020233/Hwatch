/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved. \n
 *
 * Description: app at service init header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-10-16, Create file. \n
 */
#ifndef APP_AT_INIT_H
#define APP_AT_INIT_H

#include <stdint.h>
#include <dut.h>
#include "errcode.h"
#include "uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


void app_at_init(void);
dut_cmd_level_t dut_get_cmd_permission(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
