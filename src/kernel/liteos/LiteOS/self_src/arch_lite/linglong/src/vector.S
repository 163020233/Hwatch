/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Linglong Vector Table Implementation
 * Author: Huawei LiteOS Team
 * Create: 2024-07-24
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

#include "asm/asm.h"
#include "asm/memmap_config.h"
#include "arch/exception.h"
#include "arch/asm.h"

/*********************** Vector Table ***********************/
.section .exc.vector.text, "ax"
.align 32
.global exception_handlers
exception_handlers:

.align 32
_Reserved_IRQ0:
    j DefaultIrqHandler

.align 32
_NmiHandler:
    j NmiIrqHandler

.align 32
_MaskableIrqHandler:
    j OsIrqHandler

.align 32
_Reserved_IRQ3:
    j DefaultIrqHandler

.align 32
_Reserved_IRQ4:
    j DefaultIrqHandler

.align 32
_Reserved_IRQ5:
    j DefaultIrqHandler

.align 32
_Reserved_IRQ6:
    j DefaultIrqHandler

.align 32
_Reserved_IRQ7:
    j DefaultIrqHandler

.align 32
_IllegalException:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_ILLEGAL
    mov.a.ssr a1, isr1
    j OsExcDispatch

.align 32
_ProgramFetchErrorExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_PROG_FETCH_ERR
    mov.a.ssr a1, isr1
    j OsExcDispatch

.align 32
_DataLoadErrorExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_DATA_LOAD_ERR
    mov.a.ssr a1, isr0
    j OsExcDispatch

.align 32
_DataStoreMpuErrorExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_DATA_STORE_ERR
    mov.a.ssr a1, isr0
    j OsExcDispatch

.align 32
_ReservedExc4:
    j DefaultExcHandler

.align 32
_IntDivByZeroExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_INT_DIV_ZERO
    mov.a.ssr a1, isr1
    j OsExcDispatch

.align 32
_DebugExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_DEBUG
    mov.a.ssr a1, isr1
    j OsExcDispatch

.align 32
_SyscallExc:
    push.a128 a4:a5:a6:a7
    push.a128 a0:a1:a2:a3
    mov.i a0, OS_EXC_SYSCALL
    mov.a.ssr a1, isr1
    j OsExcDispatch
