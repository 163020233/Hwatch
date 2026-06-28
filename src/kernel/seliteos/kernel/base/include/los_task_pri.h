/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Task Module Private HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2022-06-15
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

#ifndef LOS_TASK_PRI_H
#define LOS_TASK_PRI_H

#include "los_task.h"
#include "arch/task.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OS_TASK_STATUS_UNUSED       (UINT16)0x0001

typedef struct LosTaskCBtag {
    VOID            *stackPointer;      /* Task stack pointer */
    UINT16          taskStatus;         /* Task status */
    UINT16          priority;           /* Task priority */
    UINT32          stackSize;          /* Task stack size */
    UINTPTR         topOfStack;         /* Task stack top */
    const CHAR      *taskName;          /* Task name */

#ifdef LOSCFG_BOX
    VOID            *userStackPointer;  /* User Task stack top */
    UINT32          userStackSize;      /* User Task stack size */
    UINTPTR         userTopOfStack;     /* User Task stack top */

    UINT16          driverBoxStatus;    /* Driver box status */
    UINT16          driverBoxNestCnt;   /* User Task Switch Driver Box Nest Count */

    VOID            *ta_header;         /* Ta Header Info */
#endif
} LosTaskCB;

STATIC_INLINE LosTaskCB *OsCurrTaskGet(VOID)
{
    return (LosTaskCB *)ArchCurrTaskGet();
}

extern UINT32 OsStartToRun(LosTaskCB *newTask);
extern const CHAR *OsCurTaskNameGet(VOID);
extern VOID LOS_TaskUsrExit(const VOID *ret);
void TaskReturn(unsigned long a0, unsigned a1);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOS_TASK_PRI_H */
