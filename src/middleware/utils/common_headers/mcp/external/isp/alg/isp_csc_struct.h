/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_csc_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_CSC_STRUCT_H__
#define __ISP_CSC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* csc *******************************************/
#define ISP_CSC_DC_NUM                  3
#define ISP_CSC_COEF_NUM                9

/* CSC */
typedef struct {
    td_s16 csc_in_dc[ISP_CSC_DC_NUM];   /* Input dc component for csc matrix */
    td_s16 csc_out_dc[ISP_CSC_DC_NUM];  /* Output dc component for csc matrix */
    td_s16 csc_coef[ISP_CSC_COEF_NUM];  /* 3x3 coefficients for csc matrix */
} ext_isp_csc_matrx;

typedef struct {
    td_bool enable;            /* Enable/Disable CSC Function */

    ext_isp_color_gamut color_gamut; /* Color gamut type; COLOR_GAMUT_BT2020  */
    td_u8   hue;              /* Csc Hue Value */
    td_u8   luma;             /* Csc Luma Value */
    td_u8   contr;            /* Csc Contrast Value */
    td_u8   satu;             /* Csc Saturation Value */
    td_bool limited_range_en; /* Enable/Disable: Enable Limited range output mode(default full range output) */
    td_bool ext_csc_en;       /* Enable/Disable: Enable extended luma range */
    td_bool ct_mode_en;       /* Enable/Disable: Enable ct mode */
    ext_isp_csc_matrx csc_magtrx;     /* Color Space Conversion matrix */
} ext_isp_csc_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
