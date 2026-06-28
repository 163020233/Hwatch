/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: Riscv Assembly Defines and Macros HeadFile
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

#ifndef _ARCH_STACK_OPS_H
#define _ARCH_STACK_OPS_H

#include "arch/cpu.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef LOSCFG_LIB_CONFIGURABLE
.extern g_irqStackTop
#else
.extern __irq_stack_top
#endif

.extern TrapHandle
.extern OsIntEntry
.extern OsSchedProcSchedFlag
.extern OsExcHwiLose

#ifdef LOSCFG_HWI_STACK_CHECK
.extern g_hwiStackCanary
.extern OsHwiStackProtectInit
.extern OsHwiStackOverflowHandler
#endif

/* task TCB offset */
#define TASK_CB_KERNEL_SP       0x0
#define TASK_CB_STATUS          0x4
#define EXC_SIZE_ON_STACK       (INT_SIZE_ON_STACK + EXC_SPECIAL_REG_SIZE)

.equ        REG_HARD_FAULT_MASK, 0x40000000
.equ        REG_LOCK_UP_MASK,    0x80000000
.equ        MCAUSE_MASK_INT_BIT,        0x80000000
.equ        TMESEPS_PTES_MASK,          0x100
.equ        MCAUSE_MPIL_BIT,            16
.equ        MINTSTATUS_MIL_BIT,         24

#ifdef LOSCFG_ARCH_LINX_M
.equ        MCAUSE_MASK_INT_NUM,        0x00000FFF /* bit 0-11 represent exception/interrupt code */
#else
.equ        MCAUSE_MASK_INT_NUM,        0x7FFFFFFF
#endif

.macro SET_MSPLIMIT stack, tmp
    addi sp, sp, -4
    sw \tmp, 0(sp)
    la \tmp, \stack
    lw \tmp, 0(\tmp)
    srli \tmp, \tmp, 4
    slli \tmp, \tmp, 4
    ori \tmp, \tmp, 1
    csrw msplimit, \tmp
    lw \tmp, 0(sp)
    addi sp, sp, 4
.endm

.macro INIT_GP_TO_DEFAULT
    .option push
    .option norelax
    la    gp, __global_pointer$
    .option pop
.endm

.macro  PUSH_FPU_CALLER_REG
    FSREG  ft11,  0 * FPUREGBYTES(sp)
    FSREG  ft10,  1 * FPUREGBYTES(sp)
    FSREG  ft9,   2 * FPUREGBYTES(sp)
    FSREG  ft8,   3 * FPUREGBYTES(sp)
    FSREG  fa7,   4 * FPUREGBYTES(sp)
    FSREG  fa6,   5 * FPUREGBYTES(sp)
    FSREG  fa5,   6 * FPUREGBYTES(sp)
    FSREG  fa4,   7 * FPUREGBYTES(sp)
    FSREG  fa3,   8 * FPUREGBYTES(sp)
    FSREG  fa2,   9 * FPUREGBYTES(sp)
    FSREG  fa1,  10 * FPUREGBYTES(sp)
    FSREG  fa0,  11 * FPUREGBYTES(sp)
    FSREG  ft7,  12 * FPUREGBYTES(sp)
    FSREG  ft6,  13 * FPUREGBYTES(sp)
    FSREG  ft5,  14 * FPUREGBYTES(sp)
    FSREG  ft4,  15 * FPUREGBYTES(sp)
    FSREG  ft3,  16 * FPUREGBYTES(sp)
    FSREG  ft2,  17 * FPUREGBYTES(sp)
    FSREG  ft1,  18 * FPUREGBYTES(sp)
    FSREG  ft0,  19 * FPUREGBYTES(sp)
.endm

.macro PUSH_INTEGER_CALLER_REG base
#ifdef LOS_COMPILE_LDM
    addi   sp, sp, -INTEGER_CALLER_REG_SIZE
    stmia  {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
#else
    addi   sp, sp, -INTEGER_CALLER_REG_SIZE
    SREG   t6,  (\base +  0 * REGBYTES)(sp)
    SREG   t5,  (\base +  1 * REGBYTES)(sp)
    SREG   t4,  (\base +  2 * REGBYTES)(sp)
    SREG   t3,  (\base +  3 * REGBYTES)(sp)
    SREG   a7,  (\base +  4 * REGBYTES)(sp)
    SREG   a6,  (\base +  5 * REGBYTES)(sp)
    SREG   a5,  (\base +  6 * REGBYTES)(sp)
    SREG   a4,  (\base +  7 * REGBYTES)(sp)
    SREG   a3,  (\base +  8 * REGBYTES)(sp)
    SREG   a2,  (\base +  9 * REGBYTES)(sp)
    SREG   a1,  (\base + 10 * REGBYTES)(sp)
    SREG   a0,  (\base + 11 * REGBYTES)(sp)
    SREG   t2,  (\base + 12 * REGBYTES)(sp)
    SREG   t1,  (\base + 13 * REGBYTES)(sp)
    SREG   t0,  (\base + 14 * REGBYTES)(sp)
    SREG   ra,  (\base + 15 * REGBYTES)(sp)
#endif
.endm

.macro PUSH_OTHER_CALLER_REG
    addi  sp, sp, -INTEGER_CALLEE_REG_SIZE
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi  sp, sp, -(FPU_STATUS_REG_SIZE + FPU_CALLER_REG_SIZE)
    PUSH_FPU_CALLER_REG
    addi  sp, sp, -FPU_CALLEE_REG_SIZE
#endif
    addi  sp, sp, -CPU_SPECIAL_REG_SIZE
#ifdef LOSCFG_ARCH_FPU_ENABLE
    frcsr  t2 /* use t2 after save it in PUSH_CALLER_REG */
    SREG   t2,  FCSR_STACK_OFFSET(sp) /* store frcsr to to previously reserved (4*REGBYTES) space for FPU */
#endif
.endm

.macro PUSH_CALLER_REG
#ifndef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    PUSH_INTEGER_CALLER_REG 0
#endif
    PUSH_OTHER_CALLER_REG
.endm

.macro POP_FPU_CALLER_REG
    FLREG  ft11,  0 * FPUREGBYTES(sp)
    FLREG  ft10,  1 * FPUREGBYTES(sp)
    FLREG  ft9,   2 * FPUREGBYTES(sp)
    FLREG  ft8,   3 * FPUREGBYTES(sp)
    FLREG  fa7,   4 * FPUREGBYTES(sp)
    FLREG  fa6,   5 * FPUREGBYTES(sp)
    FLREG  fa5,   6 * FPUREGBYTES(sp)
    FLREG  fa4,   7 * FPUREGBYTES(sp)
    FLREG  fa3,   8 * FPUREGBYTES(sp)
    FLREG  fa2,   9 * FPUREGBYTES(sp)
    FLREG  fa1,  10 * FPUREGBYTES(sp)
    FLREG  fa0,  11 * FPUREGBYTES(sp)
    FLREG  ft7,  12 * FPUREGBYTES(sp)
    FLREG  ft6,  13 * FPUREGBYTES(sp)
    FLREG  ft5,  14 * FPUREGBYTES(sp)
    FLREG  ft4,  15 * FPUREGBYTES(sp)
    FLREG  ft3,  16 * FPUREGBYTES(sp)
    FLREG  ft2,  17 * FPUREGBYTES(sp)
    FLREG  ft1,  18 * FPUREGBYTES(sp)
    FLREG  ft0,  19 * FPUREGBYTES(sp)
.endm

.macro POP_INTEGER_CALLER_REG base
#ifdef LOS_COMPILE_LDM
    ldmia  {ra, t0 - t2, a0 - a7, t3 - t6}, (sp)
    addi   sp, sp, INTEGER_CALLER_REG_SIZE
#else
    LREG   t6,  (\base +  0 * REGBYTES)(sp)
    LREG   t5,  (\base +  1 * REGBYTES)(sp)
    LREG   t4,  (\base +  2 * REGBYTES)(sp)
    LREG   t3,  (\base +  3 * REGBYTES)(sp)
    LREG   a7,  (\base +  4 * REGBYTES)(sp)
    LREG   a6,  (\base +  5 * REGBYTES)(sp)
    LREG   a5,  (\base +  6 * REGBYTES)(sp)
    LREG   a4,  (\base +  7 * REGBYTES)(sp)
    LREG   a3,  (\base +  8 * REGBYTES)(sp)
    LREG   a2,  (\base +  9 * REGBYTES)(sp)
    LREG   a1,  (\base + 10 * REGBYTES)(sp)
    LREG   a0,  (\base + 11 * REGBYTES)(sp)
    LREG   t2,  (\base + 12 * REGBYTES)(sp)
    LREG   t1,  (\base + 13 * REGBYTES)(sp)
    LREG   t0,  (\base + 14 * REGBYTES)(sp)
    LREG   ra,  (\base + 15 * REGBYTES)(sp)
    addi   sp, sp, INTEGER_CALLER_REG_SIZE
#endif
.endm

.macro POP_CALLER_REG
    addi  sp, sp, CPU_SPECIAL_REG_SIZE
#ifdef LOSCFG_ARCH_FPU_ENABLE
    addi  sp, sp, FPU_CALLEE_REG_SIZE
    POP_FPU_CALLER_REG
    addi  sp, sp, (FPU_CALLER_REG_SIZE + FPU_STATUS_REG_SIZE)
#endif
    addi  sp, sp, INTEGER_CALLEE_REG_SIZE
#ifndef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    POP_INTEGER_CALLER_REG 0
#endif
.endm

.macro  PUSH_FPU_CALLEE_REG base
    FSREG  fs11, (\base +  0 * FPUREGBYTES)(sp)
    FSREG  fs10, (\base +  1 * FPUREGBYTES)(sp)
    FSREG  fs9,  (\base +  2 * FPUREGBYTES)(sp)
    FSREG  fs8,  (\base +  3 * FPUREGBYTES)(sp)
    FSREG  fs7,  (\base +  4 * FPUREGBYTES)(sp)
    FSREG  fs6,  (\base +  5 * FPUREGBYTES)(sp)
    FSREG  fs5,  (\base +  6 * FPUREGBYTES)(sp)
    FSREG  fs4,  (\base +  7 * FPUREGBYTES)(sp)
    FSREG  fs3,  (\base +  8 * FPUREGBYTES)(sp)
    FSREG  fs2,  (\base +  9 * FPUREGBYTES)(sp)
    FSREG  fs1,  (\base + 10 * FPUREGBYTES)(sp)
    FSREG  fs0,  (\base + 11 * FPUREGBYTES)(sp)
.endm

.macro  PUSH_INTEGER_CALLEE_REG base
#ifdef  LOS_COMPILE_LDM
    addi   t0, sp, \base
    stmia  {s0 - s11}, (t0)
#else
    SREG   s11, (\base +  0 * REGBYTES)(sp)
    SREG   s10, (\base +  1 * REGBYTES)(sp)
    SREG   s9,  (\base +  2 * REGBYTES)(sp)
    SREG   s8,  (\base +  3 * REGBYTES)(sp)
    SREG   s7,  (\base +  4 * REGBYTES)(sp)
    SREG   s6,  (\base +  5 * REGBYTES)(sp)
    SREG   s5,  (\base +  6 * REGBYTES)(sp)
    SREG   s4,  (\base +  7 * REGBYTES)(sp)
    SREG   s3,  (\base +  8 * REGBYTES)(sp)
    SREG   s2,  (\base +  9 * REGBYTES)(sp)
    SREG   s1,  (\base + 10 * REGBYTES)(sp)
    SREG   s0,  (\base + 11 * REGBYTES)(sp)
#endif
.endm

.macro  PUSH_CALLEE_REG
#ifdef LOSCFG_ARCH_FPU_ENABLE
    PUSH_FPU_CALLEE_REG CPU_SPECIAL_REG_SIZE
    PUSH_INTEGER_CALLEE_REG (CPU_SPECIAL_REG_SIZE + FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    PUSH_INTEGER_CALLEE_REG CPU_SPECIAL_REG_SIZE
#endif
.endm

.macro POP_FPU_GENERAL_REG
    FLREG  fs11,  0 * FPUREGBYTES(t0)
    FLREG  fs10,  1 * FPUREGBYTES(t0)
    FLREG  fs9,   2 * FPUREGBYTES(t0)
    FLREG  fs8,   3 * FPUREGBYTES(t0)
    FLREG  fs7,   4 * FPUREGBYTES(t0)
    FLREG  fs6,   5 * FPUREGBYTES(t0)
    FLREG  fs5,   6 * FPUREGBYTES(t0)
    FLREG  fs4,   7 * FPUREGBYTES(t0)
    FLREG  fs3,   8 * FPUREGBYTES(t0)
    FLREG  fs2,   9 * FPUREGBYTES(t0)
    FLREG  fs1,  10 * FPUREGBYTES(t0)
    FLREG  fs0,  11 * FPUREGBYTES(t0)
    FLREG  ft11, 12 * FPUREGBYTES(t0)
    FLREG  ft10, 13 * FPUREGBYTES(t0)
    FLREG  ft9,  14 * FPUREGBYTES(t0)
    FLREG  ft8,  15 * FPUREGBYTES(t0)
    FLREG  fa7,  16 * FPUREGBYTES(t0)
    FLREG  fa6,  17 * FPUREGBYTES(t0)
    FLREG  fa5,  18 * FPUREGBYTES(t0)
    FLREG  fa4,  19 * FPUREGBYTES(t0)
    FLREG  fa3,  20 * FPUREGBYTES(t0)
    FLREG  fa2,  21 * FPUREGBYTES(t0)
    FLREG  fa1,  22 * FPUREGBYTES(t0)
    FLREG  fa0,  23 * FPUREGBYTES(t0)
    FLREG  ft7,  24 * FPUREGBYTES(t0)
    FLREG  ft6,  25 * FPUREGBYTES(t0)
    FLREG  ft5,  26 * FPUREGBYTES(t0)
    FLREG  ft4,  27 * FPUREGBYTES(t0)
    FLREG  ft3,  28 * FPUREGBYTES(t0)
    FLREG  ft2,  29 * FPUREGBYTES(t0)
    FLREG  ft1,  30 * FPUREGBYTES(t0)
    FLREG  ft0,  31 * FPUREGBYTES(t0)
.endm

.macro POP_INTEGER_CALLEE_REG base
#ifdef  LOS_COMPILE_LDM
    addi   t0, sp, \base
    ldmia  {s0 - s11}, (t0)
#else
    LREG   s11, (\base +  0 * REGBYTES)(sp)
    LREG   s10, (\base +  1 * REGBYTES)(sp)
    LREG   s9,  (\base +  2 * REGBYTES)(sp)
    LREG   s8,  (\base +  3 * REGBYTES)(sp)
    LREG   s7,  (\base +  4 * REGBYTES)(sp)
    LREG   s6,  (\base +  5 * REGBYTES)(sp)
    LREG   s5,  (\base +  6 * REGBYTES)(sp)
    LREG   s4,  (\base +  7 * REGBYTES)(sp)
    LREG   s3,  (\base +  8 * REGBYTES)(sp)
    LREG   s2,  (\base +  9 * REGBYTES)(sp)
    LREG   s1,  (\base + 10 * REGBYTES)(sp)
    LREG   s0,  (\base + 11 * REGBYTES)(sp)
#endif
.endm

.macro POP_ALL_REG
    lw      t0, 1 * REGBYTES(sp)
    csrw    MEPC, t0              /* load mepc from previously reserved (4 * REGBYTES) space */
    lw      t0, 0 * REGBYTES(sp)
    csrw    MSTATUS, t0           /* load mstatus from previously reserved (4 * REGBYTES) space */
#ifdef LOSCFG_ARCH_FPU_ENABLE
    lw     t0, FCSR_STACK_OFFSET(sp)
    fscsr  t0
    addi   t0, sp, CPU_SPECIAL_REG_SIZE
    POP_FPU_GENERAL_REG
    POP_INTEGER_CALLEE_REG (CPU_SPECIAL_REG_SIZE + FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    POP_INTEGER_CALLEE_REG CPU_SPECIAL_REG_SIZE
#endif
    addi  sp, sp, INT_SIZE_ON_STACK - INTEGER_CALLER_REG_SIZE
#ifndef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    POP_INTEGER_CALLER_REG 0
#endif
.endm

.macro POP_ALL_REG_IN_EXCPTION
#ifdef LOSCFG_ARCH_FPU_ENABLE
    lw     t0, FCSR_STACK_OFFSET(sp)
    fscsr  t0
    addi   t0, sp, CPU_SPECIAL_REG_SIZE
    POP_FPU_GENERAL_REG
    POP_INTEGER_CALLEE_REG (CPU_SPECIAL_REG_SIZE + FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    POP_INTEGER_CALLEE_REG CPU_SPECIAL_REG_SIZE
#endif
    addi  sp, sp, INT_SIZE_ON_STACK - INTEGER_CALLER_REG_SIZE
#ifndef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    POP_INTEGER_CALLER_REG 0
#endif
.endm

.macro RESTORE_ALL
    LREG  t0, 4 * REGBYTES(sp)
    csrw  MSTATUS, t0
    LREG  t0, 5 * REGBYTES(sp)
    csrw  MEPC, t0

#ifdef LOSCFG_ARCH_FPU_ENABLE
    lw     t0, (EXC_SPECIAL_REG_SIZE + FCSR_STACK_OFFSET)(sp)
    fscsr  t0
    addi   t0, sp, (EXC_SPECIAL_REG_SIZE + CPU_SPECIAL_REG_SIZE)
    POP_FPU_GENERAL_REG
    POP_INTEGER_CALLEE_REG (EXC_SPECIAL_REG_SIZE + CPU_SPECIAL_REG_SIZE + FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE)
#else
    POP_INTEGER_CALLEE_REG (EXC_SPECIAL_REG_SIZE + CPU_SPECIAL_REG_SIZE)
#endif
    addi  sp, sp, (EXC_SIZE_ON_STACK - INTEGER_CALLER_REG_SIZE)
#ifndef LOSCFG_ARCH_RISCV_INT_AUTO_STACK
    POP_INTEGER_CALLER_REG 0
#endif
.endm

/*
 * Usual use: csrrw sp, mscratchcswl, sp
 * Pseudocode operation.
 * if ( (mcause.pil==0) != (mintstatus.mil==0) ) then {
 *     t = rs1; rd = mscratch; mscratch = t;
 * } else {
 *     rd = rs1; // mscratch unchanged.
 * }
 */
.macro INTERRUPT_STACK_SWAP tmp0 tmp1
#if defined(LOSCFG_ARCH_RISCV_TES) && defined(LOSCFG_ARCH_INTERRUPT_PREEMPTION)
    csrr    \tmp0, MCAUSE       /* get MCAUSE.mpil[23:16] Previous interrupt level */
    csrr    \tmp1, MINTSTATUS   /* get MINTSTATUS.mil[31:24]  */
    slli    \tmp0, \tmp0, (MINTSTATUS_MIL_BIT - MCAUSE_MPIL_BIT)
    /* MCAUSE.mpil[23:16] & MINTSTATUS.mil[31:24]  The mil in the interrupt must be greater than 0. */
    and     \tmp0, \tmp0, \tmp1
    srli    \tmp0, \tmp0, MINTSTATUS_MIL_BIT
    bnez    \tmp0, 1f
    csrrw   sp, MSCRATCHCSWL, sp
1:
#else
    csrrw   sp, MSCRATCHCSWL, sp
#endif
.endm
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_STACK_OPS_H */
