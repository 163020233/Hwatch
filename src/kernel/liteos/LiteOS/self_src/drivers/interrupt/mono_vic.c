/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: Implementation Of The Arm VIC Interrupt.
 * Author: Huawei LiteOS Team
 * Create: 2021-09-01
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

#include "mono_vic.h"
#include "los_typedef.h"
#include "los_hwi_pri.h"

LITE_OS_SEC_BSS HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = { 0 };

UINT32 HalCurIrqGet(VOID)
{
    UINT32 hwiNum;
    UINT32 irqStatus;

    READ_UINT32(irqStatus, IRQ_FINALSTATUS_L);
    if (irqStatus != 0) {
        irqStatus = irqStatus & ((irqStatus - 1) ^ (UINT32)(-1));
        hwiNum = OS_USER_HWI_MAX - CLZ(irqStatus) - (IRQSRC_WORD_L + 1);
        return hwiNum;
    }

    READ_UINT32(irqStatus, IRQ_FINALSTATUS_H);
    if (irqStatus != 0) {
        irqStatus = irqStatus & ((irqStatus - 1) ^ (UINT32)(-1));
        hwiNum = OS_USER_HWI_MAX - CLZ(irqStatus);
        return hwiNum;
    }

    return 0;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    UINT32 imr;
    UINT32 status = 0;

    if (hwiNum > OS_USER_HWI_MAX) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum <= IRQSRC_WORD_L) {
        imr = ~(1 << hwiNum);
        READ_UINT32(status, IRQ_INTFORCE_L);
        WRITE_UINT32(imr & status, IRQ_INTFORCE_L);
    } else {
        imr = ~(1 << (hwiNum - IRQSRC_WORD_L - 1));
        READ_UINT32(status, IRQ_INTFORCE_H);
        WRITE_UINT32(imr & status, IRQ_INTFORCE_H);
    }
    return LOS_OK;
}

STATIC VOID HalIrqHandler(VOID)
{
    UINT32 hwiNum = HalCurIrqGet();
    if (!HWI_NUM_VALID(hwiNum)) {
        return;
    }

    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
    (VOID)HalIrqClear(hwiNum);
}

UINT32 HalIrqMask(HWI_HANDLE_T hwiNum)
{
    UINT32 imr;
    UINT32 status;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum <= IRQSRC_WORD_L) {
        imr = 1 << hwiNum;
        READ_UINT32(status, IRQ_INTMASK_L);
        WRITE_UINT32(imr | status, IRQ_INTMASK_L);
    } else {
        imr = 1 << (hwiNum - IRQSRC_WORD_L - 1);
        READ_UINT32(status, IRQ_INTMASK_H);
        WRITE_UINT32(imr | status, IRQ_INTMASK_H);
    }

    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    UINT32 imr;
    UINT32 status;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum <= IRQSRC_WORD_L) {
        imr = 1 << hwiNum;
        READ_UINT32(status, IRQ_INTEN_L);
        WRITE_UINT32(imr | status, IRQ_INTEN_L);

        imr = ~(1 << hwiNum);
        READ_UINT32(status, IRQ_INTMASK_L);
        WRITE_UINT32(imr & status, IRQ_INTMASK_L);
    } else {
        imr = 1 << (hwiNum - IRQSRC_WORD_L - 1);
        READ_UINT32(status, IRQ_INTEN_H);
        WRITE_UINT32(imr | status, IRQ_INTEN_H);

        imr = ~(1 << (hwiNum - IRQSRC_WORD_L - 1));
        READ_UINT32(status, IRQ_INTMASK_H);
        WRITE_UINT32(imr & status, IRQ_INTMASK_H);
    }

    return LOS_OK;
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    UINT32 imr;
    UINT32 status;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum <= IRQSRC_WORD_L) {
        imr = (1 << hwiNum);
        READ_UINT32(status, IRQ_INTFORCE_L);
        WRITE_UINT32(imr | status, IRQ_INTFORCE_L);
    } else {
        imr = (1 << (hwiNum - IRQSRC_WORD_L - 1));
        READ_UINT32(status, IRQ_INTFORCE_H);
        WRITE_UINT32(imr | status, IRQ_INTFORCE_H);
    }

    return LOS_OK;
}

CHAR *HalIrqVersion(VOID)
{
    return "ARM VIC";
}

STATIC HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }

    return &g_hwiForm[hwiNum];
}

STATIC const HwiControllerOps g_vicOps = {
    .clearIrq       = HalIrqClear,
    .triggerIrq     = HalIrqTrigger,
    .enableIrq      = HalIrqUnmask,
    .disableIrq     = HalIrqMask,
    .setIrqPriority = HalIrqSetPrio,
    .getCurIrqNum   = HalCurIrqGet,
    .getIrqVersion  = HalIrqVersion,
    .getHandleForm  = HalIrqGetHandleForm,
    .handleIrq      = HalIrqHandler,
};

VOID HalIrqInit(VOID)
{
    WRITE_UINT32(0xffffffff, IRQ_INTEN_L);      /* enable all irq */
    WRITE_UINT32(0xffffffff, IRQ_INTMASK_L);    /* mask all irq */
    WRITE_UINT32(0xffffffff, IRQ_INTEN_H);      /* enable all irq */
    WRITE_UINT32(0xffffffff, IRQ_INTMASK_H);    /* mask all irq */

    OsHwiControllerReg(&g_vicOps);
}
