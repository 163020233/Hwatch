/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pthread rwlock implementation
 * Author: LibCK team
 * Create: 2025-01-13
 */

#include "errno.h"
#include "pthread.h"
#include "time.h"
#include "../../include/posix_adapt.h"

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_init(pthread_rwlock_t *restrict rwl,
    const pthread_rwlockattr_t *restrict attr)
{
    (void)attr;
    int ret;

    if (rwl == NULL) {
        return EINVAL;
    }

    if (OsMutexIsInit(&rwl->mutex)) { // 不允许重复初始化
        return EBUSY;
    }

    ret = pthread_mutex_init(&rwl->mutex, NULL);
    if (ret != ENOERR) {
        return ret;
    }

    ret = pthread_cond_init(&rwl->read_cond, NULL);
    if (ret != ENOERR) {
        pthread_mutex_destroy(&rwl->mutex);
        return ret;
    }

    ret = pthread_cond_init(&rwl->write_cond, NULL);
    if (ret != ENOERR) {
        pthread_mutex_destroy(&rwl->mutex);
        pthread_cond_destroy(&rwl->read_cond);
        return ret;
    }

    rwl->pending_writers = 0;
    rwl->readers = 0;
    rwl->writer = 0;
    rwl->owner.ptr = (void *)rwl;
    rwl->owner.next = (rwlock_node_t *)&rwl->owner;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_destroy(pthread_rwlock_t *rwl)
{
    int ret;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    if (rwl->readers != 0 || rwl->writer != 0 || rwl->pending_writers != 0 ||
        !OS_RWLOCK_IS_LIST_EMPTY(&rwl->owner)) { // 正在使用不允许销毁
        return EBUSY;
    }

    ret = pthread_mutex_destroy(&rwl->mutex);
    if (ret != ENOERR) {
        return ret;
    }

    ret = pthread_cond_destroy(&rwl->read_cond);
    if (ret != ENOERR) {
        return ret;
    }

    return pthread_cond_destroy(&rwl->write_cond);
}

INLINE int OsRwlockMutexLock(pthread_mutex_t *mutex, const struct timespec *tp)
{
    if (tp == NULL) {
        return pthread_mutex_lock(mutex);
    }

    return pthread_mutex_timedlock(mutex, tp);
}

INLINE bool OsRwlockIsOwner(pthread_rwlock_t *rwl, struct TagTskCB *runTsk)
{
    rwlock_node_t *head = &rwl->owner;
    rwlock_node_t *node;

    for (node = head->next; node != head; node = node->next) {
        if (node->ptr == (void *)runTsk) {
            return TRUE;
        }
    }

    return FALSE;
}

/*
 * 描述: 调用者保证关中断
 */
static OS_SEC_L4_TEXT int OsRwlockAddNode(pthread_rwlock_t *rwl, struct TagTskCB *runTsk)
{
    struct RwList *rwListPtr;
    rwlock_node_t *ownerPtr;

    rwListPtr = (struct RwList *)malloc(sizeof(struct RwList) + sizeof(rwlock_node_t));
    if (rwListPtr == NULL) {
        return ENOMEM;
    }
    OsSpinLockTaskRq(runTsk);
    rwListPtr->next = runTsk->rwList.next;
    runTsk->rwList.next = (void *)rwListPtr;
    OsSpinUnlockTaskRq(runTsk);
    rwListPtr->ptr = (void *)rwl;

    ownerPtr = (rwlock_node_t *)((uintptr_t)rwListPtr + sizeof(struct RwList));
    ownerPtr->next = rwl->owner.next;
    rwl->owner.next = (void *)ownerPtr;
    ownerPtr->ptr = (void *)runTsk;
    return ENOERR;
}

static OS_SEC_L4_TEXT void OsRwlockRmvNode(pthread_rwlock_t *rwl, struct TagTskCB *runTsk, struct RwList *rwList)
{
    struct RwList *prev = rwList;
    struct RwList *node = rwList->next;
    rwlock_node_t *ownerHead = &rwl->owner;
    rwlock_node_t *ownerPrev = ownerHead;
    rwlock_node_t *ownerNode = ownerHead->next;

    for (; node != rwList; prev = node, node = prev->next) {
        if (node->ptr == (void *)rwl) {
            break;
        }
    }
    prev->next = node->next;

    for (; ownerNode != ownerHead; ownerPrev = ownerNode, ownerNode = ownerPrev->next) {
        if (ownerNode->ptr == (void *)runTsk) {
            break;
        }
    }
    ownerPrev->next = ownerNode->next;

    free(node);
}

/*
 * 带超时获取写锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwl,
    const struct timespec *restrict time)
{
    int ret;
    struct TagTskCB *runTsk;
    uintptr_t intSave;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    runTsk = RUNNING_TASK; // 关中断后，任务不会迁核
    if (OsRwlockIsOwner(rwl, runTsk)) { // 不能同时拿一把读写锁的读锁和写锁，或者拿两次写锁
        OsIntRestore(intSave);
        return EDEADLK;
    }

    ret = OsRwlockMutexLock(&rwl->mutex, time);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    rwl->pending_writers++;
    // 等待条件：无活跃读者且无活跃写者
    while (rwl->readers > 0 || rwl->writer != 0) {
        ret = pthread_cond_timedwait(&rwl->write_cond, &rwl->mutex, time);
        if (ret != ENOERR) {
            rwl->pending_writers--;
            pthread_mutex_unlock(&rwl->mutex);
            OsIntRestore(intSave);
            return ret;
        }
    }

    rwl->pending_writers--;

    // 标记写者已获得写锁
    ret = OsRwlockAddNode(rwl, runTsk);
    if (ret != ENOERR) {
        pthread_mutex_unlock(&rwl->mutex);
        OsIntRestore(intSave);
        return ret;
    }
    rwl->writer = 1;

    pthread_mutex_unlock(&rwl->mutex);
    OsIntRestore(intSave);

    return ENOERR;
}

/*
 * 阻塞获取写锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_wrlock(pthread_rwlock_t *rwl)
{
    return pthread_rwlock_timedwrlock(rwl, NULL);
}

/*
 * 非阻塞获取写锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_trywrlock(pthread_rwlock_t *rwl)
{
    int ret;
    uintptr_t intSave;
    struct TagTskCB *runTsk;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    runTsk = RUNNING_TASK;
    if (OsRwlockIsOwner(rwl, runTsk)) { // 不能同时拿一把读写锁的读锁和写锁，或者拿两次写锁
        OsIntRestore(intSave);
        return EDEADLK;
    }

    ret = pthread_mutex_trylock(&rwl->mutex);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    // 检查是否可以立即获得写锁
    if (rwl->readers == 0 && rwl->writer == 0) {
        ret = OsRwlockAddNode(rwl, runTsk);
        if (ret != ENOERR) {
            pthread_mutex_unlock(&rwl->mutex);
            OsIntRestore(intSave);
            return ret;
        }
        // 无活跃读者或写者，立即获取写锁
        rwl->writer = 1;
        ret = ENOERR;
    } else {
        // 无法立即获取锁，返回 EBUSY
        ret = EBUSY;
    }

    pthread_mutex_unlock(&rwl->mutex);
    OsIntRestore(intSave);

    return ret;
}

/*
 * 带超时获取读锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwl,
    const struct timespec *restrict time)
{
    int ret;
    uintptr_t intSave;
    struct TagTskCB *runTsk;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    runTsk = RUNNING_TASK;
    if (OsRwlockIsOwner(rwl, runTsk)) { // 拿读锁前，任务不该同时有该读写锁的写锁
        OsIntRestore(intSave);
        return EDEADLK;
    }

    // 加互斥锁，进入临界区
    ret = OsRwlockMutexLock(&rwl->mutex, time);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    // 等待条件：无活跃写者且无等待的写者（避免写者饥饿）
    while (rwl->writer != 0 || rwl->pending_writers > 0) {
        ret = pthread_cond_timedwait(&rwl->read_cond, &rwl->mutex, time);
        if (ret != ENOERR) {
            pthread_mutex_unlock(&rwl->mutex);
            OsIntRestore(intSave);
            return ret;
        }
    }

    // 增加读者计数
    ret = OsRwlockAddNode(rwl, runTsk);
    if (ret != ENOERR) {
        pthread_mutex_unlock(&rwl->mutex);
        OsIntRestore(intSave);
        return ret;
    }
    rwl->readers++;

    pthread_mutex_unlock(&rwl->mutex);
    OsIntRestore(intSave);

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_rdlock(pthread_rwlock_t *rwl)
{
    return pthread_rwlock_timedrdlock(rwl, NULL);
}

/*
 * 非阻塞获取读锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwl)
{
    int ret;
    uintptr_t intSave;
    struct TagTskCB *runTsk;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    runTsk = RUNNING_TASK;
    if (OsRwlockIsOwner(rwl, runTsk)) { // 拿读锁前，任务不该同时有该读写锁的写锁
        OsIntRestore(intSave);
        return EDEADLK;
    }

    ret = pthread_mutex_trylock(&rwl->mutex);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    // 检查是否可以立即获得读锁（遵循写者优先策略）
    if (rwl->writer == 0 && rwl->pending_writers == 0) {
        ret = OsRwlockAddNode(rwl, runTsk);
        if (ret != ENOERR) {
            pthread_mutex_unlock(&rwl->mutex);
            OsIntRestore(intSave);
            return ret;
        }
        // 无活跃写者且无等待的写者，立即增加读者计数
        rwl->readers++;
        ret = ENOERR;
    } else {
        // 无法立即获取读锁，返回 EBUSY
        ret = EBUSY;
    }

    pthread_mutex_unlock(&rwl->mutex);
    OsIntRestore(intSave);

    return ret;
}

/*
 * 读写锁解锁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlock_unlock(pthread_rwlock_t *rwl)
{
    int ret;
    uintptr_t intSave;
    struct TagTskCB *runTsk;

    if (rwl == NULL || !OsMutexIsInit(&rwl->mutex)) {
        return EINVAL;
    }

    intSave = OsIntLock();
    runTsk = RUNNING_TASK;
    if (!OsRwlockIsOwner(rwl, runTsk)) {
        OsIntRestore(intSave);
        return EPERM;
    }

    ret = pthread_mutex_lock(&rwl->mutex);
    if (ret != ENOERR) {
        OsIntRestore(intSave);
        return ret;
    }

    // 判断当前是释放写锁还是读锁
    if (rwl->writer != 0) {
        // 释放写锁：标记无活跃写者
        rwl->writer = 0;
        OsSpinLockTaskRq(runTsk);
        OsRwlockRmvNode(rwl, runTsk, &runTsk->rwList);
        OsSpinUnlockTaskRq(runTsk);
    } else {
        // 释放读锁：减少读者计数
        rwl->readers--;
        OsSpinLockTaskRq(runTsk);
        OsRwlockRmvNode(rwl, runTsk, &runTsk->rwList);
        OsSpinUnlockTaskRq(runTsk);

        // 如果仍有其他读者持有锁，无需唤醒等待线程
        if (rwl->readers > 0) {
            pthread_mutex_unlock(&rwl->mutex);
            OsIntRestore(intSave);
            return ENOERR;
        }
    }

    // 唤醒策略：优先唤醒等待的写者，否则唤醒所有读者
    if (rwl->pending_writers > 0) {
        ret = pthread_cond_signal(&rwl->write_cond);
    } else {
        ret = pthread_cond_broadcast(&rwl->read_cond);
    }

    pthread_mutex_unlock(&rwl->mutex);
    OsIntRestore(intSave);

    return ret;
}

/*
 * 读写锁属性初始化
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlockattr_init(pthread_rwlockattr_t *attr)
{
    (void)attr;

    return ENOERR;
}

/*
 * 读写锁属性销毁
 */
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr)
{
    (void)attr;

    return ENOERR;
}