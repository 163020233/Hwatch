/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Osal HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2024-04-14
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

#ifndef _OSAL_LITE_H
#define _OSAL_LITE_H
#include "los_task_pri.h"
#include "los_swtmr_pri.h"
#include "los_sortlink_pri.h"
#include "los_queue_pri.h"
#include "los_mux_pri.h"
#ifdef LOSCFG_KERNEL_CPUP
#include "los_cpup_pri.h"
#endif
#include "los_memory_internal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifndef UNUSED
#define UNUSED(a) ((void)(1 || (a)))
#endif

typedef struct {
    LosTaskCB   *runTask;
    LosTaskCB   *newTask;
} LosTask;
extern LosTask              g_losTask;

STATIC INLINE VOID OsSetMutexCreateInfo(LosMuxCB *mux, UINTPTR val)
{
    (void)mux;
    (void)val;
}

extern UINT8 g_excMsgArray[];
extern void FS_LockInit(void);
extern VOID ShbTskSwitchHook(UINT32 oldTaskId, UINT32 newTaskId);
extern UINT32 LOS_NextTaskIDGet(VOID);

extern VOID OsTaskWait(LOS_DL_LIST *list, UINT16 taskStatus, UINT32 timeout);
extern VOID OsTaskWake(LosTaskCB *resumedTask, UINT16 taskStatus);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _OSAL_LITE_H */