/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At gnss register header \n
 *
 */

#ifndef AT_GNSS_CMD_REGISTER_H
#define AT_GNSS_CMD_REGISTER_H

#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_DITING_GNSS_CMD_MAX_LEN   128

uint32_t uapi_at_diting_gnss_register_cmd(const at_cmd_entry_t *cmd_tbl, uint16_t cmd_num);
void at_diting_gnss_cmd_register(void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
