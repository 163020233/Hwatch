/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Cmsis/Posix(free) WRAP Interface
 * Author: Huawei LiteOS Team
 * Create: 2024-09-02
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
#ifndef WRAP_FREE_H
#define WRAP_FREE_H

#include "los_task_pri.h"
#include "los_memory.h"
#ifdef LOSCFG_COMPAT_CMSIS
#include "cmsis_os.h"
#include "los_init_pri.h"
#include "los_membox.h"
#endif
#ifdef LOSCFG_BASE_CORE_SWTMR
#include "los_swtmr_pri.h"
#endif
#ifdef LOSCFG_BASE_IPC_EVENT
#include "los_event_pri.h"
#include "los_event.h"
#endif
#ifdef LOSCFG_BASE_IPC_QUEUE
#include "los_queue.h"
#include "los_queue_pri.h"
#endif
#ifdef LOSCFG_BASE_IPC_SEM
#include "los_sem_pri.h"
#endif
#ifdef LOSCFG_BASE_IPC_MUX
#include "los_mux_pri.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_COMPAT_CMSIS
#define ERROR_IS_INVALID_SID(ret)           FALSE

#ifndef LITE_OS_SEC_RODATA_USRSPACE
#define LITE_OS_SEC_RODATA_USRSPACE
#endif

enum EventRetMode {
    OS_EVENT_MODE_RETURN_BEFORE,
    OS_EVENT_MODE_RETURN_AFTER,
    OS_EVENT_MODE_RETURN_NONE
};

#define GET_QUEUEINFO_LEN(queueInfo)        ((queueInfo).usQueueLen)
#define GET_QUEUEINFO_MSGSIZE(queueInfo)    ((queueInfo).usQueueSize)
#define GET_QUEUEINFO_COUNT(queueInfo)      ((queueInfo).usReadableCnt)
#define GET_QUEUEINFO_SPACE(queueInfo)      ((queueInfo).usWritableCnt)

#define THREAD_CMSIS2LITEOS(threadId)       (((LosTaskCB *)(threadId))->taskId)
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#define THREAD_LITEOS2CMSIS(threadId)       ((osThreadId_t)OS_TCB_FROM_TID(threadId))
#else
#define THREAD_LITEOS2CMSIS(threadId)       ((osThreadId)OS_TCB_FROM_TID(threadId))
#endif

#define SEM_CMSIS2LITEOS(semaphore_id)      (((LosSemCB *)(semaphore_id))->semId)
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#define SEM_LITEOS2CMSIS(semaphore_id)      ((osSemaphoreId_t)GET_SEM(semaphore_id))
#else
#define SEM_LITEOS2CMSIS(semaphore_id)      ((osSemaphoreId)GET_SEM(semaphore_id))
#endif

#define MUX_CMSIS2LITEOS(mId)               (((LosMuxCB*)(mId))->muxId)
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#define MUX_LITEOS2CMSIS(mId)               ((osMutexId_t)GET_MUX(mId))
#else
#define MUX_LITEOS2CMSIS(mId)               ((osMutexId)GET_MUX(mId))
#endif

#define QUEUE_CMSIS2LITEOS(qId)             (((LosQueueCB *)(qId))->queueId)
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#define QUEUE_LITEOS2CMSIS(qId)             ((osMessageQueueId_t)GET_QUEUE_HANDLE(qId))
#else
#define QUEUE_LITEOS2CMSIS(qId)             ((osMessageQId)GET_QUEUE_HANDLE(qId))
#endif

#define SWTMR_CMSIS2LITEOS(swtmrId)         (((LosSwtmrCB *)(swtmrId))->timerId)
#ifdef LOSCFG_COMPAT_CMSIS_VER_2
#define SWTMR_LITEOS2CMSIS(swtmrId)         ((osTimerId_t)(UINTPTR)OS_SWT_FROM_SWTID(swtmrId))
#else
#define SWTMR_LITEOS2CMSIS(swtmrId)         ((osTimerId)(UINTPTR)OS_SWT_FROM_SWTID(swtmrId))
#endif

#define EVENT_CMSIS2LITEOS(eventId)         ((PEVENT_CB_S)(eventId))
#define EVENT_LITEOS2CMSIS(eventId)         ((osEventFlagsId_t)(UINTPTR)(eventId))

typedef struct {
    void *(*startRoutine)(void *);
    UINTPTR         args[4];
} CmsisData;
#endif /* LOSCFG_COMPAT_CMSIS */

STATIC INLINE UINT32 WrapTaskCreate(UINT32 *tId, TSK_INIT_PARAM_S *taskInitParam, VOID *stackMem)
{
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    if (stackMem != NULL) {
        return LOS_TaskCreateStatic(tId, taskInitParam, stackMem);
    }
#else
    (VOID)stackMem;
#endif
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    return LOS_TaskCreate(tId, taskInitParam);
#else
    return LOS_NOK;
#endif
}

STATIC INLINE UINT32 WrapTaskCreateOnly(UINT32 *tId, TSK_INIT_PARAM_S *taskInitParam, VOID *stackMem)
{
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    if (stackMem != NULL) {
        return LOS_TaskCreateOnlyStatic(tId, taskInitParam, stackMem);
    }
#else
    (VOID)stackMem;
#endif
#ifdef LOSCFG_TASK_STACK_DYNAMIC_ALLOCATION
    return LOS_TaskCreateOnly(tId, taskInitParam);
#else
    return LOS_NOK;
#endif
}

STATIC INLINE UINT32 WrapTaskDelete(UINT32 taskId, const VOID *value)
{
    (VOID)value;
    return LOS_TaskDelete(taskId);
}

STATIC INLINE UINT32 WrapTaskGetState(UINT32 taskId, UINT16 *taskStatus)
{
    *taskStatus = OS_TCB_FROM_TID(taskId)->taskStatus;
    return LOS_OK;
}

STATIC INLINE UINT32 WrapTaskGetStackSize(UINT32 taskId)
{
    return OS_TCB_FROM_TID(taskId)->stackSize;
}

#ifdef LOSCFG_COMPAT_CMSIS
STATIC INLINE UINT32 WrapTaskSelfDelete(VOID)
{
    return LOS_TaskDelete(OsCurrTaskGet()->taskId);
}

STATIC INLINE UINT32 WrapTaskGetPid(UINT32 taskId)
{
    return taskId;
}

STATIC INLINE BOOL WrapTaskIsPreemptable(VOID)
{
    return (OsPercpuGet()->taskLockCnt == 0);
}

STATIC INLINE CHAR *WrapTaskGetName(UINT32 taskId)
{
    return OS_TCB_FROM_TID(taskId)->taskName;
}

STATIC INLINE UINT32 WrapTaskGetCurTaskId(VOID)
{
    return OsCurrTaskGet()->taskId;
}

STATIC INLINE UINT32 WrapTaskGetStackSpace(UINT32 taskId)
{
    LosTaskCB *taskCB = (LosTaskCB *)OS_TCB_FROM_TID(taskId);
    UINT32 intSave;
    UINT32 count = 0;
    UINT32 *topOfStack = NULL;

    intSave = LOS_IntLock();
    if ((taskCB->taskStatus & OS_TASK_STATUS_UNUSED) || OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        LOS_IntRestore(intSave);
        return 0;
    }

    /* first 4 bytes is OS_TASK_MAGIC_WORD, skip */
    topOfStack = (UINT32 *)(UINTPTR)taskCB->topOfStack + OS_STACK_MAGIC_SIZE / sizeof(CHAR *);
    while (*topOfStack == (UINT32)OS_STACK_INIT) {
        ++topOfStack;
        ++count;
    }
    LOS_IntRestore(intSave);
    count *= (UINT32)sizeof(UINT32);

    return (uint32_t)count;
}

STATIC INLINE UINT32 WrapTaskGetTaskCount(VOID)
{
    UINT32 count = 0;
    UINT32 taskId;
    LosTaskCB *taskCB = NULL;

    for (taskId = 0; taskId < LOSCFG_BASE_CORE_TSK_LIMIT; taskId++) {
        taskCB = OS_TCB_FROM_TID(taskId);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }
        count++;
    }

    return count;
}

STATIC INLINE VOID *WrapTaskGetCurTaskArg(VOID)
{
#ifdef LOSCFG_OBSOLETE_API
    /* Corresponds to the LOS_TASK_PARAM_INIT_ARG macro. */
    return (VOID *)OsCurrTaskGet()->args[0];
#else
    return (VOID *)OsCurrTaskGet()->args;
#endif
}
#endif /* LOSCFG_COMPAT_CMSIS */

#ifdef LOSCFG_BASE_CORE_SWTMR
STATIC INLINE BOOL WrapSwtmrCreate(UINT8 mode, SWTMR_PROC_FUNC handler,
                                   UINT16 *swtmrId, UINTPTR arg, BOOL needAlign)
{
#ifndef LOSCFG_BASE_CORE_SWTMR_ALIGN
    (VOID)needAlign;
    return LOS_SwtmrCreate(1, mode, handler, swtmrId, arg);
#else
    UINT32 ret = LOS_SwtmrCreate(1, mode, handler, swtmrId, arg);
    if (ret == LOS_OK) {
        OS_SWT_FROM_SWTID(*swtmrId)->alignEnable = needAlign;
    }
    return ret;
#endif
}

STATIC INLINE UINT32 WrapSwtmrStart(UINT16 swtId, UINT32 interval, UINT32 expiry)
{
    return OsSwtmrStartTimer(swtId, interval, expiry);
}

#ifdef LOSCFG_COMPAT_CMSIS
STATIC INLINE BOOL WrapSwtmrIsRunning(UINT16 swtId)
{
    return ((UINT32)OS_SWT_FROM_SWTID(swtId)->state == OS_SWTMR_STATUS_TICKING);
}
#endif /* LOSCFG_COMPAT_CMSIS */

#ifdef LOSCFG_COMPAT_POSIX
LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 WrapSwtmrGetOverrun(UINT32 swtmrId)
{
    if (swtmrId >= OS_SWTMR_MAX_TIMERID) {
        return (UINT32)-1;
    }

    return OS_SWT_FROM_SWTID(swtmrId)->overrun;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 WrapSwtmrGetTime(UINT16 swtmrId, UINT32 *tick, UINT32 *interval)
{
    UINT32 ret;
    LosSwtmrCB *swtmr = NULL;

    /* get expire time */
    ret = LOS_SwtmrTimeGet(swtmrId, tick);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_SWTMR_NOT_STARTED)) {
        return LOS_NOK;
    }
    swtmr = OS_SWT_FROM_SWTID(swtmrId);
    *interval = swtmr->interval;

    return LOS_OK;
}
#endif /* LOSCFG_COMPAT_POSIX */
#endif

#ifdef LOSCFG_COMPAT_CMSIS
#ifdef LOSCFG_BASE_IPC_EVENT
STATIC INLINE UINT32 WrapEventCreate(UINTPTR * eventId)
{
    PEVENT_CB_S eventCB = NULL;
    UINT32 ret;

    eventCB = (PEVENT_CB_S)LOS_MemAlloc(m_aucSysMem0, sizeof(EVENT_CB_S));
    if (eventCB == NULL) {
        return LOS_NOK;
    }
    ret = LOS_EventInit(eventCB);
    if (ret == LOS_OK) {
        *eventId = (UINTPTR)eventCB;
        return LOS_OK;
    } else {
        if (LOS_MemFree(m_aucSysMem0, eventCB) != LOS_OK) {
            PRINT_ERR("[%s] memory free fail!\n", __func__);
        }
        return LOS_NOK;
    }
}

STATIC INLINE UINT32 WrapEventDelete(PEVENT_CB_S eventCB)
{
    UINT32 ret = LOS_EventDestroy(eventCB);
    if (ret == LOS_OK) {
        if (LOS_MemFree(m_aucSysMem0, (VOID *)eventCB) != LOS_OK) {
            PRINT_ERR("[%s] memory free fail!\n", __func__);
        }
    }
    return ret;
}

STATIC INLINE UINT32 WrapEventRead(PEVENT_CB_S eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    return LOS_EventRead(eventCB, eventMask, mode, timeout);
}

STATIC INLINE UINT32 WrapEventWriteAndGetEvent(PEVENT_CB_S eventCB, UINT32 events, UINT32 retMode)
{
    (VOID)retMode;
    UINT32 ret = LOS_EventWrite(eventCB, events);
    if (ret == LOS_OK) {
        return eventCB->uwEventID;
    }
    return ret;
}

STATIC INLINE UINT32 WrapEventClearAndGetEvent(PEVENT_CB_S eventCB, UINT32 events)
{
    UINT32 intSave;
    UINT32 ret;
    UINT32 rflags;
    intSave = LOS_IntLock();
    rflags = eventCB->uwEventID;

    ret = LOS_EventClear(eventCB, events);
    LOS_IntRestore(intSave);
    if (ret == LOS_OK) {
        return rflags;
    }
    return ret;
}

STATIC INLINE UINT32 WrapTaskEventRead(UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    LosTaskCB *runTask = OsCurrTaskGet();

    return LOS_EventRead(&(runTask->event), eventMask, mode, timeout);
}

STATIC INLINE UINT32 WrapTaskEventWriteAndGetEvent(UINT32 id, UINT32 events, UINT32 retMode)
{
    LosTaskCB *taskCB = OS_TCB_FROM_TID(id);
    UINT32 ret;
    UINT32 intSave;
    UINT32 eventid = 0;

    if (OS_TASK_ID_CHECK_INVALID(taskCB->taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if ((taskCB->taskStatus & OS_TASK_STATUS_UNUSED)) {
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if (OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        return LOS_ERRNO_TSK_IS_ZOMBIE;
    }

    if (retMode == OS_EVENT_MODE_RETURN_BEFORE) {
        intSave = LOS_IntLock();
        eventid = taskCB->event.uwEventID;
        LOS_IntRestore(intSave);
    }

    ret = LOS_EventWrite(&taskCB->event, events);

    if (retMode == OS_EVENT_MODE_RETURN_AFTER) {
        intSave = LOS_IntLock();
        eventid = taskCB->event.uwEventID;
        LOS_IntRestore(intSave);
    }

    if (ret == LOS_OK) {
        return eventid;
    }
    return ret;
}

STATIC INLINE UINT32 WrapTaskEventClearAndGetEvent(UINT32 id, UINT32 events)
{
    LosTaskCB *taskCB = OS_TCB_FROM_TID(id);
    UINT32 eventSave;
    UINT32 intSave;
    UINT32 ret;

    if (OS_TASK_ID_CHECK_INVALID(taskCB->taskId)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if (OS_TASK_IS_ZOMBIE(taskCB->taskStatus)) {
        return LOS_ERRNO_TSK_IS_ZOMBIE;
    }

    intSave = LOS_IntLock();
    eventSave = taskCB->event.uwEventID;
    LOS_IntRestore(intSave);
    ret = LOS_EventClear(&taskCB->event, events);
    if (ret == LOS_OK) {
        return eventSave;
    }
    return ret;
}

STATIC INLINE UINT32 WrapCurTaskEventClearAndGetEvent(UINT32 events)
{
    UINT32 intSave;
    UINT32 ret;
    LosTaskCB *runTask = NULL;
    EVENT_CB_S *eventCB = NULL;
    UINT32 rflags;

    intSave = LOS_IntLock();
    runTask = OsCurrTaskGet();
    eventCB = &(runTask->event);
    rflags = eventCB->uwEventID;
    ret = LOS_EventClear(eventCB, events);
    LOS_IntRestore(intSave);
    if (ret == LOS_OK) {
        return rflags;
    }
    return ret;
}

#ifdef LOSCFG_COMPAT_CMSIS_VER_2
LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 MappingEventRet(UINT32 ret)
{
    switch (ret) {
        case LOS_ERRNO_EVENT_PTR_NULL:
        case LOS_ERRNO_EVENT_SETBIT_INVALID:
        case LOS_ERRNO_EVENT_EVENTMASK_INVALID:
        case LOS_ERRNO_EVENT_FLAGS_INVALID:
            return (UINT32)osFlagsErrorParameter;
        case LOS_ERRNO_EVENT_READ_TIMEOUT:
            return (UINT32)osFlagsErrorTimeout;
        default:
            return (UINT32)osFlagsErrorResource;
    }
}
#endif
#endif /* LOSCFG_BASE_IPC_EVENT */

#ifdef LOSCFG_BASE_IPC_QUEUE
STATIC INLINE UINT32 WrapQueueCreate(UINT32 *queueId, VOID *pool, UINT32 queueSize, UINT32 msgSize)
{
    if ((queueSize > UINT16_MAX) || (msgSize > UINT16_MAX)) {
        return LOS_NOK;
    }

#ifdef LOSCFG_QUEUE_STATIC_ALLOCATION
    UINT64 poolSize = LOS_QUEUEMEM_SIZE((UINT64)msgSize, (UINT64)queueSize);
    if (poolSize > UINT16_MAX) {
        return LOS_NOK;
    }

    if (pool != NULL) {
        return LOS_QueueCreateStatic(NULL, (UINT16)queueSize, queueId,
                                     0, (UINT16)msgSize, pool, (UINT16)poolSize);
    }
#else
    (VOID)pool;
#endif
#ifdef LOSCFG_QUEUE_DYNAMIC_ALLOCATION
    return LOS_QueueCreate(NULL, (UINT16)queueSize, queueId, 0, (UINT16)msgSize);
#else
    return LOS_NOK;
#endif
}

STATIC INLINE UINT32 WrapQueueDelete(UINT32 queueId, BOOL userFlag)
{
    (VOID)userFlag;
    if (GET_QUEUE_INDEX(queueId) >= KERNEL_QUEUE_LIMIT) {
        return LOS_ERRNO_QUEUE_INVALID;
    }
    return  LOS_QueueDelete(queueId);
}

STATIC INLINE UINT32 WrapQueueWriteCopy(UINT32 queueId, VOID *msgPtr, UINT32 timeout)
{
    return LOS_QueueWriteCopy(queueId, msgPtr, GET_QUEUE_HANDLE(queueId)->queueSize, timeout);
}

STATIC INLINE UINT32 WrapQueueReadCopy(UINT32 queueId, VOID *msgPtr, UINT32 timeout)
{
    UINT32 bufferSize = GET_QUEUE_HANDLE(queueId)->queueSize;
    return LOS_QueueReadCopy(queueId, msgPtr, &bufferSize, timeout);
}
#endif /* LOSCFG_BASE_IPC_QUEUE */

#ifdef LOSCFG_BASE_IPC_SEM
STATIC INLINE UINT32 WrapSemPendAndGetCount(UINT32 sid, UINT32 timeout)
{
    UINT32 ret = LOS_SemPend(sid, timeout);
    if (ret == LOS_OK) {
        return GET_SEM(sid)->semCount;
    }
    return ret;
}

STATIC INLINE UINT32 WrapSemGetCount(UINT32 semId)
{
    return GET_SEM(semId)->semCount;
}
#endif

#ifdef LOSCFG_BASE_IPC_MUX
STATIC INLINE UINT32 WrapMutexGetOwner(UINT32 mutexId)
{
    LosMuxCB *muxCB = GET_MUX(mutexId);
    UINT32 intSave;
    LosTaskCB *taskCB = NULL;

    intSave = LOS_IntLock();
    taskCB = muxCB->owner;
    LOS_IntRestore(intSave);
    return taskCB->taskId;
}
#endif
#endif /* LOSCFG_COMPAT_CMSIS */

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
STATIC INLINE VOID *WrapPoolCreate(UINT32 blkNum, UINT32 blkSize)
{
    VOID *memBox = NULL;
    UINT64 poolSize;
    UINT32 boxCBSize;
    UINT32 ret;
    UINT32 bSize = blkSize;

#ifdef LOSCFG_KERNEL_MEMBOX_STATIC
    poolSize = LOS_MEMBOX_SIZE(bSize, (UINT64)blkNum);
    boxCBSize = (UINT32)poolSize;
#else
    bSize = LOS_MEMBOX_ALLIGNED(bSize);
    poolSize = bSize * (UINT64)blkNum;
    boxCBSize = (UINT32)sizeof(LOS_MEMBOX_INFO);
#endif
    if ((UINT32)(poolSize >> 32) != 0) { /* get the high 32 bits */
        PRINT_DEBUG("pool size too large! \n");
        return NULL;
    }
    memBox = LOS_MemAlloc((VOID *)m_aucSysMem0, boxCBSize);
    if (memBox == NULL) {
        return NULL;
    }

    ret = LOS_MemboxInit(memBox, (UINT32)poolSize, bSize);
    if (ret != LOS_OK) {
        (VOID)LOS_MemFree((VOID *)m_aucSysMem0, memBox);
        return NULL;
    }
    return memBox;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 WrapPoolDelete(VOID *pool)
{
    return LOS_MemFree((VOID *)m_aucSysMem0, pool);
}
#endif

#ifdef LOSCFG_BASE_IPC_QUEUE
STATIC INLINE VOID *WrapMailPoolCreate(UINT32 queueId, VOID *pool, UINT32 blkNum, UINT32 blkSize)
{
    (VOID)queueId;
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    if (LOS_MemboxInit(pool, osPoolMemSz(blkSize, blkNum), blkSize) != LOS_OK) {
        return NULL;
    }
    return pool;
#else
    (VOID)pool;
    return WrapPoolCreate(blkNum, blkSize);
#endif
}

STATIC INLINE VOID *WrapQueueMailAlloc(UINT32 queueId, VOID *mailPool, UINT32 tick)
{
    return OsQueueMailAlloc(queueId, mailPool, tick);
}

STATIC INLINE VOID *WrapQueueMailCAlloc(UINT32 queueId, VOID *mailPool, UINT32 tick)
{
    void *mem = OsQueueMailAlloc(queueId, mailPool, tick);
    if (mem != NULL) {
        LOS_MemboxClr(mailPool, mem);
    }
    return mem;
}

STATIC INLINE UINT32 WrapQueueMailFree(UINT32 queueId, VOID *mailPool, VOID *mailMem)
{
    return OsQueueMailFree(queueId, mailPool, mailMem);
}

STATIC INLINE UINT32 WrapQueueMailPut(UINT32 queueId, VOID *mailPool, VOID *mailMem, UINT32 opt)
{
    (VOID)mailPool;
    if (opt == OS_QUEUE_HEAD) {
        return LOS_QueueWriteHead(queueId, mailMem, sizeof(UINT32), 0);
    }

    return LOS_QueueWrite(queueId, mailMem, sizeof(UINT32), 0);
}

STATIC INLINE UINT32 WrapQueueMailGet(UINT32 queueId, VOID *mailPool, VOID *buff, UINT32 timeout)
{
    (VOID)mailPool;
    return LOS_QueueRead(queueId, buff, sizeof(UINT32), timeout);
}

STATIC INLINE UINT32 WrapQueueMailDelete(UINT32 queueId, VOID *mailPool)
{
    UINT32 ret = LOS_OK;
    if (((LOS_MEMBOX_INFO *)mailPool)->uwBlkCnt != 0) {
        return LOS_NOK;
    }
    ret = LOS_QueueDelete(queueId);
    if (ret != LOS_OK) {
        return ret;
    }
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = WrapPoolDelete(mailPool);
#endif
    return ret;
}

STATIC INLINE UINT32 WrapQueueMailClear(UINT32 queueId, VOID *mailPool)
{
    UINTPTR buff;
    UINT32 ret = LOS_OK;
    UINT32 intSave = LOS_IntLock();
    while (ret == LOS_OK) {
        ret = WrapQueueMailGet(queueId, mailPool, (VOID *)&buff, 0);
        if (ret == LOS_OK) {
            (VOID)WrapQueueMailFree(queueId, mailPool, (VOID *)buff);
        }
    }
    LOS_IntRestore(intSave);
    return ret;
}

LITE_OS_SEC_ALW_INLINE STATIC INLINE UINT32 WrapMailCreate(const osMailQDef_t *queueDef, struct osMailQ **mailHandle)
{
    UINT32 ret;
    UINT32 queueId = 0;
    struct osMailQ *mailQ = NULL;

    mailQ = (struct osMailQ *)(queueDef->pool);
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    mailQ->pool = WrapMailPoolCreate(queueId, mailQ->pool, queueDef->queue_sz, queueDef->item_sz);
#else
    mailQ->pool = WrapMailPoolCreate(queueId, NULL, queueDef->queue_sz, queueDef->item_sz);
#endif
    if (mailQ->pool == NULL) {
        return LOS_NOK;
    }
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = WrapQueueCreate(&queueId, (UINT8 *)mailQ->pool + mailQ->m_sz, queueDef->queue_sz, sizeof(VOID*));
#else
    ret = WrapQueueCreate(&queueId, NULL, queueDef->queue_sz, sizeof(VOID*));
#endif
    if (ret != LOS_OK) {
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
        (VOID)WrapPoolDelete(mailQ->pool);
#endif
        return ret;
    }

    mailQ->queue = queueId;
    *mailHandle = mailQ;
    return LOS_OK;
}

LITE_OS_SEC_ALW_INLINE STATIC_INLINE osStatus MappingQueueWriteRet(UINT32 ret)
{
    switch (ret) {
        case LOS_OK:
            return osOK;
        case LOS_ERRNO_QUEUE_TIMEOUT:
        case LOS_ERRNO_QUEUE_ISFULL:
            return osErrorTimeoutResource;
        case LOS_ERRNO_QUEUE_INVALID:
        case LOS_ERRNO_QUEUE_NOT_CREATE:
        case LOS_ERRNO_QUEUE_WRITE_PTR_NULL:
        case LOS_ERRNO_QUEUE_WRITESIZE_ISZERO:
        case LOS_ERRNO_QUEUE_WRITE_SIZE_TOO_BIG:
            return osErrorParameter;
        default:
            return osErrorResource;
    }
}

LITE_OS_SEC_ALW_INLINE STATIC_INLINE osStatus MappingQueueReadRet(UINT32 ret)
{
    switch (ret) {
        case LOS_OK:
            return osEventMessage;
        case LOS_ERRNO_QUEUE_TIMEOUT:
        case LOS_ERRNO_QUEUE_ISEMPTY:
            return osEventTimeout;
        case LOS_ERRNO_QUEUE_INVALID:
        case LOS_ERRNO_QUEUE_NOT_CREATE:
        case LOS_ERRNO_QUEUE_READ_PTR_NULL:
        case LOS_ERRNO_QUEUE_READ_SIZE_TOO_SMALL:
            return osErrorParameter;
        default:
            return osErrorResource;
    }
}
#endif /* LOSCFG_BASE_IPC_QUEUE */
#endif /* LOSCFG_COMPAT_CMSIS_VER_1 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* WRAP_FREE_H */
