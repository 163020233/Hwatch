/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "thread_adapter.h"
#include "common.h"
#include <cmsis_os.h>
// ohos begin
// brief fix codecheck
#define ALIGNMENT_16 16
// ohos end

MutexId MUTEX_InitValue()
{
    return osMutexNew(NULL);
}

void MUTEX_Lock(MutexId mutex)
{
    if (mutex == NULL) {
        return;
    }
    osMutexAcquire(mutex, osWaitForever);
}

void MUTEX_Unlock(MutexId mutex)
{
    if (mutex == NULL) {
        return;
    }
    osMutexRelease(mutex);
}

void MUTEX_GlobalLock(void)
{
    osKernelLock();
}

void MUTEX_GlobalUnlock(void)
{
    osKernelUnlock();
}

ThreadId THREAD_Create(Runnable run, void *argv, const ThreadAttr *attr)
{
    osThreadAttr_t taskAttr = {attr->name, 0, NULL, 0, NULL, attr->stackSize, (osPriority_t)(attr->priority), 0, 0};
    // ohos begin
    // brief Specify the stack memory pointer
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    taskAttr.stack_mem = memalign(ALIGNMENT_16, attr->stackSize);
#endif
    // ohos end
    ThreadId id = (ThreadId)osThreadNew((osThreadFunc_t)run, argv, &taskAttr);
#ifdef LOSCFG_TASK_STACK_STATIC_ALLOCATION
    if (id == NULL) {
        free(taskAttr.stack_mem);
    }
#endif
    return id;
}

int THREAD_Total(void)
{
    return osThreadGetCount();
}

void *THREAD_GetThreadLocal(void)
{
    return osThreadGetArgument();
}

void THREAD_SetThreadLocal(const void *local)
{
    (void)local;
}
