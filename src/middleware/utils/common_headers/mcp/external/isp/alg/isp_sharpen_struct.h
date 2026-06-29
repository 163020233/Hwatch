/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_sharpen_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_SHARPEN_STRUCT_H__
#define __ISP_SHARPEN_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* sharpen *******************************************/
/* SHARPEN */
typedef struct {
    td_u16 shoot_inner_threshold;   /* RW; Range: [0, 511]; Format: 9.0; Start threshold of soft processing
                                       interval, Not Support for 850 */
    td_u16 shoot_outer_threshold;   /* RW: Range: [0, 511]; Format: 9.0; End threshold of sort processing
                                       interval, Not Support for 850 */
    td_u16 shoot_protect_threshold; /* RW: Range: [0, 511]; Format: 9.0; Hard processing of input threshold,
                                       Not Support for 850 */
} ext_isp_sharpen_manual_shoot_threshold_attr;

typedef struct {
    td_u16 edge_rly_fine_threshold_low;      /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of var5, Not support for 850 */
    td_u16 edge_rly_fine_threshold_mid;      /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; MId threshold of var5, Not support for 850 */    
    td_u16 edge_rly_fine_threshold_high;     /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of var5, Not support for 850 */  
    td_u16 edge_rly_coarse_threshold_low;    /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; Low threshold of var7x9, Not support for 850 */
    td_u16 edge_rly_coarse_threshold_high;   /* 1A22 New Added; RW; Range: [0, 1023]; Format: 10.0; High threshold of var7x9, Not support for 850 */
    td_u8  edge_overshoot;    /* RW; Range: [0, 127]; Format: 7.0; edge overshoot, Not support for 850 */
    td_u8  edge_undershoot;   /* RW; Range: [0, 127]; Format: 7.0; edge undershoot, Not support for 850 */

    td_u8  edge_gain_by_rly[ISP_SHARPEN_RLYWGT_NUM];                  /* RW; Range: [0, 255];
              Format: 3.5; weight calculated by dirRly, Not support for 850 */
    td_u8  edge_rly_by_mot[ISP_SHARPEN_STDGAIN_NUM];     /* RW; Range: [0, 63];
             Format: 2.4; stardard gain lut calculated by motion, Not support for 850 */
    td_u8 edge_rly_by_luma[ISP_SHARPEN_STDGAIN_NUM]; /* RW; Range: [0, 63];
          Format: 2.4; stardard gain lut calculated by luma, Not support for 850 */
} ext_isp_sharpen_manual_edge_rly_attr;

typedef struct {
    td_u8 mf_gain_by_mot[ISP_SHARPEN_MOT_NUM];  /* RW; Range: [0, 32];
Format: 1.5; mf decline weight by motion, Not support for 850 */
    td_u8 hf_gain_by_mot[ISP_SHARPEN_MOT_NUM];  /* RW; Range: [0, 32];
             Format: 1.5; hf decline weight by motion, Not support for 850 */
    td_u8  lmf_gain_by_mot[ISP_SHARPEN_MOT_NUM];                /* RW; Range: [0, 255];
                 Format: 4.4; lmf gain by mot, Not support for 850 */
} ext_isp_sharpen_manual_gain_by_mot_attr;

typedef struct {
    td_u8  detail_ctrl;        /* Different sharpen strength for detail and edge.
                                  When it is bigger than 128, detail sharpen strength will be stronger than edge */
    td_u8  detail_ctrl_threshold;    /* The threshold of DetailCtrl, it is used to distinguish detail and edge. */

    td_u8  edge_filt_max_cap;  /* RW; Range: [0, 47]; Format:6.0;The max capacity of edge filtering. */

    td_u8  skin_gain;          /* RW; Range: [0, 31]; Format:5.0;Sharpen Gain for Skin Area */
    td_u8  weak_detail_gain;   /* RW; Range: [0, 127]; sharpen Gain for weakdetail. Not support for 1A22 */
} ext_isp_sharpen_manual_adv_attr_v1;    /* Just support 850 */

typedef struct {
    td_u16 max_gain_adj_by_rly;          /* 1A22 New Added; Range: [0, 4095]; MaxGainAdjByRly */
    td_u8  rly_wgt_only_for_dir_gain;    /* 1A22 New Added; Range: [0, 1]; rlyWgtOnlyForDirGain */

    td_u8  detail_ctrl_en;        /* 1A22 New Added; RW; Range: [0, 1]; bEnDetailCtrl */                
    td_u8  detail_over_shoot;     /* 1A22 New Added; RW; Range: [0, 127]; detailOshtAmt */    
    td_u8  detail_under_shoot;    /* 1A22 New Added; RW; Range: [0, 127]; detailUshtAmt */    
    td_u8  detail_osht_thr0;      /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[0] */
    td_u8  detail_osht_thr1;      /* 1A22 New Added; RW; Range: [0, 255]; detailOshtThr[1] */
    td_u8  detail_usht_thr0;      /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[0] */
    td_u8  detail_usht_thr1;      /* 1A22 New Added; RW; Range: [0, 255]; detailUshtThr[1] */

    td_u8  edge_filt_dir_rt0;    /* 1A22 New Added; Range: [0, 31]; dirRt[0] */
    td_u8  edge_filt_dir_rt1;    /* 1A22 New Added; Range: [0, 31]; dirRt[1] */

    td_u8  skin_ctrl_en;       /* 1A22 New Added; Range: [0, 1]; */
    td_u8  skin_max_u;       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_min_u;       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_max_v;       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_min_v;       /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_cnt_thd0;    /* 1A22 New Added; Range: [0, 9]; */
    td_u8  skin_cnt_thd1;    /* 1A22 New Added; Range: [0, 9]; */
    td_u8  skin_edge_sft;    /* 1A22 New Added; Range: [0, 12]; */
    td_u8  skin_edge_thd0;     /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_edge_thd1;     /* 1A22 New Added; Range: [0, 255]; */
    td_u8  skin_edge_wgt0;     /* 1A22 New Added; Range: [0, 31]; */
    td_u8  skin_edge_wgt1;     /* 1A22 New Added; Range: [0, 31]; */

    td_u8 shoot_ctrl_by_var_en; /* Range:[0, 1] */
    td_u8 shoot_blend_rate; /* Range:[0, 15] */
    td_u8 shoot_var_thd0; /* Range:[0, 255] */
    td_u8 shoot_var_thd1; /* Range:[0, 255] */
    td_u8 shoot_var_wgt0; /* Range:[0, 127] */
    td_u8 shoot_var_wgt1; /* Range:[0, 127] */
    td_u8 shoot_var_diff_thd0; /* Range:[0, 255] */
    td_u8 shoot_var_diff_thd1; /* Range:[0, 255] */
    td_u8 shoot_var_diff_wgt0; /* Range:[0, 127] */
    td_u8 shoot_var_diff_wgt1; /* Range:[0, 127] */
} ext_isp_sharpen_manual_adv_attr_v2;    /* Just support 1A22 */

typedef struct {
    td_u8  luma_wgt[ISP_SHARPEN_LUMA_NUM]; /* Adjust the sharpen strength according to luma.
                                               Sharpen strength will be weaker when it decrease. */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM]; /* Undirectional sharpen strength for texture and detail enhancement */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM];    /* Directional sharpen strength for edge enhancement */
    td_u16 texture_freq;       /* Texture frequency adjustment. Texture and detail will be finer when it increase */
    td_u16 edge_freq;            /* Edge frequency adjustment.Edge will be narrower and thiner when it increase */
    td_u8  over_shoot;         /* u8OvershootAmt */
    td_u8  under_shoot;        /* u8UndershootAmt */
    td_u8  shoot_sup_strength;      /* overshoot and undershoot suppression strength, the
                                  amplitude and width of shoot will be decrease when shootSupSt increase */
    td_u8  shoot_sup_adj;      /* overshoot and undershoot suppression adjusting,
                                  adjust the edge shoot suppression strength */
    td_u8  edge_filt_strength;      /* RW; Range: [0, 63]; Format:6.0;The strength of edge filtering. */

    td_u8  r_gain;             /* RW; Sharpen Gain for Red Area ,
                                  Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */
    td_u8  g_gain;             /* RW; Range: [0, 255]; Format:8.0; Sharpen Gain for Green Area */
    td_u8  b_gain;             /* RW; Sharpen Gain for Blue Area,
                                  Range: 850 = [0, 31] | Hi3516CV610 = [0, 63] */

    td_u16 max_sharp_gain;     /* RW; Range: [0, 0x7FF]; Format:8.3; Maximum sharpen gain */

    union {
        ext_isp_sharpen_manual_adv_attr_v1 adv_attr_v1;
        ext_isp_sharpen_manual_adv_attr_v2 adv_attr_v2;
    };

    ext_isp_sharpen_manual_shoot_threshold_attr shoot_threshold_attr; /* RW; Not support for 850 */
    ext_isp_sharpen_manual_edge_rly_attr edge_rly_attr;               /* RW; Not support for 850 */
    ext_isp_sharpen_manual_gain_by_mot_attr gain_by_mot_attr;              /* RW; Not support for 850 */
} ext_isp_sharpen_manual_attr;

typedef struct {
    td_u8  roi_region_cnt; /* 1A22 New Added; Range: [0, 10]; skinEdgeWgt[1] */
    td_u16 roi_x0[ISP_SHAREPEN_ROI_NUM_MAX]; /* 1A22 New Added; Range: [0, 16384]; skinEdgeWgt[1] */
    td_u16 roi_y0[ISP_SHAREPEN_ROI_NUM_MAX]; /* 1A22 New Added; Range: [0, 16384]; skinEdgeWgt[1] */
    td_u16 roi_x1[ISP_SHAREPEN_ROI_NUM_MAX]; /* 1A22 New Added; Range: [0, 16384]; skinEdgeWgt[1] */
    td_u16 roi_y1[ISP_SHAREPEN_ROI_NUM_MAX]; /* 1A22 New Added; Range: [0, 16384]; skinEdgeWgt[1] */
} ext_isp_sharpen_skin_roi;

typedef struct {
    td_bool enable;    /* RW; Range:[0, 1]; Format:1.0;Enable/Disable sharpen module */
    td_u8 skin_umin;   /* RW; Range: [0, 255];  Format:8.0; U min value of the range of skin area */
    td_u8 skin_vmin;   /* RW; Range: [0, 255];  Format:8.0; V min value of the range of skin area */
    td_u8 skin_umax;   /* RW; Range: [0, 255];  Format:8.0; U max value of the range of skin area */
    td_u8 skin_vmax;   /* RW; Range: [0, 255];  Format:8.0; V max value of the range of skin area */
    ext_isp_op_type op_type;
    ext_isp_sharpen_manual_attr manual_attr;
    ext_isp_sharpen_auto_attr   auto_attr;
    ext_isp_sharpen_skin_roi    skin_roi;
} ext_isp_sharpen_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
