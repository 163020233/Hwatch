/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_ldci_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_LDCI_STRUCT_H__
#define __ISP_LDCI_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define EXT_ISP_LDCI_LPF_LUT_SIZE 5
#define EXT_ISP_DCI_BLD_GAIN_LEN 5
#define EXT_ISP_DCI_CPOS_LEN 8
#define EXT_ISP_DCI_CNEG_LEN 8
#define EXT_ISP_DCI_DITHER_ROUND_MAX 2
#define EXT_ISP_DCI_INPUT_RANGE_MAX 1
#define EXT_ISP_DCI_OUTPUT_RANGE_MAX 1
#define EXT_ISP_DCI_BLD_GAIN_MAX 64
#define EXT_ISP_DCI_CPOS_THR_MAX 255
#define EXT_ISP_DCI_CNEG_THR_MAX 255
#define EXT_ISP_DCI_CPOS_SLP_MAX 511
#define EXT_ISP_DCI_CPOS_SLP_MIN (-512)
#define EXT_ISP_DCI_CNEG_SLP_MAX 511
#define EXT_ISP_DCI_CNEG_SLP_MIN (-512)
#define EXT_ISP_DCI_CPOS_GAIN_MAX 15
#define EXT_ISP_DCI_CPOS_GAIN_MIN (-16)
#define EXT_ISP_DCI_CNEG_GAIN_MAX 15
#define EXT_ISP_DCI_CNEG_GAIN_MIN (-16)

/************************************* ldci *******************************************/
typedef struct {
    td_u8 wgt;   /* Format:1.7;Weight of Gaussian distribution */
    td_u8 sigma; /* RW;Range: [0x1, 0xFF];Format:0.8;Sigma of Gaussian distribution */
    td_u8 mean;  /* RW;Range: [0x0, 0xFF];Format:0.8;Mean of Gaussian distribution */
} ext_isp_ldci_gauss_coef_attr;

typedef struct {
    ext_isp_ldci_gauss_coef_attr  he_pos_wgt;
    ext_isp_ldci_gauss_coef_attr  he_neg_wgt;
} ext_isp_ldci_he_wgt_attr;

typedef struct {
    ext_isp_ldci_he_wgt_attr  he_wgt;
    td_u16                   blc_ctrl;  /* RW;Range: [0x0, 0x1FF];Format:9.0;Restrain dark region */
} ext_isp_ldci_manual_attr;

typedef struct {
    ext_isp_ldci_he_wgt_attr  he_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u16                   blc_ctrl[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range: [0x0, 0x1FF];Format:9.0;auto mode,
                                                                     Restrain dark region */
} ext_isp_ldci_auto_attr;

typedef struct {
    td_u8 dither_round; /* RW;Range: [0x0, 0x2] */
    td_bool binc_en; /* RW;Range: [0x0, 0x1] */
    td_bool color_com_en;  /* RW;Range: [0x0, 0x1] */
    td_bool alpha_fix; /* RW;Range: [0x0, 0x1] */
    td_u8 input_range;  /* RW;Range: [0x0, 0x1] */
    td_u8 output_range;  /* RW;Range: [0x0, 0x1] */
    td_u8 bld_gain[EXT_ISP_DCI_BLD_GAIN_LEN]; /* RW;Range: [0x0, 0x40] */
    td_u8 cpos_thr[EXT_ISP_DCI_CPOS_LEN]; /* RW;Range: [0x0, 0xFF] */
    td_u8 cneg_thr[EXT_ISP_DCI_CNEG_LEN]; /* RW;Range: [0x0, 0xFF] */
    td_s16 cpos_slp[EXT_ISP_DCI_CPOS_LEN]; /* RW;Range: [-512, 511] */
    td_s16 cneg_slp[EXT_ISP_DCI_CNEG_LEN]; /* RW;Range: [-512, 511] */
    td_s8 cpos_gain[EXT_ISP_DCI_CPOS_LEN]; /* RW;Range: [-16, 15] */
    td_s8 cneg_gain[EXT_ISP_DCI_CNEG_LEN]; /* RW;Range: [-16, 15] */
} ext_isp_gdci_ldci_attr;

typedef struct {
    td_bool                 enable;          /* RW;Range:[0x0,0x1];Format:1.0; */
    td_u8                   gauss_lpf_sigma; /* RW;Range: [0x1, 0xFF];Format:0.8;
                                                Coefficient of Gaussian low-pass filter */
    td_u8                   lpf_coef[EXT_ISP_LDCI_LPF_LUT_SIZE]; /* RW;Range: [0x0, 0xFF] */
    ext_isp_op_type          op_type;
    ext_isp_ldci_manual_attr manual_attr;
    ext_isp_ldci_auto_attr   auto_attr;
    ext_isp_gdci_ldci_attr   dci_attr;
    td_u16                  tpr_incr_coef;   /* RW;Range: [0x0, 0x100];Format:0.9;
                                                Increase Coefficient of temporal filter */
    td_u16                  tpr_decr_coef;   /* RW;Range: [0x0, 0x100];Format:0.9;
                                                Decrease Coefficient of temporal filter */
} ext_isp_ldci_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
