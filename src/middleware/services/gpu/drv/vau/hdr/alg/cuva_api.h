/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2024. All rights reserved.
 * Description: CUVA
 */

#ifndef __CUVA_API_H__
#define __CUVA_API_H__

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#include "td_type.h"
#include "cuva_api_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define HIPP_HDR_TM_LUT_SIZE 64

td_s32 cuva_update_tmlut_core(cuva_api_param *pst_cuva_in_param, td_u32 *p_tm_coef_lut, td_u32 tm_lut_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif