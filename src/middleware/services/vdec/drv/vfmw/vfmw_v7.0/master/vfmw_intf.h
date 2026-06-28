/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __VFMW_INTF_H__
#define __VFMW_INTF_H__

#include "vfmw_ext.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern vfmw_export_fun g_vfmw_export_funcs;

td_s32 vfmw_init(td_void *args);
td_s32 vfmw_exit(td_void *args);
td_s32 vfmw_suspend(td_void *args);
td_s32 vfmw_resume(td_void *args);
td_s32 vfmw_get_image(td_s32 chan_id, td_void *image, td_void *ext_frm_info);
td_s32 vfmw_release_image(td_s32 chan_id, const td_void *image);
td_s32 vfmw_control(td_s32 chan_id, td_s32 cmd, td_void *args, td_u32 len);
td_s32 vfmw_mod_init(td_void);
td_void vfmw_mod_exit(td_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
