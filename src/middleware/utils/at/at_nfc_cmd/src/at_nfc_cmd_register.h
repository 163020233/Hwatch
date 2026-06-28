/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: At register header \n
 *
 * Date: 2025-09-19 \n
 */

#ifndef AT_NFC_CMD_REGISTER_H
#define AT_NFC_CMD_REGISTER_H

#include <td_base.h>
#include "at.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define EXT_AT_NFC_CMD_MAX_LEN   128

td_u32 uapi_at_nfc_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num);
td_void at_nfc_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif
