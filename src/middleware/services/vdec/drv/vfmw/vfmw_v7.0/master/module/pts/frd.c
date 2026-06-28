/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */
#include "limits.h"

#include "vfmw_osal.h"
#include "dbg.h"
#include "frd.h"

/* -------------------------------LOCAL MACRO---------------------------------- */
#define FRD_FPS_24  24
#define FRD_FPS_25  25
#define FRD_FPS_30  30
#define FRD_FPS_50  50
#define FRD_FPS_60  60
#define FRD_FPS_120 120

#define FRD_INFRAME_RATE_JUMP_THRESHOLD 10
#define FRD_INFRAME_RATE_WAVE_THRESHOLD 2
#define FRD_SAMPLE_FRAME_RATE_MAX_DIFF  1200

#define FRD_MAX_PTS_DIFF_IN_100US 100000
#define FRD_MIN_PTS_DIFF_IN_100US 40

/* -----------------------------LOCAL DEFINITION------------------------------- */
typedef enum {
    NORMAL_FRATE_MODE = 0,
    SAMPLE_FRATE_MODE,
    INVALIDE_FRATE_MODE,
} frd_frate_mode;

/* ----------------------------INTERNAL FUNCTION------------------------------- */


STATIC td_u32 acquire_now_pts(td_s64 pts_diff, const frd_alg_info *frd_info)
{
    td_s32 tmp;
    td_u32 now_rate = 0;
    if ((pts_diff <= (td_s64)(FRD_MAX_PTS_DIFF_IN_100US * (td_u64)(frd_info->pts_unit))) &&
        (pts_diff >= (td_s64)(FRD_MIN_PTS_DIFF_IN_100US * (td_u64)(frd_info->pts_unit)))) {
        tmp = (td_s32)(frd_divide((td_u64)pts_diff, frd_info->pts_unit));
        if (tmp != 0) {
            now_rate = (td_u32)(1000000 / tmp); /* 1000000 :a number */
            now_rate = FRD_ROUND(now_rate);
            now_rate = now_rate / 10; /* 10 :a number */
        }
    }
    return now_rate;
}


STATIC td_u32 frd_in_pts_sqn_cal_now_rate(frd_alg_info *frd_info, td_u32 env, frd_frate_mode mode)
{
    td_u32 now_rate;
    td_s64 pts_diff;

    pts_diff = 0;
    switch (mode) {
        case NORMAL_FRATE_MODE:
            if (env == 1) {
                vfmw_check_ret(frd_check_calc_no_overflow_sub(frd_info->in_pts_sqn[frd_info->cur_pts_ptr],
                    frd_info->in_pts_sqn[0]), 0);
                pts_diff = frd_info->in_pts_sqn[frd_info->cur_pts_ptr] - frd_info->in_pts_sqn[0];
                vfmw_check_ret(frd_check_calc_no_overflow_mul(pts_diff, 10), 0); /* 10 :a number */
                pts_diff = (td_s64)frd_divide((td_u64)(10 * pts_diff), frd_info->cur_pts_ptr); /* 10 :a number */
            } else {
                vfmw_check_ret(frd_check_calc_no_overflow_sub(frd_info->in_pts_sqn[frd_info->cur_pts_ptr],
                    frd_info->in_pts_sqn[frd_info->cur_pts_ptr - 1]), 0);
                pts_diff = frd_info->in_pts_sqn[frd_info->cur_pts_ptr] -
                    frd_info->in_pts_sqn[frd_info->cur_pts_ptr - 1];
                vfmw_check_ret(frd_check_calc_no_overflow_mul(pts_diff, 10), 0); /* 10 :a number */
                pts_diff = 10 * pts_diff; /* 10 :a number */
            }
            break;

        case SAMPLE_FRATE_MODE:
            if (env == 1) {
                vfmw_check_ret(frd_check_calc_no_overflow_sub(frd_info->sample_pts_info[frd_info->sample_act_num - 1],
                    frd_info->sample_pts_info[0]), 0);
                pts_diff = frd_info->sample_pts_info[frd_info->sample_act_num - 1] - frd_info->sample_pts_info[0];
                vfmw_check_ret(frd_check_calc_no_overflow_mul(pts_diff, 10), 0); /* 10 :a number */
                pts_diff = (td_s64)frd_divide((td_u64)(10 * pts_diff), frd_info->sample_act_num); /* 10 :a number */
            } else {
                vfmw_check_ret(frd_check_calc_no_overflow_sub(frd_info->sample_pts_info[frd_info->sample_act_num - 1],
                    frd_info->sample_pts_info[frd_info->sample_act_num - 2]), 0); /* 2 :a number */
                pts_diff = frd_info->sample_pts_info[frd_info->sample_act_num - 1] -
                           frd_info->sample_pts_info[frd_info->sample_act_num - 2]; /* 2 :a number */
                vfmw_check_ret(frd_check_calc_no_overflow_mul(pts_diff, 10), 0); /* 10 :a number */
                pts_diff = 10 * pts_diff; /* 10 :a number */
            }
            break;

        default:
            break;
    }

    now_rate = acquire_now_pts(pts_diff, frd_info);

    return now_rate;
}

STATIC td_void frd_in_pts_sqn_change_in_frame_fate(frd_alg_info *frd_info, td_u32 now_rate)
{
    frd_info->in_frame_rate = now_rate;

    if (now_rate > 50) { /* 50 :a number */
        frd_info->length = frd_info->in_frame_rate / 10; /* 10 :a number */
    } else {
        frd_info->length = 5; /* 5 :a number */
    }

    if (frd_info->in_frame_rate > 300) { /* 300 :a number */
        frd_info->stable_threshold = frd_info->in_frame_rate / 100; /* 100 :a number */
    } else {
        frd_info->stable_threshold = 3; /* 3 :a number */
    }

    return;
}

STATIC td_void frd_cal_avrg_frame_rate(frd_alg_info *frd_info)
{
    td_u32 now_rate;
    td_u32 rate_diff;
    td_u32 cal_rate;
    td_u32 wave_delta;

    now_rate = frd_in_pts_sqn_cal_now_rate(frd_info, 1, NORMAL_FRATE_MODE);

    rate_diff = (frd_info->in_frame_rate > now_rate) ? (frd_info->in_frame_rate - now_rate) :
                (now_rate - frd_info->in_frame_rate);

    if (frd_info->in_frame_rate_last >= 200 && frd_info->in_frame_rate_last <= 400) { /* 200 :a number 400 :a number */
        wave_delta = 50; /* 50 :a number */
    } else {
        wave_delta = frd_info->in_frame_rate_last / FRD_INFRAME_RATE_JUMP_THRESHOLD / FRD_INFRAME_RATE_WAVE_THRESHOLD;
    }

    /* detect the tiny rate wave */
    if (rate_diff > wave_delta) {
        /* 2 :a number */
        cal_rate = (frd_info->in_frame_rate != 0) ? (now_rate + frd_info->in_frame_rate) / 2 : now_rate;
        frd_in_pts_sqn_change_in_frame_fate(frd_info, cal_rate);
        frd_info->in_frame_rate_last = now_rate;
    } else {
        /* the second time frd_info->in_frame_rate_last == now_rate,we adjust in_frame_rate as
           now_rate,because the tiny ratediff between in_frame_rate and actual FrameRate may exist */
        if (frd_info->in_frame_rate_last == now_rate) {
            if (FRD_ABS(rate_diff) > (frd_info->in_frame_rate_last / FRD_INFRAME_RATE_JUMP_THRESHOLD /
                                      FRD_INFRAME_RATE_WAVE_THRESHOLD) &&
                frd_info->in_frame_rate != 0) {
                cal_rate = (now_rate + frd_info->in_frame_rate) / 2; /* 2 :a number */
            } else {
                cal_rate = now_rate;
            }

            frd_in_pts_sqn_change_in_frame_fate(frd_info, cal_rate);
        } else {
            frd_info->in_frame_rate_last = now_rate;
        }
    }

    /* reset the PTSSqn,reserve the latest one pts data */
    frd_reset(frd_info, 1);

    return;
}

STATIC td_void frd_adjust_rate_diff(frd_alg_info *frd_info, td_u32 now_rate)
{
    td_u32 stable;
    td_s32 rate_diff;
    td_u32 now_rate_tmp = now_rate;

    frd_info->in_frame_rate_eque_time++;

    /* if 3 == frd_info->in_frame_rate_eque_time, we think the fps has been stable, so reset the unable_time */
    if (frd_info->in_frame_rate_eque_time == 3 ||
        now_rate_tmp == frd_info->in_frame_rate) {
        frd_info->unable_time = 0;
    }

    /* For stream about 30fps, use bigger JUMP hreshold, accept wider pts diff range */
    /* calculate the times of occurrence of stability of new frame rate */
    if (frd_info->in_frame_rate_last >= 200 && frd_info->in_frame_rate_last <= 400) { /* 200 :a number 400 :a number */
        stable = 10; /* 10 :a number */
    } else {
        stable = frd_info->stable_threshold;
    }

    if (frd_info->in_frame_rate_eque_time == stable) {
        now_rate_tmp = frd_in_pts_sqn_cal_now_rate(frd_info, 1, NORMAL_FRATE_MODE);
        rate_diff = (td_s32)(now_rate_tmp - frd_info->in_frame_rate);
        if (FRD_ABS(rate_diff) > (td_s32)(frd_info->in_frame_rate) / FRD_INFRAME_RATE_JUMP_THRESHOLD) {
            frd_in_pts_sqn_change_in_frame_fate(frd_info, now_rate_tmp);

            frd_info->in_frame_rate_last = frd_info->in_frame_rate;
        }
    }

    /* when InPtsSqn is full,calculate the average fps */
    if (frd_info->cur_pts_ptr == frd_info->length - 1) {
        frd_cal_avrg_frame_rate(frd_info);
    } else {
        frd_info->cur_pts_ptr++;
    }

    return;
}

STATIC td_void frd_adjust_rate(frd_alg_info *frd_info, td_u32 now_rate)
{
    td_u32 jump_delta;
    td_s32 rate_diff;

    rate_diff = (td_s32)(now_rate - frd_info->in_frame_rate_last);

    /* For stream about 30fps, use bigger JUMP hreshold, accept wider pts diff range */
    if (frd_info->in_frame_rate_last >= 200 && frd_info->in_frame_rate_last <= 400) { /* 200 :a number 400 :a number */
        jump_delta = 200; /* 200 :a number */
    } else {
        jump_delta = frd_info->in_frame_rate_last / FRD_INFRAME_RATE_JUMP_THRESHOLD;
    }

    if ((td_s32)(jump_delta) < FRD_ABS(rate_diff)) {
        frd_info->unable_time++;

        if (frd_info->unable_time == 10) { /* 10 :a number */
            frd_info->in_frame_rate = 0;
            frd_info->unable_time = 0;
        }

        frd_info->in_frame_rate_last = now_rate;

        /* the stable_threshold = fps/10; in order to detect the fps change quickly; */
        if (now_rate > 300) { /* 300 :a number */
            frd_info->stable_threshold = now_rate / 100; /* 100 :a number */
        } else {
            frd_info->stable_threshold = 3; /* 3 :a number */
        }

        /* reset the PTSSqn,reserve the lastest two pts data */
        frd_reset(frd_info, 2); /* 2 :a number */
    } else {
        frd_adjust_rate_diff(frd_info, now_rate);
    }

    return;
}

STATIC td_void frd_pts_sample(frd_alg_info *frd_info, td_s64 pts)
{
    td_u32 now_rate;

    frd_info->in_pts_sqn[frd_info->cur_pts_ptr] = pts;

    now_rate = frd_in_pts_sqn_cal_now_rate(frd_info, 0, NORMAL_FRATE_MODE);
    if (now_rate != 0) {
        frd_adjust_rate(frd_info, now_rate);
    } else {
        /* now_rate = 0,we think it is a unstable fps */
        frd_info->unable_time++;

        if (frd_info->unable_time == 10) { /* 10 :a number */
            frd_info->in_frame_rate = 0;
            frd_info->unable_time = 0;
        }

        frd_reset(frd_info, 1);
    }

    return;
}

STATIC td_void frd_update_new_rate(frd_alg_info *frd_info, td_s64 pts)
{
    td_u32 delta;
    td_u32 new_rate = 0;

    if (!frd_check_calc_no_overflow_sub(pts, frd_info->cal_pts)) {
        frd_info->que_cnt = 0;
        return;
    }
    if (pts > frd_info->cal_pts) {
        delta = (td_u32)frd_divide((td_u64)(pts - frd_info->cal_pts), 60); /* 60 :a number */
    } else {
        delta = (td_u32)frd_divide((td_u64)(frd_info->cal_pts - pts), 60); /* 60 :a number */
    }

    if (delta != 0) {
        new_rate = (td_u32)frd_divide((td_u64)(10000 * (td_u64)(frd_info->pts_unit)), delta); /* 10000 :a number */
    }

    if (FRD_ABS((td_s32)(new_rate - frd_info->que_rate)) < 10) { /* 10 :a number */
        if (frd_info->que_stable < 3) { /* 3 :a number */
            frd_info->que_stable++;
        }
    } else {
        frd_info->que_rate = new_rate;
        frd_info->que_stable = 0;
    }

    frd_info->cal_pts = pts;
    frd_info->que_cnt = 0;

    return;
}

STATIC td_void frd_update_max_rate_and_min_rate(frd_alg_info *frd_info, td_u32 now_rate)
{
    if (frd_info->sample_max_rate == 0) {
        frd_info->sample_max_rate = frd_info->sample_min_rate = now_rate;
    } else {
        frd_info->sample_max_rate = now_rate > frd_info->sample_max_rate ? now_rate : frd_info->sample_max_rate;
        frd_info->sample_min_rate = (now_rate == 0 ? frd_info->sample_min_rate :
                                    (now_rate < frd_info->sample_min_rate ?
                                    now_rate : frd_info->sample_min_rate));
    }
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */

td_bool frd_check_calc_no_overflow_mul(td_s64 arg1, td_s64 arg2)
{
    vfmw_check_ret((arg2 >= 1) || (arg2 <= -1), TD_TRUE);
    if (arg1 >= 0) {
        vfmw_check_ret(arg1 <= (LLONG_MAX / arg2), TD_FALSE);
    } else {
        vfmw_check_ret(arg1 >= (LLONG_MIN / arg2), TD_FALSE);
    }
    return TD_TRUE;
}

td_bool frd_check_calc_no_overflow_add(td_s64 arg1, td_s64 arg2)
{
    if (((arg1 >= 0) && (arg2 <= 0)) || ((arg1 <= 0) && (arg2 >= 0))) {
        return TD_TRUE;
    } else {
        if (arg1 >= 0) {
            vfmw_check_ret(arg1 <= (LLONG_MAX - arg2), TD_FALSE);
        } else {
            vfmw_check_ret(arg1 >= (LLONG_MIN - arg2), TD_FALSE);
        }
    }
    return TD_TRUE;
}

td_bool frd_check_calc_no_overflow_sub(td_s64 arg1, td_s64 arg2)
{
    if (((arg1 >= 0) && (arg2 >= 0)) || ((arg1 <= 0) && (arg2 <= 0))) {
        return TD_TRUE;
    } else {
        if (arg1 >= 0) {
            vfmw_check_ret(arg1 <= (LLONG_MAX + arg2), TD_FALSE);
        } else {
            vfmw_check_ret(arg1 >= (LLONG_MIN + arg2), TD_FALSE);
        }
    }
    return TD_TRUE;
}

td_void frd_reset(frd_alg_info *frd_info, td_u32 pts_num)
{
    td_s64 now_pts;
    td_s64 pre_pts;
    td_u32 i;

    vfmw_check(frd_info != TD_NULL);
    if (pts_num != 0) {
        if (pts_num == 1) {
            now_pts = frd_info->in_pts_sqn[frd_info->cur_pts_ptr];

            for (i = 0; i < FRD_IN_PTS_SEQUENCE_LENGTH; i++) {
                frd_info->in_pts_sqn[i] = VFMW_INVALID_PTS;
            }

            frd_info->in_pts_sqn[0] = now_pts;
            frd_info->cur_pts_ptr = 1;
            frd_info->in_frame_rate_eque_time = 0;
        } else {
            now_pts = frd_info->in_pts_sqn[frd_info->cur_pts_ptr];
            pre_pts = frd_info->in_pts_sqn[frd_info->cur_pts_ptr - 1];

            for (i = 0; i < FRD_IN_PTS_SEQUENCE_LENGTH; i++) {
                frd_info->in_pts_sqn[i] = VFMW_INVALID_PTS;
            }

            frd_info->in_pts_sqn[0] = pre_pts;
            frd_info->in_pts_sqn[1] = now_pts;
            frd_info->cur_pts_ptr = 2; /* 2 :a number */
            frd_info->in_frame_rate_eque_time = 1;
        }
    } else {
        for (i = 0; i < FRD_IN_PTS_SEQUENCE_LENGTH; i++) {
            frd_info->in_pts_sqn[i] = VFMW_INVALID_PTS;
        }

        frd_info->in_frame_rate_eque_time = 0;
        frd_info->cal_pts = VFMW_INVALID_PTS;
        frd_info->que_cnt = 0;
        frd_info->que_stable = 0;
    }

    return;
}

td_void frd_round_frm_rate(td_u32 frame_rate, td_u32 *new_frame_rate)
{
    td_u32 integer;
    td_u32 rate = 0;

    vfmw_check(new_frame_rate != TD_NULL);

    integer = frame_rate / 10; /* 10 :a number */

    switch (integer) {
        case (FRD_FPS_24 - 1):
            rate = 240; /* 240 :a number */
            break;

        case (FRD_FPS_24):
            if (FRD_ABS((td_s32)(frame_rate - 240)) < 5) { /* 240 :a number 5 :a number */
                rate = 240; /* 240 :a number */
            } else {
                rate = 250; /* 250 :a number */
            }
            break;

        case (FRD_FPS_25):
            rate = 250; /* 250 :a number */
            break;

        case (FRD_FPS_30 - 1):
        case (FRD_FPS_30):
        case (FRD_FPS_30 + 1):
            rate = 300; /* 300 :a number */
            break;

        case (FRD_FPS_50 - 1):
        case (FRD_FPS_50):
        case (FRD_FPS_50 + 1):
            rate = 500; /* 500 :a number */
            break;

        case (FRD_FPS_60 - 1):
        case (FRD_FPS_60):
        case (FRD_FPS_60 + 1):
            rate = 600; /* 600 :a number */
            break;

        case (FRD_FPS_120 - 1):
        case (FRD_FPS_120):
        case (FRD_FPS_120 + 1):
            rate = 1200; /* 1200 :a number */
            break;

        default:
            rate = 0;
            break;
    }

    *new_frame_rate = rate;

    return;
}

td_u64 frd_divide(td_u64 dividend, td_u32 divider)
{
    td_u64 ret = 0;

    vfmw_unused(dividend);
    vfmw_unused(divider);
    vfmw_check_ret(os_do_div != TD_NULL, ret);

    if (divider != 0) {
        ret = os_do_div(dividend, divider);
    }

    return ret;
}

td_u32 frd_frame_rate_detect(frd_alg_info *frd_info, td_s64 pts)
{
    if (frd_info == TD_NULL) {
        dprint(PRN_ERROR, "frd_info pointer is null. \n");
        return 0;
    }

    if (frd_info->cal_pts == VFMW_INVALID_PTS) {
        frd_info->cal_pts = pts;
    } else {
        frd_info->que_cnt = frd_info->que_cnt + 1;
    }

    if (frd_info->que_cnt == 61) { /* 61 :a number */
        frd_update_new_rate(frd_info, pts);
    }

    if (frd_info->in_pts_sqn[0] != VFMW_INVALID_PTS) {
        frd_pts_sample(frd_info, pts);
    } else {
        frd_in_pts_sqn_change_in_frame_fate(frd_info, 0);

        frd_reset(frd_info, 0);

        frd_info->in_pts_sqn[0] = pts;
        frd_info->cur_pts_ptr = 1;
        frd_info->unable_time = 0;
        frd_info->in_frame_rate_last = frd_info->in_frame_rate;
    }

    if (frd_info->que_stable == 3 && frd_info->in_frame_rate == 0) { /* 3 :a number */
        frd_info->in_frame_rate = frd_info->que_rate;
    }

    return frd_info->in_frame_rate;
}

td_u32 frd_sample_frm_rate_detect(frd_alg_info *frd_info, td_s64 pts)
{
    td_u32 frame_rate = 0;
    td_u32 now_rate;
    td_s32 max_avg_diff;
    td_s32 min_avg_diff;

    vfmw_check_ret(frd_info != TD_NULL, 0);
    if (frd_info->sample_max_num > FRD_IN_PTS_SEQUENCE_LENGTH ||
        frd_info->sample_max_num == 0) {
        return frame_rate;
    }

    if (frd_info->sample_act_num >= FRD_IN_PTS_SEQUENCE_LENGTH) {
        /* 1000 :a number 10 :a number */
        vfmw_check_ret((frd_info->sample_last_frame_rate / 1000) * 10 < VFMW_MAX_UINT32, 0);
        /* 1000 :a number 10 :a number 1000 :a number */
        frame_rate = (frd_info->sample_last_frame_rate / 1000) * 10 + (frd_info->sample_last_frame_rate % 1000);

        return frame_rate;
    }

    if (frd_info->sample_pts_info[0] != VFMW_INVALID_PTS) {
        frd_info->sample_pts_info[frd_info->sample_act_num] = pts;
    } else {
        frd_info->sample_pts_info[0] = pts;
    }

    frd_info->sample_act_num++;

    if (frd_info->sample_act_num < 2) { /* 2 :a number */
        return frame_rate;
    }

    now_rate = frd_in_pts_sqn_cal_now_rate(frd_info, 0, SAMPLE_FRATE_MODE);

    frd_update_max_rate_and_min_rate(frd_info, now_rate);

    if (frd_info->sample_act_num >= frd_info->sample_max_num) {
        now_rate = frd_in_pts_sqn_cal_now_rate(frd_info, 1, SAMPLE_FRATE_MODE);
        if ((now_rate >= frd_info->sample_max_rate) || (now_rate <= frd_info->sample_min_rate)) {
            now_rate = (frd_info->sample_max_rate + frd_info->sample_min_rate) / 2; /* 2 :a number */
        }

        frd_info->sample_avg_rate = now_rate;
        max_avg_diff = FRD_ABS((td_s32)(frd_info->sample_max_rate - now_rate));
        min_avg_diff = FRD_ABS((td_s32)(frd_info->sample_min_rate - now_rate));
        if ((max_avg_diff <= FRD_SAMPLE_FRAME_RATE_MAX_DIFF) &&
            (min_avg_diff <= FRD_SAMPLE_FRAME_RATE_MAX_DIFF)) {
            frame_rate = frd_info->sample_avg_rate;
        }
    }

    return frame_rate;
}

