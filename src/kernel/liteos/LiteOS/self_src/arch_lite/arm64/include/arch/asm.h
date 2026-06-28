/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Aarch64 Assembly Defines and Macros HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#ifndef _ARCH_ASM_H
#define _ARCH_ASM_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define FUNCTION(x)     \
    .global x;          \
    .text;              \
x:                      \

#define ASLR_FUNCTION(x)                    \
    .global x;                              \
    .type x, %function;                     \
    .section ".laslr.text", "ax";           \
x:                                          \

/**
 * Get CoreID. Referenced to ArchCurrCpuid()
 * reg0: 64bit general propose register
 * reg1: 32/64bit general propose register
 * regOut: 32/64bit general propose register
 * Result saved in regOut.
 * changed: PSTATE.<N,Z,C>
 */
.macro GET_CORE_ID reg0, reg1, regOut
#ifdef LOSCFG_KERNEL_SMP
    mrs     \reg0, mpidr_el1
    ubfx    \reg1, \reg0, #0, #8                /* reg1: affinity level 0 */
    ubfx    \regOut, \reg0, #8, #8              /* regOut: affinity level 1 */
    tst     \reg0, #0x1000000                   /* check bit24: MT (multithreading) */
    csel    \regOut, \regOut, \reg1, ne         /* regOut = MT ? regOut : reg1 */
#else
    mov     \regOut, #0
#endif
.endm

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_ASM_H */
