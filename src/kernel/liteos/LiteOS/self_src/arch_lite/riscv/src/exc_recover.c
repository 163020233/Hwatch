/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 * Description : LiteOS exception recvoer.
 * Author : Huawei LiteOS Team
 * Create : 2026-04-28
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

#include "los_task_pri.h"
#include "los_sem_pri.h"
#include "los_mux_pri.h"
#include "los_rwsem_pri.h"
#include "arch/exc_recover.h"

#define MAX_EXC_RECOVER_NUM 4
UINT32 g_exc_recover_swap_sp = 0;
typedef struct {
    CHAR *taskName;
    EXC_RECOVER_FUNC exchook;
    VOID *userdata;
} ExcRecoverInfo;

STATIC ExcRecoverInfo g_exc_recover_info[MAX_EXC_RECOVER_NUM] = {0};
STATIC BOOL g_firstExcStatus = TRUE;

STATIC INT32 FindFreeSlot(VOID)
{
    for (INT32 i = 0; i < MAX_EXC_RECOVER_NUM; i++) {
        if (g_exc_recover_info[i].taskName == NULL && g_exc_recover_info[i].exchook == NULL) {
            return i;
        }
    }
    return -1;
}

STATIC INT32 FindSlotByTaskName(const CHAR *taskName)
{
    if (taskName == NULL) {
        return -1;
    }

    for (INT32 i = 0; i < MAX_EXC_RECOVER_NUM; i++) {
        if (g_exc_recover_info[i].taskName != NULL && strcmp(g_exc_recover_info[i].taskName, taskName) == 0) {
            return i;
        }
    }
    return -1;
}

STATIC VOID FreeSlotResources(INT32 slot)
{
    if (slot < 0 || slot >= MAX_EXC_RECOVER_NUM) {
        return;
    }

    if (g_exc_recover_info[slot].taskName != NULL) {
        (void)LOS_MemFree(OS_SYS_MEM_ADDR, g_exc_recover_info[slot].taskName);
        g_exc_recover_info[slot].taskName = NULL;
    }

    g_exc_recover_info[slot].exchook = NULL;
    g_exc_recover_info[slot].userdata = NULL;
}

UINT32 LOS_RecoverSetExcHook(EXC_RECOVER_FUNC excHook, CHAR *taskName, VOID *user)
{
    UINT32 ret = LOS_OK;
    UINT32 intSave;
    CHAR *newTaskName = NULL;
    UINT32 nameLen;
    INT32 slot;

    if ((excHook == NULL) || (taskName == NULL)) {
        return LOS_NOK;
    }

    SCHEDULER_LOCK(intSave);
    slot = FindSlotByTaskName(taskName);
    if (slot >= 0) {
        FreeSlotResources(slot);
    } else {
        slot = FindFreeSlot();
        if (slot < 0) {
            ret = LOS_NOK;
            goto EXIT;
        }
    }

    nameLen = strlen(taskName) + 1;
    newTaskName = (CHAR *)LOS_MemAlloc(OS_SYS_MEM_ADDR, nameLen);
    if (newTaskName == NULL) {
        ret = LOS_NOK;
        goto EXIT;
    }

    if (memcpy_s(newTaskName, nameLen, taskName, nameLen) != 0) {
        (void)LOS_MemFree(OS_SYS_MEM_ADDR, newTaskName);
        newTaskName = NULL;
        ret = LOS_NOK;
        goto EXIT;
    }

    g_exc_recover_info[slot].taskName = newTaskName;
    g_exc_recover_info[slot].exchook = excHook;
    g_exc_recover_info[slot].userdata = user;
    newTaskName = NULL;

EXIT:
    SCHEDULER_UNLOCK(intSave);
    if (newTaskName != NULL) {
        (void)LOS_MemFree(OS_SYS_MEM_ADDR, newTaskName);
    }
    return ret;
}

UINT32 LOS_RecoverUnsetExcHook(const CHAR *taskName)
{
    UINT32 ret = LOS_NOK;
    UINT32 intSave;
    INT32 slot;

    if (taskName == NULL) {
        return LOS_NOK;
    }

    SCHEDULER_LOCK(intSave);
    slot = FindSlotByTaskName(taskName);
    if (slot >= 0) {
        FreeSlotResources(slot);
        ret = LOS_OK;
    }

    SCHEDULER_UNLOCK(intSave);
    return ret;
}

VOID ExcptionTaskSwitch(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();
    if (runTask == NULL || runTask->taskName == NULL) {
        return;
    }

    INT32 slot = FindSlotByTaskName(runTask->taskName);
    if (slot < 0) {
        return;
    }

    if (OS_INT_INACTIVE && g_firstExcStatus) {
        g_firstExcStatus = FALSE;

        if (g_exc_recover_info[slot].exchook != NULL) {
            g_exc_recover_info[slot].exchook(g_exc_recover_info[slot].userdata);
        }

        UINT32 intSave;
        SCHEDULER_LOCK(intSave);

#ifdef LOSCFG_DEBUG_TASK_RESOURCE
        OsDbgTaskResource(runTask->taskId);
#endif

        g_firstExcStatus = TRUE;
        SCHEDULER_UNLOCK(intSave);
        LOS_TaskDelete(runTask->taskId);
    }
}
