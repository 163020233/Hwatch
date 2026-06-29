/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-11
 */

// task_runner.h
#ifndef LITE_TASK_RUNNER_H
#define LITE_TASK_RUNNER_H

#include <cstdint>
#include "los_queue.h"
#include "los_task.h"
#include "los_sem.h"
#include "thread_init.h"

namespace OHOS {
constexpr UINT32 OSAL_SEM_INVALID = 0xFFFFFFFFU;
const UINT32 ID_UN_INIT_VALUE = 0xFFFFFFFFU;

// 函数指针类型：任务执行体
typedef void (*TaskRunnerFunc)(void *arg);

// 回调函数类型：任务完成后调用
typedef void (*TaskRunnerCallback)(void *callback_arg);

// 同步任务的返回值类型
enum class SyncResultType {
    NONE,  // 无返回值
    I32,   // 返回int_32
    F32,   // 返回float32
    PTR    // 返回指针
};

// 同步任务结果
struct SyncResult {
    SyncResultType type;
    union {
        int32_t i32;
        float f32;
        void *ptr;
    } value;
};

// 任务命令
#pragma pack(push, 4)
struct TaskCommand {
    TaskRunnerFunc func;
    void *func_arg;

    TaskRunnerCallback callback;
    void *callback_arg;

    // 是否是同步任务
    bool is_sync = false;
    // 同步信号量
    UINT32 sem = OSAL_SEM_INVALID;
    // 指向结果存储区
    SyncResult *sync_result = nullptr;
};
#pragma pack(pop)

class LiteTaskRunner {
public:
    // 构造时指定任务名、优先级、栈大小、队列深度，当前栈大小为48K
    LiteTaskRunner(const char *name, UINT16 priority, UINT32 stackSize = 0xC000, UINT16 queueDepth = 8,
        use_mem_type memType = USE_EXTEND_MEM);

    ~LiteTaskRunner();

    // 启动任务线程（应在系统初始化阶段调用）
    bool Start();

    void Stop();

    // 投递普通任务
    bool PostAsyncTask(TaskRunnerFunc func, void *arg);

    // 投递带回调的任务
    bool PostAsyncTaskWithCallback(
        TaskRunnerFunc func, void *func_arg, TaskRunnerCallback callback, void *callback_arg);

    int32_t PostSyncTaskInt32(int32_t (*func)(void *), void *arg);

    // 同步执行 float 返回值的任务
    float PostSyncTaskFloat(float (*func)(void *), void *arg);

    // 同步执行无返回值任务（仅等待完成）
    bool PostSyncTaskVoid(void (*func)(void *), void *arg);

    // 同步执行返回 void* 的任务
    void *PostSyncTaskPtr(void *(*func)(void *), void *arg);

private:
    // LOS 任务入口
    static VOID TaskEntry(VOID *arg);

    char name_[32];
    UINT16 priority_;
    UINT32 stackSize_;
    UINT16 queueDepth_;

    UINT32 taskId_ = ID_UN_INIT_VALUE;
    UINT32 queueId_ = ID_UN_INIT_VALUE;
    bool started_ = false;

    bool should_exit_ = false;
    UINT32 exit_sem_ = OSAL_SEM_INVALID;
    // USE_EXTEND_MEM, USE_DTCM_MEM
    use_mem_type use_mem_;
    UINT32 stackAddr_ = 0;

private:
    static VOID ProcessSyncTask(TaskCommand &cmd);
};
}  // namespace OHOS

#endif  // TASK_RUNNER_H