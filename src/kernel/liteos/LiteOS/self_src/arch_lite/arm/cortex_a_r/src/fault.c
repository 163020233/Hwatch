/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: ARMv7 Exc Implementation
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
#include "los_printf_pri.h"
#include "los_task_pri.h"
#include "los_init.h"
#if defined(LOSCFG_SHELL_EXCINFO_DUMP) || defined(LOSCFG_EXC_INTERACTION)
#include "los_exc_pri.h"
#include "los_hwi_pri.h"
#endif
#ifdef LOSCFG_COREDUMP
#include "los_coredump.h"
#endif
#ifdef LOSCFG_GDB
#include "gdb_int.h"
#endif
#include "los_mp_pri.h"

#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif
#ifdef LOSCFG_SHELL
#include "shell_pri.h"
#endif
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
#include "console_pri.h"
#endif
#ifdef LOSCFG_KERNEL_HPL
#include "los_hpl_pri.h"
#endif

#ifdef LOSCFG_LIB_CONFIGURABLE
    UINTPTR g_svcStackTop = (UINTPTR)(&__svc_stack_top);
    UINTPTR g_excStackTop = (UINTPTR)(&__exc_stack_top);
#endif

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr);
UINT32 g_curNestCount = 0;
STATIC EXC_PROC_FUNC g_excHook = (EXC_PROC_FUNC)OsExcHook;
#ifdef LOSCFG_KERNEL_SMP
STATIC SPIN_LOCK_INIT(g_excSerializerSpin);
#endif

#define OS_MAX_BACKTRACE 15U
#define INSTR_SET_MASK   0x01000020U
#define THUMB_INSTR_LEN  2U
#define ARM_INSTR_LEN    4U
#define POINTER_SIZE     4U

#define GET_FS(fsr) (((fsr) & 0xFU) | (((fsr) & (1U << 10)) >> 6))
#define GET_WNR(dfsr) ((dfsr) & (1U << 11))

STATIC const StackInfo g_excStack[] = {
    { &__undef_stack, OS_EXC_UNDEF_STACK_SIZE, "udf_stack" },
    { &__abt_stack,   OS_EXC_ABT_STACK_SIZE,   "abt_stack" },
    { &__fiq_stack,   OS_EXC_FIQ_STACK_SIZE,   "fiq_stack" },
    { &__svc_stack,   OS_EXC_SVC_STACK_SIZE,   "svc_stack" },
    { &__irq_stack,   OS_EXC_IRQ_STACK_SIZE,   "irq_stack" },
    { &__exc_stack,   OS_EXC_STACK_SIZE,       "exc_stack" }
};

STATIC INT32 OsDecodeFS(UINT32 bitsFS)
{
    switch (bitsFS) {
        case 0x05:  /* 0b00101 */
        case 0x07:  /* 0b00111 */
            PRINT_EXC("Translation fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        case 0x09:  /* 0b01001 */
        case 0x0b:  /* 0b01011 */
            PRINT_EXC("Domain fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        case 0x0d:  /* 0b01101 */
        case 0x0f:  /* 0b01111 */
            PRINT_EXC("Permission fault, %s\n", (bitsFS & 0x2) ? "page" : "section");
            break;
        default:
            PRINT_EXC("Unknown fault! FS:0x%x. "
                "Check IFSR and DFSR in ARM Architecture Reference Manual.\n",
                bitsFS);
            break;
    }

    return LOS_OK;
}

STATIC INT32 OsDecodeInstructionFSR(UINT32 regIFSR)
{
    INT32 ret;
    UINT32 bitsFS = GET_FS(regIFSR); /* FS bits[4]+[3:0] */

    ret = OsDecodeFS(bitsFS);
    return ret;
}

STATIC INT32 OsDecodeDataFSR(UINT32 regDFSR)
{
    INT32 ret = 0;
    UINT32 bitWnR = GET_WNR(regDFSR); /* WnR bit[11] */
    UINT32 bitsFS = GET_FS(regDFSR);  /* FS bits[4]+[3:0] */

    if (bitWnR) {
        PRINT_EXC("Abort caused by a write instruction. ");
    } else {
        PRINT_EXC("Abort caused by a read instruction. ");
    }

    if (bitsFS == 0x01) { /* 0b00001 */
        PRINT_EXC("Alignment fault.\n");
        return ret;
    }
    ret = OsDecodeFS(bitsFS);
    return ret;
}

STATIC VOID OsExcUpdatePC(UINT32 excType, ExcContext *excBufAddr)
{
    /* undefinited exception handling or software interrupt */
    if ((excType == OS_EXCEPT_UNDEF_INSTR) || (excType == OS_EXCEPT_SWI)) {
        if ((excBufAddr->regCPSR & INSTR_SET_MASK) == 0) { /* work status: ARM */
            excBufAddr->PC = excBufAddr->PC - ARM_INSTR_LEN;
        } else if ((excBufAddr->regCPSR & INSTR_SET_MASK) == 0x20) { /* work status: Thumb */
            excBufAddr->PC = excBufAddr->PC - THUMB_INSTR_LEN;
        }
    }
}

STATIC VOID OsExcType(UINT32 excType, const ExcContext *excBufAddr)
{
    if (excType == OS_EXCEPT_PREFETCH_ABORT) {
        PRINT_EXC("prefetch_abort fault fsr:0x%x, far:0x%0+8x\n", OsGetIFSR(), OsGetIFAR());
        (VOID)OsDecodeInstructionFSR(OsGetIFSR());
    } else if (excType == OS_EXCEPT_DATA_ABORT) {
        PRINT_EXC("data_abort fsr:0x%x, far:0x%0+8x\n", OsGetDFSR(), OsGetDFAR());
        (VOID)OsDecodeDataFSR(OsGetDFSR());
    }
}

STATIC const CHAR *g_excTypeString[] = {
    "reset",
    "undefined instruction",
    "software interrupt",
    "prefetch abort",
    "data abort",
    "fiq",
    "address abort",
    "irq"
};

STATIC VOID OsExcSysInfo(UINT32 excType, const ExcContext *excBufAddr)
{
    LosTaskCB *runTask = OsCurrTaskGet();

    PRINT_EXC("excType:%s\n"
              "taskName = %s\n"
              "taskId = %u\n"
              "task stackSize = %u\n"
              "system mem addr = %p\n"
              "excBuffAddr pc = 0x%x\n"
              "excBuffAddr lr = 0x%x\n"
              "excBuffAddr sp = 0x%x\n"
              "excBuffAddr fp = 0x%x\n",
        g_excTypeString[excType], runTask->taskName, runTask->taskId, runTask->stackSize,
        OS_SYS_MEM_ADDR, excBufAddr->PC, excBufAddr->LR, excBufAddr->SP, excBufAddr->R11);
}

STATIC VOID OsExcRegsInfo(const ExcContext *excBufAddr)
{
    /*
     * Split register information into two parts:
     * Ensure printing does not rely on memory modules.
     */
    PRINT_EXC("R0         = 0x%x\n"
              "R1         = 0x%x\n"
              "R2         = 0x%x\n"
              "R3         = 0x%x\n"
              "R4         = 0x%x\n"
              "R5         = 0x%x\n"
              "R6         = 0x%x\n",
        excBufAddr->R0, excBufAddr->R1, excBufAddr->R2, excBufAddr->R3,
        excBufAddr->R4, excBufAddr->R5, excBufAddr->R6);
    PRINT_EXC("R7         = 0x%x\n"
              "R8         = 0x%x\n"
              "R9         = 0x%x\n"
              "R10        = 0x%x\n"
              "R11        = 0x%x\n"
              "R12        = 0x%x\n"
              "CPSR       = 0x%x\n",
        excBufAddr->R7, excBufAddr->R8, excBufAddr->R9, excBufAddr->R10,
        excBufAddr->R11, excBufAddr->R12, excBufAddr->regCPSR);
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

#ifdef LOSCFG_DEBUG_VERSION
#define DUMPSIZE                   128U
#define DUMPREGS                   12U
#define IS_VALID_ADDR(ptr)         (((ptr) >= SYS_MEM_BASE) &&       \
                                    ((ptr) <= g_sys_mem_addr_end) && \
                                    IS_ALIGNED((ptr), sizeof(CHAR *)))
#define IS_VALID_RANGE(addr, size) (IS_VALID_ADDR((UINTPTR)(addr)) && \
                                    IS_VALID_ADDR((UINTPTR)(addr) + (size)))

STATIC VOID OsDumpContextMem(const ExcContext *excBufAddr)
{
    UINT32 count = 0;
    const UINT32 *excReg = NULL;

    for (excReg = &(excBufAddr->R0); count <= DUMPREGS; excReg++, count++) {
        if (IS_VALID_RANGE(((*excReg) - (DUMPSIZE >> 1)), DUMPSIZE)) {
            PRINT_EXC("\ndump mem around R%u:%u", count, (*excReg));
            OsDumpMemByte(DUMPSIZE, ((*excReg) - (DUMPSIZE >> 1)));
        }
    }

    if (IS_VALID_RANGE((excBufAddr->SP - (DUMPSIZE >> 1)), DUMPSIZE)) {
        PRINT_EXC("\ndump mem around SP:%p", excBufAddr->SP);
        OsDumpMemByte(DUMPSIZE, (excBufAddr->SP - (DUMPSIZE >> 1)));
    }
}
#endif

#if defined(LOSCFG_BACKTRACE) && !defined(LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION)
/* this function is used to validate fp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidFP(UINTPTR regFP, UINTPTR start, UINTPTR end)
{
    return (regFP > start) && (regFP < end);
}

STATIC INLINE BOOL FindSuitableStack(UINTPTR regFP, UINTPTR *start, UINTPTR *end)
{
    UINT32 index;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;
    const StackInfo *stack = NULL;

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

    /* Search in the exc stacks */
    for (index = 0; index < sizeof(g_excStack) / sizeof(StackInfo); index++) {
        stack = &g_excStack[index];
        stackStart = (UINTPTR)stack->stackTop;
        stackEnd = stackStart + LOSCFG_KERNEL_CORE_NUM * stack->stackSize;
        if (IsValidFP(regFP, stackStart, stackEnd)) {
            found = TRUE;
            goto FOUND;
        }
    }

FOUND:
    if (found == TRUE) {
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

#ifndef LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION
UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR tmpFP;
    UINTPTR backLR;
    UINTPTR backFP = fp;
    UINTPTR stackStart, stackEnd;
    UINT32 count;

    if (FindSuitableStack(fp, &stackStart, &stackEnd) == FALSE) {
        PRINT_EXC("fp error, backtrace failed!\n");
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

    for (count = 0; count < maxDepth; count++) {
        if (!IsValidFP(backFP, stackStart, stackEnd)) {
            break;
        }
        tmpFP = backFP;
        backLR = *((UINTPTR *)(tmpFP));
        backFP = *((UINTPTR *)(tmpFP - POINTER_SIZE));

        if (count < ignrDepth) {
            continue;
        }

        if (callChain == NULL) {
            PRINT_EXC("traceback %u -- lr = 0x%x    fp = 0x%x\n", count - ignrDepth, backLR, backFP);
        } else {
            callChain[count - ignrDepth] = backLR;
        }

        if (backFP == tmpFP) {
            break;
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
#endif

UINT32 ArchExcInit(VOID)
{
    OsExcStackInfoReg(g_excStack, sizeof(g_excStack) / sizeof(g_excStack[0]));
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr)
{
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    OsConsoleDisable();
#endif

    OsExcType(excType, excBufAddr);
    OsExcSysInfo(excType, excBufAddr);
    OsExcRegsInfo(excBufAddr);

#ifdef LOSCFG_BACKTRACE
#ifdef LOSCFG_ARCH_ARM_WITH_THUMB_INSTRUCTION
    BackTraceWithSp(excBufAddr->SP);
#else
    BackTraceWithFp(excBufAddr->R11);
#endif
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

#ifdef LOSCFG_COREDUMP
    LOS_CoreDumpV2(excType, excBufAddr);
#endif
}

VOID OsCallStackInfo(VOID)
{
    UINT32 count = 0;
    LosTaskCB *runTask = OsCurrTaskGet();
    UINTPTR stackBottom = runTask->topOfStack + runTask->stackSize;
    UINT32 *stackPointer = (UINT32 *)stackBottom - 1;

    PRINT_EXC("runTask->stackPointer = %p\n"
        "runTask->topOfStack = 0x%lx\n",
        stackPointer, runTask->topOfStack);

    while ((stackPointer > (UINT32 *)runTask->topOfStack) && (count < OS_MAX_BACKTRACE)) {
        if ((*stackPointer > (UINTPTR)(&__text_start)) &&
            (*stackPointer < (UINTPTR)(&__text_end)) &&
            IS_ALIGNED((*stackPointer), POINTER_SIZE)) {
            if ((*(stackPointer - 1) > (UINT32)runTask->topOfStack) &&
                (*(stackPointer - 1) < stackBottom) &&
                IS_ALIGNED((*(stackPointer - 1)), POINTER_SIZE)) {
                count++;
                PRINT_EXC("traceback %u -- lr = 0x%x\n", count, *stackPointer);
            }
        }
        stackPointer--;
    }
    PRINT_EXC("\n");
}

#ifdef LOSCFG_GDB
VOID OsUndefIncExcHandleEntry(ExcContext *excBufAddr)
{
    excBufAddr->PC -= 4;  /* lr in undef is pc + 4 */
    if (gdb_undef_hook(excBufAddr, OS_EXCEPT_UNDEF_INSTR)) {
        return;
    }

    OsExcUpdatePC(OS_EXCEPT_UNDEF_INSTR, excBufAddr);
    if (g_excHook != NULL) {
        g_excHook(OS_EXCEPT_UNDEF_INSTR, excBufAddr);
    }
#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif
    while (1) {
        ;
    }
}

#if __LINUX_ARM_ARCH__ >= 7
VOID OsPrefetchAbortExcHandleEntry(ExcContext *excBufAddr)
{
    excBufAddr->PC -= 4;  /* lr in prefetch abort is pc + 4 */
    if (gdbhw_hook(excBufAddr, OS_EXCEPT_PREFETCH_ABORT)) {
        return;
    }

    if (g_excHook != NULL) {
        g_excHook(OS_EXCEPT_PREFETCH_ABORT, excBufAddr);
    }

#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif
    while (1) {
        ;
    }
}

VOID OsDataAbortExcHandleEntry(ExcContext *excBufAddr)
{
    excBufAddr->PC -= 8;  /* lr in data abort is pc + 8 */
    if (gdbhw_hook(excBufAddr, OS_EXCEPT_DATA_ABORT)) {
        return;
    }

    if (g_excHook != NULL) {
        g_excHook(OS_EXCEPT_DATA_ABORT, excBufAddr);
    }
#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif
    while (1) {
        ;
    }
}
#endif /* __LINUX_ARM_ARCH__ */
#endif /* LOSCFG_GDB */

#ifdef LOSCFG_KERNEL_SMP
#define EXC_WAIT_INTER 50U
#define EXC_WAIT_TIME  2000U
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
    UINT32 time = 0;

    while (time < EXC_WAIT_TIME) {
        for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
            if ((i != cpuid) && (g_percpu[i].excFlag == CPU_RUNNING)) {
                LOS_Mdelay(EXC_WAIT_INTER);
                time += EXC_WAIT_INTER;
                break;
            }
        }
        /* Other CPUs are all haletd or in the exc. */
        if (i == LOSCFG_KERNEL_CORE_NUM) {
            break;
        }
    }
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

STATIC VOID HaltOtherCores(VOID)
{
    UINT32 ret;
    UINT32 target;
    /* use halt ipi to stop other active cores */
    target = (UINT32)(OS_MP_CPU_ALL & ~CPUID_TO_AFFI_MASK(ArchCurrCpuid()));
    ret = HalIrqSendIpi(target, LOS_MP_IPI_HALT);
    if (ret != LOS_OK) {
        PRINT_EXC("The interrupt %d is invalid, irq send inter-core interrupt failed.\n", (INT32)LOS_MP_IPI_HALT);
    }
}
#endif

/*
 * Description : EXC handler entry
 * Input       : excType    --- exc type
 *               excBufAddr --- address of EXC buf
 */
LITE_OS_SEC_TEXT_INIT VOID OsExcHandleEntry(UINT32 excType, ExcContext *excBufAddr)
{
#ifdef LOSCFG_KERNEL_SMP
#ifdef LOSCFG_EXC_INTERACTION
    UINT32 i;
    UINT32 taskId;
    LosTaskCB *taskCB = NULL;
    /* wait 10 times, at most 100ms */
    for (i = 0; i < 10; i++) {
        for (taskId = 0; taskId < g_taskMaxNum; taskId++) {
            taskCB = OS_TCB_FROM_TID(taskId);
            if ((IsShellTask(taskId) == TRUE) && (taskCB->taskStatus & OS_TASK_STATUS_RUNNING)) {
                break;
            }
        }
        /* shell task is not running.
         * however, although delay, shell task may can not pend if isr cause exception
         * when shell task is running in current core.
         */
        if (taskId == g_taskMaxNum) {
            HaltOtherCores();
            /* wait 10ms for other core to halt */
            LOS_Mdelay(10);
            break;
        }
        /* wait 10ms for shell task on other cores to pend */
        LOS_Mdelay(10);
    }
#else
    HaltOtherCores();
#endif
    OsPercpuGet()->excFlag = CPU_EXC;
    LOCKDEP_CLEAR_LOCKS();
    CheckAllCpuStatus();
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
#endif

    g_curNestCount++;

    OsExcUpdatePC(excType, excBufAddr);
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
        ;
    }
}
