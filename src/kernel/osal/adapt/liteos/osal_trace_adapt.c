/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Trace
 * Author:
 * Create: 2026-4-1
 */

#include <stdint.h>
#include "osal_adapt.h"
#include "los_trace.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void osal_trace_event_mask_set(unsigned int mask)
{
    LOS_TraceEventMaskSet(mask);
}

unsigned int osal_trace_start(void)
{
    return (unsigned int)LOS_TraceStart();
}

void osal_trace_stop(void)
{
    LOS_TraceStop();
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
