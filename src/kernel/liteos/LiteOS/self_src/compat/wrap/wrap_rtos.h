/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Cmsis/Posix(free) WRAP Interface
 * Author: RTOS_Lite Team
 * Create: 2025-02-25
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
#ifndef WRAP_RTOS_H
#define WRAP_RTOS_H

#include "los_typedef.h"
#include "los_task.h"
#include "sre_task_external.h"
#include "los_memory.h"
#include "los_hwi.h"
#include "los_tick.h"
#ifdef OS_OPTION_COMPAT_CMSIS
#include "cmsis_os.h"
#include "los_membox.h"
#endif

#ifdef OS_OPTION_LOS
#define LOSCFG_BASE_CORE_SWTMR
#include "los_swtmr.h"
#include "sre_swtmr_external.h"
#endif
#ifdef OS_OPTION_EVENT
#define LOSCFG_BASE_IPC_EVENT
#include "los_event.h"
#endif
#ifdef OS_OPTION_QUEUE
#define LOSCFG_BASE_IPC_QUEUE
#include "los_queue.h"
#include "sre_queue_external.h"
#endif
#ifdef OS_OPTION_BIN_SEM
#define LOSCFG_BASE_IPC_SEM
#define LOSCFG_BASE_IPC_MUX
#include "los_sem.h"
#include "sre_sem_external.h"
#include "los_mux.h"
#define OS_SEM_BINARY_COUNT_MAX             g_maxSem
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#ifdef OS_OPTION_COMPAT_CMSIS
#define ERROR_IS_INVALID_SID(ret)           FALSE

#define CMSIS_LOG_ERROR(fmt, args...)

#define LOS_ERRTYPE_ERROR                   SRE_ERRTYPE_ERROR
#define LOS_ERRNO_QUEUE_MAIL_FREE_ERROR     SRE_ERRTYPE_ERROR

enum {
    OS_QUEUE_HEAD = 0,
    OS_QUEUE_TAIL = 1
};

enum EventRetMode {
    OS_EVENT_MODE_RETURN_BEFORE,
    OS_EVENT_MODE_RETURN_AFTER,
    OS_EVENT_MODE_RETURN_NONE
};

#define THREAD_CMSIS2LITEOS(threadId)       (((struct TagTskCB *)(threadId))->taskPID)
#define THREAD_LITEOS2CMSIS(threadId)       ((osThreadId)GET_TCB_HANDLE(threadId))

#define SEM_CMSIS2LITEOS(semaphore_id)      (((struct TagSemCB *)(semaphore_id))->semID)
#define SEM_LITEOS2CMSIS(semaphore_id)      ((osSemaphoreId)GET_SEM(semaphore_id))

#define MUX_CMSIS2LITEOS(mId)               (((struct TagSemCB *)(mId))->semID)
#define MUX_LITEOS2CMSIS(mId)               ((osMutexId)GET_SEM(mId))

#define QUEUE_CMSIS2LITEOS(qId)             (((struct TagQueCB *)(qId) - g_allQueue) + 1)
#define QUEUE_LITEOS2CMSIS(qId)             ((osMessageQId)GET_QUEUE_HANDLE((qId) - 1))

#define SWTMR_CMSIS2LITEOS(swtmrId)         (((struct TagSwTmrCtrl *)(swtmrId))->swtmrIndex)
#define OS_SWT_FROM_SWTID(swtmrId)          ((struct TagSwTmrCtrl *)(g_swtmrCBArray + OS_SWTMR_ID_2_INDEX(swtmrId)))
#define SWTMR_LITEOS2CMSIS(swtmrId)         ((osTimerId)(UINTPTR)OS_SWT_FROM_SWTID(swtmrId))

#define EVENT_CMSIS2LITEOS(eventId)         ((struct EventCb *)(eventId))
#define EVENT_LITEOS2CMSIS(eventId)         ((osEventFlagsId_t)(UINTPTR)(eventId))

typedef struct {
    void *(*startRoutine)(void *);
    UINTPTR         args[4];
} CmsisData;
#endif /* OS_OPTION_COMPAT_CMSIS */

STATIC_INLINE UINT32 OsMain(VOID)
{
    return SRE_OK;
}

extern INT32 main(VOID);
STATIC_INLINE VOID OsStart(VOID)
{
    UINT32 ret = main();
    if (ret != OS_OK) {
        CMSIS_LOG_ERROR("OsStart fail! ret: %x\n", ret);
    }
}

STATIC_INLINE UINT32 WrapTaskCreate(UINT32 *tId, TSK_INIT_PARAM_S *taskInitParam, VOID *stackMem)
{
    (VOID)stackMem;
    /* The DRB interface cannot be called directly for the time being, so replace it with LOS first. */
    return LOS_TaskCreate(tId, taskInitParam);
}

STATIC_INLINE UINT32 WrapTaskDelete(UINT32 taskId, const VOID *value)
{
    (VOID)value;
    return DRB_TaskDelete(taskId);
}

STATIC_INLINE UINT32 WrapTaskGetState(UINT32 taskId, UINT16 *taskStatus)
{
    *taskStatus = GET_TCB_HANDLE(taskId)->taskStatus;
    return SRE_OK;
}

STATIC_INLINE UINT32 WrapTaskGetStackSize(UINT32 taskId)
{
    return GET_TCB_HANDLE(taskId)->stackSize;
}

#ifdef OS_OPTION_COMPAT_CMSIS
STATIC_INLINE UINT32 WrapTaskSelfDelete(VOID)
{
    return DRB_TaskDelete(RUNNING_TASK->taskPID);
}

STATIC_INLINE UINT32 WrapTaskGetPid(UINT32 taskId)
{
    return taskId;
}

STATIC_INLINE BOOL WrapTaskIsPreemptable(VOID)
{
    return (OS_TASK_LOCK_DATA == 0);
}

STATIC_INLINE CHAR *WrapTaskGetName(UINT32 taskId)
{
#if (!defined(OS_OPTION_HUNT) && defined(OS_OPTION_TASK_INFO))
    return GET_TCB_HANDLE(taskId)->name;
#else
    (VOID)taskId;
    return NULL;
#endif
}

STATIC_INLINE UINT32 WrapTaskGetCurTaskId(VOID)
{
    return RUNNING_TASK->taskPID;
}

STATIC_INLINE UINT32 WrapTaskGetStackSpace(UINT32 taskId)
{
    struct TagTskCB *taskCB = (struct TagTskCB *)GET_TCB_HANDLE(taskId);
    UINT32 intSave;
    UINT32 count = 0;
    UINT32 *topOfStack = NULL;

    intSave = DRB_IntLock();
    if (TSK_IS_UNUSED(taskCB)) {
        DRB_IntRestore(intSave);
        return 0;
    }

    /* first 4 bytes is OS_TASK_MAGIC_WORD, skip */
    topOfStack = (UINT32 *)(UINTPTR)taskCB->topOfStack + OS_STACK_MAGIC_SIZE / sizeof(CHAR *);
    while (*topOfStack == (UINT32)OS_TSK_STACK_TOP_MAGIC) {
        ++topOfStack;
        ++count;
    }
    DRB_IntRestore(intSave);
    count *= (UINT32)sizeof(UINT32);

    return (uint32_t)count;
}

STATIC_INLINE UINT32 WrapTaskGetTaskCount(VOID)
{
    UINT32 count = 0;
    UINT32 taskId;
    struct TagTskCB *taskCB = NULL;

    for (taskId = 0; taskId < g_tskModInfo.maxNum; taskId++) {
        taskCB = GET_TCB_HANDLE(taskId);
        if (TSK_IS_UNUSED(taskCB)) {
            continue;
        }
        count++;
    }

    return count;
}

STATIC_INLINE VOID *WrapTaskGetCurTaskArg(VOID)
{
#ifdef OS_OPTION_TSK_OBSOLETE_API
    /* Corresponds to the OS_OPTION_TSK_OBSOLETE_API macro. */
    return (VOID *)RUNNING_TASK->args[0];
#else
    return (VOID *)RUNNING_TASK->args;
#endif
}
#endif /* OS_OPTION_COMPAT_CMSIS */

#ifdef LOSCFG_BASE_CORE_SWTMR
STATIC_INLINE BOOL WrapSwtmrCreate(UINT8 mode, SWTMR_PROC_FUNC handler,
                                   UINT16 *swtmrId, UINTPTR arg, BOOL needAlign)
{
    (VOID)needAlign;
    UINT8 losMode;
    switch (mode) {
        case osTimerOnce:
            losMode = LOS_SWTMR_MODE_ONCE;
            break;
        case osTimerPeriodic:
            losMode = LOS_SWTMR_MODE_PERIOD;
            break;
        case osTimerDelay:
            losMode = LOS_SWTMR_MODE_NO_SELFDELETE;
            break;
        default:
            return LOS_ERRNO_SWTMR_MODE_INVALID;
    }
    return LOS_SwtmrCreate(1, losMode, handler, swtmrId, arg);
}

STATIC_INLINE UINT32 WrapSwtmrStart(UINT16 swtId, UINT32 interval, UINT32 expiry)
{
    (VOID)expiry;
    OS_SWT_FROM_SWTID(swtId)->interval = interval;
    return LOS_SwtmrStart(swtId);
}

#ifdef OS_OPTION_COMPAT_CMSIS
STATIC_INLINE BOOL WrapSwtmrIsRunning(UINT16 swtId)
{
    return ((UINT32)OS_SWT_FROM_SWTID(swtId)->state == OS_TIMER_PRE_RUNNING);
}
#endif /* OS_OPTION_COMPAT_CMSIS */
#endif

#ifdef OS_OPTION_COMPAT_CMSIS
#ifdef LOSCFG_BASE_IPC_EVENT
STATIC_INLINE UINT32 WrapEventCreate(UINTPTR * eventId)
{
    struct EventCb *eventCB = NULL;
    UINT32 ret;

    eventCB = (struct EventCb *)OsMemAlloc(OS_MID_EVENT, OS_MEM_DEFAULT_PT0, sizeof(EVENT_CB_S));
    if (eventCB == NULL) {
        return LOS_NOK;
    }
    ret = DRB_EventInit(eventCB);
    if (ret == SRE_OK) {
        *eventId = (UINTPTR)eventCB;
        return SRE_OK;
    } else {
        if (OsMemFree(OS_MID_EVENT, eventCB) != SRE_OK) {
            CMSIS_LOG_ERROR("[%s] memory free fail!\n", __func__);
        }
        return LOS_NOK;
    }
}

STATIC_INLINE UINT32 WrapEventDelete(struct EventCb *eventCB)
{
    UINT32 ret = DRB_EventDestroy(eventCB);
    if (ret == SRE_OK) {
        if (OsMemFree(OS_MID_EVENT, (VOID *)eventCB) != SRE_OK) {
            CMSIS_LOG_ERROR("[%s] memory free fail!\n", __func__);
        }
    }
    return ret;
}

STATIC_INLINE UINT32 WrapEventRead(struct EventCb *eventCB, UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    return DRB_EventRead(eventCB, eventMask, mode, timeout);
}

STATIC_INLINE UINT32 WrapEventWriteAndGetEvent(struct EventCb *eventCB, UINT32 events, UINT32 retMode)
{
    (VOID)retMode;
    UINT32 ret = DRB_EventWrite(eventCB, events);
    if (ret == SRE_OK) {
        return eventCB->eventId;
    }
    return ret;
}

STATIC_INLINE UINT32 WrapEventClearAndGetEvent(struct EventCb *eventCB, UINT32 events)
{
    UINT32 intSave;
    UINT32 ret;
    UINT32 rflags;
    intSave = DRB_IntLock();
    rflags = eventCB->eventId;

    ret = DRB_EventClear(eventCB, events);
    DRB_IntRestore(intSave);
    if (ret == SRE_OK) {
        return rflags;
    }
    return ret;
}

STATIC_INLINE UINT32 WrapTaskEventRead(UINT32 eventMask, UINT32 mode, UINT32 timeout)
{
    struct TagTskCB *runTask = RUNNING_TASK;

    return DRB_EventRead(&(runTask->eventCb), eventMask, mode, timeout);
}

STATIC_INLINE UINT32 WrapTaskEventWriteAndGetEvent(UINT32 id, UINT32 events, UINT32 retMode)
{
    struct TagTskCB *taskCB = GET_TCB_HANDLE(id);
    UINT32 ret;
    UINT32 intSave;
    UINT32 eventid = 0;

    if (CHECK_TSK_PID_OVERFLOW(taskCB->taskPID)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (TSK_IS_UNUSED(taskCB)) {
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    if (retMode == OS_EVENT_MODE_RETURN_BEFORE) {
        intSave = DRB_IntLock();
        eventid = taskCB->eventCb.eventId;
        DRB_IntRestore(intSave);
    }

    ret = DRB_EventWrite(&taskCB->eventCb, events);

    if (retMode == OS_EVENT_MODE_RETURN_AFTER) {
        intSave = DRB_IntLock();
        eventid = taskCB->eventCb.eventId;
        DRB_IntRestore(intSave);
    }

    if (ret == SRE_OK) {
        return eventid;
    }
    return ret;
}

STATIC_INLINE UINT32 WrapTaskEventClearAndGetEvent(UINT32 id, UINT32 events)
{
    struct TagTskCB *taskCB = GET_TCB_HANDLE(id);
    UINT32 eventSave;
    UINT32 intSave;
    UINT32 ret;

    if (CHECK_TSK_PID_OVERFLOW(taskCB->taskPID)) {
        return LOS_ERRNO_TSK_ID_INVALID;
    }

    if (TSK_IS_UNUSED(taskCB)) {
        return LOS_ERRNO_TSK_NOT_CREATED;
    }

    intSave = DRB_IntLock();
    eventSave = taskCB->eventCb.eventId;
    DRB_IntRestore(intSave);
    ret = DRB_EventClear(&taskCB->eventCb, events);
    if (ret == SRE_OK) {
        return eventSave;
    }
    return ret;
}

STATIC_INLINE UINT32 WrapCurTaskEventClearAndGetEvent(UINT32 events)
{
    UINT32 intSave;
    UINT32 ret;
    struct TagTskCB *runTask = NULL;
    struct EventCb *eventCB = NULL;
    UINT32 rflags;

    intSave = DRB_IntLock();
    runTask = RUNNING_TASK;
    eventCB = &(runTask->eventCb);
    rflags = eventCB->eventId;
    ret = DRB_EventClear(eventCB, events);
    DRB_IntRestore(intSave);
    if (ret == SRE_OK) {
        return rflags;
    }
    return ret;
}
#endif /* LOSCFG_BASE_IPC_EVENT */

#ifdef LOSCFG_BASE_IPC_QUEUE
STATIC_INLINE UINT32 WrapQueueCreate(UINT32 *queueId, VOID *pool, UINT32 queueSize, UINT32 msgSize)
{
    if ((queueSize > UINT16_MAX) || (msgSize > UINT16_MAX)) {
        return LOS_NOK;
    }

#ifdef LOSCFG_QUEUE_STATIC_ALLOCATION
    UINT64 poolSize = OS_QUEUE_MEMSIZE((UINT64)msgSize, (UINT64)queueSize);
    if (poolSize > UINT16_MAX) {
        return LOS_NOK;
    }

    if (pool != NULL) {
        return DRB_QueueCreateStatic((UINT16)queueSize, (UINT16)msgSize,
                                     queueId, pool, (UINT16)poolSize);
    }
#else
    (VOID)pool;
#endif
    return DRB_QueueCreate((UINT16)queueSize, (UINT16)msgSize, queueId);
}

STATIC_INLINE UINT32 WrapQueueDelete(UINT32 queueId, BOOL userFlag)
{
    (VOID)userFlag;
    if ((queueId - 1) >= g_maxQueue) {
        return LOS_ERRNO_QUEUE_INVALID;
    }
    return  DRB_QueueDelete(queueId);
}

STATIC_INLINE UINT32 WrapQueueWriteCopy(UINT32 queueId, VOID *msgPtr, UINT32 timeout)
{
    return DRB_QueueWrite(queueId, msgPtr, GET_QUEUE_HANDLE(queueId)->nodeSize, timeout, OS_QUEUE_NORMAL);
}

STATIC_INLINE UINT32 WrapQueueReadCopy(UINT32 queueId, VOID *msgPtr, UINT32 timeout)
{
    UINT32 bufferSize = GET_QUEUE_HANDLE(queueId)->nodeSize;
    return DRB_QueueRead(queueId, msgPtr, &bufferSize, timeout);
}
#endif /* LOSCFG_BASE_IPC_QUEUE */

#ifdef LOSCFG_BASE_IPC_SEM
STATIC_INLINE UINT32 WrapSemPendAndGetCount(UINT32 sid, UINT32 timeout)
{
    UINT32 ret = DRB_SemPend((SemHandle)sid, timeout);
    if (ret == SRE_OK) {
        return GET_SEM(sid)->semCount;
    }
    return ret;
}

STATIC_INLINE UINT32 WrapSemGetCount(UINT32 semId)
{
    return GET_SEM(semId)->semCount;
}
#endif

#ifdef LOSCFG_BASE_IPC_MUX
STATIC_INLINE UINT32 WrapMutexGetOwner(UINT32 mutexId)
{
    struct TagSemCB *muxCB = GET_SEM(mutexId);
    UINT32 intSave;
    struct TagTskCB *taskCB = NULL;

    intSave = DRB_IntLock();
    taskCB = GET_TCB_HANDLE(muxCB->semOwner);
    DRB_IntRestore(intSave);
    return taskCB->taskPID;
}
#endif
#endif /* OS_OPTION_COMPAT_CMSIS */

#ifdef LOSCFG_COMPAT_CMSIS_VER_1
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
STATIC_INLINE VOID *WrapPoolCreate(UINT32 blkNum, UINT32 blkSize)
{
    VOID *memBox = NULL;
    UINT64 poolSize;
    UINT32 boxCBSize;
    UINT32 ret;
    UINT32 bSize = blkSize;

    poolSize = DRB_MEMBOX_SIZE(bSize, (UINT64)blkNum);
    boxCBSize = (UINT32)poolSize;

    if ((UINT32)(poolSize >> 32) != 0) { /* get the high 32 bits */
        CMSIS_LOG_ERROR("pool size too large! \n");
        return NULL;
    }
    memBox = OsMemAlloc(OS_MID_EVENT, OS_MEM_DEFAULT_PT0, boxCBSize);
    if (memBox == NULL) {
        return NULL;
    }

    ret = DRB_MemboxInit(memBox, (UINT32)poolSize, bSize);
    if (ret != SRE_OK) {
        (VOID)OsMemFree(OS_MID_EVENT, memBox);
        return NULL;
    }
    return memBox;
}

LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 WrapPoolDelete(VOID *pool)
{
    return OsMemFree(OS_MID_EVENT, pool);
}
#endif

#ifdef LOSCFG_BASE_IPC_QUEUE
STATIC_INLINE VOID *WrapMailPoolCreate(UINT32 queueId, VOID *pool, UINT32 blkNum, UINT32 blkSize)
{
    (VOID)queueId;
#ifdef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    if (DRB_MemboxInit(pool, osPoolMemSz(blkSize, blkNum), blkSize) != SRE_OK) {
        return NULL;
    }
    return pool;
#else
    (VOID)pool;
    return WrapPoolCreate(blkNum, blkSize);
#endif
}

STATIC_INLINE VOID *WrapQueueMailAlloc(UINT32 queueId, VOID *mailPool, UINT32 tick)
{
    return OsQueueMailAlloc(queueId, mailPool, tick);
}

STATIC_INLINE VOID *WrapQueueMailCAlloc(UINT32 queueId, VOID *mailPool, UINT32 tick)
{
    void *mem = OsQueueMailAlloc(queueId, mailPool, tick);
    if (mem != NULL) {
        DRB_MemboxClr(mailPool, mem);
    }
    return mem;
}

STATIC_INLINE UINT32 WrapQueueMailFree(UINT32 queueId, VOID *mailPool, VOID *mailMem)
{
    return OsQueueMailFree(queueId, mailPool, mailMem);
}

STATIC_INLINE UINT32 WrapQueueMailPut(UINT32 queueId, VOID *mailPool, VOID *mailMem, UINT32 opt)
{
    (VOID)mailPool;
    if (mailMem == NULL) {
        return LOS_ERRNO_QUEUE_WRITE_PTR_NULL;
    }

    if (opt == OS_QUEUE_HEAD) {
        return DRB_QueueWrite(queueId, &mailMem, (UINT32)sizeof(CHAR *), 0, OS_QUEUE_URGENT);
    }

    return DRB_QueueWrite(queueId, &mailMem, (UINT32)sizeof(CHAR *),  0, OS_QUEUE_NORMAL);
}

STATIC_INLINE UINT32 WrapQueueMailGet(UINT32 queueId, VOID *mailPool, VOID *buff, UINT32 timeout)
{
    (VOID)mailPool;
    UINT32 bufferSize = sizeof(UINT32);
    return DRB_QueueRead(queueId, buff, &bufferSize, timeout);
}

STATIC_INLINE UINT32 WrapQueueMailDelete(UINT32 queueId, VOID *mailPool)
{
    UINT32 ret = SRE_OK;
    if (((struct OsMemBoxInfo *)mailPool)->blkCnt != 0) {
        return LOS_NOK;
    }
    ret = DRB_QueueDelete(queueId);
    if (ret != SRE_OK) {
        return ret;
    }
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
    ret = WrapPoolDelete(mailPool);
#endif
    return ret;
}

STATIC_INLINE UINT32 WrapQueueMailClear(UINT32 queueId, VOID *mailPool)
{
    UINTPTR buff;
    UINT32 ret = SRE_OK;
    UINT32 intSave = DRB_IntLock();
    while (ret == SRE_OK) {
        ret = WrapQueueMailGet(queueId, mailPool, (VOID *)&buff, 0);
        if (ret == SRE_OK) {
            (VOID)WrapQueueMailFree(queueId, mailPool, (VOID *)buff);
        }
    }
    DRB_IntRestore(intSave);
    return ret;
}

LITE_OS_SEC_ALW_INLINE STATIC_INLINE UINT32 WrapMailCreate(const osMailQDef_t *queueDef, struct osMailQ **mailHandle)
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
    if (ret != SRE_OK) {
#ifndef LOSCFG_COMPAT_CMSIS_STATIC_ALLOCATION
        (VOID)WrapPoolDelete(mailQ->pool);
#endif
        return ret;
    }

    mailQ->queue = queueId;
    *mailHandle = mailQ;
    return SRE_OK;
}

LITE_OS_SEC_ALW_INLINE STATIC_INLINE osStatus MappingQueueWriteRet(UINT32 ret)
{
    switch (ret) {
        case SRE_OK:
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
        case SRE_OK:
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

#endif /* WRAP_RTOS_H */