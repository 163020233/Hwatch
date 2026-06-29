/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_ge_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_GE_STRUCT_H__
#define __ISP_GE_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* ge *******************************************/
/* Crosstalk Removal */
typedef struct {
    td_bool  enable;      /* RW; Range: [0, 1];Format 1.0;Enable/disable the crosstalk removal module */
    td_u8    slope;       /* Crosstalk slope value. */
    td_u8    sensi_slope;  /* Crosstalk sensitivity. */
    td_u16   sensi_thr;   /* Crosstalk sensitivity threshold. */
    td_u16   strength[ISP_AUTO_ISO_STRENGTH_NUM];   /* RW; Range: [0,256];Crosstalk strength value. */
    td_u16   threshold[ISP_AUTO_ISO_STRENGTH_NUM];  /* Crosstalk threshold. */
    td_u16   np_offset[ISP_AUTO_ISO_STRENGTH_NUM];   /* Set Noise profile value. */
} ext_isp_cr_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
