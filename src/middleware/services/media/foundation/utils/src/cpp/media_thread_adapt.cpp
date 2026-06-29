/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "media_thread_adapt.h"
#include <cstdlib>
#include "media_log.h"
#include "securec.h"
#include "media_mem.h"

#ifdef CMSIS_SUPPORT
#include <cmsis_os2.h>
#ifdef HI3322_CHIP_V100
#include "osal_addr.h"
#include "chip_init.h"
#endif
#else
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_PRI_MAX 99
#define TIME_SCALE_S_2_US 1000000
#define TIME_SCALE 1000
#ifdef CMSIS_SUPPORT
static void MediaSetCmsisThreadPriorityAttr(osThreadAttr_t &attr, uint32_t priority)
{
    if (priority == 0) {
        return;
    }

    if (priority > THREAD_PRI_MAX) {
        priority = THREAD_PRI_MAX;
    }
    
    uint32_t tmp = priority * (static_cast<uint32_t>(osPriorityHigh) - static_cast<uint32_t>(osPriorityLow3)) /
        THREAD_PRI_MAX + static_cast<uint32_t>(osPriorityLow3);
    attr.priority = static_cast<osPriority_t>(tmp);
}
#else
pthread_mutex_t g_globalMediaMutexLock = PTHREAD_MUTEX_INITIALIZER;
static bool MediaSetPosixThreadAttr(pthread_attr_t *attr, const MediaThreadattr *halAttr)
{
    bool set = false;
    if (halAttr->detached) {
        pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
        set = true;
    }

    if (halAttr->stackSize > 0) {
        pthread_attr_setstacksize(attr, halAttr->stackSize);
        set = true;
    }

    if (halAttr->sched == THREAD_SCHED_INVALID || halAttr->sched > THREAD_SCHED_RR || halAttr->prioty == 0) {
        return set;
    }

    int32_t threadSched;
    int32_t pri = ((halAttr->prioty > 0) && (halAttr->prioty < THREAD_PRI_MAX)) ? halAttr->prioty : THREAD_PRI_MAX;
    if (halAttr->sched == THREAD_SCHED_FIFO) {
        threadSched = SCHED_FIFO;
    } else if (halAttr->sched == THREAD_SCHED_RR) {
        threadSched = SCHED_RR;
    } else {
        threadSched = SCHED_OTHER;
        pri = 0;
    }
    pthread_attr_setschedpolicy(attr, threadSched);

    struct sched_param schedParam;
    (void)memset_s(&schedParam, sizeof(schedParam), 0, sizeof(schedParam));
    schedParam.sched_priority = pri;
    pthread_attr_setschedparam(attr, static_cast<const struct sched_param *>(&schedParam));

    return true;
}
#endif

MediaMutexHandle MediaMutexCreate(MediaMutexAttr *attr)
{
#ifdef CMSIS_SUPPORT
    osMutexAttr_t mutexAttr = { nullptr, 0, nullptr, 0 };
    osMutexAttr_t *pAttr = nullptr;
    if (attr != nullptr && attr->isRecursive) {
        mutexAttr.attr_bits |= osMutexRecursive;
        pAttr = &mutexAttr;
    }
    osMutexId_t mutex = osMutexNew(pAttr);
    return mutex;
#else
    pthread_mutex_t *mutexId = static_cast<pthread_mutex_t *>(MediaMemCalloc(sizeof(pthread_mutex_t)));
    if (mutexId == nullptr) {
        return nullptr;
    }
    pthread_mutexattr_t mutexAttr = {0};
    pthread_mutexattr_t *pAttr = nullptr;
    bool needSetAttr = (attr != nullptr && attr->isRecursive && pthread_mutexattr_init(&mutexAttr) == 0);
    if (needSetAttr) {
        if (pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_RECURSIVE) == 0) {
            pAttr = &mutexAttr;
        }
    }
    (void)pthread_mutex_init(mutexId, pAttr);
    if (needSetAttr) {
        pthread_mutexattr_destroy(&mutexAttr);
    }
    return mutexId;
#endif
}

void MediaMutexDestroy(MediaMutexHandle *mutex)
{
    if (mutex == nullptr || *mutex == nullptr) {
        return;
    }
#ifdef CMSIS_SUPPORT
    (void)osMutexDelete(*mutex);
#else
    if (*mutex == nullptr) {
        return;
    }

    (void)pthread_mutex_destroy((pthread_mutex_t *)*mutex);
    MediaMemFree(*mutex);
#endif
    *mutex = nullptr;
}

void MediaMutexLock(MediaMutexHandle mutex)
{
    if (mutex == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexAcquire(mutex, osWaitForever);
#else
    (void)pthread_mutex_lock((pthread_mutex_t *)mutex);
#endif
}

void MediaMutexUnLock(MediaMutexHandle mutex)
{
    if (mutex == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    (void)osMutexRelease(mutex);
#else
    (void)pthread_mutex_unlock((pthread_mutex_t *)mutex);
#endif
}

void MediaMutexGlobalLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelLock();
#else
    pthread_mutex_lock(&g_globalMediaMutexLock);
#endif
}

void MediaMutexGlobalUnLock(void)
{
#ifdef CMSIS_SUPPORT
    osKernelUnlock();
#else
    pthread_mutex_unlock(&g_globalMediaMutexLock);
#endif
}

void MediaInitStaticMutexLock(MediaMutexHandle *mutex)
{
    if (mutex == nullptr || *mutex != nullptr) {
        return;
    }

    MediaMutexGlobalLock();
    if (*mutex == nullptr) {
        MediaMutexAttr attr = { false };
        *mutex = MediaMutexCreate(&attr);
    }
    MediaMutexGlobalUnLock();
}

typedef struct {
    MediaThreadFunType process;
    void *argument;
    MediaThreadIdHandle mainId;
    MediaThreadIdHandle subId;
    bool detached;
	MediaThreadCondHandle cond;
    void *stackMem;
} MediaThreadMember;

#define THREAD_MANAGER_MAX_CNT 10
static MediaMutexHandle g_mediaThreadManagerLock;
static MediaThreadMember *g_mediaThreadManager[THREAD_MANAGER_MAX_CNT] = { nullptr };

static uint32_t MediaGetValidThreadManagerIdx()
{
    for (uint32_t i = 0; i < THREAD_MANAGER_MAX_CNT; i++) {
        if (g_mediaThreadManager[i] == nullptr) {
            return i;
        }
    }

    return UINT32_MAX;
}

static uint32_t MediaGetThreadManagerIdxFromIdHandle(const MediaThreadIdHandle id)
{
    if (id == nullptr) {
        return UINT32_MAX;
    }

    for (uint32_t i = 0; i < THREAD_MANAGER_MAX_CNT; i++) {
        if (g_mediaThreadManager[i] != nullptr &&
            g_mediaThreadManager[i]->subId != nullptr && g_mediaThreadManager[i]->subId == id) {
            return i;
        }
    }

    return UINT32_MAX;
}

static void FreeStackMemory(MediaThreadMember *member) {
    if (member != nullptr && member->stackMem != nullptr) {
        free(member->stackMem);
        member->stackMem = nullptr;
    }
}

#define THREAD_EVENT_EXIT_FLAG 0x45584954U /* EXIT ascii */
#ifdef CMSIS_SUPPORT
static void MediaThreadFunc(void *arg)
#else
static void *MediaThreadFunc(void *arg)
#endif
{
    if (arg == nullptr) {
#ifdef CMSIS_SUPPORT
        return;
#else
        return nullptr;
#endif
    }
    MediaThreadMember *member = (MediaThreadMember *)arg;
    member->process(member->argument);
    MediaMutexLock(g_mediaThreadManagerLock);
    if (member->detached) {
        uint32_t idx = MediaGetThreadManagerIdxFromIdHandle(member->subId);
        if (idx < THREAD_MANAGER_MAX_CNT) {
            g_mediaThreadManager[idx] = nullptr;
        }
        MediaThreadCondDestroy(&member->cond);
        FreeStackMemory(member);
        MediaMemFree(arg);
    }
#ifdef CMSIS_SUPPORT
    else {
        MediaThreadCondSignal(member->cond);
    }
#endif
    MediaMutexUnLock(g_mediaThreadManagerLock);
#ifndef CMSIS_SUPPORT
    return nullptr;
#endif
}

static bool MediaCheckAlignThread(const MediaThreadattr *attr)
{
    if (strcmp(attr->name, "PlaySch") != 0 &&
        strcmp(attr->name, "playerTask") != 0 &&
        strcmp(attr->name, "tonePlayerTask") != 0 &&
        strcmp(attr->name, "VideoPlayThread") != 0 &&
        strcmp(attr->name, "A2dpSinkThread") != 0 &&
        strcmp(attr->name, "AudioCaptureTask") != 0 &&
        strcmp(attr->name, "StreamProc") != 0) {
        return true;
    }
    return false;
}

static int32_t MediaDoCreateThread(MediaThreadMember *member, const MediaThreadattr *attr)
{
#ifdef CMSIS_SUPPORT
    MediaThreadIdHandle subId = nullptr;
    if (attr == nullptr) {
        subId = osThreadNew(MediaThreadFunc, member, nullptr);
    } else {
        if (!attr->detached) {
            member->cond = MediaThreadCondCreate();
            if (member->cond == NULL) {
                return -1;
            }
        }
        uint32_t attrBits = osThreadDetached;
        osThreadAttr_t taskAttr = {
            attr->name, attrBits, nullptr, 0, nullptr, attr->stackSize, osPriorityNormal, 0, 0
        };
#ifdef HI3322_CHIP_V100
        if (MediaCheckAlignThread(attr)) {
            const unsigned int ALIGN_BITS = 16;
            taskAttr.stack_mem = osal_pool_mem_alloc_align(OS_EXTEND_MEM_ADDR, taskAttr.stack_size, ALIGN_BITS);
		    member->stackMem = taskAttr.stack_mem;
        }
#endif
        MediaSetCmsisThreadPriorityAttr(taskAttr, attr->prioty);
        subId = osThreadNew(MediaThreadFunc, member, &taskAttr);
    }
    member->subId = subId;
#else
    pthread_attr_t taskAttr;
    pthread_attr_t *pTaskAttr = nullptr;
    bool needSetAttr = (attr != nullptr && pthread_attr_init(&taskAttr) == 0);
    if (needSetAttr && MediaSetPosixThreadAttr(&taskAttr, attr)) {
        pTaskAttr = &taskAttr;
    }

    pthread_t thdId = 0;
    if (pthread_create(&thdId, pTaskAttr, MediaThreadFunc, member) != 0) {
        if (needSetAttr) {
            (void)pthread_attr_destroy(&taskAttr);
        }
        return -1;
    }
    if (needSetAttr) {
        (void)pthread_attr_destroy(&taskAttr);
    }
    member->subId = (MediaThreadIdHandle)(uintptr_t)thdId;
#endif

    return  member->subId != nullptr ? 0 : -1;
}

MediaThreadIdHandle MediaThreadCreate(MediaThreadFunType func, void *argument, const MediaThreadattr *attr)
{
    if (func == nullptr) {
        return nullptr;
    }

    MediaThreadMember *member = static_cast<MediaThreadMember *>(MediaMemCalloc(sizeof(MediaThreadMember)));
    if (member == nullptr) {
        return nullptr;
    }

    MediaThreadIdHandle id = nullptr;
    MediaInitStaticMutexLock(&g_mediaThreadManagerLock);
    MediaMutexLock(g_mediaThreadManagerLock);
    uint32_t idx = MediaGetValidThreadManagerIdx();
    if (idx >= THREAD_MANAGER_MAX_CNT) {
        MediaMemFree(member);
        member = nullptr;
        goto EXIT;
    }

    member->process = func;
    member->argument = argument;
    member->mainId = MediaThreadGetId();
    member->detached = attr != nullptr ? attr->detached : false;
    member->stackMem = nullptr;
    member->cond = nullptr;
    if (MediaDoCreateThread(member, attr) != 0) {
        FreeStackMemory(member);
        MediaMemFree(member);
        member = nullptr;
        goto EXIT;
    }
    g_mediaThreadManager[idx] = member;
    id = member->subId;

EXIT:
    MediaMutexUnLock(g_mediaThreadManagerLock);
    return id;
}

MediaThreadIdHandle MediaThreadGetId(void)
{
#ifdef CMSIS_SUPPORT
    return osThreadGetId();
#else
    return (MediaThreadIdHandle)(uintptr_t)pthread_self();
#endif
}

void MediaThreadJoin(MediaThreadIdHandle *thdId)
{
    if (thdId == nullptr || *thdId == nullptr) {
        return;
    }
#ifndef CMSIS_SUPPORT
    pthread_join((pthread_t)(uintptr_t)*thdId, NULL);
#endif
    MediaMutexLock(g_mediaThreadManagerLock);
    uint32_t idx = MediaGetThreadManagerIdxFromIdHandle(*thdId);
    if (idx < THREAD_MANAGER_MAX_CNT) {
        MediaThreadMember *member = g_mediaThreadManager[idx];
         if (member == nullptr) {
            *thdId = nullptr;
            MediaMutexUnLock(g_mediaThreadManagerLock);
            return;
        }
        if (member->detached) { /* detach sub thread, will free handle after sub thread exit */
            *thdId = nullptr;
            FreeStackMemory(member);
            MediaMutexUnLock(g_mediaThreadManagerLock);
            return;
        }
#ifdef CMSIS_SUPPORT
        MediaThreadCondWait(member->cond, g_mediaThreadManagerLock);
        MediaThreadCondDestroy(&member->cond);
#endif
        g_mediaThreadManager[idx] = nullptr;
		FreeStackMemory(member);
        MediaMemFree(member);
        member = nullptr;
    }
    MediaMutexUnLock(g_mediaThreadManagerLock);
    *thdId = nullptr;
}

#define THREAD_EVENT_FLAG 0x00000001U
MediaThreadCondHandle MediaThreadCondCreate(void)
{
#ifdef CMSIS_SUPPORT
    return osEventFlagsNew(nullptr);
#else
    pthread_cond_t *condHandle = static_cast<pthread_cond_t *>(MediaMemCalloc(sizeof(pthread_cond_t)));
    if (condHandle == nullptr) {
        return nullptr;
    }

    pthread_cond_init(condHandle, nullptr);
    return condHandle;
#endif
}

void MediaThreadCondSignal(MediaThreadCondHandle condHandle)
{
    if (condHandle == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    osEventFlagsSet(condHandle, THREAD_EVENT_FLAG);
#else
    pthread_cond_signal((pthread_cond_t *)condHandle);
#endif
}

void MediaThreadCondWait(MediaThreadCondHandle condHandle, MediaMutexHandle mutexHandle)
{
    if (mutexHandle == nullptr || condHandle == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    MediaMutexUnLock(mutexHandle);
    osEventFlagsWait(condHandle, THREAD_EVENT_FLAG, osFlagsWaitAll, osWaitForever);
    MediaMutexLock(mutexHandle);
#else
    (void)pthread_cond_wait((pthread_cond_t *)condHandle, (pthread_mutex_t *)mutexHandle);
#endif
}

void MediaThreadCondTimeWait(MediaThreadCondHandle condHandle, MediaMutexHandle mutexHandle, uint32_t delayUs)
{
    if (mutexHandle == nullptr || condHandle == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    MediaMutexUnLock(mutexHandle);
    osEventFlagsWait(condHandle, THREAD_EVENT_FLAG, osFlagsWaitAll,
        static_cast<uint64_t>(delayUs) * osKernelGetTickFreq() / TIME_SCALE_S_2_US);
    MediaMutexLock(mutexHandle);
#else
    struct timespec outtime;
    if (delayUs >= TIME_SCALE_S_2_US) {
        outtime.tv_sec = delayUs / TIME_SCALE_S_2_US;
        uint32_t outtimeNsec = (delayUs % TIME_SCALE_S_2_US) * TIME_SCALE;
        outtime.tv_nsec = outtimeNsec;
    } else {
        outtime.tv_sec = 0;
        outtime.tv_nsec = delayUs * TIME_SCALE;
    }

    pthread_cond_timedwait((pthread_cond_t *)condHandle, (pthread_mutex_t *)mutexHandle, &outtime);
#endif
}

void MediaThreadCondDestroy(MediaThreadCondHandle *condHandle)
{
    if (condHandle == nullptr || *condHandle == nullptr) {
        return;
    }

#ifdef CMSIS_SUPPORT
    osEventFlagsDelete(*condHandle);
    *condHandle = nullptr;
#else
    pthread_cond_destroy((pthread_cond_t *)*condHandle);
    MediaMemFree(*condHandle);
    *condHandle = nullptr;
#endif
}

void MediaThreadSetDetach(MediaThreadIdHandle thdId, bool detach)
{
    MediaMutexLock(g_mediaThreadManagerLock);
    uint32_t idx = MediaGetThreadManagerIdxFromIdHandle(thdId);
    if (idx < THREAD_MANAGER_MAX_CNT) {
        MediaThreadMember *member = g_mediaThreadManager[idx];
        member->detached = detach;
    }
    MediaMutexUnLock(g_mediaThreadManagerLock);
}

#ifdef __cplusplus
};
#endif
