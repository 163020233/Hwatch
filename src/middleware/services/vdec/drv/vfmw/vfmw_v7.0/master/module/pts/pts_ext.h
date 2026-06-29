/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __PTS_INFO_H__
#define __PTS_INFO_H__

#include "vfmw.h"
#include "vfmw_proc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 pts_info_init(td_void);
td_s32 pts_info_exit(td_void);
td_s32 pts_info_alloc(td_s32 chan_id);
td_s32 pts_info_free(td_s32 chan_id);
td_s32 pts_info_reset(td_s32 chan_id);
td_s32 pts_info_set_frm_rate(td_s32 chan_id, const vfmw_pts_frmrate *frame_rate);
td_s32 pts_info_get_frm_rate(td_s32 chan_id, vfmw_pts_frmrate *frame_rate);
td_s32 pts_info_convert(td_s32 chan_id, vfmw_image *image);
td_s32 pts_info_enable_sample_frd(td_s32 chan_id);
td_s32 pts_info_disable_sample_frd(td_s32 chan_id);
td_s32 pts_info_read(td_void *buffer, td_s32 chan_id);
td_s32 pts_info_write(cmd_str_ptr buffer, td_u32 count);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
