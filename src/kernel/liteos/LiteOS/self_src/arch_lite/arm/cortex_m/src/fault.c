/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2022. All rights reserved.
 * Description: Cortex-M Exception Handler
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

#include "arch/exception.h"
#include "los_memory_pri.h"
#include "los_task_pri.h"
#include "los_printf_pri.h"
#include "nvic.h"
#include "los_init.h"
#if defined(LOSCFG_SHELL_EXCINFO_DUMP)
#include "los_exc_pri.h"
#include "los_hwi_pri.h"
#endif

#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif

#ifdef LOSCFG_TRUSTZONE
#include "arch/secure/non_secure_call.h"
#endif

#ifdef LOSCFG_KERNEL_HPL
#include "los_hpl_pri.h"
#endif

#define USGFAULT               (1U << 18)
#define BUSFAULT               (1U << 17)
#define MEMFAULT               (1U << 16)
#define DIV0FAULT              (1U << 4)

#ifdef LOSCFG_ARCH_INTERRUPT_TAKEOVER
#define NON_MASKABLE_IRQN      (-14)
#define HARD_FAULT_IRQN        (-13)
#define MEMORY_MANAGEMENT_IRQN (-12)
#define BUSFAULT_IRQN          (-11)
#define USAGEFAULT_IRQN        (-10)
#define SVCALL_IRQN            (-5)
#endif

#define MASK_16_BIT            16

#define REG0_IDX               0
#define REG4_IDX               4
#define REG12_IDX              12

#define ARM_EXCEPTION_SYSTICK      15

UINT32 g_curNestCount = 0;
STATIC EXC_PROC_FUNC g_excHook = NULL;
ExcInfo g_excInfo;
UINT8 g_excTbl[FAULT_STATUS_REG_BIT] = {
    0, 0, 0, 0, 0, 0, OS_EXC_UF_DIVBYZERO, OS_EXC_UF_UNALIGNED,
    0, 0, 0, 0, OS_EXC_UF_NOCP, OS_EXC_UF_INVPC, OS_EXC_UF_INVSTATE, OS_EXC_UF_UNDEFINSTR,
    0, 0, 0, OS_EXC_BF_STKERR, OS_EXC_BF_UNSTKERR, OS_EXC_BF_IMPRECISERR, OS_EXC_BF_PRECISERR, OS_EXC_BF_IBUSERR,
    0, 0, 0, OS_EXC_MF_MSTKERR, OS_EXC_MF_MUNSTKERR, 0, OS_EXC_MF_DACCVIOL, OS_EXC_MF_IACCVIOL
};
ExcInfoArray g_excArray[OS_EXC_TYPE_MAX - 1];
STATIC const CHAR *g_phaseName[] = {
    "init",
    "task",
    "interrupt",
};

STATIC INLINE VOID OsExcSysInfo(VOID)
{
    LosTaskCB *runTask = OsCurrTaskGet();

    if (runTask != NULL) {
        PRINT_EXC("TaskName = %s\n"
                  "TaskId = %u\n"
                  "Task stackSize = %u\n"
                  "System mem addr = %p\n",
            runTask->taskName, runTask->taskId,
            runTask->stackSize, m_aucSysMem0);
    }
}

BOOL ArchExcActive(VOID)
{
    UINT32 ipsr = ArchGetIpsr();
    /* 15:SysTick interrupt number and less then this number is exception. */
    return ipsr < ARM_EXCEPTION_SYSTICK && ipsr > 0;
}

LITE_OS_SEC_TEXT_INIT STATIC VOID OsExcInfoDisplay(const ExcInfo *exc, const ExcContext *excBufAddr)
{
    UINT32 i;
    const UINTPTR *reg = NULL;

    PRINT_EXC("Phase\t= fault in %s\n"
              "Type\t= 0x%x\n"
              "FaultAddr\t= 0x%x\n"
              "intNumOrTaskId\t= 0x%x\n",
        g_phaseName[exc->phase], exc->type, exc->faultAddr, exc->intNumOrTaskId);

    reg = &excBufAddr->R0;
    for (i = REG0_IDX; i < REG4_IDX; i++, reg++) {
        PRINT_EXC("R%u\t= 0x%x\n", i, *reg);
    }
    reg = &excBufAddr->R4;
    for (; i < REG12_IDX; i++, reg++) {
        PRINT_EXC("R%u\t= 0x%x\n", i, *reg);
    }
    PRINT_EXC("R12\t= 0x%x\n"
              "PriMask\t= 0x%x\n"
              "SP\t= 0x%x\n"
              "LR\t= 0x%x\n"
              "PC\t= 0x%x\n"
              "xPSR\t= 0x%x\n",
        excBufAddr->R12, excBufAddr->PriMask,
        excBufAddr->SP, excBufAddr->LR, excBufAddr->PC, excBufAddr->xPSR);
}

LITE_OS_SEC_TEXT_INIT VOID OsExcHandleEntry(UINT32 excType, UINT32 faultAddr, UINT32 pid,
                                            const ExcContext *excBufAddr)
{
    UINT16 tmpFlag = (excType >> MASK_16_BIT) & OS_NULL_SHORT; /* 2:in interrupt,1:faul addr valid */
    g_curNestCount++;
    g_excInfo.nestCnt = (UINT16)g_curNestCount;

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
    if (func != NULL) {
        OsSetExcInfoOffset(0);
    }
#endif

    g_excInfo.type = excType & OS_NULL_SHORT;

    if (tmpFlag & OS_EXC_FLAG_FAULTADDR_VALID) {
        g_excInfo.faultAddr = faultAddr;
    } else {
        g_excInfo.faultAddr = OS_EXC_IMPRECISE_ACCESS_ADDR;
    }

    if (ArchCurrTaskGet() != NULL && (((LosTaskCB *)ArchCurrTaskGet())->taskEntry != NULL)) {
        if (tmpFlag & OS_EXC_FLAG_IN_HWI) {
            g_excInfo.phase = OS_EXC_IN_HWI;
            g_excInfo.intNumOrTaskId = pid;
        } else {
            g_excInfo.phase = OS_EXC_IN_TASK;
            g_excInfo.intNumOrTaskId = ((LosTaskCB *)ArchCurrTaskGet())->taskId;
            OsExcSysInfo();
        }
    } else {
        g_excInfo.phase = OS_EXC_IN_INIT;
        g_excInfo.intNumOrTaskId = OS_NULL_INT;
    }

    if (excType & OS_EXC_FLAG_NO_FLOAT) {
        g_excInfo.context = (ExcContext *)((CHAR *)excBufAddr - LOS_OFF_SET_OF(ExcContext, R4));
    } else {
        g_excInfo.context = (ExcContext *)excBufAddr;
    }

    OsExcInfoDisplay((const ExcInfo *)&g_excInfo, g_excInfo.context);

#ifdef LOSCFG_BACKTRACE
    BackTraceWithSp((UINTPTR)g_excInfo.context);
#endif

#ifdef LOSCFG_KERNEL_TRACE
    OsTraceRecordDump(FALSE);
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    if (func != NULL) {
        OsIrqNestingCntSet(0);     /* 0: int nest count */
        func(OsGetExcInfoDumpAddr(), OsGetExcInfoLen(), 0, OsGetExcInfoBuf());
        OsIrqNestingCntSet(1);     /* 1: int nest count */
    }
#endif

    if (g_excHook != NULL) {
        g_excHook(excType, excBufAddr);
    }

#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif

    while (1) { }
}

UINT32 LOS_ExcRegHook(EXC_PROC_FUNC excHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_excHook = excHook;
    ArchIntRestore(intSave);
    return 0;
}

#ifdef LOSCFG_ARCH_INTERRUPT_TAKEOVER
LITE_OS_SEC_TEXT_INIT UINT32 ArchExcInit(VOID)
{
    g_hwiVec[HARD_FAULT_IRQN + OS_SYS_VECTOR_CNT] = OsExcHardFault;
    g_hwiVec[NON_MASKABLE_IRQN + OS_SYS_VECTOR_CNT] = OsExcNMI;
    g_hwiVec[MEMORY_MANAGEMENT_IRQN + OS_SYS_VECTOR_CNT] = OsExcMemFault;
    g_hwiVec[BUSFAULT_IRQN + OS_SYS_VECTOR_CNT] = OsExcBusFault;
    g_hwiVec[USAGEFAULT_IRQN + OS_SYS_VECTOR_CNT] = OsExcUsageFault;
    g_hwiVec[SVCALL_IRQN + OS_SYS_VECTOR_CNT] = OsExcSvcCall;
    /* Enable USGFAULT, BUSFAULT, MEMFAULT */
    *(volatile UINT32 *)OS_NVIC_SHCSR |= (USGFAULT | BUSFAULT | MEMFAULT);
    /* Enable DIV 0 and unaligned exception */
    *(volatile UINT32 *)OS_NVIC_CCR |= DIV0FAULT;
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);
#endif

#ifdef LOSCFG_TRUSTZONE

#define SVC_STACK_SIZE  6
VOID NonSecureSvcHandler(UINT32 *svcStack)
{
    UINT8 svcNum = ((UINT8 *)svcStack[SVC_STACK_SIZE])[-2];     /* -2： index of svc num */
    VOID *allocPtr = NULL;
    UINT32 ret;

    /* svcStack: R0, R1, R2, R3, R12, LR, PC, xPSR */
    switch (svcNum) {
        case SVC_ALLOC_SECURE_CONTEXT:
            allocPtr = (VOID *)ArchSecureAllocContext(svcStack[0]);
            if ((allocPtr != NULL) && (svcStack[1] == TRUE)) {
                ArchSecureLoadContext(allocPtr);
                g_secureContext = allocPtr;
            }
            svcStack[0] = (UINT32)allocPtr;     /* return */
            break;
        case SVC_FREE_SECURE_CONTEXT:
            ret = ArchSecureFreeContext((SecureContext *)svcStack[0]);
            svcStack[0] = ret;                  /* return */
            break;
        case SVC_INIT_SECURE_CONTEXT:
            ArchSecureInitContext();
            break;
        default:
            PRINT_EXC("not supported! svc id:%u\n", svcNum);
            break;
    }
}

#endif /* LOSCFG_TRUSTZONE */
