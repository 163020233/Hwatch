/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Function of mpi_isp_sns_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2023-01-13
 */

#ifndef __MPI_ISP_SNS_STRUCT_H__
#define __MPI_ISP_SNS_STRUCT_H__

#include "isp_clut_struct.h"
#include "isp_de_struct.h"
#include "isp_wdr_struct.h"
#include "isp_lcac_struct.h"
#include "isp_ae_struct.h"
#include "isp_nr_struct.h"
#include "isp_drc_struct.h"
#include "isp_pregamma_struct.h"
#include "isp_lsc_struct.h"
#include "isp_ca_struct.h"
#include "isp_wb_struct.h"
#include "isp_de_struct.h"
#include "isp_gdci_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define ISP_INVALID_IMAGE_MODE            (-1)
#define ISP_DO_NOT_NEED_SWITCH_IMAGEMODE (-2)
#define EXT_ISP_ACS_LIGHT_NUM              32
#define EXT_ISP_ACS_CHN_NUM                2
#define HIST_THRESH_NUM                    4
#define ISP_RGB_CHN_NUM                 3
#define AI_MAX_STEP_FNO_NUM             1024

typedef struct {
    td_s32 kp;             /* RW; Range:[0, 100000]; Format:32.0; the proportional gain of PID algorithm,
                              default value is 7000 */
    td_s32 ki;             /* RW; Range:[0, 1000]; Format:32.0; the integral gain of PID algorithm,
                              default value is 100 */
    td_s32 kd;             /* RW; Range:[0, 100000]; Format:32.0; the derivative gain of PID algorithm,
                              default value is 3000 */
    td_u32 min_pwm_duty;   /* RW; Range:[0, 1000]; Format:32.0; which is the min pwm duty for dciris control */
    td_u32 max_pwm_duty;   /* RW; Range:[0, 1000]; Format:32.0; which is the max pwm duty for dciris control */
    td_u32 open_pwm_duty;  /* RW; Range:[0, 1000]; Format:32.0; which is the open pwm duty for dciris control */
} ext_isp_dciris_attr;

typedef struct {
    td_s32            sensor_id;
} ext_isp_sns_attr_info;

/* Slave mode sensor sync signal generate module */
typedef struct {
    union {
        struct {
            td_u32  bit16_rsv           :  16;
            td_u32  bit_h_inv           :  1;
            td_u32  bit_v_inv           :  1;
            td_u32  bit12_rsv           :  12;
            td_u32  bit_h_enable        :  1;
            td_u32  bit_v_enable        :  1;
        } bits;
        td_u32 bytes;
    } cfg;

    td_u32  vs_time;
    td_u32  hs_time;
    td_u32  vs_cyc;
    td_u32  hs_cyc;
    td_u32  hs_dly_cyc;
    td_u32  slave_mode_time;
} ext_isp_slave_sns_sync;

typedef struct {
    td_bool update;
    td_u16  black_level[ISP_BAYER_CHN_NUM];
} ext_isp_cmos_black_level;

typedef struct {
    td_bool enable;
    td_u8   nddm_strength[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_mf_detail_strength[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   nddm_hf_detail_strength[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8   detail_smooth_range[ISP_AUTO_ISO_STRENGTH_NUM];
} ext_isp_cmos_demosaic;

#define WDR_MAX_FRAME          2
#define BAYER_CALIBRATION_PARA_NUM 3
typedef struct {
    td_bool  enable;
    td_bool  bnr_mono_sensor_en;
    td_bool  nr_lsc_enable;
    td_u8    bnr_lsc_max_gain;
    td_u16   bnr_lsc_cmp_strength;

    td_u8    lut_fine_str[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    chroma_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    wdr_frame_str[ISP_WDR_FRAME_MAX];
    td_u8    fusion_frame_str[ISP_WDR_FRAME_MAX];
    td_u16   coarse_str[ISP_BAYER_CHN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];
    td_u16   lut_coring_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u16   lut_coring_ratio[EXT_ISP_BAYERNR_LUT_LENGTH];
} ext_isp_cmos_bayernr;

#define BAYER_CALIBTAION_MAX_NUM        50
typedef struct {
    td_u16   calibration_lut_num;
    td_float calibration_coef[BAYER_CALIBTAION_MAX_NUM][BAYER_CALIBRATION_PARA_NUM];
} ext_isp_cmos_noise_calibration;

typedef struct {
    ext_isp_de_attr attr;
} ext_isp_cmos_detail;

typedef struct {
    td_u8  luma_wgt[ISP_SHARPEN_LUMA_NUM];       /*  Range: [0, 127] */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM];    /*  Range: [0, 4095]; Format:7.5 */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM];       /*  Range: [0, 4095]; Format:7.5 */
    td_u16 texture_freq;          /*  Range: [0, 4095];Format:6.6 */
    td_u16 edge_freq;             /*  Range: [0, 4095];Format:6.6 */
    td_u8  over_shoot;            /*  Range: [0, 127] */
    td_u8  under_shoot;           /*  Range: [0, 127] */
    td_u8  shoot_sup_strength;         /*  Range: [0, 255] */
    td_u8  shoot_sup_adj;         /*  Range: [0, 15] */
    td_u8  detail_ctrl;           /*  Range: [0, 255] */
    td_u8  detail_ctrl_threshold;       /*  Range: [0, 255] */
    td_u8  edge_filt_strength;         /*  Range: [0, 63] */
    td_u8  edge_filt_max_cap;     /*  Range: [0, 47] */
    td_u8  r_gain;                /*  Range: [0, 31] */
    td_u8  g_gain;                /*  Range: [0, 255] */
    td_u8  b_gain;                /*  Range: [0, 31] */
    td_u8  skin_gain;             /*  Range: [0, 31] */
    td_u16 max_sharp_gain;        /*  Range: [0, 0x7FF]; Format:8.3 */
} ext_isp_cmos_sharpen_manual;

typedef struct {
    td_u8  luma_wgt[ISP_SHARPEN_LUMA_NUM][ISP_AUTO_ISO_STRENGTH_NUM];     /*  Range: [0, 127] */
    td_u16 texture_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];  /*  Range: [0, 4095]; Format:7.5 */
    td_u16 edge_strength[ISP_SHARPEN_GAIN_NUM][ISP_AUTO_ISO_STRENGTH_NUM];     /*  Range: [0, 4095]; Format:7.5 */
    td_u16 texture_freq[ISP_AUTO_ISO_STRENGTH_NUM];         /*  Range: [0, 4095]; Format:6.6 */
    td_u16 edge_freq[ISP_AUTO_ISO_STRENGTH_NUM];            /*  Range: [0, 4095]; Format:6.6 */
    td_u8  over_shoot[ISP_AUTO_ISO_STRENGTH_NUM];           /*  Range: [0, 127] */
    td_u8  under_shoot[ISP_AUTO_ISO_STRENGTH_NUM];          /*  Range: [0, 127] */
    td_u8  shoot_sup_strength[ISP_AUTO_ISO_STRENGTH_NUM];        /*  Range: [0, 255] */
    td_u8  shoot_sup_adj[ISP_AUTO_ISO_STRENGTH_NUM];        /*  Range: [0, 15] */
    td_u8  detail_ctrl[ISP_AUTO_ISO_STRENGTH_NUM];          /*  Range: [0, 255] */
    td_u8  detail_ctrl_threshold[ISP_AUTO_ISO_STRENGTH_NUM];      /*  Range: [0, 255] */
    td_u8  edge_filt_strength[ISP_AUTO_ISO_STRENGTH_NUM];        /*  Range: [0, 63] */
    td_u8  edge_filt_max_cap[ISP_AUTO_ISO_STRENGTH_NUM];    /*  Range: [0, 47] */
    td_u8  r_gain[ISP_AUTO_ISO_STRENGTH_NUM];               /*  Range: [0, 31] */
    td_u8  g_gain[ISP_AUTO_ISO_STRENGTH_NUM];               /*  Range: [0, 255] */
    td_u8  b_gain[ISP_AUTO_ISO_STRENGTH_NUM];               /*  Range: [0, 31] */
    td_u8  skin_gain[ISP_AUTO_ISO_STRENGTH_NUM];            /*  Range: [0, 31] */
    td_u16 max_sharp_gain[ISP_AUTO_ISO_STRENGTH_NUM];       /*  Range: [0, 0x7FF]; Format:8.3 */
} ext_isp_cmos_sharpen_auto;

typedef struct {
    td_u8 skin_umin;
    td_u8 skin_vmin;
    td_u8 skin_umax;
    td_u8 skin_vmax;
    ext_isp_cmos_sharpen_manual manual_attr;
    ext_isp_cmos_sharpen_auto   auto_attr;
} ext_isp_cmos_sharpen;

typedef struct {
    td_bool enable;            /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Edge Mark */
    td_u8   threshold;         /* RW; range: [0, 255];  Format:8.0; */
    td_u32  color;             /* RW; range: [0, 0xFFFFFF];  Format:32.0; */
} ext_isp_cmos_edgemark;

typedef struct {
    td_bool enable;         /* RW; Range:[0, 1]; Format:1.0;Enable/Disable HLC module */
    td_u8   luma_thr;       /* RW; range: [0, 255];  Format:8.0; */
    td_u8   luma_target;    /* RW; range: [0, 255];  Format:8.0; */
} ext_isp_cmos_hlc;

typedef struct {
    td_bool enable;
    ext_isp_op_type op_type;

    td_u16  manual_strength;
    td_u16  auto_strength;

    td_u8   spatial_filter_coef;
    td_u8   range_filter_coef;
    td_u8   contrast_ctrl;
    td_s8   detail_adjust_factor;
    td_u8   range_ada_max;

    td_u8   flt_scale_fine;
    td_u8   flt_scale_coarse;
    td_u8   grad_rev_max;
    td_u8   grad_rev_thr;

    td_u8   bright_gain_limit;
    td_u8   bright_gain_limit_step;
    td_u8   dark_gain_limit_luma;
    td_u8   dark_gain_limit_chroma;

    td_u8   purple_reduction_strength;
    td_u8   local_mixing_bright_max;
    td_u8   local_mixing_bright_min;
    td_u8   local_mixing_bright_thr;
    td_s8   local_mixing_bright_slo;
    td_u8   local_mixing_dark_max;
    td_u8   local_mixing_dark_min;
    td_u8   local_mixing_dark_thr;
    td_s8   local_mixing_dark_slo;
    td_u16  color_correction_lut[EXT_ISP_DRC_CC_NODE_NUM];
    td_u16  tone_mapping_value[EXT_ISP_DRC_TM_NODE_NUM];

    td_u8   asymmetry;
    td_u8   second_pole;
    td_u8   stretch;
    td_u8   compress;

    td_u8   curve_sel;

    td_u16 x_point[EXT_ISP_DRC_CUBIC_POINT_NUM];
    td_u16 y_point[EXT_ISP_DRC_CUBIC_POINT_NUM];
    td_u16 slope[EXT_ISP_DRC_CUBIC_POINT_NUM];
} ext_isp_cmos_drc;

typedef struct {
    td_bool  fusion_mode;
    td_bool  motion_comp;

    td_u16   short_thr;
    td_u16   long_thr;

    td_bool  force_long;
    td_u16   force_long_low_thr;
    td_u16   force_long_hig_thr;

    td_bool  short_expo_chk;
    td_u16   short_check_thd;
    td_bool  md_ref_flicker;

    td_u8    md_thr_low_gain[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    md_thr_hig_gain[ISP_AUTO_ISO_STRENGTH_NUM];

    ext_isp_bnr_mode  bnr_mode;
    td_u16   fusion_thr[WDR_MAX_FRAME];
    td_u8    mdt_still_thd;
    td_u8    mdt_long_blend;
} ext_isp_cmos_wdr;

typedef struct {
    td_bool        enable;           /* RW;Range:[0,1];Format:1.0; */
    td_bool        user_lut_enable;    /* RW;Range:[0,1];0:Auto Lut 1:User Lut */
    ext_isp_op_type op_type;
    td_u8          auto_strength;
    td_u8          manual_strength;
} ext_isp_cmos_dehaze;

typedef struct {
    td_bool enable;
    td_u32  pregamma_lut[PREGAMMA_NODE_NUM];
} ext_isp_cmos_pregamma;

#define GAMMA_NODE_NUMBER      257   /* Update NODE NUMBER */
#define GAMMA_EXT_NODE_NUMBER  1025  /* Update NODE NUMBER */
#define GAMMA_OUTSEG_NUMBER    8     /* Update NODE NUMBER */
typedef struct {
    td_u16  gamma_lut[GAMMA_EXT_NODE_NUMBER];
} ext_isp_cmos_gamma;

typedef struct {
    td_u32  max_width;
    td_u32  max_height;
} ext_isp_cmos_sensor_max_resolution;

typedef struct {
    td_u16  strength[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u16  blend_ratio[ISP_AUTO_ISO_STRENGTH_NUM];
} ext_isp_cmos_dpc;

typedef struct {
    td_u16 r_gain[EXT_ISP_LSC_GRID_POINTS];
    td_u16 gr_gain[EXT_ISP_LSC_GRID_POINTS];
    td_u16 gb_gain[EXT_ISP_LSC_GRID_POINTS];
    td_u16 b_gain[EXT_ISP_LSC_GRID_POINTS];
} ext_isp_lsc_cabli_table;

typedef struct {
    td_u16 r_gain[EXT_ISP_RLSC_POINTS];
    td_u16 gr_gain[EXT_ISP_RLSC_POINTS];
    td_u16 gb_gain[EXT_ISP_RLSC_POINTS];
    td_u16 b_gain[EXT_ISP_RLSC_POINTS];
} ext_isp_bnr_lsc_calib_table;

typedef struct {
    td_u16  mesh_strength;
    td_u8   mesh_scale;
    ext_isp_lsc_cabli_table lsc_calib_table[EXT_ISP_MLSC_GAIN_LUT_NUM];
    ext_isp_bnr_lsc_calib_table bnr_lsc_calib_table;
} ext_isp_cmos_lsc;

typedef struct {
    td_bool   enable;
    td_u16    y_ratio_lut[EXT_ISP_CA_YRATIO_LUT_LENGTH];  /* 1.10bit  Y Ratio For UV; Max = 2047 FW Limit */
    td_s16    iso_ratio[ISP_AUTO_ISO_STRENGTH_NUM];     /* 1.10bit  ISO Ratio  For UV,Max = 2047 FW Limi */
} ext_isp_cmos_ca;

typedef struct {
    td_bool enable;
    td_u32  gain_r;
    td_u32  gain_g;
    td_u32  gain_b;
    ext_isp_clut_lut clut_lut;
} ext_isp_cmos_clut;

typedef struct {
    td_u16 x;                    /* RW;Range:[0x0,0x100];Format:8.0;The X point of the knee */
    td_u32 y;                    /* RW;Range:[0x0,0x100000];Format:21.0;The Y point of the  knee */
} ext_isp_cmos_expander_point;

#define EXPANDER_NODE_NUM                257
#define ISP_EXPANDER_POINT_NUM_MAX (EXPANDER_NODE_NUM - 1)
typedef struct {
    td_bool enable;
    td_u8   bit_depth_in;            /* RW;Range:[0xC,0x14];Format:5.0;The Bit depth of input */
    td_u8   bit_depth_out;           /* RW;Range:[0xC,0x14];Format:5.0;The Bit depth of output */
    td_u16  expander_point_num;      /* Range:[1, 256] */
    ext_isp_cmos_expander_point expander_point[ISP_EXPANDER_POINT_NUM_MAX];
} ext_isp_cmos_expander;

typedef struct {
    td_bool enable;                                 /* RW,Range: [   0, 1]      */
    td_u8   slope;                                  /* RW,Range: [   0, 0xE]    */
    td_u8   sensi_slope;                             /* RW,Range: [   0, 0xE]    */
    td_u16  sensi_thr;                              /* RW,Range: [   0, 0x3FFF] */
    td_u16  threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW,Range: [   0, 0x3FFF] */
    td_u16  strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW,Range: [   0, 0x100]  */
    td_u16  np_offset[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW,Range: [0x200, 0x3FFF] */
} ext_isp_cmos_ge;

typedef struct {
    td_bool enable;                               /* RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable */
    td_u8   threshold[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    td_u8   strength[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ext_isp_cmos_afc;

typedef struct {
    td_bool  enable;
    td_u8    gauss_lpf_sigma;
    td_u8    he_pos_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    he_pos_sigma[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    he_pos_mean[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    he_neg_wgt[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    he_neg_sigma[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u8    he_neg_mean[ISP_AUTO_ISO_STRENGTH_NUM];
    td_u16   blc_ctrl[ISP_AUTO_ISO_STRENGTH_NUM];
} ext_isp_cmos_ldci;

typedef struct {
    td_u8  de_purple_cr_str;
    td_u8  de_purple_cb_str;
} ext_isp_cmos_lcac_manual;

typedef struct {
    td_u8  de_purple_cr_str[ISP_EXP_RATIO_STRENGTH_NUM];
    td_u8  de_purple_cb_str[ISP_EXP_RATIO_STRENGTH_NUM];
} ext_isp_cmos_lcac_auto;

typedef struct {
    td_bool        enable;
    ext_isp_op_type op_mode;
    td_u16         var_thr;
    td_u16         r_luma_thd[ISP_LCAC_DET_NUM];
    td_u16         g_luma_thd[ISP_LCAC_DET_NUM];
    td_u16         b_luma_thd[ISP_LCAC_DET_NUM];
    td_u16         y_luma_thd[ISP_LCAC_DET_NUM];
    td_s16         cbcr_ratio[ISP_LCAC_DET_NUM];
    td_u16         purple_det_range;
    ext_isp_cmos_lcac_manual  manual_attr;
    ext_isp_cmos_lcac_auto    auto_attr;
} ext_isp_cmos_lcac;

typedef struct {
    td_u32  sensor_id;
    td_u8   sensor_mode;
} ext_isp_cmos_sensor_mode;

typedef struct {
    td_u32   exp_ratio[EXP_RATIO_NUM];
} ext_isp_cmos_wdr_switch_attr;

typedef struct {
    td_u16 g_param_high_ct[EXT_ISP_LSC_GRID_POINTS];
    td_u16 g_param_low_ct[EXT_ISP_LSC_GRID_POINTS];
} ext_isp_acs_y_shading_lut;

typedef struct {
    td_float avg_rg_map[EXT_ISP_LSC_GRID_POINTS];
    td_float avg_bg_map[EXT_ISP_LSC_GRID_POINTS];
    td_float prof_rg_map[EXT_ISP_LSC_GRID_POINTS];
    td_float prof_bg_map[EXT_ISP_LSC_GRID_POINTS];
} ext_isp_acs_color_shading_lut;

typedef struct {
    td_s16   light_index[EXT_ISP_ACS_LIGHT_NUM * EXT_ISP_ACS_CHN_NUM];
    td_float model_ar_min;
    td_float model_ar_step;
    td_float model_ab_min;
    td_float model_ab_step;
    td_s16   light_type_g_high;
    td_s16   light_type_g_low;
} ext_isp_acs_calib_param;

typedef struct {
    td_bool                       enable;
    td_u16                        acs_shading_str;
    ext_isp_acs_calib_param        acs_calib_param;
    ext_isp_acs_y_shading_lut      acs_y_shading_lut;
    ext_isp_acs_color_shading_lut  acs_color_shading_lut;
} ext_isp_cmos_acs;

#define     LSC_ISO_PARAM_LEN                3
typedef struct {
    td_bool mesh_dgain_en;
    td_u32 isp_dgain_th;
    td_u32 iso[LSC_ISO_PARAM_LEN];
    td_u16 mesh_str[LSC_ISO_PARAM_LEN];
    td_u16 corner_rg_th[LSC_ISO_PARAM_LEN];
    td_u16 corner_bg_th[LSC_ISO_PARAM_LEN];
} ext_isp_lsc_adapt_param;

typedef union {
    td_u64  key;
    struct {
        td_u64  bit1_drc              : 1;   /* [0] */
        td_u64  bit1_demosaic         : 1;   /* [1] */
        td_u64  bit1_pregamma         : 1;   /* [2] */
        td_u64  bit1_gamma            : 1;   /* [3] */
        td_u64  bit1_sharpen          : 1;   /* [4] */
        td_u64  bit1_edge_mark        : 1;   /* [5] */
        td_u64  bit1_hlc              : 1;   /* [6] */
        td_u64  bit1_ldci             : 1;   /* [7] */
        td_u64  bit1_dpc              : 1;   /* [8] */
        td_u64  bit1_lsc              : 1;   /* [9] */
        td_u64  bit1_ge               : 1;   /* [10] */
        td_u64  bit1_anti_false_color : 1;   /* [11] */
        td_u64  bit1_bayer_nr         : 1;   /* [12] */
        td_u64  bit1_detail           : 1;   /* [13] */
        td_u64  bit1_ca               : 1;   /* [14] */
        td_u64  bit1_expander         : 1;   /* [15] */
        td_u64  bit1_clut             : 1;   /* [16] */
        td_u64  bit1_wdr              : 1;   /* [17] */
        td_u64  bit1_dehaze           : 1;   /* [18] */
        td_u64  bit1_lcac             : 1;   /* [19] */
        td_u64  bit1_acs              : 1;   /* [20] */
        td_u64  bit1_cac              : 1;   /* [21] */
        td_u64  bit1_gdci             : 1;   /* [22] */
        td_u64  bit1_lsc_adp          : 1;   /* [23] */
        td_u64  bit41_rsv             : 40;  /* [24:63] */
    };
} ext_isp_cmos_alg_key;

typedef struct {
    ext_isp_cmos_alg_key              key;
    const ext_isp_cmos_drc             *drc;
    const ext_isp_cmos_demosaic        *demosaic;
    const ext_isp_cmos_pregamma        *pregamma;
    const ext_isp_cmos_gamma           *gamma;
    const ext_isp_cmos_sharpen         *sharpen;
    const ext_isp_cmos_edgemark        *edge_mark;
    const ext_isp_cmos_hlc             *hlc;
    const ext_isp_cmos_ldci            *ldci;
    const ext_isp_cmos_dpc             *dpc;
    const ext_isp_cmos_lsc             *lsc;
    const ext_isp_cmos_ge              *ge;
    const ext_isp_cmos_afc             *anti_false_color;
    const ext_isp_cmos_bayernr         *bayer_nr;
    const ext_isp_cmos_detail          *detail;
    const ext_isp_cmos_ca              *ca;
    const ext_isp_cmos_expander        *expander;
    const ext_isp_cmos_clut            *clut;
    const ext_isp_cmos_wdr             *wdr;
    const ext_isp_cmos_dehaze          *dehaze;
    const ext_isp_cmos_lcac            *lcac;
    const ext_isp_cmos_acs             *acs;
    const ext_isp_gdci_attr            *gdci;
    const ext_isp_lsc_adapt_param      *lsc_adapt_param;
    ext_isp_cmos_noise_calibration     noise_calibration;
    ext_isp_cmos_sensor_max_resolution sensor_max_resolution;
    ext_isp_cmos_sensor_mode           sensor_mode;
    ext_isp_cmos_wdr_switch_attr       wdr_switch_attr;
} ext_isp_cmos_default;

typedef struct {
    td_u16   width;
    td_u16   height;
    td_float fps;
    td_u8    sns_mode;
} ext_isp_cmos_sensor_image_mode;

/* sensor registers to ISP lib */
typedef struct {
    td_void (*pfn_cmos_sensor_init)(td_s32 pipe_id);
    td_void (*pfn_cmos_sensor_exit)(td_s32 pipe_id);
    td_void (*pfn_cmos_sensor_global_init)(td_s32 pipe_id);
    td_s32 (*pfn_cmos_set_image_mode)(td_s32 pipe_id, ext_isp_cmos_sensor_image_mode *sensor_image_mode);
    td_s32 (*pfn_cmos_get_image_mode)(td_s32 pipe_id, ext_isp_cmos_sensor_image_mode *sensor_image_mode);
    td_s32 (*pfn_cmos_set_wdr_mode)(td_s32 pipe_id, td_u8 mode);

    /* the algs get data which is associated with sensor, except 3a */
    td_s32 (*pfn_cmos_get_isp_default)(td_s32 pipe_id, ext_isp_cmos_default *def);
    td_s32 (*pfn_cmos_get_isp_black_level)(td_s32 pipe_id, ext_isp_cmos_black_level *black_level);
    td_s32 (*pfn_cmos_get_sns_reg_info)(td_s32 pipe_id, ext_isp_sns_regs_info *sns_regs_info);

    /* the function of sensor set pixel detect */
    td_void (*pfn_cmos_set_pixel_detect)(td_s32 pipe_id, td_bool enable);
    td_s32 (*pfn_cmos_get_awb_gains)(td_s32 pipe_id, td_u32 *sensor_awb_gain);

    /* the function of sensor vcm cfg */
    td_s32 (*pfn_cmos_set_vcm_cfg)(td_s32 pipe_id, td_u32 msb, td_u32 lsb);
} ext_isp_sensor_exp_func;

/*********************************************************************************************************/
/* sensor's interface to ae
 * eg: 0.35db, accu_type=AE_ACCURACY_DB, accuracy=0.35
 *  and the multiply of 0.35db is power(10, (0.35/20))
 *  eg: 1/16, 2/16, 3/16 multiplies, accu_type=AE_ACCURACY_LINEAR, accuracy=0.0625
 *  eg: 1,2,4,8,16 multiplies, accu_type=AE_ACCURACY_DB, accuracy=6
 */
typedef enum {
    AE_ACCURACY_DB = 0,
    AE_ACCURACY_LINEAR,
    AE_ACCURACY_TABLE,

    AE_ACCURACY_BUTT,
} ext_isp_ae_accuracy_type;

typedef struct {
    ext_isp_ae_accuracy_type accu_type;
    float   accuracy;
    float   offset;
} ext_isp_ae_accuracy;

typedef struct {
    td_bool step_fno_table_change;    /* W; Range:[0, 1]; Format:1.0; Step-F number mapping table change or not */
    td_bool zero_is_max;              /* RW; Range:[0, 1]; Format:1.0; Step 0 corresponds to max aperture or not,
                                         it's related to the specific iris */
    td_u16  total_step;               /* RW; Range:[1, 1024]; Format:16.0; Total steps of  Piris's aperture,
                                         it's related to the specific iris */
    td_u16  step_count;               /* RW; Range:[1, 1024]; Format:16.0; Used steps of Piris's aperture.
                                         when Piris's aperture is too small, the F number precision is not enough,
                                         so not all the steps can be used. It's related to the specific iris */
    td_u16 step_fno_table[AI_MAX_STEP_FNO_NUM];  /* RW; Range:[1, 1024]; Format:16.0; Step-F number mapping table.
                                                    F1.0 is expressed as 1024, F32 is expressed as 1,
                                                    it's related to the specific iris */
    ext_isp_iris_f_no  max_iris_fno_target;  /* RW; Range:[0, 10]; Format:4.0;
                                               Max F number of Piris's aperture, it's related to the specific iris */
    ext_isp_iris_f_no  min_iris_fno_target;  /* RW; Range:[0, 10]; Format:4.0;
                                               Min F number of Piris's aperture, it's related to the specific iris */

    td_bool fno_ex_valid;           /* RW; Range:[0, 1]; Format:1.0; use equivalent gain to present FNO or not */
    td_u32  max_iris_fno_target_linear;  /* RW; Range:[1, 1024]; Format:16.0;
                                            Max equivalent gain of F number of Piris's aperture,
                                            only used when fno_ex_valid is true, it's related to the specific iris */
    td_u32  min_iris_fno_target_linear;  /* RW; Range:[1, 1024]; Format:16.0;
                                            Min equivalent gain of F number of Piris's aperture,
                                            only used when fno_ex_valid is true, it's related to the specific iris */
} ext_isp_piris_attr;

typedef struct {
    td_bool quick_start_enable;
    td_u8 black_frame_num;
    td_bool ir_mode_en;
    td_u32 init_exposure_ir;
    td_u32 iso_thr_ir;
    td_u16 ir_cut_delay_time;
} ext_isp_ae_quick_start_param;

typedef struct {
    td_u8   hist_thresh[HIST_THRESH_NUM];
    td_u8   ae_compensation;

    td_u32  lines_per500ms;
    td_u32  flicker_freq;
    td_float fps;
    td_u32  hmax_times; /* unit is ns */
    td_u32  init_exposure;
    td_u32  init_int_time;
    td_u32  init_again;
    td_u32  init_dgain;
    td_u32  init_isp_dgain;
    td_u32  init_ae_speed;
    td_u32  init_ae_tolerance;

    td_u32  full_lines_std;
    td_u32  full_lines_max;
    td_u32  full_lines;
    td_u32  binning_full_lines;
    td_u32  max_int_time;     /* RW;unit is line */
    td_u32  min_int_time;
    td_u32  max_int_time_target;
    td_u32  min_int_time_target;
    ext_isp_ae_accuracy int_time_accu;

    td_u32  max_again;
    td_u32  min_again;
    td_u32  max_again_target;
    td_u32  min_again_target;
    ext_isp_ae_accuracy again_accu;

    td_u32  max_dgain;
    td_u32  min_dgain;
    td_u32  max_dgain_target;
    td_u32  min_dgain_target;
    ext_isp_ae_accuracy dgain_accu;

    td_u32  max_isp_dgain_target;
    td_u32  min_isp_dgain_target;
    td_u32  isp_dgain_shift;

    td_u32  max_int_time_step;
    td_bool max_time_step_enable;
    td_u32  max_inc_time_step[ISP_WDR_FRAME_MAX];
    td_u32  max_dec_time_step[ISP_WDR_FRAME_MAX];
    td_u32  lf_max_short_time;
    td_u32  lf_min_exposure;

    ext_isp_ae_route ae_route_attr;
    td_bool ae_route_ex_valid;
    ext_isp_ae_route_ex ae_route_attr_ex;

    ext_isp_ae_route ae_route_sf_attr;
    ext_isp_ae_route_ex ae_route_sf_attr_ex;

    td_u16 man_ratio_enable;
    td_u32 arr_ratio[EXP_RATIO_NUM];

    ext_isp_iris_type  iris_type;
    ext_isp_piris_attr piris_attr;
    ext_isp_iris_f_no  max_iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                        Max F number of Piris's aperture, it's related to the specific iris */
    ext_isp_iris_f_no  min_iris_fno;  /* RW; Range:[0, 10]; Format:4.0;
                                        Min F number of Piris's aperture, it's related to the specific iris */

    ext_isp_ae_strategy ae_exp_mode;

    td_u16 iso_cal_coef;
    td_u8  ae_run_interval;
    td_u32 exp_ratio_max;
    td_u32 exp_ratio_min;
    td_bool diff_gain_support;
    ext_isp_ae_quick_start_param quick_start;
    ext_isp_prior_frame prior_frame;
    td_bool ae_gain_sep_cfg;
    td_bool lhcg_support;
    td_u32 sns_lhcg_exp_ratio;

    td_u8 ae_stat_pos;          /* RW; Range:[0, 1]; Format:1.0 */
} ext_isp_ae_sensor_default;

typedef struct {
    ext_isp_fswdr_mode fswdr_mode;
} ext_isp_ae_fswdr_attr;

typedef struct {
    td_u32 reg_addr;
    td_u32 reg_value;
} ext_isp_ae_param_reg;

typedef struct {
    td_u32 tar_fps;
    td_u32 exp_time;
    td_u32 exp_again;
    td_u32 exp_dgain;
    td_u32 exp_isp_dgain;
    ext_isp_ae_param_reg time_reg[10]; /* 10 */
    ext_isp_ae_param_reg again_reg[10]; /* 10 */
    ext_isp_ae_param_reg dgain_reg[10]; /* 10 */
} ext_isp_ae_convert_param;

typedef struct {
    td_u32 int_time_min[ISP_WDR_FRAME_MAX];
    td_u32 int_time_max[ISP_WDR_FRAME_MAX];
    td_u32 pre_int_time[ISP_WDR_FRAME_MAX];
} ext_isp_ae_int_time_range;

typedef struct {
    td_bool is_ir_mode;
    td_u32 exp_time;
    td_u32 a_gain;
    td_u32 d_gain;
    td_u32 ispd_gain;
    td_u32 exposure;
    td_u32 init_iso;
    td_u32 lines_per500ms;
    td_u32 piris_fno;
    td_u16 wb_r_gain;
    td_u16 wb_g_gain;
    td_u16 wb_b_gain;
    td_u16 sample_r_gain;
    td_u16 sample_b_gain;
    td_u16 init_ccm[EXT_ISP_CCM_MATRIX_SIZE];
    td_bool ae_route_ex_valid;
    td_bool quick_start_en;
    ext_isp_ae_route ae_route;
    ext_isp_ae_route_ex ae_route_ex;
    ext_isp_ae_route ae_route_sf;
    ext_isp_ae_route_ex ae_route_sf_ex;
} ext_isp_init_attr;

#define ISP_SNS_SAVE_INFO_MAX 2
typedef struct {
    td_bool           is_init;                /* TD_TRUE: Sensor init */
    td_bool           is_sync_init;           /* TD_TRUE: Sync Reg init */
    td_u8    img_mode;
    ext_isp_wdr_mode  wdr_mode;

    ext_isp_sns_regs_info regs_info[ISP_SNS_SAVE_INFO_MAX]; /* [0]: Sensor reg info of cur-frame;
                                                               [1]: Sensor reg info of pre-frame */

    td_u32      full_lines[ISP_SNS_SAVE_INFO_MAX];     /* [0]: FullLines of cur-frame;
                                                          [1]: Pre FullLines of pre-frame */
    td_u32      full_lines_std;                        /* FullLines std */
    td_u32      wdr_int_time[ISP_WDR_FRAME_MAX];
    td_u32      sns_wb_gain[ISP_BAYER_CHN_NUM];
} isp_sns_state;

/* sensor registers to AE lib */
typedef struct {
    td_s32 (*pfn_cmos_get_ae_default)(td_s32 pipe_id, ext_isp_ae_sensor_default *ae_sns_dft);

    /* the function of sensor set fps */
    td_void (*pfn_cmos_fps_set)(td_s32 pipe_id, td_float f32_fps, ext_isp_ae_sensor_default *ae_sns_dft);
    td_void (*pfn_cmos_slow_framerate_set)(td_s32 pipe_id, td_u32 full_lines,
        ext_isp_ae_sensor_default *ae_sns_dft);

    /* while isp notify ae to update sensor regs, ae call these funcs. */
    td_void (*pfn_cmos_inttime_update)(td_s32 pipe_id, td_u32 int_time);
    td_void (*pfn_cmos_gains_update)(td_s32 pipe_id, td_u32 again, td_u32 dgain);

    td_void (*pfn_cmos_again_calc_table)(td_s32 pipe_id, td_u32 *again_lin, td_u32 *again_db);
    td_void (*pfn_cmos_dgain_calc_table)(td_s32 pipe_id, td_u32 *dgain_lin, td_u32 *dgain_db);

    td_void (*pfn_cmos_get_inttime_max)(td_s32 pipe_id,
        td_u32 *ratio, ext_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time);

    /* long frame mode set */
    td_void (*pfn_cmos_ae_fswdr_attr_set)(td_s32 pipe_id, ext_isp_ae_fswdr_attr *ae_fswdr_attr);
    td_void (*pfn_cmos_ae_quick_start_status_set)(td_s32 pipe_id, td_bool quick_start_status);
    td_void (*pfn_cmos_exp_param_convert)(td_s32 pipe_id, ext_isp_ae_convert_param *exp_param);
} ext_isp_ae_sensor_exp_func;

/*********************************************************************************************************/
/* sensor's interface to awb */
typedef struct {
    td_u16 color_temp;              /* RW;  range:[2000,10000]; format:16.0; the current color temperature */
    td_u16 ccm[EXT_ISP_CCM_MATRIX_SIZE];    /* RW;  range: [0x0, 0xFFFF]; format:8.8;
                                       CCM matrixes for different color temperature */
} ext_isp_awb_ccm_tab;

typedef struct {
    td_u16  ccm_tab_num;                   /* RW;  range: [0x3, 0x7]; format:16.0; the number of CCM matrixes */
    ext_isp_awb_ccm_tab ccm_tab[EXT_ISP_CCM_MATRIX_NUM];
} ext_isp_awb_ccm;

typedef struct {
    td_bool valid;

    td_u8   saturation[ISP_AUTO_ISO_STRENGTH_NUM]; /* adjust saturation, different iso with different saturation */
} ext_isp_awb_agc_table;

typedef struct {
    td_u16 hue_shift[EXT_ISP_COLOR_SECTOR_NUM]; /* RW; Range: [0x0, 0xFFFF]; Format:8.8;CCM */
    td_u16 sat_shift[EXT_ISP_COLOR_SECTOR_NUM];
} ext_isp_color_sector_tab; /* Not support for 850 */

typedef struct {
    td_bool valid;
    ext_isp_color_sector_tab color_tab[EXT_ISP_CCM_MATRIX_NUM];
} ext_isp_color_sector; /* Not support for 850 */

typedef struct {
    td_u16  wb_ref_temp;       /* RW;reference color temperature for WB  */
    td_u16  gain_offset[ISP_BAYER_CHN_NUM];  /* RW; gain offset for white balance */
    td_s32  wb_para[AWB_CURVE_PARA_NUM];      /* RW; parameter for wb curve,p1,p2,q1,a1,b1,c1 */

    td_u16  golden_rgain;      /* rgain for the golden sample */
    td_u16  golden_bgain;      /* bgain for the golden sample */
    td_u16  sample_rgain;      /* rgain for the current sample */
    td_u16  sample_bgain;      /* bgain for the current sample */
    ext_isp_awb_agc_table agc_tbl;
    ext_isp_awb_ccm ccm;
    ext_isp_color_sector sector; /* Not support for 850 */
    td_u16    init_rgain;           /* init WB gain */
    td_u16    init_ggain;
    td_u16    init_bgain;
    td_u8     awb_run_interval;       /* RW;AWB run interval */
    td_u16    init_ccm[EXT_ISP_CCM_MATRIX_SIZE];
} ext_isp_awb_sensor_default;

typedef struct {
    td_s32 (*pfn_cmos_get_awb_default)(td_s32 pipe_id, ext_isp_awb_sensor_default *awb_sns_dft);
} ext_isp_awb_sensor_exp_func;

/******************************************************************************************************/

typedef struct {
    ext_isp_sensor_exp_func sns_isp_exp;
    ext_isp_ae_sensor_exp_func sns_ae_exp;
    ext_isp_awb_sensor_exp_func sns_awb_exp;
} ext_isp_sensor_register;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
