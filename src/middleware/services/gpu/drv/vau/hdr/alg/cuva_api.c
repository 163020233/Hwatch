/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description : CUVA
 */

#include "cuva_coef_common.h"
#include "securec.h"
#include "drv_vau_type.h"
#include "cuva_api.h"

#define PP_HDR_LINEAR_BIT 21
#define HIPP_TM_LUT_BIT 14
#define HIPP_HDR_TM_LUT_SEG 8

typedef struct {
    td_u32 x_step[HIPP_HDR_TM_LUT_SEG];
    td_u32 x_pos[HIPP_HDR_TM_LUT_SEG];
    td_u32 x_num[HIPP_HDR_TM_LUT_SEG];
} hipp_hdr_tm_para;

typedef struct {
    td_u16 tm_scale_coef;
    hipp_hdr_tm_para st_tm;
} cuva_static_reg;

/* Param: */
/* TM */
static const td_u16 g_tm_scale_coef_pq = 7;
static const td_u16 g_tm_scale_coef_hlg = 12;

static const td_u32 g_tm_x_step[HIPP_HDR_TM_LUT_SEG] = { 5, 7, 9, 10, 11, 13, 15, 17 };
static const td_u32 g_tm_x_pos[HIPP_HDR_TM_LUT_SEG] = { 128, 1024, 2048, 6144, 16384, 131072, 655360, 2097151 };
static const td_u32 g_tm_x_num[HIPP_HDR_TM_LUT_SEG] = { 4, 11, 13, 17, 22, 36, 52, 63 };

static const td_u32 g_tmlut_hlg2sdr[HIPP_HDR_TM_LUT_SIZE] = {
    1982, 1982, 2276, 2467, 2613, 2995, 3242, 3427, 3575, 3700, 3807, 3901, 4191, 4398, 4684, 4879,
    5023, 5135, 5304, 5429, 5528, 5611, 5683, 5907, 6078, 6220, 6341, 6449, 6544, 6630, 6708, 6778,
    6843, 6902, 6956, 7006, 7052, 7202, 7310, 7385, 7435, 7464, 7476, 7474, 7460, 7437, 7404, 7364,
    7318, 7266, 7210, 7150, 7086, 6807, 6505, 6195, 5888, 5589, 5302, 5029, 4772, 4531, 4306, 4096,
};
static const td_u32 g_tmlut_pq2sdr[HIPP_HDR_TM_LUT_SIZE] = {
    2815, 2815, 2815, 2815, 2814, 2812, 2811, 2809, 2807, 2806, 2804, 2802, 2795, 2789, 2775, 2762,
    2748, 2735, 2709, 2683, 2657, 2632, 2607, 2510, 2418, 2331, 2248, 2169, 2094, 2022, 1954, 1889,
    1827, 1768, 1712, 1658, 1607, 1424, 1271, 1143, 1024, 910,  819,  745,  683,  630,  585,  546,
    512,  482,  455,  431,  410,  341,  293,  256,  228,  205,  186,  171,  158,  146,  137,  128,
};

static td_s32 cuva_callut_tm(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, cuva_curve_param *pst_curve_param,
    cuva_static_reg *pst_cuva_static_reg, td_u32 *p_u32_tm_coef_lut);

static td_void cuva_callut_tm_in(hipp_hdr_tm_para *pst_tm_param, td_u64 *tm_lut_in, td_u32 len_tm_lut_in);

static td_void cuva_init_reg(const cuva_api_param *pst_cuva_in_param, cuva_static_reg *pst_cuva_static_reg)
{
    memcpy_s(pst_cuva_static_reg->st_tm.x_step, sizeof(pst_cuva_static_reg->st_tm.x_step), g_tm_x_step,
        sizeof(g_tm_x_step));
    memcpy_s(pst_cuva_static_reg->st_tm.x_pos, sizeof(pst_cuva_static_reg->st_tm.x_pos), g_tm_x_pos,
        sizeof(g_tm_x_pos));
    memcpy_s(pst_cuva_static_reg->st_tm.x_num, sizeof(pst_cuva_static_reg->st_tm.x_num), g_tm_x_num,
        sizeof(g_tm_x_num));

    if (pst_cuva_in_param->metadata.transfer_character == 0) {
        pst_cuva_static_reg->tm_scale_coef = g_tm_scale_coef_pq;
    } else {
        pst_cuva_static_reg->tm_scale_coef = g_tm_scale_coef_hlg;
    }
}

static td_void cuva_use_hdr10_para(const cuva_api_param *pst_cuva_in_param, td_u32 *p_tm_coef_lut)
{
    if (pst_cuva_in_param->metadata.transfer_character == 0) { /* dynamic HDR10 --> HDR10 */
        memcpy_s(p_tm_coef_lut, sizeof(td_u32) * HIPP_HDR_TM_LUT_SIZE, g_tmlut_pq2sdr, sizeof(g_tmlut_pq2sdr));
    } else { /* dynamic HLG --> HDR10 */
        memcpy_s(p_tm_coef_lut, sizeof(td_u32) * HIPP_HDR_TM_LUT_SIZE, g_tmlut_hlg2sdr, sizeof(g_tmlut_hlg2sdr));
    }
}

static td_void cuva_exception_processing(cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_curve_param,
    td_u32 *p_tm_coef_lut, td_u32 tm_lut_size, td_s32 *res)
{
    cuva_static_reg st_cuva_static_reg;
    td_u64 tm_lut_in[HIPP_HDR_TM_LUT_SIZE] = { 0 }; /*  linear,  HIPP_HDR_LINEAR_BIT, 0: parameters */
    td_u64 tm_lut_out[HIPP_HDR_TM_LUT_SIZE] = { 0 };
    td_s64 thr_min = 0 - ((td_s64)5 << PP_HDR_LINEAR_BIT) / 1000; /*  5, 1000: ratio */
    td_s64 idx, delta;
    td_u32 reset_lut_en = 0;
    td_u32 monotony_en;

    cuva_init_reg(pst_cuva_in_param, &st_cuva_static_reg);
    cuva_callut_tm_in(&(st_cuva_static_reg.st_tm), tm_lut_in, HIPP_HDR_TM_LUT_SIZE);

    if (*res != EXT_SUCCESS) {
        cuva_use_hdr10_para(pst_cuva_in_param, p_tm_coef_lut);
        return;
    }

    for (idx = 1; idx < tm_lut_size; idx++) {
        tm_lut_out[idx] = (tm_lut_in[idx] * p_tm_coef_lut[idx]) >> (st_cuva_static_reg.tm_scale_coef);
        tm_lut_out[idx] = pq_clip3(0, ((1 << PP_HDR_LINEAR_BIT) - 1), tm_lut_out[idx]);

        if (idx > 1) {
            delta = (td_s64)tm_lut_out[idx] - (td_s64)tm_lut_out[idx - 1];
            if ((delta <= thr_min) || (p_tm_coef_lut[idx] == 0)) { /* (rate2 >= ratio2_max) */
                reset_lut_en = 1;
                *res = -2; /*  -2: Abnormal TMLUT, use default hdr10 lut; */
                break;
            }
        }
    }

    /* monotonicity check */
    for (idx = 1; idx <= pst_curve_param->p_3spline_num; idx++) {
        monotony_en = cuva_3spline_estimate_monotony(pst_curve_param, (td_u32)idx);
        if (monotony_en != 1) {
            reset_lut_en = 1;
            *res = -3; /*  -3: Abnormal 3spline, use default hdr10 lut; */
            break;
        }
    }

    if (reset_lut_en == 1) {
        cuva_use_hdr10_para(pst_cuva_in_param, p_tm_coef_lut);
    }
}

static td_s32 cuva_callut_tm(cuva_api_param *pst_cuva_in_param, td_u32 idx_tm, cuva_curve_param *pst_curve_param,
    cuva_static_reg *pst_cuva_static_reg, td_u32 *p_u32_tm_coef_lut)
{
    hipp_hdr_tm_para *pst_tm_param = &(pst_cuva_static_reg->st_tm);
    const td_u64 u64one = 1;
    td_u64 tm_lut_in[HIPP_HDR_TM_LUT_SIZE] = { 0 }; /*  linear,  HIPP_HDR_LINEAR_BIT */
    td_u64 idx;
    td_u64 y_nl_in, y_nl_tm, y_l_tm, temp;
    td_s32 res;
    td_float temp_f = 0, y_l_d_f = 0;
    td_float sys_gamma_f = 1.2f; /* 1.2f: system gamma */

    cuva_callut_tm_in(pst_tm_param, tm_lut_in, HIPP_HDR_TM_LUT_SIZE);  // 2^21-1

    for (idx = u64one; idx < HIPP_HDR_TM_LUT_SIZE; idx++) {
        if (pst_cuva_in_param->metadata.transfer_character == 0) { /* dynamic HDR10 --> HDR10 */
            temp_f = (td_float)tm_lut_in[idx] / 2097151.0f;  // 2097151.0f: 2^21-1
        } else {  /* dynamic HLG --> HDR10 */
            temp_f = (td_float)tm_lut_in[idx] / 2097151.0f;  // 2097151.0f: 2^21-1
        /* input : Linear Ys --> Linear Yd */
            y_l_d_f = powf(temp_f, sys_gamma_f);
            temp_f = 0.1f * y_l_d_f;  // 0.1f: 1000 / 10000, 1000 max luminance of HLG
        }
        y_nl_in = linear_2_pq(temp_f);

        /* TM */
        y_nl_in = ((y_nl_in << CUVA_PROCESS_BIT) + (u64one << (L2PQ_LUT_B_SCALE2P - 1))) >>
            L2PQ_LUT_B_SCALE2P;
        res = cuva_calparam_range_conversion_tm(pst_cuva_in_param, idx_tm, pst_curve_param, (td_u32)y_nl_in,
                                                (td_s64 *)&y_nl_tm);
        if (res != EXT_SUCCESS) {
            return res;
        }

        /* output: non-linear --> Linear */
        temp = ((y_nl_tm << PQ2L_INPUT_SCALE2P) + (u64one << (CUVA_PROCESS_BIT - 1))) >>
            CUVA_PROCESS_BIT;
        pq_2_linear((td_u32)temp, &y_l_tm);

        /* output: lum scale */
        y_l_tm = (pst_cuva_in_param->maxlum_display == 0) ? 0 : (((y_l_tm << PP_HDR_LINEAR_BIT) +
            ((u64one << PQ2L_OUTPUT_SCALE2P) * pst_cuva_in_param->maxlum_display) / DIVISOR_2) /
            ((u64one << PQ2L_OUTPUT_SCALE2P) * pst_cuva_in_param->maxlum_display));

        /* K, tm-Gain */
        if (tm_lut_in[idx] == 0) {
            p_u32_tm_coef_lut[idx] = 0;
        } else {
            p_u32_tm_coef_lut[idx] = (td_u32)(((y_l_tm << pst_cuva_static_reg->tm_scale_coef) +
                tm_lut_in[idx] / DIVISOR_2) / tm_lut_in[idx]);
        }
        p_u32_tm_coef_lut[idx] = (td_u32)(pq_clip3(0, (u64one << HIPP_TM_LUT_BIT) - 1, p_u32_tm_coef_lut[idx]));
    }

    cuva_repair0_1dlut(p_u32_tm_coef_lut, HIPP_HDR_TM_LUT_SIZE);

    return EXT_SUCCESS;
}

static td_void cuva_callut_tm_in(hipp_hdr_tm_para *pst_tm_param, td_u64 *tm_lut_in, td_u32 len_tm_lut_in)
{
    const td_u64 one = 1;
    td_u64 idx;
    td_u32 seg_idx = 0; /*  idx of big segment */
    td_u32 max_idx = uapi_min(len_tm_lut_in - 1, pst_tm_param->x_num[HIPP_HDR_TM_LUT_SEG - 1]);
    td_u32 num_seg_idx = pst_tm_param->x_num[seg_idx];
    td_u32 pre_seg_idx;
    td_u32 num_pre_seg_idx = 0;
    td_u32 pre_pos = 0;
    td_u32 maxval_in = (1 << PP_HDR_LINEAR_BIT) - 1;  // 2^21-1

    for (idx = 1; idx <= max_idx; idx++) {
        if (idx > num_seg_idx) {
            seg_idx++;
            num_seg_idx = pst_tm_param->x_num[seg_idx];
            pre_seg_idx = seg_idx - 1;
            num_pre_seg_idx = pst_tm_param->x_num[pre_seg_idx];
            pre_pos = pst_tm_param->x_pos[pre_seg_idx];
        }
        tm_lut_in[idx] = pre_pos + (one << pst_tm_param->x_step[seg_idx]) * (idx - num_pre_seg_idx);
        tm_lut_in[idx] = uapi_min(tm_lut_in[idx], maxval_in);
    }

    if (pst_tm_param->x_num[HIPP_HDR_TM_LUT_SEG - 1] < (len_tm_lut_in - 1)) {
        for (idx = (pst_tm_param->x_num[HIPP_HDR_TM_LUT_SEG - 1] + 1); idx < len_tm_lut_in; idx++) {
            tm_lut_in[idx] = tm_lut_in[pst_tm_param->x_num[HIPP_HDR_TM_LUT_SEG - 1]];
        }
    }
}

static td_s32 cuva_update_lut(cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_cuva_curve_param,
    cuva_static_reg *pst_cuva_static_reg, td_u32 *p_tm_coef_lut, td_u32 tm_lut_size)
{
    td_u32 idx_tm, targeted_system_disp_maxlum, max_lum;
    td_s32 res;

    memset_s(p_tm_coef_lut, sizeof(td_u32) * tm_lut_size, 0, sizeof(td_u32) * tm_lut_size);

    for (idx_tm = 0; idx_tm < CUVA_MAX_PARANUM_TM; idx_tm++) {
        targeted_system_disp_maxlum = pst_cuva_in_param->metadata.targeted_system_display_maximum_luminance_pq[idx_tm];

        if (targeted_system_disp_maxlum != 2080) { /*  2080: parameters */
            pst_cuva_curve_param->find_curve_en = 1;
            break;
        }
    }

    idx_tm = pq_clip3(0, CUVA_MAX_PARANUM_TM - 1, idx_tm);
    pst_cuva_curve_param->base_flag = pst_cuva_in_param->metadata.base_enable_flag[idx_tm];
    pst_cuva_curve_param->p_3spline_flag = pst_cuva_in_param->metadata.p_3spline_enable_flag[idx_tm];
    if (pst_cuva_curve_param->find_curve_en == 0) {
        pst_cuva_curve_param->base_flag = 0;
        pst_cuva_curve_param->p_3spline_flag = 0;
    }

    /* Base Curve */
    res = cuva_calparam_base_curve_hdr2hdr(pst_cuva_in_param, idx_tm, pst_cuva_curve_param, &max_lum);
    if (res != EXT_SUCCESS) {
        return res;
    }

    /* 3-Spline Curve */
    cuva_calparam_3spline_curve(pst_cuva_in_param, idx_tm, pst_cuva_curve_param, max_lum);

    /* Set TM */
    res = cuva_callut_tm(pst_cuva_in_param, idx_tm, pst_cuva_curve_param, pst_cuva_static_reg, p_tm_coef_lut);
    if (res != EXT_SUCCESS) {
        return res;
    }

    return EXT_SUCCESS;
}

#ifdef HDR_ALG_DFX_SUPPORT
static td_void cuva_hdr_dfx_metadata_1st(const cuva_api_param *pst_cuva_in_param)
{
    printf("metadata:  \n");
    printf("minlum_display = %f nit. \n", (double)pst_cuva_in_param->minlum_display / 10000);  /* 10000: gain */
    printf("maxlum_display = %7d nit. \n", pst_cuva_in_param->maxlum_display);
    printf("maxlum_mastering = %7d nit. \n", pst_cuva_in_param->maxlum_mastering);
    printf("system_start_code = %7d. \n", pst_cuva_in_param->metadata.system_start_code);
    printf("minimum_maxrgb_pq = %7d. \n", pst_cuva_in_param->metadata.minimum_maxrgb_pq);
    printf("average_maxrgb_pq = %7d. \n", pst_cuva_in_param->metadata.average_maxrgb_pq);
    printf("variance_maxrgb_pq = %7d. \n", pst_cuva_in_param->metadata.variance_maxrgb_pq);
    printf("maximum_maxrgb_pq = %7d. \n", pst_cuva_in_param->metadata.maximum_maxrgb_pq);
    printf("tone_mapping_enable_mode_flag = %7d. \n", pst_cuva_in_param->metadata.tone_mapping_enable_mode_flag);
    printf("tone_mapping_param_enable_num = %7d. \n", pst_cuva_in_param->metadata.tone_mapping_param_enable_num);
    printf("targeted_system_display_maximum_luminance_pq = [%7d, %7d]. \n",
        pst_cuva_in_param->metadata.targeted_system_display_maximum_luminance_pq[0],
        pst_cuva_in_param->metadata.targeted_system_display_maximum_luminance_pq[1]);

    printf("base_enable_flag = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_enable_flag[0],
        pst_cuva_in_param->metadata.base_enable_flag[1]);
    printf("base_param_m_p = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_m_p[0],
        pst_cuva_in_param->metadata.base_param_m_p[1]);
    printf("base_param_m_m = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_m_m[0],
        pst_cuva_in_param->metadata.base_param_m_m[1]);
    printf("base_param_m_a = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_m_a[0],
        pst_cuva_in_param->metadata.base_param_m_a[1]);
    printf("base_param_m_b = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_m_b[0],
        pst_cuva_in_param->metadata.base_param_m_b[1]);
    printf("base_param_m_n = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_m_n[0],
        pst_cuva_in_param->metadata.base_param_m_n[1]);
    printf("base_param_k1 = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_k1[0],
        pst_cuva_in_param->metadata.base_param_k1[1]);
    printf("base_param_k2 = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_k2[0],
        pst_cuva_in_param->metadata.base_param_k2[1]);
    printf("base_param_k3 = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_k3[0],
        pst_cuva_in_param->metadata.base_param_k3[1]);

    printf("base_param_delta_enable_mode = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_delta_enable_mode[0],
        pst_cuva_in_param->metadata.base_param_delta_enable_mode[1]);
    printf("base_param_enable_delta = [%7d, %7d]. \n", pst_cuva_in_param->metadata.base_param_enable_delta[0],
        pst_cuva_in_param->metadata.base_param_enable_delta[1]);
    printf("p_3spline_enable_flag = [%7d, %7d]. \n", pst_cuva_in_param->metadata.p_3spline_enable_flag[0],
        pst_cuva_in_param->metadata.p_3spline_enable_flag[1]);
    printf("p_3spline_enable_num = [%7d, %7d]. \n", pst_cuva_in_param->metadata.p_3spline_enable_num[0],
        pst_cuva_in_param->metadata.p_3spline_enable_num[1]);
}

static td_void cuva_hdr_dfx_metadata_2nd(const cuva_api_param *pst_cuva_in_param)
{
    printf("p_3spline_th_enable_mode = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.p_3spline_th_enable_mode[0][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mode[0][1],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mode[1][1]);
    printf("p_3spline_th_enable_mb = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.p_3spline_th_enable_mb[0][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mb[0][1],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mb[1][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_mb[1][1]);
    printf("p_3spline_th_enable = [%7d, %7d, %7d, %7d]. \n", pst_cuva_in_param->metadata.p_3spline_th_enable[0][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable[0][1], pst_cuva_in_param->metadata.p_3spline_th_enable[1][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable[1][1]);
    printf("p_3spline_th_enable_delta1 = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[0][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[0][1],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[1][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta1[1][1]);
    printf("p_3spline_th_enable_delta2 = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[0][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[0][1],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[1][0],
        pst_cuva_in_param->metadata.p_3spline_th_enable_delta2[1][1]);
    printf("p_3spline_enable_strength = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.p_3spline_enable_strength[0][0],
        pst_cuva_in_param->metadata.p_3spline_enable_strength[0][1],
        pst_cuva_in_param->metadata.p_3spline_enable_strength[1][0],
        pst_cuva_in_param->metadata.p_3spline_enable_strength[1][1]);
    printf("color_saturation_mapping_enable_flag = %7d. \n",
        pst_cuva_in_param->metadata.color_saturation_mapping_enable_flag);
    printf("color_saturation_enable_num = %7d. \n", pst_cuva_in_param->metadata.color_saturation_enable_num);
    printf("color_saturation_enable_gain = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.color_saturation_enable_gain[0],
        pst_cuva_in_param->metadata.color_saturation_enable_gain[1],
        pst_cuva_in_param->metadata.color_saturation_enable_gain[2],  /* 2: param */
        pst_cuva_in_param->metadata.color_saturation_enable_gain[3]); /* 3: param */
    printf("color_saturation_enable_gain = [%7d, %7d, %7d, %7d]. \n",
        pst_cuva_in_param->metadata.color_saturation_enable_gain[4],  /* 4: param */
        pst_cuva_in_param->metadata.color_saturation_enable_gain[5],  /* 5: param */
        pst_cuva_in_param->metadata.color_saturation_enable_gain[6],  /* 6: param */
        pst_cuva_in_param->metadata.color_saturation_enable_gain[7]); /* 7: param */
    printf("transfer_character = %4d. \n", pst_cuva_in_param->metadata.transfer_character);
    printf("\n");
}

static td_void cuva_hdr_dfx_curve_1st(const cuva_curve_param *pst_curve_param)
{
    printf("curve:  \n");
    printf("p_3spline_num = %2d. \n", pst_curve_param->p_3spline_num);
    printf("base_param_delta_mode = %2d. \n", pst_curve_param->base_param_delta_mode);
    printf("base_param_delta = %2d. \n", pst_curve_param->base_param_delta);

    printf("TH1 = [%7.6f, %7.6f, %7.6f]. \n", (double)pst_curve_param->th1[0] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th1[1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th1[2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("TH2 = [%7.6f, %7.6f, %7.6f]. \n", (double)pst_curve_param->th2[0] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th2[1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th2[2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("TH3 = [%7.6f, %7.6f, %7.6f]. \n", (double)pst_curve_param->th3[0] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th3[1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->th3[2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("\n");

    printf("base_param.m_a = %7.6f. \n", (double)pst_curve_param->base_param.m_a / (1 << CUVA_PROCESS_BIT));
    printf("base_param.m_b = %7.6f. \n", (double)pst_curve_param->base_param.m_b / (1 << CUVA_PROCESS_BIT));
    printf("base_param.m_p = %7.6f. \n", (double)pst_curve_param->base_param.m_p / (1 << CUVA_PROCESS_BIT));
    printf("base_param.m_m = %7.6f. \n", (double)pst_curve_param->base_param.m_m / (1 << CUVA_PROCESS_BIT));
    printf("base_param.m_n = %7.6f. \n", (double)pst_curve_param->base_param.m_n / (1 << CUVA_PROCESS_BIT));
    printf("base_param.k1 = %7.6f. \n", (double)pst_curve_param->base_param.k1 / (1 << CUVA_PROCESS_BIT));
    printf("base_param.k2 = %7.6f. \n", (double)pst_curve_param->base_param.k2 / (1 << CUVA_PROCESS_BIT));
    printf("base_param.k3 = %7.6f. \n", (double)pst_curve_param->base_param.k3 / (1 << CUVA_PROCESS_BIT));
    printf("\n");
}

static td_void cuva_hdr_dfx_curve_2nd(const cuva_curve_param *pst_curve_param)
{
    printf("1spline.base_offset = %7.6f. \n", (double)pst_curve_param->base_offset / (1 << CUVA_PROCESS_BIT));
    printf("1spline.mb00 = %7.6f. \n", (double)pst_curve_param->mb[0][0] / (1 << CUVA_PROCESS_BIT));
    printf("\n");

    printf("3spline.1.ma = [%7.6f, %7.6f]. \n", (double)pst_curve_param->ma[0][1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->ma[1][1] / (1 << CUVA_PROCESS_BIT));
    printf("3spline.1.mb = [%7.6f, %7.6f]. \n", (double)pst_curve_param->mb[0][1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->mb[1][1] / (1 << CUVA_PROCESS_BIT));
    printf("3spline.1.mc = [%7.6f, %7.6f]. \n", (double)pst_curve_param->mc[0][1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->mc[1][1] / (1 << CUVA_PROCESS_BIT));
    printf("3spline.1.md = [%7.6f, %7.6f]. \n", (double)pst_curve_param->md[0][1] / (1 << CUVA_PROCESS_BIT),
        (double)pst_curve_param->md[1][1] / (1 << CUVA_PROCESS_BIT));
    printf("\n");

    printf("3spline.2.ma = [%7.6f, %7.6f]. \n",
        (double)pst_curve_param->ma[0][2] / (1 << CUVA_PROCESS_BIT),   /* 2: param */
        (double)pst_curve_param->ma[1][2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("3spline.2.mb = [%7.6f, %7.6f]. \n",
        (double)pst_curve_param->mb[0][2] / (1 << CUVA_PROCESS_BIT),   /* 2: param */
        (double)pst_curve_param->mb[1][2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("3spline.2.mc = [%7.6f, %7.6f]. \n",
        (double)pst_curve_param->mc[0][2] / (1 << CUVA_PROCESS_BIT),   /* 2: param */
        (double)pst_curve_param->mc[1][2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("3spline.2.md = [%7.6f, %7.6f]. \n",
        (double)pst_curve_param->md[0][2] / (1 << CUVA_PROCESS_BIT),   /* 2: param */
        (double)pst_curve_param->md[1][2] / (1 << CUVA_PROCESS_BIT));  /* 2: param */
    printf("\n");
}

static td_void cuva_hdr_dfx(cuva_api_param *pst_cuva_in_param,
    cuva_curve_param *pst_curve_param, const td_u32 *p_u32_tm_coef_lut, td_u32 tm_lut_size, td_s32 res)
{
    td_u32 idx;

    if (res == 0) {
        printf("Res = 0, HI_SUCCESS. \n");
    } else if (res == -1) {  /* -1: param */
        printf("Res = -1, HI_FAILURE, illegal metadata. \n");
    } else if (res == -2) {  /* -2: param */
        printf("Res = -2, Abnormal TMLut, use default hdr10 lut. \n");
    } else if (res == -3) {  /* -3: param */
        printf("Res = -3, Abnormal 3spline, use default hdr10 lut. \n");
    } else if (res == 404) {  /* 404: param */
        printf("Res = 404, Without CUVA processing. \n");
    }

    /* TM-Gain */
    printf("TM_Gain_LUT ={  \n");
    for (idx = 0; idx < tm_lut_size; idx++) {
        printf("%7d, ", p_u32_tm_coef_lut[idx]);

        if ((idx + 1) % 8 == 0) {  /* 8: param */
            printf(" \n");
        }
    }
    printf(" }; \n\n");

    cuva_hdr_dfx_metadata_1st(pst_cuva_in_param);
    cuva_hdr_dfx_metadata_2nd(pst_cuva_in_param);
    cuva_hdr_dfx_curve_1st(pst_curve_param);
    cuva_hdr_dfx_curve_2nd(pst_curve_param);
}
#endif

td_s32 cuva_update_tmlut_core(cuva_api_param *pst_cuva_in_param, td_u32 *p_tm_coef_lut, td_u32 tm_lut_size)
{
    cuva_curve_param st_cuva_curve_param;
    cuva_static_reg st_cuva_static_reg;
    td_s32 res = 404; /* 404: init value, without CUVA processing. */

    memset_s(&st_cuva_curve_param, sizeof(cuva_curve_param), 0, sizeof(cuva_curve_param));

    cuva_init_display(pst_cuva_in_param, &st_cuva_curve_param);

    /* init reg of TM */
    cuva_init_reg(pst_cuva_in_param, &st_cuva_static_reg);

    res = cuva_update_lut(pst_cuva_in_param, &st_cuva_curve_param, &st_cuva_static_reg, p_tm_coef_lut, tm_lut_size);

    cuva_exception_processing(pst_cuva_in_param, &st_cuva_curve_param, p_tm_coef_lut, tm_lut_size, &res);

#ifdef HDR_ALG_DFX_SUPPORT
    cuva_hdr_dfx(pst_cuva_in_param, &st_cuva_curve_param, p_tm_coef_lut, HIPP_HDR_TM_LUT_SIZE, res);
#endif

    return res;
}
