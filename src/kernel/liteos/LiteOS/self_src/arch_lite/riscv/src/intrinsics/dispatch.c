/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS dispatch module Generator.
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
#include "los_task_pri.h"

__attribute__((noinline)) MRET VOID POP_GPR_REG OsChangeTskStack(VOID *stackPointer)
{
    TaskContext *context = (TaskContext *)stackPointer;
    RISCV_CSRRW_MEPC(context->mepc);
    RISCV_CSRRW_MSTATUS(context->mstatus);

    RISCV_ADDI_TO_SP((UINTPTR)context, 0);
}

__attribute__((noinline)) VOID SwitchNewTask(LosTaskCB *newTask)
{
    OsChangeTskStack(newTask->stackPointer);
}

VOID ArchStartToRun(LosTaskCB *newTask)
{
    RISCV_CSRRC_MSTATUS(LOS_MSTATUS_MIE);
    newTask->taskStatus |= OS_TASK_STATUS_RUNNING;
    g_newTask = newTask;
    SwitchNewTask(newTask);
}

VOID PUSH_GPR_REG ArchTaskSchedule(LosTaskCB *newTask, LosTaskCB *runTask)
{
    UINT32 a1 = RISCV_ANDI(RISCV_CSRRC_MSTATUS(LOS_MSTATUS_MIE | LOS_MSTATUS_MPIE), LOS_MSTATUS_MIE);

    /* set mstatus to 0x1800 or 0x1880 */
    a1 = (a1 | 0x180) << 0x4;
    RISCV_CSRRS_MSTATUS(a1);
    RISCV_CSRRW_MEPC(RISCV_ADDI_FROM_RA(0));

    TaskContext *context = (TaskContext *)RISCV_GET_TASK_STACK();
    context->mepc = RISCV_CSRRS_MEPC(0);
    context->mstatus = RISCV_CSRRS_MSTATUS(0);
    runTask->stackPointer = (VOID *)context;

    SwitchNewTask(newTask);
}

VOID OsDisableIRQ(VOID)
{
    RISCV_CSRRC_MSTATUS(LOS_MSTATUS_MIE | LOS_MSTATUS_MPIE);
    return;
}

UINT32 ArchIntLock(VOID)
{
    UINT32 ret = RISCV_CSRRS_MSTATUS(0);
    RISCV_CSRRC_MSTATUS(LOS_MSTATUS_MIE);
    return ret;
}

UINT32 ArchIntUnlock(VOID)
{
    UINT32 ret = RISCV_CSRRS_MSTATUS(0);
    RISCV_CSRRS_MSTATUS(LOS_MSTATUS_MIE);
    return ret;
}

VOID ArchIntRestore(UINT32 intSave)
{
    RISCV_CSRRW_MSTATUS(intSave);
    return;
}
