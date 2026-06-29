/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef SENSOR_THREAD_H
#define SENSOR_THREAD_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <cmsis_os.h>
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    bool isRecursive;
} MutexAttr;

typedef void *MutexId;

MutexId SensorMutexCreate(MutexAttr *attr);

void SensorMutexDestroy(MutexId *mutex);

void SensorMutexLock(MutexId mutex);

void SensorMutexUnLock(MutexId mutex);

void SensorMutexGlobalLock(void);

void SensorMutexGlobalUnLock(void);

void SensorInitStaticMutexLock(MutexId *mutex);

typedef void *ThreadId;
typedef void (*ThreadFunc)(void *argv);

typedef struct {
    const char *name;    // name of the thread
    uint32_t stackSize;  // size of stack
    uint8_t priority;    // initial thread priority
    ThreadFunc func;
    void *argv;
} SensorThreadCfg;

ThreadId SensorThreadCreate(const SensorThreadCfg *cfg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif  // SENSOR_THREAD_H
