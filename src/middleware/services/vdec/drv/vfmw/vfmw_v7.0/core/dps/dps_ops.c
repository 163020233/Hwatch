/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "dps.h"
#include "dbg.h"
#include "vfmw_define.h"
#ifdef VFMW_HEVC_SUPPORT
#include "dps_h265.h"
#endif
#ifdef VFMW_H264_SUPPORT
#include "dps_h264.h"
#endif
#ifdef VFMW_MVC_SUPPORT
#include "dps_mvc.h"
#endif
#include "dps_ops.h"

static dps_ops g_dps_ops[] = {
#ifdef VFMW_HEVC_SUPPORT
    {
        .vid_std = VFMW_HEVC,
        .get_max_slice_num = dps_h265_get_max_slice_num,
        .get_pic_param_size = dps_h265_get_pic_param_size,
        .get_slice_param_size = dps_h265_get_slice_param_size,
        .make_dps_pic = dps_h265_make_dps_pic,
        .make_dps_slice = dps_h265_make_dps_slice,
        .get_ref_info = dps_h265_get_ref_info,
        .release_seg = dps_h265_release_seg,
    },
#endif

#ifdef VFMW_H264_SUPPORT
    {
        .vid_std = VFMW_H264,
        .get_max_slice_num = dps_h264_get_max_slice_num,
        .get_pic_param_size = dps_h264_get_pic_param_size,
        .get_slice_param_size = dps_h264_get_slice_param_size,
        .make_dps_pic = dps_h264_make_dps_pic,
        .make_dps_slice = dps_h264_make_dps_slice,
        .get_ref_info = dps_h264_get_ref_info,
        .release_seg = dps_h264_release_seg,
    },
#endif

#ifdef VFMW_MVC_SUPPORT
    {
        .vid_std = VFMW_MVC,
        .get_max_slice_num = dps_mvc_get_max_slice_num,
        .get_pic_param_size = dps_mvc_get_pic_param_size,
        .get_slice_param_size = dps_mvc_get_slice_param_size,
        .make_dps_pic = dps_mvc_make_dps_pic,
        .make_dps_slice = dps_mvc_make_dps_slice,
        .get_ref_info = dps_mvc_get_ref_info,
        .release_seg = dps_mvc_release_seg,
    },
#endif
};

td_s32 dps_get_max_param_size(td_void)
{
    dps_ops *ops = g_dps_ops;
    td_s32 std_num = (td_s32)(sizeof(g_dps_ops) / sizeof(dps_ops));
    td_s32 i;
    td_u32 size = 0;
    td_u32 tmp_size;

    for (i = 0; i < std_num; i++) {
        tmp_size = 0;
        if (ops[i].get_pic_param_size != TD_NULL) {
            tmp_size = (td_u32)ops[i].get_pic_param_size();
            tmp_size = vfmw_align_up(tmp_size, VFMW_CTX_ALIGN);
            tmp_size *= MAX_DPS_NUM;
        }

        if (ops[i].get_slice_param_size != TD_NULL && ops[i].get_max_slice_num != TD_NULL) {
            vfmw_check_ret((td_s64)ops[i].get_slice_param_size() * ops[i].get_max_slice_num() < VFMW_MAX_SINT32, 0);
            tmp_size += (td_u32)ops[i].get_slice_param_size() * (td_u32)ops[i].get_max_slice_num();
            tmp_size = vfmw_align_up(tmp_size, VFMW_CTX_ALIGN);
            tmp_size *= MAX_DPS_NUM;
        }

        if (tmp_size > size) {
            size = tmp_size;
        }
    }

    return (td_s32)size;
}

dps_ops *dps_get_ops(td_s32 vid_std)
{
    td_s32 cnt;
    td_s32 ops_num;
    dps_ops *ops;

    ops = g_dps_ops;
    ops_num = (td_s32)(sizeof(g_dps_ops) / sizeof(dps_ops));
    for (cnt = 0; cnt < ops_num; cnt++) {
        if (ops[cnt].vid_std == vid_std) {
            return ops + cnt;
        }
    }

    return TD_NULL;
}
