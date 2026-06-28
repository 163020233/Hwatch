/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2021. All rights reserved.
 * Description: General interrupt controller version 2.0 (GICv2).
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
 * Notes: Reference from arm documents:
 *        https://static.docs.arm.com/ihi0048/bb/IHI0048B_b_gic_architecture_specification.pdf
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

#include "gic_common.h"
#include "gic_v2.h"
#include "los_hwi_pri.h"
#include "los_mp_pri.h"

STATIC UINT32 g_curIrqNum[LOSCFG_KERNEL_CORE_NUM] = { 0 };
STATIC HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT] = { 0 };
#if defined(LOSCFG_DEBUG_HWI) || defined(LOSCFG_KERNEL_SMP)
#define PROCESSOR_TARGETS_MASK  0x3
#endif

#ifdef LOSCFG_KERNEL_SMP
#define INTID_MASK          0xF
#define FILTER_MASK         0x3
#define FILTER_SHIFT        24
#define CPU_TARGET_MASK     0xFF
#define CPU_TARGET_SHIFT    16
/*
 * filter description
 *   0b00: forward to the cpu interfaces specified in cpu_mask
 *   0b01: forward to all cpu interfaces
 *   0b10: forward only to the cpu interface that request the irq
 */
STATIC VOID GicWriteSgi(UINT32 intId, UINT32 cpuMask, UINT32 filter)
{
    UINT32 val = ((filter & FILTER_MASK) << FILTER_SHIFT) |
                 ((cpuMask & CPU_TARGET_MASK) << CPU_TARGET_SHIFT) |
                 (intId & INTID_MASK);

    GIC_REG_32(GICD_SGIR) = val;
}

UINT32 HalIrqSendIpi(UINT32 target, UINT32 ipi)
{
    if (!HWI_NUM_VALID(ipi)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    GicWriteSgi(ipi, target, 0);
    return LOS_OK;
}

UINT32 HalIrqSetAffinity(UINT32 hwiNum, UINT32 cpuMask)
{
    UINT32 offset = hwiNum / INT_COUNTS_4_PER_REG;
    UINT32 index = hwiNum & PROCESSOR_TARGETS_MASK;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (((cpuMask & LOSCFG_KERNEL_CPU_MASK) != cpuMask) || (cpuMask > UINT8_MAX)) {
        return LOS_ERRNO_HWI_AFFI_INVALID;
    }

    GIC_REG_8(GICD_ITARGETSR(offset) + index) = (UINT8)cpuMask;
    return LOS_OK;
}
#endif

UINT32 HalCurIrqGet(VOID)
{
    return g_curIrqNum[ArchCurrCpuid()];
}

UINT32 HalIrqMask(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    GIC_REG_32(GICD_ICENABLER(hwiNum >> BIT_SHIFT_5)) = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    GIC_REG_32(GICD_ISENABLER(hwiNum >> BIT_SHIFT_5)) = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    GIC_REG_32(GICD_ISPENDR(hwiNum >> BIT_SHIFT_5)) = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    GIC_REG_32(GICD_ICPENDR(hwiNum >> BIT_SHIFT_5)) = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    GIC_REG_32(GICC_EOIR) = hwiNum;
    return LOS_OK;
}

STATIC VOID HalIrqHandler(VOID)
{
    UINT32 iar = GIC_REG_32(GICC_IAR);
    UINT32 hwiNum = iar & 0x3FFU;

    if (hwiNum >= LOSCFG_PLATFORM_HWI_LIMIT) {
        return;
    }

    g_curIrqNum[ArchCurrCpuid()] = hwiNum;
    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);

    /* use original iar to do the EOI */
    GIC_REG_32(GICC_EOIR) = iar;
}

CHAR *HalIrqVersion(VOID)
{
    UINT32 pidr = GIC_REG_32(GICD_PIDR2V2);
    CHAR *irqVerString = NULL;

    switch (pidr >> GIC_REV_OFFSET) {
        case GICV1:
            irqVerString = "GICv1";
            break;
        case GICV2:
            irqVerString = "GICv2";
            break;
        default:
            irqVerString = "unknown";
    }
    return irqVerString;
}

STATIC HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }

    return &g_hwiForm[hwiNum];
}

VOID HalIrqInitPercpu(VOID)
{
    /* unmask interrupts */
    GIC_REG_32(GICC_PMR) = 0xFF;

    /* enable gic cpu interface */
    GIC_REG_32(GICC_CTLR) = 1;
}

STATIC UINT32 StubSetIrqPriority(HWI_HANDLE_T hwiNum, HWI_PRIOR_T priority)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    (VOID)hwiNum;
    (VOID)priority;
    return LOS_OK;
}

#ifdef LOSCFG_DEBUG_HWI
STATIC UINT32 GicGetAffinity(UINT32 hwiNum)
{
    if (hwiNum < GIC_MIN_SPI_NUM) {
        return 0;
    }

    UINT32 offset = hwiNum / INT_COUNTS_4_PER_REG;
    UINT32 index = hwiNum & PROCESSOR_TARGETS_MASK;
    return (UINT32)GIC_REG_8(GICD_ITARGETSR(offset) + index); /* 31: max bits of unit32 */
}

STATIC UINT32 HalIrqStatusGet(HWI_HANDLE_T hwiNum, HwiStatus *status)
{
    UINT32 bitPos = hwiNum % INT_COUNTS_32_PER_REG;

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    status->pri = OS_HWI_UNSUPPORTED_STATUS;
    status->enable = ((GIC_REG_32(GICD_ISENABLER(hwiNum >> BIT_SHIFT_5)) >> bitPos) & 1U);
    status->pending = ((GIC_REG_32(GICD_ISPENDR(hwiNum >> BIT_SHIFT_5)) >> bitPos) & 1U);
    status->affinity = GicGetAffinity(hwiNum);
    return LOS_OK;
}
#endif

STATIC const HwiControllerOps g_gicv2Ops = {
    .triggerIrq         = HalIrqTrigger,
    .clearIrq           = HalIrqClear,
    .enableIrq          = HalIrqUnmask,
    .disableIrq         = HalIrqMask,
    .setIrqPriority     = StubSetIrqPriority,
    .getCurIrqNum       = HalCurIrqGet,
    .getIrqVersion      = HalIrqVersion,
    .getHandleForm      = HalIrqGetHandleForm,
    .handleIrq          = HalIrqHandler,
#ifdef LOSCFG_KERNEL_SMP
    .sendIpi            = HalIrqSendIpi,
    .setIrqCpuAffinity  = HalIrqSetAffinity,
#endif
#ifdef LOSCFG_DEBUG_HWI
    .getIrqStatus       = HalIrqStatusGet,
#endif
};

VOID HalIrqInit(VOID)
{
    UINT32 i;

    /* set external interrupts to be level triggered, active low. */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_16_PER_REG) {
        GIC_REG_32(GICD_ICFGR(i >> BIT_SHIFT_4)) = 0;
    }

    /* set external interrupt to the corresponding CPU */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_4_PER_REG) {
        GIC_REG_32(GICD_ITARGETSR(i >> BIT_SHIFT_2)) = TARGET_CPU_MASK;
    }

    /* set priority on all interrupts */
    for (i = 0; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_4_PER_REG) {
        GIC_REG_32(GICD_IPRIORITYR(i >> BIT_SHIFT_2)) = GICD_INT_DEF_PRI_X4;
    }

    /* disable all interrupts. */
    for (i = 0; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_32_PER_REG) {
        GIC_REG_32(GICD_ICENABLER(i >> BIT_SHIFT_5)) = ~0;
    }

    HalIrqInitPercpu();

    /* enable gic distributor control */
    GIC_REG_32(GICD_CTLR) = 1;

    /* register interrupt controller's operations */
    OsHwiControllerReg(&g_gicv2Ops);
#ifdef LOSCFG_KERNEL_SMP
    UINT32 ret;

    /* register inter-processor interrupt */
    ret = LOS_HwiCreate(LOS_MP_IPI_WAKEUP, OS_HWI_PRIO_LOWEST, 0, OsMpWakeHandler, 0);
    ret |= LOS_HwiCreate(LOS_MP_IPI_SCHEDULE, OS_HWI_PRIO_LOWEST, 0, OsMpScheduleHandler, 0);
    ret |= LOS_HwiCreate(LOS_MP_IPI_HALT, OS_HWI_PRIO_LOWEST, 0, OsMpHaltHandler, 0);
#ifdef LOSCFG_KERNEL_SMP_CALL
    ret |= LOS_HwiCreate(LOS_MP_IPI_FUNC_CALL, OS_HWI_PRIO_LOWEST, 0, OsMpFuncCallHandler, 0);
#endif
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_HwiCreate failed, ret:0x%x\n", ret);
    }
#endif
}
