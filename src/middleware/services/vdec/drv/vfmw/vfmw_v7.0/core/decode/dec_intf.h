/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_INTF_H__
#define __DEC_INTF_H__

#include "vfmw.h"
#include "dec_base.h"
#include "dec_ctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* DEC Module interface */
td_s32 dec_open(td_void);
td_s32 dec_close(td_void);
td_s32 dec_suspend(const td_void *opt);
td_s32 dec_resume(const td_void *opt);
td_s32 dec_wakeup(td_s32 chan_id);
td_s32 dec_control(td_s32 chan_id, td_u32 cmd, td_void *args);
td_s32 dec_get_mem_need_size(vfmw_svr_mem_need *mem);
td_s32 dec_process(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
