/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Task Low Level Implementations Headfile
 * Author: Huawei LiteOS Team
 * Create: 2021-11-18
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

#ifndef _ARCH_TASK_H
#define _ARCH_TASK_H

#include "los_typedef.h"
#include "arch/cpu.h"
#include "arch/regs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOSCFG_STACK_POINT_ALIGN_SIZE 16

#define GEN_REGS_NUM 32
#define VEC_REGS_NUM 32
#define SHARED_REGS_NUM 12
#define L_REGS_NUM  4

/* The size of this structure must be smaller than or equal to the size specified by OS_TSK_STACK_ALIGN (16 bytes). */
typedef struct {
    UINT32 l[L_REGS_NUM];       // 8bit vector register
    UINT64 vs[SHARED_REGS_NUM]; // 64bit shared vector register
    UINT64 v[VEC_REGS_NUM];     // 64bit vector register

#ifdef LOSCFG_ARCH_FPU_ENABLE
    /* fpu registers */
    UINT32 fpctl;               // fpu control register
    UINT32 fpsr;                // fpu status register
    UINT32 fpieaddr;            // fpu interrupt event address register
    UINT32 reserved2;           // align 16 bytes
#endif

    /* hardware loop 0/1 */
    UINT32 lcount0;             // hardware loop0 count
    UINT32 lbeg0;               // hardware loop0 begin address
    UINT32 lend0;               // hardware loop0 end address
    UINT32 lcount1;             // hardware loop1 count
    UINT32 lbeg1;               // hardware loop1 begin address
    UINT32 lend1;               // hardware loop1 end address

    /* circular buffer */
    UINT32 cbegin;              // circular buffer begin
    UINT32 cend;                // circular buffer end

#ifdef LOSCFG_ARCH_SOUP_ENABLE
    /* stack overflow/underflow protection */
    UINT32 supUspb;             // superviser upper stack pointer boundary register
    UINT32 supLspb;             // superviser lower stack pointer boundary register
    UINT32 reserved[2];         // align 16 bytes
#endif

    /* pc, fbr, sr, sp */
    UINT32 pc;                  // program counter
    UINT32 fbr;                 // function backtracing register
    UINT32 sr;                  // status register
    UINT32 sp;                  // stack pointer. NOT required, just for align

    UINT32 a[GEN_REGS_NUM];     // general registers
} TaskContext;

extern VOID *g_runTask[LOSCFG_KERNEL_CORE_NUM];

STATIC INLINE VOID *ArchCurrTaskGet(VOID)
{
    UINTPTR currTask;
    __asm__ __volatile__ ("mov.a.ssr %0, gpsr\n" : "=r"(currTask));
    return (VOID *)currTask;
}

STATIC INLINE VOID ArchCurrTaskSet(VOID *val)
{
    __asm__ __volatile__ ("mov.ssr.a gpsr, %0\n" : : "r"((UINTPTR)val));
}

STATIC INLINE UINTPTR ArchGetTaskFp(const VOID *stackPointer)
{
    return ((TaskContext *)stackPointer)->fbr;
}

/*
 * Description : task stack initialization
 * Input       : taskId    -- task ID
 *               stackSize -- task stack size
 *               topStack  -- stack top of task (low address)
 * Return      : pointer to the task context
 */
extern VOID *ArchTaskStackInit(UINT32 taskId, UINT32 stackSize, VOID *topStack);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_TASK_H */
