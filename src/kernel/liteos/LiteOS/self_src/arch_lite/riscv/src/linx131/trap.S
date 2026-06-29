/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS trap module implementation.
 * Author: Huawei LiteOS Team
 * Create: 2022-12-20
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

#include "arch/stack_ops.h"
#include "asm/interrupt_config.h"

.global  TrapVector

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    .section .bss
    .align   4
    .global  g_mcause
g_mcause:
    .dword   0
#endif

.macro POP_IRQ_REG
    lw      t0, 1 * REGBYTES(sp)  /* retrieve the address at which exception happened */
    csrw    MEPC, t0              /* load mepc from previously reserved (4 * REGBYTES) space */
    lw      t0, 0 * REGBYTES(sp)  /* enable global interrupts */
    csrw    MSTATUS, t0           /* load mstatus from previously reserved (4 * REGBYTES) space */
    POP_CALLER_REG
.endm

/* riscv interrupt & exception entry */
.section .itcm.text,"ax",@progbits
.align 4
TrapVector:
    PUSH_CALLER_REG /* sp must be 16 bytes aligned, now size is 32 REGBYTES (64 REGBYTES when FPU on) */

    csrr    a0, mcause
    li      a1, MCAUSE_MASK_INT_BIT
    li      a2, MCAUSE_MASK_INT_NUM
    and     a1, a0, a1
    and     a0, a0, a2            /* interrupt num */

    beqz    a1, TrapHandle        /* mstatus and mepc will be saved in TrapHandle */
    li      a2, NUM_HAL_INTERRUPT_NMI
    beq     a0, a2, TrapHandle

    csrr    t0, mstatus
    sw      t0, 0 * REGBYTES(sp)  /* store mstatus to previously reserved (4*REGBYTES) space by PUSH_CALLER_REG */
    csrr    t0, mepc
    sw      t0, 1 * REGBYTES(sp)  /* store mepc to previously reserved (4*REGBYTES) space by PUSH_CALLER_REG */

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    csrr    a1, 0xBFE
    beqz    a1, 1f /* first interruprt, go to 1f, Switch the interrupt stack */
    j       2f
1:
    csrw    mscratch, sp
#ifdef LOSCFG_LIB_CONFIGURABLE
    la      sp, g_irqStackTop
    lw      sp, 0(sp)
#else
    la      sp, __irq_stack_top
#endif
2:
    addi    sp, sp, -4 * REGBYTES /* use 2 REGBYTES but sp must be 16 aligned when compile option -mpush-pop is on */
    SREG    a1, 0 * REGBYTES(sp)  /* save 0xBFE */
    csrr    a1, 0x7ED
    csrw    0xBFE, a1

    la      a1, g_mcause
    lw      t0, 0(a1)
    SREG    t0, 1 * REGBYTES(sp)
    csrr    t1, mcause
    sw      t1, 0(a1)

    li      a1, NUM_HAL_INTERRUPT_TIMER
    beq     a0, a1, 3f /* If the interrupt is a tick interrupt, other interrupts are not allowed during OsIntEntry. */

    li      t0, LOS_MSTATUS_MPIE
    csrrc   zero, mstatus, t0

    la      a1, 3f
    csrw    mepc, a1
    mret /* mret is set MIE to MPIE, will go to mepc */
3:
    call    OsIntEntry
#ifdef LOSCFG_HWI_STACK_CHECK
    la      t0, g_hwiStackCanary
    lw      t0, 0(t0)
    lw      t1, 0(t0)

    la      t2, g_stackCanaryValue
    lw      t2, 0(t2)

    bne     t1, t2, stack_overflow
#endif /* LOSCFG_HWI_STACK_CHECK */
    la      a0, g_mcause
    LREG    t0, 1 * REGBYTES(sp)
    csrw    mcause, t0
    sw      t0,  0(a0)

    LREG    a0, 0 * REGBYTES(sp)  /* load 0xBFE */
    csrw    0xBFE, a0
    addi    sp, sp, 4 * REGBYTES
    beqz    a0, 4f

    POP_IRQ_REG
    mret /* mret is return from interrupt, will go to mepc */
#else
    csrw    mscratch, sp
#ifdef LOSCFG_LIB_CONFIGURABLE
    la      sp, g_irqStackTop
    lw      sp, 0(sp)
#else
    la      sp, __irq_stack_top
#endif
    call    OsIntEntry
#ifdef LOSCFG_HWI_STACK_CHECK
    la      t0, g_hwiStackCanary
    lw      t0, 0(t0)
    lw      t1, 0(t0)

    la      t2, g_stackCanaryValue
    lw      t2, 0(t2)

    bne     t1, t2, stack_overflow
#endif /* LOSCFG_HWI_STACK_CHECK */
#endif
4:
    csrr    sp, mscratch
    call    OsSchedProcSchedFlag

    POP_IRQ_REG
    mret

#ifdef LOSCFG_HWI_STACK_CHECK
stack_overflow:
    call    OsHwiStackOverflowHandler
    j       stack_overflow
#endif /* LOSCFG_HWI_STACK_CHECK */