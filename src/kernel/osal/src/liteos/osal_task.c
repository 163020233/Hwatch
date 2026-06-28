/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: task
 * Author: FBB OS Team
 * Create: 2021-12-16
 */

#include <los_task.h>
#include <los_base.h>
#include <los_tick.h>
#include <los_sched_pri.h>
#include <los_config.h>
#include <los_memory.h>
#include "soc_osal.h"
#include "osal_inner.h"

#define MINIMAL_STACK_SIZE LOSCFG_BASE_CORE_TSK_MIN_STACK_SIZE

static osal_kernel_status g_kernelState = OSAL_KERNEL_STATUS_INACTIVE;

osal_task *osal_kthread_create_static_ext(osal_kthread_init *init_handle, void *topStack)
{
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    if (init_handle == NULL) {
        osal_log("param invalid!\n");
        return NULL;
    }

    osal_task *task = (osal_task *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(osal_task));
    if (task == NULL) {
        osal_log("MemAlloc fail!\n");
        return NULL;
    }

    TSK_INIT_PARAM_S my_task = { 0 };
    my_task.pcName       = (char *)init_handle->taskname;
    my_task.uwStackSize  = (init_handle->stacksize <= MINIMAL_STACK_SIZE) ? MINIMAL_STACK_SIZE : init_handle->stacksize;
    my_task.usTaskPrio   = init_handle->taskprio;
    my_task.uwResved     = LOS_TASK_STATUS_DETACHED;
    my_task.pfnTaskEntry = (TSK_ENTRY_FUNC)init_handle->handler;
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    LOS_TASK_PARAM_INIT_ARG(my_task, init_handle->data);
#else
    my_task.auwArgs[0] = (AARCHPTR)init_handle->data;
#endif
#ifdef LOSCFG_WCFS_SCHEDULER
    if (init_handle->policy == OSAL_THREAD_POLICY_CFS) {
        my_task.schedPolicy = TASK_POLICY_CFS;
    } else if (init_handle->policy == OSAL_THREAD_POLICY_RT) {
        my_task.schedPolicy = TASK_POLICY_RT;
    } else {
        my_task.schedPolicy = TASK_POLICY_DEFAULT;
    }
#endif
    unsigned int temp_task_id = 0;
    unsigned int ret = LOS_TaskCreateStatic(&temp_task_id, &my_task, topStack);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, (void*)task);
        osal_log("TaskCreate fail! ret=%#x.\n", ret);
        return NULL;
    }
    task->task = (void *)(UINTPTR)temp_task_id;
    return task;
#else
    osal_unused(init_handle);
    osal_unused(topStack);
    return NULL;
#endif
}

osal_task *osal_kthread_create_ext(osal_kthread_init *init_handle)
{
    if (init_handle == NULL) {
        osal_log("param invalid!\n");
        return NULL;
    }

    osal_task *task = (osal_task *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(osal_task));
    if (task == NULL) {
        osal_log("MemAlloc fail!\n");
        return NULL;
    }

    TSK_INIT_PARAM_S my_task = { 0 };
    my_task.pcName       = (char *)init_handle->taskname;
    my_task.uwStackSize  = (init_handle->stacksize <= MINIMAL_STACK_SIZE) ? MINIMAL_STACK_SIZE : init_handle->stacksize;
    my_task.usTaskPrio   = init_handle->taskprio;
    my_task.uwResved     = LOS_TASK_STATUS_DETACHED;
    my_task.pfnTaskEntry = (TSK_ENTRY_FUNC)init_handle->handler;
#ifdef HW_LITEOS_OPEN_VERSION_NUM
    LOS_TASK_PARAM_INIT_ARG(my_task, init_handle->data);
#else
    my_task.auwArgs[0] = (AARCHPTR)init_handle->data;
#endif

#ifdef LOSCFG_WCFS_SCHEDULER
    if (init_handle->policy == OSAL_THREAD_POLICY_CFS) {
        my_task.schedPolicy = TASK_POLICY_CFS;
    } else {
        my_task.schedPolicy = TASK_POLICY_RT;
    }
#endif
    unsigned int temp_task_id = 0;
    unsigned int ret = LOS_TaskCreate(&temp_task_id, &my_task);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, (void*)task);
        osal_log("TaskCreate fail! ret=%#x.\n", ret);
        return NULL;
    }
    task->task = (void *)(UINTPTR)temp_task_id;
    return task;
}

osal_task *osal_kthread_create(osal_kthread_handler handler, void *data, const char *name, unsigned int stack_size)
{
    if (handler == NULL) {
        osal_log("param invalid!\n");
        return NULL;
    }

    osal_task *task = (osal_task *)LOS_MemAlloc((void*)m_aucSysMem0, sizeof(osal_task));
    if (task == NULL) {
        osal_log("MemAlloc fail!\n");
        return NULL;
    }

    TSK_INIT_PARAM_S my_task = { 0 };
    my_task.pcName       = (char *)name;
    my_task.uwStackSize  = (stack_size <= MINIMAL_STACK_SIZE) ? MINIMAL_STACK_SIZE : stack_size;
    my_task.usTaskPrio   = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    my_task.uwResved     = LOS_TASK_STATUS_DETACHED;
    my_task.pfnTaskEntry = (TSK_ENTRY_FUNC)handler;

#ifdef HW_LITEOS_OPEN_VERSION_NUM
    LOS_TASK_PARAM_INIT_ARG(my_task, data);
#else
    my_task.auwArgs[0] = (AARCHPTR)data;
#endif

    unsigned int temp_task_id = 0;
    unsigned int ret = LOS_TaskCreate(&temp_task_id, &my_task);
    if (ret != LOS_OK) {
        LOS_MemFree((void*)m_aucSysMem0, (void*)task);
        osal_log("TaskCreate fail! ret=%#x.\n", ret);
        return NULL;
    }
    task->task = (void *)(UINTPTR)temp_task_id;
    return task;
}

int osal_kthread_set_priority(osal_task *task, unsigned int priority)
{
    if (task == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int ret = LOS_TaskPriSet((unsigned int)(UINTPTR)task->task, priority);
    if (ret != LOS_OK) {
        osal_log("TaskPriSet fail! ret=%#x.\n", ret);
    }
    return (int)ret;
}

int osal_kthread_set_policy(osal_task *task, osal_thread_policy policy)
{
#ifdef LOSCFG_WCFS_SCHEDULER
    if (task == NULL) {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    unsigned int ret;
    if (policy == OSAL_THREAD_POLICY_CFS) {
        ret = LOS_SetTaskPolicyCFS((unsigned int)(UINTPTR)task->task);
    } else if (policy == OSAL_THREAD_POLICY_DEFAULT || policy == OSAL_THREAD_POLICY_RT) {
        ret = LOS_SetTaskPolicyRT((unsigned int)(UINTPTR)task->task);
    } else {
        osal_log("param invalid!\n");
        return OSAL_FAILURE;
    }
    if (ret != LOS_OK) {
        return OSAL_FAILURE;
    }
    return OSAL_SUCCESS;
#else
    osal_log("[ERROR LOG]: Scheduling policy not supported.\n");
    return OSAL_SUCCESS;
#endif
}

void osal_kthread_lock(void)
{
    LOS_TaskLock();
}

void osal_kthread_unlock(void)
{
    LOS_TaskUnlock();
}

void osal_kthread_set_affinity(osal_task *task, int cpu_mask)
{
    osal_unused(task, cpu_mask);
    return;
}

int osal_kthread_should_stop(void)
{
    return 0;
}

void osal_kthread_destroy(osal_task *task, unsigned int stop_flag)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("param invalid!\n");
        return;
    }
    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    task->task = NULL;
    LOS_MemFree((void*)m_aucSysMem0, (void*)task);
    if (stop_flag == 0) {
        return;
    }
    ret = LOS_TaskDelete(uwtask_id);
    if (ret != LOS_OK && ret != LOS_ERRNO_TSK_NOT_CREATED) {
        osal_log("TaskDelete fail! ret=%#x.\n", ret);
    }
}

void osal_schedule(void)
{
    LOS_Schedule();
}

void osal_kthread_suspend(osal_task *task)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("param invalid!\n");
        return;
    }

    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    ret = LOS_TaskSuspend(uwtask_id);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_TSK_ALREADY_SUSPENDED)) {
        osal_log("TaskSuspend fail! ret=%#x.\n", ret);
        return;
    }
}

void osal_kthread_resume(osal_task *task)
{
    unsigned int ret;
    if (task == NULL) {
        osal_log("param invalid!\n");
        return;
    }

    unsigned int uwtask_id = (unsigned int)(UINTPTR)task->task;
    ret = LOS_TaskResume(uwtask_id);
    if ((ret != LOS_OK) && (ret != LOS_ERRNO_TSK_NOT_SUSPENDED)) {
        osal_log("TaskResume fail! ret=%#x.\n", ret);
        return;
    }
}

void osal_kthread_schedule(unsigned int sleep_ns)
{
    osal_unused(sleep_ns);
    return;
}

void osal_kthread_recycle(void)
{
    LOS_TaskResRecycle();
}

void osal_kthread_set_uninterrupt(void)
{
    return;
}

void osal_kthread_set_running(void)
{
    return;
}

void osal_cond_resched(void)
{
    return;
}

void osal_yield(void)
{
    // linux/sched.h:#define cond_resched() do {} while (0)
    return;
}

void osal_kneon_begin(void)
{
    return;
}

void osal_kneon_end(void)
{
    return;
}

/* Description: get cur thread id */
long osal_get_current_tid(void)
{
    unsigned int task_id = LOS_CurTaskIDGet();
    if (task_id == LOS_ERRNO_TSK_ID_INVALID) {
        osal_log("CurTaskIDGet fail!\n");
        return OSAL_FAILURE;
    }
    return (long)task_id;
}

/* Description: get pid of cur thread id */
long osal_get_current_pid(void)
{
    return osal_get_current_tid();
}

unsigned long osal_msleep(unsigned int msecs)
{
    LOS_Msleep(msecs);
    return 0;
}

void osal_msleep_uninterruptible(unsigned int msecs)
{
    LOS_Msleep(msecs);
}

void osal_udelay(unsigned int usecs)
{
    LOS_Udelay(usecs);
}

void osal_mdelay(unsigned int msecs)
{
    LOS_Mdelay(msecs);
}

unsigned int osal_kernel_init(void)
{
    if (OS_INT_ACTIVE) {
        return OSAL_FAILURE;
    }

    if (g_kernelState != OSAL_KERNEL_STATUS_INACTIVE) {
        return OSAL_FAILURE;
    }

    if (OsMain() == LOS_OK) {
        g_kernelState = OSAL_KERNEL_STATUS_READY;
        return OSAL_SUCCESS;
    } else {
        return OSAL_FAILURE;
    }
}

unsigned int osal_kernel_start(void)
{
    if (OS_INT_ACTIVE) {
        return OSAL_FAILURE;
    }
    if (g_kernelState != OSAL_KERNEL_STATUS_READY) {
        return OSAL_FAILURE;
    }

    g_kernelState = OSAL_KERNEL_STATUS_RUNING;
    OsStart();
    return OSAL_SUCCESS;
}

osal_kernel_status osal_kernel_get_state(void)
{
    if (!g_taskScheduled) {
        if (g_kernelState == OSAL_KERNEL_STATUS_READY) {
            return OSAL_KERNEL_STATUS_READY;
        } else {
            return OSAL_KERNEL_STATUS_INACTIVE;
        }
    } else if (OsPercpuGet()->taskLockCnt > 0) {
        return OSAL_KERNEL_STATUS_SCHEDULE_LOCK;
    } else {
        return OSAL_KERNEL_STATUS_RUNING;
    }
}
