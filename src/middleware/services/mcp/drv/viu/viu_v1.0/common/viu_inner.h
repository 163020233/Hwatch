/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2015-2021. All rights reserved.
 * Description: viu_inner.h
 * Author: AuthorNameMagicTag
 * Create: 2015-09-22
 */

#ifndef VIU_INNER_H
#define VIU_INNER_H

#include "drv_viu_struct.h"
#include "viu_public.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    td_bool enable;
    void EXT_ISP_ATTRIBUTE *regs;
} viu_img_quality_cfg;

typedef struct {
    ext_dis_dsf_hw_cfg ext_dis_dsf_hw_cfg[2];
} viu_dis_dsf_hw_cfg;

typedef struct {
    td_u32 distortion_level;
    td_bool is_ldc;
    td_bool is_lp_filter;
    /* FPD&GME ROI */
    td_u32 roi_offset_x;
    td_u32 roi_offset_y;
    td_u32 roi_width;
    td_u32 roi_height;

    /* FPD&GME shared parameters */
    td_bool use_default_gme_param;
    td_u32 gme_down_sample_factor;
    td_u32 gme_neighbor_min_dist;
    td_u32 gme_max_num_key_pts;

    /* config the feature detector parameter */
    td_u32 det_quality_level; /* quality level */
    td_u32 min_eigen_val;

    /* config GME parameter */
    td_u32 trk_max_count; /* max iteration count */
    td_u32 gme_refresh_thresh;
    td_u32 gme_replenish_thres;
    td_u32 max_mv_norm;
    td_u32 gme_hori_limit;  /* dynamic */
    td_u32 gme_vert_limit;  /* dynamic */
    td_u32 prescreen_trk_thr;
    td_u32 ldc_ratio;

    /* config the motion filter parameter */
    td_u32 motion_anchor; /* 0:ANCHOR_CORNER 1:ANCHOR_CENTER */

    td_bool use_default_ml_and_drift_param;
    td_bool motion_limit_check;
    td_u32 ml_zoom_in_limit;
    td_u32 ml_zoom_out_limit;
    td_u32 ml_rotate_thresh;
    td_u32 ml_skew_ratio_limit;
    td_u32 ml_skew_angle_limit;

    td_bool drift_param_fix;
    td_u32 drift_param_min_coef;
    td_u32 drift_param_max_coef;
    td_u32 drift_strength;
    td_u32 stab_motion_adjust_coef;

    td_u32 min_inlier_ratio;
    td_bool use_default_filtering_param;
    td_u32 ave_mot_strength;
    td_u32 ave_mot_forget_factor;

    td_bool use_default_rs_limit_param;
    td_u32 rs_hori_min_thresh;
    td_u32 rs_hori_diff_thresh;
    td_u32 rs_vert_min_thresh;
    td_u32 rs_vert_diff_thresh;
    td_u32 rs_correction;  /* rolling shutter correction coeficient */

    td_bool use_default_params;
    td_u32 fuse_corr_thresh_xy;
    td_u32 fuse_corr_thresh_z;
    td_u32 gyro_steady_thre_xy;
    td_u32 gyro_steady_thre_z;
    td_u32 gme_gyro_max_diff_ratio_thre;
    td_u32 gme_reliable_cnt_thr;

    td_bool after3_dnr;
} dis_debug_cfg;

typedef enum {
    VIU_NR_INTF_X = 0,
    VIU_NR_INTF_BUTT
} viu_nr_intf;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* VIU_INNER_H */
