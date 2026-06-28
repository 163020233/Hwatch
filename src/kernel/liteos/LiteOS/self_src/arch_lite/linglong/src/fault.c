/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LingLong Fault Implementation
 * Author: Huawei LiteOS Team
 * Create: 2021-11-30
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

#include "los_typedef.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_init.h"
#ifdef LOSCFG_KERNEL_SMP
#include "los_mp_pri.h"
#endif
#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif

#include "arch/exception.h"
#include "arch/task.h"

#ifdef LOSCFG_KERNEL_HPL
#include "los_hpl_pri.h"
#endif

#include "los_elf_symbol_pri.h"

static const char *g_excNameString[8] = {
    [OS_EXC_ILLEGAL] = "IllegalException",
    [OS_EXC_PROG_FETCH_ERR] = "ProgramFetchError",
    [OS_EXC_DATA_LOAD_ERR] = "DataLoadError",
    [OS_EXC_DATA_STORE_ERR] = "DataStoreError",
    [OS_EXC_RESERVED4] = "Reserved4",
    [OS_EXC_INT_DIV_ZERO] = "DivByZero",
    [OS_EXC_DEBUG] = "DebugException",
    [OS_EXC_SYSCALL] = "Syscall",
};

/* last NMI/IRQ/EXC return PC, available both in standalone or non-standalone stack mode. */
UINTPTR g_epc[LOSCFG_KERNEL_CORE_NUM] = {0};

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr);
UINT32 g_curNestCount = 0;
STATIC EXC_PROC_FUNC g_excHook = (EXC_PROC_FUNC)OsExcHook;
STATIC NMI_PROC_FUNC g_osNmiHook = NULL;

UINTPTR ArchGetEPC(UINT32 coreId)
{
    return g_epc[coreId];
}

#ifdef LOSCFG_BACKTRACE
STATIC INLINE UINTPTR ArchGetFp(VOID)
{
    UINTPTR fp;
    __asm__ __volatile__(
        "mov.a.ssr %0, fbr"
        : "=r"(fp)
    );
    return fp;
}

/* this function is used to validate fp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidFP(UINTPTR regFP, UINTPTR start, UINTPTR end)
{
    /* NOT includes top of stack: magic word */
    return (regFP > start) && (regFP < end);
}

STATIC INLINE BOOL FindSuitableStack(UINTPTR regFP, UINTPTR *start, UINTPTR *end)
{
    UINT32 index;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;
    const UINTPTR stackAddrTbl[][2] = {  /* 2: store both start and end address */
        {(UINTPTR)&__stack, (UINTPTR)&__stack_top},
    };
    const UINT32 tblSize = sizeof(stackAddrTbl) / sizeof(stackAddrTbl[0]);

    /* Search in the task stacks */
    for (index = 0; index < g_taskMaxNum; index++) {
        taskCB = OS_TCB_FROM_TID(index);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }

        stackStart = taskCB->topOfStack;
        stackEnd = taskCB->topOfStack + taskCB->stackSize;
        if (IsValidFP(regFP, stackStart, stackEnd)) {
            found = TRUE;
            goto FOUND;
        }
    }

    /* Search in standalone stacks */
    for (index = 0; index < tblSize; index++) {
        stackStart = stackAddrTbl[index][0];
        stackEnd = stackAddrTbl[index][1];
        if (IsValidFP(regFP, stackStart, stackEnd)) {
            found = TRUE;
            goto FOUND;
        }
    }

FOUND:
    if (found) {
        *start = stackStart;
        *end = stackEnd;
    }

    return found;
}

STATIC VOID BackTraceWithFp(UINTPTR fp)
{
    PRINT_EXC("*******backtrace begin*******\n");
    (VOID)ArchBackTraceGet(fp, NULL, OS_MAX_BACKTRACE, 0);
    PRINT_EXC("*******backtrace end*******\n");
}
#endif

VOID ArchBackTrace(VOID)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR fp = ArchGetFp();
    PRINT_EXC("fp:0x%08x\n", fp);
    BackTraceWithFp(fp);
#endif
}

VOID ArchBackTraceWithSp(const VOID *stackPointer)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR fp = ArchGetTaskFp(stackPointer);
    PRINT_EXC("fp:0x%08x\n", fp);
    BackTraceWithFp(fp);
#else
    (VOID)stackPointer;
#endif
}

STATIC BOOL StackRangeUpdate(UINTPTR backFP, UINTPTR *stackStart, UINTPTR *stackEnd)
{
    UINTPTR stackStartNew;
    UINTPTR stackEndNew;
    if (FindSuitableStack(backFP, &stackStartNew, &stackEndNew) == TRUE) {
        if (stackStartNew != *stackStart) {
            *stackStart = stackStartNew;
            *stackEnd = stackEndNew;
            return TRUE;
        }
    }
    return FALSE;
}

STATIC INLINE VOID ArchBacktracePcPrint(UINT32 count, UINT32 ignrDepth, UINTPTR backPC, UINTPTR backFP)
{
#ifdef LOSCFG_KERNEL_DYNLOAD
    LDSymbolInfo symInfo;
    if (LD_FindSymByAddr(NULL, backPC, &symInfo) != LOS_OK) {
#endif
        PRINT_EXC("traceback %u -- pc = 0x%x    fp = 0x%x\n", count - ignrDepth, backPC, backFP);
#ifdef LOSCFG_KERNEL_DYNLOAD
    } else {
        PRINT_EXC("traceback %u -- pc = 0x%x    fp = 0x%x    (%s:%s+0x%x)\n",
                  count - ignrDepth, backPC, backFP,
                  symInfo.moduleName, symInfo.symName, backPC - symInfo.symAddr);
    }
#endif
}

UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR tmpFP;
    UINTPTR backPC;
    UINTPTR backFP = fp;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    UINT32 count = 0;

    if (FindSuitableStack(fp, &stackStart, &stackEnd) == FALSE) {
        if (callChain == NULL) {
            PRINT_EXC("fp error, backtrace failed!\n");
        }
        return 0;
    }

    /*
     * Check whether it is the leaf function.
     * Generally, the frame pointer points to the address of link register, while in the leaf function,
     * there's no function call, and compiler will not store the link register, but the frame pointer
     * will still be stored and updated. In that case we needs to find the right position of frame pointer.
     */
    tmpFP = *((UINTPTR *)(fp));
    if (IsValidFP(tmpFP, stackStart, stackEnd)) {
        backFP = tmpFP;
        if (callChain == NULL) {
            PRINT_EXC("traceback fp fixed, trace using   fp = 0x%x\n", backFP);
        }
    }

    while (IsValidFP(backFP, stackStart, stackEnd)) {
        tmpFP = backFP;
        backPC = *((UINTPTR *)(tmpFP));
        backFP = *((UINTPTR *)(tmpFP + sizeof(UINTPTR)));

        if (count >= ignrDepth) {
            if (callChain == NULL) {
                ArchBacktracePcPrint(count, ignrDepth, backPC, backFP);
            } else {
                callChain[count - ignrDepth] = backPC;
            }
        }

        count++;
        if ((count == maxDepth) || (backFP == tmpFP)) {
            break;
        }

        if (StackRangeUpdate(backFP, &stackStart, &stackEnd)) {
            if (callChain == NULL) {
                PRINT_EXC("<irq/exc signal>\n");
            }
        }
    }

    return count;
#else
    (VOID)fp;
    (VOID)callChain;
    (VOID)maxDepth;
    return 0;
#endif
}

UINT32 ArchExcInit(VOID)
{
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);


STATIC INLINE VOID ArchBacktraceGenRegPrint(UINTPTR genRegId, UINTPTR genReg)
{
#ifdef LOSCFG_KERNEL_DYNLOAD
    LDSymbolInfo symInfo;
    if (LD_FindSymByAddr(NULL, genReg, &symInfo) != LOS_OK) {
#endif
        PRINT_EXC("a%d          0x%x\n", genRegId, genReg);
#ifdef LOSCFG_KERNEL_DYNLOAD
    } else {
        PRINT_EXC("a%d          0x%x    (%s:%s+0x%x)\n", genRegId, genReg,
                  symInfo.moduleName, symInfo.symName, genReg - symInfo.symAddr);
    }
#endif
}

LITE_OS_SEC_TEXT_INIT STATIC VOID OsExcInfoDisplayContext(const ExcContext *exc)
{
    PRINT_EXC("type         0x%x\n"
              "flags        0x%x\n"
              "emaddr       0x%x\n"
              "sp           0x%x\n"
              "sr           0x%x\n"
              "fbr          0x%x\n",
        exc->excType, exc->flags, exc->emaddr, exc->sp, exc->sr, exc->fbr);

    for (int i = 0; i < EXC_GEN_REG_NUM; i++) {
        ArchBacktraceGenRegPrint(i, exc->a[i]);
    }
}

LITE_OS_SEC_TEXT_INIT STATIC VOID OsExcInfoDisplay(UINT32 excType, const ExcContext *exc)
{
    /*
     * exc->sp:
     * a4:a7            | 16B
     * a0:a3            | 16B
     * pc sr isr emaddr | 16B
     */
    ExcStack *excStk = (ExcStack *)(UINTPTR)(exc->sp + OS_EXC_STACK_OFFSET);
    if (excType > OS_EXC_MAX) {
        return;
    }

    PRINT_EXC("ExcName          %s\n"
              "ExcSP            0x%x\n"
              "ExcPC            0x%x\n"
              "ExcSR            0x%x\n",
        g_excNameString[excType], (UINTPTR)excStk, excStk->pc, excStk->sr);

#ifdef LOSCFG_KERNEL_DYNLOAD
    LDSymbolInfo symInfo;
    if (LD_FindSymByAddr(NULL, excStk->pc, &symInfo) == LOS_OK) {
        PRINT_EXC("ExcPC in (%s:%s+0x%x)\n",
                  symInfo.moduleName, symInfo.symName,
                  excStk->pc - symInfo.symAddr);
    }
#endif

    OsExcInfoDisplayContext(exc);
}

#ifdef LOSCFG_KERNEL_SMP
STATIC SPIN_LOCK_INIT(g_excSerializerSpin);

#define INVALID_CPUID  0xFFFF
STATIC UINT32 g_excCpuid = INVALID_CPUID;

STATIC VOID CpuStatusOutput(VOID)
{
    UINT32 i;

    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        switch (g_percpu[i].excFlag) {
            case CPU_RUNNING:
                PRINT_EXC("cpu%u is running.\n", i);
                break;
            case CPU_HALT:
                PRINT_EXC("cpu%u is halted.\n", i);
                break;
            case CPU_EXC:
                PRINT_EXC("cpu%u is in exc.\n", i);
                break;
            default:
                break;
        }
    }
}

STATIC VOID WaitAllCpuStop(UINT32 cpuid)
{
    UINT32 i;

    do {
        for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
            if ((i != cpuid) && (g_percpu[i].excFlag == CPU_RUNNING)) {
                break;
            }
        }
    } while (i != LOSCFG_KERNEL_CORE_NUM);

    return;
}

STATIC VOID CheckAllCpuStatus(VOID)
{
    UINT32 currCpuid = ArchCurrCpuid();

    LOS_SpinLock(&g_excSerializerSpin);
    if (g_excCpuid == INVALID_CPUID) {
        g_excCpuid = ArchCurrCpuid();
    } else if (g_excCpuid != ArchCurrCpuid()) {
        LOS_SpinUnlock(&g_excSerializerSpin);
        while (1) {}
    }
    LOS_SpinUnlock(&g_excSerializerSpin);

    WaitAllCpuStop(currCpuid);
    CpuStatusOutput();
}
#endif

VOID OsCallStackInfo(VOID)
{
#ifdef LOSCFG_BACKTRACE
    LosTaskCB *runTask = OsCurrTaskGet();
    UINTPTR fp = ArchGetTaskFp(runTask->stackPointer);
    BackTraceWithFp(fp);
#endif
}

LITE_OS_SEC_TEXT_INIT VOID OsExcHandleEntry(UINT32 excType, ExcContext *excBufAddr)
{
#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret;

    /* use halt ipi to stop other active cores */
    UINT32 target = (UINT32)(OS_MP_CPU_ALL & ~CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
    ret = HalIrqSendIpi(target, LOS_MP_IPI_HALT);
    if (ret != LOS_OK) {
        PRINT_EXC("The interrupt %d is invalid, irq send inter-core interrupt failed.\n", LOS_MP_IPI_HALT);
    }

    OsPercpuGet()->excFlag = CPU_EXC;
    LOCKDEP_CLEAR_LOCKS();
    CheckAllCpuStatus();
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
#endif

    g_curNestCount++;

    if (g_excHook != NULL) {
        if (g_curNestCount == 1) {
#ifdef LOSCFG_SHELL_EXCINFO_DUMP
            if (func != NULL) {
                OsSetExcInfoOffset(0);
            }
#endif
            g_excHook(excType, excBufAddr);
        } else {
            OsCallStackInfo();
        }

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
        if (func != NULL) {
            PRINT_EXC("Be sure your space bigger than 0x%x\n", OsGetExcInfoOffset());
            OsIrqNestingCntSet(0);     /* 0: int nest count */
            func(OsGetExcInfoDumpAddr(), OsGetExcInfoLen(), 0, OsGetExcInfoBuf());
            OsIrqNestingCntSet(1);     /* 1: int nest count */
        }
#endif
    }
#ifdef LOSCFG_EXC_INTERACTION
    OsKeepExcInteractionTask();
#endif

#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif

    while (1) {
        /* Exception cannot return, wait here */
    }
}

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr)
{
    OsExcInfoDisplay(excType, excBufAddr);

#ifdef LOSCFG_BACKTRACE
    BackTraceWithFp(excBufAddr->fbr);
#endif
#ifndef LOSCFG_EXC_SIMPLE_INFO
    (VOID)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
#endif

#ifdef LOSCFG_KERNEL_TRACE
    if (g_traceDumpHook != NULL) {
        g_traceDumpHook(FALSE);
    }
#endif
}

UINT32 ArchSetExcHook(EXC_PROC_FUNC excHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_excHook = excHook;
    ArchIntRestore(intSave);
    return 0;
}

BOOL ArchExcActive(VOID)
{
    return FALSE;
}

EXC_PROC_FUNC ArchGetExcHook(VOID)
{
    return g_excHook;
}

UINT32 ArchSetNMIHook(NMI_PROC_FUNC nmiHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_osNmiHook = nmiHook;
    ArchIntRestore(intSave);
    return 0;
}

NMI_PROC_FUNC ArchGetNMIHook(VOID)
{
    return g_osNmiHook;
}

LITE_OS_SEC_TEXT VOID OsNMIHandler(UINT32 excType, const VOID *excBufAddr)
{
    PRINT_ERR("NMI cause:%u\n", excType);
    if (g_osNmiHook != NULL) {
        g_osNmiHook(excBufAddr);
    }
}
