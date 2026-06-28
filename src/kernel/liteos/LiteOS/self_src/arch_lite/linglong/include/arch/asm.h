/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Assembly Defines and Macros HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2021-12-17
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

#define ASLR_FUNCTION(x)                    \
    .global x;                              \
    .type x, %function;                     \
    .section ".laslr.text", "ax";           \
x:

.macro GET_CORE_ID  t0
#ifdef LOSCFG_KERNEL_SMP
    bextu.ssr.i  \t0, crthid, 2, 6
#else
    mov.val.i    \t0, 0
#endif
.endm

.macro GET_STACK stack_start, stack_size, t0, t1, top, bottom
#ifdef LOSCFG_KERNEL_SMP
    GET_CORE_ID  \t0
    { mov.i \t1, \stack_size ; mov.i \top, \stack_start }
    mula.u32.u16 \top, \t0, \t1
    add          \bottom, \top, \t1
#else
    mov.i        \top, \stack_start
    add.i        \bottom, \top, \stack_size
#endif
.endm

.macro SAVE_EPC stackAddr, errStackSave, t0, t1
#ifdef LOSCFG_KERNEL_SMP
    { bextu.ssr.i \t1, crthid, 2, 6 ; mov.i \t0, \errStackSave }
#else
    { mov.i \t1, 0; mov.i \t0, \errStackSave }
#endif
    add.sll.2 \t0, \t1, \t0
    st.a32.i \stackAddr, \t0, 0
.endm

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ARCH_ASM_H */
