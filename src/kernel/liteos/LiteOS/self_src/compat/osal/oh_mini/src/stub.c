/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapt File
 * Author: Huawei LiteOS Team
 * Create : 2025-03-01
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
 *---------------------------------------------------------------------------*/

#ifdef __RTOS_LITE__
#include "kal.h"
#include "osal_lite.h"
#include "los_exc_info.h"
#include "los_priqueue_pri.h"
#include "errno.h"
#include "unistd.h"
#include "stdio.h"
#include "string.h"
#if defined(LOSCFG_FS_FAT)
#include "sys/mount.h"
#include "ff.h"
#include "fatfs.h"
#include "errcode_fat.h"
#endif

int LOS_FsLock(void)
{
    return 0;
}

void LOS_FsUnlock(void)
{
    return;
}

void FS_LockInit(void)
{
    return;
}

#if defined(LOSCFG_FS_FAT)
int LOS_PartitionFormat(const char *partName, char *fsType, void *data)
{
    (void)fsType;
    (void)data;
    return format(partName, 0, O_RDWR);
}

FRESULT f_chdrive(const TCHAR* path)
{
    (VOID)path;
    return FR_OK;
}
#endif

/* printf.c stub */
WEAK VOID UartVprintf(const CHAR *fmt, va_list ap)
{
    (VOID)fmt;
    (VOID)ap;
}

#if !defined(LOSCFG_SAFETY_KERNEL)
VOID OsTaskWait(LOS_DL_LIST *list, UINT16 taskStatus, UINT32 timeout)
{
    LosTaskCB *runTask = NULL;
    LOS_DL_LIST *pendObj = NULL;

    runTask = OsCurrTaskGet();
    runTask->taskStatus &= ~OS_TASK_STATUS_READY;
    pendObj = &runTask->pendList;
    runTask->taskStatus |= taskStatus;
    LOS_ListTailInsert(list, pendObj);
    if (timeout != LOS_WAIT_FOREVER) {
        runTask->taskStatus |= OS_TASK_STATUS_PEND_TIME;
        OsTaskAdd2TimerList((LosTaskCB *)runTask, timeout);
    }
}

VOID OsTaskWake(LosTaskCB *resumedTask, UINT16 taskStatus)
{
    LOS_ListDelete(&resumedTask->pendList);

    resumedTask->taskStatus &= ~taskStatus;
    if (resumedTask->taskStatus & OS_TASK_STATUS_PEND_TIME) {
        OsTimerListDelete(resumedTask);
        resumedTask->taskStatus &= ~OS_TASK_STATUS_PEND_TIME;
    }

    if ((resumedTask->taskStatus & OS_TASK_STATUS_SUSPEND) == 0) {
        resumedTask->taskStatus |= OS_TASK_STATUS_READY;
        OsPriQueueEnqueue(&resumedTask->pendList, resumedTask->priority, PRI_QUEUE_TAIL);
    }
}
#endif

#ifdef LOSCFG_MEM_MUL_POOL
VOID *LOS_MemPoolGet(VOID *pool)
{
    if (pool == NULL) {
        return OsMemMulPoolHeadGet();
    }

    return ((LosMemPoolInfo *)pool)->nextPool;
}
#endif
#endif