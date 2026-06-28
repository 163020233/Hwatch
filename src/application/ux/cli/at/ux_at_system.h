/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux at system service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_SYSTEM_H
#define UX_AT_SYSTEM_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t at_ux_get_sdk_version(void);
at_ret_t at_ux_get_all_version(void);
at_ret_t at_ux_sys_set_process(const ux_at_para2_args_t *args);
at_ret_t at_ux_sys_get_process(const ux_at_para2_args_t *args);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif