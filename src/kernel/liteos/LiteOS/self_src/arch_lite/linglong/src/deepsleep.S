/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Linglong Lowpower Implementation
 * Author: Huawei LiteOS Team
 * Create: 2023-10-22
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

#include "asm/memmap_config.h"
#include "arch/asm.h"

.extern g_saveCpuReg    /* where all register saved */

#define REG_PAIR_SIZE      16

/************************** SAVE HELPER **************************/
.macro SAVE_GENERAL_REGS
    st.a128.i a0:a1:a2:a3,     a14, REG_PAIR_SIZE * 0
    st.a128.i a4:a5:a6:a7,     a14, REG_PAIR_SIZE * 1
    st.a128.i a8:a9:a10:a11,   a14, REG_PAIR_SIZE * 2
    st.a128.i a12:a13:a14:a15, a14, REG_PAIR_SIZE * 3
    st.a128.i a16:a17:a18:a19, a14, REG_PAIR_SIZE * 4
    st.a128.i a20:a21:a22:a23, a14, REG_PAIR_SIZE * 5
    st.a128.i a24:a25:a26:a27, a14, REG_PAIR_SIZE * 6
    st.a128.i a28:a29:a30:a31, a14, REG_PAIR_SIZE * 7
    add.i a14, a14, REG_PAIR_SIZE * 8
.endm

/** We only saved necessary SSRs (SP, FBR, SR, GPSR) and late SSRs (VECBASE, FPCTL, FPSR, SUP_USPB, SUP_LSPB),
  * other SSRs associated with hardware loops (LBEG, LEND, LCOUNT), circular buffer (CBEG, CEND) instruction are 
  * NOT included because they're not expected to use here.
  */
.macro SAVE_SSR
    mov.a.ssr a4, gpsr
    mov.a.ssr a5, sp
    mov.a.ssr a6, fbr
    mov.a.ssr a7, sr
    st.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 0
    mov.a.ssr a4, vecbase
    mov.a.ssr a5, fpctl
    mov.a.ssr a6, fpsr
    st.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 1
    mov.a.ssr a4, sup_uspb
    mov.a.ssr a5, sup_lspb
    st.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 2

    add.i a14, a14, REG_PAIR_SIZE * 3
.endm

/** pc/fp/a14/a15 is on the top frame, we saved top frame because further function call will overrides
 *  current frame thus leads to wrong stack pop out value.
 */
.macro SAVE_STACK
    ld.a128.i.s a4:a5:a6:a7, 0
    st.a128.i   a4:a5:a6:a7, a14, 0
    add.i a14, a14, REG_PAIR_SIZE
.endm

/************************** RESTORE HELPER **************************/
.macro RESTORE_GENERAL_REGS
    add.i a14, a14, -REG_PAIR_SIZE * 8
    ld.a128.i a0:a1:a2:a3,     a14, REG_PAIR_SIZE * 0
    ld.a128.i a4:a5:a6:a7,     a14, REG_PAIR_SIZE * 1
    ld.a128.i a8:a9:a10:a11,   a14, REG_PAIR_SIZE * 2
    ld.a128.i a12:a13:a14:a15, a14, REG_PAIR_SIZE * 3
    ld.a128.i a16:a17:a18:a19, a14, REG_PAIR_SIZE * 4
    ld.a128.i a20:a21:a22:a23, a14, REG_PAIR_SIZE * 5
    ld.a128.i a24:a25:a26:a27, a14, REG_PAIR_SIZE * 6
    ld.a128.i a28:a29:a30:a31, a14, REG_PAIR_SIZE * 7
.endm

.macro RESTORE_SSR
    add.i a14, a14, -REG_PAIR_SIZE * 3

    ld.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 0
    mov.ssr.a gpsr, a4
    mov.ssr.a sp, a5
    mov.ssr.a fbr, a6
    mov.ssr.a sr, a7
    ld.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 1
    mov.late.ssr.a vecbase, a4
    mov.late.ssr.a fpctl, a5
    mov.late.ssr.a fpsr, a6
    ld.a128.i a4:a5:a6:a7, a14, REG_PAIR_SIZE * 2
    mov.late.ssr.a sup_uspb, a4
    mov.late.ssr.a sup_lspb, a5
.endm

.macro RESTORE_STACK
    add.i a14, a14, -REG_PAIR_SIZE

    ld.a128.i   a4:a5:a6:a7, a14, 0
    st.a128.i.s a4:a5:a6:a7, 0
.endm

/************************** LOWPOWER FUNCTIONS **************************/
.type OsSRSaveRegister, %function
.global OsSRSaveRegister
OsSRSaveRegister:
    mov.i a14, g_saveCpuReg
    SAVE_GENERAL_REGS
    SAVE_STACK
    SAVE_SSR
    retrr

.type OsSRRestoreRegister, %function
.global OsSRRestoreRegister
OsSRRestoreRegister:
    mov.i a14, g_saveCpuReg + REG_PAIR_SIZE * 12
    RESTORE_SSR
    RESTORE_STACK
    RESTORE_GENERAL_REGS
    rasinv /* invidate RAS(Return Address Stack) buffer, force core to pop value from stack */
    retrr
