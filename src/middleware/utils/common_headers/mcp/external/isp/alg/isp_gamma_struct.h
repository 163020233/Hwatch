/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_gamma_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_GAMMA_STRUCT_H__
#define __ISP_GAMMA_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* gammar *******************************************/
#define GAMMA_NODE_NUM                  1025

/* GAMMA */
typedef enum {
    ISP_GAMMA_CURVE_DEFAULT = 0x0,
    ISP_GAMMA_CURVE_SRGB,
    ISP_GAMMA_CURVE_HDR,
    ISP_GAMMA_CURVE_USER_DEFINE,
    ISP_GAMMA_CURVE_BUTT
} ext_isp_gamma_curve_type;

typedef struct {
    td_bool   enable;                     /* RW; Range:[0, 1]; Format:1.0;Enable/Disable Gamma Function */
    td_u16    table[GAMMA_NODE_NUM];    /* RW; Range:[0, 4095]; Format:12.0;Gamma LUT nodes value */

    ext_isp_gamma_curve_type curve_type;    /* RW; Range:[0, 3]; Format:2.0;Gamma curve type */
} ext_isp_gamma_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
