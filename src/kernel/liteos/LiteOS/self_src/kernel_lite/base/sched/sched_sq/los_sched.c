/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: Scheduler
 * Author: Huawei LiteOS Team
 * Create: 2018-08-29
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

#include "los_trace.h"
#include "los_task_pri.h"
#include "los_priqueue_pri.h"
#include "los_percpu_pri.h"
#include "los_task_pri.h"
#include "los_mux_debug_pri.h"
#include "los_cpup_pri.h"
#ifdef LOSCFG_KERNEL_PERF_PER_TASK
#include "los_perf_pri.h"
#endif
#ifdef LOSCFG_WCFS_SCHEDULER
#include "los_cfsqueue_pri.h"
#endif

#ifdef LOSCFG_COMPAT_RT_THREAD
UINT32 g_threadTick[MAX_TASKS];
#endif

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
UINTPTR   g_newTask_topOfStack;
#endif

VOID OsSchedResched(VOID)
{
    LosTaskCB *runTask = NULL;
    LosTaskCB *newTask = NULL;

    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    if (!OsPreemptableInSched()) {
        return;
    }

    runTask = OsCurrTaskGet();
    newTask = OsGetTopTask();

    /* always be able to get one task */
    LOS_ASSERT(runTask != NULL);
    LOS_ASSERT(newTask != NULL);

    newTask->taskStatus &= ~OS_TASK_STATUS_READY;

    if (runTask == newTask) {
        return;
    }

    runTask->taskStatus &= ~OS_TASK_STATUS_RUNNING;
    newTask->taskStatus |= OS_TASK_STATUS_RUNNING;

    LOS_TRACE(TASK_INSCHED, runTask->taskId, newTask->taskId);

#ifdef LOSCFG_KERNEL_SMP
    /* mask new running task's owner processor */
    runTask->lastCpu = runTask->currCpu;
    newTask->currCpu = ArchCurrCpuid();
#endif

    if (g_cpupOps.taskCycleEndStart != NULL) {
        g_cpupOps.taskCycleEndStart(newTask);
    }

#ifdef LOSCFG_KERNEL_PERF_PER_TASK
    OsPerfTaskSwitch(runTask, newTask);
#endif

if (g_stackcheckOps.osTaskSwitchCheck != NULL) {
    g_stackcheckOps.osTaskSwitchCheck(runTask, newTask);
}

#ifndef LOSCFG_SCHED_LATENCY
    OsSchedTaskSwitch(newTask, runTask);
#endif

#ifdef LOSCFG_DEBUG_SCHED_STATISTICS
    OsSchedStat(runTask, newTask);
    SET_CUR_STATE_TIME(newTask);
    SET_CUR_STATE_TIME(runTask);
#ifdef LOSCFG_KERNEL_SMP
    if ((newTask->lastCpu != OS_TASK_INVALID_CPUID) &&
        (newTask->lastCpu != newTask->currCpu)) {
        OsSchedStatMgr(newTask);
    }
#endif
#endif

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    if (newTask->timeSlice == 0) {
        newTask->timeSlice = KERNEL_TIMESLICE_TIMEOUT;
#ifdef LOSCFG_COMPAT_RT_THREAD
        if (g_threadTick[newTask->taskId] != 0) {
            newTask->timeSlice = g_threadTick[newTask->taskId];
        }
#endif
    }
#endif

#ifdef LOSCFG_WCFS_SCHEDULER
    if (newTask->schedPolicy == TASK_POLICY_CFS) {
        newTask->execStart = LOS_TickCountGet();
    }
    if (runTask->schedPolicy == TASK_POLICY_CFS) {
        OsUpgradeVruntime();

        // After updating vruntime, it needs to be reinserted into the correct position in the queue.
        if (runTask->inCfsQueue) {
            OsPriQueueDequeue(&runTask->pendList);
            OsPriQueueEnqueue(&runTask->pendList, runTask->priority, PRI_QUEUE_TAIL);
        }
    }
#endif

    OsCurrTaskSet((VOID*)newTask);
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    g_newTask_topOfStack = newTask->topOfStack;
#endif
    /* do the task context switch */
    ArchTaskSchedule(newTask, runTask);
}

VOID OsSchedPreempt(VOID)
{
    LosTaskCB *runTask = NULL;
    UINT32 intSave;

    if (!OsPreemptable()) {
        return;
    }

    SCHEDULER_LOCK(intSave);

    /* add run task back to ready queue */
    runTask = OsCurrTaskGet();
    runTask->taskStatus |= OS_TASK_STATUS_READY;

#ifdef LOSCFG_BASE_CORE_TIMESLICE
    if (runTask->timeSlice == 0) {
        OsPriQueueEnqueue(&runTask->pendList, runTask->priority, PRI_QUEUE_TAIL);
    } else {
#endif
        OsPriQueueEnqueue(&runTask->pendList, runTask->priority, PRI_QUEUE_HEAD);
#ifdef LOSCFG_BASE_CORE_TIMESLICE
    }
#endif

    /* reschedule to new thread */
    OsSchedResched();

    SCHEDULER_UNLOCK(intSave);
}

#ifdef LOSCFG_BASE_CORE_TIMESLICE
LITE_OS_SEC_TEXT VOID OsTimesliceCheck(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();
#ifdef LOSCFG_WCFS_SCHEDULER
    if (runTask->schedPolicy == TASK_POLICY_CFS) {
        return;
    }
#endif
    if (runTask->timeSlice != 0) {
        runTask->timeSlice--;
        if (runTask->timeSlice == 0) {
            LOS_Schedule();
        }
    }
}
#endif

#ifndef LOSCFG_SCHED_LATENCY
/*
 * Description : Process pending schedFlag tagged by others cores
 */
LITE_OS_SEC_TEXT_MINOR VOID OsSchedProcSchedFlag(VOID)
{
    Percpu *percpu = NULL;

    OsTaskProcSignal();

    /* check if needs to schedule */
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

#ifdef LOSCFG_SYSTEM_VIEW
    LOS_HookExe(LOS_HOOK_SYSVIEW_MV_TSK_TO_OVERFLOW_DELAY, LOS_HOOK_PARAM1, (UINTPTR)runTask);
#endif

    runTask->taskStatus |= OS_TASK_STATUS_PEND;
    LOS_ListTailInsert(list, &runTask->pendList);
    if (timeout != LOS_WAIT_FOREVER) {
        runTask->taskStatus |= OS_TASK_STATUS_PEND_TIME;
        OsTaskAdd2TimerList(runTask, timeout);
    }
    SET_CUR_STATE_TIME(runTask);
    OsSchedResched();
}

/*
 * Description : Wakes the task from the pending list and adds the task to the priority queue.
 * Input       : resumedTask --- resumed task
 */
VOID OsSchedWake(LosTaskCB *resumedTask)
{
    LOS_ListDelete(&resumedTask->pendList);

    resumedTask->taskStatus &= ~OS_TASK_STATUS_PEND;
    if (resumedTask->taskStatus & OS_TASK_STATUS_PEND_TIME) {
        OsTimerListDelete(resumedTask);
        resumedTask->taskStatus &= ~OS_TASK_STATUS_PEND_TIME;
    }

    if (!(resumedTask->taskStatus & (OS_TASK_STATUS_SUSPEND))) {
#ifdef LOSCFG_SYSTEM_VIEW
        LOS_HookExe(LOS_HOOK_SYSVIEW_MV_TSK_TO_READY, LOS_HOOK_PARAM1, (UINTPTR)resumedTask);
#endif
        LOS_TRACE(TASK_WAKE, resumedTask->taskId);
        resumedTask->taskStatus |= OS_TASK_STATUS_READY;
        OsPriQueueEnqueue(&resumedTask->pendList, resumedTask->priority, PRI_QUEUE_TAIL);
        SET_CUR_STATE_TIME(resumedTask);
    }
}

/*
 * Description : Change task priority.
 * Input       : taskCB    --- task control block
 *               priority  --- priority
 */
VOID OsSchedPrioModify(LosTaskCB *taskCB, UINT16 priority)
{
    LOS_ASSERT(LOS_SpinHeld(&g_taskSpin));

    LOS_TRACE(TASK_PRIOSET, taskCB->taskId, taskCB->taskStatus, taskCB->priority, priority);

    if (taskCB->taskStatus & OS_TASK_STATUS_READY) {
        OsPriQueueDequeue(&taskCB->pendList);
        OsPriQueueEnqueue(&taskCB->pendList, priority, PRI_QUEUE_TAIL);
    }

    taskCB->priority = priority;
}

UINT32 OsSchedSuspend(LosTaskCB *taskCB)
{
    UINT32 intSave;
    UINT16 tempStatus;
    UINT32 errRet = LOS_OK;

    LOS_TRACE(TASK_SUSPEND, taskCB->taskId, taskCB->taskStatus, OsCurrTaskGet()->taskId);

    SCHEDULER_LOCK(intSave);

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

        taskCB->taskStatus |= OS_TASK_STATUS_SUSPEND;
        SET_CUR_STATE_TIME(taskCB);
        OsTaskReSched();

        goto LOS_RETURN;
    }

    if (tempStatus & OS_TASK_STATUS_READY) {
        OsPriQueueDequeue(&taskCB->pendList);
        taskCB->taskStatus &= ~OS_TASK_STATUS_READY;
    }

    taskCB->taskStatus |= OS_TASK_STATUS_SUSPEND;
    SET_CUR_STATE_TIME(taskCB);

LOS_RETURN:
    SCHEDULER_UNLOCK(intSave);
    return errRet;
}
