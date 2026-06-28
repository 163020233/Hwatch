/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_antifalsecolor_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_ANTIFALSECOLOR_STRUCT_H__
#define __ISP_ANTIFALSECOLOR_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* anti false color *******************************************/
/* antifalsecolor */
typedef struct {
    td_u8    threshold;     /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    td_u8    strength;      /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ext_isp_antifalsecolor_manual_attr;

typedef struct {
    td_u8  threshold[ISP_AUTO_ISO_STRENGTH_NUM];    /* RW;Range:[0x0,0x20];Format:6.0;Threshold for antifalsecolor */
    td_u8  strength[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0x0,0x1F];Format:5.0;Strength of antifalsecolor */
} ext_isp_antifalsecolor_auto_attr;

typedef struct {
    td_bool  enable;                               /* RW;Range:[0x0,0x1];Format:1.0; AntiFalseColor Enable */
    ext_isp_op_type op_type;
    ext_isp_antifalsecolor_auto_attr auto_attr;
    ext_isp_antifalsecolor_manual_attr manual_attr;
} ext_isp_antifalsecolor_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
