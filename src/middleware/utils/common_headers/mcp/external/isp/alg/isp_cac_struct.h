/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_cac_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_CAC_STRUCT_H__
#define __ISP_CAC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */
#define ISP_CAC_THR_NUM              2
#define ISP_CAC_EXP_RATIO_NUM        16
#define ISP_CAC_CURVE_NUM            3
#define ISP_AUTO_ISO_NUM             16


#define ISP_CAC_PURPLE_LIM_MAX                 511
#define ISP_CAC_PURPLE_LIM_MIN                 (-512)
#define ISP_CAC_ACAC_EDGE_GAIN_MAX             63
#define ISP_CAC_ACAC_RB_STR_MAX                5
#define ISP_CAC_ACAC_SATU_THD_MAX              255

#define ISP_CAC_FCF_PURPLE_STR_MAX             64
#define ISP_CAC_FCF_PURPLE_STR_MIN             0
#define ISP_CAC_HOR_RATIO_THD_MAX              1023
#define ISP_CAC_HOR_RATIO_THD_MIN              0
#define ISP_CAC_VER_RATIO_THD_MAX              1023
#define ISP_CAC_VER_RATIO_THD_MIN              0
#define ISP_CAC_DET_WIDTH_MAX                  13
#define ISP_CAC_DET_WIDTH_MIN                  3
#define ISP_CAC_DET_HEIGHT_MAX                 7
#define ISP_CAC_DET_HEIGHT_MIN                 3
#define ISP_CAC_HL_WIN_START_X_MAX             4607
#define ISP_CAC_HL_WIN_START_X_MIN             0
#define ISP_CAC_HL_WIN_START_Y_MAX             4607
#define ISP_CAC_HL_WIN_START_Y_MIN             0
#define ISP_CAC_HL_WIN_END_X_MAX               4608
#define ISP_CAC_HL_WIN_END_X_MIN               0
#define ISP_CAC_HL_WIN_END_Y_MAX               4608
#define ISP_CAC_HL_WIN_END_Y_MIN               0
/************************************* cac *******************************************/
typedef struct {
    td_u16  edge_threshold[ISP_CAC_THR_NUM][ISP_AUTO_ISO_STRENGTH_NUM]; /* RW; range: [0, 4095]; format:12.0;
                                                                        cac edge threshold */
    td_u16  edge_gain[ISP_AUTO_ISO_STRENGTH_NUM];           /* RW; range: 1A22 = [0, 63];
                                                         format:10.0; cac edge gain */
    td_u16  cac_rb_strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW; range: 1A22 = [0, 5];
                                                         format:5.0; cac strength on rb */
    td_u16  purple_alpha[ISP_AUTO_ISO_STRENGTH_NUM];        /* RW; range: [0, 63]; format:6.0; cac purple alpha */
    td_u16  edge_alpha[ISP_AUTO_ISO_STRENGTH_NUM];          /* RW; range: [0, 63]; format:6.0; cac edge  alpha */
    td_u16  satu_low_threshold[ISP_AUTO_ISO_STRENGTH_NUM];  /* RW; range: 1A22 = [0, 255];
                                                         format:12.0; cac purple saturation low threshold */
} ext_isp_cac_acac_auto_attr;

typedef struct {
    td_u16  edge_threshold[ISP_CAC_THR_NUM];  /* RW; range: [0, 4095]; format:12.0; cac edge threshold */
    td_u16  edge_gain;              /* RW; range: 1A22 = [0, 63]; format:10.0;
                                       cac edge gain */
    td_u16  cac_rb_strength;        /* RW; range: 1A22 = [0, 5]; format:5.0;
                                       cac strength on rb */
    td_u16  purple_alpha;           /* RW; range: [0, 63]; format:6.0; cac purple alpha */
    td_u16  edge_alpha;             /* RW; range: [0, 63]; format:6.0; cac edge alpha */
    td_u16  satu_low_threshold;     /* RW; range: 1A22 = [0, 255]; format:12.0;
                                       cac purple saturation low threshold */
} ext_isp_cac_acac_manual_attr;

typedef struct {
    ext_isp_cac_acac_manual_attr     acac_manual;
    ext_isp_cac_acac_auto_attr       acac_auto;
} ext_isp_cac_acac_attr;

typedef struct {
    td_u8   de_purple_cr_strength[ISP_CAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0;
                                                         correction strength of the R channel */
    td_u8   de_purple_cb_strength[ISP_CAC_EXP_RATIO_NUM]; /* RW; range: [0, 8]; format:4.0;
                                                         correction strength of the B channel */
} ext_isp_cac_lcac_auto_attr;

typedef struct {
    td_u8   de_purple_cr_strength;  /* RW; range: [0, 8]; format:4.0; correction strength of the R channel */
    td_u8   de_purple_cb_strength;  /* RW; range: [0, 8]; format:4.0; correction strength of the B channel */
} ext_isp_cac_lcac_manual_attr;

typedef struct {
    td_u16  purple_detect_range;        /* RW; range: [0, 410]; format:9.0; purple fringing detection scope */
    td_u16  var_threshold;              /* RW; range: [0, 4095]; format:12.0; edge detection threshold */
    td_u16  r_detect_threshold[ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component R detection thd */
    td_u16  g_detect_threshold[ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component G detection thd */
    td_u16  b_detect_threshold[ISP_CAC_CURVE_NUM]; /* RW; range: [0,4095]; format:12.0; component B detection thd */
    ext_isp_cac_lcac_manual_attr     lcac_manual;
    ext_isp_cac_lcac_auto_attr       lcac_auto;
} ext_isp_cac_lcac_attr;

typedef struct {
    td_bool enable;                     /* RW; range: [0, 1]; format:1.0; cac enable */
    ext_isp_op_type op_type;            /* RW; range: [0, 1]; format:1.0; purple fringing correction working mode */
    td_s16  purple_upper_limit;         /* RW; range: Hi3516CV610 = [-512, 511]; format:s10.0;
                                           cac purple high limitation */
    td_s16  purple_lower_limit;         /* RW; range: Hi3516CV610 = [-512, 511]; format:s10.0;
                                           cac purple low limitation */

    td_u8   fcf_purple_det_str;
    td_u16  hor_ratio_thd;
    td_u16  ver_ratio_thd;
    td_u8   det_width;
    td_u8   det_height;
    td_u16  hl_win_start_x;
    td_u16  hl_win_start_y;
    td_u16  hl_win_end_x;
    td_u16  hl_win_end_y;

    ext_isp_cac_acac_attr     acac_cfg;
    ext_isp_cac_lcac_attr     lcac_cfg;
} ext_isp_cac_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
