/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ARMv7 JMP Implementation
 * Author: Huawei LiteOS Team
 * Create: 2023-08-29
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
.syntax unified
#ifdef LOSCFG_ARCH_CORTEX_M0
.arch armv6-m
#elif defined(LOSCFG_ARCH_CORTEX_M55)
.arch armv8.1-m.main
#else
.arch armv7e-m
#endif
.thumb

.section .text
.thumb

#define FUNCTION(x)       \
    .type x, %function;   \
    .global x;            \
x:;                       \
    .fnstart;             \
    .cantunwind;          \

FUNCTION(longjmp)
    ldmia   r0!, {r4-r12,lr}
    ldr     sp, [r0]
    tst     r1, #0
    it      eq
    moveq   r1, #1
    mov     r0, r1
    mov     pc, lr
    .fnend

FUNCTION(setjmp)
    stmia   r0!, {r4-r12,lr}
    str     sp, [r0]
    mov     r0, #0
    mov     pc, lr
    .fnend
