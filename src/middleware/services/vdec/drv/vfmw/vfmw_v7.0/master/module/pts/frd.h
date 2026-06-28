/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#ifndef __FRD_ALG_H__
#define __FRD_ALG_H__

#include "vfmw.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* ----------------------------EXTERNAL MACRO---------------------------------- */
#define FRD_IN_PTS_SEQUENCE_LENGTH 100

#define frd_abs_(x)   (((x) < 0) ? -(x) : (x))
#define frd_round_(x) ((((x) % 10) > 4) ? ((x) / 10 + 1) * 10 : (x))

#define FRD_ABS frd_abs_
#define FRD_ROUND frd_round_

/* ----------------------------EXTERNAL DEFINITION----------------------------- */
typedef struct {
    td_u32 pts_unit;
    td_u32 cur_pts_ptr; /* pointer of past pts information, pointing to the oldest frame rate in record */
    td_u32 length;
    td_u32 que_cnt;
    td_u32 que_rate;
    td_u32 que_stable;
    td_u32 unable_time;
    td_u32 in_frame_rate_last; /* last input frame rate */
    td_u32 stable_threshold;
    /* counter of stable frame rate, to avoid display shake caused by shake of frame rate */
    td_u32 in_frame_rate_eque_time;
    td_u32 in_frame_rate;
    td_s64 cal_pts;
    td_s64 in_pts_sqn[FRD_IN_PTS_SEQUENCE_LENGTH]; /* past pts information */
    td_s64 first_pts; /* the first frame pts to calc unit ms or us */
    td_s64 second_pts; /* the second frame pts to calc unit ms or us */
    td_u32 calc_unit_done;

    /* for sample frame rate detect */
    td_bool sample_frd_enable;
    td_u32 sample_max_rate;
    td_u32 sample_min_rate;
    td_u32 sample_avg_rate;
    td_u32 sample_act_num;
    td_u32 sample_max_num;
    td_u32 sample_last_frame_rate;
    td_s64 sample_pts_info[FRD_IN_PTS_SEQUENCE_LENGTH]; /* past pts information */
} frd_alg_info;

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
td_bool frd_check_calc_no_overflow_mul(td_s64 arg1, td_s64 arg2);
td_bool frd_check_calc_no_overflow_add(td_s64 arg1, td_s64 arg2);
td_bool frd_check_calc_no_overflow_sub(td_s64 arg1, td_s64 arg2);
td_void frd_reset(frd_alg_info *frd_info, td_u32 pts_num);
td_void frd_round_frm_rate(td_u32 frame_rate, td_u32 *new_frame_rate);
td_u64 frd_divide(td_u64 dividend, td_u32 divider);
td_u32 frd_frame_rate_detect(frd_alg_info *frd_info, td_s64 pts);
td_u32 frd_sample_frm_rate_detect(frd_alg_info *frd_info, td_s64 pts);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
