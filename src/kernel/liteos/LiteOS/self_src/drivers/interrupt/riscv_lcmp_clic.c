/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description : Implementation Of The Riscv CLIC Interrupt.
 * Author: Huawei LiteOS Team
 * Create : 2021-09-03
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

#include "los_hwi_pri.h"
#include "soc/clic.h"
#include "arch/regs.h"
#include "arch/cpu.h"

STATIC UINT32 g_curIrqNum[LOSCFG_KERNEL_CORE_NUM];

#define REG_CLIC_CFG                              (CLIC_REG_BASE + CLIC_CFG)
#define REG_CLIC_INTIP(n)                         (CLIC_REG_BASE + CLIC_INTIP(n))
#define REG_CLIC_INTIE(n)                         (CLIC_REG_BASE + CLIC_INTIE(n))
#define REG_CLIC_INTATTR(n)                       (CLIC_REG_BASE + CLIC_INTATTR(n))
#define REG_CLIC_INTCTL(n)                        (CLIC_REG_BASE + CLIC_INTCTL(n))

#define CLIC_CFG_REGISTER_BITS_NUM 8U
#define CLIC_CFG_INTERRUPT_OFFSET  1U
#define SET_SYS_PRIOR(usrPrior) \
    ((UINT8)((UINT8)(LOSCFG_HWI_PRIO_LIMIT - 1) + CLIC_CFG_INTERRUPT_OFFSET - (usrPrior)))

/* CLICINTCTLBITS : Number of bits implemented in clicintctl[i] */
#if LOSCFG_HWI_PRIO_LIMIT > (1U << REG_CLIC_INFO_CLICINTCTLBITS)
#error "Actual interrupt indication bits less than cliccfg.nlbits."
#endif

__attribute__((aligned(64))) HAL_HWI_PROC_FUNC g_vectorIntTable[LOSCFG_PLATFORM_HWI_LIMIT] = {0}; /* non-vector */

STATIC UINT32 g_clicCfgLevelBitsNum = 1; /* the lowest level bits num is 1 */
#ifdef LOSCFG_HWI_FORM_DYNAMIC_ALLOCATION
STATIC LITE_OS_SEC_BSS HwiHandleInfo *g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = {0};
#define HWI_FORM_ADDR_GET(hwiNum)     g_hwiForm[(hwiNum)]
#define HWI_HANDLE_INVALID            ((VOID *)OS_NULL_INT)
#else
STATIC LITE_OS_SEC_BSS HwiHandleInfo  g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = {0};
#define HWI_FORM_ADDR_GET(hwiNum)     &g_hwiForm[(hwiNum)]
#define HWI_HANDLE_INVALID            NULL
#endif

UINT32 HalIrqSetHandler(UINT32 hwiNum, HAL_HWI_PROC_FUNC handler)
{
    UINT32 intSave;
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    intSave = LOS_IntLock();
    g_vectorIntTable[hwiNum] = handler;
    LOS_IntRestore(intSave);
    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    WRITE_UINT8(1, REG_CLIC_INTIE(hwiNum));
    return LOS_OK;
}

UINT32 HalIrqMask(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    WRITE_UINT8(0, REG_CLIC_INTIE(hwiNum));
    return LOS_OK;
}

UINT32 HalIrqMaskGet(UINT32 hwiNum, UINT8 *isMask)
{
    if (!HWI_NUM_VALID(hwiNum) || (isMask == NULL)) {
        return LOS_NOK;
    }

    READ_UINT8(*isMask, REG_CLIC_INTIE(hwiNum));

    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    WRITE_UINT8(1, REG_CLIC_INTIP(hwiNum));
    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    WRITE_UINT8(0, REG_CLIC_INTIP(hwiNum));
    return LOS_OK;
}

UINT32 HalIrqPendingGet(UINT32 hwiNum, UINT8 *isPending)
{
    if (!HWI_NUM_VALID(hwiNum) || (isPending == NULL)) {
        return LOS_NOK;
    }

    READ_UINT8(*isPending, REG_CLIC_INTIP(hwiNum));
    /* get one bit */
    return LOS_OK;
}

UINT8 HalIrqGetClicCfg(VOID)
{
    return GET_UINT8(REG_CLIC_CFG);
}

VOID HalIrqSetClicCfg(UINT8 clicCfg)
{
    WRITE_UINT8(clicCfg, REG_CLIC_CFG);
}

UINT32 HalIrqSetClicIntAttr(UINT32 hwiNum, UINT8 clicintattr)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    WRITE_UINT8(clicintattr, REG_CLIC_INTATTR(hwiNum));
    return LOS_OK;
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    UINT8 localLevel;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    if (priority >= LOSCFG_HWI_PRIO_LIMIT) {
        return LOS_ERRNO_HWI_PRIO_INVALID;
    }
    /* riscv's priority range is [1, 7] */
    localLevel = SET_SYS_PRIOR(priority);
    localLevel = (UINT8)((UINT32)localLevel << (8U - g_clicCfgLevelBitsNum)); /* max 8 bit */
    UINT8 mask = ((UINT8)(-1)) >> (g_clicCfgLevelBitsNum);
    UINT8 clicIntCtl = GET_UINT8(REG_CLIC_INTCTL(hwiNum));
    clicIntCtl = clicIntCtl & mask;
    clicIntCtl = clicIntCtl | localLevel;
    WRITE_UINT8(clicIntCtl, REG_CLIC_INTCTL(hwiNum));
    return LOS_OK;
}

CHAR *HalIrqVersion(VOID)
{
    return (CHAR *)"CLIC";
}

#ifdef LOSCFG_HWI_FORM_DYNAMIC_ALLOCATION
UINT32 HalIrqSetHandleForm(HWI_HANDLE_T hwiNum, HwiHandleInfo *hwiInfo)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_NOK;
    }

    g_hwiForm[hwiNum] = hwiInfo;

    return LOS_OK;
}
#endif

HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return HWI_HANDLE_INVALID;
    }

    return HWI_FORM_ADDR_GET(hwiNum);
}

UINT32 HalCurIrqGet(VOID)
{
    return g_curIrqNum[ArchCurrCpuid()];
}

STATIC INLINE VOID HalIrqHandlerCall(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return;
    }
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    UINT32 preNum = g_curIrqNum[ArchCurrCpuid()];
#endif
    g_curIrqNum[ArchCurrCpuid()] = hwiNum;
    OsIntHandle(hwiNum, HWI_FORM_ADDR_GET(hwiNum));
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    if (OsIrqNestingCntGet() > 0) {
        g_curIrqNum[ArchCurrCpuid()] = preNum;
    }
#endif
}

#ifdef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
VOID HalIrqHandler(UINT32 hwiNum)
{
    HalIrqHandlerCall(hwiNum);
}
#else
STATIC VOID HalIrqHandler(VOID)
{
    HalIrqHandlerCall(READ_CSR(MCAUSE) & 0xfffU);
}
#endif

#ifdef LOSCFG_DEBUG_HWI
STATIC INLINE UINT32 GetUsrPri(UINT32 sysPrior)
{
    return ((LOSCFG_HWI_PRIO_LIMIT - 1) + CLIC_CFG_INTERRUPT_OFFSET - (sysPrior));
}

UINT32 HalIrqGetPrio(UINT32 hwiNum)
{
    UINT32 clicIntCtl = GET_UINT8(REG_CLIC_INTCTL(hwiNum));

    return GetUsrPri((clicIntCtl >> (8 - g_clicCfgLevelBitsNum)));  /* max 8 bit */
}

STATIC UINT32 HalIrqStatusGet(HWI_HANDLE_T hwiNum, HwiStatus *status)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    status->pri = HalIrqGetPrio(hwiNum);
    status->affinity = 0x1U;
    READ_UINT8(status->enable, REG_CLIC_INTIE(hwiNum));

    (VOID)HalIrqPendingGet(hwiNum, (UINT8 *)&status->pending);
    return LOS_OK;
}
#endif

#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
STATIC const HwiControllerOps g_clicOps = {
    .triggerIrq     = HalIrqTrigger,
    .clearIrq       = HalIrqClear,
    .enableIrq      = HalIrqUnmask,
    .disableIrq     = HalIrqMask,
    .setIrqPriority = HalIrqSetPrio,
    .getCurIrqNum   = HalCurIrqGet,
    .getIrqVersion  = HalIrqVersion,
    .getHandleForm  = HalIrqGetHandleForm,
    .handleIrq      = HalIrqHandler,
#ifdef LOSCFG_DEBUG_HWI
    .getIrqStatus   = HalIrqStatusGet,
#endif
};
#endif

LITE_OS_SEC_TEXT_INIT VOID HalIrqInit(VOID)
{
    /* set cliccfg reg. nvbits = 0; nlbits = 4; nmbits = 0. */
    UINT8 cliccfg = 0;
    cliccfg |= (4U << 1); /* non-vector, m-mode */
    WRITE_UINT8(cliccfg, REG_CLIC_CFG);

    for (UINT32 i = 0; i < CLIC_CFG_REGISTER_BITS_NUM; i++) {
        if (((UINT8)(LOSCFG_HWI_PRIO_LIMIT - 1) & ((0x80U) >> i)) != 0) {
            g_clicCfgLevelBitsNum = 8U - i; /* max 8bit */
            break;
        }
    }
    UINT8 clicintattr = 0;
    /* clicintattr[0] = 0 non-vector;
     * clicintattr[1] = 0 level-triggered,1 edge-triggered;
     * clicintattr[2] = 0 hight level, 1 low-level;
     * clicintattr[7:6] = 3 M-mode
    */
    clicintattr |= (3U << 6);
    UINT32 idx;
    for (idx = 0; idx < LOSCFG_PLATFORM_HWI_LIMIT; idx++) {
#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
        g_vectorIntTable[idx] = (HWI_PROC_FUNC)OsIntEntry;
#endif
#ifndef LOSCFG_HWI_FORM_DYNAMIC_ALLOCATION
        g_hwiForm[idx].hook = NULL;
        g_hwiForm[idx].shareMode = 0;
#ifdef LOSCFG_SHARED_IRQ
        g_hwiForm[idx].next = NULL;
#endif
#endif

        WRITE_UINT8(0, REG_CLIC_INTIP(idx));      /* clear pending */
        WRITE_UINT8(0, REG_CLIC_INTIE(idx));      /* disabled */
        WRITE_UINT8(clicintattr, REG_CLIC_INTATTR(idx));
    }

#ifndef LOSCFG_HWI_CONTROLLER_DIRECT_CALL
    OsHwiControllerReg(&g_clicOps);
#endif
}
