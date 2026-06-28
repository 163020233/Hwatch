/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_dp_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_DP_STRUCT_H__
#define __ISP_DP_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* dp *******************************************/
/* DP */
typedef enum {
    ISP_STATE_INIT     = 0,
    ISP_STATE_SUCCESS  = 1,
    ISP_STATE_TIMEOUT  = 2,
    ISP_STATE_BUTT
} ext_isp_status;

typedef enum {
    ISP_STATIC_DP_BRIGHT = 0x0,
    ISP_STATIC_DP_DARK,
    ISP_STATIC_DP_BUTT
} ext_isp_static_dp_type;

typedef struct {
    td_bool enable_detect;  /* Set 'TD_TRUE'to start static defect-pixel calibration,
                                and firmware will set 'TD_FALSE' when finished, */
    ext_isp_static_dp_type static_dp_type; /* Select static bright/dark defect-pixel calibration */
    td_u8  start_thresh;  /* Start threshold for static defect-pixel calibraiton */
    td_u16 count_max;     /* Limit of max number of static defect-pixel calibraiton. */
    td_u16 count_min;     /* Limit of min number of static defect-pixel calibraiton. */
    td_u16 time_limit;    /* Time limit for static defect-pixel calibraiton, in frame number */

    td_u32 table[STATIC_DP_COUNT_MAX];  /* Static defect-pixel calibraiton table,
                                           0~12 bits represents the X coordinate of the defect pixel,
                                           16~28 bits represent the Y coordinate of the defect pixel. */
    td_u8  finish_thresh;   /* Finish threshold for static defect-pixel calibraiton */
    td_u16 count;          /* Finish number for static defect-pixel calibraiton */
    ext_isp_status status;  /* Status of static defect-pixel calibraiton */
} ext_isp_dp_static_calibr;

typedef struct {
    td_bool enable;       /* Enable/disable the static defect-pixel module */
    td_u16  bright_count; /* When used as input(W), indicate the number of static bright defect pixels;
                             As output(R),indicate the number of static bright and dark defect pixels. */
    td_u16  dark_count;   /* When used as input(W), indicate the number of static dark defect pixels;
                             As output(R), invalid value 0. */
    td_u32  bright_table[STATIC_DP_COUNT_MAX]; /* 0~12 bits represents the X coordinate of the defect pixel,
                                                  16~28 bits represent the Y coordinate of the defect pixel
                                                  Notice: When used as input(W), indicate static bright defect pixels
                                                  table; As output(R), indicate static bright and dark defect pixels
                                                  table. */

    td_u32  dark_table[STATIC_DP_COUNT_MAX]; /* 0~12 bits represents the X coordinate of the defect pixel,
                                                16~28 bits represent the Y coordinate of the defect pixel
                                                Notice: When used as input(W), indicate static dark defect pixels table;
                                                As output(R), invalid value. */
    td_bool show;                  /* highlight static defect pixel */
} ext_isp_dp_static_attr;

typedef struct {
    td_u16  strength;           /* RW; Range: [0, 255];Format:8.0;Dynamic DPC strength. */
    td_u16  blend_ratio;        /* RW; Range: [0, 128];Format:9.0;Blending ratio required for DPC */
    // 1A22 Add
    td_u16 flat_adapt_th0;      /* Range: [0, 255] */
    td_u16 flat_adapt_th1;      /* Range: [0, 255] */
    td_u16 dir_adapt_th0;       /* Range: [0, 255] */
    td_u16 dir_adapt_th1;       /* Range: [0, 255] */
    // Add end
} ext_isp_dp_dynamic_manual_attr;

typedef struct {
    td_u16  strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* Dynamic DPC strength. */
    td_u16  blend_ratio[ISP_AUTO_ISO_STRENGTH_NUM]; /* Blending ratio required for DPC */
    // 1A22 Add
    td_u16 flat_adapt_th0[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range: [0, 255] */
    td_u16 flat_adapt_th1[ISP_AUTO_ISO_STRENGTH_NUM]; /* Range: [0, 255] */
    td_u16 dir_adapt_th0[ISP_AUTO_ISO_STRENGTH_NUM];  /* Range: [0, 255] */
    td_u16 dir_adapt_th1[ISP_AUTO_ISO_STRENGTH_NUM];  /* Range: [0, 255] */
    // Add end
} ext_isp_dp_dynamic_auto_attr;

#define ISP_DP_ALG_THR_NUM 5
typedef struct {
    td_u8 line_thr_rb[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 line_thr_g[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 line_mad_fac_rb[ISP_DP_ALG_THR_NUM]; /* Range:[0, 63] */
    td_u8 line_mad_fac_g[ISP_DP_ALG_THR_NUM]; /* Range:[0, 63] */
    td_u8 pg_fac_rb[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 pg_fac_g[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 rg_fac_rb[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 rg_fac_g[ISP_DP_ALG_THR_NUM]; /* Range:[0, 255] */
    td_u8 ro_rb[ISP_DP_ALG_THR_NUM]; /* Range:[0, 3] */
    td_u8 ro_g[ISP_DP_ALG_THR_NUM]; /* Range:[0, 3] */
} ext_isp_dp_alg_thr;

typedef struct {
    td_bool enable;             /* Enable/disable the dynamic defect-pixel module */
    td_bool sup_twinkle_en;     /* Enable/disable the twinkle suppression  module */
    td_bool flat_adapt_en;
    td_bool flat_max_min_en;
    td_u8   dir_adapt_mode;     /* Range:[0, 2] */
    td_s8   soft_thr;           /* twinkle suppression threshold */
    td_u8   soft_slope; /* Correction controlling parameter of the pixels whose deviation is smaller than soft_thr. */
    td_u8 bright_strength;      /* RW; Range: [0, 127];Format 8.0, local DPC strength for bright area */
    td_u8 dark_strength;        /* RW; Range: [0, 127];Format 8.0, local DPC strength for dark area */

    ext_isp_op_type op_type;     /* Working mode of dynamic DPC */
    ext_isp_dp_dynamic_manual_attr manual_attr;
    ext_isp_dp_dynamic_auto_attr   auto_attr;
    ext_isp_dp_alg_thr alg_thr;
} ext_isp_dp_dynamic_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
