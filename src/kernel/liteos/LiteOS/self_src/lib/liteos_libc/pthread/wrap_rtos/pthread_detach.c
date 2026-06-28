/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread detach implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "pthread.h"
#include "../../include/posix_adapt.h"

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_detach(pthread_t thread)
{
    struct TagTskCB *tskCb;
    uintptr_t intSave;
    int ret = ENOERR;

    if (CHECK_TSK_PID((TskHandle)thread)) { // idle任务不能被detach
        return ESRCH;
    }

    intSave = OsIntLock();
    tskCb = GET_TCB_HANDLE((TskHandle)thread);
    OsSpinLockTaskRq(tskCb);
    if (!OsPthreadIsPosix(tskCb)) { // 不允许操作非posix创建的任务
        OsSpinUnlockTaskRq(tskCb);
        OsIntRestore(intSave);
        return EINVAL;
    }

    if (TSK_IS_UNUSED(tskCb)) {
        OsSpinUnlockTaskRq(tskCb);
        OsIntRestore(intSave);
        return ESRCH;
    }

    if (tskCb->joiner != NULL) { // 已被其他任务join，不允许分离
        OsSpinUnlockTaskRq(tskCb);
        OsIntRestore(intSave);
        return EINVAL;
    }

    switch (tskCb->detachState) {
        case PTHREAD_CREATE_JOINABLE:
            tskCb->detachState = PTHREAD_CREATE_DETACHED;
            break;
        case PTHREAD_CREATE_DETACHED:
            break;
        default:
            ret = EINVAL;
            break;
    }
    OsSpinUnlockTaskRq(tskCb);
    OsIntRestore(intSave);

    return ret;
}