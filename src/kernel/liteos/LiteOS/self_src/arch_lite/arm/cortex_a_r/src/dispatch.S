/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Implementation Of The OS Task Schedule
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

#include "arch/regs.h"
#include "arch/asm.h"
#include "arch/exception.h"

.extern   __irq_stack_top
.global   ArchStartToRun
.global   ArchTaskSchedule
.global   OsIrqHandler

.equ OS_PSR_INT_DIS,    0xC0U   @ Disable both FIQ and IRQ
.equ OS_PSR_FIQ_DIS,    0x40U   @ Disable FIQ
.equ OS_PSR_MODE_SVC,   0x13U

.fpu vfpv4

/* macros to align and unalign the stack on 8 byte boundary for ABI compliance */
.macro STACK_ALIGN, reg
    MOV     \reg, sp
    TST     SP, #4
    SUBEQ   SP, #4
    PUSH    { \reg }
.endm

.macro STACK_RESTORE, reg
    POP     { \reg }
    MOV     sp, \reg
.endm

/* macros to save and restore fpu regs */
.macro PUSH_FPU_REGS reg1
#ifdef LOSCFG_ARCH_FPU_ENABLE
    VMRS    \reg1, FPEXC
    PUSH    {\reg1}
    VMRS    \reg1, FPSCR
    PUSH    {\reg1}
#ifdef LOSCFG_ARCH_FPU_VFP_D32
    VPUSH   {D16-D31}
#endif
    VPUSH   {D0-D15}
#endif
.endm

.macro POP_FPU_REGS reg1
#ifdef LOSCFG_ARCH_FPU_ENABLE
    VPOP    {D0-D15}
#ifdef LOSCFG_ARCH_FPU_VFP_D32
    VPOP    {D16-D31}
#endif
    POP     {\reg1}
    VMSR    FPSCR, \reg1
    POP     {\reg1}
    VMSR    FPEXC, \reg1
#endif
.endm

/*
 * R0: new task
 */
FUNCTION(ArchStartToRun)
    MSR     CPSR_c, #(CPSR_INT_DISABLE | CPSR_SVC_MODE)

    /* save new task */
#if (__ARM_ARCH >= 7)
    /* R0 is new task, save it on tpidrprw */
    MCR     p15, 0, R0, c13, c0, 4
    ISB
#else
    ldr     R1, =g_runTask
    str     R0, [R1]
#endif

#ifdef LOSCFG_ARCH_FPU_ENABLE
/*
 * Test whether the fpu on the hardware is turned on,
 * if it is not turned on, the execution of the instruction
 * will cause an exception.
*/
    VPUSH   {S0}
    VPOP    {S0}
    VPUSH   {D0}
    VPOP    {D0}
#endif

    B       OsTaskContextLoad

/*
 * R0: new task
 * R1: run task
 */
FUNCTION(ArchTaskSchedule)
    MRS     R2, CPSR
    PUSH    {R2}
    PUSH    {LR}

    /* push r0-r12, lr */
    PUSH    {R0-R12, LR}

    /* save fpu registers */
    PUSH_FPU_REGS   R2

    /* store sp on running task */
    STR     SP, [R1]

OsTaskContextLoad:
#if (__ARM_ARCH >= 7)
    /* clear the flag of ldrex */
    CLREX
#endif

    /* switch to new task's sp */
    LDR     SP, [R0]

    /* restore fpu registers */
    POP_FPU_REGS    R2

    /* restore r0-r12, lr */
    POP     {R0-R12, LR}

    POP     {R1}
    POP     {R2}
    MSR     CPSR, R2
    BX      R1

OsIrqHandler:
    SUB     LR, LR, #4      /* task PC */

#if (__ARM_ARCH >= 7)
#ifdef LOSCFG_KERNEL_PERF
    PUSH    {R0-R3, R12, LR}
    MOV     R0, LR
    MOV     R1, FP
    BL      OsPerfSetIrqRegs
    POP     {R0-R3, R12, LR}
#endif

    /* save spsr and lr(svc's pc) onto the svc stack */
    SRSDB   #0x13!

    /* disable irq, switch to svc mode */
    CPSID   i, #0x13

    /* push caller saved regs as trashed regs */
    PUSH    {R0-R3, R12, LR}

#else
    /* The SRSDB command is not supported, so need to switch the stack and save the R0-R2. */
    LDR     SP,  =__irq_stack_top
    STMFD   SP!, {R0-R2}
    MOV     R0,  SP
    MRS     R1,  SPSR            /* read SPSR*/
    MOV     R2,  LR

    MSR    CPSR_c, #(OS_PSR_INT_DIS | OS_PSR_FIQ_DIS | OS_PSR_MODE_SVC)
    STMFD   SP!, {R2}           /* Push task  PC */
    STMFD   SP!, {LR}           /* Push task  LR */
    STMFD   SP!, {R3, R5-R7, R12}
    LDMFD   R0!, {R5-R7}        /* Move task R2-R0 from exception stack to tasks stack */
    STMFD   SP!, {R5-R7}
    STMFD   SP!, {R1}           /* Push task s CPSR (i.e. exception SPSR). */

#endif

    /* 8 bytes stack align */
    STACK_ALIGN     R0

    /*
     * save fpu regs in case in case those been
     * altered in interrupt handlers.
     */
    PUSH_FPU_REGS   R0
#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    /* check nesting count, if 0, switch sp to irq stack */
    BL      OsIrqNestingCntGet
    CMP     R0, #0
    BNE     OsIrqIntEntry
#endif

    PUSH    {R4}
    MOV     R4, SP
#ifdef LOSCFG_LIB_CONFIGURABLE
    EXC_SP_SET g_svcStackTop, OS_EXC_SVC_STACK_SIZE, R1, R2
#else
    EXC_SP_SET __svc_stack_top, OS_EXC_SVC_STACK_SIZE, R1, R2
#endif

OsIrqIntEntry:
#endif

    BLX     OsIntEntry

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    BL      OsIrqNestingCntGet
    CMP     R0, #0
    BNE     OsIrqExit
#endif

#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    MOV     SP, R4
    POP     {R4}
#endif

    /* process pending signals and resched */
    BL      OsSchedProcSchedFlag

OsIrqExit:
    /* restore fpu regs */
    POP_FPU_REGS    R0

    STACK_RESTORE   R0

OsIrqContextRestore:
#if (__ARM_ARCH >= 7)
    POP     {R0-R3, R12, LR}
    RFEIA   SP!
#else
    LDMFD   SP!, {R0}
    MSR     SPSR_cxsf, R0                          /* restore SPSR */
    LDMFD   SP!, {R0-R3, R5-R7, R12, LR, PC}^      /* restore task and CPSR */
#endif
