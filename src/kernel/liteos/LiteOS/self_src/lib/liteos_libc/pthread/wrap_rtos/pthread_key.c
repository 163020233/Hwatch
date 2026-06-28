/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread key implementation
 * Author: RTOS_Lite项目组
 * Create: 2025-01-13
 */
#include "pthread.h"
#include "../../include/posix_adapt.h"
#include "drb_atomic.h"

OS_SEC_L4_DATA void (*g_pthread_keys_destor[OS_TASK_KEYS_MAX])(void *) = {0};
OS_SEC_L4_DATA struct SreSpinLock g_pthKeyLock = {0};
#define PTHREAD_KEY_LOCK(state)          (state) = DRB_SplIrqLock(&g_pthKeyLock)
#define PTHREAD_KEY_UNLOCK(state)        DRB_SplIrqUnlock(&g_pthKeyLock, (state))

OS_SEC_L4_TEXT void OsPthreadRunDestructor(struct TagTskCB *taskCB)
{
    int i;
    void *val;
    uintptr_t intSave;
    void (*destructor)(void *);
    unsigned int tsdMask;

    OsSpinLockTaskRq(taskCB);
    tsdMask = taskCB->tsdUsed;
    taskCB->tsdUsed = 0;
    OsSpinUnlockTaskRq(taskCB);

    PTHREAD_KEY_LOCK(intSave);
    for (i = 0; i < OS_TASK_KEYS_MAX; i++) {
        if ((tsdMask & (1U << (U32)i)) != 0) {
            OsSpinLockTaskRq(taskCB);
            val = taskCB->tsd[i];
            OsSpinUnlockTaskRq(taskCB);
            destructor = g_pthread_keys_destor[i];
            if (destructor != NULL) { // 若为空，说明在pthread_key_delete已释放该资源
                destructor(val);
            }
        }
    }
    PTHREAD_KEY_UNLOCK(intSave);
}

OS_SEC_L4_TEXT U32 OsPosixPthreadKeyInit(void)
{
    return DRB_SplLockInit(&g_pthKeyLock);
}

OS_SEC_L4_TEXT void OsPthreadDestructor(void *arg)
{
    (void)arg;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_key_create(pthread_key_t *key, void (*destructor)(void *))
{
    int i;
    uintptr_t intSave;

    if (key == NULL) {
        return EINVAL;
    }

    PTHREAD_KEY_LOCK(intSave);
    for (i = 0; i < OS_TASK_KEYS_MAX; i++) {
        if (g_pthread_keys_destor[i] == NULL) {
            *key = (pthread_key_t)i;
            break;
        }
    }

    if (i == OS_TASK_KEYS_MAX) {
        PTHREAD_KEY_UNLOCK(intSave);
        return EAGAIN;
    }

    if (destructor != NULL) {
        g_pthread_keys_destor[i] = destructor;
    } else {
        g_pthread_keys_destor[i] = OsPthreadDestructor;
    }

    PTHREAD_KEY_UNLOCK(intSave);
    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_key_delete(pthread_key_t key)
{
    unsigned int i;
    uintptr_t intSave;
    struct TagTskCB *tcb;
    void *val;
    void (*destructor)(void *);

    PTHREAD_KEY_LOCK(intSave);
    if ((unsigned int)key >= OS_TASK_KEYS_MAX || g_pthread_keys_destor[key] == NULL) {
        PTHREAD_KEY_UNLOCK(intSave);
        return EINVAL;
    }

    destructor = g_pthread_keys_destor[key];
    for (i = 0; i < g_tskMaxNum; i++) {
        tcb = &g_tskCBArray[i];
        OsSpinLockTaskRq(tcb);
        if (!OsPthreadIsPosix(tcb) || (tcb->tsdUsed & (1U << (unsigned int)key)) == 0) {
            OsSpinUnlockTaskRq(tcb);
            continue;
        }
        tcb->tsdUsed = tcb->tsdUsed & ~(1U << (unsigned int)key);
        val = tcb->tsd[key];
        tcb->tsd[key] = NULL;
        OsSpinUnlockTaskRq(tcb);
        destructor(val);
    }
    g_pthread_keys_destor[key] = NULL;
    PTHREAD_KEY_UNLOCK(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_setspecific(pthread_key_t key, const void *value)
{
    uintptr_t intSave;
    struct TagOsRunQue *thisQue;
    struct TagTskCB *tcb;

    PTHREAD_KEY_LOCK(intSave);
    if ((unsigned int)key >= OS_TASK_KEYS_MAX || g_pthread_keys_destor[key] == NULL) {
        PTHREAD_KEY_UNLOCK(intSave);
        return EINVAL;
    }

    thisQue = OsSpinLockRunTaskRq();
    tcb = RUNNING_TASK;
    if (!OsPthreadIsPosix(tcb)) {
        OsSpinUnLockRunTaskRq(thisQue);
        PTHREAD_KEY_UNLOCK(intSave);
        return EINVAL;
    }
    tcb->tsd[key] = (void *)(uintptr_t)value;
    tcb->tsdUsed |= (1U << (U32)key);
    OsSpinUnLockRunTaskRq(thisQue);
    PTHREAD_KEY_UNLOCK(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT void *pthread_getspecific(pthread_key_t key)
{
    uintptr_t intSave;
    struct TagOsRunQue *thisQue;
    struct TagTskCB *tcb;
    void *res;

    if ((unsigned int)key >= OS_TASK_KEYS_MAX) {
        return NULL;
    }

    PTHREAD_KEY_LOCK(intSave);
    thisQue = OsSpinLockRunTaskRq();
    tcb = RUNNING_TASK;
    if (!OsPthreadIsPosix(tcb)) {
        OsSpinUnLockRunTaskRq(thisQue);
        PTHREAD_KEY_UNLOCK(intSave);
        return NULL;
    }

    if ((tcb->tsdUsed & (1U << (unsigned int)key)) == 0) {
        OsSpinUnLockRunTaskRq(thisQue);
        PTHREAD_KEY_UNLOCK(intSave);
        return NULL;
    }

    res = tcb->tsd[key];
    OsSpinUnLockRunTaskRq(thisQue);
    PTHREAD_KEY_UNLOCK(intSave);
    return res;
}