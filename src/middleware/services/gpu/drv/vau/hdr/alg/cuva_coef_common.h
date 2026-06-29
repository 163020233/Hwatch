/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: api for cuva hdr
 */

#ifndef __CUVA_COEF_COMMON_H__
#define __CUVA_COEF_COMMON_H__

#include "cuva_api_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define L2PQ_LUT_X_SCALE2P 18 /*  14.18   2^18*10000 */
#define L2PQ_LUT_B_SCALE2P 16 /*  0.16   must bigger or equal to CUVA_PROCESS_BIT */

#define PQ2L_INPUT_SCALE2P  16  /* must bigger or equal to CUVA_PROCESS_BIT */
#define PQ2L_OUTPUT_SCALE2P 18  /* multiple Lum */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
