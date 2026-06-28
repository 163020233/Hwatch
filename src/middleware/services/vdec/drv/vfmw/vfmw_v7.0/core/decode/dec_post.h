/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __DEC_POST_H__
#define __DEC_POST_H__

#include "dec_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_UP_MSG_SLICE_NUM 200

typedef struct {
    td_u8 err_flag;
    td_u16 slice_num;
    td_u16 over_flow;
    td_u32 prev_pic_mb0_qp;
    td_u32 switch_rounding;
    td_u32 slice_mb_range[MAX_UP_MSG_SLICE_NUM + 1][2]; /* 2 :a number */
    td_u32 is_read_from_reg;
    td_u32 correct_cnt;
} dec_report_info;

typedef struct {
    vfmw_vid_std vid_std;
    UADDR image_addr;
    UADDR image_2d_addr;
    UADDR ref_image_addr;
    UADDR curr_pmv_addr;
    td_s32 image_width;
    td_s32 image_height;
    td_s32 is_frame;
    td_s32 image_csp; /* 0:420, 1:400 */
    td_s32 valid_group_num;
    td_s32 rpr_ref_pic_type;
    td_s32 pic_type;
    td_s32 full_repair;
    td_u32 level;
    td_u32 ctb_size;
    td_u32 bit_depth;
    td_u32 tiles_enabled_flag;
    td_s32 compress_enable_flag;
    td_u32 vdh_2d_en;
    td_u32 err_flag;

    struct {
        td_u32 start_mbn;
        td_u32 end_mbn;
    } mb_group[MAX_UP_MSG_SLICE_NUM];
} dec_repair_param;

td_s32 dec_post_make_report(td_void *chan_arg, td_void *task_arg);
td_u32 dec_post_record_reg_info(td_void *chan_arg, td_void *task_arg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
