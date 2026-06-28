/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: exc info
 * Author: Huawei LiteOS Team
  * Create: 2025-03-01
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

#include "los_task_pri.h"
#include "los_exc_info.h"
#include "los_sem_pri.h"
#include "los_mux_pri.h"
#include "los_queue.h"

#if defined(CONFIG_OH_MINI)
UINT8 g_excMsgArray[MAX_EXC_MEM_SIZE] = {0};
VOID *g_currAddr = (VOID *)g_excMsgArray;
UINT32 g_excQueueMaxNum = LOSCFG_BASE_IPC_QUEUE_LIMIT;

STATIC UINT32 OsExcInfoGetFunc(UINT32 type, UINT32 index, UINT32 loop, UINT32 len, EXC_INFO_SAVE_CALLBACK func)
{
    UINT32 i;

    *((UINT32 *)g_currAddr) = (UINT32)type;
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);

    *((UINT32 *)g_currAddr) = len * loop;
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);

    for (i = index; i < loop; i++) {
        (VOID)func(i, g_currAddr);
        g_currAddr = (UINT8 *)g_currAddr + len;
    }

    return LOS_OK;
}

STATIC UINT32 OsExcTskSwitchMsgGet(VOID)
{
    UINT32 i;
    UINT32 idx;
    UINT32 loop;
    UINT32 len = sizeof(UINT32) + (sizeof(CHAR) * LOS_TASK_NAMELEN);

    idx = ShbTaskSwitchInfoIdxGet();
    if (ShbTaskSwitchInfoIsFull() == TRUE) {
        i = idx;
        loop = i + OS_TASK_SWITCH_INFO_COUNT;
    } else {
        i = 0;
        loop = idx;
    }

    return OsExcInfoGetFunc(OS_EXC_TYPE_TSK_SWITCH, i, loop, len, (EXC_INFO_SAVE_CALLBACK)ShbTaskSwitchInfoGet);
}

#ifdef LOSCFG_BASE_IPC_QUEUE
#if defined(LOSCFG_SAFETY_KERNEL)
STATIC UINT32 OsExcQueueInfoGet(UINT32 queueId, EXC_QUEUE_INFO_S *queueInfo)
{
    QUEUE_INFO_S queueInfoTmp = {0};
    UINT32 ret = LOS_QueueInfoGet(queueId, &queueInfoTmp);
    if (ret != LOS_OK) {
        return ret;
    }

    queueInfo->queueID = queueInfoTmp.info.queueId;
    queueInfo->queueLen = queueInfoTmp.info.queueLen;
    queueInfo->queueSize = queueInfoTmp.info.queueSize;
    queueInfo->queueHead = queueInfoTmp.info.queueHead;
    queueInfo->queueTail = queueInfoTmp.info.queueTail;
    queueInfo->writableCnt = queueInfoTmp.info.readWriteableCnt[0];
    queueInfo->readableCnt = queueInfoTmp.info.readWriteableCnt[1];

    int minValue = OS_WAIT_TSK_ARRAY_LEN < 2 ? OS_WAIT_TSK_ARRAY_LEN:2;

    for (int i = 0; i < minValue; i++) {
        queueInfo->waitReadTask[i] = queueInfoTmp.waitReadTask & 0xFFFFFFFF;
        queueInfo->waitWriteTask[i] = queueInfoTmp.waitWriteTask & 0xFFFFFFFF;
        queueInfo->waitMemTask[i] = queueInfoTmp.waitMemTask & 0xFFFFFFFF;
        queueInfoTmp.waitReadTask >>= EXC_QUEUE_TASK_INFO;
        queueInfoTmp.waitWriteTask >>= EXC_QUEUE_TASK_INFO;
        queueInfoTmp.waitMemTask >>= EXC_QUEUE_TASK_INFO;
    }

    return LOS_OK;
}
#else
STATIC UINT32 OsExcQueueInfoGet(UINT32 queueId, EXC_QUEUE_INFO_S *queueInfo)
{
    return LOS_QueueInfoAllGet(queueId, LOSCFG_BASE_CORE_TSK_LIMIT,
                               (QUEUE_HEAD_INFO_S *)queueInfo, sizeof(EXC_QUEUE_INFO_S));
}
#endif

STATIC UINT32 OsExcQueueMsgGet(VOID)
{
    UINT32 len = sizeof(EXC_QUEUE_INFO_S);
    UINT32 loop = g_excQueueMaxNum;

    return OsExcInfoGetFunc(OS_EXC_TYPE_QUE, 1, loop, len, (EXC_INFO_SAVE_CALLBACK)OsExcQueueInfoGet);
}
#endif

STATIC UINT32 OsExcTaskInfoGet(UINT32 taskId, ExcTaskInfo *taskInfo)
{
    TSK_INFO_S *tempTaskInfo = (TSK_INFO_S *)taskInfo;
    UINT32 ret = LOS_TaskInfoGet(taskId, tempTaskInfo);
    if (ret == LOS_OK) {
        taskInfo->bOvf            = tempTaskInfo->bOvf;
        taskInfo->uwPeakUsed      = tempTaskInfo->uwPeakUsed;
        taskInfo->uwCurrUsed      = tempTaskInfo->uwCurrUsed;
        taskInfo->uwSP            = tempTaskInfo->uwSP;
        taskInfo->uwBottomOfStack = tempTaskInfo->uwBottomOfStack;
        taskInfo->uwTopOfStack    = tempTaskInfo->uwTopOfStack;
        taskInfo->uwStackSize     = tempTaskInfo->uwStackSize;
        taskInfo->uwEventMask     = tempTaskInfo->uwEventMask;
        taskInfo->uwEvent         = tempTaskInfo->uwEvent;
        taskInfo->uwSemID         = OS_INVALID_VALUE;
        taskInfo->uwMuxID         = OS_INVALID_VALUE;

        if (tempTaskInfo->pTaskSem != NULL) {
            taskInfo->uwSemID = ((LosSemCB *)tempTaskInfo->pTaskSem)->semId;
        }
        if (tempTaskInfo->pTaskMux != NULL) {
            taskInfo->uwMuxID = ((LosMuxCB *)tempTaskInfo->pTaskMux)->muxId;
        }
    }
    return ret;
}

STATIC UINT32 OsExcTaskMsgGet(VOID)
{
    UINT32 len = sizeof(ExcTaskInfo);
    UINT32 loop = LOSCFG_BASE_CORE_TSK_LIMIT;

    return OsExcInfoGetFunc(OS_EXC_TYPE_TSK, 0, loop, len, (EXC_INFO_SAVE_CALLBACK)OsExcTaskInfoGet);
}

STATIC UINT32 OsExcContentMsgGet(VOID)
{
    UINTPTR excContentEnd = MAX_SCENE_INFO_SIZE + (UINTPTR)g_currAddr;
    errno_t ret;

    /* save exception info */
    *((UINT32 *)g_currAddr) = OS_EXC_TYPE_CONTEXT;
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);

    *((UINT32 *)g_currAddr) = sizeof(ExcInfo) + sizeof(EXC_CONTEXT_S);
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);
    ret = memcpy_s((VOID *)g_currAddr, excContentEnd - (UINTPTR)g_currAddr,
                   (VOID *)&g_excInfo, sizeof(ExcInfo));
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_currAddr = (UINT8 *)g_currAddr + sizeof(ExcInfo);

    ret = memcpy_s((VOID *)g_currAddr, excContentEnd - (UINTPTR)g_currAddr,
                   g_excInfo.context, sizeof(EXC_CONTEXT_S));
    if (ret != EOK) {
        return LOS_NOK;
    }
    g_currAddr = (UINT8 *)g_currAddr + sizeof(EXC_CONTEXT_S);

    return LOS_OK;
}

VOID OsExcMsgDump(VOID)
{
    (VOID)memset_s(g_excMsgArray, MAX_EXC_MEM_SIZE, EXC_MSG_ARRAY_INIT_VALUE, MAX_EXC_MEM_SIZE);

    *((UINT32 *)g_currAddr) = MAX_EXC_MEM_SIZE;  /* The total length of exception information. */
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);

    (VOID)OsExcContentMsgGet();
    (VOID)OsExcTaskMsgGet();
#ifdef LOSCFG_BASE_IPC_QUEUE
    (VOID)OsExcQueueMsgGet();
#endif
    (VOID)OsExcTskSwitchMsgGet();

    *((UINT32 *)g_currAddr) = OS_EXC_TYPE_MAX;
    g_currAddr = (UINT8 *)g_currAddr + sizeof(UINT32);
    return;
}
#endif