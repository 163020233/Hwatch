/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: tdm debug header.
 */

#ifndef TDM_DEBUG_H
#define TDM_DEBUG_H

#include "asm/register_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

.macro TDM_ENABLE
#ifdef LOS_TDM_DEBUG_ENABLE
    li  t0, SOFT_SOC_TEE_ENABLE_ADDR
    lw  t1, 0(t0)
    ori t1, t1, 0x2
    sw  t1, 0(t0)
#endif
.endm

.macro TDM_DISABLE
#ifdef LOS_TDM_DEBUG_ENABLE
    li  t0, SOFT_SOC_TEE_ENABLE_ADDR
    li  t1, 0xFFFFFFFD
    lw  t2, 0(t0)
    and t2, t2, t1
    sw  t2, 0(t0)
#endif
.endm

.macro TDM_FORCE_DISABLE
    li  t0, SOFT_SOC_TEE_ENABLE_ADDR
    li  t1, 0xFFFFFFFD
    lw  t2, 0(t0)
    and t2, t2, t1
    sw  t2, 0(t0)
.endm

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
