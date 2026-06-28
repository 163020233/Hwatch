/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __LOCAL__PROC__
#define __LOCAL__PROC__

#include "vfmw.h"
#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 local_proc_read(td_s32 module, td_void *p, td_s32 chan_id);
td_s32 local_proc_write(td_s32 module, cmd_str_ptr cmd, td_s32 count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
