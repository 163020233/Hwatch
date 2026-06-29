/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 * Description : LiteOS exception module implementation.
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

#include "arch/exception.h"
#include "los_task_pri.h"
#include "los_typedef.h"
#include "los_printf_pri.h"
#include "los_init.h"
#if defined(LOSCFG_SHELL_EXCINFO_DUMP) || defined(LOSCFG_EXC_INTERACTION)
#include "los_exc_pri.h"
#include "los_hwi_pri.h"
#endif
#ifdef LOSCFG_KERNEL_TRACE
#include "los_trace_pri.h"
#endif
#ifdef LOSCFG_KERNEL_HPL
#include "los_hpl_pri.h"
#endif

#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
#include "console_pri.h"
#endif
#include "los_hook_pri.h"
#ifdef LOSCFG_MEM_DEBUG
#include "los_memory_pri.h"
#endif
#include "los_exc_pri.h"

#ifdef LOSCFG_BACKTRACE_SP
#include "los_hook.h"
#endif

#define MCAUSE_IS_INTERRUPT_BIT   0x80000000
#define MCAUSE_MASK_INT_NUM       0x00000FFF
#define OS_MAX_BACKTRACE  100
#define HARDWARE_RSVD 0x1b
#ifdef LOSCFG_LIB_CONFIGURABLE
    UINTPTR g_nmiStackTop = (UINTPTR)(&__nmi_stack_top);
    UINTPTR g_excStackTop = (UINTPTR)(&__exc_stack_top);
#endif

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr);
LITE_OS_SEC_BSS ExcInfo g_excInfo;
STATIC EXC_PROC_FUNC g_excHook = OsExcHook;

#ifdef LOSCFG_BACKTRACE_FP
#define RA_OFFSET         4
#define FP_OFFSET         8
#define FP_ALIGN(value)   (((UINT32)(value) & (UINT32)(LOSCFG_STACK_POINT_ALIGN_SIZE - 1)) == 0)

#ifdef LOSCFG_SHARED_STACK
#define DEFAULT_FP_VALUE 0x0F0F0F0F
UINTPTR ActFPGet(UINTPTR regFP, LosTaskCB *taskCB)
{
    if (taskCB == NULL) {
        return regFP;
    }
    if (regFP == DEFAULT_FP_VALUE) {
        return regFP;
    }
    if (taskCB->cacheSize == 0) {
        return regFP;
    }
    if (taskCB->cachePtr == NULL) {
        regFP = DEFAULT_FP_VALUE;
        return regFP;
    }
    if ((UINTPTR)(taskCB->cachePtr) > (UINTPTR)g_sharedStackMem[ArchCurrCpuid()]) {
        regFP = regFP + (UINTPTR)(taskCB->cachePtr) - (UINTPTR)(taskCB->stackPointer);
    } else {
        regFP = regFP - (UINTPTR)((UINTPTR)(taskCB->stackPointer) - (UINTPTR)(taskCB->cachePtr));
    }
    return regFP;
}

STATIC VOID GetActFPBySP(UINTPTR *fp, const VOID *stackPointer)
{
    LosTaskCB *taskCB = NULL;
    for (UINT32 index = 0; index < g_taskMaxNum; index++) {
        taskCB = OS_TCB_FROM_TID(index);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }

        if (g_sharedStackMem[ArchCurrCpuid()] <= (UINTPTR)stackPointer &&
            (UINTPTR)stackPointer <= (UINTPTR)(g_sharedStackMem[ArchCurrCpuid()] + (UINTPTR)LOSCFG_SHARED_STACK_SIZE)) {
            goto TASKFINDEND;
        }

        if (taskCB->cacheSize != 0) {
            if ((UINTPTR)stackPointer != (UINTPTR)(taskCB->cachePtr)) {
                continue;
            }
            goto TASKFINDEND;
        }

        if ((UINTPTR)(taskCB->topOfStack) <= (UINTPTR)stackPointer &&
            (UINTPTR)stackPointer < (UINTPTR)((UINTPTR)(taskCB->topOfStack) + (UINTPTR)(taskCB->stackSize))) {
            goto TASKFINDEND;
        }
    }

TASKFINDEND:
    *fp = ActFPGet(*fp, taskCB);
}
#endif

/* this function is used to validate fp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidFP(UINTPTR regFP, UINTPTR start, UINTPTR end)
{
#ifdef LOSCFG_SHARED_STACK
    return (regFP > start) && (regFP <= end);
#else
    return (regFP > start) && (regFP <= end) && FP_ALIGN(regFP);
#endif
}

STATIC INLINE BOOL FindSuitableStack(UINTPTR regFP, UINTPTR *start, UINTPTR *end, LosTaskCB **taskCBIn)
{
    UINT32 index;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;

    /* Search in the task stacks */
    for (index = 0; index < g_taskMaxNum; index++) {
        taskCB = OS_TCB_FROM_TID(index);
        if (taskCB->taskStatus & OS_TASK_STATUS_UNUSED) {
            continue;
        }

        stackStart = taskCB->topOfStack;
        stackEnd = taskCB->topOfStack + (UINTPTR)taskCB->stackSize;

#ifdef LOSCFG_SHARED_STACK
        if (taskCB->cacheSize != 0) {
            stackStart = (UINTPTR)(taskCB->cachePtr);
            stackEnd = (UINTPTR)((UINTPTR)(taskCB->cachePtr) + (UINTPTR)taskCB->cacheSize);
            if (!IsValidFP(regFP, stackStart, stackEnd)) {
                continue;
            }
            found = TRUE;
            *taskCBIn = taskCB;
            goto FOUND;
        }
#endif

        if (IsValidFP(regFP, stackStart, stackEnd)) {
            found = TRUE;
            goto FOUND;
        }
    }

    /* Search in the exc stacks */
    stackStart = (UINTPTR)&__int_stack_start;
    stackEnd = (UINTPTR)&__int_stack_end;
    if (IsValidFP(regFP, stackStart, stackEnd)) {
        found = TRUE;
    }

FOUND:
    if (found == TRUE) {
        *start = stackStart;
        *end = stackEnd;
    }

    return found;
}
#endif

#ifdef LOSCFG_BACKTRACE_SP
#define DEFAULT_SIZE 2
LosTextAddrInfo g_textAddrInfo[DEFAULT_SIZE] __attribute__((weak)) = {
    {0, 0},
    {0, 0}
};
INT32 g_textAddrInfoSize __attribute__((weak)) = DEFAULT_SIZE;

STATIC BOOL OsCheckAddrRange(uint32_t pc, uint32_t text_start, uint32_t text_end)
{
    if (pc >= text_start && pc < text_end)
        return TRUE;
    return FALSE;
}

STATIC BOOL OsIsValidTextAddr(uint32_t pc)
{
    for (int i = 0; i < g_textAddrInfoSize; i++) {
        if (OsCheckAddrRange(pc, g_textAddrInfo[i].text_begin, g_textAddrInfo[i].text_end))
            return TRUE;
    }
    return FALSE;
}

#define SP_DEFAULT_IGNRDEPTH (2)
#define SP_OTHER_TASK_IGNRDEPTH (7)
#define SP_EXC_IGNRDEPTH (4)
LITE_OS_SEC_TEXT STATIC void OsBackTraceSPGetInfo(uint32_t *stackStart, uint32_t *stackEnd,
    uint32_t *ignrDepthDefault, uint32_t taskId)
{
    LosTaskCB *curTask  = NULL;
    uint32_t stackStartTemp = 0, stackEndTemp = 0;
    curTask = OS_TCB_FROM_TID(taskId);

    if (OS_INT_ACTIVE) {
        stackStartTemp = IRQ_STACK_TOP - IRQ_STACK_SIZE;
        stackEndTemp = IRQ_STACK_TOP;
    } else {
        stackStartTemp = curTask->topOfStack;
        stackEndTemp = curTask->topOfStack + curTask->stackSize;
#ifdef LOSCFG_SHARED_STACK
        if (curTask->cachePtr != NULL) {
            stackStartTemp = (UINTPTR)(curTask->cachePtr);
            stackEndTemp = stackStartTemp + curTask->cacheSize;
        }
#endif
    }
    *stackStart = stackStartTemp;
    *stackEnd = stackEndTemp;
    *ignrDepthDefault = SP_DEFAULT_IGNRDEPTH;
}

LITE_OS_SEC_TEXT STATIC void ArchBackTraceSP(uintptr_t args[], int argc)
{
    UINTPTR *array = (UINTPTR *)args[LOS_HOOK_PARAM0];
    UINTPTR maxDepth = args[LOS_HOOK_PARAM1];
    UINTPTR ignrDepth = args[LOS_HOOK_PARAM2];
    UINTPTR taskId = args[LOS_HOOK_PARAM3];
    unsigned long back_sp = 0;
    uint32_t stackStart = 0, stackEnd = 0, saved = 0, max_saved = 0;
    uint32_t count = 0, candidate = 0, ignrDepthDefault = 0;
    LosTaskCB *curTask  = OS_TCB_FROM_TID(taskId);
    LosTaskCB *runTask  = OsCurrTaskGet();

    OsBackTraceSPGetInfo(&stackStart, &stackEnd, &ignrDepthDefault, taskId);
    if (array != NULL)
        ignrDepthDefault += 0x1;

    __asm volatile("mv %0, sp" : "=r"(back_sp));
    if ((back_sp < stackStart) || (back_sp > stackEnd)) {
        if (curTask->taskId != runTask->taskId) {  // 其他任务回溯
            back_sp = stackStart;
            ignrDepthDefault += SP_OTHER_TASK_IGNRDEPTH;
        } else if (OS_EXC_ACTIVE) {  // 异常回溯
            back_sp = stackStart;
            ignrDepthDefault += SP_EXC_IGNRDEPTH;
        } else {
            return;
        }
    }

    max_saved = maxDepth - ignrDepth;
    while (back_sp < stackEnd) {
        candidate = *((uint32_t *)(uintptr_t)(back_sp));
        if ((candidate & 0x1) != 0) {
            back_sp += sizeof(uintptr_t);
            continue;
        }

        if (OsIsValidTextAddr(candidate) != 0) {
            count++;
            if (saved >= max_saved)
                break;
            // 默认忽略前x层
            if (count > (ignrDepth + ignrDepthDefault)) {
                if (array != NULL) {
                    array[saved] =  candidate;
                } else {
                    PRINT_EXC("traceback sp %u -- ra = 0x%x\n", saved, candidate);
                }
                saved++;
            }
        }
        back_sp += sizeof(uintptr_t);
    }
}

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE UINT32 LOS_PrepareMainTask(VOID)
{
    LOS_HookReg(LOS_HOOK_BACKTRACE_SP, ArchBackTraceSP);
    return 0;
}
LOS_SYS_INIT(LOS_PrepareMainTask, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);
#endif

STATIC VOID BackTraceWithFp(UINTPTR fp)
{
#ifdef LOSCFG_BACKTRACE_FP
    PRINT_EXC("*******backtrace fp begin*******\n");
#elif defined(LOSCFG_BACKTRACE_SP)
    PRINT_EXC("*******backtrace sp begin*******\n");
#endif
    (VOID)ArchBackTraceGet(fp, NULL, OS_MAX_BACKTRACE, 0);
#ifdef LOSCFG_BACKTRACE_FP
    PRINT_EXC("*******backtrace fp end*******\n");
#elif defined(LOSCFG_BACKTRACE_SP)
    PRINT_EXC("*******backtrace sp end*******\n");
#endif
}

UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE_FP
    UINTPTR tmpFP;
    UINTPTR backRa;
    UINTPTR backFP = fp;
    UINTPTR stackStart;
    UINTPTR stackEnd;
    UINT32 count;
    LosTaskCB *taskCB = NULL;

    if (FindSuitableStack(fp, &stackStart, &stackEnd, &taskCB) == FALSE) {
        PRINT_EXC("fp error, backtrace fp failed!\n");
        return 0;
    }

    /*
     * Check whether it is the leaf function.
     * Generally, the frame pointer minus RA_OFFSET points to the address of lr, while in the leaf function,
     * there's no function call, and compiler will not store the link register, but the frame pointer
     * will still be stored and updated. In that case we needs to find the right position of frame pointer.
     */
    tmpFP = *((UINTPTR *)(UINTPTR)(fp - RA_OFFSET));
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
        backRa = *((UINTPTR *)(UINTPTR)(tmpFP - RA_OFFSET));
        backFP = *((UINTPTR *)(UINTPTR)(tmpFP - FP_OFFSET));
#ifdef LOSCFG_SHARED_STACK
        backFP = ActFPGet(backFP, taskCB);
#endif

        if (count < ignrDepth) {
            continue;
        }

        if (callChain == NULL) {
            PRINT_EXC("traceback %u -- ra = 0x%x    fp = 0x%x\n", count - ignrDepth, backRa, backFP);
        } else {
            callChain[count - ignrDepth] = backRa;
        }

        if (backFP == tmpFP) { /* no need to check ra */
            break;
        }
    }
    return count;
#elif defined LOSCFG_BACKTRACE_SP
    LOS_HookExe(LOS_HOOK_BACKTRACE_SP, LOS_HOOK_PARAM4, (UINTPTR)(callChain), maxDepth, ignrDepth, fp);
    return 0;
#else
    (VOID)fp;
    (VOID)callChain;
    (VOID)maxDepth;
    (VOID)ignrDepth;
    return 0;
#endif
}

LITE_OS_SEC_TEXT VOID ArchBackTraceWithStackPointer(const VOID *stackPointer)
{
    UINTPTR fp = 0;
#ifdef LOSCFG_BACKTRACE_FP
    fp = ArchGetTaskFp(stackPointer);
#ifdef LOSCFG_SHARED_STACK
    GetActFPBySP(&fp, stackPointer);
#endif
    PRINT_EXC("fp:0x%08x\n", fp);
#elif defined(LOSCFG_BACKTRACE_SP)
    fp = (UINTPTR)stackPointer;
#else
    (VOID)stackPointer;
#endif
    BackTraceWithFp(fp);
}

LITE_OS_SEC_TEXT VOID ArchBackTrace(VOID)
{
    UINTPTR fp = 0;
#ifdef LOSCFG_BACKTRACE_FP
    fp = ArchGetFp();
    PRINT_EXC("fp:0x%08x\n", fp);
#elif defined(LOSCFG_BACKTRACE_SP)
    LosTaskCB *runTask  = OsCurrTaskGet();
    fp = runTask->taskId;
#endif
    BackTraceWithFp(fp);
}

#ifndef LOSCFG_EXC_SIMPLE_INFO
STATIC const CHAR *g_excOccurStage[] = {
    "Init",
    "Task",
    "Irq",
    "Exc"
};
#else
/*
 * refer to TaskContext defined in task.h
 * g_xRegsMap start from X4(instead of X0) to X31
 * the index of array maps subscript of x-regs plus 4, e.g. index0 -> X4, index1 -> X5, index2 -> X6 ...
 * the value of elements means offset of member in TaskContext, e.g. X4 -> the 2nd member of TaskContext
 */
STATIC const UINT8 g_xRegsMap[] = {
/*  X4  X5  X6  X7  X8  X9  X10 */
    2,  30, 29, 28, 15, 14, 27,
/*  X11 X12 X13 X14 X15 X16 X17 */
    26, 25, 24, 23, 22, 21, 20,
/*  X18 X19 X20 X21 X22 X23 X24 */
    13, 12, 11, 10, 9,  8,  7,
/*  X25 X26 X27 X28 X29 X30 X31 */
    6,  5,  4,  19, 18, 17, 16
};
#endif

LITE_OS_SEC_TEXT_INIT_LEVEL_ONE UINT32 ArchExcInit(VOID)
{
    return LOS_OK;
}
LOS_SYS_INIT(ArchExcInit, SYS_INIT_LEVEL_ARCH, SYS_INIT_SYNC_0);

BOOL ArchExcActive(VOID)
{
    return (ExcActive() != 0);
}

LITE_OS_SEC_TEXT STATIC VOID OsExcInfoDisplayContext(const ExcInfo *exc)
{
    const ExcContext *excContext = exc->context;
    const TaskContext *taskContext = &(excContext->taskContext);

    ArchDisplayExcSysReg(exc);

#ifndef LOSCFG_EXC_SIMPLE_INFO
    PRINT_EXC("tp:0x%x\n""t0:0x%x\n""t1:0x%x\n""t2:0x%x\n"
        "s0:0x%x\n""s1:0x%x\n""a0:0x%x\n""a1:0x%x\n",
        taskContext->tp, taskContext->t0, taskContext->t1, taskContext->t2,
        taskContext->s0, taskContext->s1, taskContext->a0, taskContext->a1);
    PRINT_EXC("a2:0x%x\n""a3:0x%x\n""a4:0x%x\n""a5:0x%x\n"
        "a6:0x%x\n""a7:0x%x\n""s2:0x%x\n""s3:0x%x\n",
        taskContext->a2, taskContext->a3, taskContext->a4, taskContext->a5,
        taskContext->a6, taskContext->a7, taskContext->s2, taskContext->s3);
    PRINT_EXC("s4:0x%x\n""s5:0x%x\n""s6:0x%x\n""s7:0x%x\n"
        "s8:0x%x\n""s9:0x%x\n""s10:0x%x\n""s11:0x%x\n",
        taskContext->s4, taskContext->s5, taskContext->s6, taskContext->s7,
        taskContext->s8, taskContext->s9, taskContext->s10, taskContext->s11);
    PRINT_EXC("t3:0x%x\n""t4:0x%x\n""t5:0x%x\n""t6:0x%x\n",
        taskContext->t3, taskContext->t4, taskContext->t5, taskContext->t6);
#else
    UINT32 i;
    UINT16 regIdx;
    const UINT32 *contextRegs = (const UINT32 *)taskContext;
    /* X0 is always zero, X1 - X3 is displayed before, so start from X4 */
    for (i = 0; i < sizeof(g_xRegsMap) / sizeof(g_xRegsMap[0]); i++) {
#ifdef LOSCFG_ARCH_FPU_ENABLE
#define FPU_OFFSET_OF_TASK_CONTEXT ((FPU_EXTENSION_REG_SIZE + FPU_STATUS_REG_SIZE) / sizeof(UINT32))
        regIdx = (i == 0) ? g_xRegsMap[i] : (g_xRegsMap[i] + FPU_OFFSET_OF_TASK_CONTEXT);
#else
        regIdx = g_xRegsMap[i];
#endif
        PRINT_EXC("X%-02u:0x%x\n", i + 4, contextRegs[regIdx]); /* 4: X4 reg */
    }
#endif
}

VOID OsExcInfoDisplay(UINT32 excType, const ExcContext *excBufAddr)
{
    if (g_excInfo.nestCnt > 1) { /* Exception nesting level is 1 */
        g_excInfo.phase = OS_EXC_STAGE_EXC;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (!OS_SCHEDULER_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_INIT;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else if (OS_INT_ACTIVE) {
        g_excInfo.phase = OS_EXC_STAGE_IRQ;
        g_excInfo.thrdPid = OS_EXC_STAGE_INIT_VALUE;
    } else {
        g_excInfo.phase = OS_EXC_STAGE_TASK;
        g_excInfo.thrdPid = LOS_CurTaskIDGet();
    }
    g_excInfo.faultAddr = excBufAddr->mtval;
    g_excInfo.type = (UINT16)excType;
    g_excInfo.context = (ExcContext *)excBufAddr;

    /* Traverse all memory nodes to check if any memory node has been corrupted */
#ifdef LOSCFG_MEM_DEBUG
    OsExcMemInfo();
#endif
#ifdef LOSCFG_HARDWARE_STACK_CHECK_ENABLE
    if (excBufAddr->rsvd == HARDWARE_RSVD) {
        LosTaskCB *runTask = OsCurrTaskGet();
        PRINT_EXC("HIGHEST task ID: %s:%u SP error! StackPointer: %p TopOfStack: %p\n",
            OsCurTaskNameGet(),
            g_excInfo.thrdPid,
            runTask->stackPointer,
            runTask->topOfStack);
    }
#endif
#ifdef LOSCFG_EXC_SIMPLE_INFO
    PRINT_EXC("task:%s\n""thrdPid:0x%x\n""type:0x%x\n""nestCnt:%u\n""phase:%u\n",
        OsCurTaskNameGet(), g_excInfo.thrdPid, g_excInfo.type, g_excInfo.nestCnt, g_excInfo.phase);
#else
    PRINT_EXC("task:%s\n""thrdPid:0x%x\n""type:0x%x\n""nestCnt:%u\n""phase:%s\n",
        OsCurTaskNameGet(), g_excInfo.thrdPid, g_excInfo.type, g_excInfo.nestCnt, g_excOccurStage[g_excInfo.phase]);
#endif

    OsExcInfoDisplayContext(&g_excInfo);
}

STATIC VOID UpdateCurrTaskSP(UINT32 sp)
{
    LosTaskCB *runTask = NULL;

    if (OS_SCHEDULER_ACTIVE && !OS_INT_ACTIVE) {
        runTask = (LosTaskCB *)ArchCurrTaskGet();
        if (runTask != NULL) {
            runTask->stackPointer = (VOID *)(UINTPTR)sp;
        }
    }
}

#ifdef LOSCFG_ARCH_LINXCORE_131
VOID OsTriggerNMI(VOID)
{
    WRITE_UINT32(0x1, NMI_REG_BASE);
}

VOID OsClearNMI(VOID)
{
    WRITE_UINT32(0x0, NMI_REG_BASE);
}
#endif

LITE_OS_SEC_TEXT STATIC VOID OsNMIHandler(UINT32 excType, const ExcContext *excBufAddr)
{
    PRINT_EXC("Oops:NMI\n");
#ifdef LOSCFG_ARCH_LINXCORE_131
    OsClearNMI();
#endif
#ifdef LOSCFG_MEM_DEBUG
    OsCheckPool((void*)m_aucSysMem0);
#endif
    OsExcInfoDisplay(excType, excBufAddr);
    for (;;) {
    }
}

LITE_OS_SEC_TEXT STATIC VOID OsExcHandler(UINT32 excType, const ExcContext *excBufAddr)
{
#ifdef LOSCFG_CONSOLE_UNIFIED_SERIAL_OUTPUT
    OsConsoleDisable();
#endif

    OsExcInfoDisplay(excType, excBufAddr);
#ifdef LOSCFG_BACKTRACE_FP
    BackTraceWithFp(excBufAddr->taskContext.s0);
#elif defined(LOSCFG_BACKTRACE_SP)
    LosTaskCB *runTask  = OsCurrTaskGet();
    BackTraceWithFp(runTask->taskId);
#endif

#ifndef LOSCFG_EXC_SIMPLE_INFO
    (VOID)OsDbgTskInfoGet(OS_ALL_TASK_MASK);
    OsExcStackInfo();
#endif
}

VOID OsExcHook(UINT32 excType, const ExcContext *excBufAddr)
{
    if ((excType & MCAUSE_MASK_INT_NUM) == NUM_HAL_INTERRUPT_NMI) {
        OsNMIHandler(excType, excBufAddr);
    } else {
        OsExcHandler(excType, excBufAddr);
    }
}

VOID OsExcHandleEntry(UINT32 excType, const ExcContext *excBufAddr)
{
    g_excCount[ArchCurrCpuid()] += 1;
    LOS_HookExe(LOS_HOOK_EXC_USER, LOS_HOOK_PARAM2, excType, (UINTPTR)excBufAddr);
#ifndef LOSCFG_EXC_SIMPLE_INFO
    ArchDisplayExcReason(excType, excBufAddr);
#endif

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    LogReadWriteFunc func = OsGetExcInfoRW();
#endif

    if (g_excInfo.nestCnt == 1) { /* 1: nest cnt, first exc */
        UpdateCurrTaskSP(excBufAddr->taskContext.sp);
#ifdef LOSCFG_SHELL_EXCINFO_DUMP
        if (func != NULL) {
            OsSetExcInfoOffset(0);
        }
#endif
        if (g_excHook != NULL) {
            g_excHook(excType, excBufAddr);
        }
    } else {
        OsExcInfoDisplay(excType, excBufAddr);
    }

#ifdef LOSCFG_SHELL_EXCINFO_DUMP
    if (func != NULL) {
        PRINT_EXC("Be sure your space bigger than OsGetExcInfoOffset():0x%x\n", OsGetExcInfoOffset());
        OsIrqNestingCntSet(0);     /* 0: int nest count */
        func(OsGetExcInfoDumpAddr(), OsGetExcInfoLen(), 0, OsGetExcInfoBuf());
        OsIrqNestingCntSet(1);     /* 1: int nest count */
    }
#endif

#ifdef LOSCFG_KERNEL_TRACE
    if (g_traceDumpHook != NULL) {
        g_traceDumpHook(FALSE);
    }
#endif

#ifdef LOSCFG_KERNEL_HPL
    OsHplExceptionHandle();
#endif
    g_excCount[ArchCurrCpuid()] -= 1;
    for (;;) {
    }
}

VOID OsNMIHandlerEntry(const ExcContext *excBufAddr)
{
    UINT32 excType;

    g_excCount[ArchCurrCpuid()] += 1;
    excType = excBufAddr->mcause;
    if (g_excHook != NULL) {
        g_excHook(excType, excBufAddr);
    }
    g_excCount[ArchCurrCpuid()] -= 1;
}

UINT32 ArchSetExcHook(EXC_PROC_FUNC excHook)
{
    UINT32 intSave;

    intSave = ArchIntLock();
    g_excHook = excHook;
    ArchIntRestore(intSave);
    return 0;
}

EXC_PROC_FUNC ArchGetExcHook(VOID)
{
    return g_excHook;
}

UINT32 ArchSetNMIHook(NMI_PROC_FUNC nmiHook)
{
    (void)nmiHook;
    return 0;
}

NMI_PROC_FUNC ArchGetNMIHook(VOID)
{
    return NULL;
}
