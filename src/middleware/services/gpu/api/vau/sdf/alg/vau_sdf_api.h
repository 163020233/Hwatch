/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: sdf head file
 */

#ifndef _VAU_SDF_API_H_
#define _VAU_SDF_API_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <securec.h>
#include "vau_sdf_type.h"
#include "vau_sdf_comm_define.h"
#include "vau_sdf_reg_define.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

td_void sdf_alg_process(td_u8 *sdf_picture_addr, vau_sdf_head *head_info, vau_sdf_hw_config *hw_config,
    ext_sdf_input_info *sdf_input_info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif

