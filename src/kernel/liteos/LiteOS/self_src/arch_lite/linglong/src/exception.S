/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Linglong Exception Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-01-05
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

.extern OsExcHandleEntry
.extern g_epc

.global DefaultExcHandler
DefaultExcHandler:
    b .

.global OsExcDispatch
.type OsExcDispatch, function
OsExcDispatch:
    /* get current sp */
    mov.a.ssr a3, sp

    set.sr.ssoup.dis

    ld.a32.i.s a4, 32 /* 32: a1-a7 */
    SAVE_EPC a4, g_epc, a5, a6

    /* switch to exc stack */
#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    GET_STACK __stack, STACK_SIZE, a5, a6, a7, a4
    mov.ssr.a sp, a4
#endif

    /* save a8-a31 */
    push.a128 a28:a29:a30:a31
    push.a128 a24:a25:a26:a27
    push.a128 a20:a21:a22:a23
    push.a128 a16:a17:a18:a19
    push.a128 a12:a13:a14:a15
    push.a128 a8:a9:a10:a11

    /* save orig. a0-a7 */
    ld.a128.i a8:a9:a10:a11, a3, 16
    ld.a128.iz a12:a13:a14:a15, a3
    push.a128 a8:a9:a10:a11
    push.a128 a12:a13:a14:a15

    /* save SSRs */
    mov.a.ssr a4, sr
    mov.a.ssr a5, fbr
    push.a128 a4:a5:a6:a7

    mov.a.ssr a2, emaddr
    push.a128 a0:a1:a2:a3

    /* handle fault */
    mov.a.ssr a1, sp
    call OsExcHandleEntry
    b .
