/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS System Start Module Implementation.
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

#include "arch/cpu.h"
#include "arch/pmp.h"

#ifdef LOSCFG_LASLR
#define STARTUP_ENTRY kernel_startup
#else
#define STARTUP_ENTRY reset_vector
#endif

.extern TrapVector
.global __startup_stack_top
.global __irq_stack_top
.global __nmi_stack_top
.global __exc_stack_top
.global STARTUP_ENTRY
.extern BoardConfig
#ifdef LOSCFG_ARCH_LINX_M
.extern g_vectorIntTable
#endif
.option rvc
.section .entry.text, "ax"
.type   STARTUP_ENTRY, @function
STARTUP_ENTRY:
    .option push
    .option norelax
    tail HandleReset
    .option pop

.section .text, "ax"
.option rvc
HandleReset:
    .option push
    .option norelax
    /* initialize global pointer */
    la      gp, __global_pointer$
    la      a0, TrapVector
    .option pop

    csrw    MTVEC, a0
#ifdef LOSCFG_ARCH_LINX_M
    la      t0, g_vectorIntTable
    csrw    MTVT, t0
    la      t0, __irq_stack_top
    csrw    MSCRATCH, t0
#ifdef LOSCFG_ARCH_PMU
    /* disable all event counter, linx131 not support mcountinhibit */
    li      t0, UINT32_CUT_MASK
    csrw    0x320, t0
#ifdef LOSCFG_ARCH_RISCV_TES
    /*
     * XLEN - 1      9      8     7 - 6    5      4       3     2     1     0
     * |    ROZ  | uudis | tce | tspsw | uvsro | pad | ufsro | tvl | ete | eme |
     * tce:Trusted Counter Enable, is used to enable the use of performance counter
     */
    li      t0, 1 << 8
    csrs    tmescr, t0
#endif
#endif
#endif
    csrwi   MSTATUS, 0
    csrwi   mie, 0

#ifdef LOSCFG_ARCH_FPU_ENABLE
   /* set to initial state of FPU */
    li      t0, LOS_MSTATUS_FS
    /* Trusted code which uses floating point instructions also needs to consider
     * how the floating point control and status
     * register, fcsr, and the fs bits in the mstatus register are managed.
    */
    csrs    mstatus, t0
    fssr    x0
#endif

    /* initialize stack pointer */
    la      sp, __startup_stack_top

    /* Board features. */
    call    BoardConfig

    /* jump to main func. */
    tail    main
