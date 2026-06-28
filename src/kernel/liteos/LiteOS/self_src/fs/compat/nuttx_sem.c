/* ---------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Adapt the NuttX semaphore interface
 * Author: Huawei LiteOS Team
 * Create: 2021-01-25
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
 * --------------------------------------------------------------------------- */

#include "sys/types.h"
#include "unistd.h"
#include "semaphore.h"
#include "nuttx/mutex.h"
#include "los_task_pri.h"
#include "los_sem_pri.h"
#include "vfs_extend.h"

#ifdef LOSCFG_BOX
#define SET_SEMID(sem, semHandle) (((sem)->semId) = GET_SEM_INDEX(semHandle))
#define GET_SEMID(sem)            ((sem)->semId)
#define SEM_IS_NULL(sem)          (((sem)->semId) == 0)
#else
#define SET_SEMID(sem, semHandle) (((sem)->sem) = GET_SEM(semHandle))
#define GET_SEMID(sem)            (((LosSemCB *)((sem)->sem))->semId)
#define SEM_IS_NULL(sem)          (((sem)->sem) == NULL)
#endif

int nxsem_init(sem_t *sem, int pshared, unsigned int value)
{
    UINT32 ret;
    UINT32 semHandle = 0;

    (void)pshared;
    if (sem == NULL || value > LOS_SEM_COUNT_MAX) {
        return -EINVAL;
    }
    ret = LOS_SemCreate((unsigned short)value, &semHandle);
    if (ret != LOS_OK) {
        return -EFAULT;
    }
    SET_SEMID(sem, semHandle);
    return (int)ret;
}

int nxsem_post(sem_t *sem)
{
    UINT32 ret;
    if ((sem == NULL) || SEM_IS_NULL(sem)) {
        return -EINVAL;
    }
    if (FS_OPERATE_IN_EXC) {
        return LOS_OK;
    }
    ret = LOS_SemPost(GET_SEMID(sem));
    if (ret != LOS_OK) {
        return -EFAULT;
    }
    return (int)ret;
}

int nxsem_wait(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || SEM_IS_NULL(sem)) {
        return -EINVAL;
    }
    if (FS_OPERATE_IN_EXC) {
        return LOS_OK;
    }
    ret = LOS_SemPend(GET_SEMID(sem), LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        return -EFAULT;
    }
    return (int)ret;
}

int nxsem_destroy(sem_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || SEM_IS_NULL(sem)) {
        return -EINVAL;
    }
    ret = LOS_SemDelete(GET_SEMID(sem));
    if (ret != LOS_OK) {
        return -EFAULT;
    }
    return (int)ret;
}

#ifndef LOSCFG_COMPAT_POSIX
pid_t getpid(void)
{
    return (pid_t)(((LosTaskCB *)(OsCurrTaskGet()))->taskId);
}
#endif

int nxrmutex_init(rmutex_t *rmutex)
{
    UINT32 ret;

    if (rmutex == NULL) {
        return -EINVAL;
    }

    ret = LOS_SemCreate(1, &rmutex->mux);
    if (ret != LOS_OK) {
        return -EFAULT;
    }

    rmutex->count  =  0;
    rmutex->holder = -1; /* no task hold */

    return LOS_OK;
}

int nxrmutex_lock(rmutex_t *rmutex)
{
    pid_t pid;
    UINT32 ret;

    if (rmutex == NULL) {
        return -EINVAL;
    }

    if (FS_OPERATE_IN_EXC) {
        return LOS_OK;
    }

    pid = getpid();
    if (pid == rmutex->holder) {
        rmutex->count++;
    } else {
        ret = LOS_SemPend(rmutex->mux, LOS_WAIT_FOREVER);
        if (ret != LOS_OK) {
            return -EFAULT;
        }
        rmutex->holder = pid;
        rmutex->count = 1;
    }

    return LOS_OK;
}

int nxrmutex_unlock(rmutex_t *rmutex)
{
    pid_t pid;
    UINT32 ret;

    if (rmutex == NULL) {
        return -EINVAL;
    }

    if (FS_OPERATE_IN_EXC) {
        return LOS_OK;
    }

    pid = getpid();
    if (pid != rmutex->holder) {
        return -EACCES;
    }

    if (rmutex->count > 1) {
        rmutex->count--;
    } else {
        rmutex->count = 0;
        rmutex->holder = -1;
        ret = LOS_SemPost(rmutex->mux);
        if (ret != LOS_OK) {
            rmutex->count = 1;
            rmutex->holder = pid;
            return -EFAULT;
        }
    }

    return LOS_OK;
}

int nxrmutex_destroy(rmutex_t *rmutex)
{
    UINT32 ret;

    if (rmutex == NULL) {
        return -EINVAL;
    }

    ret = LOS_SemDelete(rmutex->mux);
    if (ret != LOS_OK) {
        return -EFAULT;
    }

    return (int)ret;
}
