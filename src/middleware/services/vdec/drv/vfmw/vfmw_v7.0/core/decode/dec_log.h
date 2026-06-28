/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_LOG_H__
#define __DEC_LOG_H__

#include "vfmw.h"
#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 dec_log_read(td_void *buf, td_s32 chan_id, td_s32 len);
td_s32 dec_log_write(cmd_str_ptr buffer, td_u32 count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
