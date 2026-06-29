/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At npu register header \n
 *
 */

#ifndef AT_NPU_CMD_REGISTER_H
#define AT_NPU_CMD_REGISTER_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_NPU_CMD_MAX_LEN   128

uint32_t uapi_at_npu_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_npu_cmd_register(void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
