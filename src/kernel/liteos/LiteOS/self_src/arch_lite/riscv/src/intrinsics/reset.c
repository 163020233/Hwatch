/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS reset module Generator.
 * Author: Huawei LiteOS Team
 * Create : 2022-12-20
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
 * ---------------------------------------------------------------------------- */

#include "arch/intrinsics.h"
#include "los_config.h"
#include "arch/generator.h"

NORELAX __attribute__((section(".text"), noinline, noreturn)) VOID HandleReset(VOID)
{
    RISCV_ADDI_TO_GP((UINT32)&__global_pointer$, 0);
    RISCV_CSRRW_MTVEC((UINT32)TrapVector);

    /* Correct intrinsics:RISCV_CSRRW_MTVT is required for the new version of the compiler. */
    WRITE_CSR(0x307, g_vectorIntTable);

    RISCV_CSRRW_MSCRATCH((UINT32)&__irq_stack_top);
    RISCV_CSRRWI_MSTATUS(0);
    RISCV_CSRRWI_MIE(0);

    RISCV_ADDI_TO_SP((UINT32)&__startup_stack_top, 0);

    /* Correct optimization is required for the new version of the compiler. */
    asm volatile(
        "call    BoardConfig\n"
        "call    main\n"
    );

    for (; ;) {
    }
}

__attribute__((section(".entry.text"))) VOID reset_vector(VOID)
{
    HandleReset();
}
