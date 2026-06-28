/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2018. All rights reserved.
 * Description: NVIC(Nested Vectored Interrupt Controller) for Cortex-M.
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
/* ----------------------------------------------------------------------------
 * This file has been modified in accordance with Misra-C specifications.
 * ---------------------------------------------------------------------------- */

#include "nvic.h"
#include "arch/cpu.h"
#include "los_hwi_pri.h"
#include "los_task_pri.h"

#define NVIC_ISER_IRQNUM_MASK           0x1FU
#define SYS_TICK_INT_NUM                15U /* SysTick_IRQn + OS_SYS_VECTOR_CNT */
#define NVIC_REG32_READ(addr)           (*(volatile UINT32 *)(UINTPTR)(addr))
#define NVIC_REG32_WRITE(addr, val)     (*(volatile UINT32 *)(UINTPTR)(addr) = (val))
#define NVIC_REG8_WRITE(addr, val)      (*(volatile UINT8 *)(UINTPTR)(addr) = (val))

STATIC UINT32 g_curIrqNum = 0;

STATIC LITE_OS_SEC_BSS HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = { 0 };

LITE_OS_SEC_DATA_VEC HWI_PROC_FUNC g_hwiVec[LOSCFG_PLATFORM_HWI_LIMIT] = {
    [0]  = (HWI_PROC_FUNC)0,               /* [0] Top of Stack */
    [1]  = (HWI_PROC_FUNC)Reset_Handler,   /* [1] Reset */
    [2]  = (HWI_PROC_FUNC)OsExcNMI,        /* [2] NMI Handler */
    [3]  = (HWI_PROC_FUNC)OsExcHardFault,  /* [3] Hard Fault Handler */
    [4]  = (HWI_PROC_FUNC)OsExcMemFault,   /* [4] MPU Fault Handler */
    [5]  = (HWI_PROC_FUNC)OsExcBusFault,   /* [5] Bus Fault Handler */
    [6]  = (HWI_PROC_FUNC)OsExcUsageFault, /* [6] Usage Fault Handler */
    [7]  = (HWI_PROC_FUNC)0,               /* [7] Reserved */
    [8]  = (HWI_PROC_FUNC)0,               /* [8] Reserved */
    [9]  = (HWI_PROC_FUNC)0,               /* [9] Reserved */
    [10] = (HWI_PROC_FUNC)0,               /* [10] Reserved */
    [11] = (HWI_PROC_FUNC)IrqEntryV7M,     /* [11] SVCall Handler */
    [12] = (HWI_PROC_FUNC)IrqEntryV7M,     /* [12] Debug Monitor Handler */
    [13] = (HWI_PROC_FUNC)0,               /* [13] Reserved */
    [14] = (HWI_PROC_FUNC)OsPendSV,        /* [14] PendSV Handler */
    [15] = (HWI_PROC_FUNC)IrqEntryV7M,     /* [15] SysTick Handler */
};


LITE_OS_SEC_TEXT_MINOR VOID IrqEntryV7M(VOID)
{
    UINT32 hwiNum = ArchGetIpsr();
    if (hwiNum > (UINT32)OS_USER_HWI_MAX) {
        return;
    }
    g_curIrqNum = hwiNum;
    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    UINT32 intSave;
    UINT32 irqNum;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    intSave = LOS_IntLock();
    BARRIER();
    NVIC_REG32_WRITE(NVIC_ISERx(irqNum), 1UL << (irqNum & NVIC_ISER_IRQNUM_MASK));
    BARRIER();
    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    UINT32 intSave;
    UINT32 irqNum;

    /* SysTick cannot use this function to set the interrupt priority. It should be set in HalClockStart. */
    if (hwiNum == SYS_TICK_INT_NUM) {
        return LOS_OK;
    }

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (!HWI_PRI_VALID(priority)) {
        return LOS_ERRNO_HWI_PRIO_INVALID;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    intSave = LOS_IntLock();
    NVIC_REG8_WRITE(NVIC_IPRx(irqNum), (UINT8)NVIC_CALC_PRIOVALUE(priority));
    LOS_IntRestore(intSave);

    return LOS_OK;
}

UINT32 HalIrqMask(UINT32 hwiNum)
{
    UINT32 intSave;
    UINT32 irqNum;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    intSave = LOS_IntLock();
    NVIC_REG32_WRITE(NVIC_ICERx(irqNum), 1UL << (irqNum & NVIC_ISER_IRQNUM_MASK));
    DSB();
    ISB();
    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    UINT32 intSave;
    UINT32 irqNum;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    intSave = LOS_IntLock();
    NVIC_REG32_WRITE(NVIC_ISPRx(irqNum), 1UL << (irqNum & NVIC_ISER_IRQNUM_MASK));
    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    UINT32 irqNum;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    NVIC_REG32_WRITE(NVIC_ICPRx(irqNum), 1UL << (irqNum & NVIC_ISER_IRQNUM_MASK));
    return LOS_OK;
}

UINT32 HalCurIrqGet(VOID)
{
    g_curIrqNum = ArchGetIpsr();
    return g_curIrqNum;
}

CHAR *HalIrqVersion(VOID)
{
    return (CHAR *)"NVIC";
}

HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    /* If SysTick is used to implement tick, you need to create interrupt 15 in HalClockInit. */
    if ((!HWI_NUM_VALID(hwiNum)) && (hwiNum != SYS_TICK_INT_NUM)) {
        return NULL;
    }

    return &g_hwiForm[hwiNum];
}

UINT32 HalIrqPendingGet(UINT32 hwiNum, UINT8 *isPending)
{
    UINT32 irqNum;

    if (!HWI_NUM_VALID(hwiNum) || (isPending == NULL)) {
        return LOS_NOK;
    }

    irqNum = hwiNum - (UINT32)OS_SYS_VECTOR_CNT;
    *isPending = ((GET_UINT32(NVIC_ISPRx(irqNum)) & (1UL << (irqNum & NVIC_ISER_IRQNUM_MASK))) != 0) ? 1 : 0;

    return LOS_OK;
}

STATIC VOID SetPrioGroup(UINT32 prioGroup)
{
    UINT32 regData;

    /* clear VECTKEY/PRIGROUP bit */
    regData = NVIC_REG32_READ(SCB_AIRCR) & (~(SCB_AIRCR_VECTKEY_MASK | SCB_AIRCR_PRIGROUP_MASK));
    /* set VECTKEY/PRIGROUP */
    regData = regData | (SCB_AIRCR_VECTKEY_KEY << SCB_AIRCR_VECTKEY_IDX) |
              ((prioGroup << SCB_AIRCR_PRIGROUP_IDX) & SCB_AIRCR_PRIGROUP_MASK);
    NVIC_REG32_WRITE(SCB_AIRCR, regData);
}

LITE_OS_SEC_TEXT_INIT VOID HalIrqInit(VOID)
{
    UINT32 idx;

    for (idx = OS_SYS_VECTOR_CNT; idx < LOSCFG_PLATFORM_HWI_LIMIT; idx++) {
        g_hwiVec[idx] = (HWI_PROC_FUNC)IrqEntryV7M;
    }

    /* Interrupt vector table location */
    NVIC_REG32_WRITE(SCB_VTOR, (UINT32)(UINTPTR)g_hwiVec);
#if (CORTEX_M_VERSION >= 0x03U) /* only for Cortex-M3 and above */
    SetPrioGroup(OS_NVIC_AIRCR_PRIGROUP);
#endif

#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
    static const HwiControllerOps g_nvicOps = {
        .triggerIrq     = HalIrqTrigger,
        .enableIrq      = HalIrqUnmask,
        .disableIrq     = HalIrqMask,
        .setIrqPriority = HalIrqSetPrio,
        .getCurIrqNum   = HalCurIrqGet,
        .getIrqVersion  = HalIrqVersion,
        .getHandleForm  = HalIrqGetHandleForm,
        .clearIrq       = HalIrqClear,
    };
    /* register interrupt controller's operations */
    OsHwiControllerReg(&g_nvicOps);
#endif
    return;
}
