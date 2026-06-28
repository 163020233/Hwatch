/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: SMP scheduler base on Per-CPU priority queue
 * Author: Huawei LiteOS Team
 * Create: 2022-06-01
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
 * --------------------------------------------------------------------------- */

#include "los_init.h"
#include "los_trace.h"
#include "los_swtmr.h"
#include "los_task_pri.h"
#include "los_priqueue_pri.h"
#include "los_percpu_pri.h"
#include "los_mux_debug_pri.h"
#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
#include "los_perf_pri.h"
#endif

#define OS_SCHED_MIGRATION_PERIOD   1000 /* 1000ms */

/* Per-cpu priority queue */
PriQueue g_priQueue[LOSCFG_KERNEL_CORE_NUM];
/* Active cpu mask to indicate which cpus are idled */
STATIC UINT32 g_activeCpu = LOSCFG_KERNEL_CPU_MASK;

/*
 * Load Balance Abstract Layer.
 * Each load calculation algorithm should implement the following functions.
 */
STATIC VOID OsSchedLoadRefresh(VOID);
STATIC UINT32 OsSchedCurrLoadGet(VOID);
STATIC UINT32 OsSchedLoadGetByCpuid(UINT32 cpuId);
STATIC UINT32 OsSchedLoadGetByTask(LosTaskCB *task);
STATIC BOOL OsSchedIsInbalanced(UINT32 thisLoad, UINT32 maxLoad);
STATIC BOOL OsSchedIfTaskCanMigrate(UINT32 thisLoad, UINT32 maxLoad, UINT32 taskLoad);
STATIC VOID OsSchedLoadUpdate(UINT32 thisCpu, UINT32 maxCpu, UINT32 taskLoad);

STATIC INLINE UINT32 OsMpIdleMask(VOID)
{
    return ((~g_activeCpu) & LOSCFG_KERNEL_CPU_MASK);
}

STATIC INLINE VOID OsMpSetIdle(UINT32 cpu)
{
    g_activeCpu &= ~CPUID_TO_AFFI_MASK(cpu);
}

STATIC INLINE VOID OsMpSetActive(UINT32 cpu)
{
    g_activeCpu |= CPUID_TO_AFFI_MASK(cpu);
}

STATIC INLINE VOID OsSchedTaskCntInc(PriQueue *priQueue)
{
    priQueue->readyTasks++;
}

STATIC INLINE VOID OsSchedTaskCntDec(PriQueue *priQueue)
{
    priQueue->readyTasks--;

    LOS_ASSERT(priQueue->readyTasks >= 0);
}

/* This function is only used in OsStart. */
VOID OsSchedLock(VOID)
{
    LOS_ASSERT(ArchIntLocked());

    LosTaskCB *runTask = ArchCurrTaskGet();
    PriQueue *currPriQueue = &g_priQueue[ArchCurrCpuid()];

    runTask->taskExtStatus = OS_TASK_STATUS_SCHED;
    currPriQueue->prevTask = runTask;
    LOS_SpinLock(&currPriQueue->lock);
}

/* This function is only used in OsTaskEntry. */
VOID OsSchedUnlock(VOID)
{
    LOS_ASSERT(ArchIntLocked());

    PriQueue *currPriQueue = &g_priQueue[ArchCurrCpuid()];
    currPriQueue->prevTask->taskExtStatus &= ~OS_TASK_STATUS_SCHED;
    LOS_SpinUnlockNoSched(&currPriQueue->lock);
}

SPIN_LOCK_S *OsSchedLockByTask(LosTaskCB *task)
{
    PriQueue *priQueue = NULL;

    /*
     * When creating a new task, the task obtains the percpu lock before running,
     * in this case, currCpu is invalid.
     */
    if (task->currCpu == OS_TASK_INVALID_CPUID) {
        task->currCpu = OsSchedPickCpu(task);
    }

    while (1) {
        priQueue = &g_priQueue[task->currCpu];
        OsSpinLockRaw(&priQueue->lock);
        if (priQueue == &g_priQueue[task->currCpu]) {
            break;
        }
        OsSpinUnlockRaw(&priQueue->lock);
    }

    return (&priQueue->lock);
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE VOID OsPriQueueInit(VOID)
{
    LOS_DL_LIST *priQueues = NULL;
    UINT32 priority;
    UINT32 cpuId = 0;

    (VOID)memset(g_priQueue, 0, sizeof(PriQueue) * LOSCFG_KERNEL_CORE_NUM);

    while (cpuId < LOSCFG_KERNEL_CORE_NUM) {
        priority = 0;
        priQueues = g_priQueue[cpuId++].queues;

        while (priority < OS_PRIORITY_QUEUE_NUM) {
            LOS_ListInit(&priQueues[priority++]);
        }
    }
}

/*
 * CPU picking strategy:
 * 1. If binded on one core, just return the cpu.
 * 2. Not first run, choose last ever ran cpu.
 * 3. There's idled core, choose from them.
 * 4. Choose the affinity's smallest cpu, and let the migrator do the work.
 */
UINT32 OsSchedPickCpu(LosTaskCB *task)
{
    UINT32 cpuId, cpuidMask, affinity;
    UINT32 idleMask = OsMpIdleMask();

    affinity  = (UINT32)(task->cpuAffiMask);
    cpuId     = CTZ(affinity);
    cpuidMask = CPUID_TO_AFFI_MASK(cpuId);
    if (!(affinity & ~cpuidMask)) {
        return cpuId;
    }

    if ((task->lastCpu != OS_TASK_INVALID_CPUID) &&
        (CPUID_TO_AFFI_MASK(task->lastCpu) & affinity)) {
        return task->lastCpu;
    }

    /* Mask with affinity */
    idleMask &= affinity;
    if (idleMask) {
        cpuId = CTZ(idleMask);
    }

    return cpuId;
}

STATIC VOID OsPriQueueAdd(PriQueue *priQueue, LosTaskCB *task, PriQueueHeadTail mode)
{
    LOS_ASSERT(LOS_SpinHeld(&priQueue->lock));

    LOS_DL_LIST *priQueues = NULL;
    UINT32 *bitmap = NULL;
    UINT32 priority;

    priQueues = priQueue->queues;
    bitmap    = &priQueue->bitmap;
    priority  = task->priority;

    (*bitmap) |= PRIQUEUE_PRIOR0_BIT >> priority;

    if (mode == PRI_QUEUE_HEAD) {
        LOS_ListHeadInsert(&priQueues[priority], &task->pendList);
    } else {
        LOS_ListTailInsert(&priQueues[priority], &task->pendList);
    }

    OsSchedTaskCntInc(priQueue);
}

STATIC VOID OsPriQueueRem(PriQueue *priQueue, LosTaskCB *task)
{
    LOS_ASSERT(LOS_SpinHeld(&priQueue->lock));

    LOS_DL_LIST *priQueues = NULL;
    UINT32 *bitmap = NULL;
    UINT32 priority;

    priQueues = priQueue->queues;
    bitmap    = &priQueue->bitmap;
    priority  = task->priority;

    LOS_ListDelete(&task->pendList);

    if (LOS_ListEmpty(&priQueues[priority])) {
        (*bitmap) &= ~(PRIQUEUE_PRIOR0_BIT >> priority);
    }

    OsSchedTaskCntDec(priQueue);
}

VOID OsPriQueueSimpleEnqueue(LOS_DL_LIST *queueNode, PriQueueHeadTail mode)
{
    LosTaskCB *task = NULL;

    task = LOS_DL_LIST_ENTRY(queueNode, LosTaskCB, pendList);

    LOS_ASSERT(LOS_SpinHeld(&g_priQueue[task->currCpu].lock));
    OsPriQueueAdd(&g_priQueue[task->currCpu], task, mode);
}

VOID OsPriQueueDequeue(LOS_DL_LIST *queueNode)
{
    LosTaskCB *task = NULL;
    UINT32 cpuId;

    task = LOS_DL_LIST_ENTRY(queueNode, LosTaskCB, pendList);

    LOS_ASSERT(task->currCpu != OS_TASK_INVALID_CPUID);
    LOS_ASSERT(task->currCpu < LOSCFG_KERNEL_CORE_NUM);

    cpuId = task->currCpu;
    LOS_ASSERT(LOS_SpinHeld(&g_priQueue[cpuId].lock));

    OsPriQueueRem(&g_priQueue[cpuId], task);
}

VOID OsPriQueueEnqueueProtect(LOS_DL_LIST *queueNode, UINT32 priority, PriQueueHeadTail mode)
{
    LosTaskCB *task = NULL;
    SPIN_LOCK_S *lock = NULL;

    task = LOS_DL_LIST_ENTRY(queueNode, LosTaskCB, pendList);

    /* Choose an appropriate cpu to run */
    task->currCpu = OsSchedPickCpu(task);

    lock = OsSchedLockByTask(task);

    /* Add the task to the chosen cpu's pri-queue */
    OsPriQueueAdd(&g_priQueue[task->currCpu], task, mode);
    OsSpinUnlockRaw(lock);
}

/*
 * If the queue has been taken out by other operations, returns True to tell the calling function to
 * re-execute. Otherwise, returns False for the calling function to continue execution.
 */
BOOL OsPriQueueDequeueProtect(LOS_DL_LIST *queueNode)
{
    LosTaskCB *task = NULL;
    BOOL done = TRUE;
    SPIN_LOCK_S *lock = NULL;

    task = LOS_DL_LIST_ENTRY(queueNode, LosTaskCB, pendList);

    LOS_ASSERT(task->currCpu != OS_TASK_INVALID_CPUID);
    LOS_ASSERT(task->currCpu < LOSCFG_KERNEL_CORE_NUM);

    lock = OsSchedLockByTask(task);

    if (!(task->taskStatus & OS_TASK_STATUS_READY)) {
        /* Task status changed */
        done = FALSE;
        goto OUT;
    }

    OsPriQueueRem(&g_priQueue[task->currCpu], task);

OUT:
    OsSpinUnlockRaw(lock);
    return done;
}

LosTaskCB *OsGetTopTaskByPriQueue(PriQueue *priQueue, UINT32 cpuId)
{
    LosTaskCB *newTask = NULL;
    LOS_DL_LIST *priQueues = NULL;
    UINT32 priority;
    UINT32 bitmap;

    priQueues = priQueue->queues;
    bitmap    = priQueue->bitmap;

    while (bitmap) {
        priority = CLZ(bitmap);

        LOS_DL_LIST_FOR_EACH_ENTRY(newTask, &priQueues[priority], LosTaskCB, pendList) {
            /*
             * If a task has been added to priqueue on other cores and the scheduling is not complete,
             * the task cannot participate in the scheduling of the local core.
             */
            if ((newTask->taskExtStatus & OS_TASK_STATUS_SCHED) &&
                (newTask->lastCpu != newTask->currCpu)) {
                continue;
            }

            if (newTask->taskExtStatus & OS_TASK_STATUS_DEQUE) {
                continue;
            }

            if (newTask->cpuAffiMask & (1U << cpuId)) {
                OsPriQueueRem(priQueue, newTask);
                goto OUT;
            }
        }

        bitmap &= ~(1U << (OS_PRIORITY_QUEUE_NUM - priority - 1));
    }

OUT:
    newTask->lastCpu = cpuId;
    return newTask;
}

VOID OsSchedResched(VOID)
{
    LosTaskCB *runTask = NULL;
    LosTaskCB *newTask = NULL;
    UINT32 cpuId = ArchCurrCpuid();
    PriQueue *currPriQueue = &g_priQueue[cpuId];

    LOS_ASSERT(LOS_SpinHeld(&currPriQueue->lock));

    if (!OsPreemptableInSched()) {
        return;
    }

    runTask = OsCurrTaskGet();
    newTask = OsGetTopTaskByPriQueue(currPriQueue, cpuId);

    /* Always be able to get one task */
    LOS_ASSERT(newTask != NULL);

    /* Ensure that the task status does not change to empty during scheduling. */
    newTask->taskStatus |= OS_TASK_STATUS_RUNNING;
    newTask->taskStatus &= ~OS_TASK_STATUS_READY;

    if (runTask == newTask) {
        runTask->taskExtStatus &= ~OS_TASK_STATUS_SCHED;
        return;
    }

    LOS_TRACE(TASK_INSCHED, runTask->taskId, newTask->taskId);

    currPriQueue->prevTask = runTask;
    runTask->taskStatus &= ~OS_TASK_STATUS_RUNNING;

#ifdef LOSCFG_KERNEL_SMP
    /* Mark new task's owner processor */
    newTask->currCpu = cpuId;
    newTask->lastCpu = cpuId;
#endif

#ifdef LOSCFG_KERNEL_CPUP
    OsTaskCycleEndStart(newTask);
#endif

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OsPerfTaskSwitch(runTask, newTask);
#endif

#ifdef LOSCFG_BASE_CORE_TSK_MONITOR
    OsTaskSwitchCheck(runTask, newTask);
#endif

#ifndef LOSCFG_SCHED_LATENCY
    OsSchedTaskSwitch(newTask, runTask);
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    OsSchedStat(runTask, newTask);
#endif

    if (newTask->taskId == OsPercpuGet()->idleTaskId) {
        OsMpSetIdle(cpuId);
    } else {
        OsMpSetActive(cpuId);
    }

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    if (newTask->timeSlice == 0) {
        newTask->timeSlice = KERNEL_TIMESLICE_TIMEOUT;
    }
#endif

    OsCurrTaskSet((VOID*)newTask);

    /* Do the task context switch */
    ArchTaskSchedule(newTask, runTask);

    currPriQueue = &g_priQueue[ArchCurrCpuid()];
    currPriQueue->prevTask->taskExtStatus &= ~OS_TASK_STATUS_SCHED;
}

VOID OsSchedPreempt(VOID)
{
    LosTaskCB *runTask = NULL;
    UINT32 intSave;
    UINT32 cpuId = ArchCurrCpuid();
    UINT16 mode = PRI_QUEUE_HEAD;

    if (!OsPreemptable()) {
        return;
    }

    SCHEDULER_LOCK(intSave);

    runTask = OsCurrTaskGet();
    runTask->taskExtStatus = OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_SCHED;

    LOS_ASSERT(runTask->currCpu != OS_TASK_INVALID_CPUID);

    /* Affinity has been changed. Need to find a new cpu. */
    if (!(CPUID_TO_AFFI_MASK(runTask->currCpu) & runTask->cpuAffiMask)) {
        cpuId = OsSchedPickCpu(runTask);
        runTask->currCpu = cpuId;
    }
    OsSpinUnlockRaw(&g_taskSpin);

    OsSchedLockTwoRaw(ArchCurrCpuid(), runTask->currCpu);

    runTask->taskStatus |= OS_TASK_STATUS_READY;

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    if (runTask->timeSlice == 0) {
        mode = PRI_QUEUE_TAIL;
    }
#endif
    /* Add run task back to the per-cpu pri-queue. */
    OsPriQueueAdd(&g_priQueue[runTask->currCpu], runTask, mode);

    OsSchedUnlockRemoteRaw(runTask->currCpu);

    runTask->taskExtStatus &= ~OS_TASK_STATUS_ENQUE;
    /* Reschedule to new thread */
    OsSchedResched();
    LOS_SpinUnlockNoSched(&g_priQueue[ArchCurrCpuid()].lock);

    LOS_IntRestore(intSave);
}

VOID OsSchedAffiChange(VOID *task)
{
    LosTaskCB *taskCB = (LosTaskCB *)task;

    while (taskCB->taskStatus & OS_TASK_STATUS_READY) {
        if (OsPriQueueDequeueProtect(&taskCB->pendList) == FALSE) {
            continue;
        }
        OsPriQueueEnqueueProtect(&taskCB->pendList, taskCB->priority, PRI_QUEUE_TAIL);
        break;
    }
}

#ifdef LOSCFG_BASE_CORE_TIMESLICE
VOID OsTimesliceCheck(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();
    if (runTask->timeSlice != 0) {
        runTask->timeSlice--;
        if (runTask->timeSlice == 0) {
            LOS_Schedule();
        }
    }
}
#endif

STATIC BOOL OsSchedCheckIfLeastBusy(UINT32 thisCpu, UINT32 thisLoad)
{
    UINT32 search = 0;
    BOOL leastBusy = TRUE;

    for (; search < LOSCFG_KERNEL_CORE_NUM; search++) {
        if (search == thisCpu) {
            continue;
        }

        if (OsSchedLoadGetByCpuid(search) < thisLoad) {
            leastBusy = FALSE;
            break;
        }
    }

    return leastBusy;
}

STATIC UINT32 OsSchedMostBusyCpuGet(UINT32 thisCpu, UINT32 *maxLoad)
{
    UINT32 search = 0;
    UINT32 maxCpu, tmpLoad;

    maxCpu = thisCpu;
    for (; search < LOSCFG_KERNEL_CORE_NUM; search++) {
        if (search == thisCpu) {
            continue;
        }

        tmpLoad = OsSchedLoadGetByCpuid(search);
        if (tmpLoad > *maxLoad) {
            *maxLoad = tmpLoad;
            maxCpu = search;
        }
    }

    return maxCpu;
}

STATIC VOID OsSchedDoPullTask(UINT32 thisCpu, UINT32 maxCpu, UINT32 thisLoad, UINT32 maxLoad)
{
    UINT32 priority, bitmap, taskLoad;
    PriQueue *thisQueue = NULL;
    PriQueue *maxQueue = NULL;
    LosTaskCB *pullTask = NULL;
    LosTaskCB *nextTask = NULL;
    LOS_DL_LIST *maxPriQueues = NULL;
    BOOL isInbalanced = TRUE;

    thisQueue    = &g_priQueue[thisCpu];
    maxQueue     = &g_priQueue[maxCpu];
    maxPriQueues = maxQueue->queues;

    PRINT_DEBUG("Load Balance between cpu%u[%u] <- cpu%u[%u]\n", thisCpu, thisLoad, maxCpu, maxLoad);

    LOS_SpinLock(&maxQueue->lock);
    bitmap = maxQueue->bitmap;

    while ((bitmap != 0) && (isInbalanced == TRUE)) {
        priority = CLZ(bitmap);

        LOS_DL_LIST_FOR_EACH_ENTRY_SAFE(pullTask, nextTask, &maxPriQueues[priority], LosTaskCB, pendList) {
            /* Check if can migrate */
            if (!(pullTask->cpuAffiMask & (1U << thisCpu))) {
                continue;
            }

            if (pullTask->taskExtStatus & OS_TASK_STATUS_SCHED) {
                continue;
            }

            taskLoad = OsSchedLoadGetByTask(pullTask);
            if (!(OsSchedIfTaskCanMigrate(thisLoad, maxLoad, taskLoad))) {
                continue;
            }

            PRINT_DEBUG("    pulled task %s[%u] [%u:%u]\n", pullTask->taskName, taskLoad, thisLoad, maxLoad);

            /* Do the migration */
            OsPriQueueRem(maxQueue, pullTask);

            LOS_SpinLock(&thisQueue->lock);
            OsPriQueueAdd(thisQueue, pullTask, PRI_QUEUE_TAIL);
            LOS_SpinUnlockNoSched(&thisQueue->lock);

            pullTask->currCpu = thisCpu;

            thisLoad += taskLoad;
            maxLoad -= taskLoad;
            OsSchedLoadUpdate(thisCpu, maxCpu, taskLoad);

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
            OsSchedStatMgr(pullTask);
#endif

            isInbalanced = OsSchedIsInbalanced(thisLoad, maxLoad);
            if (isInbalanced != TRUE) {
                break;
            }
        }

        bitmap &= ~(1U << (OS_PRIORITY_QUEUE_NUM - priority - 1));
    }

    LOS_SpinUnlockNoSched(&maxQueue->lock);
}

VOID OsSchedLoadBalanceLocal(VOID)
{
    UINT32 thisCpu, maxCpu;
    UINT32 thisLoad, maxLoad;
    UINT32 intSave;
    BOOL isInbalanced = FALSE;

    SCHEDULER_LOCK(intSave);

    OsSchedLoadRefresh();

    thisCpu = ArchCurrCpuid();
    thisLoad = OsSchedCurrLoadGet();
    if (!OsSchedCheckIfLeastBusy(thisCpu, thisLoad)) {
        goto UNLOCK_OUT;
    }

    maxLoad = thisLoad;
    maxCpu = OsSchedMostBusyCpuGet(thisCpu, &maxLoad);
    if (maxCpu == thisCpu) {
        goto UNLOCK_OUT;
    }

    isInbalanced = OsSchedIsInbalanced(thisLoad, maxLoad);
    if (isInbalanced != TRUE) {
        goto UNLOCK_OUT;
    }

    /* Pull tasks from maxload. */
    OsSchedDoPullTask(thisCpu, maxCpu, thisLoad, maxLoad);

UNLOCK_OUT:
    SCHEDULER_UNLOCK(intSave);
}

/*
 * This function is used to periodic check the balance.
 * If not, then rebalance them by pulling task from the busiest queue.
 */
STATIC VOID OsSchedBalanceTimer(VOID)
{
    OsSchedLoadBalanceLocal();
}

STATIC VOID OsSchedDelegateTask(VOID)
{
    UINT16 swtmrId;
    UINT32 ret;

    ret = LOS_SwtmrCreate(LOS_MS2Tick(OS_SCHED_MIGRATION_PERIOD), LOS_SWTMR_MODE_PERIOD,
                          (SWTMR_PROC_FUNC)OsSchedBalanceTimer, &swtmrId, 0);
    if (ret != LOS_OK) {
        PRINT_ERR("Swtmr Create failed err:0x%x\n", ret);
        return;
    }
    ret = LOS_SwtmrStart(swtmrId);
    if (ret != LOS_OK) {
        PRINT_ERR("Swtmr Start failed err:0x%x\n", ret);
    }
}

STATIC UINT32 OsSchedCreateDelegateTasks(VOID)
{
    TSK_INIT_PARAM_S taskInitParam = {0};
    UINT32 taskId;
    UINT32 ret = 0;
    UINT32 cpu = 0;

    while (cpu < LOSCFG_KERNEL_SMP_CORE_NUM && ret == LOS_OK) {
        taskInitParam.pfnTaskEntry = (TSK_ENTRY_FUNC)OsSchedDelegateTask;
        taskInitParam.uwStackSize = KERNEL_TSK_DEFAULT_STACK_SIZE;
        taskInitParam.pcName = "SchedDelegateTask";
        taskInitParam.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
        taskInitParam.uwResved = LOS_TASK_STATUS_DETACHED;
        taskInitParam.usCpuAffiMask = CPUID_TO_AFFI_MASK(cpu);

        ret = LOS_TaskCreate(&taskId, &taskInitParam);
        cpu++;
    }

    return ret;
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE UINT32 OsSchedInit(VOID)
{
    UINT32 ret;

    ret = OsSchedCreateDelegateTasks();
    if (ret != LOS_OK) {
        PRINT_ERR("Scheduler delegate task create failed. ret : 0x%x\n", ret);
        return ret;
    }

    PRINTK("Scheduler MQ initialized.\n");

    return LOS_OK;
}
LOS_SYS_INIT(OsSchedInit, SYS_INIT_LEVEL_KERNEL, SYS_INIT_SYNC_3);

/*
 * Simple load balance algorithm:
 * This algorithm uses counts of ready tasks as cpu load.
 * Each task's load is counted as 1.
 */
#ifdef LOSCFG_SCHED_LOAD_BALANCE_SIMPLE

STATIC UINT32 OsSchedCurrLoadGet(VOID)
{
    UINT32 readyTasks = (UINT32)g_priQueue[ArchCurrCpuid()].readyTasks;

    /*
     * If load balance is handled in swtmr tasks, we need to eliminate
     * the swtmr task itself, then it's fair when compared with an idle cpu.
     */
#ifndef LOSCFG_BASE_CORE_SWTMR_IN_ISR
    readyTasks -= 1;
#endif
    return readyTasks;
}

STATIC UINT32 OsSchedLoadGetByCpuid(UINT32 cpuId)
{
    return (UINT32)g_priQueue[cpuId].readyTasks;
}

STATIC UINT32 OsSchedLoadGetByTask(LosTaskCB *task)
{
    (VOID)task;

    return 1;
}

STATIC BOOL OsSchedIsInbalanced(UINT32 thisLoad, UINT32 maxLoad)
{
    UINT32 imbalance;

    if (maxLoad <= thisLoad) {
        return FALSE;
    }

    imbalance = maxLoad - thisLoad;
    /*
     * Check if there's enough imbalance:
     * 1. imbalance is at least 25% of maxLoad
     * 2. imbalance is over than 2 tasks
     */
    if (((imbalance * 4) < maxLoad) || (imbalance < 2)) {
        return FALSE;
    }

    return TRUE;
}

STATIC VOID OsSchedLoadRefresh(VOID)
{
}

STATIC VOID OsSchedLoadUpdate(UINT32 thisCpu, UINT32 maxCpu, UINT32 taskLoad)
{
    (VOID)thisCpu;
    (VOID)maxCpu;
    (VOID)taskLoad;
}

STATIC BOOL OsSchedIfTaskCanMigrate(UINT32 thisLoad, UINT32 maxLoad, UINT32 taskLoad)
{
    (VOID)thisLoad;
    (VOID)maxLoad;
    (VOID)taskLoad;

    return TRUE;
}

#endif /* LOSCFG_SCHED_LOAD_BALANCE_SIMPLE */

/*
 * CPUP load balance algorithm:
 * This algorithm uses the CPU usage calculated by CPUP as the CPU load.
 * Each task's load is the CPU usage of the task in one second.
 */
#ifdef LOSCFG_SCHED_LOAD_BALANCE_CPUP

#if (OS_SCHED_MIGRATION_PERIOD != 1000)
#error "balance migration period must be 1 second"
#endif

#define OS_SCHED_IMBALANCE_THRESHOLD    200 /* two-core imbalance threshold */
#define OS_SCHED_CPU_OVERLOAD_THRESHOLD 800 /* core overload threshold */

typedef struct {
    UINT32          cpuLoad;            /* dynamic cpu load  */
    UINT32          preCpupLoad;        /* load calculated by CPUP in the last period */
    INT32           loadDiff;           /* load difference in this period */
} SchedStatus;

/* Per-cpu scheduling status */
STATIC SchedStatus g_schedStatus[LOSCFG_KERNEL_CORE_NUM] = {{0}};

/*
 * cpuLoad Update strategy:
 * 1. If no migration occurs in the previous period
 * Formula is:
 *      cpuLoad = currCpupLoad
 *      preCpupLoad = currCpupLoad
 * 2. If migration occurred in the previous period
 * Formula is:
 *      cpuLoad = preCpupLoad + loadDiff
 *      preCpupLoad = currCpupLoad
 */
STATIC VOID OsSchedLoadRefresh(VOID)
{
    UINT32 cpuId;
    UINT32 currCpupLoad[LOSCFG_KERNEL_CORE_NUM] = {0};
    SchedStatus *cpuSchedStatus = NULL;
    BOOL loadNeedUpdate;
    INT32 estimatedLoad;

    /* Update loads of all cores when CPUP enters new period. */
    loadNeedUpdate = OsCpupCurrAllCpuUsage(currCpupLoad);
    if (loadNeedUpdate == FALSE) {
        return;
    }

    for (cpuId = 0; cpuId < LOSCFG_KERNEL_CORE_NUM; cpuId++) {
        cpuSchedStatus = &g_schedStatus[cpuId];

        if (cpuSchedStatus->loadDiff == 0) {
            cpuSchedStatus->cpuLoad = currCpupLoad[cpuId];
        } else {
            estimatedLoad = (INT32)cpuSchedStatus->preCpupLoad + cpuSchedStatus->loadDiff;
            cpuSchedStatus->cpuLoad = (estimatedLoad > 0 ? (UINT32)estimatedLoad : 0);
        }
        cpuSchedStatus->preCpupLoad = currCpupLoad[cpuId];
        cpuSchedStatus->loadDiff = 0;
    }
}

STATIC UINT32 OsSchedCurrLoadGet(VOID)
{
    return g_schedStatus[ArchCurrCpuid()].cpuLoad;
}

STATIC UINT32 OsSchedLoadGetByCpuid(UINT32 cpuId)
{
    return g_schedStatus[cpuId].cpuLoad;
}

STATIC UINT32 OsSchedLoadGetByTask(LosTaskCB *task)
{
    return LOS_HistoryTaskCpuUsage(task->taskId, CPUP_LAST_ONE_SECONDS);
}

STATIC BOOL OsSchedIsInbalanced(UINT32 thisLoad, UINT32 maxLoad)
{
    if (maxLoad <= thisLoad) {
        return FALSE;
    }

    if ((maxLoad - thisLoad) < OS_SCHED_IMBALANCE_THRESHOLD) {
        return FALSE;
    }

    return TRUE;
}

STATIC VOID OsSchedLoadUpdate(UINT32 thisCpu, UINT32 maxCpu, UINT32 taskLoad)
{
    g_schedStatus[thisCpu].cpuLoad += taskLoad;
    g_schedStatus[thisCpu].loadDiff += (INT32)taskLoad;
    g_schedStatus[maxCpu].cpuLoad -= taskLoad;
    g_schedStatus[maxCpu].loadDiff -= (INT32)taskLoad;
}

STATIC BOOL OsSchedIfTaskCanMigrate(UINT32 thisLoad, UINT32 maxLoad, UINT32 taskLoad)
{
    /*
     * Check whether the task can be migrated from the core with the highest load to this core:
     * 1. The value of maxLoad minus thisLoad must be greater than (2 * taskLoad).
     */
    if ((taskLoad * 2) <= (maxLoad - thisLoad)) {
        return TRUE;
    }

    /*
     * 2. If maxLoad is too high, it's allowed to move to more idle CPU.
     */
    if ((maxLoad > OS_SCHED_CPU_OVERLOAD_THRESHOLD) && (taskLoad < maxLoad) &&
        (thisLoad < (maxLoad - taskLoad))) {
        return TRUE;
    }

    return FALSE;
}

#endif /* LOSCFG_SCHED_LOAD_BALANCE_CPUP */

#ifndef LOSCFG_SCHED_LATENCY
/*
 * Description : Process pending schedFlag tagged by others cores
 */
VOID OsSchedProcSchedFlag(VOID)
{
    Percpu *percpu = NULL;

    OsTaskProcSignal();

    /* Check if needs to schedule */
    percpu = OsPercpuGet();

    LOS_TRACE(TASK_SIGNAL, OsCurrTaskGet()->taskId, percpu->schedFlag);

    if (OsPreemptable()) {
        if (percpu->schedFlag == INT_PEND_RESCH) {
            OsSetSchedFlag(INT_NO_RESCH);
            OsSchedPreempt();
        }
#ifndef LOSCFG_KERNEL_SMP
        else if (percpu->schedFlag == INT_SUSPEND_DELETE_RESCH) {
            OsSetSchedFlag(INT_NO_RESCH);
            OsSchedResched();
        }
#endif
    }

    return;
}
#endif

/*
 * Description : pend a task in list
 * Input       : runTask    --- Task to be suspended
 *               list       --- wait task list
 *               timeOut    --- Expiry time
 */
VOID OsSchedWait(LosTaskCB *runTask, LOS_DL_LIST *list, UINT32 timeout)
{
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    runTask->taskStatus |= OS_TASK_STATUS_PEND;
    LOS_ListTailInsert(list, &runTask->pendList);
    if (timeout != LOS_WAIT_FOREVER) {
        runTask->taskStatus |= OS_TASK_STATUS_PEND_TIME;
        OsTaskAdd2TimerList(runTask, timeout);
    }

    runTask->taskExtStatus = OS_TASK_STATUS_SCHED;
    OsSpinUnlockRaw(&g_taskSpin);

    OsSpinLockRaw(&g_priQueue[ArchCurrCpuid()].lock);

    OsSchedResched();

    /*
     * Since the task may be switched to another core,
     * the lock needs to be acquired again through the current cpu id.
     */
    OsSpinUnlockRaw(&g_priQueue[ArchCurrCpuid()].lock);

    OsSpinLockRaw(&g_taskSpin);
}

/*
 * Description : Wakes the task from the pending list and adds the task to the priority queue.
 * Input       : resumedTask --- resumed task
 */
VOID OsSchedWake(LosTaskCB *resumedTask)
{
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    OsTaskWaitCanOperate(resumedTask);

    LOS_ListDelete(&resumedTask->pendList);

    /*
     * Tasks in pending state may have a new affinity set,
     * so the task may be on the previous core's timer list.
     * Moreover, timerList is shared by multiple cores and needs to be protected by a global lock.
     */
    resumedTask->taskStatus &= ~OS_TASK_STATUS_PEND;
    if (resumedTask->taskStatus & OS_TASK_STATUS_PEND_TIME) {
        OsTimerListDelete(resumedTask);
        resumedTask->taskStatus &= ~OS_TASK_STATUS_PEND_TIME;
    }

    resumedTask->taskExtStatus = OS_TASK_STATUS_WAKEUP;
    OsSpinUnlockRaw(&g_taskSpin);

    if ((!(resumedTask->taskStatus & OS_TASK_STATUS_SUSPEND)) &&
        (!(resumedTask->taskStatus & OS_TASK_STATUS_RUNNING))) {
        resumedTask->taskStatus |= OS_TASK_STATUS_READY;
        LOS_TRACE(TASK_WAKE, resumedTask->taskId);
        OsPriQueueEnqueueProtect(&resumedTask->pendList, resumedTask->priority, PRI_QUEUE_TAIL);
    }

    resumedTask->taskExtStatus &= ~OS_TASK_STATUS_WAKEUP;

    OsSpinLockRaw(&g_taskSpin);
}

/*
 * Description : Change task priority.
 * Input       : taskCB    --- task control block
 *               priority  --- priority
 */
VOID OsSchedPrioModify(LosTaskCB *taskCB, UINT16 priority)
{
    BOOL needRetry;

    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    LOS_TRACE(TASK_PRIOSET, taskCB->taskId, taskCB->taskStatus, taskCB->priority, priority);

    do {
        /*
         * By default, the loop is executed only once. If priqueue dequeue fails,
         * we need to re-judge the task status and perform the corresponding operation.
         */
        needRetry = FALSE;
        if ((taskCB->taskExtStatus & OS_TASK_IS_INOPERABLE) != 0) {
            needRetry = TRUE;
            continue;
        }

        if (taskCB->taskStatus & OS_TASK_STATUS_READY) {
            taskCB->taskExtStatus = OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE;
            OsSpinUnlockRaw(&g_taskSpin);

            OsSpinLockRaw(&g_priQueue[taskCB->currCpu].lock);

            if (!(taskCB->taskStatus & OS_TASK_STATUS_READY)) {
                OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
                taskCB->taskExtStatus &= ~(OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE);

                OsSpinLockRaw(&g_taskSpin);
                needRetry = TRUE;
                continue;
            }
            OsPriQueueRem(&g_priQueue[taskCB->currCpu], taskCB);

            taskCB->priority = priority;
            OsPriQueueAdd(&g_priQueue[taskCB->currCpu], taskCB, PRI_QUEUE_TAIL);

            OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
            taskCB->taskExtStatus &= ~(OS_TASK_STATUS_ENQUE | OS_TASK_STATUS_DEQUE);

            OsSpinLockRaw(&g_taskSpin);
        } else {
            taskCB->priority = priority;
        }
    } while (needRetry);

    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));
}

UINT32 OsSchedSuspend(LosTaskCB *taskCB)
{
    UINT32 intSave;
    UINT16 tempStatus;
    UINT32 errRet = LOS_OK;
    BOOL needRetry;

    LOS_TRACE(TASK_SUSPEND, taskCB->taskId, taskCB->taskStatus, OsCurrTaskGet()->taskId);

    SCHEDULER_LOCK(intSave);

    do {
        /*
         * By default, the loop is executed only once.
         * If priqueue dequeue fails, or the task cannot be operated,
         * we need to re-judge the task status and perform the corresponding operation.
         */
        needRetry = FALSE;
        if ((taskCB->taskExtStatus & OS_TASK_IS_INOPERABLE) != 0) {
            needRetry = TRUE;
            continue;
        }

        tempStatus = taskCB->taskStatus;
        if (tempStatus & OS_TASK_STATUS_UNUSED) {
            errRet = LOS_ERRNO_TSK_NOT_CREATED;
            goto LOS_RETURN;
        }

        if (OS_TASK_IS_ZOMBIE(tempStatus)) {
            errRet = LOS_ERRNO_TSK_IS_ZOMBIE;
            goto LOS_RETURN;
        }

        if (tempStatus & OS_TASK_STATUS_SUSPEND) {
            errRet = LOS_ERRNO_TSK_ALREADY_SUSPENDED;
            goto LOS_RETURN;
        }

        if (tempStatus & OS_TASK_STATUS_RUNNING) {
            if (!OsTaskSuspendCheckOnRun(taskCB, &errRet)) {
                goto LOS_RETURN;
            }

            /* self suspend */
            taskCB->taskExtStatus = OS_TASK_STATUS_SCHED;
            OsSpinUnlockRaw(&g_taskSpin);

            OsSpinLockRaw(&g_priQueue[ArchCurrCpuid()].lock);
            taskCB->taskStatus |= OS_TASK_STATUS_SUSPEND;
            OsSchedResched();

            LOS_SpinUnlockNoSched(&g_priQueue[ArchCurrCpuid()].lock);
            LOS_IntRestore(intSave);
            return LOS_OK;
        }

        if (tempStatus & OS_TASK_STATUS_READY) {
            taskCB->taskExtStatus = OS_TASK_STATUS_DEQUE;
            OsSpinUnlockRaw(&g_taskSpin);

            OsSpinLockRaw(&g_priQueue[taskCB->currCpu].lock);

            if (!(taskCB->taskStatus & OS_TASK_STATUS_READY)) {
                OsSpinUnlockRaw(&g_priQueue[taskCB->currCpu].lock);
                taskCB->taskExtStatus &= ~OS_TASK_STATUS_DEQUE;

                OsSpinLockRaw(&g_taskSpin);
                needRetry = TRUE;
                continue;
            }

            OsPriQueueDequeue(&taskCB->pendList);

            LOS_SpinUnlockNoSched(&g_priQueue[taskCB->currCpu].lock);

            taskCB->taskStatus |= OS_TASK_STATUS_SUSPEND;
            taskCB->taskStatus &= ~OS_TASK_STATUS_READY;
            BARRIER();
            taskCB->taskExtStatus &= ~OS_TASK_STATUS_DEQUE;

            LOS_IntRestore(intSave);
            return LOS_OK;
        }
    } while (needRetry);

    taskCB->taskStatus |= OS_TASK_STATUS_SUSPEND;

LOS_RETURN:
    SCHEDULER_UNLOCK(intSave);
    return errRet;
}
