/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS trap module Generator.
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
#include "arch/generator.h"
#include "arch/stack_ops.h"

#define MCAUSE_MASK_INT_NUM        0x7FFFFFFF
#define EXC_TYPE_HARD_FAULT        16U
#define EXC_TYPE_LOCK_UP           17U

/* non vectored, clic, New vector base is at least 64-byte aligned. */
__attribute__ ((aligned (64))) MRET VOID PUSH_GPR_REG POP_GPR_REG TrapVector(VOID)
{
    UINT32 mcause = RISCV_CSRRS_MCAUSE(0);
    UINT32 isInt = mcause &  MCAUSE_MASK_INT_BIT;

    if (isInt == 0) {
        /* Exception can reuses the 16-byte stack space automatically applied by the compiler,
         * and does not need to be manually updated. */
        ExcContext *excContext = (ExcContext *)RISCV_ADDI_FROM_SP(0);

        /* Correct intrinsics:RISCV_CSRRS_CXCPTSC is required for the new version of the compiler. */
        excContext->ccause = READ_CSR(0xFC2);

        excContext->mcause = RISCV_CSRRS_MCAUSE(0);
        excContext->mtval = RISCV_CSRRS_MTVAL(0);
        excContext->gp = RISCV_ADDI_FROM_GP(0);
        excContext->taskContext.mepc = RISCV_CSRRS_MEPC(0);
        excContext->taskContext.mstatus = RISCV_CSRRS_MSTATUS(0);
        RISCV_SET_MSCRATCH((UINTPTR)excContext);
        if (g_excInfo.nestCnt == 0) {
            UINT32 trapSp = (UINT32)&__exc_stack_top;
            RISCV_ADDI_TO_SP(trapSp, 0);
        }
        g_excInfo.nestCnt++;
        if (excContext->ccause & REG_LOCK_UP_MASK) {
            OsExcHandleEntry(EXC_TYPE_LOCK_UP, excContext);
        } else if (excContext->ccause & REG_HARD_FAULT_MASK) {
            OsExcHandleEntry(EXC_TYPE_HARD_FAULT, excContext);
        } else {
            OsExcHandleEntry(excContext->mcause, excContext);
        }
        excContext = (ExcContext *)RISCV_GET_TASK_STACK();
        RISCV_CSRRW_MEPC(excContext->taskContext.mepc);
        RISCV_CSRRW_MSTATUS(excContext->taskContext.mstatus);
    } else {
        TaskContext *context = (TaskContext *)RISCV_ADDI_FROM_SP(0);
        context->mepc = RISCV_CSRRS_MEPC(0);
        context->mstatus = RISCV_CSRRS_MSTATUS(0);

        context = (TaskContext *)RISCV_CSRRW_MSCRATCH((UINTPTR)context);
        RISCV_ADDI_TO_SP((UINTPTR)context, 0);
        OsIntEntry();
        context = (TaskContext *)&__irq_stack_top;
        context = (TaskContext *)RISCV_CSRRW_MSCRATCH((UINTPTR)context);
        RISCV_ADDI_TO_SP((UINTPTR)context, 0);
        OsSchedProcSchedFlag();
        RISCV_CSRRW_MEPC(context->mepc);
        RISCV_CSRRW_MSTATUS(context->mstatus);
    }
}

