/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Pthread mutex file
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
#include "pthread.h"
#include "stdlib.h"
#include "errno.h"
#include "los_bitmap.h"
#include "los_mux_pri.h"
#include "los_mux_debug_pri.h"
#include "los_task_pri.h"
#include "los_mp_pri.h"
#include "time_pri.h"
#include "los_lockdep.h"
#include "pthread_mux_debug_pri.h"
#include "los_lockdep_pri.h"
#else
#include "errno.h"
#include "pthread.h"
#include "time.h"
#include "../../include/posix_adapt.h"
#include "drb_sem.h"
#endif

#define MUTEXATTR_TYPE_MASK 0x0fU

#if defined(__LITEOS__)
#define OS_SEC_L4_TEXT
#define DEFAULT_VISIBILITY
#define PTHREAD_PRIORITY_LOWEST LOS_TASK_PRIORITY_LOWEST
#define PTHREAD_MUTEX_TYPE_IS_INVALID(type) ((type) < PTHREAD_MUTEX_NORMAL || (type) > PTHREAD_MUTEX_ERRORCHECK)
#define PTHREAD_MUTEXATTR_GET_TYPE(type) ((INT32)((type) & MUTEXATTR_TYPE_MASK))
#define PTHREAD_MUTEXATTR_SET_TYPE(type) ((UINT8)(((type) & ~MUTEXATTR_TYPE_MASK) | (UINT32)(type)))

STATIC INLINE INT32 WrapMutexAttrSetProtocol(pthread_mutexattr_t *attr, int protocol)
{
    switch (protocol) {
        case PTHREAD_PRIO_NONE:
        case PTHREAD_PRIO_INHERIT:
        case PTHREAD_PRIO_PROTECT:
            attr->protocol = (UINT8)protocol;
            return ENOERR;
        default:
            return EINVAL;
    }
}
#else
#define PTHREAD_PRIORITY_LOWEST OS_TSK_PRIORITY_LOWEST
#if defined(OS_OPTION_SEM_RECUR_PV)
#define PTHREAD_MUTEX_TYPE_IS_INVALID(type) ((type) < PTHREAD_MUTEX_NORMAL || (type) > PTHREAD_MUTEX_RECURSIVE)
#else
#define PTHREAD_MUTEX_TYPE_IS_INVALID(type) ((type) < PTHREAD_MUTEX_NORMAL || (type) > PTHREAD_MUTEX_DEFAULT)
#endif
#define PTHREAD_MUTEXATTR_GET_TYPE(type) ((int)((type) & MUTEXATTR_TYPE_MASK))
#define PTHREAD_MUTEXATTR_SET_TYPE(type) ((unsigned char)(type))

INLINE int WrapMutexAttrSetProtocol(pthread_mutexattr_t *attr, int protocol)
{
    switch (protocol) {
        case PTHREAD_PRIO_NONE:
        case PTHREAD_PRIO_INHERIT:
            attr->protocol = (unsigned char)protocol;
            return ENOERR;
        case PTHREAD_PRIO_PROTECT: // 会阻止优先级高于mutexAttrPrioceiling的Thread, ck不支持
            return ENOTSUP;
        default:
            return EINVAL;
    }
}
#endif

#if defined(__LITEOS__)
STATIC UINT32 OsMuxPendForPosix(pthread_mutex_t *mutex, UINT32 timeout);
STATIC INT32 OsMuxPostForPosix(pthread_mutex_t *mutex);

STATIC INT32 MapErrno(UINT32 err)
{
    INT32 posixRet;

    if (err == LOS_OK) {
        return ENOERR;
    }
    switch (err) {
        case LOS_ERRNO_MUX_INVALID:
        case LOS_ERRNO_MUX_PTR_NULL:
            posixRet = EINVAL;
            break;
        case LOS_ERRNO_MUX_PENDED:
        case LOS_ERRNO_MUX_UNAVAILABLE:
            posixRet = EBUSY;
            break;
        case LOS_ERRNO_MUX_ALL_BUSY:
            posixRet = EAGAIN;
            break;
        case LOS_ERRNO_MUX_PEND_IN_LOCK:
            posixRet = EDEADLK;
            break;
        case LOS_ERRNO_MUX_PEND_INTERR:
            posixRet = EPERM;
            break;
        case LOS_ERRNO_MUX_TIMEOUT:
            posixRet = ETIMEDOUT;
            break;
        default:
            posixRet = EINVAL;
            break;
    }
    errno = posixRet;
    return posixRet;
}
#endif

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_init(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

#if defined POSIX_MUTEX_DEFAULT_INHERIT
    attr->protocol    = PTHREAD_PRIO_INHERIT;
#elif defined POSIX_MUTEX_DEFAULT_PROTECT
    attr->protocol    = PTHREAD_PRIO_PROTECT;
#else
    attr->protocol    = PTHREAD_PRIO_NONE;
#endif
    attr->prioceiling = PTHREAD_PRIORITY_LOWEST;
    attr->type        = PTHREAD_MUTEX_DEFAULT;
    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_destroy(pthread_mutexattr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    (VOID)memset(attr, 0, sizeof(pthread_mutexattr_t));

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol)
{
    if (attr == NULL) {
        return EINVAL;
    }

    return WrapMutexAttrSetProtocol(attr, protocol);
}

#if defined(__LITEOS__)
int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *attr, int *protocol)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_getprotocol(const pthread_mutexattr_t *restrict attr,
    int *restrict protocol)
#endif
{
    if ((attr != NULL) && (protocol != NULL)) {
        *protocol = attr->protocol;
    } else {
        return EINVAL;
    }

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr, int prioceiling)
{
    if ((attr == NULL) ||
        (prioceiling < LOS_TASK_PRIORITY_HIGHEST) ||
        (prioceiling > LOS_TASK_PRIORITY_LOWEST)) {
        return EINVAL;
    }

    attr->prioceiling = (UINT8)prioceiling;

    return ENOERR;
}

int pthread_mutexattr_getprioceiling(const pthread_mutexattr_t *attr, int *prioceiling)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if (prioceiling != NULL) {
        *prioceiling = attr->prioceiling;
    }

    return ENOERR;
}

int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *oldPrioceiling)
{
    INT32 ret;
    INT32 retLock;
    if ((mutex == NULL) ||
        (prioceiling < LOS_TASK_PRIORITY_HIGHEST) ||
        (prioceiling > LOS_TASK_PRIORITY_LOWEST)) {
        return EINVAL;
    }

    retLock = pthread_mutex_lock(mutex);
    if (retLock != ENOERR) {
        return retLock;
    }

    if (oldPrioceiling != NULL) {
        *oldPrioceiling = mutex->stAttr.prioceiling;
    }

    ret = pthread_mutexattr_setprioceiling(&mutex->stAttr, prioceiling);

    retLock = pthread_mutex_unlock(mutex);
    if ((ret == ENOERR) && (retLock != ENOERR)) {
        return retLock;
    }

    return ret;
}

int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling)
{
    if ((mutex != NULL) && (prioceiling != NULL)) {
        *prioceiling = mutex->stAttr.prioceiling;
        return ENOERR;
    }
    return EINVAL;
}
#endif

#if defined(__LITEOS__)
int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int *outType)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_gettype(const pthread_mutexattr_t *restrict attr,
    int *restrict outType)
#endif
{
    INT32 type;

    if ((attr == NULL) || (outType == NULL)) {
        return EINVAL;
    }

    type = PTHREAD_MUTEXATTR_GET_TYPE(attr->type);
    if (PTHREAD_MUTEX_TYPE_IS_INVALID(type)) {
        return EINVAL;
    }

    *outType = type;
    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if ((attr == NULL) ||
        PTHREAD_MUTEX_TYPE_IS_INVALID(type)) {
        return EINVAL;
    }

    attr->type = PTHREAD_MUTEXATTR_SET_TYPE((UINT32)(type));
    return ENOERR;
}

#if defined(__LITEOS__)
STATIC UINT32 CheckMutexAttr(const pthread_mutexattr_t *attr)
{
    if (((INT8)(attr->type) < PTHREAD_MUTEX_NORMAL) ||
        (attr->type > PTHREAD_MUTEX_ERRORCHECK)) {
        return LOS_NOK;
    }
    if (((INT8)(attr->prioceiling) < LOS_TASK_PRIORITY_HIGHEST) ||
        (attr->prioceiling > LOS_TASK_PRIORITY_LOWEST)) {
        return LOS_NOK;
    }
    if (((INT8)(attr->protocol) < PTHREAD_PRIO_NONE) ||
        (attr->protocol > PTHREAD_PRIO_PROTECT)) {
        return LOS_NOK;
    }
    return LOS_OK;
}

/* Initialize mutex. If mutexAttr is NULL, use default attributes. */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexAttr)
{
    pthread_mutexattr_t useAttr;
    MuxBaseCB *muxCreated = NULL;
    UINT32 intSave;

    if (mutex == NULL) {
        return EINVAL;
    }

    /* Set up the attributes we are going to use. */
    if (mutexAttr == NULL) {
        (VOID)pthread_mutexattr_init(&useAttr);
    } else {
        useAttr = *mutexAttr;
    }

    if (CheckMutexAttr(&useAttr) != LOS_OK) {
        return EINVAL;
    }

    mutex->stAttr = useAttr;
    muxCreated = &mutex->stLock;

    SCHEDULER_LOCK(intSave);
    muxCreated->muxCount = 0;
    muxCreated->owner = NULL;
    PTHREAD_MUXDEP_CHECK_INIT();
    LOS_ListInit(&muxCreated->muxList);
    SCHEDULER_UNLOCK(intSave);

    return ENOERR;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    MuxBaseCB *muxDeleted = NULL;
    UINT32 intSave;

    if (mutex == NULL) {
        return EINVAL;
    }

    muxDeleted = &mutex->stLock;

    SCHEDULER_LOCK(intSave);
    if (muxDeleted->muxCount != 0) {
        SCHEDULER_UNLOCK(intSave);
        return EBUSY;
    }
    (VOID)memset(mutex, 0, sizeof(pthread_mutex_t));

    SCHEDULER_UNLOCK(intSave);
    return ENOERR;
}

UINT32 OsMuxPreCheck(const pthread_mutex_t *mutex, const LosTaskCB *runTask)
{
    if (mutex == NULL) {
        return EINVAL;
    }

    if (OS_INT_ACTIVE) {
        return EPERM;
    }

    /* DO NOT recommend to use blocking API in system tasks */
    if (runTask->taskFlags & OS_TASK_FLAG_SYSTEM) {
        PRINT_DEBUG("Warning: DO NOT recommend to use %s in system tasks.\n", __FUNCTION__);
    }

    if (CheckMutexAttr(&mutex->stAttr) != LOS_OK) {
        return EINVAL;
    }

    return ENOERR;
}

/* Lock mutex, waiting for it if necessary. */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    MuxBaseCB *muxPended = NULL;
    LosTaskCB *runTask = NULL;
    UINT32 ret;

    runTask = (LosTaskCB*)OsCurrTaskGet();
    ret = OsMuxPreCheck(mutex, runTask);
    if (ret != ENOERR) {
        return (INT32)ret;
    }

    muxPended = &mutex->stLock;
    if ((mutex->stAttr.type == PTHREAD_MUTEX_ERRORCHECK) &&
        (muxPended->muxCount != 0) &&
        (muxPended->owner == runTask)) {
        return EDEADLK;
    }

    ret = OsMuxPendForPosix(mutex, LOS_WAIT_FOREVER);
    return MapErrno(ret);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    MuxBaseCB *muxPended = NULL;
    LosTaskCB *runTask = NULL;
    UINT32 ret;

    runTask = (LosTaskCB*)OsCurrTaskGet();
    ret = OsMuxPreCheck(mutex, runTask);
    if (ret != ENOERR) {
        return (INT32)ret;
    }

    muxPended = &mutex->stLock;

    if ((mutex->stLock.owner != NULL) && (mutex->stLock.owner != runTask)) {
        return EBUSY;
    }
    if ((mutex->stAttr.type != PTHREAD_MUTEX_RECURSIVE) && (muxPended->muxCount != 0)) {
        return EBUSY;
    }

    ret = OsMuxPendForPosix(mutex, 0);
    return MapErrno(ret);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    LosTaskCB *runTask = NULL;
    UINT32 ret;

    runTask = (LosTaskCB*)OsCurrTaskGet();
    ret = OsMuxPreCheck(mutex, runTask);
    if (ret != ENOERR) {
        return (INT32)ret;
    }

    return OsMuxPostForPosix(mutex);
}

STATIC VOID OsMuxBitmapSet(const pthread_mutex_t *mutex, const LosTaskCB *runTask, const MuxBaseCB *muxPended)
{
    if ((GET_MUX_OWNER(muxPended)->priority > runTask->priority) &&
        (mutex->stAttr.protocol == PTHREAD_PRIO_INHERIT)) {
        LOS_BitmapSet(&(GET_MUX_OWNER(muxPended)->priBitMap), GET_MUX_OWNER(muxPended)->priority);
        OsSchedPrioModify(GET_MUX_OWNER(muxPended), runTask->priority);
    }
}

STATIC UINT32 OsMuxPendForPosix(pthread_mutex_t *mutex, UINT32 timeout)
{
    UINT32 retErr = LOS_OK;
    UINT32 intSave;
    MuxBaseCB *muxPended = &mutex->stLock;
    LosTaskCB *runTask = NULL;

    SCHEDULER_LOCK(intSave);
    if ((muxPended->muxList.pstPrev == NULL) ||
        (muxPended->muxList.pstNext == NULL)) {
        /* This is for mutex macro initialization. */
        muxPended->muxCount = 0;
        muxPended->owner = NULL;
        PTHREAD_MUXDEP_CHECK_INIT();
        LOS_ListInit(&muxPended->muxList);
    }
    runTask = OsCurrTaskGet();
    if (muxPended->muxCount == 0) {
        PTHREAD_MUXDEP_INSERT(muxPended);
        PTHREAD_MUXDEP_CHECK_IN(muxPended);
        muxPended->muxCount++;
        muxPended->owner = runTask;
        if ((runTask->priority > mutex->stAttr.prioceiling) &&
            (mutex->stAttr.protocol == PTHREAD_PRIO_PROTECT)) {
            LOS_BitmapSet(&runTask->priBitMap, runTask->priority);
            OsSchedPrioModify(runTask, mutex->stAttr.prioceiling);
        }
        goto DEP_RECORD;
    }

    if ((muxPended->owner == runTask) &&
        (mutex->stAttr.type == PTHREAD_MUTEX_RECURSIVE)) {
        muxPended->muxCount++;
        goto OUT_UNLOCK;
    }

    if (!OsPreemptableInSched()) {
        retErr = LOS_ERRNO_MUX_PEND_IN_LOCK;
        goto OUT_UNLOCK;
    }

    PTHREAD_MUXDEP_CHECK_IN(muxPended);
    PTHREAD_MUXDEP_INSERT(muxPended);

    OsMuxBitmapSet(mutex, runTask, muxPended);
    retErr = OsMuxPendOp(runTask, muxPended, timeout);
    if (retErr != LOS_OK) {
        goto OUT_UNLOCK;
    }

DEP_RECORD:
    PTHREAD_MUXDEP_RECORD(muxPended);
OUT_UNLOCK:
    SCHEDULER_UNLOCK(intSave);
    return retErr;
}

STATIC INT32 OsMuxPostForPosix(pthread_mutex_t *mutex)
{
    UINT32 intSave;
    UINT32 ret;
    UINT16 bitMapPri;
    MuxBaseCB *muxPosted = &mutex->stLock;
    LosTaskCB *runTask = NULL;

    SCHEDULER_LOCK(intSave);

    if (muxPosted->muxCount == 0) {
        SCHEDULER_UNLOCK(intSave);
        return EPERM;
    }

    runTask = OsCurrTaskGet();
    if (muxPosted->owner != runTask) {
        SCHEDULER_UNLOCK(intSave);
        return EPERM;
    }

    if ((--muxPosted->muxCount != 0) &&
        (mutex->stAttr.type == PTHREAD_MUTEX_RECURSIVE)) {
        SCHEDULER_UNLOCK(intSave);
        return ENOERR;
    }

    if (mutex->stAttr.protocol == PTHREAD_PRIO_PROTECT) {
        bitMapPri = LOS_HighBitGet(runTask->priBitMap);
        if (bitMapPri != LOS_INVALID_BIT_INDEX) {
            LOS_BitmapClr(&runTask->priBitMap, bitMapPri);
            OsSchedPrioModify(runTask, bitMapPri);
        }
    }

    /* Whether a task block the mutex lock. */
    ret = OsMuxPostOp(runTask, (MuxBaseCB *)muxPosted);
    PTHREAD_MUXDEP_CHECK_OUT(muxPosted);
    PTHREAD_MUXDEP_CHECK_DEINIT(muxPosted);
    SCHEDULER_UNLOCK(intSave);
    if ((ret == MUX_SCHEDULE) ||
        (mutex->stAttr.protocol == PTHREAD_PRIO_PROTECT)) {
        LOS_MpSchedule(OS_MP_CPU_ALL);
        LOS_Schedule();
    }

    return ENOERR;
}

int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *absTime)
{
    MuxBaseCB *muxPended = NULL;
    LosTaskCB *runTask = NULL;
    UINT32 ret;
    UINT32 absTicks;
    struct timespec to = {0};

    runTask = (LosTaskCB*)OsCurrTaskGet();
    ret = OsMuxPreCheck(mutex, runTask);
    if (ret != ENOERR) {
        return (INT32)ret;
    }

    muxPended = &mutex->stLock;

    if ((mutex->stAttr.type == PTHREAD_MUTEX_ERRORCHECK) &&
        (muxPended->muxCount != 0) &&
        (muxPended->owner == runTask)) {
        return EDEADLK;
    }

    if ((absTime == NULL) || (!ValidTimespec(absTime)) || clock_gettime(CLOCK_REALTIME, &to)) {
        return EINVAL;
    }

    /* calculate abs time */
    to.tv_sec = absTime->tv_sec - to.tv_sec;
    to.tv_nsec = absTime->tv_nsec - to.tv_nsec;
    if (to.tv_nsec < 0) {
        to.tv_sec--;
        to.tv_nsec += OS_SYS_NS_PER_SECOND;
    }
    if (to.tv_sec < 0) {
        return ETIMEDOUT;
    }
    /* convert to tick */
    absTicks = OsTimespec2Tick(&to);

    ret = OsMuxPendForPosix(mutex, absTicks);
    return MapErrno(ret);
}
#else
INLINE bool OsMutexIsUninit(pthread_mutex_t *mutex)
{
    return (mutex == NULL || !OsMutexIsInit(mutex));
}

/* pthread mutex attr中的proctol属性无效，是否开启优先级继承由宏控制 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_init(pthread_mutex_t *restrict mutex,
    const pthread_mutexattr_t *restrict attr)
{
    int ret;
    struct OsMemCbInfo mci = {0};

    if (mutex == NULL || OsMutexIsInit(mutex)) { // 不允许重复初始化
        return EINVAL;
    }

    if (attr == NULL) {
        mutex->type = PTHREAD_MUTEX_DEFAULT;
    } else {
        mutex->type = (unsigned char)attr->type;
    }

    switch (mutex->type) {
        case PTHREAD_MUTEX_DEFAULT:
            ret = (int)OsSemPosixCreate(OS_SEM_FULL, SEM_T_BIN_MUTEX,
                SEM_MODE_PRIOR, (SemHandle *)&mutex->mutex_sem, &mci);
            break;
        case PTHREAD_MUTEX_RECURSIVE:
#if defined(OS_OPTION_SEM_RECUR_PV)
            ret = (int)OsSemPosixCreate(OS_SEM_FULL, SEM_T_BIN_RCMUTEX,
                SEM_MODE_PRIOR, (SemHandle *)&mutex->mutex_sem, &mci);
            break;
#endif
        case PTHREAD_MUTEX_ERRORCHECK:
            return ENOTSUP;
        default:
            ret = EINVAL;
            break;
    }
    if (ret != ENOERR) {
        return (ret == OS_ERRNO_SEM_ALL_BUSY) ? EAGAIN : EINVAL;
    }
    mutex->magic = PTHREAD_MUTEX_INIT_MAGIC;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    int ret;

    if (OsMutexIsUninit(mutex)) {
        return EINVAL;
    }

    ret = (int)DRB_SemDelete(mutex->mutex_sem);
    if (ret != ENOERR) {
        return (ret == OS_ERRNO_SEM_PENDED) ? EBUSY : EINVAL;
    }
    mutex->magic = PTHREAD_MUTEX_DESTROY_MAGIC;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    if (OsMutexIsUninit(mutex)) {
        return EINVAL;
    }

    return (DRB_SemPend(mutex->mutex_sem, OS_WAIT_FOREVER) == ENOERR) ? ENOERR : EINVAL;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    int ret;

    if (OsMutexIsUninit(mutex)) {
        return EINVAL;
    }

    ret = (int)DRB_SemPend(mutex->mutex_sem, OS_NO_WAIT);
    if (ret == OS_ERRNO_SEM_UNAVAILABLE) {
        ret = EBUSY;
    } else if (ret != ENOERR) {
        ret = EINVAL;
    }

    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    int ret;

    if (OsMutexIsUninit(mutex)) {
        return EINVAL;
    }

    ret = (int)DRB_SemPost(mutex->mutex_sem);
    if (ret == OS_ERRNO_SEM_MUTEX_NOT_OWNER_POST) {
        ret = EPERM;
    } else if (ret != ENOERR) {
        ret = EINVAL;
    }

    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
    const struct timespec *restrict time)
{
    int ret;
    uintptr_t intSave;
    unsigned int ticks;

    if (time == NULL) {
        return EINVAL;
    }

    if (OsMutexIsUninit(mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    ret = OsTimeOut2Ticks(time, &ticks);
    /*
     * 标准手册说明，若当前mutex是空闲可获取的，即使已经超时也不应该返回错误码，而应该直接获取，因此需要尝试获取一次
     * "Under no circumstance shall the function fail with a timeout if the mutex can be locked immediately"
     */
    if (ret == ETIMEDOUT) {
        ticks = OS_NO_WAIT;
    } else if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    ret = (int)DRB_SemPend(mutex->mutex_sem, ticks);
    if (ret == OS_ERRNO_SEM_TIMEOUT || ret == OS_ERRNO_SEM_UNAVAILABLE) {
        ret = ETIMEDOUT;
    } else if (ret != ENOERR) {
        ret = EINVAL;
    }
    OsIntRestore(intSave);

    return ret;
}
#endif
