/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description: LiteOS System Stack Allocate.
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

.global __startup_stack_top
.global __irq_stack_top
.global __nmi_stack_top
.global __exc_stack_top

.section ".int_stack", "wa", %nobits
.align 4  /* 2^4 bytes aligned for stacks */

__irq_stack:
    .space LOSCFG_IRQ_STACK_SIZE
__irq_stack_top:

__startup_stack:
#if defined(LOSCFG_STARTUP_STACK_SIZE) && (LOSCFG_STARTUP_STACK_SIZE > 0)
    .space LOSCFG_STARTUP_STACK_SIZE
#endif
__startup_stack_top:

__exc_stack:
#if defined(LOSCFG_EXC_STACK_SIZE) && (LOSCFG_EXC_STACK_SIZE > 0)
    .space LOSCFG_EXC_STACK_SIZE
#endif
__exc_stack_top:

__nmi_stack:
#if defined(LOSCFG_NMI_STACK_SIZE) && (LOSCFG_NMI_STACK_SIZE > 0)
    .space LOSCFG_NMI_STACK_SIZE
#endif
__nmi_stack_top:
