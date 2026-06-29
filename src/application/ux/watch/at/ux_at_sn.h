/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux at sn \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-18， Create file. \n
 */
#ifndef UX_AT_SN_H
#define UX_AT_SN_H

#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t at_sn_set_process(const ux_at_para1_args_t *args);
at_ret_t at_sn_read_process(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif