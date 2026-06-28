/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: ARCH cpu trace interface for project
 *
 * Create:  2021-10-25
 */
#ifndef ARCH_TRACE_H
#define ARCH_TRACE_H

#include <stdint.h>
#include "platform_core.h"
#if defined (SUPPORT_CPU_TRACE) || defined (SUPPORT_CPU_TRACE_V2)
#include "cpu_trace_unified.h"
#include "cpu_trace_porting.h"
#endif
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define  RISCV_TRACE_MEM_REGION_START   TRACE_MEM_REGION_START
#define  RISCV_TRACE_MEM_REGION_LENGTH  TRACE_MEM_REGION_LENGTH

#if defined (SUPPORT_CPU_TRACE) || defined (SUPPORT_CPU_TRACE_V2)
typedef cpu_trace_item_t  riscv_cpu_trace_item_t;
#endif

/**
 * @brief  Disable cpu trace for riscv.
 */
static inline void riscv_cpu_trace_disable(void)
{
#if defined (SUPPORT_CPU_TRACE) || defined (SUPPORT_CPU_TRACE_V2)
    uapi_cpu_trace_disable(cpu_trace_get_self_id());
#endif
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
