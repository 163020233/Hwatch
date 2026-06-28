/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __LOCAL__H__
#define __LOCAL__H__

#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
td_s32 local_open(td_void);
td_s32 local_close(td_void);
td_s32 local_init(td_void *args, td_u32 len);
td_s32 local_exit(const td_void *args);
td_s32 local_suspend(const td_void *args);
td_s32 local_resume(const td_void *args);
td_s32 local_get_image(td_s32 chan_id, td_void *image);
td_s32 local_release_image(td_s32 chan_id, const td_void *image);
td_s32 local_check_image(td_s32 chan_id, td_void *image);
td_s32 local_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len);
td_s32 local_set_callback(vfmw_adpt_type type, fun_vfmw_event_report fp_event_report);
td_s32 local_check_eos(td_s32 chan_id, td_s32 *flag);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif