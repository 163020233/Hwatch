/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2022. All rights reserved.
 * Description: plc decoder wrap file.
 * Author: audio
 */
#include <stdbool.h>
#include "securec.h"
#include "audio_errno.h"
#include "plc.h"

typedef struct {
    td_u32 erase_cnt;        /* consecutive erased frames */
    td_u32 pitch_overlap;    /* overlap based on pitch */
    td_u32 pitch_offset;     /* offset into pitch period */
    td_u32 pitch;            /* pitch estimate */
    td_u32 pitch_buf_len;    /* current pitch buffer length */
    td_u8 *pitch_buf_end;    /* end of pitch buffer */
    td_u8 *pitch_buf_start;  /* start of pitch buffer */
    td_u8 *pitch_buf;        /* buffer for cycles of speech */
    td_u8 *last_quarter;     /* saved last quarter wavelength */
    td_u8 *history;          /* history buffer */
    td_u32 bit_depth;
    td_u32 bit_bytes;
    td_u32 ch;
    td_u32 history_len;
    td_u32 overlap_max;
    td_u32 overlap_incr;
} pcm_plc_state;


static td_s16 scale_sat_16bit(td_s64 t)
{
    if (t > ((td_s64)(td_s16)MAX_INT16 * 65535)) {         /* 32767 is Q15, 65535 is Q16 */
        t = MAX_INT16;                     /* 32767 is Q15 */
    } else if (t < ((td_s64)(td_s16)MIN_INT16 * 65535)) { /* 32768 is Q15, 65535 is Q16 */
        t = MIN_INT16;                    /* 32768 is Q15 */
    } else {
        t = (t >> 16); /* 16 is bit */
    }

    return (td_s16)t;
}


static td_s32 scale_sat_32bit(td_s64 t)
{
    if (t > ((td_s64)(td_s32)MAX_INT32 * 65535)) {         /* 32767 is Q15, 65535 is Q16 */
        t = MAX_INT32;                     /* 32767 is Q15 */
    } else if (t < ((td_s64)(td_s32)MIN_INT32 * 65535)) { /* 32768 is Q15, 65535 is Q16 */
        t = MIN_INT32;                    /* 32768 is Q15 */
    } else {
        t = (t >> 16); /* 16 is bit */
    }

    return (td_s32)t;
}

static td_s32 get_24bit_num(td_u8 *ptr)
{
    td_s32 res = (td_s32)(((td_u32)ptr[0] << PLC_BIT_SHIFT_8) |
        ((td_u32)ptr[PLC_VAR_1] << PLC_BIT_SHIFT_16) |
        ((td_u32)ptr[PLC_VAR_2] << PLC_BIT_SHIFT_24));
    return res;
}

td_void plc_deinit(plc_handle h_state)
{
    pcm_plc_state *plc_state = (pcm_plc_state *)h_state;
    if (plc_state == NULL) {
        return;
    }
    if (plc_state->pitch_buf != NULL) {
        free(plc_state->pitch_buf);
    }

    if (plc_state->last_quarter != NULL) {
        free(plc_state->last_quarter);
    }

    if (plc_state->history != NULL) {
        free(plc_state->history);
    }

    free(plc_state);

    return;
}

td_s32 plc_init(plc_handle *h_state, td_u32 samples_per_frame,
    td_u32 ch, td_u32 bit_depth)
{
    pcm_plc_state *plc_state = TD_NULL;
    plc_state = (pcm_plc_state *)calloc(1, sizeof(pcm_plc_state));
    if (plc_state == TD_NULL) {
        return ERR_HACODEC_ALLOC_MEM_FAILED;
    }
    plc_state->history_len = samples_per_frame * ch;
    plc_state->bit_depth = bit_depth;
    plc_state->ch = ch;
    plc_state->bit_bytes = bit_depth >> 3; /* 3bits */
    plc_state->overlap_max = plc_state->history_len >> 1;
    plc_state->overlap_incr = (24 * plc_state->history_len / 60); /* 24 60 is number */

    plc_state->erase_cnt = 0;
    plc_state->pitch_buf = (td_u8 *)calloc(1, plc_state->bit_bytes * plc_state->history_len);
    plc_state->last_quarter = (td_u8 *)calloc(1, plc_state->bit_bytes * plc_state->overlap_max);
    plc_state->history = (td_u8 *)calloc(1, plc_state->bit_bytes * plc_state->history_len);
    if ((plc_state->pitch_buf == NULL) || (plc_state->last_quarter == NULL) || (plc_state->history == NULL)) {
        plc_deinit((plc_handle)plc_state);
        return ERR_HACODEC_ALLOC_MEM_FAILED;
    }

    plc_state->pitch_buf_end = &plc_state->pitch_buf[plc_state->history_len * plc_state->bit_bytes];

    *h_state = (plc_handle)plc_state;
    return AUDIO_SUCCESS;
}

static td_void save_speech(pcm_plc_state *plc_state, td_s32 *s)
{
    td_s32 ret;

    /* copy in the new frame */
    ret = memcpy_s(plc_state->history, plc_state->history_len * plc_state->bit_bytes,
        s, plc_state->bit_bytes * plc_state->history_len);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    return;
}

/*
 * Overlap add the end of the erasure with the start of the first good frame
 * Scale the synthetic speech by the gain factor before the OLA.
 */
static td_void overlap_add_at_end_16bit_2ch(pcm_plc_state *plc_state, td_s16 *s, td_s16 *f, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t1, t2;

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        t1 = ((td_s64)lw * f[cnt - 1 - i] + (td_s64)rw * s[i]);
        t2 = ((td_s64)lw * f[cnt - 1 - i - 1] + (td_s64)rw * s[i + 1]);

        s[i] = scale_sat_16bit(t1);
        s[i + 1] = scale_sat_16bit(t2);
        lw -= incrg;
        rw += incr;
    }
    return;
}

static td_void overlap_add_at_end_16bit(pcm_plc_state *plc_state, td_s16 *s, td_s16 *f, td_u32 ch, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t;
    if (ch == 2) { /* 2ch */
        overlap_add_at_end_16bit_2ch(plc_state, s, f, cnt);
        return;
    }

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        t = ((td_s64)lw * f[cnt - 1 - i] + (td_s64)rw * s[i]);

        s[i] = scale_sat_16bit(t);
        lw -= incrg;
        rw += incr;
    }
    return;
}

/*
 * Overlap add the end of the erasure with the start of the first good frame
 * Scale the synthetic speech by the gain factor before the OLA.
 */
static td_void overlap_add_at_end_32bit_2ch(pcm_plc_state *plc_state, td_s32 *s, td_s32 *f, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t1, t2;

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        t1 = ((td_s64)lw * f[cnt - 1 - i] + (td_s64)rw * s[i]);
        t2 = ((td_s64)lw * f[cnt - 1 - i - 1] + (td_s64)rw * s[i + 1]);

        s[i] = scale_sat_32bit(t1);
        s[i + 1] = scale_sat_32bit(t2);
        lw -= incrg;
        rw += incr;
    }
    return;
}

static td_void overlap_add_at_end_32bit(pcm_plc_state *plc_state, td_s32 *s, td_s32 *f, td_u32 ch, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t;

    if (ch == 2) { /* 2ch */
        overlap_add_at_end_32bit_2ch(plc_state, s, f, cnt);
        return;
    }

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        t = ((td_s64)lw * f[cnt - 1 - i] + (td_s64)rw * s[i]);

        s[i] = scale_sat_32bit(t);
        lw -= incrg;
        rw += incr;
    }
    return;
}

/*
 * Overlap add the end of the erasure with the start of the first good frame
 * Scale the synthetic speech by the gain factor before the OLA.
 */
static td_void overlap_add_at_end_24bit_2ch(pcm_plc_state *plc_state, td_u8 *s, td_u8 *f, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t1, t2;
    td_s32 s_data1, f_data1, s_data2, f_data2, cur_res1, cur_res2;

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        f_data1 = get_24bit_num(f + PLC_VAR_3 * (td_s64)(cnt - 1 - i));
        f_data2 = get_24bit_num(f + PLC_VAR_3 * (td_s64)(cnt - 1 - i - 1));
        s_data1 = get_24bit_num(s + PLC_VAR_3 * (td_s64)i);
        s_data2 = get_24bit_num(s + PLC_VAR_3 * (td_s64)(i + 1));
        t1 = ((td_s64)lw * f_data1 + (td_s64)rw * s_data1);
        t2 = ((td_s64)lw * f_data2 + (td_s64)rw * s_data2);
        cur_res1 = scale_sat_32bit(t1);
        cur_res2 = scale_sat_32bit(t2);

        s[PLC_VAR_3 * i] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_8);
        s[PLC_VAR_3 * i + PLC_VAR_1] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_16);
        s[PLC_VAR_3 * i + PLC_VAR_2] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_24);

        s[PLC_VAR_3 * (i + 1)] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_8);
        s[PLC_VAR_3 * (i + 1) + PLC_VAR_1] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_16);
        s[PLC_VAR_3 * (i + 1) + PLC_VAR_2] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_24);
        lw -= incrg;
        rw += incr;
    }
    return;
}

static td_void overlap_add_at_end_24bit(pcm_plc_state *plc_state, td_u8 *s, td_u8 *f, td_u32 ch, td_u32 cnt)
{
    td_s32 incr = 0;
    td_s32 gain = (1 << 15) - ((td_s32)plc_state->erase_cnt - 1) * ATTEN_FAC; /* 15 is bit */
    td_s32 incrg;
    td_s32 lw, rw;
    td_u32 i;
    td_s64 t;
    td_s32 s_data, f_data, cur_res;
    if (ch == 2) { /* 2ch */
        overlap_add_at_end_24bit_2ch(plc_state, s, f, cnt);
        return;
    }

    if (gain < 0) {
        gain = 0;
    }

    if (cnt != 0) {
        incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    } else {
        incr = 65535;       /* 65535 is Q16 */
    }

    incrg = (incr * gain) >> 15;            /* 15 is bit */
    lw = (((1 << 16) - incr) * gain) >> 15; /* 15,16 is bit */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        f_data = get_24bit_num(f + PLC_VAR_3 * (cnt - 1 - i));
        s_data = get_24bit_num(s + PLC_VAR_3 * i);
        t = ((td_s64)lw * f_data + (td_s64)rw * s_data);

        cur_res = scale_sat_32bit(t);

        s[PLC_VAR_3 * i] = (td_u8)(cur_res >> PLC_BIT_SHIFT_8);
        s[PLC_VAR_3 * i + PLC_VAR_1] = (td_u8)(cur_res >> PLC_BIT_SHIFT_16);
        s[PLC_VAR_3 * i + PLC_VAR_2] = (td_u8)(cur_res >> PLC_BIT_SHIFT_24);
        lw -= incrg;
        rw += incr;
    }
    return;
}

/*
 * Overlap add the end of the erasure with the start of the first good frame
 * Scale the synthetic speech by the gain factor before the OLA.
 */
static td_void overlap_add_at_end(pcm_plc_state *plc_state, td_u8 *s, td_u8 *f, td_u32 bit_depth, td_u32 ch, td_u32 cnt)
{
    if (bit_depth == PLC_BIT_DEPTH_16) {
        overlap_add_at_end_16bit(plc_state, (td_s16 *)s, (td_s16 *)f, ch, cnt);
    } else if (bit_depth == PLC_BIT_DEPTH_24) {
        overlap_add_at_end_24bit(plc_state, (td_u8 *)s, (td_u8 *)f, ch, cnt);
    } else {
        overlap_add_at_end_32bit(plc_state, (td_s32 *)s, (td_s32 *)f, ch, cnt);
    }
    return;
}

/*
 * A good frame was received and decoded.
 * If right after an erasure, do an overlap add with the synthetic signal.
 * Add the frame to history buffer.
 */
static td_s32 overlap_buf[FRAMESZ];
td_void plc_add_history_data(plc_handle h_state, td_s32 *s)
{
    pcm_plc_state *plc_state = (pcm_plc_state *)h_state;
    td_u32 overlap_len;
    td_s32 ret;
    if (plc_state->erase_cnt != 0) {
        /*
         * longer erasures require longer overlaps
         * to smooth the transition between the synthetic
         * and real signal.
         */
        overlap_len = plc_state->pitch_overlap + (plc_state->erase_cnt - 1) * plc_state->overlap_incr;
        if (overlap_len > plc_state->history_len) {
            overlap_len = plc_state->history_len;
        }

        ret = memcpy_s(overlap_buf, FRAMESZ * sizeof(td_s32),
            &plc_state->history[(plc_state->history_len - plc_state->pitch_overlap) * plc_state->bit_bytes],
            plc_state->pitch_overlap * plc_state->bit_bytes);
        if (ret != EOK) {
            sap_err_log_fun(memcpy_s, ret);
        }

        overlap_add_at_end(plc_state, (td_u8 *)s, (td_u8 *)overlap_buf,
            plc_state->bit_depth, plc_state->ch, overlap_len);
        plc_state->erase_cnt = 0;
    }
    save_speech(plc_state, s);
    return;
}

static td_void overlap_add2_16bit_2ch(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 cnt)
{
    td_s16 *l1, *r1, *o1;
    td_s32 incr, lw, rw;
    td_u32 i;
    td_s64 t1, t2;
    if (cnt == 0) {
        return;
    }

    l1 = (td_s16 *)l;
    r1 = (td_s16 *)r;
    o1 = (td_s16 *)o;

    incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        t1 = (td_s64)lw * (td_s64)l1[cnt - 1 - i] + (td_s64)rw * (td_s64)r1[i];
        t2 = (td_s64)lw * (td_s64)l1[cnt - 1 - i - 1] + (td_s64)rw * (td_s64)r1[i + 1];
        o1[i] = (td_s16)scale_sat_16bit(t1);
        o1[i + 1] = (td_s16)scale_sat_16bit(t2);
        lw -= incr;
        rw += incr;
    }
}

static td_void overlap_add2_16bit(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 ch, td_u32 cnt)
{
    td_s16 *l1, *r1, *o1;
    td_s32 incr, lw, rw;
    td_u32 i;
    td_s64 t;

    if (ch == PLC_VAR_2) {
        overlap_add2_16bit_2ch(l, r, o, cnt);
        return;
    }

    if (cnt == 0) {
        return;
    }

    l1 = (td_s16 *)l;
    r1 = (td_s16 *)r;
    o1 = (td_s16 *)o;

    incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        t = (td_s64)lw * (td_s64)l1[cnt - 1 - i] + (td_s64)rw * (td_s64)r1[i];
        o1[i] = scale_sat_16bit(t);
        lw -= incr;
        rw += incr;
    }

    return;
}

static td_void overlap_add2_32bit_2ch(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 cnt)
{
    td_s32 *l1, *r1, *o1;
    td_s32 incr, lw, rw;
    td_u32 i;
    td_s64 t1, t2;

    l1 = (td_s32 *)l;
    r1 = (td_s32 *)r;
    o1 = (td_s32 *)o;

    incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        t1 = (td_s64)lw * (td_s64)l1[cnt - 1 - i] + (td_s64)rw * (td_s64)r1[i];
        t2 = (td_s64)lw * (td_s64)l1[cnt - 1 - i - 1] + (td_s64)rw * (td_s64)r1[i + 1];
        o1[i] = scale_sat_32bit(t1);
        o1[i + 1] = scale_sat_32bit(t2);

        lw -= incr;
        rw += incr;
    }
}

static td_void overlap_add2_32bit(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 ch, td_u32 cnt)
{
    td_s32 *l1, *r1, *o1;
    td_s32 incr, lw, rw;
    td_u32 i;
    td_s64 t;
    if (cnt == 0) {
        return;
    }

    if (ch == 2) { /* 2ch */
        overlap_add2_32bit_2ch(l, r, o, cnt);
        return;
    }

    l1 = (td_s32 *)l;
    r1 = (td_s32 *)r;
    o1 = (td_s32 *)o;

    incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        t = (td_s64)lw * (td_s64)l1[cnt - 1 - i] + (td_s64)rw * (td_s64)r1[i];
        o1[i] = scale_sat_32bit(t);

        lw -= incr;
        rw += incr;
    }

    return;
}

static td_void overlap_add2_24bit_2ch(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 cnt)
{
    td_s32 incr, lw, rw, l_data1, r_data1, l_data2, r_data2, cur_res1, cur_res2;
    td_u32 i;
    td_s64 t1, t2;

    if (cnt == 0) {
        return;
    }

    incr = (td_s32)((65535 / cnt) << 1); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt - 1; i += 2) { /* 2 step */
        l_data1 = get_24bit_num(l + PLC_VAR_3 * (td_s64)(cnt - 1 - i));
        l_data2 = get_24bit_num(l + PLC_VAR_3 * (td_s64)(cnt - 1 - i - 1));
        r_data1 = get_24bit_num(r + PLC_VAR_3 * (td_s64)i);
        r_data2 = get_24bit_num(r + PLC_VAR_3 * (td_s64)(i + 1));
        t1 = (td_s64)lw * (td_s64)l_data1 + (td_s64)rw * (td_s64)r_data1;
        t2 = (td_s64)lw * (td_s64)l_data2 + (td_s64)rw * (td_s64)r_data2;
        cur_res1 = scale_sat_32bit(t1);
        cur_res2 = scale_sat_32bit(t2);

        o[PLC_VAR_3 * i] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_8);
        o[PLC_VAR_3 * i + PLC_VAR_1] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_16);
        o[PLC_VAR_3 * i + PLC_VAR_2] = (td_u8)(cur_res1 >> PLC_BIT_SHIFT_24);

        o[PLC_VAR_3 * (i + 1)] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_8);
        o[PLC_VAR_3 * (i + 1) + PLC_VAR_1] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_16);
        o[PLC_VAR_3 * (i + 1) + PLC_VAR_2] = (td_u8)(cur_res2 >> PLC_BIT_SHIFT_24);
        lw -= incr;
        rw += incr;
    }

    return;
}

static td_void overlap_add2_24bit(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 ch, td_u32 cnt)
{
    td_s32 incr, lw, rw, l_data, r_data, cur_res;
    td_u32 i;
    td_s64 t;
    if (ch == 2) { /* 2ch */
        overlap_add2_24bit_2ch(l, r, o, cnt);
        return;
    }

    if (cnt == 0) {
        return;
    }

    incr = (td_s32)(65535 / cnt); /* 65535 is Q16 */
    lw = 65535 - incr;  /* 65535 is Q16 */
    rw = incr;

    for (i = 0; i < cnt; i++) {
        l_data = get_24bit_num(l + PLC_VAR_3 * (cnt - 1 - i));
        r_data = get_24bit_num(r + PLC_VAR_3 * i);
        t = (td_s64)lw * (td_s64)l_data + (td_s64)rw * (td_s64)r_data;
        cur_res = scale_sat_32bit(t);

        o[PLC_VAR_3 * i] = (td_u8)(cur_res >> PLC_BIT_SHIFT_8);
        o[PLC_VAR_3 * i + PLC_VAR_1] = (td_u8)(cur_res >> PLC_BIT_SHIFT_16);
        o[PLC_VAR_3 * i + PLC_VAR_2] = (td_u8)(cur_res >> PLC_BIT_SHIFT_24);
        lw -= incr;
        rw += incr;
    }

    return;
}

/*
 * Overlap add left and right sides
 */
static td_void overlap_add2(td_u8 *l, td_u8 *r, td_u8 *o, td_u32 bit_depth, td_u32 ch, td_u32 cnt)
{
    if (cnt == 0) {
        return;
    }

    if (bit_depth == PLC_BIT_DEPTH_16) {
        overlap_add2_16bit(l, r, o, ch, cnt);
    } else if (bit_depth == PLC_BIT_DEPTH_24) {
        overlap_add2_24bit(l, r, o, ch, cnt);
    } else {
        overlap_add2_32bit(l, r, o, ch, cnt);
    }

    return;
}

static td_void plc_one_frame(pcm_plc_state *plc_state, td_u8 *out)
{
    td_s32 ret;
    td_u32 bit_bytes = plc_state->bit_bytes;
    ret = memcpy_s(plc_state->pitch_buf, plc_state->history_len * bit_bytes, plc_state->history,
        plc_state->history_len * bit_bytes);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }
    plc_state->pitch_overlap = plc_state->history_len >> 2; /* shift right 2 bits */
    plc_state->pitch = plc_state->pitch_overlap;
    /* save original last plc_state->pitch_overlap samples */
    ret = memcpy_s(plc_state->last_quarter, plc_state->bit_bytes * plc_state->overlap_max,
        plc_state->pitch_buf_end - plc_state->pitch_overlap * bit_bytes, bit_bytes * plc_state->pitch_overlap);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    plc_state->pitch_buf_len = plc_state->pitch;
    plc_state->pitch_buf_start = plc_state->pitch_buf_end - plc_state->history_len * bit_bytes;
    overlap_add2(plc_state->last_quarter, plc_state->pitch_buf_start,
        plc_state->pitch_buf_start, plc_state->bit_depth, plc_state->ch, plc_state->pitch_overlap);

    ret = memcpy_s(out, plc_state->pitch_overlap * plc_state->bit_bytes,
        plc_state->pitch_buf_start, plc_state->pitch_overlap * plc_state->bit_bytes);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    ret = memcpy_s(out + plc_state->pitch_overlap * plc_state->bit_bytes,
        (plc_state->history_len - plc_state->pitch_overlap) * bit_bytes,
        plc_state->history + plc_state->pitch_overlap * plc_state->bit_bytes,
        (plc_state->history_len - plc_state->pitch_overlap) * bit_bytes);
    if (ret != EOK) {
        sap_err_log_fun(memcpy_s, ret);
    }

    return;
}

td_void plc_decode_frame(plc_handle h_state, td_s32 *out)
{
    pcm_plc_state *plc_state = (pcm_plc_state *)h_state;
    if (plc_state->erase_cnt == 0) {
        plc_one_frame(plc_state, (td_u8 *)out);
    }

    plc_state->erase_cnt++;
    save_speech(plc_state, out);
    return;
}
