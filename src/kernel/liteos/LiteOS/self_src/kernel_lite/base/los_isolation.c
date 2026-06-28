/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description : LiteOS Kernel Isolation Module Implementation..
 * Author : Huawei LiteOS Team
 * Create : 2026-06-09
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
 */

#include "los_isolation.h"
#include "los_errno.h"
#include "string.h"
#include "los_task_pri.h"

STATIC IsolationTaskInfo g_isolationTaskTable[ISOLATION_TASK_MAX_NUM];
__attribute__((aligned(4))) UINT8 g_kernelCommonStack[KERNEL_COMMON_STACK_SIZE + 4]; // 冗余4字节
UINTPTR g_commonTopOfStack = 0;

IsolationTaskInfo *g_currentIsolationTask = NULL;
OsIsolationContext g_osIsolationContext = {0};
UINT32 g_isolation_swap_sp = 0;

STATIC UINT32 OsIsolationFindTaskIndex(const CHAR *taskName)
{
    UINT32 i;
    if (taskName == NULL) {
        return ISOLATION_TASK_MAX_NUM;
    }
    for (i = 0; i < ISOLATION_TASK_MAX_NUM; i++) {
        if ((g_isolationTaskTable[i].enterCallback != NULL) &&
            (strcmp(g_isolationTaskTable[i].taskName, taskName) == 0)) {
            return i;
        }
    }
    return ISOLATION_TASK_MAX_NUM;
}

STATIC UINT32 OsIsolationAllocTask(VOID)
{
    UINT32 i;
    for (i = 0; i < ISOLATION_TASK_MAX_NUM; i++) {
        if (g_isolationTaskTable[i].enterCallback == NULL) {
            return i;
        }
    }
    return ISOLATION_TASK_MAX_NUM;
}

UINT32 LOS_IsolationRegionRegister(IsolationTaskInfo *config)
{
    UINT32 intSave;
    UINT32 taskIndex;
    CHAR *newTaskName = NULL;
    UINT32 nameLen;

    if (config == NULL || config->taskName == NULL) {
        return LOS_NOK;
    }

    if (config->enterCallback == NULL || config->exitCallback == NULL) {
        return LOS_NOK;
    }

    intSave = LOS_IntLock();

    taskIndex = OsIsolationFindTaskIndex(config->taskName);
    if (taskIndex < ISOLATION_TASK_MAX_NUM) {
        LOS_IntRestore(intSave);
        return LOS_NOK;
    }

    taskIndex = OsIsolationAllocTask();
    if (taskIndex >= ISOLATION_TASK_MAX_NUM) {
        LOS_IntRestore(intSave);
        return LOS_NOK;
    }

    nameLen = strlen(config->taskName) + 1;
    newTaskName = (CHAR *)LOS_MemAlloc(OS_SYS_MEM_ADDR, nameLen);
    if (newTaskName == NULL) {
        LOS_IntRestore(intSave);
        return LOS_NOK;
    }

    if (memcpy_s(newTaskName, nameLen, config->taskName, nameLen) != 0) {
        (VOID) LOS_MemFree(OS_SYS_MEM_ADDR, newTaskName);
        LOS_IntRestore(intSave);
        return LOS_NOK;
    }

    (VOID) memset_s((VOID *)g_kernelCommonStack,
        ISOLATION_STACK_SIZE,
        (INT32)OS_STACK_INIT,
        ISOLATION_STACK_SIZE);

    (VOID) memset_s((VOID *)g_kernelCommonStack, OS_STACK_MAGIC_SIZE, (INT32)OS_STACK_MAGIC_WORD, OS_STACK_MAGIC_SIZE);
    g_commonTopOfStack = (UINT32)g_kernelCommonStack + KERNEL_COMMON_STACK_SIZE;

    g_isolationTaskTable[taskIndex].taskName = newTaskName;
    g_isolationTaskTable[taskIndex].enterCallback = config->enterCallback;
    g_isolationTaskTable[taskIndex].exitCallback = config->exitCallback;
    g_isolationTaskTable[taskIndex].baseAddress = config->baseAddress;
    g_isolationTaskTable[taskIndex].regionSize = config->regionSize;

    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 LOS_IsolationRegionUnregister(const CHAR *taskName)
{
    UINT32 intSave;
    UINT32 taskIndex;

    if (taskName == NULL) {
        return LOS_NOK;
    }

    intSave = LOS_IntLock();

    taskIndex = OsIsolationFindTaskIndex(taskName);
    if (taskIndex >= ISOLATION_TASK_MAX_NUM) {
        LOS_IntRestore(intSave);
        return LOS_NOK;
    }

    if (g_isolationTaskTable[taskIndex].taskName != NULL) {
        (VOID) LOS_MemFree(OS_SYS_MEM_ADDR, g_isolationTaskTable[taskIndex].taskName);
        g_isolationTaskTable[taskIndex].taskName = NULL;
    }
    g_isolationTaskTable[taskIndex].enterCallback = NULL;
    g_isolationTaskTable[taskIndex].exitCallback = NULL;

    if (g_currentIsolationTask == &g_isolationTaskTable[taskIndex]) {
        g_currentIsolationTask = NULL;
    }

    LOS_IntRestore(intSave);
    return LOS_OK;
}

IsolationTaskInfo *OsIsolationTaskCheck(const CHAR *taskName)
{
    UINT32 i;
    if (taskName == NULL) {
        return NULL;
    }

    for (i = 0; i < ISOLATION_TASK_MAX_NUM; i++) {
        if ((g_isolationTaskTable[i].enterCallback != NULL) &&
            (strcmp(g_isolationTaskTable[i].taskName, taskName) == 0)) {
            return &g_isolationTaskTable[i];
        }
    }
    return NULL;
}

VOID OsIsolationTaskSwitch(LosTaskCB *taskCB)
{
    IsolationTaskInfo *oldTask = NULL;
    IsolationTaskInfo *newTask = NULL;

    if (taskCB == NULL) {
        return;
    }

    oldTask = g_currentIsolationTask;
    newTask = OsIsolationTaskCheck(taskCB->taskName);

    if (oldTask != NULL && oldTask->exitCallback != NULL) {
        (VOID) oldTask->exitCallback();
    }

    if (newTask != NULL) {
        if ((newTask->enterCallback != NULL) && (newTask->enterCallback() != LOS_OK)) {
            g_currentIsolationTask = NULL;
            return;
        }
        g_currentIsolationTask = newTask;
    } else {
        g_currentIsolationTask = NULL;
    }
}

VOID OsIsolationContextSave(OsIsolationContext *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (g_currentIsolationTask != NULL) {
        ctx->enterCallback = g_currentIsolationTask->enterCallback;
        ctx->exitCallback = g_currentIsolationTask->exitCallback;
    }

    LosTaskCB *runTask = OsCurrTaskGet();
    if (runTask != NULL) {
        ctx->topOfStack = runTask->topOfStack;
        ctx->stackSize = runTask->stackSize;
        runTask->topOfStack = (UINTPTR)&g_kernelCommonStack;
        runTask->stackSize = KERNEL_COMMON_STACK_SIZE;
    #ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
        g_newTask_topOfStack = runTask->topOfStack;
    #endif
    }

    if (g_currentIsolationTask != NULL) {
        if (g_currentIsolationTask->exitCallback != NULL) {
            (VOID) g_currentIsolationTask->exitCallback();
        }
    }
}

VOID OsIsolationContextRestore(OsIsolationContext *ctx)
{
    if (ctx == NULL) {
        return;
    }

    if (ctx->enterCallback != NULL) {
        (VOID) ctx->enterCallback();
    }

    LosTaskCB *runTask = OsCurrTaskGet();
    if (runTask != NULL) {
        runTask->topOfStack = ctx->topOfStack;
        runTask->stackSize = ctx->stackSize;
    #ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
        g_newTask_topOfStack = runTask->topOfStack;
    #endif
    }
}
