/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_flicker_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_FLICKER_STRUCT_H__
#define __ISP_FLICKER_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* flicker *******************************************/
typedef enum {
    FLICKER_TYPE_NONE = 0,
    FLICKER_TYPE_UNKNOW,
    FLICKER_TYPE_50HZ,
    FLICKER_TYPE_60HZ,
    FLICKER_TYPE_BUTT,
} ext_isp_calc_flicker_type;

typedef struct  {
    td_u32 lines_per_second;      /* The total line number of 1 second */
} ext_isp_calc_flicker_input;

typedef struct {
    ext_isp_calc_flicker_type flicker_type;  /* The calculate result of flicker type */
} ext_isp_calc_flicker_output;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
