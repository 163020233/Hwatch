/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: CPUP
 * Author:
 * Create: 2026-4-1
 */
#include <stdint.h>
#include "osal_adapt.h"
#include "los_cpup.h"
#include "los_memory.h"
#include "los_tick.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_CPUP

unsigned int osal_allcpu_usage(unsigned short maxNum, osal_cpup_info_t *cpupInfo,
                               unsigned int mode, unsigned short flag)
{
    UINT32 ret, i;
    CPUP_INFO_S *internalInfo;
    if (cpupInfo == NULL) {
        return LOS_ERRNO_CPUP_TASK_PTR_NULL;
    }

    if (maxNum == 0) {
        return  LOS_ERRNO_CPUP_MAXNUM_INVALID;
    }
    
    internalInfo = (CPUP_INFO_S *)LOS_MemAlloc(m_aucSysMem0, maxNum * sizeof(CPUP_INFO_S));
    if (internalInfo == NULL) {
        return LOS_ERRNO_CPUP_NO_MEMORY;
    }

    ret = LOS_AllCpuUsage(maxNum, internalInfo, mode, flag);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, internalInfo);
        return (unsigned int)ret;
    }
    for (i = 0; i < maxNum; i++) {
        cpupInfo[i].status = internalInfo[i].usStatus;
        cpupInfo[i].usage  = internalInfo[i].uwUsage;
    }

    LOS_MemFree((void*)m_aucSysMem0, internalInfo);
    return (unsigned int)ret;
}

void osal_cpup_reset(void)
{
    LOS_CpupReset();
}

void osal_cpup_stop(void)
{
#ifdef LOSCFG_CPUP_START_STOP
    LOS_CpupStop();
#endif
}

void osal_cpup_start(void)
{
#ifdef LOSCFG_CPUP_START_STOP
    LOS_CpupStart();
#endif
}

unsigned int osal_history_task_cpup_usage(osal_task *task, unsigned int mode)
{
    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    return LOS_HistoryTaskCpuUsage(uwtask_id, mode);
}

unsigned int osal_cpup_history_cpu_usage(unsigned int mode)
{
    return (unsigned int)LOS_HistorySysCpuUsage(mode);
}

void osal_get_cpu_cycle(unsigned int *highCnt, unsigned int *lowCnt)
{
    LOS_GetCpuCycle(highCnt, lowCnt);
}

#endif
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
