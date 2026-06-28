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
.global  OsPerfSetIrqRegs

/* riscv interrupt & exception entry */
.section .itcm.text, "ax"
.align 6
TrapVector:
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
#ifdef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    /* get MCAUSE to gp */
    csrr    gp, MCAUSE
    li      tp, MCAUSE_MASK_INT_BIT
    and     tp, tp, gp
#ifdef LOSCFG_KENREL_BULITIN_ROM
    beqz    tp, OsExcTrapHandleHook        /* mcause and mepc will be saved in TrapHandle */
#else
    beqz    tp, TrapHandle        /* mcause and mepc will be saved in TrapHandle */
#endif
#endif

    PUSH_CALLER_REG /* sp must be 16 bytes aligned, now size is 32 REGBYTES (64 REGBYTES when FPU on) */

#ifdef LOSCFG_KERNEL_PERF
    csrr    a0, MEPC
    mv      a1, s0
    call    OsPerfSetIrqRegs
#endif

#ifdef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    li      a2, MCAUSE_MASK_INT_NUM
    and     a0, gp, a2            /* interrupt num */
    mv      t0, gp
    /* c-source OsIntEntry may use lwgp, so recover gp here. */
    INIT_GP_TO_DEFAULT
#else
    csrr    a0, MCAUSE
    li      a1, MCAUSE_MASK_INT_BIT
    li      a2, MCAUSE_MASK_INT_NUM
    and     a1, a0, a1
    and     a0, a0, a2            /* interrupt num */

#ifdef LOSCFG_KENREL_BULITIN_ROM
    beqz    a1, OsExcTrapHandleHook         /* mstatus and mepc will be saved in TrapHandle */
#else
    beqz    a1, TrapHandle        /* mcause and mepc will be saved in TrapHandle */
#endif

    csrr    t0, MCAUSE
#endif
    sw      t0, 0 * REGBYTES(sp)  /* store mcause to previously reserved (4*REGBYTES) space by PUSH_CALLER_REG */
    csrr    t0, MEPC
    sw      t0, 1 * REGBYTES(sp)  /* store mepc to previously reserved (4*REGBYTES) space by PUSH_CALLER_REG */

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
FirstInterruptHandle:
    csrrci  a0, mnxti, LOS_MSTATUS_MIE /* Get highest current interrupt and disable interrupts. */
    /* beqz    a0, TaskSwitch  if irq lose, here cause bug */
    beqz    a0, InterruptLose
    INTERRUPT_STACK_SWAP t0, t1 /* conditionally swap the stack pointer. */
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_irq_topOfStack, t0
#endif
ServiceLoop:
    lw      a1, (a0)
    jalr    a1                         /* Call C ABI Routine */
#ifdef LOSCFG_HWI_STACK_CHECK
    la      t0, g_hwiStackCanary
    lw      t0, 0(t0)
    lw      t1, 0(t0)

    la      t2, g_stackCanaryValue
    lw      t2, 0(t2)

    bne     t1, t2, stack_overflow
#endif /* LOSCFG_HWI_STACK_CHECK */
    csrrci  a0, mnxti, LOS_MSTATUS_MIE
    bnez    a0, ServiceLoop            /* OsIntHandle will enable and then disable interrupts */
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
InterruptRestore:
#ifdef LOSCFG_KENREL_BULITIN_ROM
    la      a0, g_irqStackTop
    lw      a0, 0(a0)
#else
    la      a0, __irq_stack_top
#endif
    bne     a0, sp, NoSwitch           /* if a0 is equal to sp, there is no interrupt. */
InterruptPost:
    csrrw   sp, MSCRATCH, sp
    lw      t0, 0 * REGBYTES(sp)
    csrw    MCAUSE, t0
#else   /* LOSCFG_ARCH_INTERRUPT_PREEMPTION */
    INTERRUPT_STACK_SWAP t0, t1
#ifdef LOSCFG_ARCH_VECTOR_ENABLE
    li      t2, LOS_MSTATUS_VS
    csrc    mstatus, t2      /* disable vpu */
#endif
#ifdef LOSCFG_TRIGGER_ENABLE
    call    TriggerSaveCSR
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_irq_topOfStack, t0
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
    call    TriggerRestoreCSR
#else
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_irq_topOfStack, t0
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
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    csrw    msplimit, zero
#endif
    INTERRUPT_STACK_SWAP t0, t1
#endif  /* LOSCFG_ARCH_INTERRUPT_PREEMPTION */

TaskSwitch:
    call    OsSchedProcSchedFlag
#ifdef LOSCFG_ARCH_VECTOR_ENABLE
    call    CurVpuTaskCheck
    beqz    a0, 1f
    li      t0, LOS_MSTATUS_VS_ON
    csrs    mstatus, t0      /* enable vpu */
1:
#endif

NoSwitch:
#ifdef LOSCFG_ARCH_RISCV_TES
    /* Forcible recovery Tes Env */
    li      t0, TMESEPS_PTES_MASK
    csrs    tmeseps, t0
#endif

    lw      t0, 1 * REGBYTES(sp)  /* retrieve the address at which exception happened */
    csrw    MEPC, t0              /* load mepc from previously reserved (4 * REGBYTES) space */
    lw      t0, 0 * REGBYTES(sp)  /* enable global interrupts */
    csrw    MCAUSE, t0            /* load mcause from previously reserved (4 * REGBYTES) space */

    POP_CALLER_REG

#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    SET_MSPLIMIT g_newTask_topOfStack, t0
#endif
    mret

InterruptLose:
    lw      a1, 1 * REGBYTES(sp)
    lw      a0, 0 * REGBYTES(sp)
    call    OsExcHwiLose
    j       NoSwitch

#ifdef LOSCFG_HWI_STACK_CHECK
stack_overflow:
    call    OsHwiStackOverflowHandler
    j       stack_overflow
#endif /* LOSCFG_HWI_STACK_CHECK */