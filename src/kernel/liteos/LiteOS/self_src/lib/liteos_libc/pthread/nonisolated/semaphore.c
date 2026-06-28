/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Semaphore file
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
#include "semaphore.h"
#include "limits.h"
#include "sys/types.h"
#include "errno.h"
#include "los_sem_pri.h"
#include "time_pri.h"
#include "los_sys.h"
#else
#include "fcntl.h"
#include "../../include/posix_adapt.h"
#include "drb_sem.h"
#include "drb_atomic.h"
#include "semaphore.h"
#endif

#if defined(__LITEOS__)
STATIC INT32 MapErrno(UINT32 err)
{
    INT32 posixRet;

    if (err == LOS_OK) {
        return ENOERR;
    }
    switch (err) {
        case LOS_ERRNO_SEM_ALL_BUSY:
            posixRet = ENOSPC;
            break;
        case LOS_ERRNO_SEM_INVALID:
        case LOS_ERRNO_SEM_UNAVAILABLE:
            posixRet = EINVAL;
            break;
        case LOS_ERRNO_SEM_PENDED:
            posixRet = EBUSY;
            break;
        case LOS_ERRNO_SEM_PEND_IN_LOCK:
            posixRet = EPERM;
            break;
        case LOS_ERRNO_SEM_PEND_INTERR:
            posixRet = EINTR;
            break;
        case LOS_ERRNO_SEM_TIMEOUT:
            posixRet = ETIMEDOUT;
            break;
        case LOS_ERRNO_SEM_OVERFLOW:
            posixRet = ENOSPC;
            break;
        default:
            posixRet = EINVAL;
            break;
    }
    errno = posixRet;

    return posixRet;
}

static inline unsigned int SemaphoreGetId(const sem_t *sem)
{
    return ((LosSemCB *)(sem->sem))->semId;
}
static inline unsigned short SemaphoreGetCount(const sem_t *sem)
{
    return ((LosSemCB *)(sem->sem))->semCount;
}
/* Initialize semaphore to value, shared is not supported in Huawei LiteOS. */
int sem_init(sem_t *sem, int shared, unsigned int value)
{
    UINT32 semHandle = 0;
    UINT32 ret;

    (VOID)shared;
    if ((sem == NULL) || (value > SEM_VALUE_MAX)) {
        errno = EINVAL;
        return -1;
    }

    ret = LOS_SemCreate(value, &semHandle);
    if (MapErrno(ret) != ENOERR) {
        return -1;
    }

    sem->sem = GET_SEM(semHandle);

    return 0;
}

int sem_destroy(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || (sem->sem == NULL)) {
        errno = EINVAL;
        return -1;
    }

    ret = LOS_SemDelete(SemaphoreGetId(sem));
    if (MapErrno(ret) != ENOERR) {
        return -1;
    }
    return 0;
}

int sem_wait(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || (sem->sem == NULL)) {
        errno = EINVAL;
        return -1;
    }

    ret = LOS_SemPend(SemaphoreGetId(sem), LOS_WAIT_FOREVER);
    if (MapErrno(ret) == ENOERR) {
        return 0;
    } else {
        return -1;
    }
}

int sem_trywait(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || (sem->sem == NULL)) {
        errno = EINVAL;
        return -1;
    }

    ret = LOS_SemPend(SemaphoreGetId(sem), LOS_NO_WAIT);
    if (MapErrno(ret) == ENOERR) {
        return 0;
    } else {
        if ((errno != EINVAL) || (ret == LOS_ERRNO_SEM_UNAVAILABLE)) {
            errno = EAGAIN;
        }
        return -1;
    }
}

int sem_timedwait(sem_t *sem, const struct timespec *timeout)
{
    UINT32 ret;
    UINT32 tickCnt;

    if ((sem == NULL) || (sem->sem == NULL)) {
        errno = EINVAL;
        return -1;
    }

    if (!ValidTimespec(timeout)) {
        errno = EINVAL;
        return -1;
    }

    tickCnt = OsTimespec2Tick(timeout);
    ret = LOS_SemPend(SemaphoreGetId(sem), tickCnt);
    if (MapErrno(ret) == ENOERR) {
        return 0;
    } else {
        return -1;
    }
}

int sem_post(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || (sem->sem == NULL)) {
        errno = EINVAL;
        return -1;
    }

    ret = LOS_SemPost(SemaphoreGetId(sem));
    if (MapErrno(ret) != ENOERR) {
        return -1;
    }

    return 0;
}

int sem_getvalue(sem_t *sem, int *currVal)
{
    INT32 val;

    if ((sem == NULL) || (sem->sem == NULL) || (currVal == NULL)) {
        errno = EINVAL;
        return -1;
    }
    val = SemaphoreGetCount(sem);

    *currVal = val;
    return 0;
}

sem_t *sem_open(const char *name, int openFlag, ...)
{
    (VOID)name;
    (VOID)openFlag;
    errno = ENOSYS;
    return NULL;
}

int sem_close(sem_t *sem)
{
    (VOID)sem;
    errno = ENOSYS;
    return -1;
}

int sem_unlink(const char *name)
{
    (VOID)name;
    errno = ENOSYS;
    return -1;
}
#else
OS_SEC_L4_BSS struct SreSpinLock g_semaphoreInitLock;
#define SEMAPHORE_INIT_IRQ_LOCK(state)  (state) = DRB_SplIrqLock(&g_semaphoreInitLock)
#define SEMAPHORE_INIT_IRQ_UNLOCK(state) DRB_SplIrqUnlock(&g_semaphoreInitLock, (state))

OS_SEC_L4_TEXT U32 OsPosixSemaphoreInit(void)
{
    return DRB_SplLockInit(&g_semaphoreInitLock);
}
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_close(sem_t *sem)
{
    uintptr_t intSave;
    struct TagSemCB *semCb;
    int ret = ENOERR;

    if (sem == NULL || sem->semHandle >= g_maxSem) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    SEMAPHORE_INIT_IRQ_LOCK(intSave);
    semCb = GET_SEM(sem->semHandle);
    if (OsSemIsUnused(semCb)) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    if (semCb->refCount <= 0) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    semCb->refCount--;
    /* 如果先前已调用sem_unlink，则已变为无名信号量；若本次调用后不再有引用，则要删除信号量 */
    if ((semCb->refCount == 0) && (semCb->name[0] == '\0')) {
        ret = (int)DRB_SemDelete((SemHandle)(semCb->semID));
        if (ret != ENOERR) {
            SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
            errno = EINVAL;
            return PTHREAD_OP_FAIL;
        }
    }
    SEMAPHORE_INIT_IRQ_UNLOCK(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_destroy(sem_t *sem)
{
    unsigned int ret;
    uintptr_t intSave;

    if (sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    SEMAPHORE_INIT_IRQ_LOCK(intSave);
    ret = DRB_SemDelete(sem->semHandle);
    if (ret != ENOERR) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = (ret == OS_ERRNO_SEM_PENDED) ? EBUSY : EINVAL;
        return PTHREAD_OP_FAIL;
    }

    SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_getvalue(sem_t *restrict sem, int *restrict val)
{
    unsigned int ret;

    if (val == NULL || sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    ret = DRB_SemGetCnt(sem->semHandle, (U32 *)(uintptr_t)val);
    if (ret != ENOERR) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_init(sem_t *sem, int shared, unsigned int value)
{
    unsigned int ret;
    uintptr_t intSave;

    (void)shared;
    if (sem == NULL || value > OS_SEM_COUNT_MAX) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    // 初始化时使用semaphore创建删除锁，主要保护posix新增的几个参数的并发修改
    SEMAPHORE_INIT_IRQ_LOCK(intSave);
    ret = DRB_SemCreate(value, &sem->semHandle);
    if (ret != ENOERR) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = (ret == OS_ERRNO_SEM_ALL_BUSY) ? ENOSPC : EINVAL;
        return PTHREAD_OP_FAIL;
    }
    SEMAPHORE_INIT_IRQ_UNLOCK(intSave);

    return ENOERR;
}

INLINE bool OsSemCheckIsCreatedByName(const char *name, struct TagSemCB **semCb)
{
    int i;
    for (i = 0; i < g_maxSem; i++) {
        *semCb = GET_SEM(i);
        if (OsSemIsUnused(*semCb)) {
            continue;
        }
        if (strcmp((const char *)(*semCb)->name, name) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

INLINE sem_t *OsSemCreateNew(unsigned int val, const char *name)
{
    unsigned int ret;
    unsigned short semId;
    struct TagSemCB *semCb;
    struct OsMemCbInfo mci = {0};

    ret = OsSemPosixCreate(val, SEM_T_COUNT, SEM_MODE_FIFO, &semId, &mci);
    if (ret != ENOERR) {
        errno = (ret == OS_ERRNO_SEM_ALL_BUSY) ? ENOSPC : EINVAL;
        return SEM_FAILED;
    }

    semCb = GET_SEM(semId);
    if (strncpy_s((char *)semCb->name, OS_SEM_NAME_LEN, name, strlen(name)) != EOK) {
        OS_GOTO_SYS_ERROR();
    }

    return (sem_t *)&(semCb->handle);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT sem_t *sem_open(const char *name, int flags, ...)
{
    unsigned int val;
    int mode;
    va_list arg;
    uintptr_t intSave;
    struct TagSemCB *semCb;
    sem_t *sem;

    if (name == NULL || strlen(name) == 0 || strlen(name) >= OS_SEM_NAME_LEN) {
        errno = EINVAL;
        return SEM_FAILED;
    }

    SEMAPHORE_INIT_IRQ_LOCK(intSave);
    if (OsSemCheckIsCreatedByName(name, &semCb)) {
        if (((U32)flags & (O_EXCL | O_CREAT)) == (O_EXCL | O_CREAT)) {
            SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
            errno = EEXIST;
            return SEM_FAILED;
        }
        semCb->refCount++;
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        return (sem_t *)&(semCb->handle);
    }
    if (((U32)flags & O_CREAT) == 0) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = ENOENT;
        return SEM_FAILED;
    }

    va_start(arg, flags);
    mode = (int)va_arg(arg, unsigned int);
    val = va_arg(arg, unsigned int);
    va_end(arg);
    (void)mode;

    if (val > OS_SEM_COUNT_MAX) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = EINVAL;
        return SEM_FAILED;
    }

    sem = OsSemCreateNew(val, name);
    SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
    return sem;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_post(sem_t *sem)
{
    unsigned int ret;

    if (sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    ret = DRB_SemPost(sem->semHandle);
    if (ret != ENOERR) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_timedwait(sem_t *restrict sem, const struct timespec *restrict at)
{
    unsigned int ret;
    unsigned int ticks;
    uintptr_t intSave;

    if (at == NULL || sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    /* 防止被中断打断后，超时时间不符合预期 */
    intSave = OsIntLock();
    ret = (unsigned int)OsTimeOut2Ticks(at, &ticks);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    ret = DRB_SemPend(sem->semHandle, ticks);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        errno = (ret == OS_ERRNO_SEM_TIMEOUT) ? ETIMEDOUT : EINVAL;
        return PTHREAD_OP_FAIL;
    }
    OsIntRestore(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_trywait(sem_t *sem)
{
    unsigned int ret;

    if (sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    ret = DRB_SemPend(sem->semHandle, OS_NO_WAIT);
    if (ret != ENOERR) {
        errno = EAGAIN;
        return PTHREAD_OP_FAIL;
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_unlink(const char *name)
{
    uintptr_t intSave;
    struct TagSemCB *semCb;
    unsigned int ret;

    if (name == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    SEMAPHORE_INIT_IRQ_LOCK(intSave);
    if (!OsSemCheckIsCreatedByName(name, &semCb)) {
        SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
        errno = ENOENT;
        return PTHREAD_OP_FAIL;
    }

    // 若信号量没有其他引用了，就直接删除
    if (semCb->refCount == 0) {
        ret = DRB_SemDelete((SemHandle)semCb->semID);
        if (ret != ENOERR) {
            SEMAPHORE_INIT_IRQ_UNLOCK(intSave);
            errno = EINVAL;
            return PTHREAD_OP_FAIL;
        }
    }

    // 若信号量还有其他引用，则仅清除名字，即退化为无名信号量使用；原有引用还能正常使用，但是没法创建新的引用
    (void)memset_s(semCb->name, OS_SEM_NAME_LEN, 0, OS_SEM_NAME_LEN);
    SEMAPHORE_INIT_IRQ_UNLOCK(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int sem_wait(sem_t *sem)
{
    unsigned int ret;

    if (sem == NULL) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    ret = DRB_SemPend(sem->semHandle, OS_WAIT_FOREVER);
    if (ret != ENOERR) {
        errno = EINVAL;
        return PTHREAD_OP_FAIL;
    }

    return ENOERR;
}
#endif
