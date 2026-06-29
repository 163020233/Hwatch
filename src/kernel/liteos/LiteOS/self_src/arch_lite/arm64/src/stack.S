/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Aarch64 stack init Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-05-09
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

#include "arch/regs.h"
#include "arch/exception.h"

.arch armv8-a
.text
.align 5

.global __irq_stack_top
.global __irq_stack
.global __startup_stack_top
.global __startup_stack

.global stack_init
.type   stack_init,function

/* param0 is stack top, param1 is stack size, param2 is magic num */
.macro STACK_MAGIC_SET param0, param1, param2
    ldr     x0, =\param0
    mov     x2, \param1
    ldr     x1, =\param2
    bl      excstack_magic_set
.endm

stack_init:

/* clear out the interrupt and exception stack and set magic num to check the overflow */
do_excstatck:
    ldr     x1, =OS_STACK_INIT
    ldr     x2, =__startup_stack
    ldr     x3, =__irq_stack_top
/* Main loop sets 32 bytes at a time. */
excstatck_loop:
    str     x1, [x2], #8
    str     x1, [x2], #8
    str     x1, [x2], #8
    str     x1, [x2], #8
    cmp     x2,  x3
    blt     excstatck_loop
    mov     x4, x30   /* save link register */
    STACK_MAGIC_SET __irq_stack, #OS_EXC_IRQ_STACK_SIZE, OS_STACK_MAGIC_WORD
    STACK_MAGIC_SET __startup_stack, #OS_EXC_START_STACK_SIZE, OS_STACK_MAGIC_WORD
    mov     x30, x4
    ret


/*
 * set magic num to stack top for all cpu
 * x0 is stack top, x2 is stack size, x1 is magic num
 */
excstack_magic_set:
    mov     x3, #0
excstack_magic_loop:
    str     x1, [x0]
    add     x0, x0, x2
    add     x3, x3, #1
    cmp     x3, #CORE_NUM
    blt     excstack_magic_loop
    ret

.section .bss.prebss.stack
    .align 4

__startup_stack:
    .skip OS_EXC_START_STACK_SIZE * CORE_NUM
__startup_stack_top:
__irq_stack:
    .skip OS_EXC_IRQ_STACK_SIZE * CORE_NUM
__irq_stack_top:

