/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * Description:the header file of vi proc
 * Author: AuthorNameMagicTag
 * Create: 2019-03-13
 */

#ifndef VIU_PROC_H
#define VIU_PROC_H

#include "drv_mcp_dependency.h"

MCP_OSAL_PROC_READ_FUNC(viu);

td_s32 viu_proc_help(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);
td_s32 viu_proc_set_save_path(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);
td_s32 viu_proc_dump_pipe_raw(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);
td_s32 viu_proc_dump_chn_frame(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);
td_s32 viu_proc_set_magic_num(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);
td_s32 viu_proc_set_pattern(td_u32 argc, td_char (*argv)[MCP_PROC_CMD_SINGEL_LENGTH_MAX], td_void *private);

td_void viu_isp_dev_dfx(td_void);
td_void viu_isp_pipe_dfx(td_void);
#endif /* VIU_PROC_H */

