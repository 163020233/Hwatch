/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: At register header \n
 *
 */

#ifndef AT_NET_SW22_CMD_REGISTER_H
#define AT_NET_SW22_CMD_REGISTER_H

#include <td_base.h>
#include "at.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXT_AT_NET_SW22_CMD_MAX_LEN   128

td_u32 uapi_at_net_sw22_register_cmd(TD_CONST at_cmd_entry_t *cmd_tbl, td_u16 cmd_num);
td_void at_net_sw22_cmd_register(td_void);

#ifdef __cplusplus
}
#endif

#endif
