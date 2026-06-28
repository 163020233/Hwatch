/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Task Operations HeadFile
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

/**
 * @defgroup task
 * @ingroup kernel
 */

#ifndef _ARCH_TASK_H
#define _ARCH_TASK_H

#include "los_typedef.h"
#include "arch/cpu.h"
#ifdef LOSCFG_TRUSTZONE
#include "arch/secure/non_secure_call.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if ((defined(__CC_ARM) && defined(__TARGET_FPU_VFP))                         \
     || (defined(__CLANG_ARM) && defined(__VFP_FP__) && !defined(__SOFTFP__)) \
     || (defined(__ICCARM__) && defined(__ARMVFP__ ))                          \
     || (defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)))
#define FPU_USED   1
#else
#define FPU_USED   0
#endif

#define LOSCFG_STACK_POINT_ALIGN_SIZE 8

extern VOID *g_runTask;
extern VOID *g_oldTask;

STATIC INLINE VOID *ArchCurrTaskGet(VOID)
{
    return g_runTask;
}

STATIC INLINE VOID ArchCurrTaskSet(VOID *val)
{
    g_runTask = val;
}

typedef struct tagContext {
#ifdef LOSCFG_ARCH_ARM_V8M
    UINTPTR spLimit;
#ifdef LOSCFG_TRUSTZONE
    SecureContext *secureContextSp;
    UINT32 returnAddr;              /* LR, Bit[6] of EXC_RETURN indicates whether a Secure or Non-secure stack
                                       is used to restore stack frame on exception return */
#endif
#endif
    UINT32 R4;
    UINT32 R5;
    UINT32 R6;
    UINT32 R7;
    UINT32 R8;
    UINT32 R9;
    UINT32 R10;
    UINT32 R11;
    UINT32 PriMask;
#if FPU_USED
    UINT32 excReturn;
#endif
    UINT32 R0;
    UINT32 R1;
    UINT32 R2;
    UINT32 R3;
    UINT32 R12;
    UINT32 LR;
    UINT32 PC;
    UINT32 xPSR;
} TaskContext;

/*
 * Description : task stack initialization
 * Input       : taskId    -- task ID
 *               stackSize -- task stack size
 *               topStack  -- stack top of task (low address)
 * Return      : pointer to the task context
 */
extern VOID *ArchTaskStackInit(UINT32 taskId, UINT32 stackSize, VOID *topStack);

#ifdef LOSCFG_TRUSTZONE
extern SecureContext *g_secureContext;

VOID *ArchAllocSecureContext(UINT32 size, UINT32 taskId);
UINT32 ArchFreeSecureContext(VOID *topStack);
VOID ArchInitSecureContext(VOID);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_TASK_H */
