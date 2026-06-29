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

#ifndef __NET_WS_THREAD_H__
#define __NET_WS_THREAD_H__

#include <stdio.h>
#include "cmsis_os2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RWS_WAIT_FOREVER 0xffffffffu

/**
 @brief Callback type of thread function.
 @param userObject User object provided during thread creation.
 */
typedef void (*WsThreadFunct)(void *userObject);

struct WsThreadStruct {
    WsThreadFunct threadFunction;
    osThreadId_t thread;
    void *stackMem;
};

/**
 @brief Type of all public objects.
 */
typedef void *WsHandle;

/**
 @brief Mutex object handle.
 */
typedef WsHandle WsMutex;

/**
 @brief Mutex object handle.
 */
typedef WsHandle WsSem;

/**
 @brief Thread object handle.
 */
typedef struct WsThreadStruct *WsThread;

WsThread WsThreadCreate(WsThreadFunct threadFunction, void *userObject, const char *name);

void WsThreadMsleep(const unsigned int millisec);

WsMutex WsMutexCreateRecursive(void);

void WsMutexLock(WsMutex mutex);

void WsMutexUnLock(WsMutex mutex);

void WsMutexDelete(WsMutex mutex);

WsSem WsSemCreate(void);

void WsSemDelete(WsSem sem);

void WsSemSignal(WsSem sem);

int WsThreadSuspend(WsThread workThread);

int WsThreadResume(WsThread workThread);

int WsSemWait(WsSem sem, unsigned int timeout_ms);

#ifdef __cplusplus
}
#endif

#endif
