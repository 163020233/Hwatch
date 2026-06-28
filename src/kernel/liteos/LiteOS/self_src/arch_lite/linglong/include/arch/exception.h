/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Exception HeadFile
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

#ifndef _ARCH_EXCEPTION_H
#define _ARCH_EXCEPTION_H

#ifndef __ASSEMBLER__
#include "los_typedef.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_KERNEL_SMP
#define CORE_NUM                LOSCFG_KERNEL_SMP_CORE_NUM
#else
#define CORE_NUM                1
#endif

/* Initial bit64 stack value. */
#define OS_STACK_INIT           0xCACACACA
/* Bit64 stack top magic number. */
#define OS_STACK_MAGIC_WORD     0xCCCCCCCC

#define OS_EXC_START_STACK_SIZE 0x1000
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
#define OS_EXC_IRQ_STACK_SIZE   0x6000
#else /* !LOSCFG_ARCH_INTERRUPT_PREEMPTION */
#define OS_EXC_IRQ_STACK_SIZE   0x1000
#endif /* LOSCFG_ARCH_INTERRUPT_PREEMPTION */

#define OS_EXC_ILLEGAL              0
#define OS_EXC_PROG_FETCH_ERR       1
#define OS_EXC_DATA_LOAD_ERR        2
#define OS_EXC_DATA_STORE_ERR       3
#define OS_EXC_RESERVED4            4
#define OS_EXC_INT_DIV_ZERO         5
#define OS_EXC_DEBUG                6
#define OS_EXC_SYSCALL              7
#define OS_EXC_MAX                  7

#define EXC_GEN_REG_NUM         32
#define OS_MAX_BACKTRACE        15

#define OS_EXC_STACK_OFFSET     32

#ifndef __ASSEMBLER__
typedef struct ExcContext {
    UINT32 excType; // exception type
    UINT32 flags;   // could be isr0, isr1, isr2
    UINT32 emaddr;  // contains a program/data address that caused an error
    UINT32 sp;      // stack pointer (to exception stack)

    UINT32 sr;      // status register
    UINT32 fbr;     // function backtracing register
    UINT32 reserved[2]; // for 16-bytes align

    UINT32 a[EXC_GEN_REG_NUM]; // general registers
} ExcContext;

/* automatic exception stack before enter exception */
typedef struct ExcStack {
    UINT32 pc;      // program counter
    UINT32 sr;      // status register
    UINT32 isr;     // interrupt/exception status register, could be isr0, isr1 or isr2
    UINT32 emaddr;  // contains a program/data address that caused an error
} ExcStack;

#define LOS_ExcRegHook ArchSetExcHook
typedef VOID (*NMI_PROC_FUNC)(const VOID *excBufAddr);
typedef VOID (*EXC_PROC_FUNC)(UINT32 type, const ExcContext *exc);
UINT32 ArchSetExcHook(EXC_PROC_FUNC excHook);
UINT32 ArchSetNMIHook(NMI_PROC_FUNC nmiHook);
EXC_PROC_FUNC ArchGetExcHook(VOID);
NMI_PROC_FUNC ArchGetNMIHook(VOID);
extern BOOL ArchExcActive(VOID);

STATIC INLINE VOID ArchHaltCpu(VOID)
{
    __asm__ __volatile__ ("syscall");
}

VOID ArchBackTraceWithSp(const VOID *stackPointer);
VOID ArchBackTrace(VOID);
UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth);
UINT32 ArchExcInit(VOID);
UINTPTR ArchGetEPC(UINT32 coreId);

/* Stack pointers for different modes. */
extern UINTPTR __stack;
extern UINTPTR __stack_top;

#endif /* __ASSEMBLER__ */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_EXCEPTION_H */
