/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_clut_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_CLUT_STRUCT_H__
#define __ISP_CLUT_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* clut *******************************************/
/* CLUT */
typedef struct {
    td_bool enable; /* RW; Range:[0, 1];Format:1.0; Enable/Disable CLUT Function, */
    td_u32  gain_r; /* RW; Range:[0, 4095];Format:12.0, */
    td_u32  gain_g; /* RW; Range:[0, 4095];Format:12.0, */
    td_u32  gain_b; /* RW; Range:[0, 4095];Format:12.0, */
} ext_isp_clut_attr;

typedef struct {
    td_u32 lut[EXT_ISP_CLUT_LUT_LENGTH];
} ext_isp_clut_lut;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
