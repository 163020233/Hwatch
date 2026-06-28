/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2023. All rights reserved.
 * Description: diag dfx cmd
 */

#ifndef DIAG_CMD_DFX_CASE_H
#define DIAG_CMD_DFX_CASE_H

#include "errcode.h"
#include "diag.h"

typedef enum {
    DIAG_DFX_CMD_CASE_GET_STAT  = 0,
    DIAG_DFX_CMD_CASE_REPORT_MSG = 1,
    DIAG_DFX_CMD_CASE_REPORT_FIX_MSG = 2,
    DIAG_DFX_CMD_CASE_LAST_DUMP = 3,
    DIAG_DFX_CMD_CASE_SYS_MSG = 4,
    DIAG_DFX_CMD_CASE_FAULT_MOCKED = 5,
    DIAG_DFX_CMD_CASE_SET_LOG_LEVEL = 6,
    DIAG_DFX_CMD_CASE_REGISTER_STAT = 7,
    DIAG_DFX_CMD_CASE_TRANSMIT = 8,
    DIAG_DFX_CMD_CASE_MASSDATA = 9,
    DIAG_DFX_CMD_CASE_MAX,
} diag_dfx_cmd_case_id_t;

errcode_t diag_dfx_cmd(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);

#endif /* DIAG_CMD_DFX_CASE_H */
