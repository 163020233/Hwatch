/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: posix头文件
 * Author: RTOS_Lite项目组
 * Create: 2025-3-4
 * History: 创建文件
 */
#ifndef POSIX_ADAPT_H
#define POSIX_ADAPT_H

#include "sys/_pthreadtypes.h"
#include "time.h"
#include "pthread.h"
#include "drb_task.h"
#include "sre_sys_external.h"
#include "sre_list_external.h"
#include "sre_task_external.h"
#include "sre_sem_external.h"
#include "sre_cpu_external.h"
#include "sre_fscmem_external.h"
#include "sre_task_sched_external.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef VOID
#define VOID void
#endif

#ifndef STATIC
#define STATIC static
#endif

typedef signed int            INT32;
typedef signed long long      INT64;
typedef unsigned int          UINT32;
typedef unsigned long long    UINT64;

#define ENOERR                0
#define PTHREAD_OP_FAIL       (-1)

#define CLOCK_ID_BUTT           12 // time

#define MAX_CLOCKS              CLOCK_ID_BUTT // time

#define OS_SYS_NS_PER_US     1000000
#define OS_SYS_NS_PER_SECOND 1000000000

#define DEFAULT_VISIBILITY __attribute__((visibility("default")))

#define weak_alias(name, aliasname) _weak_alias (name, aliasname)
#define _weak_alias(name, aliasname) \
    extern __typeof (name) (aliasname) __attribute__ ((weak, alias (#name)))

struct timeval64 {
    S64 tv_sec;
    S64 tv_usec;
};

struct timespec64 {
    S64 tv_sec;
    S64 tv_nsec;
};

/*
 * 描述: 外部保证关中断、上锁
 */
INLINE bool OsPthreadIsPosix(struct TagTskCB *tskCb)
{
    return tskCb->isPosix;
}

INLINE bool OsMutexIsInit(pthread_mutex_t *mutex)
{
    return (mutex->magic == PTHREAD_MUTEX_INIT_MAGIC);
}

INLINE bool OsCondIsInit(pthread_cond_t *cond)
{
    return (cond->init == PTHREAD_COND_INIT_MAGIC);
}

extern U8 g_posixMemPtNo;
extern void(*g_pthread_keys_destor[OS_TASK_KEYS_MAX])(void *);

extern int OsTimeOut2Ticks(const struct timespec *time, U32 *ticks);
extern void OsTimeGetHwTime(struct timespec *hwTime);
extern void OsPthreadExit(void *retval);
extern void OsPthreadNotifyParents(unsigned short semId, unsigned short semCount);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* POSIX_ADAPT_H */