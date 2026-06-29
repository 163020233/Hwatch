/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread join implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "time.h"
#include "pthread.h"
#include "../../include/posix_adapt.h"

/*
 * 描述: 检查入参
 */
static OS_SEC_L4_TEXT int OsPthreadJoinCheckPara(pthread_t thread)
{
    if (CHECK_TSK_PID(thread)) { // idle任务不能被join
        return EINVAL;
    }

    return ENOERR;
}

/*
 * 描述: 调用者保证关中断，检查背景环境
 */
static OS_SEC_L4_TEXT int OsPthreadJoinCheckBG(void)
{
    if (OS_INT_ACTIVE) {
        return EINVAL;
    }

    if (OS_TASK_LOCK_DATA != 0) {
        return EINVAL;
    }

    return ENOERR;
}

/*
 * 描述: 调用者保证关中断并将runQ上锁
 */
static OS_SEC_L4_TEXT int OsPthreadJoinCheckTarget(struct TagTskCB *tskCb)
{
    /* 不允许操作非posix创建的任务 */
    if (!OsPthreadIsPosix(tskCb)) {
        return EINVAL;
    }

    /* 关中断时，running task不会迁核 */
    if (tskCb == RUNNING_TASK) {
        return EDEADLK;
    }

    /* 目标任务已经删除 */
    if (tskCb->taskStatus == OS_TSK_UNUSED) {
        return ESRCH;
    }

    if (tskCb->detachState != PTHREAD_CREATE_JOINABLE) {
        return EINVAL;
    }

    /* 目标任务已被其他任务连接 */
    if (tskCb->joiner != NULL) {
        return EINVAL;
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_join(pthread_t thread, void **status)
{
    int ret;
    struct TagTskCB *tskCb;
    struct TagTskCB *runTsk;
    uintptr_t intSave;
    bool isDiffCore;

    ret = OsPthreadJoinCheckPara(thread);
    if (ret != ENOERR) {
        return ret;
    }

    intSave = OsIntLock();
    ret = OsPthreadJoinCheckBG();
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    tskCb = GET_TCB_HANDLE(thread);
    OsSpinLockTaskRq(tskCb);
    ret = OsPthreadJoinCheckTarget(tskCb);
    if (ret != ENOERR) {
        OsSpinUnlockTaskRq(tskCb);
        OsIntRestore(intSave);
        return ret;
    }

    TSK_CREATE_DEL_LOCK();
    runTsk = RUNNING_TASK;
    /* 等待目标任务结束 */
    tskCb->joiner = runTsk;
    isDiffCore = (tskCb->coreId != runTsk->coreId);
    if (isDiffCore) {
        OsSpinLockTaskRq(runTsk);
    }

    OsTskReadyDel(runTsk);
    runTsk->joined = tskCb;
    TSK_STATUS_SET(runTsk, OS_TSK_JOIN_WAIT);

    if (isDiffCore) {
        OsSpinUnlockTaskRq(runTsk);
    }
    OsSpinUnlockTaskRq(tskCb);
    TSK_CREATE_DEL_UNLOCK();

    /* 触发任务调度 */
    OsTskSchedule();

    /* 让出期间可能会切核，重新获取；回来时是关中断的 */
    runTsk = RUNNING_TASK;
    OsSpinLockTaskRq(runTsk);
    /* 被join的任务将自己的返回值通过该任务的retval传递 */
    if (status != NULL) {
        *status = runTsk->retval;
    }

    runTsk->retval = NULL;
    OsSpinUnlockTaskRq(runTsk);

    OsIntRestore(intSave);
    return ret;
}
