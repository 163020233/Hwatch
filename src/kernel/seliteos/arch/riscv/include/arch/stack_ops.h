/* ----------------------------------------------------------------------------
 * Copyright (c) Huawei Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Riscv Assembly Defines and Macros HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2023-05-17
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

#ifndef ARCH_STACK_OPS_H
#define ARCH_STACK_OPS_H

#include "arch/cpu.h"
#include "task_cb.h"
#include "tdm_debug.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

.extern TrapHandle
.extern g_sysMode

#define MACRO_SP_MODE_OFFSET    0
#define MACRO_SP_MODE_REG       1
#define MACRO_SP_MODE_SYMBOL    2

.equ        MCAUSE_MASK_INT_BIT,        0x80000000

.equ        TMESEPS_PTES_MASK,          0x100

#ifdef LOSCFG_ARCH_LINX_M
.equ        MCAUSE_MASK_INT_NUM,        0x00000FFF
#else
.equ        MCAUSE_MASK_INT_NUM,        0x7FFFFFFF
#endif

.equ        MCAUSE_MPP_BIT_SHIFT,       0x1c
.equ        MCAUSE_MPP_BIT_MASK,        0x3

.equ        MSTATUS_MPP_BIT_SHIFT,      0x8
.equ        MSTATUS_MPP_BIT_MASK,       0x18

#ifdef  LOSCFG_ARCH_RISCV_TES
.equ        SP_LIMIT_EN,            1
#endif

/*
 * gp is already in use for other purposes,
 * preventing la from being optimized for gp
 */
.macro GET_SYMBOL_ADDR_WITHOUT_RELAX tmp, symbol
.option push
.option norelax
    la   \tmp, \symbol
.option pop
.endm

.macro SET_BIT_TMESEPS_PTES tmp
#ifdef LOSCFG_ARCH_RISCV_TES
    /* Forcible recovery Tes Env */
    li      \tmp, TMESEPS_PTES_MASK
    csrs    tmeseps, \tmp
#endif
.endm

.macro INIT_GP_TO_DEFAULT
    GET_SYMBOL_ADDR_WITHOUT_RELAX gp, __global_pointer$
.endm

.macro SWITCH_SYSMODE_TO_KENERL val, addr
    GET_SYMBOL_ADDR_WITHOUT_RELAX \addr, g_sysMode
    li    \val, LOS_MSTATUS_MPP
    SREG  \val, 0(\addr)
.endm

.macro RESTORE_SYSMODE val, addr
    la    \addr, g_sysMode
    SREG  \val, 0(\addr)
.endm

.macro PUSH_INTEGER_CALLER_REG base
#ifdef LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS
    c.push {ra, t0 - t2, a0 - a7, t3 - t6}, -INTEGER_CALLER_REG_SIZE
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

.macro PUSH_CALLER_REG
    PUSH_INTEGER_CALLER_REG 0
    addi    sp, sp, -(INTEGER_CALLEE_REG_SIZE + CPU_SPECIAL_REG_SIZE)
.endm

.macro POP_INTEGER_CALLER_REG base
#ifdef LOSCFG_LCMP_CUSTOM_INST16_EXTENSIONS
    c.pop  {ra, t0 - t2, a0 - a7, t3 - t6}, INTEGER_CALLER_REG_SIZE
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
    addi    sp, sp, (INTEGER_CALLEE_REG_SIZE + CPU_SPECIAL_REG_SIZE)
    POP_INTEGER_CALLER_REG 0
.endm

.macro  PUSH_INTEGER_CALLEE_REG base
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
.endm

.macro  PUSH_CALLEE_REG
    PUSH_INTEGER_CALLEE_REG CPU_SPECIAL_REG_SIZE
.endm

.macro  POP_INTEGER_CALLEE_REG base
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
.endm

.macro POP_ALL_REG
    POP_INTEGER_CALLEE_REG CPU_SPECIAL_REG_SIZE
    addi   sp, sp, (CPU_SPECIAL_REG_SIZE + INTEGER_CALLEE_REG_SIZE)
    POP_INTEGER_CALLER_REG 0
.endm

.macro SET_SP_LIMIT stack, tmp, mode
.if \mode == MACRO_SP_MODE_OFFSET // stack is variable
    lw   \tmp, \stack
    ori  \tmp, \tmp, SP_LIMIT_EN
    csrw TMSPLIMIT, \tmp
.elseif  \mode == MACRO_SP_MODE_REG // stack is reg
    ori  \tmp, \stack, SP_LIMIT_EN
    csrw TMSPLIMIT, \tmp
.elseif  \mode == MACRO_SP_MODE_SYMBOL
    la   \tmp, \stack
    ori  \tmp, \tmp, SP_LIMIT_EN
    csrw TMSPLIMIT, \tmp
.else
    .error  "Unsupported macro argument mode"
.endif
.endm

.macro RESTORE_SP_LIMIT tmp
    csrr \tmp, tmsplimitsw
    csrw TMSPLIMIT, \tmp
.endm

.macro CHECK_AND_CLEAR_SP_LIMIT isInterrupt
    bnez  \isInterrupt, isInt
    csrci TMSPLIMIT, 1
isInt:
.endm

.macro SWITCH_TO_KENERL_STACK status, tmp
    /* check user or kernel mode */
    mv    \tmp, \status
    srli  \tmp, \tmp, MCAUSE_MPP_BIT_SHIFT
    andi  \tmp, \tmp, MCAUSE_MPP_BIT_MASK
    bnez  \tmp, 1f
    SP_SWITCH_TO_KENERL \tmp
1:
.endm

.macro SWITCH_TO_USER_STACK tmp
    srli    \tmp, \tmp, MSTATUS_MPP_BIT_SHIFT
    andi    \tmp, \tmp, MSTATUS_MPP_BIT_MASK
    bnez    \tmp, 1f
    SP_SWITCH_TO_USER \tmp
1:
.endm

.macro SP_SWITCH_TO_KENERL tmp
    GET_SYMBOL_ADDR_WITHOUT_RELAX \tmp, g_runningTask
    LREG  \tmp, 0(\tmp)
    SREG  sp, TASK_CB_USER_SP(\tmp)
    LREG  sp, TASK_CB_KERNEL_SP(\tmp)
    SET_SP_LIMIT TASK_CB_KERNEL_TOP_SP(\tmp), \tmp, MACRO_SP_MODE_OFFSET
.endm

.macro SP_SWITCH_TO_USER tmp
    GET_SYMBOL_ADDR_WITHOUT_RELAX \tmp, g_runningTask
    LREG  \tmp, 0(\tmp)
    SREG  sp, TASK_CB_KERNEL_SP(\tmp)
    LREG  sp, TASK_CB_USER_SP(\tmp)
    SET_SP_LIMIT TASK_CB_USER_TOP_SP(\tmp), \tmp, MACRO_SP_MODE_OFFSET
.endm

.macro TRET_INSTR
#ifdef LOSCFG_ARCH_RISCV_NO_TRET
    ret   /* make sure tmescr.ete not enable, a2 ~ a7, t0 ~ t6 will be auto cleared, except (a0-a1) */
#else
    tret  /* a2 ~ a7, t0 ~ t6 will be auto cleared, except (a0-a1) */
#endif
.endm

.macro TES_ENTRY_MARKER
    c.nop 21
.endm


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARCH_STACK_OPS_H */
