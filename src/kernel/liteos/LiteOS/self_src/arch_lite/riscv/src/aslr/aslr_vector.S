/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: Implementation Of The System Startup.
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
#include "asm/memmap_config.h"

.global reset_vector
.global aslr_exc_entry

.extern OsAslrProcess
.extern OsGetAddrAndClear
.extern AslrBoardConfig

.option rvc
.section ".laslr.init", "ax"
reset_vector:  /* aslr_startup */
    tail AslrHandleReset

.section ".laslr.init", "ax"
.option rvc
AslrHandleReset:
    .option push
    .option norelax
    lla     t0, g_aslrInitFlag
    .option pop
    lw      t1, 0(t0)
    beqz    t1, 1f
    jr      t1
1:
    /* initialize stack pointer */
    li      t0, ASLR_STARTUP_STACK
    mv      sp, t0

    lla     t0, aslr_exc_entry
    csrw    mtvec, t0

    csrwi   MSTATUS, 0
    csrwi   mie, 0

    jal     AslrBoardConfig

#ifdef LOSCFG_ARCH_FPU_ENABLE
   /* set to initial state of FPU */
    li      t0, LOS_MSTATUS_FS
    csrs    MSTATUS, t0
    fssr    x0
#endif

    /* call reloc process */
    jal     OsAslrProcess
    bnez    a0, aslr_exc_entry /* OsAslrProcess not return OK, goto exc entry */

    fence

    /* jump to kernel_startup entry */
    jal     OsGetAddrAndClear
    .option push
    .option norelax
    lla     t1, g_aslrInitFlag
    .option pop
    sw      a0, 0(t1)
    jr      a0

.align 6
.section ".laslr.init", "ax"
aslr_exc_entry:
    j .
