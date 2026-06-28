/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: LiteOS Semaphore Implementation
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#include "asm/semaphore.h"
#include "los_sem.h"

#define SEM_COUNT_MAX   1

int _sema_init(losMutexDef_t *sem, unsigned int value)
{
    UINT32 ret;

    if (sem == NULL) {
        return -1;
    } else if (value > SEM_COUNT_MAX) {
        PRINT_ERR("The semaphore count value can only be 0 or 1!\n");
        sem->sem = UNINIT_VALUE;
        return -1;
    }

    ret = LOS_SemCreate((UINT16)value, &(sem->sem));
    if (ret != LOS_OK) {
        sem->sem = UNINIT_VALUE;
        return -1;
    }

    return 0;
}

int _sema_lock(losMutexDef_t *sem)
{
    UINT32 ret;

    if (sem == NULL) {
        return -1;
    }

    if (sem->sem == UNINIT_VALUE) {
        if (_sema_init(sem, (unsigned int)sem->count) != 0) {
            return -1;
        }
    }

    ret = LOS_SemPend(sem->sem, LOS_WAIT_FOREVER);
    if (ret == LOS_OK) {
        return 0;
    }

    return -1;
}

int _sema_trylock(losMutexDef_t *sem)
{
    UINT32 ret;

    if (sem == NULL) {
        return -1;
    }

    if (sem->sem == UNINIT_VALUE) {
        if (_sema_init(sem, (unsigned int)sem->count) != 0) {
            return -1;
        }
    }

    ret = LOS_SemPend(sem->sem, LOS_NO_WAIT);
    if (ret == LOS_OK) {
        return 0;
    }

    return -1;
}

int _sema_unlock(losMutexDef_t *sem)
{
    UINT32 ret;

    if (sem == NULL) {
        return -1;
    }

    if (sem->sem == UNINIT_VALUE) {
        if (_sema_init(sem, (unsigned int)sem->count) != 0) {
            return -1;
        }
    }

    ret = LOS_SemPost(sem->sem);
    if (ret != LOS_OK) {
        return -1;
    }

    return 0;
}

int _sema_destory(losMutexDef_t *sem)
{
    UINT32 ret;

    if ((sem == NULL) || (sem->sem == UNINIT_VALUE)) {
        return -1;
    }

    ret = LOS_SemDelete(sem->sem);
    if (ret != LOS_OK) {
        return -1;
    }

    return 0;
}

int down_trylock(struct semaphore *sem)
{
    int ret = _sema_trylock((losMutexDef_t*)sem);
    return (ret == 0) ? 0 : 1;
}
