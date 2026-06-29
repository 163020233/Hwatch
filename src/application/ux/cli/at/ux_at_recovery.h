/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux recovery at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_RECOVERY_H
#define UX_AT_RECOVERY_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
 
at_ret_t at_recovery_set_process(const ux_at_para4_args_t *args);
typedef at_ret_t (*recovery_cmd_handle_t)(const char *value, const char *format_flag, const char *dev_id);
 
typedef struct {
    const char *recovery_cmd;
    recovery_cmd_handle_t cmd_handle;
} recovery_cmd_t;
 
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
