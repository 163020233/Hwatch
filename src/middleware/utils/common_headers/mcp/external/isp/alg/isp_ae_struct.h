/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_ae_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_AE_STRUCT_H__
#define __ISP_AE_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* ae *******************************************/
typedef enum {
    AE_MODE_SLOW_SHUTTER = 0,
    AE_MODE_FIX_FRAME_RATE  = 1,
    AE_MODE_BUTT
} ext_isp_ae_mode;

/* Defines the maximum exposure time or gain and minimum exposure time or gain */
typedef struct {
    td_u32 max; /* RW;Range:[0,0xFFFFFFFF];Format:32.0;Maximum value */
    td_u32 min; /* RW;Range:[0,0xFFFFFFFF];Format:32.0;limited Range:[0,max],Minimum value */
} ext_isp_ae_range;

/* Defines the ISP exposure delay attribute */
typedef struct {
    td_u16 black_delay_frame; /* RW; Range:[0, 65535]; Format:16.0; AE black delay frame count */
    td_u16 white_delay_frame; /* RW; Range:[0, 65535]; Format:16.0; AE white delay frame count */
} ext_isp_ae_delay;

/*
  Defines the anti-flicker mode.
  0 = The epxosure time is fixed to be the multiplies of 1/(2*frequency) sec,
    it may lead to over exposure in the high-luminance environments.
  1 = The anti flicker may be closed to avoid over exposure in the high-luminance environments.
 */
typedef enum {
    ISP_ANTIFLICKER_NORMAL_MODE = 0x0,
    ISP_ANTIFLICKER_AUTO_MODE = 0x1,
    ISP_ANTIFLICKER_MODE_BUTT
} ext_isp_antiflicker_mode;

/* Defines the anti-flicker attribute */
typedef struct {
    td_bool enable;  /* RW; Range:[0, 1]; Format:1.0; */
    td_u8 frequency; /* RW; Range:[0, 255]; Format:8.0;
                        usually this value is 50 or 60  which is the frequency of the AC power supply */
    ext_isp_antiflicker_mode mode;
} ext_isp_antiflicker;

typedef struct {
    td_bool enable; /* RW; Range:[0, 1]; Format:1.0; */

    td_u8 luma_diff; /* RW; Range:[0, 100]; Format:8.0; if subflicker mode enable, current luma is less than AE
                        compensation plus LumaDiff, AE will keep min antiflicker shutter time
                        (for example: 1/100s or 1/120s) to avoid flicker. while current luma is larger than AE
                        compensation plus the LumaDiff, AE will reduce shutter time to avoid over-exposure and
                        introduce flicker in the pircture */
} ext_isp_subflicker;

typedef enum {
    AE_EXP_HIGHLIGHT_PRIOR = 0,
    AE_EXP_LOWLIGHT_PRIOR  = 1,
    AE_STRATEGY_MODE_BUTT
} ext_isp_ae_strategy;

typedef struct {
    /* base parameter */
    ext_isp_ae_range exp_time_range;  /* sensor exposure time (unit: us), it's related to the specific sensor */
    ext_isp_ae_range a_gain_range;    /* sensor analog gain (unit: times, 10bit precision), it's related to the
                                        specific sensor */
    ext_isp_ae_range d_gain_range;    /* sensor digital gain (unit: times, 10bit precision), it's related to the
                                        specific sensor */
    ext_isp_ae_range ispd_gain_range; /* ISP digital gain (unit: times, 10bit precision), it's related to the
                                         ISP digital gain range */
    ext_isp_ae_range sys_gain_range;  /* system gain (unit:times, 10bit precision), it's related to the specific sensor
                                         and ISP Dgain range */
    td_u32 gain_threshold;            /* Gain threshold for slow shutter mode (unit: times, 10bit precision) */

    td_u8 speed;                          /* AE adjust step for dark scene to bright scene switch */
    td_u16 black_speed_bias;              /* AE adjust step bias for bright scene to dark scene switch */
    td_u8 tolerance;                      /* AE adjust tolerance */
    td_u8 compensation;                   /* AE compensation */
    td_u16 ev_bias;                       /* AE EV bias */
    ext_isp_ae_strategy ae_strategy_mode; /* Support Highlight prior or Lowlight prior */
    td_u16 hist_ratio_slope;              /* AE hist ratio slope */
    td_u8 max_hist_offset;                /* Max hist offset */

    ext_isp_ae_mode ae_mode; /* AE mode(slow shutter/fix framerate)(onvif) */
    ext_isp_antiflicker antiflicker;
    ext_isp_subflicker subflicker;
    ext_isp_ae_delay ae_delay_attr;

    td_bool manual_exp_value; /* manual exposure value or not */
    td_u32 exp_value;         /* manual exposure value */

    ext_isp_fswdr_mode fswdr_mode; /* FSWDR running mode */
    td_bool wdr_quick;             /* WDR use delay strategy or not; If is true, WDR AE adjust will be faster */

    td_u16 iso_cal_coef; /* The coefficient between Standard ISO and origin ISO (unit: times, 8bit precision) */
} ext_isp_ae_attr;

typedef struct {
    ext_isp_op_type exp_time_op_type;
    ext_isp_op_type a_gain_op_type;
    ext_isp_op_type d_gain_op_type;
    ext_isp_op_type ispd_gain_op_type;

    td_u32 exp_time;   /* sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32 a_gain;     /* sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 d_gain;     /* sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 isp_d_gain; /* ISP digital gain(unit: times, 10bit precision),it's related to the ISP digital gain range */
} ext_isp_me_attr;

typedef enum {
    LONG_FRAME      = 0,
    SHORT_FRAME       = 1,
    PRIOR_FRAME_BUTT
} ext_isp_prior_frame;

typedef struct {
    td_bool bypass;
    ext_isp_op_type op_type;
    td_u8 ae_run_interval;     /* set the AE run interval */
    td_bool hist_stat_adjust;  /* TD_TRUE: 256 bin histogram statistic config will adjust when large red or blue
                                  area detected. TD_FALSE: 256 bin histogram statistic config will not change */
    td_bool ae_route_ex_valid; /* use extend AE route or not */
    ext_isp_me_attr manual_attr;
    ext_isp_ae_attr auto_attr;
    ext_isp_prior_frame prior_frame; /* AE prior frame */
    td_bool ae_gain_sep_cfg;         /* long and short frame gain separately configure or not */
    td_bool advance_ae;              /* open advance AE or not */
} ext_isp_exposure_attr;

typedef struct {
    ext_isp_op_type exp_ratio_type;  /* RW; Range:[0, 1]; Format:1.0;
                                       OP_TYPE_AUTO: The ExpRatio used in ISP is generated by firmware;
                                       OP_TYPE_MANUAL: The ExpRatio used in ISP is set by exp_ratio */
    td_u32 exp_ratio[EXP_RATIO_NUM]; /* RW; Range:[0x40, 0xFFF]; Format:26.6; 0x40 means 1 times.
                                        When exp_ratio_type is OP_TYPE_AUTO, exp_ratio is invalid.
                                        When exp_ratio_type is OP_TYPE_MANUAL, exp_ratio is quotient of
                                        long exposure time / short exposure time. */
    td_u32 exp_ratio_max;            /* RW; Range:[0x40, 0x4000]; Format:26.6; 0x40 means 1 times.
                                        When exp_ratio_type is OP_TYPE_AUTO, exp_ratio_max is max(upper limit) of
                                        ExpRatio generated by firmware.
                                        When exp_ratio_type is OP_TYPE_MANUAL, exp_ratio_max is invalid. */
    td_u32 exp_ratio_min;            /* RW; Range:[0x40, 0x4000]; Format:26.6; limited Range:[0x40, exp_ratio_max],
                                        0x40 means 1 times.
                                        When exp_ratio_type is OP_TYPE_AUTO, exp_ratio_min is min(lower limit) of
                                        ExpRatio generated by firmware.
                                        When exp_ratio_type is OP_TYPE_MANUAL, exp_ratio_min is invalid. */
    td_u16 tolerance;                /* RW; Range:[0x0, 0xFF]; Format:8.0;et the dynamic range tolerance.
                                        Format: unsigned 6.2-bit fixed-point. 0x4 means 1dB. */
    td_u16 speed;                    /* RW; Range:[0x0, 0xFF]; Format:8.0; exposure ratio adjust speed */
    td_u16 ratio_bias;               /* RW; Range:[0x0, 0xFFFF]; Format:16.0; exposure ratio bias */
    td_u16 high_light_target;        /* RW; Range:[0x0, 0x400]; Format:10.0; the high light target. */
    td_u16 exp_coef_min;             /* 0x400 means 1 times; min exposure coefficient */
} ext_isp_wdr_exposure_attr;

typedef enum {
    ISP_AE_FOUR_PLANE_MODE_DISABLE = 0,
    ISP_AE_FOUR_PLANE_MODE_ENABLE,
    ISP_AE_FOUR_PLANE_MODE_BUTT
} ext_isp_ae_four_plane_mode;

typedef enum {
    ISP_AE_NORMAL = 0,
    ISP_AE_ROOT,
    ISP_AE_MODE_BUTT
} ext_isp_ae_stat_mode;

/* Crops the input image of the AE module */
typedef struct {
    td_bool enable; /* RW; Range: [0,1];  Format:1.0;AE crop enable. */
    td_u16 x;       /* Format:13.0;AE image crop start x, limited range:[0, ImageWidth - 256] */
    td_u16 y;       /* Format:13.0;AEimage crop start y, limited range:[0, ImageHeight - 120]  */
    td_u16 w;       /* Format:14.0;AE image crop width,  limited range:[256, ImageWidth] */
    td_u16 h;       /* Format:14.0;AE image crop height  limited range:[120, ImageHeight] */
} ext_isp_ae_crop;

/* AE */
typedef enum {
    ISP_AE_AFTER_DG = 0,
    ISP_AE_AFTER_WB,
    ISP_AE_AFTER_DRC,
    ISP_AE_SWITCH_BUTT
} ext_isp_ae_switch;

typedef enum {
    ISP_AE_HIST_SKIP_EVERY_PIXEL = 0,
    ISP_AE_HIST_SKIP_EVERY_2ND_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_3RD_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_4TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_5TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_8TH_PIXEL,
    ISP_AE_HIST_SKIP_EVERY_9TH_PIXEL,
    ISP_AE_HIST_SKIP_BUTT
} ext_isp_ae_hist_skip;

typedef enum {
    ISP_AE_HIST_START_FORM_FIRST_COLUMN = 0,
    ISP_AE_HIST_START_FORM_SECOND_COLUMN,
    ISP_AE_HIST_OFFSET_X_BUTT
} ext_isp_ae_hist_offset_x;

typedef enum {
    ISP_AE_HIST_START_FORM_FIRST_ROW = 0,
    ISP_AE_HIST_START_FORM_SECOND_ROW,
    ISP_AE_HIST_OFFSET_Y_BUTT
} ext_isp_ae_hist_offset_y;

/* Defines the mode configuration for the sampling points during global histogram statistics. */
typedef struct {
    ext_isp_ae_hist_skip hist_skip_x; /* Histogram decimation in horizontal direction: 0=every pixel;
                                        1=every 2nd pixel; 2=every 3rd pixel; 3=every 4th pixel; 4=every 5th pixel;
                                        5=every 8th pixel; 6+=every 9th pixel */
    ext_isp_ae_hist_skip hist_skip_y; /* Histogram decimation in vertical direction: 0=every pixel; 1=every 2nd pixel;
                                        2=every 3rd pixel; 3=every 4th pixel; 4=every 5th pixel; 5=every 8th pixel;
                                        6+=every 9th pixel */
    ext_isp_ae_hist_offset_x hist_offset_x; /* 0= start from the first column; 1=start from second column */
    ext_isp_ae_hist_offset_y hist_offset_y; /* 0= start from the first row; 1= start from second row */
} ext_isp_ae_hist_config;

typedef struct {
    ext_isp_ae_switch ae_switch; /* RW; Range:[0, 2]; Format:2.0;
                                   The position of AE histogram in ISP BE pipeline */
    ext_isp_ae_hist_config hist_config;
    ext_isp_ae_four_plane_mode four_plane_mode; /* RW; Range:[0, 1]; Format:2.0;Four Plane Mode Enable */
    ext_isp_ae_stat_mode hist_mode;             /* RW; Range:[0, 1]; Format:2.0;AE Hist Rooting Mode */
    ext_isp_ae_stat_mode aver_mode;             /* RW; Range:[0, 1]; Format:2.0;AE Aver Rooting Mode */
    ext_isp_ae_stat_mode max_gain_mode;         /* RW; Range:[0, 1]; Format:2.0;Max Gain Rooting Mode */
    ext_isp_ae_crop crop;
    ext_isp_ae_crop fe_crop;
    td_u8 weight[ISP_AE_ZONE_ROW][ISP_AE_ZONE_COLUMN]; /* RW; Range:[0x0, 0xF]; Format:4.0; AE weighting table */
    td_u8 be_weight[ISP_BE_AE_ZONE_ROW][ISP_BE_AE_ZONE_COLUMN]; /* RW; Range:[0x0, 0xF]; Format:4.0; */
} ext_isp_ae_statistics_cfg;

typedef enum {
    ISP_IRIS_F_NO_32_0 = 0,
    ISP_IRIS_F_NO_22_0,
    ISP_IRIS_F_NO_16_0,
    ISP_IRIS_F_NO_11_0,
    ISP_IRIS_F_NO_8_0,
    ISP_IRIS_F_NO_5_6,
    ISP_IRIS_F_NO_4_0,
    ISP_IRIS_F_NO_2_8,
    ISP_IRIS_F_NO_2_0,
    ISP_IRIS_F_NO_1_4,
    ISP_IRIS_F_NO_1_0,
    ISP_IRIS_F_NO_BUTT,
} ext_isp_iris_f_no;

#define ISP_AE_ROUTE_MAX_NODES 16
typedef struct {
    td_u32 int_time;            /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0;
                                   sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32 sys_gain;            /* RW; Range:[0x400, 0xFFFFFFFF]; Format:22.10;
                                   system gain (unit: times, 10bit precision), it's related to the specific sensor and
                                   ISP Dgain range */
    ext_isp_iris_f_no iris_fno; /* RW; Range:[0, 10]; Format:4.0;
                                  the F number of the iris's aperture, only support for Piris */
    td_u32 iris_fno_lin;        /* RW; Range:[0x1, 0x400]; Format:32.0;
                                   the equivalent gain of F number of the iris's aperture, only support for Piris */
} ext_isp_ae_route_node;

typedef struct {
    td_u32 total_num; /* RW; Range:[0, 0x10]; Format:8.0; total node number of AE route */
    ext_isp_ae_route_node route_node[ISP_AE_ROUTE_MAX_NODES];
} ext_isp_ae_route;

#define ISP_AE_ROUTE_EX_MAX_NODES 16
typedef struct {
    td_u32 int_time;  /* RW; Range:(0x0, 0xFFFFFFFF]; Format:32.0;
                         sensor exposure time (unit: us), it's related to the specific sensor */
    td_u32 a_gain;    /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;
                         sensor analog gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 d_gain;    /* RW; Range:[0x400, 0x3FFFFF]; Format:22.10;
                         sensor digital gain (unit: times, 10bit precision), it's related to the specific sensor */
    td_u32 isp_d_gain;          /* RW; Range:[0x400, 0x40000]; Format:22.10;
                                   ISP digital gain (unit: times, 10bit precision) */
    ext_isp_iris_f_no iris_fno; /* RW; Range:[0, 10]; Format:4.0;
                                  the F number of the iris's aperture, only support for Piris */
    td_u32 iris_fno_lin;        /* RW; Range:[0x1, 0x400]; Format:32.0;
                                   the equivalent gain of F number of the iris's aperture, only support for Piris */
} ext_isp_ae_route_ex_node;

typedef struct {
    td_u32 total_num; /* RW; Range:[0, 0x10]; Format:8.0; total node number of extend AE route */
    ext_isp_ae_route_ex_node route_ex_node[ISP_AE_ROUTE_EX_MAX_NODES];
} ext_isp_ae_route_ex;

typedef struct {
    td_u32 exp_time;                        /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_u32 short_exp_time;                  /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_u32 median_exp_time;                 /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_u32 long_exp_time;                   /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0; */
    td_u32 a_gain;                          /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32 d_gain;                          /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32 a_gain_sf;                       /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32 d_gain_sf;                       /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32 isp_d_gain;                      /* R; Range:[0x400, 0xFFFFFFFF]; Format:22.10; */
    td_u32 exposure;                        /* R; Range:[0x40, 0xFFFFFFFF]; Format:26.6; */
    td_bool exposure_is_max;                /* R; Range:[0, 1]; Format:1.0; */
    td_s16 hist_error;                      /* R; Range:[-32768, 32767]; Format:16.0; */
    td_u32 ae_hist1024_value[ISP_HIST_NUM]; /* R; Range:[0x0, 0xFFFFFFFF]; Format:32.0;
                                           1024 bins histogram for channel 1 */
    td_u8 ave_lum;
    td_u32 lines_per500ms; /* exposure lines per 500ms */
    td_u32 piris_fno;
    td_u32 fps; /* actual fps */
    td_u32 iso;
    td_u32 isosf;
    td_u32 iso_calibrate;
    td_u32 ref_exp_ratio;
    td_u16 wdr_exp_coef;      /* 0x400 means 1 times */
    td_u32 first_stable_time; /* AE first stable time for quick start */
    td_u32 quick_star_iso;
    ext_isp_ae_route ae_route;          /* Actual AE route */
    ext_isp_ae_route_ex ae_route_ex;    /* Actual AE route_ex */
    ext_isp_ae_route ae_route_sf;       /* Actual AE route_sf */
    ext_isp_ae_route_ex ae_route_sf_ex; /* Actual AE route_sf_ex */
} ext_isp_exp_info;

typedef struct {
    td_u16 grid_y_pos[ISP_AE_ZONE_ROW + 1];    /* R */
    td_u16 grid_x_pos[ISP_AE_ZONE_COLUMN + 1]; /* R */
    td_u8 status;                              /* R;0:not update, 1: update,others:reserved */
} ext_isp_ae_grid_info;

typedef struct {
    td_u16 grid_y_pos[ISP_BE_AE_ZONE_ROW + 1];    /* R */
    td_u16 grid_x_pos[ISP_BE_AE_ZONE_COLUMN + 1]; /* R */
    td_u8 status;                                 /* R;0:not update, 1: update,others:reserved */
} ext_isp_be_ae_grid_info;

typedef struct {
    td_u32 fe_hist1024_value[ISP_WDR_FRAME_MAX][ISP_HIST_NUM];      /* Global 1024 bins histogram of FE */
    td_u16 fe_global_avg[ISP_WDR_FRAME_MAX][ISP_BAYER_PATTERN_NUM]; /* lobal average value of FE, */
    td_u16 fe_zone_avg[ISP_WDR_FRAME_MAX][ISP_AE_ZONE_ROW][ISP_AE_ZONE_COLUMN][ISP_BAYER_PATTERN_NUM]; /* Zone avrg
                                                                                                          value of FE */
    td_u32 be_hist1024_value[ISP_HIST_NUM];      /* Global 1024 bins histogram of BE */
    td_u16 be_global_avg[ISP_BAYER_PATTERN_NUM]; /* Global average value of BE */
    td_u16 be_zone_avg[ISP_BE_AE_ZONE_ROW][ISP_BE_AE_ZONE_COLUMN][ISP_BAYER_PATTERN_NUM]; /* Zone average value of BE */
    ext_isp_ae_grid_info fe_grid_info;
    ext_isp_be_ae_grid_info be_grid_info;
    td_u64 pts;
} ext_isp_ae_statistics;

typedef enum {
    ISP_IRIS_DC_TYPE = 0,
    ISP_IRIS_P_TYPE,

    ISP_IRIS_TYPE_BUTT,
} ext_isp_iris_type;

typedef struct {
    td_bool enable;               /* RW; Range:[0, 1]; Format:1.0; smart ae enable or not */
    td_bool ir_mode;              /* RW; Range:[0, 1]; Format:1.0; smart ae IR mode or not */
    ext_isp_op_type smart_exp_type;  /* RW; Range:[0, 1]; Format:1.0;
                                       OP_TYPE_AUTO: The ExpCoef used in ISP is  generated by firmware;
                                       OP_TYPE_MANUAL: The ExpCoef used in ISP is set by u32ExpCoef */
    td_u16  exp_coef;             /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                     When exp_hdr_lv_type is OP_TYPE_AUTO, u32ExpCoef is invalid.
                                     When exp_hdr_lv_type is OP_TYPE_MANUAL, u32ExpCoef is the quotient of exposure. */
    td_u8   luma_target;          /* RW; Range:[0x0, 0xFF]; Format:8.0; luma target of smart ae. */
    td_u16  exp_coef_max;         /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                     When exp_hdr_lv_type is OP_TYPE_AUTO, u32ExpCoefvMax is max(upper limit) of ExpCoef
                                     generated by firmware.
                                     When exp_hdr_lv_type is OP_TYPE_MANUAL, u32ExpCoefMax is invalid. */
    td_u16  exp_coef_min;         /* RW; Range:[0x0, 0xFFFF]; Format:6.10; 0x400 means 1 times.
                                     When exp_hdr_lv_type is OP_TYPE_AUTO, u32ExpCoefMax is min(lower limit) of ExpCoef
                                     generated by firmware.
                                     When exp_hdr_lv_type is OP_TYPE_MANUAL, u32ExpCoefMin is invalid. */
    td_u8   smart_interval;       /* RW; Range:[0x1, 0xFF]; Format:8.0; smart ae run interval */
    td_u8   smart_speed;          /* RW; Range:[0x0, 0xFF]; Format:8.0; smart ae adjust step */
    td_u16  smart_delay_num;      /* RW; Range:[0x0, 0x400]; Format:16.0; smart ae adjust delay frame num */
} ext_isp_smart_exposure_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
