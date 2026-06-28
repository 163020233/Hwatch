/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description : drv vau hdr file
 */

#ifndef DRV_VAU_HDR_H
#define DRV_VAU_HDR_H

#include "td_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef enum {
    HDR_COLOR_PRIMARIES_BT709         = 0x1,
    HDR_COLOR_PRIMARIES_UNSPECIFIED   = 0x2,
    HDR_COLOR_PRIMARIES_BT601         = 0x5,
    HDR_COLOR_PRIMARIES_BT2020        = 0x9,
    HDR_COLOR_PRIMARIES_MAX
} hdr_color_primaries;

typedef enum {
    HDR_TRANS_CHARAC_UNSPECIFIED      = 0x2,
    HDR_TRANS_CHARAC_GAMMA2_2_TYPE    = 0xd,
    HDR_TRANS_CHARAC_PQ_TYPE          = 0x10,
    HDR_TRANS_CHARAC_HLG_TYPE         = 0x12,
    HDR_TRANS_CHARAC_MAX
} hdr_transfer_characteristics;

typedef enum {
    HDR_MATRIX_COEFFICIENTS_BT709     = 0x1,
    HDR_MATRIX_COEFFICIENTS_BT601     = 0x5,
    HDR_MATRIX_COEFFICIENTS_BT2020NCL = 0x9,
    HDR_MATRIX_COEFFICIENTS_MAX
} hdr_matrix_coefficients;

typedef struct {
    hdr_color_primaries color_primaries;
    hdr_transfer_characteristics transfer_characteristics;
    hdr_matrix_coefficients matrix_coefficients;
    td_u8 video_full_range_flag;
} hdr_image_cicp;

typedef struct {
    td_float base_headroom;
    td_float alternate_headroom;
    td_float min_gainmap[3];
    td_float max_gainmap[3];
    td_float gamma_values[3];
    td_float baseline_image_offset[3];
    td_float alternate_image_offset[3];
} hdr_extend_info;

typedef struct {
    td_u16 display_primaries_x[3];
    td_u16 display_primaries_y[3];
    td_u16 white_point_x;
    td_u16 white_point_y;
    td_u32 max_display_mastering_luminance;
    td_u32 min_display_mastering_luminance;
    td_u16 max_content_light_level;
    td_u16 max_frame_average_light_level;
} hdr_static_metadata;

#define HDR_TM_PARAM_NUM  2
#define HDR_3SPLINE_NUM   2
#define HDR_COLOR_SAT_NUM 8
typedef struct {
    td_u16 system_start_code;                                                // u(8)
    td_u16 minimum_maxrgb_pq;                                                // u(12)
    td_u16 average_maxrgb_pq;                                                // u(12)
    td_u16 variance_maxrgb_pq;                                               // u(12)
    td_u16 maximum_maxrgb_pq;                                                // u(12)
    td_u16 tone_mapping_enable_mode_flag;                                    // u(1)
    td_u16 tone_mapping_param_enable_num;                                    // u(1)
    td_u16 targeted_system_display_maximum_luminance_pq[HDR_TM_PARAM_NUM];   // u(12)
    td_u16 base_enable_flag[HDR_TM_PARAM_NUM];                               // u(1)
    td_u16 base_param_m_p[HDR_TM_PARAM_NUM];                                 // u(14)
    td_u16 base_param_m_m[HDR_TM_PARAM_NUM];                                 // u(6)
    td_u16 base_param_m_a[HDR_TM_PARAM_NUM];                                 // u(10)
    td_u16 base_param_m_b[HDR_TM_PARAM_NUM];                                 // u(10)
    td_u16 base_param_m_n[HDR_TM_PARAM_NUM];                                 // u(6)
    td_u16 base_param_k1[HDR_TM_PARAM_NUM];                                  // u(2)
    td_u16 base_param_k2[HDR_TM_PARAM_NUM];                                  // u(2)
    td_u16 base_param_k3[HDR_TM_PARAM_NUM];                                  // u(4)
    td_u16 base_param_delta_enable_mode[HDR_TM_PARAM_NUM];                   // u(3)
    td_u16 base_param_enable_delta[HDR_TM_PARAM_NUM];                        // u(7)
    td_u16 p_3spline_enable_flag[HDR_TM_PARAM_NUM];                          // u(1)
    td_u16 p_3spline_enable_num[HDR_TM_PARAM_NUM];                           // u(1)
    td_u16 p_3spline_th_enable_mode[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];      // u(2)
    td_u16 p_3spline_th_enable_mb[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];        // f(8)
    td_u16 p_3spline_th_enable[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];           // f(12)
    td_s16 p_3spline_th_enable_delta1[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];    // f(10)
    td_s16 p_3spline_th_enable_delta2[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];    // f(10)
    td_s16 p_3spline_enable_strength[HDR_3SPLINE_NUM][HDR_TM_PARAM_NUM];     // f(8)
    td_u16 color_saturation_mapping_enable_flag;                             // u(1)
    td_u16 color_saturation_enable_num;                                      // u(3)
    td_u16 color_saturation_enable_gain[HDR_COLOR_SAT_NUM];                  // u(8)

    td_u16 monitor_mode_enable;
    td_u16 transfer_character;
    td_u16 graphic_source_display_value;
} hdr_dynamic_metadata;

typedef struct {
    hdr_static_metadata static_metadata;
    hdr_dynamic_metadata dynamic_metadata;
} hdr_metadata_standard;

typedef struct {
    td_u8 image_cnt;
    td_bool is_base_image;
    td_bool is_nonstandard;
    hdr_image_cicp cicp_info;
    hdr_extend_info extend_info;
#ifdef GPU_HDR_METADATA_VALID
    hdr_metadata_standard metadata_standard;
#endif
} ext_hdr_attr;

#define DEGMM_LUT_SIZE               64
#define GMM_LUT_SIZE                 64
#define TM_LUT_SIZE                  64

typedef enum {
    HDR_SINGLE_LAYER             = 0x0,
    HDR_STANDARD_DOUBLE_LAYER    = 0x1,
    HDR_NONSTANDARD_DOUBLE_LAYER = 0x2,
    HDR_BYPASS                   = 0x3,
    HDR_SOURCE_TYPE_MAX
} hdr_source_type;

typedef enum {
    HDR_GMAP_SIZE_MODE_ONE_TO_ONE   = 0x0,
    HDR_GMAP_SIZE_MODE_ONE_TO_TWO_H = 0x1,
    HDR_GMAP_SIZE_MODE_ONT_TO_TWO_V = 0x2,
    HDR_GMAP_SIZE_MODE_ONE_TO_FOUR  = 0x3,
    HDR_GMAP_SIZE_MODE_MAX
} hdr_gmap_size_mode;

typedef struct {
    td_s32 lum_r;         // 50
    td_s32 log_r;         // 10
    td_float base_g;      // 2.3
    td_float base;        // 4
    td_u16 method_idx;    // 0:lutTable 1:model 2:test mode
    td_float hdr_gamma;   // 2.2
    td_float gain_min;    // 1.0
} hdr_pic_disp_param;

typedef struct {
    td_u32 hdr_layer_width;
    td_u32 hdr_layer_height;
    td_u32 hdr_layer_max_value;
    td_u32 ae_lv;
    td_float hdr_luma_ratio;
    td_u32 sdr_display_luma;
    td_u32 is_hdr;
    td_float ev_dif;
} hdr_metadata_nonstandard;

/* csc coef */
typedef struct {
    td_bool csc_en;
    td_u32 scale;
    td_s32 ac_coef[3][3]; /* 3 x 3 matrix */
    td_s32 dc_coef_in[33];
    td_s32 dc_coef_out[3];
    td_u32 clip_min;
    td_u32 clip_max;
} hdr_csc_cfg;

typedef struct {
    td_bool tmap_en;
    td_u32 tmap_luma_cal_scale;
    td_u32 tmap_luma_cal[3];
    td_u32 tmap_mix_alpha;
    td_u32 tmap_mix_alpha_scale;
    td_u32 tmap_coef_scale;
    td_u32 tmap_out_clip_min;
    td_u32 tmap_out_clip_max;
    td_s32 tmap_out_dc[3];
    td_u32 tmap_lut[TM_LUT_SIZE];
} hdr_tmap_cfg;

typedef struct {
    td_bool gamma_en;
    td_u32 gamma_lut[GMM_LUT_SIZE];
} hdr_gamma_cfg;

typedef struct {
    td_bool degmma_en;
    td_u32 degamma_lut[DEGMM_LUT_SIZE];
} hdr_degamma_cfg;

typedef struct {
    td_bool cmb_en;            // only for debug
    td_bool base_only_en;      // only for debug
    td_bool gainmap_only_en;   // only for debug
    td_bool pow_en;

    hdr_gmap_size_mode gainmap_size_mode;
    td_u32 gainmap_components;

    td_float weight;
    td_u32 clip_min;
    td_u32 clip_max;
    td_float gain_min1[3];   /* 3 for r,g,b */
    td_float gain_min2[3];   /* 3 for r,g,b */
    td_float gain_max[3];    /* 3 for r,g,b */
    td_float k_base[3];      /* 3 for r,g,b */
    td_float k_alternate[3]; /* 3 for r,g,b */
} hdr_rcbn_cfg;

typedef struct {
    td_bool scale_en;
    td_u32 gain[4];     /* 4 for r,g,b,w */
    td_s32 offset[4];   /* 4 for r,g,b,w */
    td_u32 clip_min[4]; /* 4 for r,g,b,w */
    td_u32 clip_max[4]; /* 4 for r,g,b,w */
} hdr_scale_cfg;

typedef struct {
    td_u32 img_width;
    td_u32 img_height;
    hdr_image_cicp cicp_base;
} hdr_img_info_base;

typedef struct {
    td_u32 img_width;
    td_u32 img_height;
    td_u32 gainmap_components;
    hdr_extend_info extend_info;
    hdr_image_cicp cicp_gainmap; /* Fix setting: 2 2 5 1 */
    hdr_image_cicp cicp_alternate;
} hdr_img_info_gainmap;

// input
typedef struct {
    hdr_source_type hdr_source_type;
    td_float display_headroom;
    hdr_img_info_base img_info_base;
    hdr_img_info_gainmap img_info_gainmap;
    hdr_metadata_nonstandard metadata_nonstandard;
    hdr_metadata_standard metadata_standard;
    td_bool is_hdr_render;
} drv_hdr_param;

// output
typedef struct {
    hdr_csc_cfg y2r_gainmap_cfg;
    hdr_csc_cfg y2r_base_cfg;
    hdr_csc_cfg gmap1_cfg;
    hdr_csc_cfg gmap2_cfg;
    hdr_tmap_cfg tmap_cfg;
    hdr_degamma_cfg degamma_cfg;
    hdr_gamma_cfg gamma_cfg;
    hdr_rcbn_cfg combine_cfg;
    hdr_scale_cfg scale_cfg;
} drv_hdr_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* DRV_VAU_HDR_H */
