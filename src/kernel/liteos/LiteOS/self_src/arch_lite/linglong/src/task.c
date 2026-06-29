/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Task Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-11-30
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

#include "los_task_pri.h"
#include "arch/task.h"

VOID ArchTaskExit(VOID)
{
    (VOID)LOS_IntLock();
    while (1) { }
}

VOID *ArchTaskStackInit(UINT32 taskId, UINT32 stackSize, VOID *topStack)
{
    UINT32 index;
    TaskContext *taskContext = NULL;

    OsStackInit(topStack, stackSize);

    taskContext = (TaskContext *)(((UINTPTR)topStack + stackSize) - sizeof(TaskContext));

    /* initialize the task context */
    taskContext->pc = (UINTPTR)OsTaskEntry;
    /* set interrupt level and disable interrupts (SR.DI=1, SR.DNMI=0, SR.IPL=0) */
    taskContext->sr = (0x01 << SR_BTBEN_POS) | (0x01 << SR_DI_POS);
#ifndef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    taskContext->sr |= (0x01 << SR_DIMF_POS);
#endif
#ifdef LOSCFG_ARCH_SOUP_ENABLE
    taskContext->sr |= (0x01 << SR_SSOUP_POS);
#endif
    taskContext->fbr = (UINTPTR)(taskContext + 1);
    taskContext->sp = (UINTPTR)taskContext;
    taskContext->a[0] = taskId;

#ifdef LOSCFG_ARCH_FPU_ENABLE
    taskContext->fpctl = 0;
    taskContext->fpsr = 0;
    taskContext->fpieaddr = 0;
#endif

    taskContext->lcount0 = 0;
    taskContext->lbeg0 = 0;
    taskContext->lend0 = 0;
    taskContext->lcount1 = 0;
    taskContext->lbeg1 = 0;
    taskContext->lend1 = 0;
    taskContext->cbegin = 0;
    taskContext->cend = 0;

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    taskContext->supUspb = (UINTPTR)topStack + stackSize;
    taskContext->supLspb = (UINTPTR)topStack;
#endif

    /* 0xAAA0000000000001LL : reg initialed magic word  */
    for (index = 1; index < GEN_REGS_NUM; index++) {
        taskContext->a[index] = 0xAAA00001L + index;  /* A1 - A31 */
    }

    for (index = 0; index < VEC_REGS_NUM; index++) {
        taskContext->v[index] = 0xBBB0000000000001LL + index; /* V0 - V31 */
    }

    for (index = 0; index < SHARED_REGS_NUM; index++) {
        taskContext->vs[index] = 0xCCC0000000000001LL + index; /* V32 - V43 */
    }

    for (index = 0; index < L_REGS_NUM; index++) {
        taskContext->l[index] = (UINT8)(0xC1 + index); /* L0 - L3 */
    }

    return (VOID *)taskContext;
}
