/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Arm Thumb Instruction Backtrace Implementation
 * Author: Huawei LiteOS Team
 * Create: 2024-10-09
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

#include "los_task_pri.h"
#include "los_printf_pri.h"
#include "los_memory_pri.h"
#include "arch/cpu.h"

#ifdef LOSCFG_BACKTRACE
#define OS_MAX_BACKTRACE       15
#define THUM_OFFSET            2
#define STACK_OFFSET           4
#define MASK_OFFSET            2
#define HIGH_8_OFFSET          8
#define BL_CMD_OFFSET          4
#define BLX_CMD_OFFSET         2
#define PUSH_MASK_WITH_LR      0xb5
#define PUSH_MASK              0xb4
#define LOW_11_BITS_MASK       0x7FF
#define HIGH_8_BITS_MASK       0xFF00
#define SIGN_BIT_MASK          0x400000
#define HIGH_OFFSET_NUMBER     12
#define LOW_OFFSET_NUMBER      1

#define BL_INS                 0xF000
#define BLX_INX                0x4700

STATIC const UINTPTR g_textStart = (const UINTPTR)&__text_start;
STATIC const UINTPTR g_textEnd = (const UINTPTR)&__text_end;
STATIC const UINTPTR g_estack = (const UINTPTR)&_estack;
#endif

#ifdef LOSCFG_BACKTRACE
/* this function is used to validate sp or validate the checking range start and end. */
STATIC INLINE BOOL IsValidSP(UINTPTR regSP, UINTPTR start, UINTPTR end)
{
    return (regSP >= start) && (regSP < end);
}

STATIC INLINE BOOL FindSuitableStack(const UINTPTR *regSP, UINTPTR *start, UINTPTR *end)
{
    UINT32 index, topOfStack, stackBottom;
    BOOL found = FALSE;
    LosTaskCB *taskCB = NULL;

    /* Search in the task stacks */
    for (index = 0; index < g_taskMaxNum; index++) {
        taskCB = OS_TCB_FROM_TID(index);
        if ((taskCB->taskStatus & OS_TASK_STATUS_UNUSED) != 0) {
            continue;
        }
        topOfStack = taskCB->topOfStack;
        stackBottom = taskCB->topOfStack + taskCB->stackSize;

        if (IsValidSP(*regSP, topOfStack, stackBottom)) {
            found = TRUE;
            goto FOUND;
        }
    }

FOUND:
    if (found == TRUE) {
        *start = topOfStack;
        *end = stackBottom;
    } else if (*regSP < g_estack) {
        *start = *regSP;
        *end = g_estack;
        found = TRUE;
    }

    return found;
}

STATIC INLINE UINTPTR LoopUntilEntry(UINTPTR addr)
{
    UINTPTR address = addr;
    while (address > (UINTPTR)g_textStart) {
        if (((*((UINT16 *)address) >> HIGH_8_OFFSET) == PUSH_MASK_WITH_LR) ||
            ((*((UINT16 *)address) >> HIGH_8_OFFSET) == PUSH_MASK)) {
            break;
        }
        address -= THUM_OFFSET;
    }

    return address;
}

STATIC INLINE UINTPTR CalculateBLTargetAddress(UINTPTR bl)
{
    UINTPTR target;
    UINT32 off0, off1, off;

    off0 = *(UINT16 *)bl & LOW_11_BITS_MASK;
    off1 = *(UINT16 *)(bl + MASK_OFFSET) & LOW_11_BITS_MASK;
    off = (off0 << HIGH_OFFSET_NUMBER) + (off1 << LOW_OFFSET_NUMBER);
    if ((off & SIGN_BIT_MASK) != 0) {
        target = bl + BL_CMD_OFFSET - ((~(off - 1)) & 0x7FFFFF); // 0x7FFFFF : offset mask
    } else {
        target = bl + BL_CMD_OFFSET + off;
    }

    return target;
}

STATIC INLINE UINTPTR CalculateTargetAddress(UINTPTR bl)
{
    UINTPTR target;
    STATIC UINTPTR tmpBL = 0;

    if ((((*(UINT16 *)(bl - BLX_CMD_OFFSET)) & HIGH_8_BITS_MASK) == BLX_INX)) {
        if (tmpBL != 0) {
            target = LoopUntilEntry(tmpBL);
            tmpBL = bl - BLX_CMD_OFFSET;
            return target;
        }
        tmpBL = bl - BLX_CMD_OFFSET;
        return LoopUntilEntry(tmpBL);
        /* Get 2 bytes twice to avoid unaligned access */
    } else if (((*(UINT16 *)(bl - BL_CMD_OFFSET + 2) & BL_INS) == BL_INS) &&
               ((*(UINT16 *)(bl - BL_CMD_OFFSET) & BL_INS) == BL_INS)) {
        tmpBL = bl - BL_CMD_OFFSET;
        return CalculateBLTargetAddress(tmpBL);
    }

    return 0;
}

VOID BackTraceWithSp(UINTPTR sp)
{
    PRINT_EXC("*******backtrace %s*******\n", "begin");
    (VOID)ArchBackTraceGet(sp, NULL, OS_MAX_BACKTRACE, 0);
    PRINT_EXC("*******backtrace %s*******\n", "end");
}
#endif

UINT32 ArchBackTraceGet(UINTPTR fp, UINTPTR *callChain, UINT32 maxDepth, UINT32 ignrDepth)
{
#ifdef LOSCFG_BACKTRACE
    UINTPTR stackPointer = fp;
    UINT32 count;
    UINTPTR topOfStack = 0;
    UINTPTR stackBottom = 0;
    STATIC UINTPTR tmpJump = 0;

    if (FindSuitableStack(&stackPointer, &topOfStack, &stackBottom) == FALSE) {
        PRINT_EXC("fp error, backtrace failed!\n");
        return 0;
    }

    for (count = 0; (stackPointer < stackBottom) && (count < maxDepth); stackPointer += STACK_OFFSET) {
        if ((*(UINT32 *)stackPointer >= (UINT32)g_textStart) &&
            (*(UINT32 *)stackPointer <= (UINT32)g_textEnd) &&
            IS_ALIGNED(*(UINT32 *)stackPointer - 1, THUM_OFFSET)) {
            /* Get the entry address of current function. */
            UINTPTR checkBL = CalculateTargetAddress(*(UINT32 *)stackPointer - 1);
            if ((checkBL == 0) || (checkBL == tmpJump)) {
                continue;
            }
            tmpJump = checkBL;

            if (count < ignrDepth) {
                count++;
                continue;
            }

            if (callChain == NULL) {
                PRINT_EXC("traceback %u -- lr = 0x%x    fp = 0x%x\n",
                          count - ignrDepth, *(UINT32 *)stackPointer - 1, tmpJump);
            } else {
                callChain[count - ignrDepth] = *(UINTPTR *)stackPointer - 1;
            }
            count++;
        }
    }
    (VOID)topOfStack;
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
    UINTPTR sp = ArchSPGet();
    PRINT_EXC("sp:0x%08x\n", sp);
    BackTraceWithSp(sp);
#endif
}

VOID ArchBackTraceWithSp(const VOID *stackPointer)
{
#ifdef LOSCFG_BACKTRACE
    BackTraceWithSp((UINTPTR)stackPointer);
#else
    (VOID)stackPointer;
#endif
}