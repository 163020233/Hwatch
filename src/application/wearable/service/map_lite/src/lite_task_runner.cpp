/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

#include "lite_task_runner.h"
#include "securec.h"
#include "amap_log_adapter.h"

namespace OHOS {

static constexpr UINT32 EXIT_WAIT_TIMEOUT_MS = 100;

LiteTaskRunner::LiteTaskRunner(
    const char *name, UINT16 priority, UINT32 stackSize, UINT16 queueDepth, use_mem_type memType)
    : priority_(priority), stackSize_(stackSize), queueDepth_(queueDepth), should_exit_(false), use_mem_(memType),
      stackAddr_(0)
{
    if (name) {
        (void)strncpy_s(name_, sizeof(name_), name, sizeof(name_) - 1);
    } else {
        (void)strcpy_s(name_, sizeof(name_), "TaskRunner");
    }
}

LiteTaskRunner::~LiteTaskRunner()
{
    // 安全停止任务线程
    Stop();
}

bool LiteTaskRunner::Start()
{
    AMAP_LOGD("LiteTaskRunner::Start() name=%s", name_);
    if (started_) {
        return true;
    }

    // 创建退出信号量（初始计数为 0）
    AMAP_LOGD("LiteTaskRunner creating exit sem");
    UINT32 ret = LOS_SemCreate(0, &exit_sem_);
    if (ret != LOS_OK) {
        AMAP_LOGE("[%s] Sem create failed: %u", name_, ret);
        return false;
    }
    AMAP_LOGD("LiteTaskRunner exit sem created: %u", exit_sem_);
    AMAP_LOGD("LiteTaskRunner creating queue, depth=%u, size=%u", queueDepth_, sizeof(TaskCommand));
    // 创建队列
    ret = LOS_QueueCreate(name_, queueDepth_, &queueId_, 0, sizeof(TaskCommand));
    if (ret != LOS_OK) {
        AMAP_LOGE("[%s] Queue create failed: %u", name_, ret);
        LOS_SemDelete(exit_sem_);
        exit_sem_ = OSAL_SEM_INVALID;
        return false;
    }
    AMAP_LOGD("LiteTaskRunner queue created: %u", queueId_);
    AMAP_LOGD("LiteTaskRunner creating task, stack=%u, prio=%u", stackSize_, priority_);
    // 创建任务
    TSK_INIT_PARAM_S taskInit{};
    taskInit.pfnTaskEntry = (TSK_ENTRY_FUNC)TaskEntry;
    taskInit.uwStackSize = stackSize_;
    taskInit.pcName = name_;
    taskInit.usTaskPrio = priority_;
    taskInit.pArgs = (VOID *)this;

    VOID *topStack = nullptr;
    if (use_mem_ == USE_EXTEND_MEM) {
        topStack = memalign(ALIGNMENT_16, stackSize_);
        if (topStack == nullptr) {
            AMAP_LOGE("[%s] Stack alloc failed, falling back to DTCM", name_);
            stackAddr_ = 0;
        } else {
            stackAddr_ = (UINT32)topStack;
        }
    }

    if (topStack != nullptr) {
        ret = LOS_TaskCreateStatic(&taskId_, &taskInit, topStack);
    } else {
        ret = LOS_TaskCreate(&taskId_, &taskInit);
    }
    if (ret != LOS_OK) {
        AMAP_LOGE("[%s] Task create failed: %u", name_, ret);
        LOS_QueueDelete(queueId_);
        queueId_ = ID_UN_INIT_VALUE;
        LOS_SemDelete(exit_sem_);
        exit_sem_ = OSAL_SEM_INVALID;
        if (stackAddr_ != 0) {
            free((VOID *)stackAddr_);
            stackAddr_ = 0;
        }
        return false;
    }
    AMAP_LOGD("LiteTaskRunner task created: %u", taskId_);
    started_ = true;
    should_exit_ = false;
    return true;
}

void LiteTaskRunner::Stop()
{
    if (!started_) {
        return;
    }

    should_exit_ = true;
    // 清空队列（避免残留任务执行）
    TaskCommand cmd;
    UINT32 bufferSize = sizeof(cmd);
    while (LOS_QueueReadCopy(queueId_, &cmd, &bufferSize, 0) == LOS_OK) {
        // 丢弃任务，不执行！
        // （如果是同步任务，需特殊处理：post sem 防止调用方 hang）
        if (cmd.is_sync && cmd.sem != OSAL_SEM_INVALID) {
            LOS_SemPost(cmd.sem);  // 避免调用方永久阻塞
        }
    }

    // 退出命令确保唤醒
    cmd = {};
    (void)LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);

    if (exit_sem_ != OSAL_SEM_INVALID) {
        (void)LOS_SemPend(exit_sem_, EXIT_WAIT_TIMEOUT_MS);
        (void)LOS_SemDelete(exit_sem_);
        exit_sem_ = OSAL_SEM_INVALID;
    }
    // 清理队列
    if (queueId_ != ID_UN_INIT_VALUE) {
        (void)LOS_QueueDelete(queueId_);
        queueId_ = ID_UN_INIT_VALUE;
    }
    // 释放申请的栈内存
    if (stackAddr_ != 0) {
        free((VOID *)stackAddr_);
        stackAddr_ = 0;
    }
    // 重置状态
    started_ = false;
    taskId_ = ID_UN_INIT_VALUE;
}

VOID LiteTaskRunner::TaskEntry(VOID *arg)
{
    LiteTaskRunner *self = reinterpret_cast<LiteTaskRunner *>(arg);
    TaskCommand cmd;
    UINT32 ret;
    UINT32 bufferSize;

    while (1) {
        bufferSize = sizeof(cmd);
        ret = LOS_QueueReadCopy(self->queueId_, &cmd, &bufferSize, LOS_WAIT_FOREVER);
        if (ret != LOS_OK) {
            continue;
        }

        // 退出命令：func == nullptr 且 should_exit_ 为 true
        if (self->should_exit_ && cmd.func == nullptr) {
            // 退出前通知当前线程
            if (self->exit_sem_ != OSAL_SEM_INVALID) {
                LOS_SemPost(self->exit_sem_);
            }
            return;
        }
        // 执行任务
        if (cmd.func) {
            if (cmd.is_sync) {
                // 同步任务执行
                ProcessSyncTask(cmd);
            } else {
                // 异步任务
                cmd.func(cmd.func_arg);
            }
        }
        // 执行回调，仅异步场景执行
        if (!cmd.is_sync && cmd.callback) {
            cmd.callback(cmd.callback_arg);
        }
    }
}

void LiteTaskRunner::ProcessSyncTask(TaskCommand &cmd)
{
    if (cmd.func == nullptr) {
        AMAP_LOGE("ProcessSyncTask: func is nullptr!");
        if (cmd.sem != OSAL_SEM_INVALID) {
            LOS_SemPost(cmd.sem);
        }
        return;
    }
    if (cmd.sync_result == nullptr) {
        AMAP_LOGE("ProcessSyncTask: sync_result is nullptr!");
        // 按 NONE 类型处理
        auto f = reinterpret_cast<void (*)(void *)>(cmd.func);
        f(cmd.func_arg);
        if (cmd.sem != OSAL_SEM_INVALID) {
            LOS_SemPost(cmd.sem);
        }
        return;
    }

    switch (cmd.sync_result->type) {
        case SyncResultType::I32: {
            auto f = reinterpret_cast<int32_t (*)(void *)>(cmd.func);
            cmd.sync_result->value.i32 = f(cmd.func_arg);
            break;
        }
        case SyncResultType::F32: {
            auto f = reinterpret_cast<float (*)(void *)>(cmd.func);
            cmd.sync_result->value.f32 = f(cmd.func_arg);
            break;
        }
        case SyncResultType::PTR: {
            auto f = reinterpret_cast<void *(*)(void *)>(cmd.func);
            cmd.sync_result->value.ptr = f(cmd.func_arg);
            break;
        }
        case SyncResultType::NONE:
        default: {
            auto f = reinterpret_cast<void (*)(void *)>(cmd.func);
            f(cmd.func_arg);
            break;
        }
    }
    // 唤醒调用方
    if (cmd.sem != OSAL_SEM_INVALID) {
        LOS_SemPost(cmd.sem);
    }
}

bool LiteTaskRunner::PostAsyncTask(TaskRunnerFunc func, void *arg)
{
    return PostAsyncTaskWithCallback(func, arg, nullptr, nullptr);
}

bool LiteTaskRunner::PostAsyncTaskWithCallback(
    TaskRunnerFunc func, void *func_arg, TaskRunnerCallback callback, void *callback_arg)
{
    if (!started_ || queueId_ == LOS_ERRNO_QUEUE_INVALID) {
        return false;
    }

    TaskCommand cmd = {};
    cmd.func = func;
    cmd.func_arg = func_arg;
    cmd.callback = callback;
    cmd.callback_arg = callback_arg;

    UINT32 ret = LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);
    return (ret == LOS_OK);
}

int32_t LiteTaskRunner::PostSyncTaskInt32(int32_t (*func)(void *), void *arg)
{
    if (!started_ || queueId_ == LOS_ERRNO_QUEUE_INVALID) {
        return -1;  // 错误码
    }

    UINT32 sem;
    UINT32 ret = LOS_SemCreate(0, &sem);
    if (ret != LOS_OK) {
        return -1;
    }

    SyncResult result;
    result.type = SyncResultType::I32;

    TaskCommand cmd = {};
    cmd.func = reinterpret_cast<TaskRunnerFunc>(func);
    cmd.func_arg = arg;
    cmd.is_sync = true;
    cmd.sem = sem;
    cmd.sync_result = &result;
    if (cmd.func == nullptr) {
        return -1;
    }
    ret = LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);
    if (ret != LOS_OK) {
        LOS_SemDelete(sem);
        return -1;
    }
    // 阻塞等待任务完成
    LOS_SemPend(sem, LOS_WAIT_FOREVER);
    LOS_SemDelete(sem);
    return result.value.i32;
}

float LiteTaskRunner::PostSyncTaskFloat(float (*func)(void *), void *arg)
{
    if (!started_ || queueId_ == LOS_ERRNO_QUEUE_INVALID) {
        return 0.0f;
    }

    UINT32 sem;
    UINT32 ret = LOS_SemCreate(0, &sem);
    if (ret != LOS_OK) {
        return 0.0f;
    }

    SyncResult result;
    result.type = SyncResultType::F32;

    TaskCommand cmd = {};
    cmd.func = reinterpret_cast<TaskRunnerFunc>(func);
    cmd.func_arg = arg;
    cmd.is_sync = true;
    cmd.sem = sem;
    cmd.sync_result = &result;

    ret = LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);
    if (ret != LOS_OK) {
        LOS_SemDelete(sem);
        return 0.0f;
    }

    LOS_SemPend(sem, LOS_WAIT_FOREVER);
    LOS_SemDelete(sem);

    return result.value.f32;
}

bool LiteTaskRunner::PostSyncTaskVoid(void (*func)(void *), void *arg)
{
    if (!started_ || queueId_ == LOS_ERRNO_QUEUE_INVALID) {
        return false;
    }

    UINT32 sem;
    UINT32 ret = LOS_SemCreate(0, &sem);
    if (ret != LOS_OK) {
        return false;
    }

    SyncResult result;
    result.type = SyncResultType::NONE;

    TaskCommand cmd = {};
    cmd.func = reinterpret_cast<TaskRunnerFunc>(func);
    cmd.func_arg = arg;
    cmd.is_sync = true;
    cmd.sem = sem;
    cmd.sync_result = &result;

    ret = LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);
    if (ret != LOS_OK) {
        LOS_SemDelete(sem);
        return false;
    }

    LOS_SemPend(sem, LOS_WAIT_FOREVER);
    LOS_SemDelete(sem);

    return true;
}

void *LiteTaskRunner::PostSyncTaskPtr(void *(*func)(void *), void *arg)
{
    if (!started_ || queueId_ == LOS_ERRNO_QUEUE_INVALID) {
        return nullptr;
    }

    UINT32 sem;
    UINT32 ret = LOS_SemCreate(0, &sem);
    if (ret != LOS_OK) {
        return nullptr;
    }

    SyncResult result;
    result.type = SyncResultType::PTR;

    TaskCommand cmd = {};
    cmd.func = reinterpret_cast<TaskRunnerFunc>(func);
    cmd.func_arg = arg;
    cmd.is_sync = true;
    cmd.sem = sem;
    cmd.sync_result = &result;

    ret = LOS_QueueWriteCopy(queueId_, &cmd, sizeof(cmd), LOS_NO_WAIT);
    if (ret != LOS_OK) {
        LOS_SemDelete(sem);
        return nullptr;
    }

    LOS_SemPend(sem, LOS_WAIT_FOREVER);
    LOS_SemDelete(sem);

    return result.value.ptr;
}
}  // namespace OHOS
