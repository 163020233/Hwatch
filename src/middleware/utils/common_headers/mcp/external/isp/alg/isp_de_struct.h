/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_de_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_DE_STRUCT_H__
#define __ISP_DE_STRUCT_H__

#include "mpi_mcp_dependency.h"
#include "drv_isp_comm_struct.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* ge *******************************************/
#define EXT_ISP_DE_LUMA_GAIN_LUT_N      17

typedef struct {
    td_u16       global_gain;         /* RW;Range:[0, 0x100];Format:1.8 */
    td_u16       gain_lf;             /* RW;Range:[0, 0x20];Format:2.4 */
    td_u16       gain_hf;             /* RW;Range:[0, 0x20];Format:2.4 */
} ext_isp_de_manual_attr;

typedef struct {
    td_u16       global_gain[ISP_AUTO_ISO_STRENGTH_NUM]; /* RW;Range:[0x0, 0x100];Format:1.8 */
    td_u16       gain_lf[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0x0, 0x20];Format:2.4 */
    td_u16       gain_hf[ISP_AUTO_ISO_STRENGTH_NUM];     /* RW;Range:[0x0, 0x20];Format:2.4 */
} ext_isp_de_auto_attr;

typedef struct {
    td_bool       enable;                                    /* RW;Range:[0x0,0x1];Format:1.0; De Enable */
    td_u16        luma_gain_lut[EXT_ISP_DE_LUMA_GAIN_LUT_N]; /* RW;Range:[0x0,0x100];Format:1.8 */

    ext_isp_op_type        op_type;
    ext_isp_de_auto_attr   auto_attr;
    ext_isp_de_manual_attr manual_attr;
} ext_isp_de_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
