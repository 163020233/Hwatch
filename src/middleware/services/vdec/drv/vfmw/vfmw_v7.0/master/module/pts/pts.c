/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */
#include "limits.h"

#include "pts_ext.h"
#include "vfmw_osal.h"
#include "frd.h"
#include "vfmw_adpt.h"
#include "vfmw_define.h"
#include "liteos_proc.h"
#include "pts.h"

/* -------------------------------LOCAL MACRO---------------------------------- */
#define PTS_MAX_CHAN_NUM        VFMW_CHAN_NUM
#ifdef  VFMW_MAX_FRAME_RATE
#define PTS_MAX_FRAME_RATE      VFMW_MAX_FRAME_RATE
#else
#define PTS_MAX_FRAME_RATE      60                  /* fps */
#endif
#define PTS_MIN_FRAME_RATE      12                  /* fps */
#define PTS_MAX_PASS_TIME       (120 * 1000 * 1000) /* us */
#define PTS_MAX_FRAME_TIME_CNT  2
#define PTS_MAX_FRAME_TIME_JUMP 1000
#define PTS_DEFAULT_SAMPLE_NUM  10

#define PTS_CMD_NUM 2
#define PTS_CMD_LEN 50
#define PTS_UNIT_MS 1
#define PTS_UNIT_US 1000

#define PTS_CHAN_NUM VFMW_CHAN_NUM

/* -----------------------------LOCAL DEFINITION------------------------------- */
typedef struct {
    td_bool inited;
    td_s64 last_src_pts;           /* Last valid src pts */
    td_u32 last_src_frm_idx;       /* The index of last frame with a valid src pts */
    td_u32 frame_num;              /* Total frame number */
    td_u32 frame_time_cnt;         /* Frame interval counter */
    td_s64 last_calc_frm_time;     /* Last calculated frame time(us) */
    td_s64 calc_frame_time;        /* Calculated frame time(us) */
    td_s64 set_frame_time;         /* Setted frame time(us) */
    td_s64 ges_frame_time;         /* Guessed frame time(us) */
    td_s32 inter_pts_delta;        /* interleaved source, use to adjust pts for field to frame */
    vfmw_pts_frmrate user_frame_rate; /* User config frame rate */
    frd_alg_info pts_info;
    td_u32 force_pts_mode;
} pts_chan;

typedef struct {
    td_bool inited;
    pts_chan *chan[PTS_CHAN_NUM];
    os_sema sema;
} pts_entry;

/* ------------------------------LOCAL VALUE----------------------------------- */
static pts_entry g_pts_entry;
static td_u32 g_cfg_sample_num = PTS_DEFAULT_SAMPLE_NUM;
static td_u32 g_cur_sample_num = PTS_DEFAULT_SAMPLE_NUM;

/* ----------------------------INTERNAL FUNCTION------------------------------- */
static pts_entry *pts_get_entry(td_void)
{
    return &g_pts_entry;
}

STATIC pts_chan *pts_get_chan(td_s32 chan_id)
{
    pts_entry *entry = TD_NULL;
    pts_chan *chan = TD_NULL;

    vfmw_check_ret(chan_id >= 0, TD_NULL);
    vfmw_check_ret(chan_id < PTS_CHAN_NUM, TD_NULL);

    entry = pts_get_entry();
    vfmw_check_ret(entry != TD_NULL, TD_NULL);

    chan = entry->chan[chan_id];

    return chan;
}

STATIC td_void pst_info_init_param(td_s32 chan_id)
{
    td_s32 i;
    pts_chan *chan;

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    chan->last_src_pts = VFMW_INVALID_PTS;
    chan->last_src_frm_idx = -1;
    chan->frame_num = 0;
    chan->frame_time_cnt = 0;
    chan->last_calc_frm_time = VFMW_INVALID_PTS;
    chan->calc_frame_time = VFMW_INVALID_PTS;
    chan->ges_frame_time = VFMW_INVALID_PTS;
    chan->inter_pts_delta = 0;
    chan->force_pts_mode = 0;
    chan->pts_info.cal_pts = VFMW_INVALID_PTS;
    chan->pts_info.que_cnt = 0;
    chan->pts_info.que_stable = 0;
    chan->pts_info.first_pts = VFMW_INVALID_PTS;
    chan->pts_info.second_pts = VFMW_INVALID_PTS;
    chan->pts_info.calc_unit_done = TD_FALSE;

    for (i = 0; i < FRD_IN_PTS_SEQUENCE_LENGTH; i++) {
        chan->pts_info.sample_pts_info[i] = VFMW_INVALID_PTS;
    }

    chan->pts_info.sample_max_rate = 0;
    chan->pts_info.sample_min_rate = 0;
    chan->pts_info.sample_avg_rate = 0;
    chan->pts_info.sample_act_num = 0;
    chan->pts_info.sample_max_num = g_cur_sample_num;
    chan->pts_info.sample_last_frame_rate = 0;

    return;
}

STATIC td_s64 pts_info_guess_frame_time(pts_chan *chan, const vfmw_image *image)
{
    if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM && image->format.frame_rate_valid == 1) {
        chan->ges_frame_time = 1000000 / image->frame_rate * 1000; /* 1000000 1000 is unit */
    } else {
        if (chan->ges_frame_time == VFMW_INVALID_PTS) {
            if ((image->image_width <= 720) && (image->image_height <= 480)) { /* 720 480 is threshold */
                chan->ges_frame_time = 33333; /* 33333 MAYBE NTSC */
            } else {
                chan->ges_frame_time = 40000; /* 40000 default */
            }
        }
    }

    return chan->ges_frame_time;
}

STATIC td_s64 pts_info_get_frame_time_user(pts_chan *chan, const vfmw_image *image)
{
    td_s64 frame_time;

    /* Choose frame_time from: 1.SetFrameTime 2.CalcFrameTime 3.GuessFrameTime */
    if (chan->set_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->set_frame_time;
    } else if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->calc_frame_time;
    } else {
        frame_time = pts_info_guess_frame_time(chan, image);
    }

    /* the vfmw_max pts difference(us) should be less than the 2 * value of pts difference
       of FrameRateMin(12fps) and current rate */
    /* 1000 :a number 1000000 :a number 12 :a number 2 :a number */
    if (image->frame_rate >= 1000 && frame_time > (1000000 / 12 * 2) &&
        /* 1000000 :a number 1000 :a number 2 :a number */
        frame_time > 1000000 / (image->frame_rate / 1000) * 2) {
        /* 1000000 :a number 1000 :a number */
        frame_time = 1000000 / (image->frame_rate / 1000);
    }

    return frame_time;
}

STATIC td_s64 pts_info_get_frame_time_other(pts_chan *chan, vfmw_image *image)
{
    td_s64 frame_time;

    /* Choose frame_time from: 1.calc_frame_time 2.set_frame_time 3.GuessFrameTime */
    if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->calc_frame_time;
    } else if (chan->set_frame_time != VFMW_INVALID_PTS) {
        frame_time = chan->set_frame_time;
    } else {
        frame_time = pts_info_guess_frame_time(chan, image);
    }

    /* the vfmw_max pts difference(us) should be less then the 2*value of
     pts difference of FrameRateMin(12fps) and current rate */
    /* CNcomment: pts?(us)?Ð¡Ð¡?Î¼pts??2Ð¡???pts??2 */
    /* 1000 :a number 1000000 :a number 12 :a number 2 :a number */
    if (image->frame_rate >= 1000 && frame_time > (1000000 / 12 * 2) &&
        /* 1000000 :a number 1000 :a number 2 :a number */
        frame_time > 1000000 / ((td_s32)image->frame_rate / 1000) * 2) {
        /* 1000000 :a number 1000 :a number */
        frame_time = 1000000 / ((td_s32)image->frame_rate / 1000);
    }

    return frame_time;
}

STATIC td_void pts_info_cal_frm_time(td_s32 chan_id, pts_chan *chan, td_u32 num_pass, td_s64 pts_pass)
{
    td_s32 pts_diff_step;
    td_u64 frame_time_abs;
    pts_chan *tmp_chan;

    tmp_chan = pts_get_chan(chan_id);
    if (tmp_chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    pts_diff_step = (chan->pts_info.pts_unit == 1) ? 1000 : 1; /* 1000 :a number */

    if (chan->calc_frame_time != VFMW_INVALID_PTS) {
        frame_time_abs =
            FRD_ABS(frd_divide((td_u64)(pts_pass * pts_diff_step), num_pass) - (td_u64)chan->last_calc_frm_time);
        if (frame_time_abs > PTS_MAX_FRAME_TIME_JUMP) {
            chan->frame_time_cnt = 0;
        }

        chan->frame_time_cnt++;

        if (chan->frame_time_cnt >= PTS_MAX_FRAME_TIME_CNT) {
            chan->calc_frame_time = (td_s64)frd_divide((td_u64)(pts_pass * pts_diff_step), num_pass);
            chan->frame_time_cnt = PTS_MAX_FRAME_TIME_CNT;
        }
    } else {
        chan->calc_frame_time = (td_s64)frd_divide((td_u64)(pts_pass * pts_diff_step), num_pass);
        chan->frame_time_cnt++;
    }

    chan->last_calc_frm_time = (td_s64)frd_divide((td_u64)(pts_pass * pts_diff_step), num_pass);

    tmp_chan->inter_pts_delta = (td_s32)frd_divide((td_u64)chan->calc_frame_time, 2000); /* 2000 :a number */
}

STATIC td_void pts_info_handle_valid_src_pts(td_s32 chan_id, vfmw_image *image)
{
    td_u32 num_pass;
    td_s64 pts_pass = 0;
    td_s64 src_pts;
    td_s64 img_pts;
    pts_chan *chan = TD_NULL;
    pts_entry *entry;

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get PtsInfo %d failed!\n", chan_id);
        os_sema_up(entry->sema);
        return;
    }

    src_pts = image->src_pts;
    if ((chan->last_src_pts == VFMW_INVALID_PTS) ||
        (chan->last_src_pts < 0 && src_pts > LLONG_MAX + chan->last_src_pts)) {
        chan->inter_pts_delta = 20; /* 20 : ms */
        goto out;
    }

    num_pass = chan->frame_num - chan->last_src_frm_idx;

    if (src_pts >= chan->last_src_pts) {
        pts_pass = src_pts - chan->last_src_pts;
    } else if (src_pts < chan->last_src_pts) {
        pts_pass = VFMW_MAX_PTS - chan->last_src_pts + src_pts;
    }

    /* calculate the frame time */
    if (num_pass != 0) {
        if (pts_pass <= PTS_MAX_PASS_TIME) {
            pts_info_cal_frm_time(chan_id, chan, num_pass, pts_pass);
        } else {
            /* keep the calc_frame_time while pts is abnormal */
            if (src_pts >= chan->last_src_pts) {
                chan->inter_pts_delta = 20; /* 20 :a number */
            } else {
                chan->inter_pts_delta = -20; /* pts back and loopback   -20 :a number */
            }
        }
    } else {
        dprint(PRN_ERROR, "Frame index %d repeat\n", chan->frame_num);
        chan->inter_pts_delta = 20; /* 20 :a number */
    }

out:
    chan->last_src_pts = src_pts;
    chan->last_src_frm_idx = chan->frame_num;
    img_pts = src_pts;

    image->pts = img_pts;
    image->inter_pts_delta = chan->inter_pts_delta;
    os_sema_up(entry->sema);

    return;
}

STATIC td_void pts_info_handle_invalid_src_pts(td_s32 chan_id, vfmw_image *image)
{
    td_u32 num_pass;
    td_u32 pts_diff_step;
    td_s32 ret;
    td_s64 frame_time;
    td_s64 img_pts;
    pts_chan *chan = TD_NULL;
    pts_entry *entry;

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get PtsInfo %d failed!\n", chan_id);
        os_sema_up(entry->sema);
        return;
    }

    if (chan->last_src_pts != VFMW_INVALID_PTS) {
        num_pass = chan->frame_num - chan->last_src_frm_idx;

        pts_diff_step = (chan->pts_info.pts_unit == 1) ? 1000 : 1; /* 1000 :a number */

        if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_USER) {
            frame_time = pts_info_get_frame_time_user(chan, image);
        } else {
            frame_time = pts_info_get_frame_time_other(chan, image);
        }

        // check calc overflow
        ret = frd_check_calc_no_overflow_add(chan->last_src_pts,
            (td_s64)frd_divide(((td_u64)frame_time * num_pass), pts_diff_step));
        if (ret == TD_TRUE) {
            img_pts = chan->last_src_pts + (td_s64)frd_divide(((td_u64)frame_time * num_pass), pts_diff_step);
            chan->inter_pts_delta = (td_s32)frd_divide((td_u64)frame_time, 2000); /* 2000 :a number */
        } else {
            image->pts = VFMW_INVALID_PTS;
            image->inter_pts_delta = 0;
            os_sema_up(entry->sema);
            return;
        }
    } else {
        img_pts = VFMW_INVALID_PTS;
        chan->inter_pts_delta = 0;
    }

    image->pts = img_pts;
    image->inter_pts_delta = chan->inter_pts_delta;
    os_sema_up(entry->sema);

    return;
}

STATIC td_s32 pts_info_guess_first_frame_rate(td_s32 chan_id, td_u32 src_frame_rate, vfmw_image *image)
{
    td_s32 ret;
    vfmw_pts_frmrate frm_rate;

    vfmw_check_sec_func(memset_s(&frm_rate, sizeof(vfmw_pts_frmrate), 0x0, sizeof(vfmw_pts_frmrate)));

    ret = pts_info_get_frm_rate(chan_id, &frm_rate);
    if (ret != EXT_SUCCESS) {
        dprint(PRN_ERROR, "Get frame rate failed!\n");

        return EXT_FAILURE;
    }

    if ((frm_rate.st_set_frm_rate.fps_integer >= PTS_MIN_FRAME_RATE) &&
        (frm_rate.st_set_frm_rate.fps_integer <= PTS_MAX_FRAME_RATE)) {
        /* 1000 :a number */
        image->frame_rate = frm_rate.st_set_frm_rate.fps_integer * 1000 + frm_rate.st_set_frm_rate.fps_decimal;
    } else if ((src_frame_rate >= PTS_MIN_FRAME_RATE) && (src_frame_rate <= PTS_MAX_FRAME_RATE)) {
        image->frame_rate = src_frame_rate * 1000; /* 1000 :a number */
    } else {
        if (image->image_width <= 720 && image->image_height <= 480) { /* 720 :a number 480 :a number */
            image->frame_rate = 30 * 1000; /* < NTSC, asume the framerate is 30fps  30 :a number 1000 :a number */
        } else if (image->image_width < 1280 && image->image_height < 720) { /* 1280 :a number 720 :a number */
            image->frame_rate = 25 * 1000; /* > NTSC, asume the framerate is 25fps  25 :a number 1000 :a number */
        } else {
            image->frame_rate = 30 * 1000; /* < NTSC, asume the framerate is 30fps  30 :a number 1000 :a number */
        }
    }

    return EXT_SUCCESS;
}

STATIC td_void pts_info_get_round_frame_rate(td_s32 chan_id, vfmw_image *image)
{
    td_u32 round_rate = 0;
    pts_chan *chan;

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        return;
    }

    if (((image->rwzb == 0) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_STREAM) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER)) ||
        ((chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM) &&
         (image->frame_rate == 0))) {
        frd_round_frm_rate(((image->frame_rate / 1000) * 10), &round_rate); /* 1000 :a number 10 :a number */

        if (round_rate > 0) {
            image->frame_rate = round_rate * 1000 / 10; /* 1000 :a number 10 :a number */
        }
    }
}

STATIC td_void pts_info_get_first_frame_rate(td_s32 chan_id, vfmw_image *image, frd_alg_info *frd_info,
    td_u32 frame_rate)
{
    td_u32 src_frame_rate;
    td_u32 round_rate_player = 0;
    td_u32 round_rate = 0;
    vfmw_pts_frmrate frm_rate;
    td_u32 frame_rate_tmp = frame_rate;
    td_s32 ret;

    src_frame_rate = image->frame_rate;
    vfmw_check_sec_func(memset_s(&frm_rate, sizeof(vfmw_pts_frmrate), 0x0, sizeof(vfmw_pts_frmrate)));

    if (image->frame_rate != 0) {
        goto out;
    }

    if (frd_info->sample_max_num != 0) {
        frame_rate_tmp = frd_sample_frm_rate_detect(frd_info, image->pts);
        if ((frd_info->sample_act_num >= frd_info->sample_max_num) && (frame_rate_tmp == 0)) {
            pts_info_guess_first_frame_rate(chan_id, src_frame_rate, image);
        } else if ((frd_info->sample_act_num >= frd_info->sample_max_num) && (frame_rate_tmp != 0)) {
            frd_round_frm_rate(frame_rate_tmp, &round_rate);
            ret = pts_info_get_frm_rate(chan_id, &frm_rate);
            frd_round_frm_rate(frm_rate.st_set_frm_rate.fps_integer * 10, &round_rate_player); /* 10 :a number */

            if (round_rate > 0) {
                image->frame_rate = frame_rate_tmp * 1000 / 10; /* 1000 :a number 10 :a number */
            } else if ((ret == EXT_SUCCESS) && (round_rate_player > 0)) {
                image->frame_rate = round_rate_player * 1000 / 10; /* 1000 :a number 10 :a number */
            } else {
                image->frame_rate = frame_rate_tmp * 1000 / 10; /* 1000 :a number 10 :a number */
            }
        } else {
            image->frame_rate = 0;
        }
    } else {
        pts_info_guess_first_frame_rate(chan_id, src_frame_rate, image);
    }

out:
    pts_info_get_round_frame_rate(chan_id, image);
}

STATIC td_void pts_info_frame_rate_detect(td_s32 chan_id, vfmw_image *image)
{
    td_u32 frame_rate = 0;
    pts_chan *chan = TD_NULL;
    frd_alg_info *frd_info = TD_NULL;
    pts_entry *entry = TD_NULL;
    vfmw_pts_frmrate frame_rate_info;

    vfmw_check_sec_func(memset_s(&frame_rate_info, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        dprint(PRN_ERROR, "Get chan %d failed!\n", chan_id);
        os_sema_up(entry->sema);
        return;
    }

    frd_info = &(chan->pts_info);

    /*
     * For RWZB test, VO will use default frame rate adapted to output norm.
     * If frame type was set to STREAM type, need not detect, use stream frame rate.
     * If frame type was set to USER type, need not detect, use the user set value.
      */
    if (((image->rwzb == 0) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_STREAM) &&
         (chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER)) ||
        (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM && image->frame_rate == 0)) {
        frame_rate = frd_frame_rate_detect(frd_info, image->pts);

        image->frame_rate = frame_rate * 1000 / 10; /* 1000 :a number 10 :a number */
    } else if (chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_USER) {
        /* if frame rate type was USER type, use the value from user directly. */
        if (chan->set_frame_time != VFMW_INVALID_PTS) {
            vfmw_check_sec_func(memcpy_s(&frame_rate_info, sizeof(frame_rate_info),
                                         &chan->user_frame_rate, sizeof(vfmw_pts_frmrate)));
        }

        vfmw_check_goto(frame_rate_info.st_set_frm_rate.fps_integer * 1000 < VFMW_UINT_MAX, out); /* 1000 is unit */
        image->frame_rate = frame_rate_info.st_set_frm_rate.fps_integer * 1000 + /* 1000 is unit conversion */
                            frame_rate_info.st_set_frm_rate.fps_decimal;
        os_sema_up(entry->sema);
        return;
    }
    /* To avoid the framerate of the first frame is zero */
    pts_info_get_first_frame_rate(chan_id, image, frd_info, frame_rate);

    frd_info->sample_last_frame_rate = image->frame_rate;
out:
    os_sema_up(entry->sema);

    return;
}

STATIC td_void pts_info_calc_unit(pts_chan *chan, td_s64 in_pts, td_u32 *out_pts_unit)
{
    td_s64 delta;
    td_s32 framerate_ms;
    td_s32 framerate_us;

    if (chan == TD_NULL || out_pts_unit == TD_NULL) {
        dprint(PRN_ERROR, "%s:%d chan = TD_NULL or  out_pts_unit = TD_NULL\n", __func__, __LINE__);
        return;
    }

    /* default unit us */
    *out_pts_unit = PTS_UNIT_US;

    if (in_pts == VFMW_INVALID_PTS && chan->pts_info.first_pts != VFMW_INVALID_PTS) {
        chan->pts_info.first_pts = VFMW_INVALID_PTS;
        return;
    }

    if (chan->pts_info.first_pts == VFMW_INVALID_PTS) {
        chan->pts_info.first_pts = in_pts;
        return;
    }

    if (chan->pts_info.second_pts == VFMW_INVALID_PTS) {
        chan->pts_info.second_pts = in_pts;
        delta = chan->pts_info.second_pts - chan->pts_info.first_pts;
        vfmw_check(frd_check_calc_no_overflow_sub(chan->pts_info.second_pts,
            chan->pts_info.first_pts));
        vfmw_check(delta != 0);

        framerate_ms = (td_s32)(1000 / delta); /* 1s = 1000ms */
        framerate_us = (td_s32)(1000000 / delta); /* 1s = 1000000us */
        if (framerate_ms >= 1 && framerate_ms <= PTS_MAX_FRAME_RATE) {
            *out_pts_unit = PTS_UNIT_MS; /* ms */
            chan->pts_info.calc_unit_done = TD_TRUE;
            return;
        }

        if (framerate_us >= 1 && framerate_us <= PTS_MAX_FRAME_RATE) {
            *out_pts_unit = PTS_UNIT_US; /* us */
            chan->pts_info.calc_unit_done = TD_TRUE;
            return;
        }
    }
}

/*
 For PTS_FRMRATE_TYPE_PTS:      Recover pts and detect frame rate;
 For PTS_FRMRATE_TYPE_STREAM:   Only Recover pts, use frame rate from stream;
 For PTS_FRMRATE_TYPE_USER:     Only Recover pts, use user set frame rate;
 For PTS_FRMRATE_TYPE_USER_PTS: Recover pts and detect frame rate;
 */
STATIC td_s32 pts_info_calc_stamp(td_s32 chan_id, vfmw_image *image)
{
    pts_chan *chan = TD_NULL;
    pts_entry *entry = TD_NULL;

    vfmw_check_ret(image != TD_NULL, EXT_FAILURE);

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "Start first:%d\n", chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    chan->frame_num++;

    if (chan->pts_info.calc_unit_done == TD_FALSE) {
        pts_info_calc_unit(chan, image->src_pts, &chan->pts_info.pts_unit);
    }

    os_sema_up(entry->sema);

    /* src_pts is valid */
    if (image->src_pts != VFMW_INVALID_PTS) {
        pts_info_handle_valid_src_pts(chan_id, image);
    } else {
        pts_info_handle_invalid_src_pts(chan_id, image);
    }

    pts_info_frame_rate_detect(chan_id, image);

    return EXT_SUCCESS;
}

STATIC td_void pts_info_handle_cmd(const cmd_str_ptr cmd_str, td_u32 cmd_cnt)
{
    td_u32 sample_num = 0;
    td_char *cmd_id = ((*cmd_str)[0]);

    vfmw_check_print(cmd_cnt >= 0x2, "cmd param count %d invalid.\n", cmd_cnt);

    vfmw_check(os_strncmp != TD_NULL);
    vfmw_check(os_strlen != TD_NULL);

    /* Parse command */
    if (os_strncmp(cmd_id, LOG_CMD_SAMPLE_NUM, os_strlen(cmd_id)) == 0) {
        str_to_val((*cmd_str)[1], &sample_num); /* 1 means 2nd param */
        vfmw_check_print(sample_num <= FRD_IN_PTS_SEQUENCE_LENGTH,
            "sample_num out %d of range[0,%d]\n", sample_num, FRD_IN_PTS_SEQUENCE_LENGTH);

        g_cfg_sample_num = sample_num;
        dprint(PRN_ALWS, "set pts sample_num %d\n", g_cfg_sample_num);
    } else {
        dprint(PRN_ALWS, "unsupport command '%s', cmd_cnt %d.\n", cmd_id, cmd_cnt);
    }

    return;
}

STATIC td_char *pts_info_show_frm_rate_type(vfmw_pts_framrate_type pts_frm_type)
{
    switch (pts_frm_type) {
        case PTS_FRMRATE_TYPE_PTS:
            return "pts";

        case PTS_FRMRATE_TYPE_STREAM:
            return "STREAM";

        case PTS_FRMRATE_TYPE_USER:
            return "USER";

        case PTS_FRMRATE_TYPE_USER_PTS:
            return "USER_PTS";

        default:
            return "Unkown";
    }
}

STATIC td_s32 pts_info_read_proc(td_void *buffer, td_s32 chan_id)
{
    pts_chan *chan = TD_NULL;
    frd_alg_info *frd_info = TD_NULL;
    vfmw_pts_frmrate *set_fps = TD_NULL;
    td_s32 used = 0;
    const td_s32 len = 0;

    os_dump_proc(buffer, len, &used, 0, LOG_PTS_FMT_HEAD);

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        os_dump_proc(buffer, len, &used, 0, "pts chan %d not exists.%-57s|\r\n", chan_id, " ");
        os_dump_proc(buffer, len, &used, 0, LOG_PTS_FMT_TAIL);
        return EXT_FAILURE;
    }

    frd_info = &(chan->pts_info);
    set_fps = &(chan->user_frame_rate);

    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "chan", chan_id);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "inite", chan->inited);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_S, "type",
                 pts_info_show_frm_rate_type(set_fps->en_frm_rate_type));
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "user_fps", set_fps->st_set_frm_rate.fps_integer,
                 set_fps->st_set_frm_rate.fps_decimal);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_L, "set_frame_time", chan->set_frame_time);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_S, "pts_unit",
                 (frd_info->pts_unit == 1) ? "ms" : "us");
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "frd_enable", frd_info->sample_frd_enable);

    if (frd_info->sample_frd_enable == TD_TRUE) {
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "cfg_sample_num", g_cfg_sample_num);
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "act_sample_num", frd_info->sample_act_num);
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "max_sample_num", frd_info->sample_max_num);
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "max_fps", frd_info->sample_max_rate / 10, /* 10 :a number */
                     frd_info->sample_max_rate % 10); /* 10 :a number */
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "min_fps", frd_info->sample_min_rate / 10, /* 10 :a number */
                     frd_info->sample_min_rate % 10); /* 10 :a number */
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "avg_fps", frd_info->sample_avg_rate / 10, /* 10 :a number */
                     frd_info->sample_avg_rate % 10); /* 10 :a number */
        os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "last_fps",
                     frd_info->sample_last_frame_rate / 1000, /* 1000 :a number */
                     frd_info->sample_last_frame_rate % 1000); /* 1000 :a number */
    }

    os_dump_proc(buffer, len, &used, 0, LOG_FMT_D, "total_frame", chan->frame_num);
    os_dump_proc(buffer, len, &used, 0, LOG_FMT_I_D, "output_fps",
                 frd_info->sample_last_frame_rate / 1000, /* 1000 :a number */
                 frd_info->sample_last_frame_rate % 1000); /* 1000 :a number */
    os_dump_proc(buffer, len, &used, 0, LOG_PTS_FMT_TAIL);

    return EXT_SUCCESS;
}

STATIC td_s32 pts_info_write_proc(cmd_str_ptr buffer, td_u32 count)
{
    pts_info_handle_cmd(buffer, count);

    return (td_s32)count;
}

/* ----------------------------EXTERNAL FUNCTION------------------------------- */
td_s32 pts_info_init(td_void)
{
    pts_entry *entry;

    entry = pts_get_entry();
    entry->inited = TD_TRUE;
    entry->sema = 0;
    os_sema_init(&entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_exit(td_void)
{
    pts_entry *entry;

    entry = pts_get_entry();
    entry->inited = TD_FALSE;
    os_sema_exit(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_alloc(td_s32 chan_id)
{
    pts_entry *entry = TD_NULL;
    pts_chan *chan = TD_NULL;

    entry = pts_get_entry();
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan != TD_NULL) {
        dprint(PRN_ERROR, "%s chan %d already inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    vfmw_check_ret(os_alloc_vir != TD_NULL, EXT_FAILURE);
    chan = os_alloc_vir(chan_id, "pts_inst", sizeof(pts_chan));
    if (chan == TD_NULL) {
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }
    entry->chan[chan_id] = chan;

    pst_info_init_param(chan_id);
    frd_reset(&chan->pts_info, 0);
    vfmw_check_sec_func(memset_s(&chan->user_frame_rate, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
    chan->set_frame_time = VFMW_INVALID_PTS;
    chan->pts_info.cal_pts = VFMW_INVALID_PTS;
    chan->pts_info.que_cnt = 0;
    chan->pts_info.que_stable = 0;
    chan->inited = TD_TRUE;
    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_free(td_s32 chan_id)
{
    pts_chan *chan = TD_NULL;
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    vfmw_check_ret(entry->inited == TD_TRUE, EXT_FAILURE);

    os_sema_down(entry->sema);

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL) {
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }
    if (chan->inited != TD_TRUE) {
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    os_free_vir(chan);
    entry->chan[chan_id] = TD_NULL;

    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_reset(td_s32 chan_id)
{
    pts_chan *chan = TD_NULL;
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    pst_info_init_param(chan_id);
    frd_reset(&chan->pts_info, 0);
    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_set_frm_rate(td_s32 chan_id, const vfmw_pts_frmrate *frame_rate)
{
    td_u32 div_num;
    pts_chan *chan = TD_NULL;
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    vfmw_check_ret(frame_rate != TD_NULL, EXT_FAILURE);

    dprint(PRN_PTS, "%s: Chan %d en_frm_rate_type %d, fps_integer %d, fps_decimal %d.\n", __func__,
           chan_id, frame_rate->en_frm_rate_type, frame_rate->st_set_frm_rate.fps_integer,
           frame_rate->st_set_frm_rate.fps_decimal);

    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    if (frame_rate->en_frm_rate_type == PTS_FRMRATE_TYPE_USER ||
        frame_rate->en_frm_rate_type == PTS_FRMRATE_TYPE_USER_PTS) {
        if (frame_rate->st_set_frm_rate.fps_integer * 1000 >= VFMW_UINT_MAX) { /* 1000 : unit */
            dprint(PRN_ERROR, "%s fps_integer err\n", __func__);
            os_sema_up(entry->sema);
            return EXT_FAILURE;
        }

        /* 1000 :a number */
        div_num = frame_rate->st_set_frm_rate.fps_integer * 1000 + frame_rate->st_set_frm_rate.fps_decimal;

        if ((div_num > PTS_MAX_FRAME_RATE * 1000) || (div_num == 0)) { /* 1000 :a number */
            dprint(PRN_ERROR, "invalid frame rate, Integer(%d), Decimal(%d) \n",
                   frame_rate->st_set_frm_rate.fps_integer,
                   frame_rate->st_set_frm_rate.fps_decimal);
            os_sema_up(entry->sema);
            return EXT_FAILURE;
        }

        chan->set_frame_time = (td_s64)(1000000000 / div_num); /* 1000000000 :a number */
    } else {
        chan->set_frame_time = VFMW_INVALID_PTS;
    }

    vfmw_check_sec_func(memcpy_s(&chan->user_frame_rate, sizeof(chan->user_frame_rate),
                                 frame_rate, sizeof(vfmw_pts_frmrate)));
    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_get_frm_rate(td_s32 chan_id, vfmw_pts_frmrate *frame_rate)
{
    pts_chan *chan = TD_NULL;

    vfmw_check_ret(frame_rate != TD_NULL, EXT_FAILURE);

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        return EXT_FAILURE;
    }

    if (chan->force_pts_mode == 1 ||
        chan->set_frame_time != VFMW_INVALID_PTS) {
        vfmw_check_sec_func(memcpy_s(frame_rate, sizeof(vfmw_pts_frmrate),
                                     &chan->user_frame_rate, sizeof(chan->user_frame_rate)));
    } else {
        vfmw_check_sec_func(memset_s(frame_rate, sizeof(vfmw_pts_frmrate), 0, sizeof(vfmw_pts_frmrate)));
        frame_rate->en_frm_rate_type = chan->user_frame_rate.en_frm_rate_type;
    }

    dprint(PRN_PTS, "%s: Chan %d en_frm_rate_type %d, fps_integer %d, fps_decimal %d.\n", __func__,
           chan_id, frame_rate->en_frm_rate_type, frame_rate->st_set_frm_rate.fps_integer,
           frame_rate->st_set_frm_rate.fps_decimal);

    return EXT_SUCCESS;
}

td_s32 pts_info_convert(td_s32 chan_id, vfmw_image *image)
{
    td_s64 in_pts;
    td_u32 in_frame_rate;
    pts_chan *chan = TD_NULL;
    pts_entry *entry = TD_NULL;

    (td_void)(in_pts); /* avoid to build err when printing is not supported. */
    (td_void)(in_frame_rate);
    entry = pts_get_entry();
    vfmw_check_ret(image != TD_NULL, EXT_FAILURE);
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    if (image->format.frame_rate_valid == 1 && chan->user_frame_rate.en_frm_rate_type != PTS_FRMRATE_TYPE_USER) {
        chan->user_frame_rate.en_frm_rate_type = PTS_FRMRATE_TYPE_STREAM;
        chan->force_pts_mode = 1;
    } else if (image->format.frame_rate_valid == 0 &&
        chan->user_frame_rate.en_frm_rate_type == PTS_FRMRATE_TYPE_STREAM) {
        chan->user_frame_rate.en_frm_rate_type = PTS_FRMRATE_TYPE_PTS;
        chan->force_pts_mode = 0;
    } else {
        chan->force_pts_mode = 0;
    }

    os_sema_up(entry->sema);
    in_pts = image->pts;
    in_frame_rate = image->frame_rate;

    if (pts_info_calc_stamp(chan_id, image) == (td_s32)EXT_FAILURE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
    }

    dprint(PRN_PTS, "pts info: w %d, h %d, pts{src %lld, in %lld, out %lld, delta %d}, frm_rate{in %d, out %d}\n",
           image->image_width, image->image_height,
           image->src_pts, in_pts, image->pts, image->inter_pts_delta,
           in_frame_rate, image->frame_rate);

    return EXT_SUCCESS;
}

td_s32 pts_info_enable_sample_frd(td_s32 chan_id)
{
    pts_chan *chan = TD_NULL;
    frd_alg_info *frd_info = TD_NULL;
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    frd_info = &(chan->pts_info);

    /* Use config sample num in prority */
    g_cur_sample_num = g_cfg_sample_num;
    frd_info->sample_max_num = g_cfg_sample_num;
    frd_info->sample_frd_enable = TD_TRUE;
    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_disable_sample_frd(td_s32 chan_id)
{
    pts_chan *chan = TD_NULL;
    frd_alg_info *frd_info = TD_NULL;
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    os_sema_down(entry->sema);
    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        dprint(PRN_ERROR, "%s chan %d is not inited.\n", __func__, chan_id);
        os_sema_up(entry->sema);
        return EXT_FAILURE;
    }

    frd_info = &(chan->pts_info);

    g_cur_sample_num = 0;
    frd_info->sample_max_num = 0;
    frd_info->sample_frd_enable = TD_FALSE;
    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_read(td_void *buffer, td_s32 chan_id)
{
    pts_entry *entry = TD_NULL;

    entry = pts_get_entry();
    os_sema_down(entry->sema);

    if (pts_info_read_proc(buffer, chan_id) == (td_s32)EXT_FAILURE) {
    }

    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_info_write(cmd_str_ptr buffer, td_u32 count)
{
    pts_entry *entry = TD_NULL;

    vfmw_check_ret(buffer != TD_NULL, EXT_FAILURE);

    entry = pts_get_entry();
    os_sema_down(entry->sema);

    pts_info_write_proc(buffer, count);

    os_sema_up(entry->sema);

    return EXT_SUCCESS;
}

td_s32 pts_get_info(td_s32 chan_id, vfmw_chan_info *vfmw_info)
{
    pts_chan *chan = TD_NULL;
    frd_alg_info *frd_info = TD_NULL;
    vfmw_check_ret(vfmw_info != TD_NULL, EXT_FAILURE);

    chan = pts_get_chan(chan_id);
    if (chan == TD_NULL || chan->inited == TD_FALSE) {
        return EXT_FAILURE;
    }

    frd_info = &(chan->pts_info);

    vfmw_info->stream_frm_rate = frd_info->sample_last_frame_rate / 1000;   /* 1000, a number */

    return EXT_SUCCESS;
}
