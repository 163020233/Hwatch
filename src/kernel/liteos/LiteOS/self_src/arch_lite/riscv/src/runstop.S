/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Risc-v Hw Runstop Implementation
 * Author: Huawei LiteOS Team
 * Create: 2023-04-20
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
 * ---------------------------------------------------------------------------- */

#if defined(LOSCFG_KERNEL_DEEPSLEEP)
#include "arch/cpu.h"
#if LOSCFG_APC_ENABLE
#include "arch/pmp.h"
#endif

.extern g_saveCpuReg
#if LOSCFG_APC_ENABLE
.extern g_savePmpReg
#endif

.global OsSRSaveRegister
.global OsSRRestoreRegister

.section .text, "ax"
.option rvc

#define CPU_ALL_COMMON_REG_SIZE 120

/* *************************************** */
/* Save CPU-Registers to RAM */
OsSRSaveRegister:
    # save common register
    la a0,  g_saveCpuReg
    SREG x1, 0 * REGBYTES(a0)
    SREG x2, 1 * REGBYTES(a0)
    SREG x3, 2 * REGBYTES(a0)
    SREG x4, 3 * REGBYTES(a0)
    SREG x5, 4 * REGBYTES(a0)
    SREG x6, 5 * REGBYTES(a0)
    SREG x7, 6 * REGBYTES(a0)
    SREG x8, 7 * REGBYTES(a0)
    SREG x9, 8 * REGBYTES(a0)
    SREG x11, 9 * REGBYTES(a0)
    SREG x12, 10 * REGBYTES(a0)
    SREG x13, 11 * REGBYTES(a0)
    SREG x14, 12 * REGBYTES(a0)
    SREG x15, 13 * REGBYTES(a0)
    SREG x16, 14 * REGBYTES(a0)
    SREG x17, 15 * REGBYTES(a0)
    SREG x18, 16 * REGBYTES(a0)
    SREG x19, 17 * REGBYTES(a0)
    SREG x20, 18 * REGBYTES(a0)
    SREG x21, 19 * REGBYTES(a0)
    SREG x22, 20 * REGBYTES(a0)
    SREG x23, 21 * REGBYTES(a0)
    SREG x24, 22 * REGBYTES(a0)
    SREG x25, 23 * REGBYTES(a0)
    SREG x26, 24 * REGBYTES(a0)
    SREG x27, 25 * REGBYTES(a0)
    SREG x28, 26 * REGBYTES(a0)
    SREG x29, 27 * REGBYTES(a0)
    SREG x30, 28 * REGBYTES(a0)
    SREG x31, 29 * REGBYTES(a0)
    addi a0, a0, CPU_ALL_COMMON_REG_SIZE

    # save specific register
    csrr a5, MTVEC
    csrr a4, MSCRATCH
    csrr a3, MEPC
    csrr a2, MCAUSE
    csrr a1, MSTATUS
    csrr t0, MTVT

    SREG a5, 0 * REGBYTES(a0)
    SREG a4, 1 * REGBYTES(a0)
    SREG a3, 2 * REGBYTES(a0)
    SREG a2, 3 * REGBYTES(a0)
    SREG a1, 4 * REGBYTES(a0)
    SREG t0, 5 * REGBYTES(a0)

#if LOSCFG_APC_ENABLE
    # save pmp register
    SAVE_PMP_REGS g_savePmpReg
#endif

    ret

/* ***************************************  */
/* Restore CPU-Registers from RAM */
OsSRRestoreRegister:
#if LOSCFG_APC_ENABLE
    # resume pmp register
    LOAD_PMP_REGS g_savePmpReg
#endif

    # enable cache
    csrwi 0x7C0, 0x1
    fence
    csrwi 0x7C1, 0x1
    fence
    csrwi 0x7C6, 0x1
    fence
    csrwi 0x7C7, 0x1 | (0x1 << 4)
    fence

    la a0,  g_saveCpuReg
    addi  a0, a0, CPU_ALL_COMMON_REG_SIZE
    # resume specific register
    LREG a5, 0 * REGBYTES(a0)
    LREG a4, 1 * REGBYTES(a0)
    LREG a3, 2 * REGBYTES(a0)
    LREG a2, 3 * REGBYTES(a0)
    LREG a1, 4 * REGBYTES(a0)
    LREG t0, 5 * REGBYTES(a0)

    csrw MTVEC, a5
    csrw MSCRATCH, a4
    csrw MEPC, a3
    csrw MCAUSE, a2
    csrw MSTATUS, a1
    csrw MTVT, t0

    # resume common register
    addi a0, a0, -CPU_ALL_COMMON_REG_SIZE
    LREG x1, 0 * REGBYTES(a0)
    LREG x2, 1 * REGBYTES(a0)
    LREG x3, 2 * REGBYTES(a0)
    LREG x4, 3 * REGBYTES(a0)
    LREG x5, 4 * REGBYTES(a0)
    LREG x6, 5 * REGBYTES(a0)
    LREG x7, 6 * REGBYTES(a0)
    LREG x8, 7 * REGBYTES(a0)
    LREG x9, 8 * REGBYTES(a0)
    LREG x11, 9 * REGBYTES(a0)
    LREG x12, 10 * REGBYTES(a0)
    LREG x13, 11 * REGBYTES(a0)
    LREG x14, 12 * REGBYTES(a0)
    LREG x15, 13 * REGBYTES(a0)
    LREG x16, 14 * REGBYTES(a0)
    LREG x17, 15 * REGBYTES(a0)
    LREG x18, 16 * REGBYTES(a0)
    LREG x19, 17 * REGBYTES(a0)
    LREG x20, 18 * REGBYTES(a0)
    LREG x21, 19 * REGBYTES(a0)
    LREG x22, 20 * REGBYTES(a0)
    LREG x23, 21 * REGBYTES(a0)
    LREG x24, 22 * REGBYTES(a0)
    LREG x25, 23 * REGBYTES(a0)
    LREG x26, 24 * REGBYTES(a0)
    LREG x27, 25 * REGBYTES(a0)
    LREG x28, 26 * REGBYTES(a0)
    LREG x29, 27 * REGBYTES(a0)
    LREG x30, 28 * REGBYTES(a0)
    LREG x31, 29 * REGBYTES(a0)
    ret

#endif
