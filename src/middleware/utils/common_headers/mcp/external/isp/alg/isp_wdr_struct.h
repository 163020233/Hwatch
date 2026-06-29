/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_wdr_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_WDR_STRUCT_H__
#define __ISP_WDR_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* wdr *******************************************/
#define NOISESET_ELENUM                 7

typedef enum {
    MERGE_WDR_MODE      = 0,
    MERGE_FUSION_MODE   = 1,
    MERGE_BUTT
} ext_isp_wdr_merge_mode;

typedef enum {
    WDR_WBGAIN_IN_DG1         = 0,
    WDR_WBGAIN_IN_WB          = 1,
    WDR_WBGAIN_POSITION_BUTT
} ext_isp_wdr_wb_gain_position;

typedef struct {
    td_u8 md_thr_low_gain;
    td_u8 md_thr_hig_gain;
} ext_isp_fswdr_manual_attr;

typedef struct {
    td_u8 md_thr_low_gain[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8 md_thr_hig_gain[ISP_AUTO_ISO_STRENGTH_NUM];
} ext_isp_fswdr_auto_attr;

typedef struct {
    td_bool short_expo_chk;     /* RW;Range:[0x0,0x1];Format:1.0; */
    td_u16  short_check_thd;   /* RW;Range:[0x0,0xFFF];Format:12.0 */
    td_bool md_ref_flicker;
    td_u8   mdt_still_thd;     /* RW;Range:[0x0,0xFE];Format:8.0 */
    td_u8   mdt_full_thd;      /* RW;Range:[0x0,0xFE];Format:8.0 */
    td_u8   mdt_long_blend;    /* RW;Range:[0x0,0xFE] */
    ext_isp_op_type op_type;
    ext_isp_fswdr_manual_attr manual_attr;
    ext_isp_fswdr_auto_attr   auto_attr;
} ext_isp_fswdr_mdt_attr;

typedef struct {
    td_bool motion_comp;        /* RW;Range:[0, 0x1];Format:1.0; enable/disable motion compensation */
    td_u16  short_thr;          /* RW;Range:[0, 0xFFF];Format:12.0; data above this threshold will be taken from
                                   short exposure only. */
    td_u16  long_thr;           /* RW;Range:[0, 0xFFF];Format:12.0; limited range:[0x0, short_thr], data below this
                                   threshold will be taken from long exposure only. */
    td_bool force_long;         /* RW;Range:[0, 1];Format:1.0; enable/disable force long; */
    td_u16  force_long_low_thr; /* RW;Range:[0, 0xFFF];Format:12.0; data above this threshold will force to choose
                                   long frame */
    td_u16  force_long_hig_thr; /* RW;Range:[0, 0xFFF];Format:12.0; data below this threshold will force to choose
                                   long frame only */
    ext_isp_fswdr_mdt_attr wdr_mdt;
} ext_isp_wdr_combine_attr;

typedef struct {
    td_u16  fusion_thr[ISP_WDR_FRAME_MAX];    /* RW;Range:[0x0,0x3FFF];Format:14.0;the threshold of the 4 frame */
} ext_isp_fusion_attr;

typedef enum {
    BNR_OFF_MODE         = 0,
    BNR_ON_MODE          = 1,
    BNR_BUTT
} ext_isp_bnr_mode;

typedef struct {
    ext_isp_bnr_mode bnr_mode;   /* RW;Range:[0, 1];Format:1.0; enable/disable the bnr of wdr */
    td_bool short_frame_nr;     /* RW;Range:[0, 1];Format:1.0; enable/disable the bnr of short frame */
    td_u8   short_frame_nr_str; /* RW;Range:[0, 0x3F];Format:8.0;the Level 2 short frame sigma weight of G channel */
    td_u8   fusion_bnr_str;     /* RW;Range:[0, 0x3F];Format:3.3 */
    td_u8   noise_floor[NOISESET_ELENUM]; /* RW;Range:[0x0,0xFF];Format:8.0 */
    td_u8   full_mdt_sig_g_wgt;  /* RW;Range:[0, 0x1F];Format:8.0 */
    td_u8   full_mdt_sig_rb_wgt; /* RW;Range:[0, 0x1F];Format:8.0 */
    td_u8   full_mdt_sig_wgt;    /* RW;Range:[0, 0x1F];Format:8.0 */
    td_u8   g_sigma_gain[3];     /* RW;Range:[0, 0xFF];Format:8.0 */
    td_u8   rb_sigma_gain[3];    /* RW;Range:[0, 0xFF];Format:8.0 */
} ext_isp_wdr_bnr_attr;

typedef struct {
    ext_isp_wdr_merge_mode   wdr_merge_mode;
    ext_isp_wdr_combine_attr wdr_combine;
    ext_isp_wdr_bnr_attr     bnr_attr;
    ext_isp_fusion_attr      fusion_attr;
    ext_isp_wdr_wb_gain_position wdr_wb_gain_position; /* RW; Range: [0x0, 0x1]; WDR Gain with or without WB Gain. */
} ext_isp_wdr_fs_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
