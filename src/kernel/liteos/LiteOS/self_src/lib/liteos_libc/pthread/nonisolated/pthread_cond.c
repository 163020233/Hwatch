/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Pthread condition file
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#if defined(__LITEOS__)
#include "pprivate.h"
#include "pthread.h"
#include "stdlib.h"
#include "limits.h"
#include "errno.h"
#include "time_pri.h"
#include "los_atomic.h"
#include "los_event_pri.h"
#include "los_sys.h"
#else
#include "errno.h"
#include "pthread.h"
#include "time.h"
#include "../../include/posix_adapt.h"
#include "drb_event.h"
#endif

#if defined(__LITEOS__)
#define BROADCAST_EVENT     1
#define COND_COUNTER_STEP   0x0004U
#define COND_FLAGS_MASK     0x0003U
#define COND_COUNTER_MASK   (~COND_FLAGS_MASK)
#define COND_INV_CLOCK_ID   INT_MAX
#define OS_SEC_L4_TEXT
#define DEFAULT_VISIBILITY
#else
#define COND_INV_CLOCK_ID   OS_MAX_S32
#define PRINT_ERR(fmt, ...)
#endif

#if defined(__LITEOS__)
STATIC INLINE INT32 CondInitCheck(const pthread_cond_t *cond)
{
    if ((cond->event.stEventList.pstPrev == NULL) &&
        (cond->event.stEventList.pstNext == NULL)) {
        return 1;
    }
    return 0;
}

STATIC INLINE INT32 WrapCondVarCreate(pthread_cond_t *cond)
{
    (VOID)LOS_EventInit(&(cond->event));

    cond->mutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (cond->mutex == NULL) {
        return ENOMEM;
    }

    (VOID)pthread_mutex_init(cond->mutex, NULL);

    cond->value = 0;
    (VOID)pthread_mutex_lock(cond->mutex);
    cond->count = 0;
    (VOID)pthread_mutex_unlock(cond->mutex);
    return ENOERR;
}
#else
INLINE int WrapCondVarCreate(pthread_cond_t *cond)
{
    OS_LIST_INIT((struct TagListObject *)(uintptr_t)&cond->head);
    OsSpinLockInitInner(&cond->spinlock);
    cond->init = PTHREAD_COND_INIT_MAGIC;

    return ENOERR;
}
#endif

#if defined(__LITEOS__)
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *shared)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_getpshared(const pthread_condattr_t *restrict attr,
    int *restrict shared)
#endif
{
    if ((attr == NULL) || (shared == NULL)) {
        return EINVAL;
    }

    *shared = PTHREAD_PROCESS_PRIVATE;

    return 0;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_setpshared(pthread_condattr_t *attr, int shared)
{
    (VOID)attr;
    if ((shared != PTHREAD_PROCESS_PRIVATE) && (shared != PTHREAD_PROCESS_SHARED)) {
        return EINVAL;
    }

    if (shared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }

    return 0;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_destroy(pthread_condattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }
    (VOID)memset(attr, 0, sizeof(pthread_condattr_t));
    attr->clock = COND_INV_CLOCK_ID;

    return 0;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_init(pthread_condattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->clock = CLOCK_REALTIME;

    return 0;
}

#if defined(__LITEOS__)
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_init(pthread_cond_t *restrict cond,
    const pthread_condattr_t *restrict attr)
#endif
{
    if (cond == NULL) {
        return EINVAL;
    }

#if !defined(__LITEOS__)
    if (OsCondIsInit(cond)) { // 不允许重复初始化
        return EBUSY;
    }
#endif

    if (attr != NULL) {
        if ((attr->clock != CLOCK_MONOTONIC) && (attr->clock != CLOCK_REALTIME)) {
            PRINT_ERR("%s: clockAttr only supports CLOCK_MONOTONIC and CLOCK_REALTIME.\n", __FUNCTION__);
            return EINVAL;
        }
        cond->stCondAttr.clock = attr->clock;
    } else {
        cond->stCondAttr.clock = CLOCK_REALTIME;
    }

    return WrapCondVarCreate(cond);
}

#if defined(__LITEOS__)
int pthread_cond_destroy(pthread_cond_t *cond)
{
    if (cond == NULL) {
        return EINVAL;
    }

    if (CondInitCheck(cond)) {
        return ENOERR;
    }

    if (LOS_EventDestroy(&cond->event) != LOS_OK) {
        return EBUSY;
    }
    if (pthread_mutex_destroy(cond->mutex) != ENOERR) {
        PRINT_ERR("%s mutex destroy fail!\n", __FUNCTION__);
        return EINVAL;
    }
    free(cond->mutex);
    cond->mutex = NULL;
    return ENOERR;
}
#else
INLINE bool OsCondParamIsValid(pthread_cond_t *cond)
{
    if (cond == NULL || !OsCondIsInit(cond)) {
        return FALSE;
    }

    return TRUE;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_destroy(pthread_cond_t *cond)
{
    uintptr_t intSave;
    if (!OsCondParamIsValid(cond)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    OsSplLock(&cond->spinlock);
    if (!ListEmpty((struct TagListObject *)(uintptr_t)&cond->head)) { // 条件变量正被使用
        OsSplUnlock(&cond->spinlock);
        OsIntRestore(intSave);
        return EBUSY;
    }

    cond->stCondAttr.clock = COND_INV_CLOCK_ID;
    cond->init = PTHREAD_COND_DESTROY_MAGIC;
    OsSplUnlock(&cond->spinlock);
    OS_SPIN_FREE(cond->spinlock);
    OsIntRestore(intSave);

    return ENOERR;
}
#endif

#if defined(__LITEOS__)
int pthread_condattr_getclock(const pthread_condattr_t *attr, clockid_t *clock)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_getclock(const pthread_condattr_t *restrict attr,
    clockid_t *restrict clock)
#endif
{
    if ((attr == NULL) || (clock == NULL)) {
#if defined(__LITEOS__)
        return -1;
#else
        return EINVAL;
#endif
    }

    *clock = attr->clock;
    return 0;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_condattr_setclock(pthread_condattr_t *attr, clockid_t clock)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if ((clock != CLOCK_MONOTONIC) && (clock != CLOCK_REALTIME)) {
        return EINVAL;
    }

    attr->clock = clock;

    return 0;
}

#if defined(__LITEOS__)
STATIC VOID PthreadCondValueModify(pthread_cond_t *cond)
{
    UINT32 flags = ((UINT32)cond->value & COND_FLAGS_MASK);
    INT32 oldVal, newVal;

    while (true) {
        oldVal = cond->value;
        newVal = (INT32)(((UINT32)(oldVal - COND_COUNTER_STEP) & COND_COUNTER_MASK) | flags);
        if (LOS_AtomicCmpXchg32bits(&cond->value, newVal, oldVal) == 0) {
            break;
        }
    }
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
    int ret = ENOERR;

    if (cond == NULL) {
        return EINVAL;
    }

    (VOID)pthread_mutex_lock(cond->mutex);
    if (cond->count > 0) {
        cond->count = 0;
        (VOID)pthread_mutex_unlock(cond->mutex);

        PthreadCondValueModify(cond);

        (VOID)LOS_EventWrite(&(cond->event), BROADCAST_EVENT);
        return ret;
    }
    (VOID)pthread_mutex_unlock(cond->mutex);

    return ret;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    int ret = ENOERR;

    if (cond == NULL) {
        return EINVAL;
    }

    (VOID)pthread_mutex_lock(cond->mutex);
    if (cond->count > 0) {
        cond->count--;
        (VOID)pthread_mutex_unlock(cond->mutex);
        PthreadCondValueModify(cond);
        (VOID)OsEventWriteOnce(&(cond->event), 0x01);

        return ret;
    }
    (VOID)pthread_mutex_unlock(cond->mutex);

    return ret;
}

STATIC INT32 PthreadCondWaitSub(pthread_cond_t *cond, INT32 value, UINT32 ticks)
{
    EventCond eventCond = { &cond->value, value, ~0x01U };
    /*
     * When the scheduling lock is held:
     * (1) value is not equal to cond->value, clear the event message and
     * do not block the current thread, because other threads is calling pthread_cond_broadcast or
     * pthread_cond_signal to modify cond->value and wake up the current thread,
     * and others threads will block on the scheduling lock until the current thread releases
     * the scheduling lock.
     * (2) value is equal to cond->value, block the current thread
     * and wait to be awakened by other threads.
     */
    return (int)OsEventReadWithCond(&eventCond, &(cond->event), 0x0fU,
                                    LOS_WAITMODE_OR | LOS_WAITMODE_CLR, ticks);
}

STATIC VOID PthreadCountSub(pthread_cond_t *cond)
{
    (VOID)pthread_mutex_lock(cond->mutex);
    if (cond->count > 0) {
        cond->count--;
    }
    (VOID)pthread_mutex_unlock(cond->mutex);
}

STATIC INT32 ProcessReturnVal(pthread_cond_t *cond, INT32 val)
{
    INT32 ret;
    switch (val) {
        /* 0: event does not occur */
        case 0:
        case BROADCAST_EVENT:
            ret = ENOERR;
            break;
        case LOS_ERRNO_EVENT_READ_TIMEOUT:
            PthreadCountSub(cond);
            ret = ETIMEDOUT;
            break;
        default:
            PthreadCountSub(cond);
            ret = EINVAL;
            break;
    }
    return ret;
}

STATIC INT32 OsGetTickFromNow(const struct timespec *absTime, clockid_t clockId, UINT32 *absTicks)
{
    struct timespec tp = {0};
    UINT64 nseconds;
    UINT64 currTime;
    UINT64 tickCnt;

    const UINT32 nsPerTick = OS_SYS_NS_PER_SECOND / LOSCFG_BASE_CORE_TICK_PER_SECOND;

    if ((absTime->tv_sec == 0) && (absTime->tv_nsec == 0)) {
        return ETIMEDOUT;
    }

    if (!ValidTimespec(absTime)) {
        return EINVAL;
    }

    /* clockId is CLOCK_REALTIME or CLOCK_MONOTONIC */
    (VOID)clock_gettime(clockId, &tp);
    currTime = (UINT64)tp.tv_sec * OS_SYS_NS_PER_SECOND + (UINT64)tp.tv_nsec;
    nseconds = (UINT64)absTime->tv_sec * OS_SYS_NS_PER_SECOND + (UINT64)absTime->tv_nsec;
    if (currTime >= nseconds) {
        return ETIMEDOUT;
    }

    tickCnt = ((nseconds - currTime) + nsPerTick - 1) / nsPerTick + 1;
    if (tickCnt > LOS_WAIT_FOREVER) {
        *absTicks = LOS_WAIT_FOREVER;
    } else {
        *absTicks = (UINT32)tickCnt;
    }
    return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
                           const struct timespec *absTime)
{
    UINT32 absTicks = 0;
    INT32 ret;
    INT32 oldValue;
    LosTaskCB *runTask = NULL;

    pthread_testcancel();
    if ((cond == NULL) || (mutex == NULL) || (absTime == NULL)) {
        return EINVAL;
    }

    runTask = (LosTaskCB*)OsCurrTaskGet();
    if ((mutex->stAttr.type == PTHREAD_MUTEX_ERRORCHECK) && (runTask != mutex->stLock.owner)) {
        return EPERM;
    }

    if (CondInitCheck(cond)) {
        ret = pthread_cond_init(cond, NULL);
        if (ret != ENOERR) {
            return ret;
        }
    }

    oldValue = cond->value;

    (VOID)pthread_mutex_lock(cond->mutex);
    cond->count++;
    (VOID)pthread_mutex_unlock(cond->mutex);

    ret = OsGetTickFromNow(absTime, cond->stCondAttr.clock, &absTicks);
    if (ret != 0) {
        return ret;
    }

    if (pthread_mutex_unlock(mutex) != ENOERR) {
        PRINT_ERR("%s: %d failed\n", __FUNCTION__, __LINE__);
    }

    ret = PthreadCondWaitSub(cond, oldValue, absTicks);
    if (pthread_mutex_lock(mutex) != ENOERR) {
        PRINT_ERR("%s: %d failed\n", __FUNCTION__, __LINE__);
    }

    ret = ProcessReturnVal(cond, ret);
    pthread_testcancel();
    return ret;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    int ret;
    int oldValue;

    if ((cond == NULL) || (mutex == NULL)) {
        return EINVAL;
    }

    if (CondInitCheck(cond)) {
        ret = pthread_cond_init(cond, NULL);
        if (ret != ENOERR) {
            return ret;
        }
    }

    oldValue = cond->value;

    (VOID)pthread_mutex_lock(cond->mutex);
    cond->count++;
    (VOID)pthread_mutex_unlock(cond->mutex);

    if (pthread_mutex_unlock(mutex) != ENOERR) {
        PRINT_ERR("%s: %d failed\n", __FUNCTION__, __LINE__);
    }

    ret = PthreadCondWaitSub(cond, oldValue, LOS_WAIT_FOREVER);

    if (pthread_mutex_lock(mutex) != ENOERR) {
        PRINT_ERR("%s: %d failed\n", __FUNCTION__, __LINE__);
    }

    switch (ret) {
        /* 0: event does not occur */
        case 0:
        case BROADCAST_EVENT:
            ret = ENOERR;
            break;
        default:
            PthreadCountSub(cond);
            ret = EINVAL;
            break;
    }

    return ret;
}
#else
/*
 * 描述: 当前运行的任务获取条件变量, 外部保证关中断
 */
static OS_SEC_L4_TEXT int OsCondPend(pthread_cond_t *restrict cond, pthread_mutex_t *restrict m, unsigned int timeout)
{
    struct TagTskCB *runTsk;
    struct TagOsRunQue *runq;

    if (timeout == OS_NO_WAIT) {
        return ETIMEDOUT;
    }

    if (OS_INT_ACTIVE) {
        return EINVAL;
    }

    if (OS_TASK_LOCK_DATA != 0) {
        return EINVAL;
    }

    runq = OsSpinLockRunTaskRq();
    runTsk = (struct TagTskCB *)RUNNING_TASK;

    /* Delete the task from the ready list */
    OsTskReadyDel(runTsk);
    runTsk->taskSem = (void *)cond;
    TSK_STATUS_SET(runTsk, OS_TSK_COND_PEND);

    /* 将任务挂到条件变量的阻塞链表 */
    OsSplLock(&cond->spinlock);
    ListTailAdd(&runTsk->pendList, (struct TagListObject *)(uintptr_t)&cond->head);
    OsSplUnlock(&cond->spinlock);

    OsTaskAttachToTimerList(runTsk, timeout);
    OsSpinUnLockRunTaskRq(runq);

    if (pthread_mutex_unlock(m) != ENOERR) {
        OS_GOTO_SYS_ERROR();
    }

#if defined(OS_OPTION_TICK)
    if (timeout != OS_WAIT_FOREVER) {
        /* 触发任务调度 */
        OsTskSchedule();

        runq = OsSpinLockRunTaskRq();
        /* 判断是否超时返回并做超时处理 */
        if (TSK_STATUS_TST(runTsk, OS_TSK_TIMEOUT)) {
            TSK_STATUS_CLEAR(runTsk, OS_TSK_TIMEOUT);
            OsSpinUnLockRunTaskRq(runq);
            if (pthread_mutex_lock(m) != ENOERR) {
                OS_GOTO_SYS_ERROR();
            }
            return ETIMEDOUT;
        }
        OsSpinUnLockRunTaskRq(runq);
    }
#endif

    OsTskScheduleFast();

    if (pthread_mutex_lock(m) != ENOERR) {
        OS_GOTO_SYS_ERROR();
    }

    return ENOERR;
}

/*
 * 描述: 唤醒阻塞于条件变量的任务, 外部保证关中断
 */
static OS_SEC_L4_TEXT int OsCondPost(struct TagTskCB *tcb)
{
    OsSpinLockTaskRq(tcb);

    /* 判断任务状态必须在上锁之后，否则有可能在上锁前任务被删除 */
    if (TSK_IS_UNUSED(tcb)) {
        OsSpinUnlockTaskRq(tcb);
        return EINVAL;
    }

    ListDelete(&tcb->pendList);
    TSK_STATUS_CLEAR(tcb, OS_TSK_COND_PEND);

    OsTaskDetachFromTimerList(tcb);

    if ((tcb->taskStatus & OS_TSK_SUSPEND_READY_BLOCK) == 0) {
        OsTskReadyAddBGD(tcb);
    }

    OsSpinUnlockTaskRq(tcb);
    return ENOERR;
}

static OS_SEC_L4_TEXT int private_cond_signal(pthread_cond_t *cond, bool isOnce)
{
    int ret = ENOERR;
    uintptr_t intSave;
    bool needSched = false;
    struct TagTskCB *tcb;
    struct TagListObject *head;

    intSave = OsIntLock();
    OsSplLock(&cond->spinlock);
    head = (struct TagListObject *)(uintptr_t)&cond->head;

    if (!ListEmpty(head)) {
        LIST_FOR_EACH(tcb, head, struct TagTskCB, pendList) {
            ret = OsCondPost(tcb);
            if (ret != ENOERR) {
                OsSplUnlock(&cond->spinlock);
                OsIntRestore(intSave);
                return EINVAL;
            }
            needSched = true;
            if (isOnce) {
                break;
            }
            tcb = LIST_COMPONENT(head, struct TagTskCB, pendList);
        }
    }

    OsSplUnlock(&cond->spinlock);

    if (needSched) {
        OsTskSchedule();
    }
    OsIntRestore(intSave);

    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_signal(pthread_cond_t *cond)
{
    if (!OsCondParamIsValid(cond)) {
        return EINVAL;
    }
    return private_cond_signal(cond, TRUE);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_broadcast(pthread_cond_t *cond)
{
    if (!OsCondParamIsValid(cond)) {
        return EINVAL;
    }
    return private_cond_signal(cond, FALSE);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_timedwait(pthread_cond_t *restrict cond,
    pthread_mutex_t *restrict m, const struct timespec *restrict ts)
{
    int ret;
    unsigned int ticks;
    uintptr_t intSave;
    if ((m == NULL) || (cond == NULL) || !OsMutexIsInit(m)) {
        return EINVAL;
    }

    if (!OsCondIsInit(cond)) { // 未初始化会新初始化一个
        ret = pthread_cond_init(cond, NULL);
        if (ret != ENOERR) {
            return ret;
        }
    }

    intSave = OsIntLock();
    if (ts == NULL) {
        ticks = OS_WAIT_FOREVER;
    } else {
        ret = OsTimeOut2Ticks(ts, &ticks);
        if (ret != ENOERR) {
            OsIntRestore(intSave);
            return ret;
        }
    }

    ret = OsCondPend(cond, m, ticks);
    OsIntRestore(intSave);
    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict m)
{
    return pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)m, NULL);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cond_clockwait(pthread_cond_t *restrict cond,
    pthread_mutex_t *restrict m, clockid_t clk, const struct timespec *restrict ts)
{
    (void)clk; // 不支持时钟选择
    return pthread_cond_timedwait((pthread_cond_t *)cond, (pthread_mutex_t *)m, ts);
}
#endif
