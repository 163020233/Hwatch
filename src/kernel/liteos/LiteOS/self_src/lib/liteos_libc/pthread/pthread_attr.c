/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Functions about pthread_attr_t
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
#include "pprivate.h"
#include "errno.h"
#include "libc.h"
#else
#include "errno.h"
#include "pthread.h"
#include "../include/posix_adapt.h"
#endif

#if defined(__LITEOS__)
#define OS_SEC_L4_TEXT
#define DEFAULT_VISIBILITY
STATIC INT32 CheckSchedPolicy(INT32 policy)
{
    if ((policy == SCHED_OTHER) || (policy == SCHED_FIFO) || (policy == SCHED_RR)) {
        return 0;
    }
    return EINVAL;
}

STATIC BOOL IsSchedPolicyInvalid(INT32 policy)
{
    if (policy == SCHED_OTHER) {
        return FALSE;
    }
    if (policy == SCHED_RR || policy == SCHED_FIFO) {
        return FALSE;
    }
    return TRUE;
}

int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->detachstate                 = PTHREAD_CREATE_JOINABLE;
    attr->schedpolicy                 = 0xFFFFFFFF;
    attr->schedparam.sched_priority   = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    attr->inheritsched                = PTHREAD_INHERIT_SCHED;
    attr->scope                       = PTHREAD_SCOPE_SYSTEM;
    attr->stackaddr_set               = 0;
    attr->stackaddr                   = NULL;
    attr->stacksize_set               = 1;
    attr->stacksize                   = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;

#ifdef LOSCFG_KERNEL_SMP
    (void)memset(attr->cpuset.__bits, 0, sizeof(attr->cpuset.__bits));
#endif

    return ENOERR;
}
#else
static OS_SEC_L4_DATA pthread_attr_t g_pthreadattr = {
    .stackaddr = NULL,
    .stacksize = 0,
    .scope = PTHREAD_SCOPE_SYSTEM,
    .inheritsched = PTHREAD_EXPLICIT_SCHED,
    .schedpolicy = PTHREAD_DEFAULT_POLICY,
    .detachstate = PTHREAD_CREATE_JOINABLE,
    .schedparam = {
        .sched_priority = PTHREAD_DEFAULT_PRIORITY,
    },
    .stackaddr_set = FALSE,
    .stacksize_set = FALSE,
};

STATIC OS_SEC_L4_TEXT int CheckSchedPolicy(int policy)
{
    if (policy <= PTHREAD_DEFAULT_POLICY) {
        return 0;
    }
    return EINVAL;
}

STATIC OS_SEC_L4_TEXT bool IsSchedPolicyInvalid(int policy)
{
    return policy != OS_SCHED_FIFO;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_init(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return ENOMEM;
    }
    attr->schedpolicy               = PTHREAD_DEFAULT_POLICY;
    attr->stackaddr                 = NULL;
    attr->stackaddr_set             = FALSE;
    attr->stacksize                 = (size_t)g_tskModInfo.defaultSize;
    attr->stacksize_set             = TRUE;
    attr->schedparam.sched_priority = PTHREAD_DEFAULT_PRIORITY;
    attr->detachstate               = PTHREAD_CREATE_JOINABLE;
    attr->inheritsched              = PTHREAD_EXPLICIT_SCHED;
    attr->scope                     = PTHREAD_SCOPE_SYSTEM;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_getattr_default_np(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->schedpolicy = g_pthreadattr.schedpolicy;
    attr->stackaddr = g_pthreadattr.stackaddr;
    if (g_pthreadattr.stacksize == 0) {
        g_pthreadattr.stacksize = (size_t)g_tskModInfo.defaultSize;
    }
    attr->stacksize = g_pthreadattr.stacksize;
    attr->schedparam.sched_priority = g_pthreadattr.schedparam.sched_priority;
    attr->detachstate = g_pthreadattr.detachstate;
    attr->inheritsched = g_pthreadattr.inheritsched;
    attr->scope = g_pthreadattr.scope;
    attr->stackaddr_set = FALSE;
    attr->stacksize_set = TRUE;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_getattr_np(pthread_t thread, pthread_attr_t *attr)
{
    int ret;
    uintptr_t intSave;
    struct TagTskCB *taskCb = NULL;

    if ((attr == NULL) || CHECK_TSK_PID((TskHandle)thread)) {
        return EINVAL;
    }

    ret = pthread_getattr_default_np(attr);
    if (ret != ENOERR) {
        return ret;
    }

    taskCb = GET_TCB_HANDLE((TskHandle)thread);
    intSave = OsIntLock();
    OsSpinLockTaskRq(taskCb);
    if (!OsPthreadIsPosix(taskCb) || TSK_IS_UNUSED(taskCb)) {
        OsSpinUnlockTaskRq(taskCb);
        OsIntRestore(intSave);
        return EINVAL;
    }

    attr->schedparam.sched_priority = (int)taskCb->priority;
    attr->detachstate = (int)taskCb->detachState;
    attr->stackaddr = (void *)taskCb->topOfStack;
    attr->stacksize = (size_t)taskCb->stackSize;
    attr->stackaddr_set = TRUE;
    attr->stacksize_set = TRUE;

    OsSpinUnlockTaskRq(taskCb);
    OsIntRestore(intSave);

    return ENOERR;
}
#endif

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_destroy(pthread_attr_t *attr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    (void)memset(attr, 0, sizeof(pthread_attr_t));
    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachState)
{
    if ((attr != NULL) && ((detachState == PTHREAD_CREATE_JOINABLE) || (detachState == PTHREAD_CREATE_DETACHED))) {
#if defined(__LITEOS__)
        attr->detachstate = (UINT32)detachState;
#else
        attr->detachstate = detachState;
#endif
        return ENOERR;
    }

    return EINVAL;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachState)
{
    if ((attr == NULL) || (detachState == NULL)) {
        return EINVAL;
    }

    *detachState = (int)attr->detachstate;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
    if (attr == NULL) {
        return EINVAL;
    }

    if (scope == PTHREAD_SCOPE_SYSTEM) {
#if defined(__LITEOS__)
        attr->scope = (unsigned int)scope;
#else
        attr->scope = scope;
#endif
        return ENOERR;
    }

    if (scope == PTHREAD_SCOPE_PROCESS) {
        return ENOTSUP;
    }

    return EINVAL;
}

#if defined(__LITEOS__)
int pthread_attr_getscope(const pthread_attr_t *attr, int *scope)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getscope(const pthread_attr_t *restrict attr, int *restrict scope)
#endif
{
    if ((attr == NULL) || (scope == NULL)) {
        return EINVAL;
    }

    *scope = (int)attr->scope;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setinheritsched(pthread_attr_t *attr, int inherit)
{
    if ((attr == NULL) || ((inherit != PTHREAD_INHERIT_SCHED) && (inherit != PTHREAD_EXPLICIT_SCHED))) {
        return EINVAL;
    }

#if defined(__LITEOS__)
    attr->inheritsched = (UINT32)inherit;
#else
    attr->inheritsched = inherit;
#endif

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inherit)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getinheritsched(const pthread_attr_t *restrict attr,
    int *restrict inherit)
#endif
{
    if ((attr == NULL) || (inherit == NULL)) {
        return EINVAL;
    }

    *inherit = (int)attr->inheritsched;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
    if ((attr == NULL) || CheckSchedPolicy(policy) != 0) {
        return EINVAL;
    }

    if (IsSchedPolicyInvalid(policy)) {
        return ENOTSUP;
    }

#if defined(__LITEOS__)
    attr->schedpolicy = (unsigned int)policy;
#else
    attr->schedpolicy = policy;
#endif

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getschedpolicy(const pthread_attr_t *restrict attr,
    int *restrict policy)
#endif
{
    if ((attr == NULL) || (policy == NULL)) {
        return EINVAL;
    }

    *policy = (int)attr->schedpolicy;

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setschedparam(pthread_attr_t *restrict attr,
    const struct sched_param *restrict param)
#endif
{
    if ((attr == NULL) || (param == NULL)) {
        return EINVAL;
    }
    if ((param->sched_priority < USER_THREAD_PRIORITY_HIGHEST) ||
        (param->sched_priority > USER_THREAD_PRIORITY_LOWEST)) {
        return ENOTSUP;
    }

    attr->schedparam = *param;

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getschedparam(const pthread_attr_t *restrict attr,
    struct sched_param *restrict param)
#endif
{
    if ((attr == NULL) || (param == NULL)) {
        return EINVAL;
    }

    *param = attr->schedparam;

    return ENOERR;
}

#if defined(__LITEOS__)
int pthread_attr_setstack(pthread_attr_t *attr, void *stackAddr, size_t stackSize)
{
    if ((attr == NULL) || (stackAddr == NULL) || (stackSize < PTHREAD_STACK_MIN) ||
        (((UINTPTR)stackAddr & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) > 0) ||
        (((UINTPTR)stackSize & (LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) > 0)) {
        return EINVAL;
    }

    attr->stackaddr_set = 1;
    attr->stackaddr     = stackAddr;
    attr->stacksize_set = 1;
    attr->stacksize     = stackSize;
    return ENOERR;
}

int pthread_attr_getstack(const pthread_attr_t *attr, void **stackAddr, size_t *stackSize)
{
    if ((attr == NULL) || (stackAddr == NULL) || (stackSize == NULL) ||
        (attr->stacksize_set == 0) || (attr->stackaddr_set == 0)) {
        return EINVAL;
    }
    *stackAddr = attr->stackaddr;
    *stackSize = attr->stacksize;
    return ENOERR;
}

/*
 * Set starting address of stack. Whether this is at the start or end of
 * the memory block allocated for the stack depends on whether the stack
 * grows up or down.
 */
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackAddr)
{
    if (attr == NULL) {
        return EINVAL;
    }

    attr->stackaddr_set = 1;
    attr->stackaddr     = stackAddr;

    return ENOERR;
}
#endif

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackAddr)
{
    if (((attr != NULL) && (stackAddr != NULL)) && attr->stackaddr_set) {
        *stackAddr = attr->stackaddr;
        return ENOERR;
    }

    return EINVAL; /* Stack address not set, return EINVAL. */
}

#if defined(__LITEOS__)
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stackSize)
{
    /* Reject inadequate stack sizes */
    if ((attr == NULL) || (stackSize < PTHREAD_STACK_MIN)) {
        return EINVAL;
    }

    attr->stacksize_set = 1;
    attr->stacksize     = stackSize;

    return ENOERR;
}
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setstack(pthread_attr_t *attr, void *stackAddr, size_t stackSize)
{
    unsigned long long stackAddrLen;

    if (attr == NULL) {
        return EINVAL;
    }

    if (stackSize < (size_t)OS_TSK_MIN_STACK_SIZE ||
        (((OS_TSK_STACK_SIZE_ALIGN - 1) & (uintptr_t)stackAddr) != 0) ||
        (((OS_TSK_STACK_SIZE_ALIGN - 1) & stackSize) != 0)) {
        return EINVAL;
    }

    stackAddrLen = (unsigned long long)(stackSize);
    /* posix接口不实现msgq, queNum默认为0 */
    if ((uintptr_t)stackAddr != 0U) {
        stackAddrLen += (uintptr_t)(stackAddr);
    }
    /* 保证栈空间在4G范围内不溢出 */
    if (stackAddrLen > OS_MAX_U32) {
        return EINVAL;
    }

    attr->stackaddr = stackAddr;
    attr->stacksize = stackSize;
    attr->stackaddr_set = TRUE;
    attr->stacksize_set = TRUE;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getstack(const pthread_attr_t *restrict attr,
    void **restrict stackAddr, size_t *restrict stackSize)
{
    if (attr == NULL || stackAddr == NULL || stackSize == NULL || !attr->stackaddr_set || !attr->stacksize_set) {
        return EINVAL;
    }

    *stackAddr = attr->stackaddr;
    *stackSize = attr->stacksize;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackAddr)
{
    unsigned long long stackAddrLen;

    if ((attr == NULL) || (((OS_TSK_STACK_SIZE_ALIGN - 1) & (uintptr_t)stackAddr) != 0)) {
        return EINVAL;
    }

    if (attr->stacksize_set) {
        if (((OS_TSK_STACK_SIZE_ALIGN - 1) & attr->stacksize) != 0) {
            return EINVAL;
        }
        stackAddrLen = (unsigned int)(attr->stacksize);
        /* posix接口不实现msgq, queNum默认为0 */
        if ((uintptr_t)stackAddr != 0U) {
            stackAddrLen += (uintptr_t)(stackAddr);
        }
        /* 保证栈空间在4G范围内不溢出 */
        if (stackAddrLen > OS_MAX_U32) {
            return EINVAL;
        }
    }

    attr->stackaddr = stackAddr;
    attr->stackaddr_set = TRUE;

    return ENOERR;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stackSize)
{
    unsigned long long stackAddrLen;

    if (attr == NULL) {
        return EINVAL;
    }

    if ((stackSize < OS_TSK_MIN_STACK_SIZE) ||
        (((OS_TSK_STACK_SIZE_ALIGN - 1) & stackSize) != 0)) {
        return EINVAL;
    }

    stackAddrLen = (unsigned long long)(stackSize);
    /* posix接口不实现msgq, queNum默认为0 */
    if (attr->stackaddr_set && (uintptr_t)attr->stackaddr != 0U) {
        if (((OS_TSK_STACK_SIZE_ALIGN - 1) & (uintptr_t)attr->stackaddr) != 0) {
            return EINVAL;
        }
        stackAddrLen += (uintptr_t)(attr->stackaddr);
    }
    /* 保证栈空间在4G范围内不溢出 */
    if (stackAddrLen > OS_MAX_U32) {
        return EINVAL;
    }

    attr->stacksize = stackSize;
    attr->stacksize_set = TRUE;

    return ENOERR;
}
#endif

#if defined(__LITEOS__)
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stackSize)
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int pthread_attr_getstacksize(const pthread_attr_t *restrict attr,
    size_t *restrict stackSize)
#endif
{
    /* Reject attempts to get a stack size when one has not been set. */
    if ((attr == NULL) || (stackSize == NULL) || (!attr->stacksize_set)) {
        return EINVAL;
    }

    *stackSize = attr->stacksize;

    return ENOERR;
}

#if defined(__LITEOS__)
/*
 * Set the cpu affinity mask
 */
int pthread_attr_setaffinity_np(pthread_attr_t* attr, size_t cpusetsize, const cpu_set_t* cpuset)
{
#ifdef LOSCFG_KERNEL_SMP
    if (attr == NULL) {
        return EINVAL;
    }

    if ((cpuset == NULL) || (cpusetsize == 0)) {
        attr->cpuset.__bits[0] = 0;
        return ENOERR;
    }

    if ((cpusetsize != sizeof(cpu_set_t)) || (cpuset->__bits[0] > LOSCFG_KERNEL_CPU_MASK)) {
        return EINVAL;
    }

    attr->cpuset = *cpuset;
#else
    (void)attr;
    (void)cpusetsize;
    (void)cpuset;
#endif

    return ENOERR;
}

/*
 * Get the cpu affinity mask
 */
int pthread_attr_getaffinity_np(const pthread_attr_t* attr, size_t cpusetsize, cpu_set_t* cpuset)
{
#ifdef LOSCFG_KERNEL_SMP
    if ((attr == NULL) || (cpuset == NULL) || (cpusetsize != sizeof(cpu_set_t))) {
        return EINVAL;
    }

    *cpuset = attr->cpuset;
#else
    (void)attr;
    (void)cpusetsize;
    (void)cpuset;
#endif

    return ENOERR;
}
#endif
