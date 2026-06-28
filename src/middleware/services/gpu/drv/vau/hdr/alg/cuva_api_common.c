/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : api for cuva hdr
 */

#include "cuva_api_common.h"
#include "cuva_coef_common.h"
#include "td_base.h"
#include "securec.h"

static td_void cuva_calparam_base_curve_maxlum(const cuva_api_param *pst_cuva_in_param,
    cuva_curve_param *pst_curve_param, td_u32 max_display_mastering_lum, td_u32 *p_max_lum);

static td_void cuva_calparam_base_curve_proc0(const cuva_api_param *pst_cuva_in_param, td_u32 max_lum,
    const cuva_curve_param *pst_curve_param, cuva_base_curve_param *pst_base_curve_param);

static td_s32 cuva_calparam_base_curve_usemetadata(const cuva_api_param *pst_cuva_in_param, td_u32 idx,
    cuva_base_curve_param *pst_base_curve_param);

static td_void cuva_calparam_base_curve_proc1(td_u32 targeted_system_display_maxlum, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_base_curve_proc2(cuva_api_param *pst_cuva_in_param, td_u32 targeted_system_display_maxlum,
    td_u32 max_lum, cuva_curve_param *pst_curve_param);

static td_u32 cuva_calparam_3spline_curve_part1(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_1spline_curve_proc0(const cuva_api_param *pst_cuva_in_param,
    cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_1spline_curve_proc1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_s64 cuva_func_hl_pow(const cuva_base_curve_param *pst_base_curve_param, td_s64 lum, td_s64 m_m);

static td_s64 cuva_func_hl(cuva_base_curve_param *pst_base_curve_param, td_s64 lum, td_s64 m_a, td_s64 m_m);

static td_s64 cuva_calparam_3spline_curve_dgd(cuva_curve_param *pst_curve_param, td_s64 th);

static td_s64 cuva_calparam_3spline_curve_gd(cuva_curve_param *pst_curve_param, td_s64 th);

static td_void cuva_1spline_curve_linear_interp_tmap(td_s64 m_p, td_s64 *p_m_a_t);

static td_void cuva_calparam_1spline_curve_adjust0(cuva_curve_param *pst_curve_param, td_u32 max_lum);

static td_void cuva_calparam_3spline_curve_proc0(cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc0_part1(cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc0_part2(cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc1(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc1_part1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc1_part2(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc1_part2_p1(cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc2(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc2_part1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc2_part2(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc2_part3(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param);

static td_void cuva_calparam_3spline_curve_proc2_part4(cuva_curve_param *pst_curve_param);

static td_s32 cuva_calparam_range_conversion_tm_3spline(cuva_curve_param *pst_curve_param, td_u32 ye_in,
    td_s64 *y_in_tm);

static td_u32 cuva_3spline_estimate_monotony_curve1(const cuva_curve_param *pst_curve_param, td_u32 idx_3spline);

static td_u32 cuva_3spline_estimate_monotony_curve2(const cuva_curve_param *pst_curve_param, td_u32 idx_3spline);

td_u64 linear_2_pq(td_float input)
{
    td_float m1 = 2610.0f / 4096.0f / 4.0f;
    td_float m2 = 2523.0f / 4096.0f * 128.0f;
    td_float c1 = 3424.0f / 4096.0f;
    td_float c2 = 2413.0f / 4096.0f * 32.0f;
    td_float c3 = 2392.0f / 4096.0f * 32.0f;

    td_float indata = pq_clip3(0.0f, 1.0f, input);
    td_float tmp = pow(indata, m1);
    td_float outdata = pow((c1 + c2 * tmp) / (1 + c3 * tmp), m2);
    td_u64 output = (td_u64)(outdata * 65535.0f + 0.5f);  // 65535.0f: 2^16-1, 0.5f: round

    return output;
}

td_void pq_2_linear(td_u32 input, td_u64 *output)
{
    td_float m1 = 2610.0f / 4096.0f / 4.0f;
    td_float m2 = 2523.0f / 4096.0f * 128.0f;
    td_float c1 = 3424.0f / 4096.0f;
    td_float c2 = 2413.0f / 4096.0f * 32.0f;
    td_float c3 = 2392.0f / 4096.0f * 32.0f;

    td_float indata = pq_clip3(0.0f, 1.0f, ((td_float)input / 65535.0f));

    td_float tmp = pow(indata, 1.0f / m2);
    td_float outdata = pow(uapi_max(tmp - c1, 0) / (c2 - c3 * tmp), (1.0f / m1));

    *output = (td_u64)(outdata * 2621440000); /* 2621440000: para */
}

td_s64 metadata_pow_all(td_s64 a, td_s64 b)
{
    td_double x = (td_double)a / (1 << CUVAHDR_BASE_S);
    td_double y = (td_double)b / (1 << CUVAHDR_BASE_S);
    td_double z = pow(x, y);
    td_s64 out = (td_s64)(z * (1 << CUVAHDR_BASE_S));

    return out;
}

td_void cuva_init_display(cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_curve_param)
{
    const td_s64 lum_gain = 10000; /* 10000: gain of luminance. */
    td_float minlum_display;
    td_float maxlum_display;

    /* Exception processing: limit the value of maxlum_display */
    if (pst_cuva_in_param->maxlum_display < CUVA_MAXLUM_DISPLAY_SDR) {
        pst_cuva_in_param->maxlum_display = CUVA_MAXLUM_DISPLAY_SDR;
    }

    minlum_display = (td_float)pst_cuva_in_param->minlum_display / 100000000; // 100000000: 10000x10000
    pst_curve_param->minlum_display_pq = (td_u32)linear_2_pq(minlum_display);

    maxlum_display = (td_float)pst_cuva_in_param->maxlum_display / 100000000; // 100000000: 10000x10000
    pst_curve_param->maxlum_display_pq = (td_u32)linear_2_pq(maxlum_display);

    pst_cuva_in_param->maxlum_display = (td_u32)(pst_cuva_in_param->maxlum_display / lum_gain);
    pst_cuva_in_param->maxlum_mastering = (td_u32)(pst_cuva_in_param->maxlum_mastering / lum_gain);
}

/* H(L)_pow : ((m_p * L^m_n) / ((k1 * m_p - k2) * L^m_n + k3))^m_m */
td_s64 cuva_func_hl_pow(const cuva_base_curve_param *pst_base_curve_param, td_s64 lum, td_s64 m_m)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 k1, k2, k3, m_p, m_n;
    td_s64 lum_pow_mn, h_lum_base, h_lum_base_div, h_lum;
    td_s64 res;
    td_s64 m_m_in = m_m;
    td_s64 lum_in = lum;

    m_m_in = (((m_m_in) * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    m_n = (((pst_base_curve_param->m_n) * (td_s64)(one << CUVAHDR_BASE_S)) +
         maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    m_p = (((pst_base_curve_param->m_p) * (td_s64)(one << CUVAHDR_BASE_S)) +
         maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    k1 = (((pst_base_curve_param->k1) * (td_s64)(one << CUVAHDR_BASE_S)) +
        maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    k2 = (((pst_base_curve_param->k2) * (td_s64)(one << CUVAHDR_BASE_S)) +
        maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    k3 = (((pst_base_curve_param->k3) * (td_s64)(one << CUVAHDR_BASE_S)) +
        maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    lum_in = ((lum_in * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);

    lum_pow_mn = metadata_pow_all(lum_in, m_n); /* bit of input and output is CUVAHDR_BASE_S */

    h_lum_base_div = (k1 * m_p - k2 * (td_s64)(one << CUVAHDR_BASE_S) + (td_s64)(one << (CUVAHDR_BASE_S - 1))) /
        (td_s64)(one << CUVAHDR_BASE_S);

    h_lum_base_div = (((td_s64)(h_lum_base_div * lum_pow_mn) + (td_s64)(one << (CUVAHDR_BASE_S - 1))) /
        (td_s64)(one << CUVAHDR_BASE_S)) + k3;

    if (h_lum_base_div != 0) {
        h_lum_base = m_p * lum_pow_mn / h_lum_base_div;
    } else {
        h_lum_base = 0;
    }

    h_lum = metadata_pow_all(h_lum_base, m_m_in);
    res = ((h_lum * (td_s64)(one << CUVA_PROCESS_BIT)) + (td_s64)(one << (CUVAHDR_BASE_S - 1))) /
        (td_s64)(one << CUVAHDR_BASE_S);

    return res;
}

/* H(L) : m_a * H(L)_pow + m_b */
td_s64 cuva_func_hl(cuva_base_curve_param *pst_base_curve_param, td_s64 lum, td_s64 m_a, td_s64 m_m)
{
    const td_u64 one = 1;
    td_s64 h_lum_pow;
    td_s64 res;

    h_lum_pow = cuva_func_hl_pow(pst_base_curve_param, lum, m_m);

    res = ((m_a * h_lum_pow) + (pst_base_curve_param->m_b * (td_s64)(one << CUVA_PROCESS_BIT)) +
        (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);

    return res;
}

/* bit-depth of input is CUVA_PROCESS_BIT */
td_s64 cuva_calparam_3spline_curve_dgd(cuva_curve_param *pst_curve_param, td_s64 th)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 dgd_a;
    td_s64 th_pow_mn, dgd_b;
    td_s64 dgd;
    td_s64 m_m_plus1;
    td_s64 m_n, th_shift;

    m_m_plus1 = pst_curve_param->base_param.m_m + maxval_base_param_scale;
    dgd_a = cuva_func_hl_pow(&(pst_curve_param->base_param), th, m_m_plus1); /* bit is CUVA_PROCESS_BIT */

    m_n = (((pst_curve_param->base_param.m_n) * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    th_shift = (((th) * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    th_pow_mn = metadata_pow_all(th_shift, m_n); /* bit of input and output is CUVAHDR_BASE_S */
    th_pow_mn = ((th_pow_mn * (td_s64)(one << CUVA_PROCESS_BIT)) +
        (td_s64)(one << (CUVAHDR_BASE_S - 1))) / (td_s64)(one << CUVAHDR_BASE_S);
    dgd_b = (th_pow_mn * pst_curve_param->base_param.m_p + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    dgd_b = (td_s64)(dgd_b * dgd_b); /* bit is (2 CUVA_PROCESS_BIT) */

    if (dgd_b == 0) {
        dgd = 0;
    } else {
        dgd = ((dgd_a * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) + dgd_b / DIVISOR_2) / dgd_b; /* 2: parameters */
    }

    return dgd;
}

td_s64 cuva_calparam_3spline_curve_gd(cuva_curve_param *pst_curve_param, td_s64 th)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 dgd, m_n_sub1, th_shift, th_mn_sub1, gd;

    dgd = cuva_calparam_3spline_curve_dgd(pst_curve_param, th);

    m_n_sub1 = ((((pst_curve_param->base_param.m_n - maxval_base_param_scale)) * (td_s64)(one << CUVAHDR_BASE_S)) +
        maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    th_shift = (((th) * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    th_mn_sub1 = metadata_pow_all(th_shift, m_n_sub1); /* bit of input and output is CUVAHDR_BASE_S */
    th_mn_sub1 = ((th_mn_sub1 * (td_s64)(one << CUVA_PROCESS_BIT)) + (td_s64)(one << (CUVAHDR_BASE_S - 1))) /
        (td_s64)(one << CUVAHDR_BASE_S);

    gd = (pst_curve_param->base_param.m_a * pst_curve_param->base_param.m_m * pst_curve_param->base_param.m_p +
        (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2: parameters */
    gd = (gd * pst_curve_param->base_param.k3 * pst_curve_param->base_param.m_n +
        (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2: parameters */
    gd = ((td_s64)(gd * th_mn_sub1 * dgd) +
        (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2: parameters */

    return gd;
}

td_s32 cuva_calparam_range_conversion_tm_3spline(cuva_curve_param *pst_curve_param, td_u32 ye_in, td_s64 *y_in_tm)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 p_3spline_num = pst_curve_param->p_3spline_num;
    td_s64 l1, l2, tmp;
    td_s64 idx_3spline = 0;

    /* 3) 1st 3spline curve, 1: parameters */
    if ((p_3spline_num >= 1) && (ye_in >= pst_curve_param->th1[1]) && (ye_in <= pst_curve_param->th3[1])) {
        idx_3spline = 1;
        /* 3) 2cd 3spline curve, 2: parameters */
    } else if ((p_3spline_num >= 2) && (ye_in >= pst_curve_param->th1[2]) && (ye_in <= pst_curve_param->th3[2])) {
        idx_3spline = 2; /* 2: parameters */
    } else {
        return EXT_FAILURE;
    }

    l1 = (td_s64)ye_in - pst_curve_param->th1[idx_3spline];
    l2 = (td_s64)ye_in - pst_curve_param->th2[idx_3spline];

    if ((ye_in >= pst_curve_param->th1[idx_3spline]) && (ye_in <= pst_curve_param->th2[idx_3spline])) {
        tmp = (td_s64)(pst_curve_param->md[0][idx_3spline] *
            ((l1 * l1 * l1 + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT)));
        *y_in_tm = (tmp + pst_curve_param->mc[0][idx_3spline] * l1 * l1 +
            pst_curve_param->mb[0][idx_3spline] * l1 * maxval_base_param_scale +
            pst_curve_param->ma[0][idx_3spline] * maxval_base_param_scale * maxval_base_param_scale +
            (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2: parameters */
    } else if ((ye_in > pst_curve_param->th2[idx_3spline]) && (ye_in <= pst_curve_param->th3[idx_3spline])) {
        tmp = (td_s64)(pst_curve_param->md[1][idx_3spline] *
            ((l2 * l2 * l2 + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT)));
        *y_in_tm = (tmp + pst_curve_param->mc[1][idx_3spline] * l2 * l2 +
            pst_curve_param->mb[1][idx_3spline] * l2 * maxval_base_param_scale +
            pst_curve_param->ma[1][idx_3spline] * maxval_base_param_scale * maxval_base_param_scale +
            (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2: parameters */
    }

    return EXT_SUCCESS;
}

/* 9.4 */
/* ye_in:  non-linear, CUVA_PROCESS_BIT */
/* ye_out: non-linear, CUVA_PROCESS_BIT */
/* *p_k:   ratio in linear, CUVA_PROCESS_BIT */
td_s32 cuva_calparam_range_conversion_tm(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    cuva_curve_param *pst_curve_param, td_u32 ye_in, td_s64 *y_in_tm)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 p_3spline_num = pst_curve_param->p_3spline_num;
    td_s64 beyong_3spline_en_0, beyong_3spline_en_1, beyong_3spline_en_2_mode3, beyong_3spline_en_2_mode12;
    td_s64 l1, k_line, b_line;
    td_s32 res;

    /* b. max_rgb_in_tm */
    beyong_3spline_en_0 = ((p_3spline_num == 1) && (ye_in > pst_curve_param->th3[1]));
    beyong_3spline_en_1 = ((p_3spline_num == 2) &&                                               /* 2:pa */
        ((ye_in > pst_curve_param->th3[1]) && (ye_in < pst_curve_param->th1[2])));               /* 2:pa */
    beyong_3spline_en_2_mode3 = ((p_3spline_num == 2) && (ye_in > pst_curve_param->th3[2]) &&    /* 2:pa */
        (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][idx_tm] == 3));                 /* 3:pa */
    beyong_3spline_en_2_mode12 = ((p_3spline_num == 2) && (ye_in > pst_curve_param->th3[2]) &&   /* 2:pa */
        ((pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][idx_tm] == 1) ||
        (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][idx_tm] == 2)));                /* 2:pa */

    if (beyong_3spline_en_0 != 0 || beyong_3spline_en_1 != 0 || beyong_3spline_en_2_mode3 != 0) {
        *y_in_tm = cuva_func_hl(&(pst_curve_param->base_param), ye_in, pst_curve_param->base_param.m_a,
            pst_curve_param->base_param.m_m);
    } else if (beyong_3spline_en_2_mode12 != 0) {
        l1 = pst_curve_param->th3[2] - pst_curve_param->th2[2];                                    /* 2,3:pa */

        k_line = ((3 * pst_curve_param->md[1][2] * l1 * l1 + 2 * pst_curve_param->mc[1][2] * l1 *  /* 2,3:pa */
            maxval_base_param_scale + pst_curve_param->mb[1][2] * maxval_base_param_scale *        /* 2,3:pa */
            maxval_base_param_scale + (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) /               /* 2,3:pa */
            (td_s64)(one << 2 * CUVA_PROCESS_BIT));

        b_line = (pst_curve_param->md[1][2] * ((l1 * l1 * l1 + maxval_base_param_scale_offset) /    /* 2:pa */
            (td_s64)(one << CUVA_PROCESS_BIT)) + pst_curve_param->mc[1][2] * l1 * l1 +              /* 2:pa */
            pst_curve_param->mb[1][2] * l1 * maxval_base_param_scale + pst_curve_param->ma[1][2] *  /* 2:pa */
            maxval_base_param_scale * maxval_base_param_scale +
            (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) / (td_s64)(one << 2 * CUVA_PROCESS_BIT);   /* 2:pa */

        *y_in_tm = ((k_line * ((td_s64)ye_in - pst_curve_param->th3[2]) +                           /* 2:pa */
            maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT)) + b_line;
    } else if ((p_3spline_num > 0) && (ye_in >= 0) && (ye_in <= pst_curve_param->th3[0])) {
        *y_in_tm = (((td_s64)(pst_curve_param->mb[0][0] * (td_s64)ye_in) + maxval_base_param_scale_offset) /
            (td_s64)(one << CUVA_PROCESS_BIT)) + pst_curve_param->base_offset;
    } else {
        res = cuva_calparam_range_conversion_tm_3spline(pst_curve_param, ye_in, y_in_tm);
        if (res != EXT_SUCCESS) {
            return res;
        }
    }

    *y_in_tm = pq_clip3(0, maxval_base_param_scale, *y_in_tm);

    return EXT_SUCCESS;
}

td_void cuva_calparam_3spline_curve_proc2_part4(cuva_curve_param *pst_curve_param)
{
    const td_s64 maxval_base_param_scale = (1 << CUVA_PROCESS_BIT);
    const td_u64 one = 1;
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 h1, h2, func_up, func_down;

    /* f.g.  */
    /* MB[1][1] */
    h1 = pst_curve_param->th2[2] - pst_curve_param->th1[2]; /* 2: parameters */
    h2 = pst_curve_param->th3[2] - pst_curve_param->th2[2]; /* 2: parameters */
    func_up = -(3 * pst_curve_param->va1 * h2 * h2 +        /* 3: parameters */
        3 * pst_curve_param->va2 * h1 * h1 -        /* 3: parameters */
        3 * pst_curve_param->va3 * h1 * h1 -        /* 3: parameters */
        3 * h2 * h2 * pst_curve_param->va2 +        /* 3: parameters */
        (h1 * h1 * h2 * pst_curve_param->gd3 + maxval_base_param_scale_offset) / maxval_base_param_scale +
        (pst_curve_param->gd1 * h1 * h2 * h2 + maxval_base_param_scale_offset) / maxval_base_param_scale);
    func_down = (2 * h2 * (h1 * h1 + h2 * h1) + maxval_base_param_scale_offset) / maxval_base_param_scale; /* 2:para */
    pst_curve_param->mb[1][2] = (func_down == 0) ? 0 : ((func_up + func_down / DIVISOR_2) / func_down);

    /* MC[0][1] */
    func_up = 3 * (td_s64)pst_curve_param->va2 * maxval_base_param_scale - /* 3: para */
        2 * pst_curve_param->gd1 * h1 -                              /* 2: para */
        3 * (td_s64)pst_curve_param->va1 * maxval_base_param_scale - /* 3: para */
        pst_curve_param->mb[1][2] * h1;                              /* 2: para */
    func_down = h1 * h1;
    pst_curve_param->mc[0][2] = (func_down == 0) ? 0 :               /* 2: para */
        (((func_up * (td_s64)(one << CUVA_PROCESS_BIT)) + func_down / DIVISOR_2) / func_down);

    /* MD[0][1] */
    func_up = h1 * pst_curve_param->gd1 + h1 * pst_curve_param->mb[1][2] +    /* 2: para */
        2 * (td_s64)pst_curve_param->va1 * maxval_base_param_scale -    /* 2: para */
        2 * (td_s64)pst_curve_param->va2 * maxval_base_param_scale;     /* 2: para */
    func_down = (h1 * h1 * h1 + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_curve_param->md[0][2] = (func_down == 0) ? 0 :                        /* 2: para */
        (((td_s64)(func_up * (td_s64)(one << CUVA_PROCESS_BIT)) + func_down / DIVISOR_2) / func_down);

    /* MC[1][1] */
    pst_curve_param->mc[1][2] = pst_curve_param->mc[0][2] +                   /* 2: para */
        ((3 * pst_curve_param->md[0][2] * h1 + maxval_base_param_scale_offset) / /* 2,3: para */
            (td_s64)(one << CUVA_PROCESS_BIT));

    /* MD[1][1] */
    func_up =
        ((td_s64)pst_curve_param->va3 * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) -            /* 2: para */
        ((td_s64)pst_curve_param->va2 * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) -            /* 2: para */
        ((h2 * pst_curve_param->gd3) * (td_s64)(one << CUVA_PROCESS_BIT)) + (pst_curve_param->mc[0][2] * h2 * h2) +
        3 * ((pst_curve_param->md[0][2] * h1 * h2 + maxval_base_param_scale_offset) /         /* 2,3: para */
            (td_s64)(one << CUVA_PROCESS_BIT) * h2);
    func_down = (td_s64)((2 * h2 * h2 * h2 + maxval_base_param_scale_offset) /   /* 2: para */
        (td_s64)(one << CUVA_PROCESS_BIT));
    pst_curve_param->md[1][2] = (func_down == 0) ? 0 : (-(func_up + func_down / DIVISOR_2) / func_down); /* 2: para */
}

td_void cuva_calparam_3spline_curve_proc2_part3(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    td_s64 up_t1, mid_t1, down_t1, down_t, up_t;
    const td_s64 maxval_scale = (1 << CUVA_PROCESS_BIT);
    td_s64 maxval_scale_offset = maxval_scale / DIVISOR_2;
    td_s64 p_3spline_strength =
        ((((td_s32)pst_cuva_in_param->metadata.p_3spline_enable_strength[idx_3spline][idx_tm] - 127) * /* 127: para */
        (td_s64)(one << CUVA_PROCESS_BIT)) + 127 / DIVISOR_2) / 127; /* 127: para */
    td_s64 p_3spline_th_mb = (((pst_cuva_in_param->metadata.p_3spline_th_enable_mb[idx_3spline][idx_tm]) <<
        CUVA_PROCESS_BIT) * 11 + 2550 / DIVISOR_2) / 2550; /* 11, 2550: parameters */

    td_u32 base_delta_mode = pst_curve_param->base_param_delta_mode;
    /* 2,3,6: mode */
    td_u32 delta_mode_not_326_en = ((base_delta_mode != 3) && ((base_delta_mode != 2) && (base_delta_mode != 6)));

    pst_curve_param->gd2 = cuva_calparam_3spline_curve_gd(pst_curve_param, pst_curve_param->th3[2]); /* 2,3:para */

    if (((pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 1) ||
        (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 2)) && /* 2,3: para */
        (pst_curve_param->va3 == pst_curve_param->th3[2]) && delta_mode_not_326_en != 0) { /* 2,3: para */
        pst_curve_param->gd3 = maxval_scale;
    } else if (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 1) {
        up_t1 = ((pst_curve_param->th3[2] - pst_curve_param->th2[2]) == 0) ? 0 : /* 2,3: para */
            ((((pst_curve_param->va3 - pst_curve_param->va1) * (td_s64)(one << CUVA_PROCESS_BIT)) +
            (pst_curve_param->th3[2] - pst_curve_param->th2[2]) / DIVISOR_2) / /* 2,3: para */
                (pst_curve_param->th3[2] - pst_curve_param->th2[2]));               /* 2,3: para */
        mid_t1 = ((pst_curve_param->th3[2] - pst_curve_param->th1[2]) == 0) ?
            0 : ((((pst_curve_param->va3 - pst_curve_param->va1) * (td_s64)(one << CUVA_PROCESS_BIT)) +
            (pst_curve_param->th3[2] - pst_curve_param->th1[2]) / DIVISOR_2) / /* 2,3: para */
                (pst_curve_param->th3[2] - pst_curve_param->th1[2]));               /* 2,3: para */
        down_t1 =
            ((pst_curve_param->th3[2] - pst_curve_param->th1[2]) == 0) ? 0 :              /* 2,3: para */
            ((((pst_curve_param->va3 - pst_curve_param->va1) * (td_s64)(one << CUVA_PROCESS_BIT)) +
            ((pst_curve_param->th3[2] - pst_curve_param->th1[2]) * 10) / DIVISOR_2) / /* 2,3,10: para */
                ((pst_curve_param->th3[2] - pst_curve_param->th1[2]) * 10));               /* 2,3,10: para */
        down_t = (down_t1 < pst_curve_param->gd1) ? pst_curve_param->gd1 : down_t1;
        up_t = (up_t1 < pst_curve_param->gd1) ? pst_curve_param->gd1 : up_t1;

        if (p_3spline_strength < 0) {
            pst_curve_param->gd3 = ((td_s64)(down_t * (-p_3spline_strength)) +
                mid_t1 * (maxval_scale + p_3spline_strength) + maxval_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
        } else {
            pst_curve_param->gd3 = ((td_s64)(up_t * p_3spline_strength) + mid_t1 *
                (maxval_scale - p_3spline_strength) + maxval_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
        }

        if (pst_curve_param->gd3 > maxval_scale) {
            pst_curve_param->gd3 = maxval_scale;
        }
    } else if (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 2) { /* 2: para */
        pst_curve_param->gd3 = pst_curve_param->gd2 - p_3spline_th_mb;
    } else if (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 3) { /* 3: para */
        pst_curve_param->gd3 = pst_curve_param->gd2;
    }
}

td_void cuva_calparam_3spline_curve_proc2_part2(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (1 << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 p_3spline_strength =
        ((((td_s32)pst_cuva_in_param->metadata.p_3spline_enable_strength[idx_3spline][idx_tm] - 127) * /* 127: para */
        (td_s64)(one << CUVA_PROCESS_BIT)) + 127 / DIVISOR_2) / 127; /* 127: parameters */
    td_u32 base_param_delta_mode = pst_curve_param->base_param_delta_mode;
    td_u32 targeted_system_display_maxlum;
    td_u32 delta_mode_not_326_en = ((base_param_delta_mode != 3) &&                                  /* 3: para */
        ((base_param_delta_mode != 2) && (base_param_delta_mode != 6))); /* 2, 6: para */

    targeted_system_display_maxlum = pst_cuva_in_param->metadata.targeted_system_display_maximum_luminance_pq[idx_tm];
    targeted_system_display_maxlum = (td_u32)((targeted_system_display_maxlum << CUVA_PROCESS_BIT) +
        (one << (CUVA_METADATA_TARGETED_DISP_LUM_BIT - 1))) >> CUVA_METADATA_TARGETED_DISP_LUM_BIT;

    pst_curve_param->va1 = cuva_func_hl(&(pst_curve_param->base_param), pst_curve_param->th1[2], /* 2: parameters */
        pst_curve_param->base_param.m_a, pst_curve_param->base_param.m_m);
    pst_curve_param->va3 = cuva_func_hl(&(pst_curve_param->base_param), pst_curve_param->th3[2], /* 2: parameters */
        pst_curve_param->base_param.m_a, pst_curve_param->base_param.m_m);
    if ((pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 1) ||
        (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 2)) { /* 1, 2: parameters */
        if (base_param_delta_mode != 3) {                                                   /* 3: para */
            pst_curve_param->va3 = pst_curve_param->maxlum_display_pq;
            if ((pst_curve_param->va3 > pst_curve_param->th3[2]) &&               /* 2: parameters */
                ((base_param_delta_mode != 2) && (base_param_delta_mode != 6))) { /* 2, 6: parameters */
                pst_curve_param->th3[2] = pst_curve_param->va3;                   /* 2: parameters */
                pst_curve_param->th2[2] = (pst_curve_param->th3[2] + pst_curve_param->th1[2]) / 2; /* 2: para */
            }
        }

        if (base_param_delta_mode == 3) { /* 3: para */
            pst_curve_param->va3 = targeted_system_display_maxlum;
        }
    }

    /* 2:para */
    pst_curve_param->va2 = pst_curve_param->va1 + (((pst_curve_param->th3[2] - /* 2: para */
        pst_curve_param->th1[2]) == 0) ? 0 : (((pst_curve_param->th2[2] - pst_curve_param->th1[2]) * /* 2: para */
        (pst_curve_param->va3 - pst_curve_param->va1) + (pst_curve_param->th3[2] - /* 2: para */
            pst_curve_param->th1[2]) / DIVISOR_2) / (pst_curve_param->th3[2] - /* 2: para */
                pst_curve_param->th1[2]))) + (((pst_curve_param->va3 - pst_curve_param->va1) * /* 2: para */
                p_3spline_strength / DIVISOR_2 + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT));
    if ((pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 1) ||
        (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 2)) { /* 2: parameters */
        if ((pst_curve_param->va2 > pst_curve_param->th2[2]) && delta_mode_not_326_en) {    /* 2: parameters */
            pst_curve_param->va2 = pst_curve_param->th2[2]; /* 2: parameters */
        }
    }

    pst_curve_param->ma[0][2] = pst_curve_param->va1; /* 0,1,2,3,10: para */
    pst_curve_param->ma[1][2] = pst_curve_param->va2; /* 0,1,2,3,10: para */
    pst_curve_param->gd1 = cuva_calparam_3spline_curve_gd(pst_curve_param, pst_curve_param->th1[2]); /* 2: para */
    pst_curve_param->mb[0][2] = pst_curve_param->gd1; /* 0,1,2,3,10: para */
}

td_void cuva_calparam_3spline_curve_proc2_part1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    td_s64 p_3spline_th = (((td_s64)pst_cuva_in_param->metadata.p_3spline_th_enable[idx_3spline][idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT)) + (4095 >> 1)) / 4095; /* 4095: parameters */
    td_s64 th_delta1 = (((td_s64)pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[idx_3spline][idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT)) * 25 + 1023 * 50) / (1023 * 100); /* 25,50,100,1023: parameters */
    td_s64 th_delta2 = (((td_s64)pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[idx_3spline][idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT)) * 25 + 1023 * 50) / (1023 * 100); /* 25,50,100,1023: parameters */

    pst_curve_param->th1[2] = p_3spline_th;                         /* 2: parameters */
    pst_curve_param->th2[2] = p_3spline_th + th_delta1;             /* 2: parameters */
    pst_curve_param->th3[2] = p_3spline_th + th_delta1 + th_delta2; /* 2: parameters */

    if (pst_curve_param->th1[2] < pst_curve_param->th3[1]) { /*  2: parameters */
        pst_curve_param->th1[2] = pst_curve_param->th3[1];
        pst_curve_param->th2[2] = (pst_curve_param->th1[2] + pst_curve_param->th3[2]) / 2; /*  2:para */
    }
}

/* 9-3-3-3:  3spline of high-lum area  */
td_void cuva_calparam_3spline_curve_proc2(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param)
{
    cuva_calparam_3spline_curve_proc2_part1(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
    if (pst_curve_param->th3[2] < pst_curve_param->th3[1]) { /* 2: parameters */
        pst_curve_param->th1[2] = 0;                         /* 2: parameters */
        pst_curve_param->th2[2] = 0;                         /* 2: parameters */
        pst_curve_param->th3[2] = 0;                         /* 2: parameters */
        pst_curve_param->ma[0][2] = 0;                       /* 2: parameters */
        pst_curve_param->ma[1][2] = 0;                       /* 2: parameters */
        pst_curve_param->mb[0][2] = 0;                       /* 2: parameters */
        pst_curve_param->mb[1][2] = 0;                       /* 2: parameters */
        pst_curve_param->mc[0][2] = 0;                       /* 2: parameters */
        pst_curve_param->mc[1][2] = 0;                       /* 2: parameters */
        pst_curve_param->md[0][2] = 0;                       /* 2: parameters */
        pst_curve_param->md[1][2] = 0;                       /* 2: parameters */
        pst_curve_param->p_3spline_num = 1;
        return;
    }

    cuva_calparam_3spline_curve_proc2_part2(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
    cuva_calparam_3spline_curve_proc2_part3(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
    cuva_calparam_3spline_curve_proc2_part4(pst_curve_param);
}

td_void cuva_calparam_3spline_curve_proc1_part1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    td_s64 th_delta1 = (((td_s64)pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[idx_3spline][idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT)) * 25 + 1023 * 50) / (1023 * 100); /* 25,50,100,1023: parameters */
    td_s64 th_delta2 = (((td_s64)pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[idx_3spline][idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT)) * 25 + 1023 * 50) / (1023 * 100); /* 25,50,100,1023: parameters */

    pst_curve_param->th1[1] = pst_curve_param->th3[0];
    pst_curve_param->th2[1] = pst_curve_param->th1[1] + th_delta1;
    pst_curve_param->th3[1] = pst_curve_param->th1[1] + th_delta1 + th_delta2;
}

td_void cuva_calparam_3spline_curve_proc1_part2(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    td_u32 idx_3spline, cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    td_s64 maxv_basepara = ((td_u64)1 << CUVA_PROCESS_BIT);
    td_s64 maxv_basepara_offset = maxv_basepara / DIVISOR_2;
    td_s64 p_3spline_strength =
        ((td_s64)(((td_s32)pst_cuva_in_param->metadata.p_3spline_enable_strength[idx_3spline][idx_tm] -
        127) * (td_s64)(one << CUVA_PROCESS_BIT)) + 127 / DIVISOR_2) / 127; /* 127: parameters */
    td_u32 base_param_delta_mode = pst_curve_param->base_param_delta_mode;
    td_u32 delta_mode_not_326_en = ((base_param_delta_mode != 3) &&      /* 3: para */
        ((base_param_delta_mode != 2) && (base_param_delta_mode != 6))); /* 2, 6: para */

/* b. 1) */
    pst_curve_param->va1 = ((td_s64)(pst_curve_param->mb[0][0] * pst_curve_param->th1[1]) +
        pst_curve_param->base_offset * maxv_basepara + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_curve_param->va3 = cuva_func_hl(&(pst_curve_param->base_param), pst_curve_param->th3[1],
        pst_curve_param->base_param.m_a, pst_curve_param->base_param.m_m);
    if ((pst_curve_param->va3 > pst_curve_param->th3[1]) && delta_mode_not_326_en != 0) {
        pst_curve_param->base_param.m_b =
            pst_curve_param->base_param.m_b - pst_curve_param->va3 + pst_curve_param->th3[1];
        pst_curve_param->va3 = pst_curve_param->th3[1];
    }

    if ((pst_curve_param->th3[1] - pst_curve_param->th1[1]) == 0) {
        pst_curve_param->va2 = pst_curve_param->va1 + (((pst_curve_param->va3 - pst_curve_param->va1) *
            p_3spline_strength / DIVISOR_2 + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT));
    } else {
        pst_curve_param->va2 = pst_curve_param->va1 +
            (((pst_curve_param->th3[1] - pst_curve_param->th1[1]) == 0) ?
                0 : (((pst_curve_param->th2[1] - pst_curve_param->th1[1]) *
                (pst_curve_param->va3 - pst_curve_param->va1) +
                    (pst_curve_param->th3[1] - pst_curve_param->th1[1]) / DIVISOR_2) /
                    (pst_curve_param->th3[1] - pst_curve_param->th1[1]))) +
                    (((pst_curve_param->va3 - pst_curve_param->va1) * p_3spline_strength / DIVISOR_2 +
                        maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT));
    }
    if ((pst_curve_param->va2 > pst_curve_param->th2[1]) && delta_mode_not_326_en != 0) {
        pst_curve_param->va2 = pst_curve_param->th2[1];
    }

    /* b. 2) */
    pst_curve_param->ma[0][1] = pst_curve_param->va1;
    pst_curve_param->ma[1][1] = pst_curve_param->va2;

    /* b. 3) */
    pst_curve_param->gd1 = pst_curve_param->mb[0][0];
    pst_curve_param->mb[0][1] = pst_curve_param->gd1;
    pst_curve_param->gd3 = cuva_calparam_3spline_curve_gd(pst_curve_param, pst_curve_param->th3[1]);
    /* b. 4) */
    cuva_calparam_3spline_curve_proc1_part2_p1(pst_curve_param);
}

td_void cuva_calparam_3spline_curve_proc1_part2_p1(cuva_curve_param *pst_curve_param)
{
    td_s64 maxv_basepara = ((td_u64)1 << CUVA_PROCESS_BIT);
    td_s64 maxv_basepara_offset = maxv_basepara / DIVISOR_2;
    td_s64 h1, h2, func_up, func_down;
    const td_u64 one = 1;
    /* b. 4) */
    /* MB[1][1] */
    h1 = pst_curve_param->th2[1] - pst_curve_param->th1[1];
    h2 = pst_curve_param->th3[1] - pst_curve_param->th2[1];
    func_up = -(3 * pst_curve_param->va1 * h2 * h2 + /* 3: parameters */
        3 * pst_curve_param->va2 * h1 * h1 - /* 3: parameters */
        3 * pst_curve_param->va3 * h1 * h1 - /* 3: parameters */
        3 * h2 * h2 * pst_curve_param->va2 + /* 3: parameters */
        (h1 * h1 * h2 * pst_curve_param->gd3 + maxv_basepara_offset) / maxv_basepara +
        (pst_curve_param->gd1 * h1 * h2 * h2 + maxv_basepara_offset) / maxv_basepara);
    func_down = (2 * h2 * (h1 * h1 + h2 * h1) + maxv_basepara_offset) / maxv_basepara; /* 2: para */
    pst_curve_param->mb[1][1] = (func_down == 0) ? 0 : ((func_up + func_down / DIVISOR_2) / func_down);
    /* MC[0][1] */
    func_up = 3 * pst_curve_param->va2 * maxv_basepara - 2 * pst_curve_param->gd1 * h1 - /* 0,1,2,3,10: para */
        3 * pst_curve_param->va1 * maxv_basepara - pst_curve_param->mb[1][1] * h1; /* 0,1,2,3,10: para */
    func_down = h1 * h1;
    pst_curve_param->mc[0][1] =
        (func_down == 0) ? 0 : (((func_up * (td_s64)(one << CUVA_PROCESS_BIT)) + func_down / DIVISOR_2) / func_down);

    /* MD[0][1] */
    func_up = h1 * pst_curve_param->gd1 + h1 * pst_curve_param->mb[1][1] +
        2 * pst_curve_param->va1 * maxv_basepara - /* 0,1,2,3,10: para */
        2 * pst_curve_param->va2 * maxv_basepara;  /* 0,1,2,3,10: para */
    func_down = (h1 * h1 * h1 + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_curve_param->md[0][1] =
        (func_down == 0) ? 0 : (((func_up * (td_s64)(one << (CUVA_PROCESS_BIT))) + func_down / DIVISOR_2) / func_down);

    /* MC[1][1] */
    pst_curve_param->mc[1][1] = pst_curve_param->mc[0][1] +
        ((3 * pst_curve_param->md[0][1] * h1 + maxv_basepara_offset) / /* 0,1,2,3: para */
        (td_s64)(one << CUVA_PROCESS_BIT));

    /* MD[1][1] */
    td_s64 temp = pst_curve_param->md[0][1] * h1 * h2 + maxv_basepara_offset;
    temp = (td_s64)(temp > 0 ? ((td_u64)temp >> CUVA_PROCESS_BIT) : ((td_u64)(temp * (-1)) >> CUVA_PROCESS_BIT) * (-1));
    func_up =
        (pst_curve_param->va3 * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) - /* 2: para */
        (pst_curve_param->va2 * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) - /* 2: para */
        ((h2 * pst_curve_param->gd3) * (td_s64)(one << CUVA_PROCESS_BIT)) +
        (pst_curve_param->mc[0][1] * h2 * h2) + 3 * temp * h2; /* 3:para */
    func_down = (((td_s64)(2 * h2 * h2 * h2)) + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT); /* 2: para */
    pst_curve_param->md[1][1] = (func_down == 0) ? 0 : (-(func_up + func_down / DIVISOR_2) / func_down);
}

/* 9-3-3-2:  3spline of low-lum area, using metadata parameters */
td_void cuva_calparam_3spline_curve_proc1(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param)
{
    /* a. */
    cuva_calparam_3spline_curve_proc1_part1(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
    /* b. */
    cuva_calparam_3spline_curve_proc1_part2(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
}

td_void cuva_calparam_3spline_curve_proc0_part1(cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    const td_s64 maxval_base_param_scale = (1 << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 b = ((15 << CUVA_PROCESS_BIT) + 50) / 100; /* 15, 50, 100: para */
    td_s64 c = ((5 << CUVA_PROCESS_BIT) + 5) / 10;    /* 5, 10: para */
    td_s64 d = ((5 << CUVA_PROCESS_BIT) + 5) / 10;    /* 5, 10: para */
    /* a. */
    pst_curve_param->th1[1] = pst_curve_param->th3[0];
    pst_curve_param->th2[1] = pst_curve_param->th1[1] + b;
    pst_curve_param->th3[1] = (maxval_base_param_scale * pst_curve_param->th2[1] + c * pst_curve_param->th2[1] -
        (td_s64)(d * pst_curve_param->th1[1]) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
}

td_void cuva_calparam_3spline_curve_proc0_part2(cuva_curve_param *pst_curve_param)
{
    const td_u64 one = 1;
    const td_s64 maxv_basepara = (1 << CUVA_PROCESS_BIT);
    const td_s64 maxv_basepara_offset = maxv_basepara / DIVISOR_2;
    td_s64 func_up, func_down;
    td_s64 h1, h2;

    /* b. 1) */
    pst_curve_param->va1 = (pst_curve_param->mb[0][0] * pst_curve_param->th1[1] + maxv_basepara_offset) /
        (td_s64)(one << CUVA_PROCESS_BIT);
    pst_curve_param->va3 = cuva_func_hl(&(pst_curve_param->base_param), pst_curve_param->th3[1],
        pst_curve_param->base_param.m_a, pst_curve_param->base_param.m_m);
    if ((pst_curve_param->th3[1] - pst_curve_param->th1[1]) == 0) {
        pst_curve_param->va2 = pst_curve_param->va1;
    } else {
        pst_curve_param->va2 = pst_curve_param->va1 + (pst_curve_param->th2[1] - pst_curve_param->th1[1]) *
            (pst_curve_param->va3 - pst_curve_param->va1) /
            (pst_curve_param->th3[1] - pst_curve_param->th1[1]);
    }

    /* b. 2) */
    pst_curve_param->ma[0][1] = pst_curve_param->va1;
    pst_curve_param->ma[1][1] = pst_curve_param->va2;

    /* b. 3) */
    pst_curve_param->gd1 = pst_curve_param->mb[0][0];
    pst_curve_param->mb[0][1] = pst_curve_param->gd1;
    pst_curve_param->gd3 = cuva_calparam_3spline_curve_gd(pst_curve_param, pst_curve_param->th3[1]);

    /* b. 4) */
    h1 = pst_curve_param->th2[1] - pst_curve_param->th1[1];
    h2 = pst_curve_param->th3[1] - pst_curve_param->th2[1];
    func_up = -(3 * pst_curve_param->va1 * h2 * h2 + /* 3: parameters */
        3 * pst_curve_param->va2 * h1 * h1 - /* 3: parameters */
        3 * pst_curve_param->va3 * h1 * h1 - /* 3: parameters */
        3 * h2 * h2 * pst_curve_param->va2 + /* 3: parameters */
        (h1 * h1 * h2 * pst_curve_param->gd3 + maxv_basepara_offset) / maxv_basepara +
        (pst_curve_param->gd1 * h1 * h2 * h2 + maxv_basepara_offset) / maxv_basepara);
    func_down = (2 * h2 * (h1 * h1 + h2 * h1) + maxv_basepara_offset) / maxv_basepara; /* 2: para */
    pst_curve_param->mb[1][1] = (func_down == 0) ? 0 : ((func_up + func_down / DIVISOR_2) / func_down);

    /* MC[0][1] */
    func_up = 3 * pst_curve_param->va2 * maxv_basepara - 2 * pst_curve_param->gd1 * h1 - /* 0,1,2,3,10: para */
        3 * pst_curve_param->va1 * maxv_basepara - pst_curve_param->mb[1][1] * h1; /* 0,1,2,3,10: para */
    func_down = h1 * h1;
    pst_curve_param->mc[0][1] =
		(func_down == 0) ? 0 : (((func_up * (td_s64)(one << CUVA_PROCESS_BIT)) + func_down / DIVISOR_2) / func_down);

    /* MD[0][1] */
    func_up = h1 * pst_curve_param->gd1 + h1 * pst_curve_param->mb[1][1] + 2 * /* 2 : para */
        pst_curve_param->va1 * maxv_basepara - 2 * pst_curve_param->va2 * maxv_basepara; /* 2 : para */
    func_down = (h1 * h1 * h1 + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_curve_param->md[0][1] =
		(func_down == 0) ? 0 : (((func_up * (td_s64)(one << (CUVA_PROCESS_BIT))) + func_down / DIVISOR_2) / func_down);

    /* MC[1][1] */
    pst_curve_param->mc[1][1] = pst_curve_param->mc[0][1] + ((3 * pst_curve_param->md[0][1] * h1 +  /* 3: para */
        maxv_basepara_offset) / (td_s64)(one << (CUVA_PROCESS_BIT)));

    /* MD[1][1], 2: para */
    func_up = (pst_curve_param->va3 * (td_s64)(one << (2 * CUVA_PROCESS_BIT))) - (pst_curve_param->va2 * (td_s64)(one <<
        (2 * CUVA_PROCESS_BIT))) - ((h2 * pst_curve_param->gd3) * (td_s64)(one << CUVA_PROCESS_BIT)) + /* 2: para */
        (pst_curve_param->mc[0][1] * h2 * h2) + 3 * /* 3: para */
        ((pst_curve_param->md[0][1] * h1 * h2 + maxv_basepara_offset) / (td_s64)(one << CUVA_PROCESS_BIT)) * h2;
    td_s64 temp = 2 * h2 * h2 * h2 + maxv_basepara_offset; /* 2: para */
    func_down = (temp > 0 ? (temp / (td_s64)(one << CUVA_PROCESS_BIT)) : /* 0: para */
        ((temp * (-1)) / (td_s64)(one << CUVA_PROCESS_BIT)) * (-1)); /* 1,3,10: para */
    pst_curve_param->md[1][1] = (func_down == 0) ? 0 : (-(func_up + func_down / DIVISOR_2) / func_down);
}

/* 9-3-3-1: 3spline of low-lum area, using default parameters */
td_void cuva_calparam_3spline_curve_proc0(cuva_curve_param *pst_curve_param)
{
    /* a */
    cuva_calparam_3spline_curve_proc0_part1(pst_curve_param);
    /* b */
    cuva_calparam_3spline_curve_proc0_part2(pst_curve_param);
}

/* input and output is 10bit */
td_void cuva_1spline_curve_linear_interp_tmap(td_s64 m_p, td_s64 *p_m_a_t)
{
    td_s32 idx;
    td_s32 idx_next = 0;
    td_s32 idx_pre = 0;
    td_u32 weight, weight_div;
    const td_u64 one = 1;
    const td_u32 maxval_param_scale = (1 << CUVA_PROCESS_BIT);

    /* Tm_ap */
    td_u32 cuva_1spline_tm_ap[CUVA_TM_AP_NUM][2] = {
        /* [m_p, m_a], 2:para */
        {(((25 << CUVA_PROCESS_BIT) + 5) / 10), (((99 << CUVA_PROCESS_BIT) + 50) / 100)}, /* 25,5,10,99,50,100:param */
        {(((35 << CUVA_PROCESS_BIT) + 5) / 10), (((879 << CUVA_PROCESS_BIT) + 500) / 1000)}, /* 35,5,10,879,500,1000 */
        {(((45 << CUVA_PROCESS_BIT) + 5) / 10), (((777 << CUVA_PROCESS_BIT) + 500) / 1000)}, /* 45,5,10,777,500,1000 */
        {(((75 << CUVA_PROCESS_BIT) + 5) / 10), (((540 << CUVA_PROCESS_BIT) + 500) / 1000)}  /* 75,5,10,540,500,1000 */
    };

    /* case1 */
    if (m_p <= cuva_1spline_tm_ap[0][0]) {
        *p_m_a_t = cuva_1spline_tm_ap[0][1];
        return;
    }

    /* case 2 */
    if (m_p >= cuva_1spline_tm_ap[CUVA_TM_AP_NUM - 1][0]) {
        *p_m_a_t = cuva_1spline_tm_ap[CUVA_TM_AP_NUM - 1][1];
        return;
    }

    /* case 3 */
    for (idx = 0; idx < (CUVA_TM_AP_NUM - 1); idx++) {
        if ((m_p >= cuva_1spline_tm_ap[idx][0]) && (m_p < cuva_1spline_tm_ap[idx + 1][0])) {
            idx_pre = idx;
            idx_next = idx + 1;
            break;
        }
    }

    weight_div = cuva_1spline_tm_ap[idx_next][0] - cuva_1spline_tm_ap[idx_pre][0];
    if (weight_div == 0) {
        weight = 0;
    } else {
        weight =
            (((((td_u32)m_p - cuva_1spline_tm_ap[idx_pre][0]) * (td_u32)(one << CUVA_PROCESS_BIT)) +
            weight_div / DIVISOR_2) / weight_div);
    }

    *p_m_a_t =
        cuva_1spline_tm_ap[idx_pre][1] * (maxval_param_scale - weight) + cuva_1spline_tm_ap[idx_next][1] * weight;
    *p_m_a_t = (*p_m_a_t + (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);
}

/* 9-3-2-3:  1spline */
td_void cuva_calparam_1spline_curve_adjust0(cuva_curve_param *pst_curve_param, td_u32 max_lum)
{
    const td_u64 one = 1;
    td_s64 max_display = pst_curve_param->maxlum_display_pq;
    td_s64 mb_mid_00, th3_mid_0, m_a_t, h_maxsource, h_maxsource_div_maxlum, wa, wa_div, mb_00, th3_0;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 base_param_delta_mode = pst_curve_param->base_param_delta_mode;

    if ((base_param_delta_mode >= 3) || (pst_curve_param->base_flag == 0)) { /* 0, 3: para */
        return;
    }

    /* a */
    mb_mid_00 = pst_curve_param->mb[0][0];
    th3_mid_0 = pst_curve_param->th3[0];

    /* b. */
    cuva_1spline_curve_linear_interp_tmap(pst_curve_param->base_param.m_p, &m_a_t);
    if (pst_curve_param->base_param.m_a <= m_a_t) {
        return;
    }

    /* H, [Max_Lum, m_a_T] */
    h_maxsource = cuva_func_hl(&(pst_curve_param->base_param), max_lum, m_a_t, pst_curve_param->base_param.m_m);

    /* WA */
    if (max_lum == 0) {
        wa = 0;
    } else {
        h_maxsource_div_maxlum = ((h_maxsource * (td_s64)(one << CUVA_PROCESS_BIT)) +
            (td_s64)(max_lum / DIVISOR_2)) / (td_s64)max_lum;
        wa_div = maxval_base_param_scale - h_maxsource_div_maxlum;
        wa = (((max_display * (td_s64)(one << CUVA_PROCESS_BIT)) + (td_s64)max_lum / DIVISOR_2) / (td_s64)max_lum) -
            h_maxsource_div_maxlum;
        wa = (wa_div == 0) ? 0 : (((wa * (td_s64)(one << CUVA_PROCESS_BIT)) + wa_div / DIVISOR_2) / wa_div);
    }

    /* MB[0][0] */
    mb_00 = (mb_mid_00 * maxval_base_param_scale + (maxval_base_param_scale - mb_mid_00) * wa +
        maxval_base_param_scale / DIVISOR_2) / (td_s64)(one << CUVA_PROCESS_BIT);
    mb_00 = uapi_max(mb_00, mb_mid_00);
    mb_00 = uapi_min(mb_00, maxval_base_param_scale);
    pst_curve_param->mb[0][0] = mb_00;

    /* TH3[0] */
    th3_0 = (th3_mid_0 * maxval_base_param_scale + ((td_s64)max_lum - th3_mid_0) * wa +
        maxval_base_param_scale / DIVISOR_2) / (td_s64)(one << CUVA_PROCESS_BIT);
    th3_0 = uapi_max(th3_0, th3_mid_0);
    th3_0 = uapi_min(th3_0, maxval_base_param_scale);
    pst_curve_param->th3[0] = th3_0;

    /* m_b */
    pst_curve_param->base_param.m_b = ((td_s64)((maxval_base_param_scale - wa) * pst_curve_param->base_param.m_b) +
        maxval_base_param_scale / DIVISOR_2) / (td_s64)(one << CUVA_PROCESS_BIT);
}

/* 9-3-2-2:  1spline */
td_void cuva_calparam_1spline_curve_proc1(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, td_u32 idx_3spline,
    cuva_curve_param *pst_curve_param)
{
    pst_curve_param->th3[0] =
        (((pst_cuva_in_param->metadata.p_3spline_th_enable[idx_3spline][idx_tm]) << CUVA_PROCESS_BIT) +
            4095 / DIVISOR_2) / 4095; /* 4095: para */

       /* 3spline_th_mb */
    pst_curve_param->mb[0][0] =
        ((((pst_cuva_in_param->metadata.p_3spline_th_enable_mb[idx_3spline][idx_tm] & 0xff) >> 2) << /* 2: para */
            CUVA_PROCESS_BIT) + 63 / DIVISOR_2) / 63; /* high 6 bit. 2, 63: parameters */
    pst_curve_param->base_offset =
        (((pst_cuva_in_param->metadata.p_3spline_th_enable_mb[idx_3spline][idx_tm] & 0x3) << CUVA_PROCESS_BIT) * 1 +
            30 / DIVISOR_2) / 30; /* low 2 bit. 1, 30: parameters */
}

/* 9-3-2-1:  1spline */
td_void cuva_calparam_1spline_curve_proc0(const cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_curve_param)
{
    td_s64 hlmaxh2 = ((6 << CUVA_PROCESS_BIT) + 5) / 10;     /* 5, 6, 10: parameters */
    td_s64 hlmaxl2 = ((3 << CUVA_PROCESS_BIT) + 5) / 10;     /* 3, 5, 10: parameters */
    td_s64 t_dmaxl2 = ((1 << CUVA_PROCESS_BIT) + 5) / 10;    /* 1, 5, 10: parameters */
    td_s64 t_dmaxh2 = ((25 << CUVA_PROCESS_BIT) + 50) / 100; /* 25, 50, 100: parameters */
    td_s64 w2, w2_div;
    td_s64 th3_0, mb_00, base_offset;
    const td_u64 one = 1;
    td_s64 rgb_pq_maxval = (td_s64)((one << CUVA_METADATA_RGB_PQ_BIT) - one);
    td_s64 average_l = ((td_s64)(pst_cuva_in_param->metadata.average_maxrgb_pq * (td_s64)(one << CUVA_PROCESS_BIT)) +
        (rgb_pq_maxval / DIVISOR_2)) / rgb_pq_maxval;
    td_s64 avmaxh3 = ((6 << CUVA_PROCESS_BIT) + 5) / 10;     /* 5, 6, 10: parameters */
    td_s64 avmaxl3 = ((3 << CUVA_PROCESS_BIT) + 5) / 10;     /* 3, 5, 10: parameters */
    td_s64 s_dmaxl3 = ((96 << CUVA_PROCESS_BIT) + 50) / 100; /* 50, 96, 100: parameters */
    td_s64 s_dmaxh3 = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 w3, w3_div;

    /* a. th3[0] */
    w2_div = hlmaxh2 - hlmaxl2;
    w2 = (w2_div == 0) ?
        0 : ((((average_l - hlmaxl2) * (td_s64)(one << CUVA_PROCESS_BIT)) + (w2_div / DIVISOR_2)) / w2_div);

    if (average_l > hlmaxh2) {
        th3_0 = t_dmaxl2;
    } else if (average_l < hlmaxl2) {
        th3_0 = t_dmaxh2;
    } else {
        th3_0 = (t_dmaxl2 * w2 + t_dmaxh2 * ((td_s64)(one << CUVA_PROCESS_BIT) - w2) +
            (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);
    }
    pst_curve_param->th3[0] = th3_0;

    /* b: MB[0][0], base_offset */
    w3_div = avmaxh3 - avmaxl3;
    w3 = (w3_div == 0) ?
        0 : ((((average_l - avmaxl3) * (td_s64)(one << CUVA_PROCESS_BIT)) + (w3_div / DIVISOR_2)) / w3_div);

    if (average_l > avmaxh3) {
        mb_00 = s_dmaxl3;
    } else if (average_l < avmaxl3) {
        mb_00 = s_dmaxh3;
    } else {
        mb_00 = ((td_s64)(s_dmaxl3 * w3) + s_dmaxh3 * ((td_s64)(one << CUVA_PROCESS_BIT) - w3) +
            (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);
    }
    pst_curve_param->mb[0][0] = mb_00;
    base_offset = 0;
    pst_curve_param->base_offset = base_offset;
}

td_u32 cuva_calparam_3spline_curve_part1(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
    cuva_curve_param *pst_curve_param)
{
    td_u32 idx_3spline;
    td_u32 flag_3spline_mode0 = 0;
    pst_curve_param->p_3spline_num = pst_cuva_in_param->metadata.p_3spline_enable_num[idx_tm] + 1;
    /*    a. */
    if ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 0) ||
        ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 1) && (pst_curve_param->p_3spline_flag == 0))) {
        pst_curve_param->p_3spline_num = 1;
        pst_cuva_in_param->metadata.p_3spline_th_enable_mode[0][idx_tm] = 0;
    }
    for (idx_3spline = 0; idx_3spline < pst_curve_param->p_3spline_num; idx_3spline++) {
        if (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 0) {
            flag_3spline_mode0 = 1;
        }
    }

    return flag_3spline_mode0;
}

/* 9.3 3spline */
td_void cuva_calparam_3spline_curve(cuva_api_param *pst_cuva_in_param, td_u32 idx, cuva_curve_param *pst_curve_param,
    td_u32 max_lum)
{
    td_u32 idx_3spline, static_mode_0, static_mode_1, static_mode_2;
    td_u32 flag_3spline_mode0;
    td_u32 idx_tm = idx;
    /* a. */
    idx_tm = pq_clip3(0, CUVA_MAX_PARANUM_TM - 1, idx_tm);
    flag_3spline_mode0 = cuva_calparam_3spline_curve_part1(pst_cuva_in_param, idx_tm, pst_curve_param);

    for (idx_3spline = 0; idx_3spline < pst_curve_param->p_3spline_num; idx_3spline++) {
        static_mode_0 = (pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 0);
        static_mode_1 = ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 1) &&
            (pst_curve_param->p_3spline_flag == 0) &&
            (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 0));
        static_mode_2 = ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 1) &&
            (pst_curve_param->p_3spline_flag == 1) && (flag_3spline_mode0 == 0));
        if (static_mode_0 != 0 || static_mode_1 != 0 || static_mode_2 != 0) {
            cuva_calparam_1spline_curve_proc0(pst_cuva_in_param, pst_curve_param);
            /* 9-3-2-3 */
            cuva_calparam_1spline_curve_adjust0(pst_curve_param, max_lum);
            /* 9-3-3-1 */
            cuva_calparam_3spline_curve_proc0(pst_curve_param);
        } else if ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 1) &&
            (pst_curve_param->p_3spline_flag == 1) &&
            (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 0)) {
            /* 9-3-2-2 */
            cuva_calparam_1spline_curve_proc1(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
            /* 9-3-2-3 */
            cuva_calparam_1spline_curve_adjust0(pst_curve_param, max_lum);
            /* 9-3-3-2 */
            cuva_calparam_3spline_curve_proc1(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
        }

        if ((pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag == 1) &&
            (pst_curve_param->p_3spline_flag == 1) &&
            ((pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 1) ||
            (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 2) ||  /* 2: parameters */
            (pst_cuva_in_param->metadata.p_3spline_th_enable_mode[idx_3spline][idx_tm] == 3))) { /* 3: parameters */
               /*  9-3-3-3 */
            if (pst_curve_param->p_3spline_num == 1) {
                pst_curve_param->p_3spline_num = 2; /* 2: parameters */
                pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][idx_tm] =
                    pst_cuva_in_param->metadata.p_3spline_th_enable_mode[0][idx_tm];
                cuva_calparam_3spline_curve_proc2(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
                break;
            } else {
                cuva_calparam_3spline_curve_proc2(pst_cuva_in_param, idx_tm, idx_3spline, pst_curve_param);
            }
        }
    }
}

/* 9.2.5 Base Curve: case2 */
td_void cuva_calparam_base_curve_proc2(cuva_api_param *pst_cuva_in_param, td_u32 targeted_system_display_maxlum,
    td_u32 max_lum, cuva_curve_param *pst_curve_param)
{
    cuva_base_curve_param *pst_base_curve_param = &(pst_curve_param->base_param);
    cuva_base_curve_param st_base_curve_param_1;
    const td_u64 one = 1;
    td_s64 min_disp = pst_curve_param->minlum_display_pq;
    td_s64 max_disp = pst_curve_param->maxlum_display_pq;
    td_s64 base_param_delta = pst_curve_param->base_param_delta;
    td_u64 max_disp_linear, targeted_system_display_maxlum_linear;
    td_s64 temp, w, h_maxsource, m_a;
    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;

    memset_s(&st_base_curve_param_1, sizeof(cuva_base_curve_param), 0, sizeof(cuva_base_curve_param));

    cuva_calparam_base_curve_proc0(pst_cuva_in_param, max_lum, pst_curve_param, &st_base_curve_param_1);
    /* b. */
    temp = ((max_disp * (td_s64)(one << PQ2L_INPUT_SCALE2P)) +
        (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);

    pq_2_linear((td_u32)temp, ((td_u64 *)&(max_disp_linear)));

    temp = (((td_s64)targeted_system_display_maxlum * (td_s64)(one << PQ2L_INPUT_SCALE2P)) +
        (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);

    pq_2_linear((td_u32)temp, ((td_u64 *)&(targeted_system_display_maxlum_linear)));

    w = (td_s64)(base_param_delta * uapi_abs(max_disp_linear - targeted_system_display_maxlum_linear)) /
        100; /* 100: para */
    w = ((w  * (td_s64)(one << CUVA_PROCESS_BIT)) +
        (td_s64)(one << (CUVA_METADATA_BASE_PARAM_DELTA_BIT + PQ2L_OUTPUT_SCALE2P - 1))) /
        (td_s64)(one << (CUVA_METADATA_BASE_PARAM_DELTA_BIT + PQ2L_OUTPUT_SCALE2P));

    w = pq_clip3((td_s64)0 - (td_s64)(one << CUVA_PROCESS_BIT), (td_s64)(one << CUVA_PROCESS_BIT), w);
    /* c. */
    pst_base_curve_param->m_p = ((maxval_base_param_scale - w) * pst_base_curve_param->m_p +
        (td_s64)(w * st_base_curve_param_1.m_p) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->m_m = ((maxval_base_param_scale - w) * pst_base_curve_param->m_m +
        (td_s64)(w * st_base_curve_param_1.m_m) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->m_n = ((maxval_base_param_scale - w) * pst_base_curve_param->m_n +
        (td_s64)(w * st_base_curve_param_1.m_n) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k1 = ((maxval_base_param_scale - w) * pst_base_curve_param->k1 +
        (td_s64)(w * st_base_curve_param_1.k1) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k2 = ((maxval_base_param_scale - w) * pst_base_curve_param->k2 +
        (td_s64)(w * st_base_curve_param_1.k2) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k3 = ((maxval_base_param_scale - w) * pst_base_curve_param->k3 +
        (td_s64)(w * st_base_curve_param_1.k3) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);

    pst_base_curve_param->m_b = min_disp;
    h_maxsource = cuva_func_hl_pow(pst_base_curve_param, max_lum, pst_base_curve_param->m_m);
    if (h_maxsource == 0) {
        m_a = 0;
    } else {
        m_a = ((max_disp - min_disp) * (td_s64)(one << CUVA_PROCESS_BIT) + h_maxsource / DIVISOR_2) / h_maxsource;
    }
    pst_base_curve_param->m_a = m_a;
}

/* 9.2.4 Base Curve: case1 */
td_void cuva_calparam_base_curve_proc1(td_u32 targeted_system_display_maxlum, cuva_curve_param *pst_curve_param)
{
    td_u64 one = 1;
    cuva_base_curve_param *pst_base_curve_param = &(pst_curve_param->base_param);
    td_s64 min_display = pst_curve_param->minlum_display_pq;
    td_s64 max_display = pst_curve_param->maxlum_display_pq;
    td_s64 base_param_delta = pst_curve_param->base_param_delta;
    td_u64 max_display_linear, targeted_system_display_maxlum_linear;
    td_s64 temp, w;

    const td_s64 maxval_base_param_scale = (td_s64)(one << CUVA_PROCESS_BIT);
    td_s64 maxval_base_param_scale_offset = maxval_base_param_scale / DIVISOR_2;
    td_s64 n = ((5 << CUVA_PROCESS_BIT) + 5) / 10;        /* 5, 10: parameters */
    td_s64 min_m_p = ((30 << CUVA_PROCESS_BIT) + 5) / 10; /* 30, 5, 10: parameters */
    td_s64 max_m_p = ((75 << CUVA_PROCESS_BIT) + 5) / 10; /* 75, 5, 10: parameters */

    if (targeted_system_display_maxlum == 0) {
        pst_base_curve_param->m_b = 0;
        pst_base_curve_param->m_a = 0;
    } else {
        pst_base_curve_param->m_b = (pst_base_curve_param->m_b * (max_display - min_display) +
            ((td_s64)targeted_system_display_maxlum / DIVISOR_2)) /
            (td_s64)targeted_system_display_maxlum;
        pst_base_curve_param->m_a = (pst_base_curve_param->m_a * (max_display - min_display) +
            ((td_s64)targeted_system_display_maxlum / DIVISOR_2)) /
            (td_s64)targeted_system_display_maxlum;
    }

    temp = ((max_display * (td_s64)(one << PQ2L_INPUT_SCALE2P)) + (td_s64)(one << (CUVA_PROCESS_BIT - 1))) /
        (td_s64)(one << CUVA_PROCESS_BIT);

    pq_2_linear((td_u32)temp, ((td_u64 *)&(max_display_linear)));

    temp = ((targeted_system_display_maxlum << PQ2L_INPUT_SCALE2P) +
        (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);

    pq_2_linear((td_u32)temp, ((td_u64 *)&(targeted_system_display_maxlum_linear)));

    w = uapi_abs(max_display_linear - targeted_system_display_maxlum_linear) / 100; /* 100: para */
    w = ((w * (td_s64)(one << CUVAHDR_BASE_S)) + (td_s64)(one << (PQ2L_OUTPUT_SCALE2P - 1))) /
        (td_s64)(one << PQ2L_OUTPUT_SCALE2P);
    n = ((n * (td_s64)(one << CUVAHDR_BASE_S)) + maxval_base_param_scale_offset) / (td_s64)(one << CUVA_PROCESS_BIT);
    w = metadata_pow_all(w, n);  /* bit of input and output is CUVAHDR_BASE_S */
    w = (td_s64)(base_param_delta * w);

    temp = w * (td_s64)(one << CUVA_PROCESS_BIT);
    w = temp / (td_s64)(one << (CUVA_METADATA_BASE_PARAM_DELTA_BIT + CUVAHDR_BASE_S));
    pst_base_curve_param->m_p = pst_base_curve_param->m_p + w;
    pst_base_curve_param->m_p = pq_clip3(min_m_p, max_m_p, pst_base_curve_param->m_p);
}

/* 9.2.3 */
/* max_display_mastering_lum: cd/m2 */
td_void cuva_calparam_base_curve_maxlum(const cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_curve_param,
    td_u32 max_display_mastering_lum, td_u32 *p_max_lum)
{
    const td_u64 one = 1;
    td_s64 max_ref_display;
    td_float max_ref_display_f;
    td_s64 max1;

    td_s64 rgb_pq_maxval = (td_s64)(one << CUVA_METADATA_RGB_PQ_BIT) - 1;
    td_s64 maximum_maxrgb = ((td_s64)((td_u64)pst_cuva_in_param->metadata.maximum_maxrgb_pq << CUVA_PROCESS_BIT) +
        rgb_pq_maxval / DIVISOR_2) / rgb_pq_maxval;
    td_s64 average_maxrgb = ((td_s64)((td_u64)pst_cuva_in_param->metadata.average_maxrgb_pq << CUVA_PROCESS_BIT) +
        rgb_pq_maxval / DIVISOR_2) / rgb_pq_maxval;
    td_s64 variance_maxrgb = ((td_s64)((td_u64)pst_cuva_in_param->metadata.variance_maxrgb_pq << CUVA_PROCESS_BIT) +
        rgb_pq_maxval / DIVISOR_2) / rgb_pq_maxval;

    td_s64 a = ((4 << CUVA_PROCESS_BIT) + 5) / 10;                /* 4, 5, 10: parameters */
    td_s64 b = ((2 << CUVA_PROCESS_BIT) + 5) / 10;                /* 2, 5, 10: parameters */
    td_s64 min_val = ((5081 << CUVA_PROCESS_BIT) + 5000) / 10000; /* 5081, 5000, 10000: parameters */

    /* a: lum-->pq */
    max_ref_display_f = (td_float)max_display_mastering_lum / 10000;  /* 10000: parameters */
    max_ref_display = (td_s64)(linear_2_pq(max_ref_display_f));

    /* b: MAX1 */
    max1 = ((td_s64)(b * maximum_maxrgb) + (a * 2 * average_maxrgb) + /* 2: parameters */
        ((td_s64)(one << CUVA_PROCESS_BIT) - a - b) * variance_maxrgb + (td_s64)(one << (CUVA_PROCESS_BIT - 1))) /
        (td_s64)(one << CUVA_PROCESS_BIT);

    /* c: max_lum */
    if (max1 > max_ref_display) {
        *p_max_lum = (td_u32)max_ref_display;
    } else if (max1 < min_val) {
        *p_max_lum = (td_u32)min_val;
    } else {
        *p_max_lum = (td_u32)max1;
    }

    if (*p_max_lum < (pst_curve_param->maxlum_display_pq)) {
        *p_max_lum = pst_curve_param->maxlum_display_pq;
    }
}

/* 9.2.2 Base Curve: case0 */
static td_void cuva_calparam_base_curve_proc0(const cuva_api_param *pst_cuva_in_param, td_u32 max_lum,
    const cuva_curve_param *pst_curve_param, cuva_base_curve_param *pst_base_curve_param)
{
    const td_u64 one = 1;
    td_s64 min_display = pst_curve_param->minlum_display_pq;
    td_s64 max_display = pst_curve_param->maxlum_display_pq;
    td_s64 p_value_h0 = ((35 << CUVA_PROCESS_BIT) + 5) / 10; /* 35, 5, 10: parameters */
    td_s64 p_value_l0 = ((40 << CUVA_PROCESS_BIT) + 5) / 10; /* 5, 10, 40: parameters */
    td_s64 tph0 = ((6 << CUVA_PROCESS_BIT) + 5) / 10;        /* 5, 6, 10: parameters */
    td_s64 tpl0 = ((3 << CUVA_PROCESS_BIT) + 5) / 10;        /* 3, 5, 10: parameters */
    td_s64 p_value_h1 = ((6 << CUVA_PROCESS_BIT) + 5) / 10;  /* 5, 6, 10: parameters */
    td_s64 tph1 = ((9 << CUVA_PROCESS_BIT) + 5) / 10;    /* 5, 9, 10: parameters */
    td_s64 tpl1 = ((75 << CUVA_PROCESS_BIT) + 50) / 100; /* 75, 50, 100: parameters */
    td_s64 rgb_pq_maxval = (td_s64)(one << CUVA_METADATA_RGB_PQ_BIT) - 1;
    td_s64 average_l = ((td_s64)((td_u64)pst_cuva_in_param->metadata.average_maxrgb_pq << CUVA_PROCESS_BIT) +
        rgb_pq_maxval / DIVISOR_2) / rgb_pq_maxval;
    td_s64 m_p, w0, w1, h_maxsource;

    /* a, b: preset params */
    pst_base_curve_param->m_m = ((24 << CUVA_PROCESS_BIT) + 5) / 10; /*  5, 10, 24: parameters */
    pst_base_curve_param->m_n = (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k1 = (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k2 = (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->k3 = (td_s64)(one << CUVA_PROCESS_BIT);
    pst_base_curve_param->m_b = min_display;

    /* c: m_p from average_maxrgb */
    if (average_l > tph0) {
        m_p = p_value_h0;
    } else if (average_l < tpl0) {
        m_p = p_value_l0;
    } else {
        w0 = (((average_l - tpl0) * (td_s64)(one << CUVA_PROCESS_BIT)) + ((tph0 - tpl0) / DIVISOR_2)) / (tph0 - tpl0);
        m_p = ((p_value_h0 * w0) + (p_value_l0 * (td_s64)(one << CUVA_PROCESS_BIT)) - (p_value_l0 * w0) +
            (td_s64)(one << (CUVA_PROCESS_BIT - 1))) / (td_s64)(one << CUVA_PROCESS_BIT);
    }

    /* d: update m_p according to max_lum */
    if (max_lum > tph1) {
        m_p = m_p + p_value_h1;
    } else if (max_lum < tpl1) {
        m_p = m_p + 0;
    } else {
        w1 = ((((td_s64)max_lum - tpl1) * (td_s64)(one << CUVA_PROCESS_BIT)) + ((tph1 - tpl1) / DIVISOR_2)) /
            (tph1 - tpl1);
        m_p = m_p + (((td_s64)(p_value_h1 * w1) + (td_s64)(one << (CUVA_PROCESS_BIT - 1))) /
            (td_s64)(one << CUVA_PROCESS_BIT));
    }
    pst_base_curve_param->m_p = m_p;

    /* e: m_a */
    h_maxsource = cuva_func_hl_pow(pst_base_curve_param, max_lum, pst_base_curve_param->m_m);
    if (h_maxsource == 0) {
        pst_base_curve_param->m_a = 0;
    } else {
        pst_base_curve_param->m_a =
            ((td_s64)((td_u64)(max_display - min_display) << CUVA_PROCESS_BIT) + (h_maxsource / DIVISOR_2)) /
                h_maxsource;
    }
}

td_s32 cuva_calparam_base_curve_usemetadata(const cuva_api_param *pst_cuva_in_param, td_u32 idx,
    cuva_base_curve_param *pst_base_curve_param)
{
    td_u32 idx_tm = idx;
    const td_u64 one = 1;
    td_u32 rgb_pq_maxval = (td_u32)((td_s32)(1 << CUVA_METADATA_RGB_PQ_BIT) - 1);
    td_u32 maximum_maxrgb = (((pst_cuva_in_param->metadata.maximum_maxrgb_pq) << CUVA_PROCESS_BIT) +
        (rgb_pq_maxval >> 1)) / rgb_pq_maxval;

    idx_tm = pq_clip3(0, CUVA_MAX_PARANUM_TM - 1, idx_tm);

    pst_base_curve_param->m_p = (td_s64)(pst_cuva_in_param->metadata.base_param_m_p[idx_tm] *
        10 * (td_s64)(one << CUVA_PROCESS_BIT) + (16383 >> 1)) / 16383;  /* 10, 16383: parameters */
    pst_base_curve_param->m_m = (td_s64)(pst_cuva_in_param->metadata.base_param_m_m[idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT) + (10 >> 1)) / 10;  /* 10: parameters */
    pst_base_curve_param->m_a = (td_s64)(pst_cuva_in_param->metadata.base_param_m_a[idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT) + (1023 >> 1)) / 1023;  /* 1023: parameters */
    pst_base_curve_param->m_b = (td_s64)(pst_cuva_in_param->metadata.base_param_m_b[idx_tm] *
        25 * (td_s64)(one << CUVA_PROCESS_BIT) + (1023 * 50)) / (1023 * 100);  /* 25,50,100,1023: parameters */
    pst_base_curve_param->m_n = (td_s64)(pst_cuva_in_param->metadata.base_param_m_n[idx_tm] *
        (td_s64)(one << CUVA_PROCESS_BIT) + 5) / 10;  /* 5,10: parameters */

    if ((pst_cuva_in_param->metadata.base_param_k1[idx_tm]) <= 1) {
        pst_base_curve_param->k1 = (pst_cuva_in_param->metadata.base_param_k1[idx_tm]) << CUVA_PROCESS_BIT;
    } else {
        return EXT_FAILURE;
    }

    if ((pst_cuva_in_param->metadata.base_param_k2[idx_tm]) <= 1) {
        pst_base_curve_param->k2 = (pst_cuva_in_param->metadata.base_param_k2[idx_tm]) << CUVA_PROCESS_BIT;
    } else {
        return EXT_FAILURE;
    }

    if ((pst_cuva_in_param->metadata.base_param_k3[idx_tm]) <= 1) {
        pst_base_curve_param->k3 = (pst_cuva_in_param->metadata.base_param_k3[idx_tm]) << CUVA_PROCESS_BIT;
    } else if ((pst_cuva_in_param->metadata.base_param_k3[idx_tm]) == 2) { /* 2: parameters */
        pst_base_curve_param->k3 = maximum_maxrgb;
    } else {
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

/* 9.2 Base Curve */
td_s32 cuva_calparam_base_curve_hdr2hdr(cuva_api_param *pst_cuva_in_param, td_u32 idx,
    cuva_curve_param *pst_curve_param, td_u32 *p_max_lum)
{
    td_u32 tone_mapping_mode_flag = pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag;
    td_u32 targeted_system_display_maxlum;
    td_u32 max_display = pst_curve_param->maxlum_display_pq;
    td_u32 base_param_delta_mode;
    const td_u64 u64one = 1;
    td_s32 res;
    td_u32 idx_tm = idx;
    idx_tm = pq_clip3(0, CUVA_MAX_PARANUM_TM - 1, idx_tm);
    /* revise metadata */
    pst_curve_param->base_param_delta_mode = pst_cuva_in_param->metadata.base_param_delta_enable_mode[idx_tm];
    pst_curve_param->base_param_delta = pst_cuva_in_param->metadata.base_param_enable_delta[idx_tm];
    if ((pst_curve_param->base_param_delta_mode == 2) || (pst_curve_param->base_param_delta_mode == 6)) { /* 2,6:para */
        pst_curve_param->base_param_delta = (td_s64)0 -
            (td_s64)pst_cuva_in_param->metadata.base_param_enable_delta[idx_tm];
    }
    base_param_delta_mode = pst_curve_param->base_param_delta_mode;

    /* max_lum */
    cuva_calparam_base_curve_maxlum(pst_cuva_in_param, pst_curve_param, pst_cuva_in_param->maxlum_mastering, p_max_lum);

    res = cuva_calparam_base_curve_usemetadata(pst_cuva_in_param, idx_tm, &(pst_curve_param->base_param));
    if (res != EXT_SUCCESS) {
        return res;
    }

    targeted_system_display_maxlum = pst_cuva_in_param->metadata.targeted_system_display_maximum_luminance_pq[idx_tm];
    targeted_system_display_maxlum = (td_u32)((((td_u64)targeted_system_display_maxlum << CUVA_PROCESS_BIT) +
        (u64one << (CUVA_METADATA_TARGETED_DISP_LUM_BIT - 1))) >> CUVA_METADATA_TARGETED_DISP_LUM_BIT);

    if ((tone_mapping_mode_flag == 0) || ((tone_mapping_mode_flag == 1) && (pst_curve_param->base_flag == 0))) {
        /* proc 0 */
        cuva_calparam_base_curve_proc0(pst_cuva_in_param, *p_max_lum, pst_curve_param, &(pst_curve_param->base_param));
    } else if ((tone_mapping_mode_flag == 1) && (pst_curve_param->base_flag == 1)) {
        if ((targeted_system_display_maxlum == max_display) || (base_param_delta_mode == 3)) { /* 3: param */
          /* use metadata param */
        } else if ((base_param_delta_mode == 0) || (base_param_delta_mode == 2) || /* 0,2: parameters */
            (base_param_delta_mode == 4) || (base_param_delta_mode == 6)) { /* 4,6: parameters */
            /* proc 1 */
            cuva_calparam_base_curve_proc1(targeted_system_display_maxlum, pst_curve_param);
        } else if ((base_param_delta_mode == 1) || (base_param_delta_mode == 5)) { /* 1,5: parameters */
            /* proc 2 */
            cuva_calparam_base_curve_proc2(pst_cuva_in_param, targeted_system_display_maxlum, *p_max_lum,
                pst_curve_param);
        } else {
            return EXT_FAILURE;
        }
    }

    return EXT_SUCCESS;
}

td_u32 cuva_3spline_estimate_monotony(cuva_curve_param *pst_curve_param, td_u32 idx_3spline)
{
    td_u32 monotony_en_1, monotony_en_2, monotony_en;

    monotony_en_1 = cuva_3spline_estimate_monotony_curve1(pst_curve_param, idx_3spline);
    monotony_en_2 = cuva_3spline_estimate_monotony_curve2(pst_curve_param, idx_3spline);
    monotony_en = (monotony_en_1 != 0) && (monotony_en_2 != 0);

    return monotony_en;
}

td_u32 cuva_3spline_estimate_monotony_curve1(const cuva_curve_param *pst_curve_param, td_u32 idx_3spline)
{
    const td_u64 one = 1;
    const td_s64 maxv_basepara = (td_s64)(one << CUVA_PROCESS_BIT);
    td_u32 monotony_en_1;
    td_s64 th1 = pst_curve_param->th1[idx_3spline];
    td_s64 th2 = pst_curve_param->th2[idx_3spline];
    td_s64 mb_0 = pst_curve_param->mb[0][idx_3spline];
    td_s64 mc_0 = pst_curve_param->mc[0][idx_3spline];
    td_s64 md_0 = pst_curve_param->md[0][idx_3spline];
    td_s64 gd_0_th1, gd_0_th2, minx_0, gd_0_minx;

    gd_0_th1 = mb_0;
    gd_0_th2 =
        (3 * md_0 * (th2 - th1) * (th2 - th1) + 2 * mc_0 * (th2 - th1) * maxv_basepara +            /* 2,3:para */
        mb_0 * maxv_basepara * maxv_basepara + (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) /       /* 2,3:para */
        (td_s64)(one << (2 * CUVA_PROCESS_BIT));                                                    /* 2,3:para */

    /* minx_0: when f1'' is 0, the value of x */
    if (md_0 == 0) {
        minx_0 = th1;
    } else {
        minx_0 = th1 - ((td_s64)(mc_0 * maxv_basepara) + 3 * md_0 / DIVISOR_2) / (3 * md_0);        /* 2,3:para */
    }

    gd_0_minx =
        (3 * md_0 * (minx_0 - th1) * (minx_0 - th1) + 2 * mc_0 * (minx_0 - th1) * maxv_basepara +   /* 2,3:para */
        mb_0 * maxv_basepara * maxv_basepara + (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) /      /* 2,3:para */
        (td_s64)(one << (2 * CUVA_PROCESS_BIT));                                                   /* 2,3:para */

    monotony_en_1 = 0;
    if ((minx_0 >= th1) && (minx_0 <= th2)) {
        if ((gd_0_th1 >= 0) && (gd_0_minx >= 0) && (gd_0_th2 >= 0)) {
            monotony_en_1 = 1;
        }
    } else {
        if ((gd_0_th1 >= 0) && (gd_0_th2 >= 0)) {
            monotony_en_1 = 1;
        }
    }

    return monotony_en_1;
}

td_u32 cuva_3spline_estimate_monotony_curve2(const cuva_curve_param *pst_curve_param, td_u32 idx_3spline)
{
    const td_u64 one = 1;
    const td_s64 maxv_basepara = (td_s64)(one << CUVA_PROCESS_BIT);
    td_u32 monotony_en_2;
    td_s64 th2 = pst_curve_param->th2[idx_3spline];
    td_s64 th3 = pst_curve_param->th3[idx_3spline];
    td_s64 mb_1 = pst_curve_param->mb[1][idx_3spline];
    td_s64 mc_1 = pst_curve_param->mc[1][idx_3spline];
    td_s64 md_1 = pst_curve_param->md[1][idx_3spline];
    td_s64 gd_1_th2, gd_1_th3, minx_1, gd_1_minx;

    gd_1_th2 = mb_1;

    gd_1_th3 =
        (3 * md_1 * (th3 - th2) * (th3 - th2) + 2 * mc_1 * (th3 - th2) * maxv_basepara +    /* 2,3:para */
        mb_1 * maxv_basepara * maxv_basepara + (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) /   /* 2,3:para */
        (td_s64)(one << (2 * CUVA_PROCESS_BIT));                                                    /* 2,3:para */

    if (md_1 == 0) {
        minx_1 = th2;
    } else {
        minx_1 = th2 - (mc_1 * maxv_basepara + 3 * md_1 / DIVISOR_2) / (3 * md_1); /* 2,3:para */
    }
    gd_1_minx =
        (3 * md_1 * (minx_1 - th2) * (minx_1 - th2) + 2 * mc_1 * (minx_1 - th2) * maxv_basepara + /* 2,3:para */
        (td_s64)(mb_1 * maxv_basepara * maxv_basepara) + (td_s64)(one << (2 * CUVA_PROCESS_BIT - 1))) /  /* 2:para */
        (td_s64)(one << (2 * CUVA_PROCESS_BIT)); /* 2:para */

    monotony_en_2 = 0;
    if ((minx_1 >= th2) && (minx_1 <= th3)) {
        if ((gd_1_th2 >= 0) && (gd_1_minx >= 0) && (gd_1_th3 >= 0)) {
            monotony_en_2 = 1;
        }
    } else {
        if ((gd_1_th2 >= 0) && (gd_1_th3 >= 0)) {
            monotony_en_2 = 1;
        }
    }

    return monotony_en_2;
}

td_void cuva_repair0_1dlut(td_u32 *p_1dlut, td_u32 lut_size)
{
    td_s64 idx;
    td_s64 effective_start_idx = 0;
    td_s64 effective_end_idx;
    if (lut_size == 0) {
        return;
    }
    effective_end_idx = lut_size - 1;

    for (idx = 0; idx < lut_size; idx++) {
        if (p_1dlut[idx] != 0) {
            effective_start_idx = pq_clip3(0, lut_size - 1, idx);
            break;
        }
    }
    for (idx = (effective_start_idx - 1); idx >= 0; idx--) {
        p_1dlut[idx] = p_1dlut[effective_start_idx];
    }

    for (idx = lut_size - 1; idx >= 0; idx--) {
        if (p_1dlut[idx] != 0) {
            effective_end_idx = pq_clip3(0, lut_size - 1, idx);
            break;
        }
    }

    for (idx = (effective_end_idx + 1); idx <= lut_size - 1; idx++) {
        p_1dlut[idx] = p_1dlut[effective_end_idx];
    }
}
