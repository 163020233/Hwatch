/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2021. All rights reserved.
 * Description : Implementation Of The Himideerv200 Interrupt.
 * Author: Huawei LiteOS Team
 * Create : 2020-06-02
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
/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */

#include "arch/regs.h"
#include "riscv_interrupt.h"
#include "los_hwi_pri.h"

#ifdef LOS_LOCATION_VECTOR_IAR
#pragma  location = ".vector"
#endif

STATIC LITE_OS_SEC_BSS HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = { 0 };

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    (VOID)hwiNum;
    return LOS_NOK;
}

HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }
    return &g_hwiForm[hwiNum];
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    /*
     * This register is supposed to be used when other interrupt pending
     * statuses are cleared by the SW nested interrupt handler without exiting
     * the handler and re-entering the interrupt handler
     */
    HalIrqClearInner(hwiNum);
#endif
    return LOS_OK;
}

STATIC INLINE VOID HalIrqHandlerCall(UINT32 hwiNum)
{
    if (hwiNum >= LOSCFG_PLATFORM_HWI_LIMIT) {
        return;
    }

    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
    (void)HalIrqClear(hwiNum);
}

#ifdef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
VOID HalIrqHandler(UINT32 hwiNum)
{
    HalIrqHandlerCall(hwiNum);
}
#else
STATIC VOID HalIrqHandler(VOID)
{
    HalIrqHandlerCall(HalCurIrqGet());
}
#endif

#ifdef LOSCFG_DEBUG_HWI
STATIC UINT32 HalIrqStatusGet(HWI_HANDLE_T hwiNum, HwiStatus *status)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    status->pri = HalIrqGetPrio(hwiNum);
    status->enable = HalIrqGetEnableStatus(hwiNum);
    status->affinity = 0x1U;
    (VOID)HalIrqPendingGet(hwiNum, (UINT8*)&status->pending);
    return LOS_OK;
}
#endif

LITE_OS_SEC_TEXT_INIT VOID HalIrqInit(VOID)
{
    UINT32 idx;

    for (idx = OS_RISCV_SYS_VECTOR_CNT; idx < LOSCFG_PLATFORM_HWI_LIMIT; idx++) {
        g_hwiForm[idx].hook = NULL;
#ifdef LOSCFG_SHARED_IRQ
        g_hwiForm[idx].shareMode = 0;
        g_hwiForm[idx].next = NULL;
#endif
    }

#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
    static const HwiControllerOps g_plicOps = {
        .clearIrq        = HalIrqClear,
        .enableIrq       = HalIrqUnmask,
        .disableIrq      = HalIrqMask,
        .setIrqPriority  = HalIrqSetPrio,
        .getCurIrqNum    = HalCurIrqGet,
        .getIrqVersion   = HalIrqVersion,
        .getHandleForm   = HalIrqGetHandleForm,
        .handleIrq       = HalIrqHandler,
#ifdef LOSCFG_DEBUG_HWI
        .getIrqStatus   = HalIrqStatusGet,
#endif
    };
    OsHwiControllerReg(&g_plicOps);
#endif
}
