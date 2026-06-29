/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: Task adapt
 * Author:
 * Create: 2026-4-1
 */

#include "osal_adapt.h"
#include "los_tick.h"

void osal_idle_handler_hook_reg(osal_idle_handler_hook hook)
{
    LOS_IdleHandlerHookReg(hook);
}

void osal_kthread_backtrace(unsigned long taskId)
{
#ifdef LOSCFG_KERNEL_MIN
    LOS_TaskBackTrace(taskId);
#endif
}

unsigned int osal_ms2tick(unsigned int ms)
{
    return LOS_MS2Tick(ms);
}