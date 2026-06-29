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

#ifndef MEDIA_THREAD_ADAPT_H
#define MEDIA_THREAD_ADAPT_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    bool isRecursive;
} MediaMutexAttr;
typedef void *MediaMutexHandle;
MediaMutexHandle MediaMutexCreate(MediaMutexAttr *attr);
void MediaMutexDestroy(MediaMutexHandle *mutex);
void MediaMutexLock(MediaMutexHandle mutex);
void MediaMutexUnLock(MediaMutexHandle mutex);
void MediaMutexGlobalLock(void);
void MediaMutexGlobalUnLock(void);
void MediaInitStaticMutexLock(MediaMutexHandle *mutex);

typedef void *MediaThreadIdHandle;
typedef void* (*MediaThreadFunType)(void *argument);
typedef enum {
    THREAD_SCHED_INVALID = 0,
    THREAD_SCHED_OTHER,
    THREAD_SCHED_FIFO,
    THREAD_SCHED_RR,
} MediaThreadSchedPolicy;
typedef struct {
    const char          *name; /* only for cmsis */
    uint32_t            stackSize;
    MediaThreadSchedPolicy   sched; /* only for posix */
    uint32_t            prioty;
    bool                detached; /* only for posix */
} MediaThreadattr;
MediaThreadIdHandle MediaThreadCreate(MediaThreadFunType func, void *argument, const MediaThreadattr *attr);
MediaThreadIdHandle MediaThreadGetId(void);
void MediaThreadJoin(MediaThreadIdHandle *thdId);

typedef void *MediaThreadCondHandle;

/*
 * create thread cond handle
 * retrun the MediaThreadCondHandle
 */
MediaThreadCondHandle MediaThreadCondCreate(void);

/*
 * send a signal to sub running thread
 * param[in] condHandle   threadCondHandle by MediaThreadCondCreate
 */
void MediaThreadCondSignal(MediaThreadCondHandle condHandle);
/*
 * wait a signal in sub running thread; it must be called in running thread use cmsis interface
 * param[in] condHandle   threadCondHandle by MediaThreadCondCreate
 * param[in] mutexHandle  threadMutexHandle by MediaMutexCreate; use only for posix
 */
void MediaThreadCondWait(MediaThreadCondHandle condHandle, MediaMutexHandle mutexHandle);
/*
 * wait a signal in sub running thread; it must be called in running thread use cmsis interface
 * param[in] condHandle   threadCondHandle by MediaThreadCondCreate
 * param[in] mutexHandle  threadMutexHandle by MediaMutexCreate; use only for posix
 * param[in] delayUs      will wait time
 */
void MediaThreadCondTimeWait(MediaThreadCondHandle condHandle, MediaMutexHandle mutexHandle, uint32_t delayUs);
/*
 * destroy thread flag handle
 * param[in] condHandle   threadCondHandle by MediaThreadCondCreate
 */
void MediaThreadCondDestroy(MediaThreadCondHandle *condHandle);

void MediaThreadSetDetach(MediaThreadIdHandle thdId, bool detach);

#ifdef __cplusplus
};
#endif
#endif // OHOS_OSAL_THREAD_H
