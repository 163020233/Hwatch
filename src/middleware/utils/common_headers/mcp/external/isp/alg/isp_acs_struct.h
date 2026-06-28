/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_acs_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_ACS_STRUCT_H__
#define __ISP_ACS_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* acs *******************************************/
/* acs */
typedef struct {
    td_bool enable;       /* RW; Range:[0,1];Format:1.0;Acs Enable */
    td_u16  y_strength;   /* RW; Range:[0,256];Format:8.0;Acs correction strength for y shading */
    td_u16  run_interval; /* RW; Range:[1,255];Format:16.0;Acs Run Interval */
    td_bool lock_enable;  /* RW; Range:[0,1];Format:1.0;Lock Gain Lut Enable */
} ext_isp_acs_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
