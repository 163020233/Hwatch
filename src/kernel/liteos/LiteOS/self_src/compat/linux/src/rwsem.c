/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Semaphore Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-02-07
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

#include "linux/rwsem.h"
#include "los_rwsem.h"
#include "los_base.h"

VOID InitRwsem(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    if (rwsem == NULL) {
        PRINT_ERR("init_rwsem<error> rwsem is NULL\n");
        return;
    }

    ret = LOS_RwsemCreate(&rwsem->rwsemHandle);
    if (ret != LOS_OK) {
        rwsem->rwsemHandle = RWSEM_UNINIT_VALUE;
        PRINT_ERR("init_rwsem<error> : 0x%x\n", ret);
    }
}

STATIC INLINE UINT32 RwsemInitCheck(struct rw_semaphore *rwsem)
{
    UINT32 ret = LOS_OK;

    if (rwsem == NULL) {
        return LOS_NOK;
    } else if (rwsem->rwsemHandle == RWSEM_UNINIT_VALUE) {
        ret = LOS_RwsemCreate(&rwsem->rwsemHandle);
        if (ret != LOS_OK) {
            PRINT_ERR("RwsemInitCheck<error> : 0x%x\n", ret);
        }
    }
    return ret;
}

void down_read(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    ret = RwsemInitCheck(rwsem);
    if (ret != LOS_OK) {
        return;
    }

    ret = LOS_RwsemPendRead(rwsem->rwsemHandle, LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        PRINT_ERR("down_read<error> : 0x%x\n", ret);
    }
}

int down_read_trylock(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    ret = RwsemInitCheck(rwsem);
    if (ret != LOS_OK) {
        return 0;
    }

    ret = LOS_RwsemPendRead(rwsem->rwsemHandle, 0);
    return (ret == LOS_OK) ? 1 : 0;
}

void up_read(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    if (rwsem == NULL) {
        return;
    }

    ret = LOS_RwsemPostRead(rwsem->rwsemHandle);
    if (ret != LOS_OK) {
        PRINT_ERR("up_read<error> : 0x%x\n", ret);
    }
}

void down_write(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    ret = RwsemInitCheck(rwsem);
    if (ret != LOS_OK) {
        return;
    }

    ret = LOS_RwsemPendWrite(rwsem->rwsemHandle, LOS_WAIT_FOREVER);
    if (ret != LOS_OK) {
        PRINT_ERR("down_write<error> : 0x%x\n", ret);
    }
}

int down_write_trylock(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    ret = RwsemInitCheck(rwsem);
    if (ret != LOS_OK) {
        return 0;
    }

    ret = LOS_RwsemPendWrite(rwsem->rwsemHandle, 0);

    return (ret == LOS_OK) ? 1 : 0;
}

void up_write(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    if (rwsem == NULL) {
        return;
    }

    ret = LOS_RwsemPostWrite(rwsem->rwsemHandle);
    if (ret != LOS_OK) {
        PRINT_ERR("up_write<error> : 0x%x\n", ret);
    }
}

void downgrade_write(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    if (rwsem == NULL) {
        return;
    }

    ret = LOS_RwsemDowngradeWrite(rwsem->rwsemHandle);
    if (ret != LOS_OK) {
        PRINT_ERR("downgrade_write<error> : 0x%x\n", ret);
    }
}

int rwsem_destory(struct rw_semaphore *rwsem)
{
    UINT32 ret;

    if (rwsem == NULL) {
        return 0;
    }

    ret = LOS_RwsemDelete(rwsem->rwsemHandle);
    if (ret != LOS_OK) {
        PRINT_ERR("rwsem_destory<error> : 0x%x\n", ret);
    }

    return (ret == LOS_OK) ? 1 : 0;
}
