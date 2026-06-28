/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_blc_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_BLC_STRUCT_H__
#define __ISP_BLC_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* blc *******************************************/
/* black level */
typedef struct {
    ext_isp_op_type op_type;
    td_u16         black_level[ISP_BAYER_CHN_NUM]; /* RW; Range: [0x0, 0xFFF];Format:12.0;
                                                      Black level values that correspond to the black levels of the
                                                      R,Gr, Gb, and B components respectively. */
} ext_isp_black_level;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
