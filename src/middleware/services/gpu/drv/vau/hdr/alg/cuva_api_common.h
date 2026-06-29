/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: api for cuva hdr
 */

#ifndef __CUVA_API_COMMON_H__
#define __CUVA_API_COMMON_H__

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define pq_clip3(low, high, x) (uapi_max(uapi_min((x), high), low))

/* dynamic metadata */
#define CUVA_MAX_PARANUM_TM 2
#define CUVA_MAX_PARANUM_3SPLINE 2
#define CUVA_MAX_PARANUM_SM 8
#define CUVA_METADATA_RGB_PQ_BIT 12
#define CUVA_METADATA_TARGETED_DISP_LUM_BIT 12
#define CUVA_METADATA_BASE_PARAM_DELTA_BIT 7
#define CUVA_MAXLUM_DISPLAY_SDR 1000000 /* 100: SDR 100nit; 10000: gain of luminance. */
#define MAX_LUM_PQ 10000
#define CUVA_PROCESS_BIT 16     /* scale for base curve parameters */
#define DIVISOR_2 2             /* x / 2 */
#define CUVAHDR_BASE_S 18
#define CUVA_TM_AP_NUM 4

typedef struct {
    td_u16 system_start_code;             /* u8                                                   */
    td_u16 minimum_maxrgb_pq;             /* u12, 4095 * maxRGB_pq                                */
    td_u16 average_maxrgb_pq;             /* u12                                                  */
    td_u16 variance_maxrgb_pq;            /* u12                                                  */
    td_u16 maximum_maxrgb_pq;             /* u12                                                  */
    td_u16 tone_mapping_enable_mode_flag; /* u2, 0: don't transfer parameters of TM;  1: transfer */
    td_u16 tone_mapping_param_enable_num; /* u2, num-1                                            */
    td_u16 targeted_system_display_maximum_luminance_pq[CUVA_MAX_PARANUM_TM]; /* u12, 2080: SDR */
    td_u16 base_enable_flag[CUVA_MAX_PARANUM_TM];             /* u1,  0: don't transfer parameters of base-TM */
    td_u16 base_param_m_p[CUVA_MAX_PARANUM_TM];               /* u14,  base_param_m_p * 10 / 16383            */
    td_u16 base_param_m_m[CUVA_MAX_PARANUM_TM];               /* u6,   base_param_m_m / 10                    */
    td_u16 base_param_m_a[CUVA_MAX_PARANUM_TM];               /* u10,  base_param_m_a / 1023                  */
    td_u16 base_param_m_b[CUVA_MAX_PARANUM_TM];               /* u10,  base_param_m_b * 0.25 / 1023           */
    td_u16 base_param_m_n[CUVA_MAX_PARANUM_TM];               /* u6,   base_param_m_n / 10                    */
    td_u16 base_param_k1[CUVA_MAX_PARANUM_TM];                /* u2                                           */
    td_u16 base_param_k2[CUVA_MAX_PARANUM_TM];                /* u2                                           */
    td_u16 base_param_k3[CUVA_MAX_PARANUM_TM];                /* u4                                           */
    td_u16 base_param_delta_enable_mode[CUVA_MAX_PARANUM_TM]; /* u3       */
    td_u16 base_param_enable_delta[CUVA_MAX_PARANUM_TM];      /* u7       */
    td_u16 p_3spline_enable_flag[CUVA_MAX_PARANUM_TM];        /* u2       */
    td_u16 p_3spline_enable_num[CUVA_MAX_PARANUM_TM];         /* u3,   +1 */
    td_u16 p_3spline_th_enable_mode[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];   /* u4 */
    td_u16 p_3spline_th_enable_mb[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];     /* u8 */
    td_u16 p_3spline_th_enable[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];        /* u12 */
    td_s16 p_3spline_th_enable_delta1[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM]; /* s10 */
    td_s16 p_3spline_th_enable_delta2[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM]; /* s10 */
    td_s16 p_3spline_enable_strength[CUVA_MAX_PARANUM_3SPLINE][CUVA_MAX_PARANUM_TM];  /* s8 */
    td_u16 color_saturation_mapping_enable_flag;                                      /* u2 */
    td_u16 color_saturation_enable_num;                                               /* u3 */
    td_u16 color_saturation_enable_gain[CUVA_MAX_PARANUM_SM];                         /* u8 */

    td_u16 monitor_mode_enable; /* u1, 1: monitor mode, media path set to 0 */
    td_u16 transfer_character;  /* u1, 0:ST2084, 1:HLG                      */
    td_u16 graphic_source_display_value; /* u8, 0: no graphic, (STB: always set 0 to CUVA API) */
    /* (TV: HDMI-RX mode get value from VS-EMDS, media and HDMI-monitor mode set to 0) */
} cuva_metadata;

typedef struct {
    /* TV: read from PQ bin;  STB: read from HDMI VS-VDB. */
    td_u32 minlum_display;
    td_u32 maxlum_display;

    /* static metadata */
    td_u32 maxlum_mastering; /* default: 4000nit */

    /* for STB: read from input video. */
    td_u32 dynamic_metadata_avaliable;

    /* for STB: output type. V900 TV must set hdr_output_enable = 0. */
    td_u32 hdr_output_enable; /* 0: SDR, 1: HDR */

    cuva_metadata metadata;
} cuva_api_param;

typedef struct {
    td_s64 m_p;
    td_s64 m_m;
    td_s64 m_a;
    td_s64 m_b;
    td_s64 m_n;
    td_s64 k1;
    td_s64 k2;
    td_s64 k3;
} cuva_base_curve_param;

typedef struct {
    td_u32 minlum_display_pq;
    td_u32 maxlum_display_pq;

    /* curve param */
    td_s16 find_curve_en;
    td_u16 base_flag;
    td_u32 base_param_delta_mode;
    td_s64 base_param_delta;
    td_u16 p_3spline_flag;
    td_u16 p_3spline_num;

    /* base curve */
    cuva_base_curve_param base_param;

    /* 1spline */
    td_s64 base_offset;

    /* 3spline */
    td_s64 th1[3];   /* 3: param */
    td_s64 th2[3];   /* 3: param */
    td_s64 th3[3];   /* 3: param */
    td_s64 ma[2][3]; /* 2,3: param */
    td_s64 mb[2][3]; /* 2,3: param */
    td_s64 mc[2][3]; /* 2,3: param */
    td_s64 md[2][3]; /* 2,3: param */

    /* 3spline, proc */
    td_s64 va1;
    td_s64 va2;
    td_s64 va3;

    td_s64 gd1;
    td_s64 gd2;
    td_s64 gd3;
} cuva_curve_param;

td_u64 linear_2_pq(td_float input);

td_void pq_2_linear(td_u32 input, td_u64 *output);

td_void cuva_init_display(cuva_api_param *pst_cuva_in_param, cuva_curve_param *pst_curve_param);

td_s32 cuva_calparam_base_curve_hdr2hdr(cuva_api_param *pst_cuva_in_param, td_u32 idx,
                                        cuva_curve_param *pst_curve_param, td_u32 *p_max_lum);

td_void cuva_calparam_3spline_curve(cuva_api_param *pst_cuva_in_param, td_u32 idx, cuva_curve_param *pst_curve_param,
                                    td_u32 max_lum);

td_s32 cuva_calparam_range_conversion_tm(const cuva_api_param *pst_cuva_in_param, td_u32 idx_tm,
                                         cuva_curve_param *pst_curve_param, td_u32 ye_in, td_s64 *y_in_tm);

td_u32 cuva_3spline_estimate_monotony(cuva_curve_param *pst_curve_param, td_u32 idx_3spline);

td_s64 metadata_pow_all(td_s64 a, td_s64 b);

td_void cuva_repair0_1dlut(td_u32 *p_1dlut, td_u32 lut_size);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif