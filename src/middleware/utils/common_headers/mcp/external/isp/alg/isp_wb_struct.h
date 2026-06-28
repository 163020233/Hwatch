/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_wb_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_WB_STRUCT_H__
#define __ISP_WB_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* wb *******************************************/

typedef struct {
    td_bool enable; /* RW; Range: [0,1];  Format:1.0;AWB crop enable */
    td_u16 x;       /* Format:13.0;AWB image crop start x, limited range:[0, ImageWidth - zone_col * 60] */
    td_u16 y;       /* Format:13.0;AWB image crop start y,limited range:[0, ImageHeight - zone_row * 14] */
    td_u16 w;       /* AWB image crop width, limited range:[zone_col * 60, ImageWidth] */
    td_u16 h;       /*  Format:14.0;
                       AWB image crop height, limited range:[zone_row * 14, ImageHeight] */
} ext_isp_awb_crop;

typedef struct {
    ext_isp_awb_switch awb_switch;            /* RW; Range: [0x0, 0x2]; Position of AWB statistics in pipeline */
    td_u16 zone_row;                          /* RW; Range: [0x1, 0x20]; Vertical Blocks,
                                                 limited range:[1, min(32, ImageHeight /AWB_MIN_HEIGHT)] */
    td_u16 zone_col;                          /* RW; Range: [0x1, 0x20]; Horizontal Blocks,
                                                 limited range:[BlkNum, min(32, Width /AWB_MIN_WIDTH)] */
    td_u16 zone_bin;                          /* Brightness Bins */
    td_u16 hist_bin_thresh[AWB_ZONE_BIN_MAX]; /* RW; Range: [0x0, 0xFFFF]; Bin Threshold */
    td_u16 white_level;                       /* RW; Range: [0x0, 0xFFFF];Upper limit of valid data for white region,
                                                 for Bayer statistics, [0x0, 0x3FF] for RGB statistics */
    td_u16 black_level;       /* RW; Range: [0x0, 0xFFFF];limited range: [0x0, white_level],
                                 Lower limit of valid data for white region .
                                 for Bayer statistics, bitwidth is 12, for RGB statistics, bitwidth is 10 */
    td_u16 cb_max;                            /* RW; Range: [0x0, 0xFFF];Maximum value of B/G for white region */
    td_u16 cb_min;                            /* RW; Range: [0x0, 0xFFF];
                                                 limited range: [0x0, cb_max]Minimum value of B/G for white region */
    td_u16 cr_max;                            /* RW; Range: [0x0, 0xFFF];Maximum value of R/G for white region */
    td_u16 cr_min;                            /* RW; Range: [0x0, 0xFFF];
                                                 limited range: [0x0, cr_max],Minimum value of R/G for white region */
    ext_isp_awb_crop crop;
} ext_isp_wb_statistics_cfg;

typedef struct {
    td_u16 grid_y_pos[AWB_ZONE_ORIG_ROW + 1];    /* R */
    td_u16 grid_x_pos[AWB_ZONE_ORIG_COLUMN + 1]; /* R */
    td_u8 status;                                /* R;0:not update, 1: update,others:reserved */
} ext_isp_awb_grid_info;

typedef struct {
    td_u16 global_r;  /* R; Range: [0x0, 0xFFFF];Global WB output Average R */
    td_u16 global_g;  /* R; Range: [0x0, 0xFFFF];Global WB output Average G */
    td_u16 global_b;  /* R; Range: [0x0, 0xFFFF];Global WB output Average B */
    td_u16 count_all; /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */

    td_u16 zone_avg_r[AWB_ZONE_NUM];     /* R; Range: [0x0, 0xFFFF];Zone Average R */
    td_u16 zone_avg_g[AWB_ZONE_NUM];     /* R; Range: [0x0, 0xFFFF];Zone Average G */
    td_u16 zone_avg_b[AWB_ZONE_NUM];     /* R; Range: [0x0, 0xFFFF];Zone Average B */
    td_u16 zone_count_all[AWB_ZONE_NUM]; /* R; Range: [0x0, 0xFFFF];normalized number of Gray points */
    ext_isp_awb_grid_info grid_info;
    td_u64 pts;
} ext_isp_wb_statistics;

/*
  Defines the AWB algorithm type
  0 = Improved gray world algorithm.
  1 = AWB algorithm that classifies the statistics and re-filters the white blocks
  2 = AWB algorithm that opt outdoor performance for dv
 */
typedef enum {
    AWB_ALG_LOWCOST = 0,
    AWB_ALG_ADVANCE = 1,
    AWB_ALG_NATURA  = 2,
    AWB_ALG_BUTT
} ext_isp_awb_alg_type;

/*
  Defines the AWB policy in the multi-illuminant scenario
  0 = Automatic saturation adjustment in the multi-illuminant scenario
  1 = Automatic ccm adjustment in the multi-illuminant scenario
 */
typedef enum {
    AWB_MULTI_LS_SAT = 0,
    AWB_MULTI_LS_CCM = 1,
    AWB_MULTI_LS_BUTT
} ext_isp_awb_multi_ls_type;

typedef enum {
    AWB_INDOOR_MODE = 0,
    AWB_OUTDOOR_MODE = 1,
    AWB_INDOOR_OUTDOOR_BUTT
} ext_isp_awb_scene_mode_status;

/* Defines the AWB gain range */
typedef struct {
    td_bool enable;
    ext_isp_op_type op_type;

    td_u16 high_rg_limit; /* RW; Range:[0x0, 0xFFF]; Format:4.8;
                             in Manual Mode, user define the Max Rgain of High Color Temperature */
    td_u16 high_bg_limit; /* RW; Range:[0x0, 0xFFF]; Format:4.8;
                             in Manual Mode, user define the Min Bgain of High Color Temperature */
    td_u16 low_rg_limit;  /* RW; Range:[0x0, 0xFFF]; Format:4.8; limited range:[0x0, high_rg_limit),
                             in Manual Mode, user define the Min Rgain of Low Color Temperature */
    td_u16 low_bg_limit;  /* RW; Range:[0, 0xFFF]; Format:4.8; limited Range:(high_bg_limit, 0xFFF],
                             in Manual Mode, user define the Max Bgain of Low Color Temperature */
} ext_isp_awb_ct_limit_attr;

typedef struct {
    td_bool enable; /* RW; Range:[0, 1]; Format:1.0; Outdoor/Indoor scenario determination enable */
    ext_isp_op_type op_type;
    ext_isp_awb_scene_mode_status scene_status; /* User should config indoor or outdoor status in Manual Mode */
    td_u32 out_thresh;                          /* RW;Format:32.0;shutter time(in us) to judge indoor or outdoor */
    td_u16 low_start;                           /* RW;Format:16.0;5000K is recommend */
    td_u16 low_stop;                            /* RW;Format:16.0;limited range:(0, low_start),
                                                   4500K is recommend, should be smaller than u8LowStart */
    td_u16 high_start;                          /* RW;Format:16.0;limited range:(low_start, 0xFFFF],
                                                   6500K is recommend, shoule be larger than u8LowStart */
    td_u16 high_stop;                           /* RW;Format:16.0;limited range:(high_start, 0xFFFF],
                                                   8000K is recommend, should be larger than u8HighStart */
    td_bool green_enhance_en; /* RW; Range:[0, 1]; Format:1.0;If this is enabled, Green channel will be enhanced
                                 based on the ratio of green plant */
    td_u8 out_shift_limit;    /* RW; Range:[0, 0xFF]; Format:8.0;Max white point zone distance to Planckian Curve */
} ext_isp_awb_in_out_attr;

typedef struct {
    td_bool enable; /* RW; Range:[0, 1]; Format:1.0;
                       If enabled, statistic parameter cr, cb will change according to iso */

    td_u16 cr_max[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFFF]; cr_max[i] >= cr_min[i] */
    td_u16 cr_min[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFFF] */
    td_u16 cb_max[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFFF]; cb_max[i] >= cb_min[i] */
    td_u16 cb_min[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; Range:[0x0, 0xFFF] */
} ext_isp_awb_cbcr_track_attr;

#define AWB_LUM_HIST_NUM 6
/* Defines the parameters of the luminance histogram statistics for white balance */
typedef struct {
    td_bool enable;                      /* RW; Range:[0, 1]; Format:1.0;
                                            If enabled, zone weight to awb is combined with zone luma */
    ext_isp_op_type op_type;             /* In auto mode, the weight distribution follows Gaussian distribution */
    td_u8 hist_thresh[AWB_LUM_HIST_NUM]; /* RW; Range:[0x0, 0xFF]; Format:8.0;
                                            In manual mode, user define luma thresh, thresh[0] is 0,
                                            thresh[5] is 0xFF,  thresh[i] greater or equal to  thresh[i-1] */
    td_u16 hist_wt[AWB_LUM_HIST_NUM];    /* RW; Range:[0x0, 0xFFFF]; Format:16.0;
                                            user can define luma weight in both manual and auto mode. */
} ext_isp_awb_lum_histgram_attr;

/* Defines the information about a separate illuminant */
typedef struct {
    td_u16 white_r_gain; /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/R of White points at this light source */
    td_u16 white_b_gain; /* RW;Range:[0x0, 0xFFF]; Format:4.8; G/B of White points at this light source */
    td_u16 exp_quant;    /* RW;shutter time * again * dgain >> 4, Not support Now */
    td_u8 light_status;  /* RW;Range:[0,2]; Format:2.0; idle  1:add light source  2:delete sensitive color */
    td_u8 radius;        /* RW;Range:[0x0, 0xFF];  Format:8.0; Radius of light source, */
} ext_isp_awb_extra_light_source_info;

#define AWB_LS_NUM 4
#define AWB_MULTI_CT_NUM 8
/* extended AWB attributes */
typedef struct {
    td_u8 tolerance;      /* RW; Range:[0x0, 0xFF]; Format:8.0; AWB adjust tolerance */
    td_u8 zone_radius;    /* RW; Range:[0x0, 0xFF]; Format:8.0; radius of AWB blocks */
    td_u16 curve_l_limit; /* RW; Range:[0x0, 0x100]; Format:9.0; Left limit of AWB Curve,
                             recomend for indoor 0xE0, outdoor 0xE0 */
    td_u16 curve_r_limit; /* RW; Range:[0x100, 0xFFF]; Format:12.0; Right Limit of AWB Curve,
                             recomend for indoor 0x130, outdoor 0x120 */

    td_bool extra_light_en; /* RW; Range:[0, 1]; Format:1.0; Enable special light source function */
    ext_isp_awb_extra_light_source_info light_info[AWB_LS_NUM];
    ext_isp_awb_in_out_attr in_or_out;

    td_bool multi_light_source_en;           /* RW; Range:[0, 1]; Format:1.0; If enabled, awb will do special process
                                                in multi light source enviroment */
    ext_isp_awb_multi_ls_type multi_ls_type; /* Saturation or CCM Tunning */
    td_u16 multi_ls_scaler;                  /* RW; Range:[0x0, 0x100]; Format:12.0;
                                                In saturation type, it means the max saturation it can achieve,
                                                in ccm type, it means the strenght of multi process. */
    td_u16 multi_ct_bin[AWB_MULTI_CT_NUM];   /* RW; Range:[0, 0XFFFF]; Format:16.0;
                                                AWB Support divide the color temperature range by 8 bins */
    td_u16 multi_ct_wt[AWB_MULTI_CT_NUM];    /* RW; Range:[0x0, 0x400];Weight for different color temperature,
                                                same value of 8 means CT weight does't work, */

    td_bool fine_tun_en;     /* RW; Range:[0x0, 0x1]; Format:1.0;If enabled, skin color scene will be optimized */
    td_u8 fine_tun_strength; /* RW; Range:[0x0, 0xFF]; Format:8.0;
                                larger value means better performance of skin color scene,
                                but will increase error probability in low color temperature scene */
} ext_isp_awb_attr_ex;

typedef struct {
    td_bool enable; /* If AWB is disabled, static wb gain will be used, otherwise auto wb gain will be used */

    td_u16 ref_color_temp;                 /* Calibration Information */
    td_u16 static_wb[ISP_BAYER_CHN_NUM];   /* Calibration Information */
    td_s32 curve_para[AWB_CURVE_PARA_NUM]; /* Calibration Information */

    ext_isp_awb_alg_type alg_type;

    td_u8 rg_strength;      /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of R Channel */
    td_u8 bg_strength;      /* RW; Range: [0x0, 0xFF]; Format:8.0; AWB Strength of B Channel */
    td_u16 speed;           /* RW; Range: [0x0, 0xFFF]; Format:12.0; Convergence speed of AWB  */
    td_u16 zone_sel;        /* RW; Range: [0, 255]; Format:8.0; A value of 0 or 0xFF means global AWB,
                               A value between 0 and 0xFF means zoned AWB */
    td_u16 high_color_temp; /* RW; Range: [0, 65535]; Format:16.0; AWB max temperature, Recommended: [8500, 10000] */
    td_u16 low_color_temp;  /* RW; Range: [0, 65535]; Format:16.0; Limited Range:[0, high_color_temp),
                               AWB min temperature, Recommended: [2000, 2500] */
    ext_isp_awb_ct_limit_attr ct_limit;
    td_bool shift_limit_en;  /* If enable, when the statistic information is out of range, it should be project back */
    td_u8 shift_limit;       /* Rplanckian curve range, Recommended: [0x30, 0x50] */
    td_bool gain_norm_en;    /* if enabled, the min of RGB gain is fixed. */
    td_bool natural_cast_en; /* if enabled, the color performance will be natural in
                                lowlight and low color temperature */

    ext_isp_awb_cbcr_track_attr cb_cr_track;
    ext_isp_awb_lum_histgram_attr luma_hist;
    td_bool awb_zone_wt_en;                                      /* if enabled, user can set weight for each zones */
    td_u8 zone_wt[(AWB_ZONE_ORIG_ROW) * (AWB_ZONE_ORIG_COLUMN)]; /* Zone Wt Table */
} ext_isp_awb_attr;

typedef struct {
    td_u16 r_gain;  /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for R  color channel  */
    td_u16 gr_gain; /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gr color channel */
    td_u16 gb_gain; /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for Gb color channel */
    td_u16 b_gain;  /* RW; Range: [0x0, 0xFFF]; Format:4.8; Multiplier for B  color channel */
} ext_isp_mwb_attr;

typedef enum {
    ALG_AWB         = 0,
    ALG_BUTT
} ext_isp_awb_alg;

typedef struct {
    td_bool bypass;         /* RW; Range: [0, 1];  Format:1.0; If enabled, awb will be bypassed */
    td_u8 awb_run_interval; /* RW; Range: [0x1, 0xFF];  Format:8.0; set the AWB run interval */
    ext_isp_op_type op_type;
    ext_isp_mwb_attr manual_attr;
    ext_isp_awb_attr auto_attr;
    ext_isp_awb_alg alg_type;
} ext_isp_wb_attr;

typedef struct {
    td_u16 r_gain;                       /* AWB result of R color channel */
    td_u16 gr_gain;                      /* AWB result of Gr color channel */
    td_u16 gb_gain;                      /* AWB result of Gb color channel */
    td_u16 b_gain;                       /* AWB result of B color channel */
    td_u16 saturation;                   /* Current saturation */
    td_u16 color_temp;                   /* Detect color temperature, maybe out of color
                                            cemeprature range */
    td_u16 ccm[EXT_ISP_CCM_MATRIX_SIZE]; /* Current color correction matrix */

    td_u16 ls0_ct;      /* color tempearture of primary light source */
    td_u16 ls1_ct;      /* color tempearture of secondary light source */
    td_u16 ls0_area;    /* area of primary light source */
    td_u16 ls1_area;    /* area of secondary light source */
    td_u8 multi_degree; /* 0 means uniform light source, larger value means multi light source */
    td_u16 active_shift;
    td_u32 first_stable_time;                   /* AWB first stable frame number */
    ext_isp_awb_scene_mode_status scene_status; /* indoor or outdoor status */
    td_s16 bv;                                  /* Bv value */
} ext_isp_wb_info;

typedef struct {
    td_u8 saturation; /* RW; Range: [0, 0xFF]; Format:8.0; set the manual saturation of CCM */
} ext_isp_saturation_manual;

typedef struct {
    td_u8 sat[ISP_AUTO_ISO_STRENGTH_NUM]; /* should be decreased with increasing ISO */
} ext_isp_saturation_auto;

typedef struct {
    ext_isp_op_type op_type;
    ext_isp_saturation_manual manual_attr;
    ext_isp_saturation_auto auto_attr;
} ext_isp_saturation_attr;

/* calibration */
typedef struct {
    td_u16 avg_r_gain;
    td_u16 avg_b_gain;
} ext_isp_awb_calibration_gain;

/* CCM */
typedef struct {
    td_u16 red_cast_gain;   /* R channel gain after CCM */
    td_u16 green_cast_gain; /* G channel gain after CCM */
    td_u16 blue_cast_gain;  /* B channel gain after CCM */
} ext_isp_color_tone_attr;

typedef struct {
    td_bool sat_en;                      /* RW; Range: [0, 1]; Format:1.0;
                                            If sat_en=1, the active CCM = SatMatrix * ManualMatrix,
                                            else tha active CCM =  ManualMatrix */
    td_u16 ccm[EXT_ISP_CCM_MATRIX_SIZE]; /* RW; Range: [0x0, 0xFFFF]; Format:8.8; Manul CCM matrix, */
} ext_isp_color_matrix_manual;

typedef struct {
    td_u16 color_temp;                   /* the current color temperature */
    td_u16 ccm[EXT_ISP_CCM_MATRIX_SIZE]; /* CCM matrixes for different color temperature */
} ext_isp_color_matrix_param;

typedef struct {
    td_bool iso_act_en;  /* if enabled, CCM will bypass in low light */
    td_bool temp_act_en; /* if enabled, CCM will bypass when color temperature is
                            larger than 8000K or less than 2500K */
    td_u16 ccm_tab_num;  /* The number of CCM matrixes */
    ext_isp_color_matrix_param ccm_tab[EXT_ISP_CCM_MATRIX_NUM];
} ext_isp_color_matrix_auto;

typedef struct {
    ext_isp_op_type op_type;
    ext_isp_color_matrix_manual manual_attr;
    ext_isp_color_matrix_auto auto_attr;
} ext_isp_color_matrix_attr;

typedef struct {
    td_bool ccm_lutb_en; /* RW; Range: [0, 1]; Format:1.0; If ccm_lytb_en=1, active the ccm built-in dgain function */
    td_u8 ccm_lutb_type; /* RW; Range: [0x0, 0x2]; 0: original mode, neither dgain nor gamma valid; 1:lutb_only, dgain
                            valid; 2: lutc_only, gamma only */
    td_u8 luma_coefr;    /* RW; Range: [0x0, 0x1F], the weight of Cr when calculate Y, refer to the gain table */
    td_u8 luma_coefb;    /* RW; Range: [0x0, 0x1F], the weight of Cb when calculate Y, refer to the gain table */
    td_u16 dgain[EXT_ISP_CCM_DGAIN_TAB_SIZE]; /* RW; Range: [0x0, 0x3FFF], 4096 in precision. Manul CCMbuilt-in dgain
                                                 matrix, */
    td_u16 gamma[EXT_ISP_CCM_DGAIN_TAB_SIZE]; /* RW; Range: [0x0, 1024] Manul CCMbuilt-in gamma matrix, */
} ext_isp_ccm_dgain_builtin_attr;

typedef struct {
    td_u8 hue_shift[EXT_ISP_COLOR_SECTOR_NUM]; /* RW; Range: [0x0, 0x28]; Format:8.0;CCM */
    td_u8 sat_shift[EXT_ISP_COLOR_SECTOR_NUM]; /* RW; Range: [0x0, 0x28]; Format:8.0;CCM */
} ext_isp_color_sector_manual;                 /* Not support for 850 */

typedef struct {
    td_u8 hue_shift[EXT_ISP_COLOR_SECTOR_NUM]; /* RW; Range: [0x0, 0x28]; Format:8.0;CCM */
    td_u8 sat_shift[EXT_ISP_COLOR_SECTOR_NUM]; /* RW; Range: [0x0, 0x28]; Format:8.0;CCM */
} ext_isp_color_sector_param;                  /* Not support for 850 */

typedef struct {
    ext_isp_color_sector_param color_tab[EXT_ISP_CCM_MATRIX_NUM];
} ext_isp_color_sector_auto; /* Not support for 850 */

typedef struct {
    td_bool enable; /* RW; Range: [0, 1];  Format:1.0; If enabled, sector on */
    ext_isp_color_sector_manual manual_attr;
    ext_isp_color_sector_auto auto_attr;
} ext_isp_color_sector_attr; /* Not support for 850 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
