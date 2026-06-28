/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DPS_H264_H__
#define __DPS_H264_H__

#include "dec_base.h"
#include "dps.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_s32 dps_h264_get_max_slice_num(td_void);
td_s32 dps_h264_get_pic_param_size(td_void);
td_s32 dps_h264_get_slice_param_size(td_void);

td_s32 dps_h264_make_dps_pic(td_void *syn_ctx, td_void *param);
td_s32 dps_h264_make_dps_slice(td_void *syn_ctx, td_void *param, td_void *prev_param);

td_s32 dps_h264_get_ref_info(const td_void *param, dps_ref_info *dps_ref);
td_s32 dps_h264_release_seg(td_s32 chan_id, const td_void *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
