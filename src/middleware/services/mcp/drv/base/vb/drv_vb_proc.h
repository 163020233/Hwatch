/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: define for external use
 * Author: AuthorNameMagicTag
 * Create: 2026-02-27
 */

#ifndef __DRV_VB_PROC_H__
#define __DRV_VB_PROC_H__

#include "drv_mcp_dependency.h"

#ifdef __cplusplus
extern "C" {
#endif /* end of #ifdef __cplusplus */

td_s32 drv_vb_proc_show(struct osal_proc_dir_entry *s);
td_s32 drv_vb_cmd_call(td_u32 argc, const td_char *argv[], td_void *private);

#ifdef __cplusplus
}
#endif /* end of #ifdef __cplusplus */

#endif /* end of #ifndef  __DRV_VB_PROC_H__ */