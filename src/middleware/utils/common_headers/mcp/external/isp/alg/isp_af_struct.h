/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_af_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_AF_STRUCT_H__
#define __ISP_AF_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* af *******************************************/
/* fines whether the peak value of the zoned IIR statistics is calculated.  */
typedef enum {
    ISP_AF_STA_NORM = 0, /* use every value of the block statistic */
    ISP_AF_STA_PEAK,     /* use the maximum value in one line of the block statistic */
    ISP_AF_STA_BUTT
} ext_isp_af_peak_mode;

/* Defines whether the zoned statistics are squared in square mode.  */
typedef enum {
    ISP_AF_STA_SUM_NORM = 0, /* statistic value accumlate */
    ISP_AF_STA_SUM_SQU,      /* statistic value square then accumlate */
    ISP_AF_STA_SUM_BUTT
} ext_isp_af_squ_mode;

/* Crops the input image of the AF module */
typedef struct {
    td_bool enable; /* RW; Range: [0,1];  Format:1.0; AF crop enable */
    td_u16 x;       /* Format:13.0;AF image crop start x, limited range:[0, ImageWidth-256] */
    td_u16 y;       /* Format:13.0;AF image crop start y, limited range:[0, ImageHeight-120] */
    td_u16 w;       /* Format:14.0;AF image crop width, limited range:[256, ImageWidth] */
    td_u16 h;       /* Format:14.0;AF image crop height, limited range:[120, ImageHeight] */
} ext_isp_af_crop;

/* Defines the position of AF module statistics.  */
typedef enum {
    ISP_AF_STATISTICS_AFTER_DGAIN = 0, /* The AF module is placed in the raw field for statistics,AF after DGain */
    ISP_AF_STATISTICS_AFTER_DRC,       /* The AF module is placed in the raw field for statistics,AF after DRC */
    ISP_AF_STATISTICS_AFTER_CSC,       /* The AF module is placed in the YUV field for statistics,AF after CSC */
    ISP_AF_STATISTICS_BUTT
} ext_isp_af_statistics_pos;

/* Configures the Bayer field of the AF module */
typedef struct {
    td_u8 gamma_gain_limit; /* RW; Range: [0x0, 0x5]; Format:3.0 */
    td_u8 gamma_value;
    ext_isp_bayer_format pattern; /* RW; Range: [0x0, 0x3]; Format:2.0;raw domain pattern */
} ext_isp_af_raw_cfg;

/* Configures the pre-filter of the AF module */
typedef struct {
    td_bool en;      /* RW; Range: [0,1]; Format:1.0; pre filter enable  .          */
    td_u16 strength; /* RW; Range: [0x0, 0xFFFF]; Format:16.0;pre filter strength    */
} ext_isp_af_pre_filter_cfg;

/* Defines AF statistics configuration parameters */
typedef struct {
    td_bool enable;                           /* AF enable. */
    td_u16 h_wnd;                             /* AF statistic window horizontal block.  */
    td_u16 v_wnd;                             /* AF statistic window veritical block.  */
    td_u16 h_size;                            /* limited Range: [256, ImageWidth],AF image width. */
    td_u16 v_size;                            /* limited Range: [120, ImageHeight],AF image height. */
    ext_isp_af_peak_mode peak_mode;           /* AF peak value statistic mode. */
    ext_isp_af_squ_mode squ_mode;             /* AF statistic square accumulate. */
    ext_isp_af_crop crop;                     /* AF input image crop */
    ext_isp_af_statistics_pos statistics_pos; /* AF statistic position, it can be set to yuv or raw */
    ext_isp_af_raw_cfg raw_cfg;               /* When AF locate at RAW domain, these para should be cfg. */
    ext_isp_af_pre_filter_cfg pre_flt_cfg;    /* pre filter cfg */
    td_u16 high_luma_th;                      /* high luma threshold. */
} ext_isp_af_cfg;

/* Configures the AF level depend gain module */
typedef struct {
    td_bool ld_en;    /* RW; Range: [0, 1]; FILTER level depend gain enable. */
    td_u16 th_low;    /* RW; range: [0x0, 0xFF]; FILTER level depend th low */
    td_u16 gain_low;  /* RW; range: [0x0, 0xFF]; FILTER level depend gain low */
    td_u16 slp_low;   /* RW; range: [0x0, 0xF];  FILTER level depend slope low */
    td_u16 th_high;   /* RW; range: [0x0, 0xFF]; FILTER level depend th high */
    td_u16 gain_high; /* RW; range: [0x0, 0xFF]; FILTER level depend gain high */
    td_u16 slp_high;  /* RW; range: [0x0, 0xF];  FILTER level depend slope high */
} ext_isp_af_ld;

/* Configures the AF coring module */
typedef struct {
    td_u16 th;  /* RW; Range: [0x0, 0x7FF];FILTER coring threshold. */
    td_u16 slp; /* RW; Range: [0x0, 0xF];  FILTER Coring Slope */
    td_u16 lmt; /* RW; Range: [0x0, 0x7FF];FILTER coring limit */
} ext_isp_af_coring;

#define IIR_EN_NUM 3
#define IIR_GAIN_NUM 7
#define IIR_SHIFT_NUM 4
/* Defines the IIR parameter configuration of horizontal filters for AF statistics. */
typedef struct {
    td_bool narrow_band;                 /* RW; Range: [0, 1]; IIR narrow band enable. */
    td_bool iir_en[IIR_EN_NUM];          /* RW; Range: [0, 1]; IIR enable. */
    td_u8 iir_shift;                     /* RW; Range: [0, 63]; IIR Shift. */
    td_s16 iir_gain[IIR_GAIN_NUM];       /* RW; Range: [-511, 511]. IIR gain,gain0 range:[0,255]. */
    td_u16 iir_shift_lut[IIR_SHIFT_NUM]; /* RW; Range: [0x0, 0x7];  IIR shift. */
    ext_isp_af_ld ld;                    /* RW; filter level depend. */
    ext_isp_af_coring coring;            /* RW; filter coring. */
} ext_isp_af_h_param;

#define FIR_GAIN_NUM 5
typedef struct {
    td_s16 firh[FIR_GAIN_NUM]; /* RW; Range: [-31, 31];   FIR gain. */
    ext_isp_af_ld ld;          /* RW; filter level depend. */
    ext_isp_af_coring coring;  /* RW; filter coring.  */
} ext_isp_af_v_param;

#define ACC_SHIFT_H_NUM 2
#define ACC_SHIFT_V_NUM 2

typedef struct {
    td_u16 acc_shift_y;                  /* RW; Range: [0x0, 0xF]; luminance Y statistic shift. */
    td_u16 acc_shift_h[ACC_SHIFT_H_NUM]; /* RW; Range: [0x0, 0xF]; IIR statistic shift. */
    td_u16 acc_shift_v[ACC_SHIFT_V_NUM]; /* RW; Range: [0x0, 0xF]; FIR statistic shift. */
    td_u16 hl_cnt_shift;                 /* RW; Range: [0x0, 0xF]; High luminance counter shift */
} ext_isp_af_fv_param;

typedef struct {
    ext_isp_af_cfg config;
    ext_isp_af_h_param h_param_iir0;
    ext_isp_af_h_param h_param_iir1;
    ext_isp_af_v_param v_param_fir0;
    ext_isp_af_v_param v_param_fir1;
    ext_isp_af_fv_param fv_param;
} ext_isp_focus_statistics_cfg;

typedef struct {
    td_u16 grid_y_pos[ISP_AF_ZONE_ROW + 1];    /* R */
    td_u16 grid_x_pos[ISP_AF_ZONE_COLUMN + 1]; /* R */
    td_u8 status;                              /* R;0:not update, 1: update,others:reserved */
} ext_isp_focus_grid_info;

typedef struct {
    td_u16 v1;     /* R; Range: [0x0, 0xFFFF];vertical fir block1 Statistics */
    td_u16 h1;     /* R; Range: [0x0, 0xFFFF];horizontal iir block1 Statistics */
    td_u16 v2;     /* R; Range: [0x0, 0xFFFF];vertical fir block2 Statistics */
    td_u16 h2;     /* R; Range: [0x0, 0xFFFF];horizontal iir block2 Statistics */
    td_u16 y;      /* R; Range: [0x0, 0xFFFF];Y Statistics */
    td_u16 hl_cnt; /* R; Range: [0x0, 0xFFFF];HlCnt Statistics */
} ext_isp_focus_zone;

typedef struct {
    ext_isp_focus_zone zone_metrics[ISP_WDR_FRAME_MAX][ISP_AF_ZONE_ROW][ISP_AF_ZONE_COLUMN]; /* R; The zoned measure of
                                                                                                contrast */
} ext_isp_fe_focus_statistics;

typedef struct {
    ext_isp_focus_zone zone_metrics[ISP_AF_ZONE_ROW][ISP_AF_ZONE_COLUMN]; /* R; The zoned measure of contrast */
} ext_isp_be_focus_statistics;

typedef struct {
    ext_isp_fe_focus_statistics fe_af_stat;
    ext_isp_be_focus_statistics be_af_stat;
    ext_isp_focus_grid_info fe_af_grid_info;
    ext_isp_focus_grid_info be_af_grid_info;
    td_u64 pts;
} ext_isp_af_statistics;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
