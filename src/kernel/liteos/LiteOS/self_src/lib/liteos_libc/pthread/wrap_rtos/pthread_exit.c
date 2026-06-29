/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread exit implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "pthread.h"
#include "../../include/posix_adapt.h"
#include "drb_sem.h"

OS_SEC_L4_TEXT void OsPthreadExit(void *retval)
{
    uintptr_t intSave;
    pthread_t thread;
    struct TagTskCB *tskCb;

    intSave = OsIntLock();
    tskCb = RUNNING_TASK;

    OsSpinLockTaskRq(tskCb);
    tskCb->retval = retval;
    thread = tskCb->taskPID;
    OsSpinUnlockTaskRq(tskCb);
    OsIntRestore(intSave);

    OsErrRecord(DRB_TaskDelete(thread));
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT _Noreturn void pthread_exit(void *retval)
{
    OsPthreadExit(retval);
    /* if PRT_PthreadExit is ok, never comes here */
    while (1) {
    }
}