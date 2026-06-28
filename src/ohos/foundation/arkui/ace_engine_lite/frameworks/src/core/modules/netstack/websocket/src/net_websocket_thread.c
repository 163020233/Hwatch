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

#include "net_websocket_debug.h"
#include "net_websocket_thread.h"
#include "net_websocket_opts.h"

#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>
#include <malloc.h>
#include "securec.h"
#include "arch/task.h"
#include "cmsis_os2.h"

WsThread WsThreadCreate(WsThreadFunct threadFunction, void *userObject, const char *name)
{
    WsThread tno = NULL;
    int res = -1;
    osThreadAttr_t attr = {0};

    if (!threadFunction) {
        return NULL;
    }

    tno = (WsThread)calloc(1, sizeof(struct WsThreadStruct));
    if (tno == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("calloc fail\n"));
        return NULL;
    }
    tno->threadFunction = threadFunction;

    void *stackMem = memalign(LOSCFG_STACK_POINT_ALIGN_SIZE, WS_THREAD_STACK_SIZE);
    if (stackMem == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Create WebsocketThread memalign fail\n"));
        free(tno);
        return NULL;
    }

    attr.name = name;
    attr.attr_bits = osThreadJoinable;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = stackMem;
    attr.stack_size = WS_THREAD_STACK_SIZE;
    attr.priority = osPriorityNormal;

    tno->stackMem = stackMem;
    tno->thread = osThreadNew((osThreadFunc_t)threadFunction, (void *)userObject, &attr);
    if (tno->thread == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Create WebsocketThread failed\n"));
        free(tno);
        free(stackMem);
        return NULL;
    }
    return tno;
}

void WsThreadMsleep(const unsigned int millisec)
{
    usleep(millisec * 1000); /* 1000: ms */
}

int WsThreadSuspend(WsThread workThread)
{
    int ret;
    if (workThread == NULL) {
        return -1;
    }
    ret = osThreadSuspend(workThread->thread);
    if (ret == osOK) {
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("websocket thread suspend ok\n"));
    } else {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("websocket thread suspend fail,ret=0x%x\n", ret));
    }
    return ret;
}

int WsThreadResume(WsThread workThread)
{
    int ret;
    if (workThread == NULL) {
        return -1;
    }
    ret = osThreadResume(workThread->thread);
    if (ret == osOK) {
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("websocket thread resume ok\n"));
    } else {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("websocket thread resume fail,ret=0x%x\n", ret));
    }
    return ret;
}

WsMutex WsMutexCreateRecursive(void)
{
    pthread_mutex_t *mutex = (pthread_mutex_t *)calloc(1, sizeof(pthread_mutex_t));
    if (mutex == NULL) {
        return NULL;
    }
    pthread_mutexattr_t attr;
    if (pthread_mutexattr_init(&attr) != 0) {
        free(mutex);
        return NULL;
    }
    if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0) {
        pthread_mutexattr_destroy(&attr);
        free(mutex);
        return NULL;
    }
    if (pthread_mutex_init(mutex, &attr) != 0) {
        pthread_mutexattr_destroy(&attr);
        free(mutex);
        return NULL;
    }
    pthread_mutexattr_destroy(&attr);
    return mutex;
}

void WsMutexLock(WsMutex mutex)
{
    if (mutex) {
        pthread_mutex_lock((pthread_mutex_t *)mutex);
    }
}

void WsMutexUnLock(WsMutex mutex)
{
    if (mutex) {
        pthread_mutex_unlock((pthread_mutex_t *)mutex);
    }
}

void WsMutexDelete(WsMutex mutex)
{
    if (mutex) {
        pthread_mutex_destroy((pthread_mutex_t *)mutex);
        free(mutex);
    }
}

WsSem WsSemCreate(void)
{
    WsSem sem = (WsSem)calloc(1, sizeof(sem_t));
    if (sem != NULL && sem_init(sem, 0, 0) == -1) {
        free(sem);
        sem = NULL;
    }
    return sem;
}

void WsSemDelete(WsSem sem)
{
    if (sem) {
        sem_destroy(sem);
        free(sem);
    }
}

void WsSemSignal(WsSem sem)
{
    if (sem) {
        sem_post(sem);
    }
}

int WsSemWait(WsSem sem, unsigned int timeout_ms)
{
    int ret = -1;

    if (sem) {
        if (timeout_ms == RWS_WAIT_FOREVER) {
            ret = sem_wait(sem);
        } else {
            struct timespec abs_timeout;
            struct timeval tv;
            gettimeofday(&tv, NULL);

            abs_timeout.tv_sec = tv.tv_sec + (long)timeout_ms / 1000;                      /* 1000: ms */
            abs_timeout.tv_nsec = tv.tv_usec * 1000 + ((long)timeout_ms % 1000) * 1000000; /* 1000, 1000000: ms */
            ret = sem_timedwait(sem, &abs_timeout);
        }
    }

    return ret;
}