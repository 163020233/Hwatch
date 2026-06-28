/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#ifndef UX_AT_PBAP_H
#define UX_AT_PBAP_H

#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

at_ret_t at_ux_pbap_set_process(const ux_at_para5_args_t *args);
at_ret_t at_ux_pbap_read_process(void);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */

#endif