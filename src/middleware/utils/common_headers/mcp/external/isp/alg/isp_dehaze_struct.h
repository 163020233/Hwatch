/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_dehaze_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_DEHAZE_STRUCT_H__
#define __ISP_DEHAZE_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* dehaze *******************************************/
#define DEHAZE_LUT_SIZE                 256
/* Defines the manual dehze attribute */
typedef struct {
    td_u8 strength;  /* RW;Range:[0,0xFF];Format:8.0;Manual dehze strength */
} ext_isp_dehaze_manual_attr;

/* Defines the automatic dehze attribute */
typedef struct {
    td_u8 strength; /* RW;Range:[0,0xFF];Format:8.0;Weighted coefficient for automatic dehaze strength. */
} ext_isp_dehaze_auto_attr;

/* Defines the ISP dehaze attribute */
typedef struct {
    td_bool  enable;           /* RW;Range:[0,1];Format:1.0; */
    td_bool  user_lut_enable;    /* RW;Range:[0,1];0:Auto Lut 1:User Lut */
    td_u8    dehaze_lut[DEHAZE_LUT_SIZE];
    ext_isp_op_type      op_type;
    ext_isp_dehaze_manual_attr manual_attr;
    ext_isp_dehaze_auto_attr   auto_attr;
    td_u16       tmprflt_incr_coef; /* RW, Range: [0x0, 0x80].filter increase coeffcient. */
    td_u16       tmprflt_decr_coef; /* RW, Range: [0x0, 0x80].filter decrease coeffcient. */
} ext_isp_dehaze_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
