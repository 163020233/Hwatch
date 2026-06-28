/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VCTRL__LOG__
#define __VCTRL__LOG__

#include "vfmw.h"
#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 vctrl_log_read(td_s32 module, td_s32 chan_id, td_void *buffer, td_s32 page_size);
td_s32 vctrl_log_write(td_s32 module, cmd_str_ptr buffer, td_s32 count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
