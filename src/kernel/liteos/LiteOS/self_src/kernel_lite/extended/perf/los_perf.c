/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2017-2020. All rights reserved.
 * Description: LiteOS Performance Monitor Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2017-01-01
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/

#include "los_perf_pri.h"
#include "perf_pmu_pri.h"
#include "perf_output_pri.h"
#ifdef LOSCFG_KERNEL_SMP
#include "los_atomic.h"
#endif

#ifdef LOSCFG_KERNEL_MEM_ALLOC
#define OS_PERF_RES_DEF(name, type, size)   type *name = NULL
#define OS_PERF_RES_ALLOC(name, type, size) do {                        \
    name = (type *)LOS_MemAllocAlign(m_aucSysMem1, sizeof(type) * (size), PERF_EVENT_ALIGN_SIZE); \
    if ((name) != NULL) {                                               \
        (VOID)memset((name), 0, sizeof(type) * (size));                 \
    }                                                                   \
} while (0)
#define OS_PERF_RES_FREE(name, type, size) do {                         \
    (VOID)LOS_MemFree(m_aucSysMem1, (name));                            \
    name = NULL;                                                        \
} while (0)
#else
#define OS_PERF_RES_DEF(name, type, size)   __attribute__((aligned(PERF_EVENT_ALIGN_SIZE))) type name[size]
#define OS_PERF_RES_ALLOC(name, type, size)
#define OS_PERF_RES_FREE(name, type, size) do  {                        \
    (VOID)memset((name), 0, sizeof(type) * (size));                     \
} while (0)
#endif

STATIC Pmu *g_pmu = NULL;
STATIC PerfCB g_perfCb = {0};
STATIC PerfEvent g_template;                /* set by LOS_PerfConfig, use as prototype */
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
STATIC OS_PERF_RES_DEF(g_taskEvents, PerfEvent, LOSCFG_BASE_CORE_TSK_LIMIT);    /* used in per-task mode */
#else
STATIC OS_PERF_RES_DEF(g_cpuEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);         /* used in per-cpu mode */
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
STATIC OS_PERF_RES_DEF(g_irqEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);         /* used in interrupt */
#endif

LITE_OS_SEC_BSS SPIN_LOCK_INIT(g_perfSpin);
#define PERF_LOCK(state)       LOS_SpinLockSave(&g_perfSpin, &(state))
#define PERF_UNLOCK(state)     LOS_SpinUnlockRestore(&g_perfSpin, (state))

#ifdef LOSCFG_KERNEL_SMP
STATIC Atomic g_perfSyncCnt;
#endif

STATIC INLINE VOID OsPerfSyncClear(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    LOS_AtomicSet(&g_perfSyncCnt, 0);
#endif
}

STATIC INLINE VOID OsPerfSyncRelease(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    LOS_AtomicInc(&g_perfSyncCnt);
#endif
}

STATIC INLINE VOID OsPerfSyncPend(VOID)
{
#ifdef LOSCFG_KERNEL_SMP
    while (LOS_AtomicRead(&g_perfSyncCnt) != LOSCFG_KERNEL_CORE_NUM) {
        ;
    }
#endif
}

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
PerfEvent *OsPerfTaskEventsGet(UINT32 taskId)
{
    LOS_ASSERT(taskId < LOSCFG_BASE_CORE_TSK_LIMIT);
    return &g_taskEvents[taskId];
}
#else
PerfEvent *OsPerfCpuEventsGet(UINT32 cpuId)
{
    LOS_ASSERT(cpuId < LOSCFG_KERNEL_CORE_NUM);
    return &g_cpuEvents[cpuId];
}
#endif

#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
PerfEvent *OsPerfIrqEventsGet(UINT32 cpuId)
{
    LOS_ASSERT(cpuId < LOSCFG_KERNEL_CORE_NUM);
    return &g_irqEvents[cpuId];
}
#endif

STATIC INLINE UINT64 OsPerfGetCurrTime(VOID)
{
#ifdef LOSCFG_PERF_CALC_TIME_BY_TICK
    return LOS_TickCountGet();
#else
    return HalClockGetCycles();
#endif
}

STATIC UINT32 OsPmuInit(VOID)
{
#ifdef LOSCFG_PERF_HW_PMU
    if (OsHwPmuInit() != LOS_OK) {
        return LOS_ERRNO_PERF_HW_INIT_ERROR;
    }
#endif

#ifdef LOSCFG_PERF_TIMED_PMU
    if (OsTimedPmuInit() != LOS_OK) {
        return LOS_ERRNO_PERF_TIMED_INIT_ERROR;
    }
#endif

#ifdef LOSCFG_PERF_SW_PMU
    if (OsSwPmuInit() != LOS_OK) {
        return LOS_ERRNO_PERF_SW_INIT_ERROR;
    }
#endif
    return LOS_OK;
}

STATIC VOID OsPmuDeinit(VOID)
{
#ifdef LOSCFG_PERF_SW_PMU
    OsSwPmuDeinit();
#endif
#ifdef LOSCFG_PERF_TIMED_PMU
    OsTimedPmuDeinit();
#endif
#ifdef LOSCFG_PERF_HW_PMU
    OsHwPmuDeinit();
#endif
}

STATIC UINT32 OsPerfConfig(PerfEventConfig *eventsCfg)
{
    UINT32 ret;
    PerfEvent *events = &g_template;

    g_pmu = OsPerfPmuGet(eventsCfg->type);
    if (g_pmu == NULL) {
        PRINT_ERR("perf config type error %u!\n", eventsCfg->type);
        return LOS_ERRNO_PERF_INVALID_PMU;
    }

    if (eventsCfg->eventsNr == 0) {
        PRINT_ERR("perf config invalid eventNr.\n");
        return LOS_ERRNO_PERF_PMU_CONFIG_ERROR;
    }

    (VOID)memset(events, 0, sizeof(*events));

    g_pmu->type = eventsCfg->type;
    events->cntDivided = (eventsCfg->predivided == FALSE) ? FALSE : TRUE;

    ret = g_pmu->config(events, eventsCfg);
    if (ret != LOS_OK) {
        PRINT_ERR("perf config failed!\n");
        (VOID)memset(events, 0, sizeof(*events));
        return LOS_ERRNO_PERF_PMU_CONFIG_ERROR;
    }
    return LOS_OK;
}

#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
VOID *OsPerfIrqEnter(UINT32 hwiNum)
{
    UINT32 intSave;
    UINT32 ret;
    PerfEvent *curr = NULL;
    UINT32 cpuId = ArchCurrCpuid();

#ifdef LOSCFG_PERF_HW_PMU
    OS_PMU_INTS(LOSCFG_KERNEL_CORE_NUM, pmuIrqTbl);
    if (hwiNum == pmuIrqTbl[cpuId]) {
        return NULL;
    }
#endif

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STARTED) {
        goto UNLOCK;
    }

    curr = g_pmu->current[cpuId];
    ret = g_pmu->stop(curr, FALSE);
    if (ret != LOS_OK) {
        PRINT_ERR("unexpected %s pmu stop failed\n", __FUNCTION__);
        goto UNLOCK;
    }

    g_pmu->current[cpuId] = OsPerfIrqEventsGet(cpuId);
    ret = g_pmu->start(g_pmu->current[cpuId], FALSE);
    if (ret != LOS_OK) {
        PRINT_ERR("unexpected %s pmu start failed\n", __FUNCTION__);
        goto UNLOCK;
    }

UNLOCK:
    PERF_UNLOCK(intSave);
    return (VOID *)curr;
}

VOID OsPerfIrqExit(UINT32 hwiNum, VOID *state)
{
    (VOID)hwiNum;

    UINT32 ret;
    UINT32 intSave;
    UINT32 cpuId = ArchCurrCpuid();

    if (state == NULL) {
        return;
    }

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STARTED) {
        goto UNLOCK;
    }

    ret = g_pmu->stop(g_pmu->current[cpuId], FALSE);
    if (ret != LOS_OK) {
        PRINT_ERR("unexpected %s pmu stop failed\n", __FUNCTION__);
        goto UNLOCK;
    }

    g_pmu->current[cpuId] = (PerfEvent *)state;
    ret = g_pmu->start(g_pmu->current[cpuId], FALSE);
    if (ret != LOS_OK) {
        PRINT_ERR("unexpected %s pmu start failed\n", __FUNCTION__);
        goto UNLOCK;
    }

UNLOCK:
    PERF_UNLOCK(intSave);
}
#endif

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
VOID OsPerfTaskSwitch(LosTaskCB *runTask, LosTaskCB *newTask)
{
    UINT32 intSave;
    UINT32 cpuId = ArchCurrCpuid();
    PerfEvent *events = NULL;

    PERF_LOCK(intSave);
    if (g_perfCb.pmuStatusPerCpu[cpuId] != PERF_PMU_STARTED) {
        goto UNLOCK;
    }

    events = OsPerfTaskEventsGet(runTask->taskId);
    if (events->nr != 0) {
        (VOID)g_pmu->stop(events, FALSE);
    }

    events = OsPerfTaskEventsGet(newTask->taskId);
    if (events->nr != 0) {
        g_pmu->current[cpuId] = events;
        (VOID)g_pmu->start(g_pmu->current[cpuId], FALSE);
    }

UNLOCK:
    PERF_UNLOCK(intSave);
}

#endif

STATIC VOID OsPerfPrintEvents(PerfEvent *events)
{
    UINT32 index;
    UINT32 eventNum = events->nr;

    for (index = 0; index < eventNum; index++) {
        Event *event = &(events->per[index]);

        /* filter out event counter with no event binded. */
        if (event->period == 0) {
            continue;
        }
        PRINT_EMG("[%s] eventType: 0x%x count: %llu\n", g_pmu->getName(event), event->eventId,
            event->count);
    }
}

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
STATIC INLINE VOID OsPerfPrintTaskBanner(UINT32 taskId, const CHAR *taskName)
{
    PRINT_EMG("perf count [tid %u / %s]:\n", taskId, taskName);
}

STATIC VOID OsPerfPrintPerTask(VOID)
{
    errno_t err;
    UINT32 taskId;
    UINT32 schedIntSave;
    UINT32 perfIntSave;
    LosTaskCB *taskCB = NULL;
    PerfEvent events;
    CHAR taskName[LOS_TASK_NAMELEN] = {0};

    SCHEDULER_LOCK(schedIntSave);
    PERF_LOCK(perfIntSave);
    for (taskId = 0; taskId < KERNEL_TSK_LIMIT; taskId++) {
        taskCB = OS_TCB_FROM_TID(taskId);
        if (taskCB->taskStatus & OS_TASK_IS_EXIT) {
            continue;
        }
        (VOID)memcpy(&events, OsPerfTaskEventsGet(taskId), sizeof(events));
        err = strncpy_s(taskName, sizeof(taskName), taskCB->taskName, LOS_TASK_NAMELEN - 1);
        if (err != EOK) {
            goto UNLOCK;
        }
        PERF_UNLOCK(perfIntSave);
        SCHEDULER_UNLOCK(schedIntSave);

        OsPerfPrintTaskBanner(taskId, taskName);
        OsPerfPrintEvents(&events);

        SCHEDULER_LOCK(schedIntSave);
        PERF_LOCK(perfIntSave);
    }
UNLOCK:
    PERF_UNLOCK(perfIntSave);
    SCHEDULER_UNLOCK(schedIntSave);
}
#else
STATIC INLINE VOID OsPerfPrintCpuBanner(UINT32 cpuId)
{
    PRINT_EMG("perf count [core %u]:\n", cpuId);
}

STATIC VOID OsPerfPrintPerCpu(VOID)
{
    UINT32 cpuId;
    UINT32 intSave;
    PerfEvent event;

    PERF_LOCK(intSave);
    for (cpuId = 0; cpuId < LOSCFG_KERNEL_CORE_NUM; cpuId++) {
        (VOID)memcpy(&event, &g_cpuEvents[cpuId], sizeof(event));
        PERF_UNLOCK(intSave);
        OsPerfPrintCpuBanner(cpuId);
        OsPerfPrintEvents(&event);
        PERF_LOCK(intSave);
    }
    PERF_UNLOCK(intSave);
}
#endif

#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
STATIC INLINE VOID OsPerfPrintIrqBanner(UINT32 cpuId)
{
    PRINT_EMG("perf count [irq@core %u]:\n", cpuId);
}

STATIC VOID OsPerfPrintPerCpuIrq(VOID)
{
    UINT32 cpuId;
    UINT32 intSave;
    PerfEvent event;

    PERF_LOCK(intSave);
    for (cpuId = 0; cpuId < LOSCFG_KERNEL_CORE_NUM; cpuId++) {
        (VOID)memcpy(&event, OsPerfIrqEventsGet(cpuId), sizeof(event));
        PERF_UNLOCK(intSave);
        OsPerfPrintIrqBanner(cpuId);
        OsPerfPrintEvents(&event);
        PERF_LOCK(intSave);
    }
    PERF_UNLOCK(intSave);
}
#endif

STATIC VOID OsPerfPrintCount(VOID)
{
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OsPerfPrintPerTask();
#else
    OsPerfPrintPerCpu();
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    OsPerfPrintPerCpuIrq();
#endif
}

STATIC INLINE VOID OsPerfPrintTs(VOID)
{
#ifdef LOSCFG_PERF_CALC_TIME_BY_TICK
    UINT64 time = (g_perfCb.endTime - g_perfCb.startTime) * OS_SYS_US_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND;
#else
    UINT64 time = (g_perfCb.endTime - g_perfCb.startTime) * OS_SYS_US_PER_SECOND / OS_SYS_CLOCK;
#endif
    PRINT_EMG("time used: %llu(us)\n", time);
}

STATIC VOID OsPerfStart(VOID)
{
    UINT32 ret;
    UINT32 cpuId = ArchCurrCpuid();

    if (g_pmu == NULL) {
        PRINT_ERR("pmu not registered!\n");
        return;
    }

    if (g_perfCb.pmuStatusPerCpu[cpuId] == PERF_PMU_STARTED) {
        PRINT_ERR("percpu status err %d\n", g_perfCb.pmuStatusPerCpu[cpuId]);
        return;
    }
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    g_pmu->current[cpuId] = OsPerfTaskEventsGet(OsCurrTaskGet()->taskId);
#else
    g_pmu->current[cpuId] = &g_cpuEvents[cpuId];
#endif
    ret = g_pmu->start(g_pmu->current[cpuId], TRUE);
    if (ret != LOS_OK) {
        PRINT_ERR("perf start on core:%u failed, ret = 0x%x\n", cpuId, ret);
        return;
    }

    g_perfCb.pmuStatusPerCpu[cpuId] = PERF_PMU_STARTED;

    OsPerfSyncRelease();
}

STATIC VOID OsPerfStop(VOID)
{
    UINT32 ret;
    UINT32 cpuid = ArchCurrCpuid();

    if (g_pmu == NULL) {
        PRINT_ERR("pmu not registered!\n");
        return;
    }

    if (g_perfCb.pmuStatusPerCpu[cpuid] == PERF_PMU_STOPED) {
        PRINT_ERR("percpu status err %d\n", g_perfCb.pmuStatusPerCpu[cpuid]);
        return;
    }

    ret = g_pmu->stop(g_pmu->current[cpuid], TRUE);
    if (ret != LOS_OK) {
        PRINT_ERR("perf stop on core:%u failed, ret = 0x%x\n", cpuid, ret);
        goto OUT;
    }

    g_perfCb.pmuStatusPerCpu[cpuid] = PERF_PMU_STOPED;

OUT:
    OsPerfSyncRelease();
}

STATIC UINT32 OsPerfBackTrace(UINTPTR *callChain, UINT32 maxDepth, PerfRegs *regs)
{
    UINT32 i;

    UINT32 count = ArchPerfBackTraceGet(regs->fp, regs->pc, callChain, maxDepth);
    PRINT_DEBUG("backtrace depth = %u, fp = 0x%x\n", count, regs->fp);

    for (i = 0; i < count; i++) {
        PRINT_DEBUG("ip[%u]: 0x%x\n", i, callChain[i]);
    }
    return count;
}

STATIC UINT32 OsPerfCollectData(Event *event, PerfSampleData *data, PerfRegs *regs)
{
    size_t size = 0;
    UINT32 depth;
    UINT32 sampleType = g_perfCb.sampleType;
    CHAR *p = (CHAR *)data;

    if (sampleType & PERF_RECORD_CPU) {
        *(UINT32 *)(p + size) = ArchCurrCpuid();
        size += sizeof(data->cpuid);
    }

    if (sampleType & PERF_RECORD_TID) {
        *(UINT32 *)(p + size) = LOS_CurTaskIDGet();
        size += sizeof(data->taskId);
    }

    if (sampleType & PERF_RECORD_TYPE) {
        *(UINT32 *)(p + size) = event->eventId;
        size += sizeof(data->eventId);
    }

    if (sampleType & PERF_RECORD_PERIOD) {
        *(UINT32 *)(p + size) = event->period;
        size += sizeof(data->period);
    }

    if (sampleType & PERF_RECORD_TIMESTAMP) {
        *(UINT64 *)(p + size) = OsPerfGetCurrTime();
        size += sizeof(data->time);
    }

    if (sampleType & PERF_RECORD_IP) {
        *(UINTPTR *)(p + size) = regs->pc;
        size += sizeof(data->pc);
    }

    if (sampleType & PERF_RECORD_CALLCHAIN) {
        depth = OsPerfBackTrace((UINTPTR *)(p + size + sizeof(data->callChain.ipNr)), PERF_MAX_CALLCHAIN_DEPTH, regs);
        *(UINT32 *)(p + size) = depth;
        size += sizeof(data->callChain.ipNr) + depth * sizeof(data->callChain.ip[0]);
    }

    return (UINT32)size;
}

/*
 * return TRUE if the taskId in the taskId list, return FALSE otherwise;
 * return TRUE if user haven't specified any taskId(which is supposed
 * to instrument the whole system)
 */
STATIC BOOL OsPerfTaskFilter(UINT32 taskId)
{
    UINT32 i;

    if (!g_perfCb.taskIdsNr) {
        return TRUE;
    }

    for (i = 0; i < g_perfCb.taskIdsNr; i++) {
        if (g_perfCb.taskIds[i] == taskId) {
            return TRUE;
        }
    }
    return FALSE;
}

STATIC INLINE UINT32 OsPerfParamValid(VOID)
{
    UINT32 index;
    UINT32 res = 0;

    if (g_pmu == NULL) {
        return 0;
    }
    PerfEvent *events = &g_template;
    UINT32 eventNum = events->nr;

    for (index = 0; index < eventNum; index++) {
        res |= events->per[index].period;
    }
    return res;
}

STATIC UINT32 OsPerfHdrInit(UINT32 id)
{
    PerfDataHdr head = {
        .magic      = PERF_DATA_MAGIC_WORD,
        .sampleType = g_perfCb.sampleType,
        .sectionId  = id,
        .eventType  = g_pmu->type,
        .len        = sizeof(PerfDataHdr),
    };
    return OsPerfOutPutWrite((CHAR *)&head, head.len);
}

VOID OsPerfUpdateEventCount(Event *event, UINT32 value)
{
    if (event == NULL) {
        return;
    }
    event->count += (value & 0xFFFFFFFF); /* event->count is UINT64 */
}

VOID OsPerfHandleOverFlow(Event *event, PerfRegs *regs)
{
    PerfSampleData data = {0};
    UINT32 len;
    UINT32 intSave;

    if ((g_perfCb.needSample == TRUE) && OsPerfTaskFilter(LOS_CurTaskIDGet())) {
        /*
         * Block smp calls. Assuming other core trying to stop perf here, the status check below may
         * success and continue running, but since we blocked interrupt, the other core shall wait for
         * us, so the resources still accessible, we can safely use it.
         */
        len = OsPerfCollectData(event, &data, regs);
        intSave = LOS_IntLock();
        if (g_perfCb.status != PERF_UNINIT) {
            (VOID)OsPerfOutPutWrite((CHAR *)&data, len);
        }
        LOS_IntRestore(intSave);
    }
}

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
VOID OsPerfTaskCreate(LosTaskCB *taskCB)
{
    UINT32 intSave;

    PERF_LOCK(intSave);
    if ((g_perfCb.status == PERF_STARTED) || (g_perfCb.status == PERF_STARTING)) {
        PerfEvent *events = OsPerfTaskEventsGet(taskCB->taskId);
        (VOID)memcpy(events, &g_template, sizeof(*events));
    }
    PERF_UNLOCK(intSave);
}

VOID OsPerfTaskDelete(LosTaskCB *taskCB)
{
    UINT32 intSave;

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_UNINIT) {
        PerfEvent *events = OsPerfTaskEventsGet(taskCB->taskId);
        (VOID)memset(events, 0, sizeof(*events));
    }
    PERF_UNLOCK(intSave);
}
#endif

STATIC UINT32 OsPerfResourceInit(VOID)
{
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OS_PERF_RES_ALLOC(g_taskEvents, PerfEvent, LOSCFG_BASE_CORE_TSK_LIMIT);
    if (g_taskEvents == NULL) {
        return LOS_ERRNO_PERF_INIT_NO_MEMERY;
    }
#else
    OS_PERF_RES_ALLOC(g_cpuEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);
    if (g_cpuEvents == NULL) {
        return LOS_ERRNO_PERF_INIT_NO_MEMERY;
    }
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    OS_PERF_RES_ALLOC(g_irqEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);
    if (g_irqEvents == NULL) {
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
        OS_PERF_RES_FREE(g_taskEvents, PerfEvent, LOSCFG_BASE_CORE_TSK_LIMIT);
#else
        OS_PERF_RES_FREE(g_cpuEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);
#endif
        return LOS_ERRNO_PERF_INIT_NO_MEMERY;
    }
#endif
    return LOS_OK;
}

STATIC VOID OsPerfResourceDeinit(VOID)
{
    g_pmu = NULL;
    (VOID)memset(&g_perfCb, 0, sizeof(g_perfCb));
    (VOID)memset(&g_template, 0, sizeof(g_template));
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OS_PERF_RES_FREE(g_taskEvents, PerfEvent, LOSCFG_BASE_CORE_TSK_LIMIT);
#else
    OS_PERF_RES_FREE(g_cpuEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    OS_PERF_RES_FREE(g_irqEvents, PerfEvent, LOSCFG_KERNEL_CORE_NUM);
#endif
    g_perfCb.status = PERF_UNINIT;
}

UINT32 LOS_PerfInit(VOID *buf, UINT32 size)
{
    UINT32 ret;
    UINT32 intSave;

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_UNINIT) {
        ret = LOS_ERRNO_PERF_STATUS_INVALID;
        goto UNLOCK;
    }

    ret = OsPerfResourceInit();
    if (ret != LOS_OK) {
        goto UNLOCK;
    }

    ret = OsPerfOutPutInit(buf, size);
    if (ret != LOS_OK) {
        ret = LOS_ERRNO_PERF_BUF_ERROR;
        goto DEINIT_RES;
    }

    ret = OsPmuInit();
    if (ret != LOS_OK) {
        goto DEINIT_ALL;
    }
    g_perfCb.status = PERF_STOPED;

    PERF_UNLOCK(intSave);
    return LOS_OK;

DEINIT_ALL:
    OsPerfOutPutDeinit();
DEINIT_RES:
    OsPerfResourceDeinit();
UNLOCK:
    PERF_UNLOCK(intSave);
    return ret;
}

UINT32 LOS_PerfConfig(PerfConfigAttr *attr)
{
    UINT32 ret;
    UINT32 intSave;

    if (attr == NULL) {
        return LOS_ERRNO_PERF_CONFIG_NULL;
    }

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STOPED) {
        ret = LOS_ERRNO_PERF_STATUS_INVALID;
        PRINT_ERR("perf config status error : 0x%x\n", g_perfCb.status);
        goto PERF_CONFIG_ERROR;
    }

    g_pmu = NULL;

    g_perfCb.needSample = (attr->needSample == FALSE) ? FALSE : TRUE;
    g_perfCb.taskFilterEnable = (attr->taskFilterEnable == FALSE) ? FALSE : TRUE;
    g_perfCb.sampleType = attr->sampleType;

    if (attr->taskFilterEnable != FALSE) {
        g_perfCb.taskIdsNr = (UINT8)OS_MIN(attr->taskIdsNr, PERF_MAX_FILTER_TSKS);
        if (memcpy_s(g_perfCb.taskIds, PERF_MAX_FILTER_TSKS * sizeof(UINT32), attr->taskIds,
                     g_perfCb.taskIdsNr * sizeof(UINT32)) != EOK) {
            ret = LOS_ERRNO_PERF_CONFIG_TASK_FILTER_ERROR;
            PRINT_ERR("In %s At line:%d execute memcpy_s error\n", __FUNCTION__, __LINE__);
            goto PERF_CONFIG_ERROR;
        }
    }
    ret = OsPerfConfig(&attr->eventsCfg);
PERF_CONFIG_ERROR:
    PERF_UNLOCK(intSave);
    return ret;
}

STATIC VOID OsPerfEventAttach(VOID)
{
    UINT32 idx;
    PerfEvent *events = NULL;

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    for (idx = 0; idx < KERNEL_TSK_LIMIT; idx++) {
        events = OsPerfTaskEventsGet(idx);
        (VOID)memcpy(events, &g_template, sizeof(PerfEvent));
    }
#else
    for (idx = 0; idx < LOSCFG_KERNEL_CORE_NUM; idx++) {
        events = &g_cpuEvents[idx];
        (VOID)memcpy(events, &g_template, sizeof(PerfEvent));
    }
#endif
#ifdef LOSCFG_KERNEL_PERF_SEPARATED_IRQ
    for (idx = 0; idx < LOSCFG_KERNEL_CORE_NUM; idx++) {
        events = OsPerfIrqEventsGet(idx);
        (VOID)memcpy(events, &g_template, sizeof(PerfEvent));
    }
#endif
}

VOID LOS_PerfStart(UINT32 sectionId)
{
    UINT32 intSave;
    UINT32 ret;

    if (OS_INT_ACTIVE) {
        PRINT_ERR("perf start cannot use in interrupt\n");
        return;
    }

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STOPED) {
        PRINT_ERR("perf start status error : 0x%x\n", g_perfCb.status);
        goto PERF_START_ERROR;
    }

    if (!OsPerfParamValid()) {
        PRINT_ERR("forgot call `LOS_Config(...)` before instrumenting?\n");
        goto PERF_START_ERROR;
    }

    if (g_perfCb.needSample == TRUE) {
        ret = OsPerfHdrInit(sectionId); /* section header init */
        if (ret != LOS_OK) {
            PRINT_ERR("perf hdr init error 0x%x\n", ret);
            goto PERF_START_ERROR;
        }
    }

    OsPerfEventAttach();

    g_perfCb.status = PERF_STARTING;

#ifdef LOSCFG_KERNEL_SMP
    PERF_UNLOCK(intSave);
#endif
    OsPerfSyncClear();
    SMP_CALL_PERF_FUNC(OsPerfStart); /* send to all cpu to start pmu */
    OsPerfSyncPend();
#ifdef LOSCFG_KERNEL_SMP
    PERF_LOCK(intSave);
#endif

    g_perfCb.status = PERF_STARTED;
    g_perfCb.startTime = OsPerfGetCurrTime();
PERF_START_ERROR:
    PERF_UNLOCK(intSave);
    return;
}

VOID LOS_PerfStop(VOID)
{
    UINT32 intSave;

    if (OS_INT_ACTIVE) {
        PRINT_ERR("perf stop cannot use in interrupt\n");
        return;
    }

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STARTED) {
        PRINT_ERR("perf stop status error : 0x%x\n", g_perfCb.status);
        goto PERF_STOP_ERROR;
    }
    g_perfCb.status = PERF_STOPPING;

#ifdef LOSCFG_KERNEL_SMP
    PERF_UNLOCK(intSave);
#endif
    OsPerfSyncClear();
    SMP_CALL_PERF_FUNC(OsPerfStop); /* send to all cpu to stop pmu */
    OsPerfSyncPend();
#ifdef LOSCFG_KERNEL_SMP
    PERF_LOCK(intSave);
#endif

    OsPerfOutPutFlush();

    PERF_UNLOCK(intSave);
    if (g_perfCb.needSample == FALSE) {
        OsPerfPrintCount();
    }
    PERF_LOCK(intSave);
    g_perfCb.status = PERF_STOPED;
    g_perfCb.endTime = OsPerfGetCurrTime();
    OsPerfPrintTs();

PERF_STOP_ERROR:
    PERF_UNLOCK(intSave);
    return;
}

UINT32 LOS_PerfDataRead(CHAR *dest, UINT32 size)
{
    return OsPerfOutPutRead(dest, size);
}

VOID LOS_PerfNotifyHookReg(const PERF_BUF_NOTIFY_HOOK func)
{
    OsPerfNotifyHookReg(func);
}

VOID LOS_PerfFlushHookReg(const PERF_BUF_FLUSH_HOOK func)
{
    OsPerfFlushHookReg(func);
}

VOID OsPerfSetIrqRegs(UINTPTR pc, UINTPTR fp)
{
    LosTaskCB *runTask = (LosTaskCB *)ArchCurrTaskGet();
    runTask->pc = pc;
    runTask->fp = fp;
}

UINT32 LOS_PerfDeinit(VOID)
{
    UINT32 ret = LOS_OK;
    UINT32 intSave;

    PERF_LOCK(intSave);
    if (g_perfCb.status != PERF_STOPED) {
        ret = LOS_ERRNO_PERF_STATUS_INVALID;
        goto OUT;
    }
    OsPerfOutPutDeinit();
    OsPmuDeinit();
    OsPerfResourceDeinit();

OUT:
    PERF_UNLOCK(intSave);
    return ret;
}
