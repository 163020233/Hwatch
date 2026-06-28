/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Function of isp_pregamma_struct.h
 * Author: AuthorNameMagicTag
 * Create: 2025-05-26
 */

#ifndef __ISP_PREGAMMA_STRUCT_H__
#define __ISP_PREGAMMA_STRUCT_H__

#include "mpi_mcp_dependency.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

/************************************* pregammar *******************************************/
#define PREGAMMA_NODE_NUM               257

/* PRE-GAMMA */
typedef struct {
    td_bool enable;                   /* RW; Range:[0, 1]; Format:1.0;Enable/Disable PreGamma Function */
    td_u32  table[PREGAMMA_NODE_NUM]; /* PreGamma LUT nodes value */
} ext_isp_pregamma_attr;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif
