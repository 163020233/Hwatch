/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_gdci_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-10-11
 */

#ifndef __ISP_GDCI_STRUCT_H__
#define __ISP_GDCI_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define GDCI_ISO_THD_NUM             8
#define GDCI_BIN_NUM                 32
#define GDCI_LUT_NUM                 (GDCI_BIN_NUM + 1)

#define GDCI_ROI_NUM_MAX              10

/************************************* gdci *******************************************/
typedef struct {
    td_u8 dci_flat_en;           /* RW;Range:[0, 2];Format:2.0; */
    td_u8 dci_color_adapt_en;    /* RW;Range:[0, 1];Format:1.0; */
    td_u8 dci_color_per;         /* RW;Range:[0, 127];Format:7.0; */
    td_u8 dci_flat_rshift;          /* RW;Range:[0, 15];Format:4.0; */
    td_u8 dci_flat_grad_thrsh;     /* RW;Range:[0, 255];Format:8.0; */
    td_u16 dci_flat_thrsh;          /* RW;Range:[0, 1023];Format:10.0; */
} ext_isp_gdci_flat_attr;

typedef struct {
    /* HE */
    td_u8 man_alg_en;           /* RW;Range:[0, 1];Format:1.0; */
    td_u8 man_alg;           /* RW;Range:[0, 127];Format:7.0; just valid when man_alg_en == 1  */
    td_u8 hist_map_gain_lft[GDCI_ISO_THD_NUM];           /* RW;Range:[0, 16];Format:5.0; */
    td_u8 hist_map_gain_rgt[GDCI_ISO_THD_NUM];           /* RW;Range:[0, 16];Format:5.0; */
    td_u8 hist_map_ablend;           /* RW;Range:[0, 31];Format:5.0; */

    /* CF */
    td_u8 hist32_ablend;           /* RW;Range:[0, 32];Format:6.0; */
    td_u8 bin_wgt[GDCI_METRIC_NUM][GDCI_BIN_NUM];           /* RW;Range:[0, 31];Format:5.0; */
    td_u8 man_adj_en[GDCI_METRIC_NUM];           /* RW;Range:[0, 1];Format:1.0; */
    td_u8 man_adj_wgt[GDCI_METRIC_NUM];           /* RW;Range:[0, 255];Format:8.0; just valid when man_adj_en == 1 */
    td_u8 metrc_ablend[GDCI_METRIC_NUM];           /* RW;Range:[0, 32];Format:6.0; */
    td_s16 adj_lut[GDCI_METRIC_NUM][GDCI_LUT_NUM];           /* RW;Range:[-512, 511];Format:11.0; */
    td_u8 glb_gain[GDCI_METRIC_NUM][GDCI_ISO_THD_NUM];           /* RW;Range:[0, 63];Format:6.0; */
    td_u8 sw_cfg_en;              /* RW;Range:[0, 1];Format:1.0; */
} ext_isp_gdci_cf_he_attr;

typedef struct {
    td_u8 blk_str_en;                    /* RW;Range:[0, 1] ;Format:1.0; */
    td_u8 accumhist_index;           /* RW;Range:[0, 255];Format:8.0; */
    td_u16 pixsum_thrsh;              /* RW;Range:[0, 1023];Format:10.0; */
    td_u8 accumhist_gain;            /* RW;Range:[0, 127];Format:7.0; */
    td_u8 minblack_gain;             /* RW;Range:[0, 63];Format:6.0; */
    td_s8 minblack_offset;           /* RW;Range:[-32, 31];Format:6.0; */
    td_u8 adjust_gain;               /* RW;Range:[0, 63];Format:6.0; */
    td_u8 timeflt_bldwgt;            /* RW;Range:[0, 32];Format:6.0; */
    td_u16 right_bond;                /* RW;Range:[0, 1023];Format:10.0; */
} ext_isp_gdci_bs_attr;

typedef struct {
    td_u8 skinprtc_en;                     /* RW;Range:[0, 1] ;Format:1.0; */
    td_u8 skinprtc_roi_en;                    /* RW;Range:[0, 1] ;Format:1.0; */
    ext_isp_op_type roi_type;               /* RW;Range:[0, 1] ;Format:1.0; roi rect mode */
    td_u8 roi_region_cnt;                  /* RW;Range:[1, 10] ;Format:4.0; just valid when op_type = MANUAL */
    ext_isp_rect roi_rect[GDCI_ROI_NUM_MAX]; /* RW; width/height range: [32，4096]; just valid when op_type = MANUAL */
    td_u8  skinlut[GDCI_SKIN_LUT_MAX_IDX_U][GDCI_SKIN_LUT_MAX_IDX_V]; /* RW;Range:[0, 256] ;Format:9.0; */
} ext_isp_gdci_skin_protect_attr;

typedef struct {
    td_bool                 enable;          /* RW;Range:[0, 1];Format:1.0; */
    td_u8                   org_ablend;      /* RW;Range:[0, 63];Format:6.0; XXXXX */
    ext_isp_gdci_flat_attr  flat_attr;
    ext_isp_gdci_cf_he_attr cf_he_attr;
    ext_isp_gdci_bs_attr    bs_attr;
    ext_isp_gdci_skin_protect_attr skin_prot_attr;
} ext_isp_gdci_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
