/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_gcac_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_GCAC_STRUCT_H__
#define __ISP_GCAC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* gcac *******************************************/
typedef struct {
    td_bool enable;            /* enable/disable global cac */
    td_u16  ver_coordinate;    /* Vertical coordinate of the optical center */
    td_u16  hor_coordinate;    /* Horizontal coordinate of the optical center */
    td_s16  param_red_a;       /* Coefficient a of the radius polynomial corresponding to channel R */
    td_s16  param_red_b;       /* Coefficient b of the radius polynomial corresponding to channel R */
    td_s16  param_red_c;       /* Coefficient c of the radius polynomial corresponding to channel R */
    td_s16  param_blue_a;      /* Coefficient a of the radius polynomial corresponding to channel B */
    td_s16  param_blue_b;      /* Coefficient b of the radius polynomial corresponding to channel B */
    td_s16  param_blue_c;      /* Coefficient c of the radius polynomial corresponding to channel B */
    td_u8   ver_norm_shift;    /* Normalized shift parameter in the vertical direction */
    td_u8   ver_norm_factor;   /* Normalized coefficient in the vertical direction */
    td_u8   hor_norm_shift;    /* Normalized shift parameter in the horizontal direction */
    td_u8   hor_norm_factor;   /* Normalized coefficient in the horizontal direction */
    td_u16  cor_var_thr;       /* Variance threshold of lateral chromatic aberration correction */
} ext_isp_global_cac_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
