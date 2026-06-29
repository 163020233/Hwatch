/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description : LiteOS adapt rtthread interrupt.
 * Author : Huawei LiteOS Team
 * Create : 2025-8-28
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
 */
#ifdef LOSCFG_WCFS_SCHEDULER
#include "los_cfsqueue_pri.h"
#include "los_task_pri.h"
#include "los_list.h"

LITE_OS_SEC_BSS CfsQueue g_cfsQueue;

/*
* weight = 1024/(1.25^(priority-20))
* Priority = 16 -> nice = 0.
*/
static const UINT32 g_prio_to_weight[OS_PRIORITY_QUEUE_NUM + 4] = {
    88761,      71755,      56483,      46273,
    36291,      29154,      23254,      18705,
    14949,      11916,      9548,       7620,
    6100,       4904,       3906,       3121,
    2501,       1991,       1586,       1277,
    1024,       820,        655,        526,
    423,        335,        272,        215,
    172,        137,        110,        87,
    87,         87,         87,         87,
};

VOID OsCfsQueueEnqueue(LOS_DL_LIST *cfsqueueItem)
{
    LOS_ASSERT(cfsqueueItem != NULL);
    LosTaskCB *runTask, *nowTask;
    runTask = LOS_DL_LIST_ENTRY(cfsqueueItem, LosTaskCB, pendList);
    LOS_DL_LIST *listOps = &g_cfsQueue.queues;
    listOps = listOps->pstNext;
    while (listOps != &g_cfsQueue.queues) {
        nowTask = LOS_DL_LIST_ENTRY(listOps, LosTaskCB, pendList);
        if (nowTask->vruntime > runTask->vruntime) {
            LOS_ListTailInsert(listOps, cfsqueueItem);
            return ;
        }
        listOps = listOps->pstNext;
    }

    // The task with the largest vruntime is inserted at the end of the queue
    LOS_ListTailInsert(&g_cfsQueue.queues, cfsqueueItem);
}

LITE_OS_SEC_TEXT_MINOR LosTaskCB *OsGetCfsTask(VOID)
{
    LOS_DL_LIST *listOps = &g_cfsQueue.queues;
    if (listOps->pstNext == listOps) {
        return NULL;
    }
    listOps = listOps->pstNext;
    LosTaskCB *runTask = NULL;
    runTask = LOS_DL_LIST_ENTRY(listOps, LosTaskCB, pendList);
    return runTask;
}

UINT64 OsGetCfsWeight(UINT64 deltaExec, UINT16 prio)
{
    return (deltaExec * (MIDDLE_WEIGHT) / g_prio_to_weight[prio]) + 1;
}

VOID OsUpgradeVruntime(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();
    UINT16 prio = runTask->priority;
    if (runTask == NULL || !OS_CFS_PRIO_IS_VALID(prio)) {
        return;
    }
    UINT64 nowTick = LOS_TickCountGet();
    UINT64 deltaExec = nowTick - runTask->execStart;
    if (deltaExec == 0) {
        return;
    }
    UINT64 temp = OsGetCfsWeight(deltaExec, prio);

    runTask->vruntime += temp;
    runTask->runtick  += deltaExec;
    // If the scheduling fails, continue recording from nowtick.
    runTask->execStart = nowTick;
}

bool OsCfsNeedSchedule(VOID)
{
    LosTaskCB *nowTask = OsCurrTaskGet();
    if (nowTask == NULL || nowTask->schedPolicy != TASK_POLICY_CFS) {
        return FALSE;
    }
    if (LOS_ListEmpty(&g_cfsQueue.queues)) {
        return FALSE;
    }

    LOS_DL_LIST *cfsqueueItem = g_cfsQueue.queues.pstNext;
    LosTaskCB *nextTask =  LOS_DL_LIST_ENTRY(cfsqueueItem, LosTaskCB, pendList);
    UINT64 nowTick = LOS_TickCountGet();

    if (nextTask == NULL || !OS_CFS_PRIO_IS_VALID(nextTask->priority)) {
        return FALSE;
    }
    if (nowTick - nowTask->execStart >= CFS_MINTIME_EXEC) {
        OsUpgradeVruntime();
    }
    // Ensure the minimum execution time for threads to avoid frequent context switching.
    if (nextTask->vruntime + OsGetCfsWeight(CFS_MINTIME_EXEC, nextTask->priority) <= nowTask->vruntime) {
        return TRUE;
    }

    return FALSE;
}

VOID LOS_PrtCfsTaskInfo(VOID)
{
    LosTaskCB *Task;
    double sumTick = (double)LOS_TickCountGet();
    for (UINT32 loop = 0; loop < g_taskMaxNum; loop++) {
        Task = OS_TCB_FROM_TID(loop);
        if (Task == NULL || Task->schedPolicy != TASK_POLICY_CFS) {
            continue;
        }
        PRINTK("----------taskName: %s    prio: %d    runtick: %lld    vruntime: %lld     CPU usage: %d%%----------\n",
            Task->taskName, Task->priority, Task->runtick, Task->vruntime, Task->runtick * 100 / sumTick); // 100tick
    }
    PRINTK("---------------------------------------------------------------------------------------------------\n");
}

#endif