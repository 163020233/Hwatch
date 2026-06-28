/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_lcac_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_LCAC_STRUCT_H__
#define __ISP_LCAC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* lcac *******************************************/
#define ISP_LCAC_DET_NUM                3
#define ISP_EXP_RATIO_STRENGTH_NUM      16

/* CAC */
typedef struct {
    td_u8 de_purple_cr_str;   /* RW;Range: [0,8];Format:4.0;Correction strength of the R channel */
    td_u8 de_purple_cb_str;   /* RW;Range: [0,8];Format:4.0;Correction strength of the B channel */
} ext_isp_depurplestr_manual_attr;

typedef struct {
    td_u8 de_purple_cr_str[ISP_EXP_RATIO_STRENGTH_NUM]; /* RW;Range: [0, 8];Format:4.0;
                                                           Correction strength of the R channel */
    td_u8 de_purple_cb_str[ISP_EXP_RATIO_STRENGTH_NUM]; /* RW;Range: [0, 8];Format:4.0;
                                                           Correction strength of the B channel */
} ext_isp_depurplestr_auto_attr;

typedef struct {
    td_bool enable;               /* RW;Range: [0, 1];Format:1.0; enable/disable local cac */
    td_u16  purple_det_range;     /* RW;Range: [0, 410];Format:9.0;Purple fringing detection scope */
    td_u16  var_thr;              /* RW;Range: [0, 4095];Format:12.0;Edge detection threshold */
    td_u16  r_det_thr[ISP_LCAC_DET_NUM];   /* RW;Range: [0,4095];Format:12.0;Component R detection threshold */
    td_u16  g_det_thr[ISP_LCAC_DET_NUM];   /* RW;Range: [0,4095];Format:12.0;Component G detection threshold */
    td_u16  b_det_thr[ISP_LCAC_DET_NUM];   /* RW;Range: [0,4095];Format:12.0;Component B detection threshold */
    td_u16  luma_det_thr[ISP_LCAC_DET_NUM];  /* RW;Range: [0,4095];Format:12.0;Component Luma detection threshold */
    td_s16  cbcr_ratio[ISP_LCAC_DET_NUM];  /* RW;Range: [-2048,2047];Format:S12.0; Cb/Cr ratio threshold */
    ext_isp_op_type      op_type;  /* RW;Range: [0, 1];Format:1.0;Purple fringing correction working mode */
    ext_isp_depurplestr_manual_attr manual_attr;
    ext_isp_depurplestr_auto_attr   auto_attr;
} ext_isp_local_cac_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
