/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS exception module implementation.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
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

#include "arch/cpu.h"
#include "arch/stack_ops.h"
#include "asm/interrupt_config.h"

.extern   OsExcHandleEntry
.extern   OsNMIHandlerEntry
.extern   LOS_TaskLock
.extern   LOS_TaskUnlockNoSched

.global TrapHandle
.global NmiEntry
.global g_excInfo
.section .itcm.text, "ax"
.align 4

#ifndef LOSCFG_EXC_STACK_SIZE
#define LOSCFG_EXC_STACK_SIZE 0
#endif
#ifndef LOSCFG_NMI_STACK_SIZE
#define LOSCFG_NMI_STACK_SIZE 0
#endif
#if (LOSCFG_EXC_STACK_SIZE == 0) && (LOSCFG_NMI_STACK_SIZE == 0)
.equ    EXC_STACK_TOP,               __irq_stack_top
.equ    EXC_STACK_BOTTOM,            (__irq_stack_top + LOSCFG_IRQ_STACK_SIZE)
.equ    NMI_STACK_TOP,               __irq_stack_top
.equ    NMI_STACK_BOTTOM,            (__irq_stack_top + LOSCFG_IRQ_STACK_SIZE)
#elif (LOSCFG_EXC_STACK_SIZE == 0)
.equ    EXC_STACK_TOP,               __nmi_stack_top
.equ    EXC_STACK_BOTTOM,            (__nmi_stack_top + LOSCFG_NMI_STACK_SIZE)
.equ    NMI_STACK_TOP,               __nmi_stack_top
.equ    NMI_STACK_BOTTOM,            (__nmi_stack_top + LOSCFG_NMI_STACK_SIZE)
#elif (LOSCFG_NMI_STACK_SIZE == 0)
.equ    EXC_STACK_TOP,               __exc_stack_top
.equ    EXC_STACK_BOTTOM,            (__exc_stack_top + LOSCFG_EXC_STACK_SIZE)
.equ    NMI_STACK_TOP,               __exc_stack_top
.equ    NMI_STACK_BOTTOM,            (__exc_stack_top + LOSCFG_EXC_STACK_SIZE)
#else
.equ    EXC_STACK_TOP,               __exc_stack_top
.equ    EXC_STACK_BOTTOM,            (__exc_stack_top + LOSCFG_EXC_STACK_SIZE)
.equ    NMI_STACK_TOP,               __nmi_stack_top
.equ    NMI_STACK_BOTTOM,            (__nmi_stack_top + LOSCFG_NMI_STACK_SIZE)
#endif
 
.macro REFRESH_SPFP STACK_TOP, STACK_BOTTOM
    la    t2, \STACK_TOP
    bltu  t2, sp, .L\@
    la    t2, \STACK_BOTTOM
    bgeu  t2, sp, .L\@
    j     .E\@
.L\@:
    la    sp, \STACK_TOP
.E\@:
.endm

TrapHandle:
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
#ifdef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    li      tp, NUM_HAL_INTERRUPT_NMI
    beq     gp, tp, SkipIntCaller
    /* hardware stacking doesn't work in exc handler, hence software stacking is needed */
    PUSH_INTEGER_CALLER_REG 0
SkipIntCaller:
    PUSH_OTHER_CALLER_REG
    /* c-source OsIntEntry may use lwgp, so recover gp here. */
    INIT_GP_TO_DEFAULT
#endif
    PUSH_CALLEE_REG

#ifdef LOSCFG_EXC_RECOVER_ENABLE
    la      a0, g_exc_recover_swap_sp
    sw      sp, 0(a0)
    la      sp, __exc_stack_top
    addi    sp, sp, -4
    sw      a1, 0(sp)
    call    ExcptionTaskSwitch
    lw      a1, 0(sp)
    la      a0, g_exc_recover_swap_sp
    lw      sp, 0(a0)
#endif

#ifdef LOSCFG_TRIGGER_ENABLE
    la      a0, g_trigger_swap_sp
    sw      sp, 0(a0)
    la      sp, __exc_stack_top
__check_trigger_exception:
    csrr    a0, mcause
    andi    a0, a0, 0x3
    li      t0, 0x3
    bne     t0, a0, __normal_exception
__trigger_exception:
    csrr    t0, mepc
    lw      t2, 0(t0)
    la      a0, g_trigger_caller
    sw      ra, 0(a0)
__calc_next_pc:
    li      t4, 0x3
    and     t5, t4, t2
    beq     t5, t4, __stand_instruction
    addi    t0, t0, 2
    j       __save_next_pc
__stand_instruction:
    addi    t0, t0, 4
__save_next_pc:
    mv      a0, t0
    call    TriggerSaveNextPC
__print_pc:
    call    ArchTriggerCallBack
__ret_from_exception:
    la      a0, g_trigger_swap_sp
    lw      sp, 0(a0)
    POP_ALL_REG_IN_EXCPTION
    mret
__normal_exception:
    la      a0, g_trigger_swap_sp
    lw      sp, 0(a0)
#endif
    addi  sp, sp, -EXC_SPECIAL_REG_SIZE
    csrr  s4, 0xFC2
    SREG  s4, 0 * REGBYTES(sp)
    csrr  s3, MCAUSE
    SREG  s3, 1 * REGBYTES(sp)
    csrr  s2, MTVAL
    SREG  s2, 2 * REGBYTES(sp)
    SREG  gp, 3 * REGBYTES(sp)
    csrr  s1, MSTATUS
    SREG  s1, 4 * REGBYTES(sp)
    csrr  s0, MEPC
    SREG  s0, 5 * REGBYTES(sp)
    SREG  tp, 6 * REGBYTES(sp)
    SREG  sp, 7 * REGBYTES(sp)
    csrw  MSCRATCH, sp      /* save exc stack */
#ifdef LOSCFG_ARCH_LINX_M
    li    a0, MCAUSE_MASK_INT_NUM
    and   a0, s3, a0            /* interrupt num */
    li    a1, NUM_HAL_INTERRUPT_NMI
    bne   a0, a1, 1f
#else
    beqz  a1, 1f
#endif
    tail  NmiEntry
1:
    tail  trapEntry;

NmiEntry:
#ifdef LOSCFG_LIB_CONFIGURABLE
    la    sp, g_nmiStackTop
    lw    sp, 0(sp)
#else
#if (LOSCFG_EXC_STACK_SIZE == 0) || (LOSCFG_NMI_STACK_SIZE == 0)
    REFRESH_SPFP NMI_STACK_TOP, NMI_STACK_BOTTOM
#else
    la    sp, __nmi_stack_top
#endif
#endif
    tail nmiHandle;

.section .text
.align 4
nmiHandle:
    /* Prevents scheduling during exceptions. */
    jal   LOS_TaskLock
# Remain in M-mode after mret
    csrr a0, MSCRATCH
    la   t0, OsNMIHandlerEntry
    jalr t0
    /* Resume scheduling */
    jal  LOS_TaskUnlockNoSched

    csrr sp, MSCRATCH       /* restore exc stack */
# Remain in M-mode after mret
    li   t0, LOS_MSTATUS_MPP
    csrs MSTATUS, t0
    RESTORE_ALL
    mret

.align 4
trapEntry:
    la   t0, g_excInfo
    lh   t1, 12(t0)
    bnez t1, 1f
#ifdef LOSCFG_LIB_CONFIGURABLE
    la    sp, g_excStackTop
    lw    sp, 0(sp)
#else
#if (LOSCFG_EXC_STACK_SIZE == 0) || (LOSCFG_NMI_STACK_SIZE == 0)
    REFRESH_SPFP EXC_STACK_TOP, EXC_STACK_BOTTOM
#else
    la    sp, __exc_stack_top
#endif
#endif
1:
    addi t1, t1, 0x1        /* g_excInfo.nestCnt++ */
    sh   t1, 12(t0)
    li   t1, REG_HARD_FAULT_MASK
    and  t2, s4, t1
    beqz t2, 2f
    li   t0, 16
    j    4f
2:
    li   t1, REG_LOCK_UP_MASK
    and  t2, s4, t1
    beqz t2, 3f
    li   t0, 17
    j    4f
3:
    /* Handle other exceptions */
    li    t0, MCAUSE_MASK_INT_NUM
    and   t0, s3, t0        /* s3 = mcause */
4:
    mv    a0, t0            /* a0 = t0 = excType */
    csrr  a1, MSCRATCH      /* a1 = excBufAddr */

    la    t0, OsExcHandleEntry
    jalr  t0
    j .