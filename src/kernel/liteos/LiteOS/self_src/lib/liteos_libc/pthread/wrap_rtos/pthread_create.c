/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread create implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "pthread.h"
#include "../../include/posix_adapt.h"
#include "drb_sem.h"

static OS_SEC_L4_TEXT void OsPthreadWrapper(uintptr_t param1, uintptr_t param2, uintptr_t param3, uintptr_t param4)
{
    void *ret;
    void *(*threadroutine)(void *) = (void *)param1;
    (void)param3;
    (void)param4;

    ret = threadroutine((void *)param2);
    OsPthreadExit(ret);
}

static OS_SEC_L4_TEXT void OsPthreadCreateParaInit(const pthread_attr_t *attr, struct TskInitParam *initParam,
    const pthread_startroutine routine, const void *arg)
{
    initParam->taskEntry = OsPthreadWrapper;
    initParam->taskPrio = attr->schedparam.sched_priority;
    initParam->queNum = 0;
    initParam->args[OS_TSK_PARA_0] = (uintptr_t)routine;
    initParam->args[OS_TSK_PARA_1] = (uintptr_t)arg;
    initParam->args[OS_TSK_PARA_2] = 0;
    initParam->args[OS_TSK_PARA_3] = 0;
    initParam->stackSize = attr->stacksize_set ? attr->stacksize : 0;
    initParam->stackAddr = attr->stackaddr_set ? (uintptr_t)(attr->stackaddr) : 0;
    initParam->name = "pthread";
    initParam->privateData = 0;
    initParam->detachState = (unsigned int)attr->detachstate;
}

static OS_SEC_L4_TEXT int OsPthreadCreateParaCheckAndInit(TskHandle *newthread, const pthread_attr_t *attrp,
    pthread_startroutine routine, void *arg, struct TskInitParam *initParam)
{
    pthread_attr_t attr = {0};

    if (newthread == NULL || routine == NULL) {
        return EINVAL;
    }

    if (attrp != NULL) {
        if ((attrp->stacksize < (int)OS_TSK_MIN_STACK_SIZE) ||
            (((OS_TSK_STACK_SIZE_ALIGN - 1) & (uintptr_t)attrp->stackaddr) != 0) ||
            (((OS_TSK_STACK_SIZE_ALIGN - 1) & attrp->stacksize) != 0) ||
            (((uintptr_t)attrp->stackaddr + attrp->stacksize) < (uintptr_t)attrp->stackaddr)) {
                return EINVAL;
        }

        if ((attrp->detachstate != PTHREAD_CREATE_JOINABLE && attrp->detachstate != PTHREAD_CREATE_DETACHED) ||
            attrp->scope != PTHREAD_SCOPE_SYSTEM ||
            (attrp->inheritsched != PTHREAD_INHERIT_SCHED && attrp->inheritsched != PTHREAD_EXPLICIT_SCHED) ||
            attrp->schedpolicy < 0 || attrp->schedpolicy > PTHREAD_DEFAULT_POLICY ||
            attrp->schedparam.sched_priority < USER_THREAD_PRIORITY_HIGHEST ||
            attrp->schedparam.sched_priority > USER_THREAD_PRIORITY_LOWEST) {
            return EINVAL;
        }
        OsPthreadCreateParaInit(attrp, initParam, routine, arg);
    } else {
        OsErrRecord((unsigned int)pthread_getattr_default_np(&attr));
        OsPthreadCreateParaInit(&attr, initParam, routine, arg);
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_create(pthread_t *restrict newthread,
    const pthread_attr_t *restrict attr, void *(*threadroutine)(void *), void *restrict arg)
{
    int ret;
    TskHandle taskId;
    struct TskInitParam initParam = {0};
    struct OsMemCbInfo cbInitParam = {g_posixMemPtNo, (unsigned int)(uintptr_t)(void *)pthread_create};

    ret = OsPthreadCreateParaCheckAndInit(newthread, attr, threadroutine, arg, &initParam);
    if (ret != ENOERR) {
        return ret;
    }

    ret = (int)OsTaskCreatePthreadOnly(&taskId, &initParam, FALSE, &cbInitParam);
    if (ret != ENOERR) {
        return EINVAL;
    }

    *newthread = (pthread_t)taskId;
    ret = (int)DRB_TaskResume(taskId);
    if (ret != ENOERR) {
        return EPERM;
    }

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT pthread_t pthread_self(void)
{
    unsigned int pid;
    OsErrRecord(DRB_TaskSelf(&pid));

    return (pthread_t)pid;
}
