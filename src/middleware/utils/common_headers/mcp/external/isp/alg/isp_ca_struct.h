/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_ca_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_CA_STRUCT_H__
#define __ISP_CA_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* ca *******************************************/
#define EXT_ISP_CA_YRATIO_LUT_LENGTH     128
#define EXT_ISP_CA_CHLUT_LENGTH          16
#define EXT_ISP_CA_YCO_LUT_LENGHT        96

/* CA */
typedef enum {
    ISP_CA_ENABLE = 0x0,
    ISP_CP_ENABLE,
    ISP_CA_BUTT
} ext_isp_ca_type;

typedef struct {
    td_u32  y_ratio_lut[EXT_ISP_CA_YRATIO_LUT_LENGTH];  /* RW;Range:[0,2047];Format:1.11 */
    td_s32  iso_ratio[ISP_AUTO_ISO_STRENGTH_NUM];      /* RW;Range:[0,2047];Format:1.10 */
    td_u32  y_sat_lut[EXT_ISP_CA_YRATIO_LUT_LENGTH];  /* RW;Range:[0,2047];Format:1.11, just for 1A22 */
    td_s16  color1_u_lut[EXT_ISP_CA_CHLUT_LENGTH];
    td_s16  color1_v_lut[EXT_ISP_CA_CHLUT_LENGTH];
    td_s16  color2_u_lut[EXT_ISP_CA_CHLUT_LENGTH];
    td_s16  color2_v_lut[EXT_ISP_CA_CHLUT_LENGTH];
    td_s16  color1_ratio_lut[EXT_ISP_CA_CHLUT_LENGTH];
    td_s16  color2_ratio_lut[EXT_ISP_CA_CHLUT_LENGTH];
} ext_isp_ca_lut;

typedef struct {
    td_u8   cp_lut_y[EXT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
    td_u8   cp_lut_u[EXT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
    td_u8   cp_lut_v[EXT_ISP_CA_YRATIO_LUT_LENGTH]; /* RW;Range:[0,255];Format:8.0; */
} ext_isp_cp_lut;

typedef struct {
    td_bool enable;             /* RW;Range:[0x0,0x1];Format:1.0 */
    ext_isp_ca_type ca_cp_en;
    ext_isp_ca_lut  ca;
    ext_isp_cp_lut  cp;
} ext_isp_ca_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
