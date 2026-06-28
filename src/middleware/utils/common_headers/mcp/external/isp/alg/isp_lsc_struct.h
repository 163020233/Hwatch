/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_lsc_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_LSC_STRUCT_H__
#define __ISP_LSC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* lsc *******************************************/
#define EXT_ISP_LSC_GRID_COL             33
#if defined(CONFIG_SOCT_1A22_SUPPORT)
#define EXT_ISP_LSC_GRID_ROW             19
#else
#define EXT_ISP_LSC_GRID_ROW             33
#endif
#define EXT_ISP_LSC_GRID_POINTS          (EXT_ISP_LSC_GRID_COL * EXT_ISP_LSC_GRID_ROW)
#define EXT_ISP_MLSC_X_HALF_GRID_NUM     ((EXT_ISP_LSC_GRID_COL - 1) / 2)
#define EXT_ISP_MLSC_Y_HALF_GRID_NUM     ((EXT_ISP_LSC_GRID_ROW - 1) / 2)
#define EXT_ISP_MLSC_GAIN_LUT_NUM        2
#define EXT_ISP_RLSC_POINTS             129

typedef enum {
    ISP_GLOBAL_STRENGTH     = 0,
    ISP_CHANNEL_STRENGTH,
    ISP_MESH_STRENGTH,
    ISP_LSC_STRENGTH_MAX
} ext_isp_lsc_strength_mode;

typedef struct {
    td_u16 mesh_str;    /* The strength of the mesh shading correction */
    td_u16 str_scale_r;      // mesh_shading_attr: [0, 1023], u2.8
    td_u16 str_scale_gr;     // mesh_shading_attr: [0, 1023], u2.8
    td_u16 str_scale_gb;     // mesh_shading_attr: [0, 1023], u2.8
    td_u16 str_scale_b;      // mesh_shading_attr: [0, 1023], u2.8
    td_u16 corner_rg_th;     // mesh_shading_attr: [0, 1024], u2.8
    td_u16 corner_bg_th;     // mesh_shading_attr: [0, 1024], u2.8
} ext_isp_shading_manual_attr;

/* mesh shading */
typedef struct {
    td_bool enable;      /* RW; Range:[0, 1];Format:1.0; TD_TRUE: enable lsc; TD_FALSE: disable lsc */
    td_u16  blend_ratio; /* RW; Range:[0, 256];Format:9.0; the blendratio of the two mesh gain lookup-table */

    ext_isp_op_type op_type;
    ext_isp_lsc_strength_mode strength_mode;
    ext_isp_shading_manual_attr manual_attr;
} ext_isp_shading_attr;

typedef struct {
    td_u16  r_gain[EXT_ISP_LSC_GRID_POINTS];  /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                R channel required for LSC. */
    td_u16  gr_gain[EXT_ISP_LSC_GRID_POINTS]; /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                Gr channel required for LSC. */
    td_u16  gb_gain[EXT_ISP_LSC_GRID_POINTS]; /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                Gb channel required for LSC. */
    td_u16  b_gain[EXT_ISP_LSC_GRID_POINTS];  /* RW; Range:[0, 1023];Member used to store the calibration data of the
                                                B channel required for LSC. */
} ext_isp_shading_gain_lut;

typedef struct {
    td_u16  r_gain[EXT_ISP_RLSC_POINTS];  /* RW; Range:[0, 65535];Member used to store the calibration data of the
                                            R channel required for BNR_LSC. */
    td_u16  gr_gain[EXT_ISP_RLSC_POINTS]; /* RW; Range:[0, 65535];Member used to store the calibration data of the
                                            Gr channel required for BNR_LSC. */
    td_u16  gb_gain[EXT_ISP_RLSC_POINTS]; /* RW; Range:[0, 65535];Member used to store the calibration data of the
                                            Gb channel required for BNR_LSC. */
    td_u16  b_gain[EXT_ISP_RLSC_POINTS];  /* RW; Range:[0, 65535];Member used to store the calibration data of the
                                            B channel required for BNR_LSC. */
    td_u16  y_gain[EXT_ISP_RLSC_POINTS];  /* RW; Range:[0, 1023] store the calibration data of the
                                            Y channel required for PNL. */
} ext_isp_bnr_lsc_gain_lut;

typedef struct {
    td_u8   mesh_scale;        /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for
                                  the 10 bit gain coefficients stored in mesh tables */
    td_u16  x_grid_width[EXT_ISP_MLSC_X_HALF_GRID_NUM]; /* limited Range:[4, u32Width/4 - 60],
                                                          Member used to store the width data of each GRID partition */
    td_u16  y_grid_width[EXT_ISP_MLSC_Y_HALF_GRID_NUM]; /* limited Range:[4, u32Height/4 - 60];
                                                        Member used to store the height data of each GRID partition */
    ext_isp_shading_gain_lut lsc_gain_lut[EXT_ISP_MLSC_GAIN_LUT_NUM];
    ext_isp_bnr_lsc_gain_lut bnr_lsc_gain_lut;
} ext_isp_shading_lut_attr;

typedef struct {
    td_u8   mesh_scale;       /* RW; Range:[0, 7];Format:3.0;Three bit value that selects the scale and precision for
                                 the 10 bit gain coefficients stored in mesh tables */
    td_u16  x_grid_width[EXT_ISP_MLSC_X_HALF_GRID_NUM]; /* limited Range:[4, img_width /4 - 60],
                                                     Member used to store the width data of each GRID partition */
    td_u16  y_grid_width[EXT_ISP_MLSC_Y_HALF_GRID_NUM]; /* limited Range:[4, img_height /4 - 60],
                                                     Member used to store the height data of each GRID partition */
    ext_isp_shading_gain_lut lsc_gain_lut;
    ext_isp_bnr_lsc_gain_lut bnr_lsc_gain_lut;
} ext_isp_mesh_shading_table;

typedef enum {
    BAYER_RAWBIT_8BIT    = 8,
    BAYER_RAWBIT_10BIT   = 10,
    BAYER_RAWBIT_12BIT   = 12,
    BAYER_RAWBIT_14BIT   = 14,
    BAYER_RAWBIT_16BIT   = 16,
    BAYER_RAWBIT_BUTT
} ext_isp_bayer_raw_bit;

typedef struct {
    ext_isp_bayer_format bayer;    /* RW; Range: [0, 3];Format ENUM;Shows bayer pattern */
    ext_isp_bayer_raw_bit raw_bit;  /* RW; Range: {8,10,12,14,16};Format ENUM;Shows input raw bitwidth */

    td_u16  img_height; /* RW; Range: [0, 65535];Format 16.0;Input raw image height */
    td_u16  img_width;  /* RW; Range: [0, 65535];Format 16.0;Input raw image width */

    td_u16  dst_img_height; /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_height], Image height that
                               crop from input raw image, set to ImgHeight if don't need to crop */
    td_u16  dst_img_width;  /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_width], Image width that
                               crop from input raw image, set to ImgWidth if don't need to crop */
    td_u16  offset_x;       /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_width - dst_img_width],
                               Horizontal offset that crop from input raw image, set to 0 if don't need to crop */
    td_u16  offset_y;       /* RW; Range: [0, 65535];Format 16.0;limited Range:[0, img_height - dst_img_height],
                               Vertical offset that crop from input raw image, set to 0 if don't need to crop */

    td_u32  mesh_scale; /* RW; Range: [0, 7];Format 3.0; Shows Mesh Scale value */

    td_u16  blc_offset_r; /* RW; Range: [0, 4095];Format 12.0;BLC value for R channel */
    td_u16  blc_offset_gr; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gr channel */
    td_u16  blc_offset_gb; /* RW; Range: [0, 4095];Format 12.0;BLC value for Gb channel */
    td_u16  blc_offset_b; /* RW; Range: [0, 4095];Format 12.0;BLC value for B channel */
} ext_isp_mlsc_calibration_cfg;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
