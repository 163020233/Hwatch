/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Cortex-M Task Implementation
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

#include "los_task_pri.h"
#include "arch/task.h"

VOID *g_runTask = NULL;
VOID *g_oldTask = NULL;

#ifdef LOSCFG_GDB
STATIC VOID OsTaskEntrySetupLoopFrame(UINT32) __attribute__((noinline, naked));
VOID OsTaskEntrySetupLoopFrame(UINT32 arg0)
{
    asm volatile("\tsub fp, sp, #0x4\n"
                 "\tpush {fp, lr}\n"
                 "\tadd fp, sp, #0x4\n"
                 "\tpush {fp, lr}\n"

                 "\tadd fp, sp, #0x4\n"
                 "\tbl OsTaskEntry\n"

                 "\tpop {fp, lr}\n"
                 "\tpop {fp, pc}\n");
}
#endif

LITE_OS_SEC_TEXT_MINOR VOID ArchTaskExit(VOID)
{
    (VOID)LOS_IntLock();
    while (1) { }
}

LITE_OS_SEC_TEXT_INIT VOID *ArchTaskStackInit(UINT32 taskId, UINT32 stackSize, VOID *topStack)
{
    TaskContext *taskContext = NULL;

    OsStackInit(topStack, stackSize);
    taskContext = (TaskContext *)(((UINTPTR)topStack + stackSize) - sizeof(TaskContext));

#ifdef LOSCFG_ARCH_FPU_ENABLE
    taskContext->excReturn = 0xFFFFFFFD;
#endif

#ifdef LOSCFG_ARCH_ARM_V8M
    taskContext->spLimit         = (UINTPTR)topStack;
#ifdef LOSCFG_TRUSTZONE
#define EXC_RETURN_TO_NS    0xFFFFFFBC
    taskContext->secureContextSp = OS_TCB_FROM_TID(taskId)->secureContextSP;
    taskContext->returnAddr      = EXC_RETURN_TO_NS;
#endif
#endif

    taskContext->R4  = 0x04040404L;
    taskContext->R5  = 0x05050505L;
    taskContext->R6  = 0x06060606L;
    taskContext->R7  = 0x07070707L;
    taskContext->R8  = 0x08080808L;
    taskContext->R9  = 0x09090909L;
    taskContext->R10 = 0x10101010L;
    taskContext->R11 = 0x11111111L;
    taskContext->PriMask = 0;
    taskContext->R0  = taskId;
    taskContext->R1  = 0x01010101L;
    taskContext->R2  = 0x02020202L;
    taskContext->R3  = 0x03030303L;
    taskContext->R12 = 0x12121212L;
    taskContext->LR  = (UINTPTR)ArchTaskExit;
    taskContext->PC  = (UINTPTR)OsTaskEntry;
    taskContext->xPSR = 0x01000000L;

    return (VOID *)taskContext;
}

#ifdef LOSCFG_TRUSTZONE
SecureContext *g_secureContext = NULL;

#define SPSEL_OFFSET            1
#define OS_USE_PSP              (1 << SPSEL_OFFSET)
#define SEC_SP_OFFSET_IN_STACK  4
VOID *ArchAllocSecureContext(UINT32 size, UINT32 taskId)
{
    VOID *allocPtr = NULL;
    BOOL selfAlloc = (taskId == OsCurrTaskGet()->taskId) ? TRUE : FALSE;
    LosTaskCB *taskCB = OS_TCB_FROM_TID(taskId);

    if (size == 0) {
        return allocPtr;
    }

    UINT32 usePsp = ArchGetControl() & OS_USE_PSP;
    UINT32 priMaskVal = ArchGetPrimask();    /* priMaskVal = 1, disable interrupts */
    if ((usePsp == OS_USE_PSP) && (priMaskVal == 1)) {
        PRINT_ERR("The security stack cannot be alloc when the irq was locked!\n");
        return allocPtr;
    } else if (usePsp == 0) {
        /* The MSP is used. */
        allocPtr = (VOID *)ArchSecureAllocContext(size);
        if ((allocPtr != NULL) && (selfAlloc == TRUE)) {
            ArchSecureLoadContext(allocPtr);
            g_secureContext = allocPtr;
        }
    } else {
        /* The SVC interrupt is triggered and services are switched to the MSP. */
        allocPtr = (VOID *)SvcAllocSecContextAsm(size, selfAlloc);
    }

    if ((allocPtr != NULL) && (selfAlloc == FALSE)) {
        *(UINT32 *)((UINT32)taskCB->stackPointer + SEC_SP_OFFSET_IN_STACK) = (UINT32)allocPtr;
    }
    return allocPtr;
}

UINT32 ArchFreeSecureContext(VOID *topStack)
{
    UINT32 ret = LOS_NOK;

    if (topStack == NULL) {
        return LOS_NOK;
    }

    UINT32 usePsp = ArchGetControl() & OS_USE_PSP;
    UINT32 priMaskVal = ArchGetPrimask();    /* priMaskVal = 1, disable interrupts */
    if ((usePsp == OS_USE_PSP) && (priMaskVal == 1)) {
        PRINT_ERR("The security stack cannot be free when the irq was locked!\n");
    } else if (usePsp == 0) {
        ret = ArchSecureFreeContext((SecureContext *)topStack);
    } else {
        ret = SvcFreeSecContextAsm(topStack);
    }
    return ret;
}

VOID ArchInitSecureContext(VOID)
{
    UINT32 usePsp = ArchGetControl() & OS_USE_PSP;
    UINT32 priMaskVal = ArchGetPrimask();    /* priMaskVal = 1, disable interrupts */
    if ((usePsp != OS_USE_PSP) || (priMaskVal == 1)) {
        PRINT_ERR("Init security stack failed!\n");
    } else {
        SvcInitSecContextAsm();
    }
    return;
}

#endif
