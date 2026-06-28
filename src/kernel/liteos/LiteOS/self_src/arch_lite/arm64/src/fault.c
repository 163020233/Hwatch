/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Aarch64 Exc Implementation
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
#include "arch/regs.h"

#include "los_init.h"
#include "los_atomic.h"
#include "los_memory_pri.h"
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_exc_pri.h"
#include "los_stackinfo_pri.h"
#include "los_mp_pri.h"

#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif

#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
#include "console_pri.h"
#endif

#ifdef LOSCFG_KERNEL_HPL
#include "los_hpl_pri.h"
#endif

#ifdef LOSCFG_LIB_CONFIGURABLE
UINTPTR g_irqStackTop = (UINTPTR)(&__irq_stack_top);
#endif

STATIC EXC_PROC_FUNC g_excHook = (EXC_PROC_FUNC)OsExcHook;
UINT32 g_curNestCount = 0;
UINT32 g_percpuNestCount[LOSCFG_KERNEL_CORE_NUM];

#define OS_MAX_BACKTRACE 15
#define DUMPSIZE         128U
#define DUMPREGS         30
/* get the 26-31bit for EC */
#define EC_END_BIT       31
#define EC_START_BIT     26

#define NBIT(val, high, low) (((val) >> (low)) & ((1U << (((high) - (low)) + 1)) - 1))
#define GET_IL(esr) ((esr) & (1U << 25))
#define IS_VALID_ADDR(ptr) (((ptr) >= SYS_MEM_BASE) &&       \
                            ((ptr) <= g_sys_mem_addr_end) && \
                            IS_ALIGNED((ptr), sizeof(CHAR *)))

VOID OsDecodeDataAbortISS(UINT32 bitsISS)
{
    UINT32 bitFnV = bitsISS & (1U << 10);   /* FnV bit[10] */
    UINT32 bitWnR = bitsISS & (1U << 6);    /* WnR bit[6] */
    UINT32 bitsDFSC = NBIT(bitsISS, 5, 0);  /* DFSC bits[5:0] */

    if (!bitFnV) {
        if (bitWnR) {
            PRINT_EXC("Abort caused by a write instruction.");
        } else {
            PRINT_EXC("Abort caused by a read instruction.");
        }
        switch (bitsDFSC) {
            case 0x21:  /* 0b100001 */
                PRINT_EXC("Alignment fault.\n");
                break;
            case 0x0:   /* 0b000000 */
            case 0x01:  /* 0b000001 */
            case 0x03:  /* 0b000011 */
            case 0x04:  /* 0b000100 */
                PRINT_EXC("Address size fault.\n");
                break;
            default:
                PRINT_EXC("\nOMG!UNKNOWN fault, "
                    "check ISS encoding for an exception from a Data Abort in AARM for armv8-a.\n");
                break;
        }
    } else {
        PRINT_EXC("FAR is not valid, and holds an UNKNOWN value.\n");
    }
}

STATIC VOID OsExcType(const ExcContext *frame, UINT32 regESR, UINT32 bitsEC)
{
    UINT32 bitIL = GET_IL(regESR);
    UINT32 bitsISS = NBIT(regESR, 24, 0); /* Instruction Specific Syndrome in ESR_ELx[24:0] */
    UINT64 regFAR;

    switch (bitsEC) {
        case OS_EXC_DABT_LOW:
        case OS_EXC_DABT_CUR:
            PRINT_EXC("Data Abort.\n");
            /* read the FAR register */
            regFAR = AARCH64_SYSREG_READ(FAR_ELx);
            /* decode the iss */
            if (bitIL) {
                OsDecodeDataAbortISS(bitsISS);
            }
            PRINT_EXC("PC at 0x%llx,FAR 0x%llx,ISS 0x%x\n", frame->regELR, regFAR, bitsISS);
            break;
        case OS_EXC_IABT_LOW:
        case OS_EXC_IABT_CUR:
            PRINT_EXC("Instruction Abort.\n");
            break;
        case OS_EXC_PC_ALIGN:
            PRINT_EXC("PC alignment fault.\n");
            break;
        case OS_EXC_SP_ALIGN:
            PRINT_EXC("SP alignment fault.\n");
            break;
        default:
            PRINT_EXC("OMG!Unknown synchronous exception, "
                "check Exception Syndrome Register in AARM for armv8-a.\n");
            break;
    }
    PRINT_EXC("ESR 0x%x: ec 0x%x, il 0x%x, iss 0x%x\n", regESR, bitsEC, bitIL, bitsISS);
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

#ifdef LOSCFG_BACKTRACE
STATIC VOID BackTraceWithFp(UINTPTR fp)
{
    PRINT_EXC("*******backtrace begin*******\n");
    (VOID)ArchBackTraceGet(fp, NULL, OS_MAX_BACKTRACE, 0);
    PRINT_EXC("*******backtrace end*******\n");
}
#endif

UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR tmpFp;
    UINTPTR backLr;
    UINTPTR backFp = fp;
    UINT32 count;

    for (count = 0; count < maxDepth; count++) {
        if ((backFp <= OS_SYS_FUNC_ADDR_START) || (backFp >= OS_SYS_FUNC_ADDR_END)) {
            break;
        }
        tmpFp = backFp;
        backFp = *((UINTPTR *)(tmpFp));
        backLr = *((UINTPTR *)(tmpFp + sizeof(CHAR *)));

        if (count < ignrDepth) {
            continue;
        }

        if (callChain == NULL) {
            PRINT_EXC("traceback %u -- lr = 0x%llx    fp = 0x%llx\n", count - ignrDepth, backLr, backFp);
        } else {
            callChain[count - ignrDepth] = backLr;
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

VOID OsCallStackInfo(VOID)
{
    UINT32 count = 0;
    LosTaskCB *runTask = OsCurrTaskGet();

    if (runTask == NULL) {
        PRINT_EXC("runTask NULL.\n");
        return;
    }

    UINTPTR *stackBottom = (UINTPTR *)(runTask->topOfStack + runTask->stackSize);
    UINTPTR *stackPointer = stackBottom - 1;

    PRINT_EXC("runTask->stackPointer = 0x%llx\n"
        "runTask->topOfStack = 0x%llx\n",
        stackPointer, runTask->topOfStack);

    while ((stackPointer > (UINTPTR *)runTask->topOfStack) && (count < OS_MAX_BACKTRACE)) {
        if ((*stackPointer > (UINTPTR)(&__text_start)) &&
            (*stackPointer < (UINTPTR)(&__text_end)) &&
            IS_ALIGNED(*stackPointer, sizeof(CHAR *))) {
            if ((*(stackPointer - 1) > (UINTPTR)runTask->topOfStack) &&
                (*(stackPointer - 1) < (UINTPTR)stackBottom) &&
                IS_ALIGNED(*(stackPointer - 1), sizeof(CHAR *))) {
                count++;
                PRINT_EXC("traceback %u -- lr = 0x%llx\n", count, *stackPointer);
            }
        }
        stackPointer--;
    }
    PRINT_EXC("\n");
}

#ifdef LOSCFG_DEBUG_VERSION
#define IS_VALID_RANGE(addr, size) (IS_VALID_ADDR((UINTPTR)(addr)) && \
                                    IS_VALID_ADDR((UINTPTR)(addr) + (size)))

STATIC VOID OsDumpContextMem(const ExcContext *excBufAddr)
{
    UINT32 count = 0;
    const UINT64 *excReg = NULL;

    for (excReg = &(excBufAddr->X[0]); count <= (DUMPREGS - 1); excReg++, count++) {
        if (IS_VALID_RANGE(((*excReg) - (DUMPSIZE >> 1)), DUMPSIZE)) {
            PRINT_EXC("\ndump mem around X%u:0x%llx", count, (*excReg));
            OsDumpMemByte(DUMPSIZE, ((*excReg) - (DUMPSIZE >> 1)));
        }
    }

    if (IS_VALID_RANGE((excBufAddr->SP - (DUMPSIZE >> 1)), DUMPSIZE)) {
        PRINT_EXC("\ndump mem around SP:%p", excBufAddr->SP);
        OsDumpMemByte(DUMPSIZE, (excBufAddr->SP - (DUMPSIZE >> 1)));
    }
}
#endif

STATIC const StackInfo g_excStack[] = {
    {&__startup_stack, OS_EXC_START_STACK_SIZE, "start_stack"},
#ifdef LOSCFG_IRQ_USE_STANDALONE_STACK
    {&__irq_stack, OS_EXC_IRQ_STACK_SIZE, "irq_stack"}
#endif
};

UINT32 ArchExcInit(VOID)
{
    OsExcStackInfoReg(g_excStack, sizeof(g_excStack) / sizeof(g_excStack[0]));
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);

VOID OsExcDumpContext(const ExcContext *excBufAddr)
{
    UINT32 i;
    LosTaskCB *runTask = OsCurrTaskGet();

    PRINT_EXC("taskName = %s\nTaskId = %u\nTask StackSize = %u\n"
        "system mem addr = %p\nPC = 0x%llx\nLR = 0x%llx\n",
        runTask->taskName, runTask->taskId, runTask->stackSize,
        m_aucSysMem0, excBufAddr->regELR, excBufAddr->LR);

    for (i = 0; i < DUMPREGS; i++) {
        PRINT_EXC("X%-02u = 0x%llx\n", i, excBufAddr->X[i]);
    }

    PRINT_EXC("LR = 0x%llx\nELR = 0x%llx\nSPSR = 0x%llx\n",
        excBufAddr->LR, excBufAddr->regELR, excBufAddr->SPSR);

#ifdef LOSCFG_BACKTRACE
    BackTraceWithFp(excBufAddr->X[29]);  /* 29: fp reg num */
#endif
#ifndef LOSCFG_EXC_SIMPLE_INFO
    (VOID)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
#endif
#ifdef LOSCFG_DEBUG_VERSION
    OsDumpContextMem(excBufAddr);
#endif
#ifdef LOSCFG_KERNEL_MEM_BESTFIT
    OsMemIntegrityMultiCheck();
#endif
#ifdef LOSCFG_KERNEL_TRACE
    if (g_traceDumpHook != NULL) {
        g_traceDumpHook(FALSE);
    }
#endif
}

VOID OsCurrentELGet(UINT32 reg)
{
    PRINT_EXC("The current exception level is EL%u\n", NBIT(reg, 3, 2));
}

STATIC Atomic g_excHookNestCnt = 0;
VOID OsExcHook(UINT32 regESR, const ExcContext *excBufAddr)
{
    if (LOS_AtomicCmpXchg32bits(&g_excHookNestCnt, 1, 0)) {
        return;
    }

    UINT32 bitsEC = NBIT(regESR, EC_END_BIT, EC_START_BIT);
    UINT32 cpuId = ArchCurrCpuid();

#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    OsConsoleDisable();
#endif

    PRINT_EXC("/*******************Exception Information*******core:%u************/"
            "\nExcClass:0x%x => ", cpuId, bitsEC);
    OsExcType(excBufAddr, regESR, bitsEC);
    OsExcDumpContext(excBufAddr);
    PRINT_EXC("/*******************Exception Information END*******core:%u**********/\n", cpuId);
}

#ifdef LOSCFG_ARCH_EXC_RECOVER
STATIC EXC_RECOVER_FUNC g_excRecover = NULL;
STATIC BOOL IsExcRecoverable(UINT32 regESR, const ExcContext *frame)
{
    ExcAddrDesc fault;
    EXC_RECOVER_FUNC excRecoverHook = g_excRecover;

    fault.regFAR = AARCH64_SYSREG_READ(FAR_ELx);
    fault.regESR = regESR;

    if ((excRecoverHook != NULL) && (excRecoverHook(&fault, frame) == OS_EXC_RECOVERABLE)) {
        return TRUE;
    }
    return FALSE;
}

UINT32 ArchExcRecoverHookSet(EXC_RECOVER_FUNC excHook)
{
    g_excRecover = excHook;

    return LOS_OK;
}
#endif

WEAK VOID OsSmcHandler(const ExcContext *frame)
{
    (VOID)frame;
    return;
}

VOID OsExceptSyncLowElHdl(const ExcContext *frame, UINT64 syncExcType)
{
    UINT32 ec = (syncExcType >> 26) & 0x1F;
    if (ec == 0b10111) { /* sync type: smc */
        OsSmcHandler(frame);
    } else {
        for (;;) {
            ;
        }
    }
}

#ifndef ARCH_EXC_WAIT_TIME_NS_MAX
#define ARCH_EXC_WAIT_TIME_NS_MAX           (100 * 1000 * 1000ULL)  /* 100ms */
#endif

#define ARCH_EXC_COUNT_FIRST                1
#define ARCH_EXC_COUNT_SECOND               2

#ifdef LOSCFG_KERNEL_SMP
STATIC VOID ArchDumpCpuStatus(VOID)
{
    for (UINT32 cpuid = 0; cpuid < LOSCFG_KERNEL_CORE_NUM; cpuid++) {
        PRINT_EXC("cpu%u status: %u\n", cpuid, OsPercpuGetByID(cpuid)->excFlag);
    }
}

STATIC VOID ArchWaitAllCoresHalt(VOID)
{
    BOOL allCoreHalt = FALSE;
    UINT64 expireTime = LOS_CurrNanosec() + ARCH_EXC_WAIT_TIME_NS_MAX;
    while (!allCoreHalt && (LOS_CurrNanosec() < expireTime)) {
        allCoreHalt = TRUE;
        for (UINT32 cpuid = 0; cpuid < LOSCFG_KERNEL_CORE_NUM; cpuid++) {
            if (OsPercpuGetByID(cpuid)->excFlag == CPU_RUNNING) {
                allCoreHalt = FALSE;
                break;
            }
        }
    }
    if (!allCoreHalt) {
        PRINT_EXC("Wait all cpu halt timeout!\n");
    }
}
#endif

VOID OsExceptSyncExcHdl(const ExcContext *frame)
{
    UINT32 regESR = (UINT32)AARCH64_SYSREG_READ(ESR_ELx);
#ifdef LOSCFG_ARCH_EXC_RECOVER
    if (IsExcRecoverable(regESR, frame) == TRUE) {
        return;
    }
#endif

#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret;

    /* use halt ipi to stop other active cores */
    UINT32 target = (UINT32)(OS_MP_CPU_ALL & ~CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
    ret = HalIrqSendIpi(target, LOS_MP_IPI_HALT);
    if (ret != LOS_OK) {
        PRINT_EXC("The interrupt %d is invalid, irq send inter-core interrupt failed.\n", (INT32)LOS_MP_IPI_HALT);
    }
    OsPercpuGet()->excFlag = CPU_EXC;
    ArchWaitAllCoresHalt();
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
#endif

    INT32 oldNestCnt;
    do {
        oldNestCnt = LOS_AtomicRead((Atomic *)&g_curNestCount);
    } while (LOS_AtomicCmpXchg32bits((Atomic *)&g_curNestCount, oldNestCnt + 1, oldNestCnt));

    if (oldNestCnt == 0) {
#ifdef LOSCFG_KERNEL_SMP
        ArchDumpCpuStatus();
#endif
#ifdef LOSCFG_SHELL_EXCINFO_DUMP
        if (func != NULL) {
            OsSetExcInfoOffset(0);
        }
#endif
    }

    g_percpuNestCount[ArchCurrCpuid()]++;
    if (g_percpuNestCount[ArchCurrCpuid()] == ARCH_EXC_COUNT_FIRST) {
        if (g_excHook != NULL) {
            g_excHook(regESR, frame);
        }
    } else if (g_percpuNestCount[ArchCurrCpuid()] == ARCH_EXC_COUNT_SECOND) {
        OsCallStackInfo();
    } else {
        goto HALT_CPU;
    }

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    if (func != NULL) {
        PRINT_EXC("Be sure your space bigger than 0x%x\n", OsGetExcInfoOffset());
        func(OsGetExcInfoDumpAddr(), OsGetExcInfoLen(), 0, OsGetExcInfoBuf());
    }
#endif
#ifdef LOSCFG_EXC_INTERACTION
    OsKeepExcInteractionTask();
#endif

#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif

HALT_CPU:
    for (;;) {
        ;
    }
}
