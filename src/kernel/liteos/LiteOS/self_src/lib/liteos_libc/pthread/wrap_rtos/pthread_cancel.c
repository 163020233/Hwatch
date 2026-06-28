/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread cancel implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "pthread.h"
#include "../../include/posix_adapt.h"
#include "drb_sem.h"

/*
 * 描述: 调用者保证关中断
 */
static OS_SEC_L4_TEXT int OsPthreadCancelOthers(struct TagTskCB *tskCb)
{
    int ret = ENOERR;
    pthread_t pid;
    unsigned char cancelState;
    unsigned char cancelType;

    OsSpinLockTaskRq(tskCb);
    if (TSK_IS_UNUSED(tskCb)) {
        OsSpinUnlockTaskRq(tskCb);
        return ESRCH; // 此时任务已经被删除了
    }

    if (!OsPthreadIsPosix(tskCb)) { // 不允许操作非posix创建的任务
        OsSpinUnlockTaskRq(tskCb);
        return EINVAL;
    }

    tskCb->cancelPending = 1;
    pid = tskCb->taskPID;
    cancelType = tskCb->cancelType;
    cancelState = tskCb->cancelState;
    OsSpinUnlockTaskRq(tskCb);

    if (cancelType == PTHREAD_CANCEL_DEFERRED) {
        return ENOERR;
    }

    if (cancelState == PTHREAD_CANCEL_ENABLE) {
        ret = (DRB_TaskDelete(pid) == ENOERR) ? ENOERR : EAGAIN;
    }
    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_cancel(pthread_t thread)
{
    struct TagTskCB *tskCb;
    uintptr_t intSave;
    int ret = ENOERR;
    unsigned char cancelState;
    unsigned char cancelType;
    unsigned int pid;
    bool isPosix;

    if (CHECK_TSK_PID((TskHandle)thread)) { // idle任务不能被取消
        return ESRCH;
    }

    intSave = OsIntLock();
    tskCb = RUNNING_TASK;
    OsSpinLockTaskRq(tskCb);
    pid = tskCb->taskPID;
    cancelType = tskCb->cancelType;
    cancelState = tskCb->cancelState;
    isPosix = tskCb->isPosix;
    OsSpinUnlockTaskRq(tskCb);
    if ((TskHandle)thread == pid) { // cancel自己
        if (!isPosix) { // 不允许操作非posix创建的任务
            return EINVAL;
        }
        if (cancelState == PTHREAD_CANCEL_ENABLE && cancelType == PTHREAD_CANCEL_ASYNCHRONOUS) {
            OsPthreadExit(PTHREAD_CANCELED);
        }
        OsIntRestore(intSave);
        return ENOERR;
    }

    tskCb = GET_TCB_HANDLE((TskHandle)thread);
    ret = OsPthreadCancelOthers(tskCb); // cancel其他任务
    OsIntRestore(intSave);

    return ret;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_setcancelstate(int state, int *oldstate)
{
    uintptr_t intSave;
    struct TagTskCB *tskCb;

    if (oldstate == NULL) {
        return EINVAL;
    }

    if (state != PTHREAD_CANCEL_ENABLE && state != PTHREAD_CANCEL_DISABLE) {
        return EINVAL;
    }

    intSave = OsIntLock();
    tskCb = RUNNING_TASK;
    OsSpinLockTaskRq(tskCb);
    *oldstate = tskCb->cancelState;
    tskCb->cancelState = (unsigned char)state;
    OsSpinUnlockTaskRq(tskCb);
    OsIntRestore(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_setcanceltype(int type, int *oldtype)
{
    uintptr_t intSave;
    struct TagTskCB *tskCb;

    if (oldtype == NULL) {
        return EINVAL;
    }

    if (type != PTHREAD_CANCEL_DEFERRED && type != PTHREAD_CANCEL_ASYNCHRONOUS) {
        return EINVAL;
    }

    intSave = OsIntLock();
    tskCb = RUNNING_TASK;
    OsSpinLockTaskRq(tskCb);
    *oldtype = tskCb->cancelType;
    tskCb->cancelType = (unsigned char)type;
    OsSpinUnlockTaskRq(tskCb);
    OsIntRestore(intSave);

    return ENOERR;
}