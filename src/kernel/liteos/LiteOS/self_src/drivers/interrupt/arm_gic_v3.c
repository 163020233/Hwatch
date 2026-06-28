/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description: General interrupt controller version 3.0 (GICv3).
 * Author: Huawei LiteOS Team
 * Create: 2018-09-15
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
#include "gic_v3.h"
#include "los_typedef.h"
#include "los_hwi_pri.h"
#include "los_mp_pri.h"
#include "los_task.h"
#ifdef LOSCFG_ARM_SIC
#include "sic.h"
#endif

STATIC UINT32 g_curIrqNum[LOSCFG_KERNEL_CORE_NUM];
STATIC HwiHandleInfo g_hwiForm[LOSCFG_PLATFORM_HWI_LIMIT];

#ifdef LOSCFG_KERNEL_SMP
STATIC UINT32 NextCpu(UINT32 cpu, UINT32 cpuMask)
{
    UINT32 next = cpu + 1;

    while (next < LOSCFG_KERNEL_CORE_NUM) {
        if (cpuMask & (1U << next)) {
            goto OUT;
        }

        next++;
    }

OUT:
    return next;
}

STATIC UINT16 GicTargetList(UINT32 *base, UINT32 cpuMask, UINT64 cluster)
{
    UINT32 nextCpu;
    UINT16 tList = 0;
    UINT32 cpu = *base;
    UINT64 mpidr = CPU_MAP_GET(cpu);

    while (cpu < LOSCFG_KERNEL_CORE_NUM) {
        tList |= 1U << (mpidr & 0xf);

        nextCpu = NextCpu(cpu, cpuMask);
        if (nextCpu >= LOSCFG_KERNEL_CORE_NUM) {
            goto out;
        }

        cpu = nextCpu;
        mpidr = CPU_MAP_GET(cpu);
        if (cluster != (mpidr & ~0xffUL)) {
            cpu--;
            goto out;
        }
    }

out:
    *base = cpu;
    return tList;
}

#define SGI1R_AFF1          1
#define SGI1R_AFF2          2
#define SGI1R_AFF3          3

#define SGI1R_AFF1_SHIFT    16
#define SGI1R_AFF2_SHIFT    32
#define SGI1R_AFF3_SHIFT    48
#define SGI1R_INTID_SHIFT   24
STATIC VOID GicSgi(UINT32 hwiNum, UINT32 cpuMask)
{
    UINT16 tList;
    UINT32 cpu = 0;
    UINT64 val, cluster;

    while ((cpuMask != 0) && (cpu < LOSCFG_KERNEL_CORE_NUM)) {
        if (cpuMask & (1U << cpu)) {
            cluster = CPU_MAP_GET(cpu) & ~0xffUL;

            tList = GicTargetList(&cpu, cpuMask, cluster);

            /* Generates a Group 1 interrupt for the current security state */
            val = ((MPIDR_AFF_LEVEL(cluster, SGI1R_AFF3) << SGI1R_AFF3_SHIFT) |
                   (MPIDR_AFF_LEVEL(cluster, SGI1R_AFF2) << SGI1R_AFF2_SHIFT) |
                   (MPIDR_AFF_LEVEL(cluster, SGI1R_AFF1) << SGI1R_AFF1_SHIFT) |
                   ((UINT64)hwiNum << SGI1R_INTID_SHIFT) | (UINT64)tList);

            GiccSetSgi1r(val);
        }

        cpu++;
    }
}

UINT32 HalIrqSendIpi(UINT32 target, UINT32 ipi)
{
    if (!HWI_NUM_VALID(ipi)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    GicSgi(ipi, target);
    return LOS_OK;
}
#endif

UINT32 HalIrqSetAffinity(UINT32 hwiNum, UINT32 cpuMask)
{
    UINT32 cpuId;

    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (cpuMask == 0) {
        return LOS_ERRNO_HWI_AFFI_INVALID;
    }

    if (hwiNum < GIC_MIN_SPI_NUM) {
        return LOS_OK;
    }

    cpuId = 31 - CLZ(cpuMask); /* 31: max bits of unit32 */
    if ((cpuId >= LOSCFG_KERNEL_CORE_NUM) || IS_CPU_MAP_INVALID(cpuId)) {
        return LOS_ERRNO_HWI_AFFI_INVALID;
    }

    /* When ARE is on, use router */
    GIC_REG_64(GICD_IROUTER(hwiNum)) = MpidrToAffinity(CPU_MAP_GET(cpuId));
    return LOS_OK;
}

STATIC INLINE VOID GicdSetGroup(UINT32 hwiNum)
{
    /* configure spi as group 0 on secure mode and group 1 on insecure mode */
    GicIntGroupSet(GICD_IGRPMODR(hwiNum >> BIT_SHIFT_5), GICD_IGROUPR(hwiNum >> BIT_SHIFT_5));
}

STATIC INLINE VOID GicrSetWaker(UINT32 cpu)
{
    GIC_REG_32(GICR_WAKER(cpu)) &= ~GICR_WAKER_PROCESSORSLEEP;
    DSB();
    ISB();
    while ((GIC_REG_32(GICR_WAKER(cpu)) & 0x4) == GICR_WAKER_CHILDRENASLEEP) {}
}

STATIC INLINE VOID GicrSetGroup(UINT32 cpu)
{
    /* configure sgi/ppi as group 0 on secure mode and group 1 on insecure mode */
    GicIntGroupSet(GICR_IGRPMOD0(cpu), GICR_IGROUPR0(cpu));
}

STATIC VOID GicdSetPmr(UINT32 hwiNum, UINT8 priority)
{
    UINT32 pos = hwiNum >> BIT_SHIFT_2; /* one irq have the 8-bit interrupt priority field */
    UINT32 newPri = GIC_REG_32(GICD_IPRIORITYR(pos));

    /* Shift and mask the correct bits for the priority */
    newPri &= ~(GIC_PRIORITY_MASK << ((hwiNum % INT_COUNTS_4_PER_REG) * GIC_PRIORITY_OFFSET));
    newPri |= (UINT32)priority << ((hwiNum % INT_COUNTS_4_PER_REG) * GIC_PRIORITY_OFFSET);

    GIC_REG_32(GICD_IPRIORITYR(pos)) = newPri;
}

STATIC VOID GicrSetPmr(UINT32 hwiNum, UINT8 priority)
{
    UINT32 cpu = GicrCpuIdGet();
    UINT32 pos = hwiNum >> BIT_SHIFT_2; /* one irq have the 8-bit interrupt priority field */
    UINT32 newPri = GIC_REG_32(GICR_IPRIORITYR(cpu, pos));

    /* Clear priority offset bits and set new priority */
    newPri &= ~(GIC_PRIORITY_MASK << ((hwiNum % INT_COUNTS_4_PER_REG) * GIC_PRIORITY_OFFSET));
    newPri |= (UINT32)priority << ((hwiNum % INT_COUNTS_4_PER_REG) * GIC_PRIORITY_OFFSET);

    GIC_REG_32(GICR_IPRIORITYR(cpu, pos)) = newPri;
}

STATIC VOID GiccInitPercpu(VOID)
{
    /* enable system register interface */
    UINT32 sre = GiccGetSre();

    if ((sre & 0x1) == 0) {
        GiccSetSre(sre | 0x1);

        /*
         * Need to check that the SRE bit has actually been set. If
         * not, it means that SRE is disabled at up EL level. We're going to
         * die painfully, and there is nothing we can do about it.
         */
        sre = GiccGetSre();
        LOS_ASSERT(sre & 0x1);
    }

#ifdef LOSCFG_ARCH_SECURE_MONITOR_MODE
    /* Enable group 0 and disable grp1ns grp1s interrupts */
    GiccSetIgrpen0(1);
    GiccSetIgrpen1(0);

    /*
     * For priority grouping.
     * The value of this field control show the 8-bit interrupt priority field
     * is split into a group priority field, that determines interrupt preemption,
     * and a subpriority field.
     */
    GiccSetBpr0(MAX_BINARY_POINT_VALUE);
#else
    /* enable group 1 interrupts */
    GiccSetIgrpen1(1);
    GiccSetBpr1(MAX_BINARY_POINT_VALUE);
#endif

    /* set priority threshold to max */
    GiccSetPmr(0xff);

    /* EOI deactivates interrupt too (mode 0) */
    GiccSetCtlr(0);
}


UINT32 HalCurIrqGet(VOID)
{
    return g_curIrqNum[ArchCurrCpuid()];
}

UINT32 HalIrqMask(UINT32 hwiNum)
{
    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);

    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum < GIC_MIN_SPI_NUM) {
#ifndef LOSCFG_ARM_SIC
        for (UINT32 i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
            GIC_REG_32(GICR_ICENABLER0(i + GICR_CLUSTER_CPU_ID_OFFSET)) = mask;
            GicWaitForRwp(GICR_CTLR(i + GICR_CLUSTER_CPU_ID_OFFSET));
        }
#else
        HalSicrIrqMask(hwiNum);
#endif
    } else {
        GIC_REG_32(GICD_ICENABLER(hwiNum >> BIT_SHIFT_5)) = mask;
        GicWaitForRwp(GICD_CTLR);
    }
    return LOS_OK;
}

UINT32 HalIrqUnmask(UINT32 hwiNum)
{
    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);

    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (hwiNum < GIC_MIN_SPI_NUM) {
#ifndef LOSCFG_ARM_SIC
        for (UINT32 i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
            GIC_REG_32(GICR_ISENABLER0(i + GICR_CLUSTER_CPU_ID_OFFSET)) = mask;
            GicWaitForRwp(GICR_CTLR(i + GICR_CLUSTER_CPU_ID_OFFSET));
        }
#else
        HalSicrIrqUnmask(hwiNum);
#endif
    } else {
        GIC_REG_32(GICD_ISENABLER(hwiNum >> BIT_SHIFT_5)) = mask;
        GicWaitForRwp(GICD_CTLR);
    }

    return LOS_OK;
}

UINT32 HalIrqTrigger(UINT32 hwiNum)
{
    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    const UINT32 mask = 1U << (hwiNum % INT_COUNTS_32_PER_REG);

    if (hwiNum < GIC_MIN_SPI_NUM) {
#ifndef LOSCFG_ARM_SIC
        UINT32 cpuId = GicrCpuIdGet();
        GIC_REG_32(GICR_ISPENDR0(cpuId)) = mask;
#else
        HalSicrTrigger(hwiNum);
#endif
    } else {
        GIC_REG_32(GICD_ISPENDR(hwiNum >> BIT_SHIFT_5)) = mask;
    }

    return LOS_OK;
}

UINT32 HalIrqClear(UINT32 hwiNum)
{
    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }
    GIC_REG_32(GICD_ICPENDR(hwiNum >> BIT_SHIFT_5)) = 1U << (hwiNum % INT_COUNTS_32_PER_REG);
    GiccSetEoir(hwiNum);
    ISB();
    return LOS_OK;
}

UINT32 HalIrqSetPrio(UINT32 hwiNum, UINT16 priority)
{
    UINT8 prio;

    GIC_LPI_CHECK_RETURN(hwiNum);

    if (!HWI_NUM_VALID(hwiNum)) {
        PRINT_ERR("Invalid irq value %u, max irq is %u\n", hwiNum, LOSCFG_PLATFORM_HWI_LIMIT);
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    if (!HWI_PRI_VALID(priority)) {
        return OS_ERRNO_HWI_PRIO_INVALID;
    }

    prio = (UINT8)(priority << PRIORITY_SHIFT);

    if (hwiNum >= GIC_MIN_SPI_NUM) {
        GicdSetPmr(hwiNum, prio);
    } else {
        GicrSetPmr(hwiNum, prio);
    }

    return LOS_OK;
}

STATIC HwiHandleInfo *HalIrqGetHandleForm(HWI_HANDLE_T hwiNum)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return NULL;
    }
    return &g_hwiForm[hwiNum];
}

STATIC VOID HalIrqHandler(VOID)
{
    UINT32 iar = GiccGetIar();
#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
    UINT32 hwiNum = iar & 0xFFFFU;
#else
    UINT32 hwiNum = iar & 0x3FFU;
#endif

    /*
     * invalid irq number, mainly the spurious interrupts 0x3ff,
     * valid irq ranges from 0~1019, we use LOSCFG_PLATFORM_HWI_LIMIT to do
     * the checking.
     */
    if (!HWI_NUM_VALID(hwiNum)) {
        return;
    }
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    UINT32 preNum = g_curIrqNum[ArchCurrCpuid()];
#endif
    g_curIrqNum[ArchCurrCpuid()] = hwiNum;
    OsIntHandle(hwiNum, &g_hwiForm[hwiNum]);
#ifdef LOSCFG_ARCH_INTERRUPT_PREEMPTION
    if (OsIrqNestingCntGet() > 0) {
        g_curIrqNum[ArchCurrCpuid()] = preNum;
    }
#endif
    GiccSetEoir(hwiNum);
}

CHAR *HalIrqVersion(VOID)
{
    UINT32 pidr = GIC_REG_32(GICD_PIDR2V3);
    CHAR *irqVerString = NULL;

    switch (pidr >> GIC_REV_OFFSET) {
        case GICV3:
            irqVerString = "GICv3";
            break;
        case GICV4:
            irqVerString = "GICv4";
            break;
        default:
            irqVerString = "unknown";
    }
    return irqVerString;
}

VOID HalIrqInitPercpu(VOID)
{
    UINT32 idx;
    UINT32 cpu = GicrCpuIdGet();

    GicrSetWaker(cpu);

#ifndef LOSCFG_ARM_SIC
    GicrSetGroup(cpu);
    GicWaitForRwp(GICR_CTLR(cpu));

    /* GICR: clear and mask sgi/ppi */
    GIC_REG_32(GICR_ICENABLER0(cpu)) = 0xffffffff;
    GIC_REG_32(GICR_ICPENDR0(cpu)) = 0xffffffff;

    GIC_REG_32(GICR_ISENABLER0(cpu)) = 0xffffffff;
#else
    HalSicrInitPercpu();
#endif
    for (idx = 0; idx < GIC_MIN_SPI_NUM; idx += 1) {
        GicrSetPmr(idx, GICR_INT_DEF_PRI);
    }

    GicWaitForRwp(GICR_CTLR(cpu));

    /* GICC init */
    GiccInitPercpu();

#ifdef LOSCFG_KERNEL_SMP
    /* unmask ipi interrupts */
    (void)HalIrqUnmask(LOS_MP_IPI_WAKEUP);
    (void)HalIrqUnmask(LOS_MP_IPI_SCHEDULE);
    (void)HalIrqUnmask(LOS_MP_IPI_HALT);
#endif
}

#ifdef LOSCFG_DEBUG_HWI
STATIC UINT32 GicGetAffinity(UINT32 hwiNum)
{
    UINT32 i;
    UINT64 regAffi;

    if (hwiNum < GIC_MIN_SPI_NUM) {
        return 0;
    }
    regAffi = GIC_REG_64(GICD_IROUTER(hwiNum));

    for (i = 0; i < LOSCFG_KERNEL_CORE_NUM; i++) {
        if (MpidrToAffinity(CPU_MAP_GET(i)) == regAffi) {
            return CPUID_TO_AFFI_MASK(i);
        }
    }
    return 0;
}

STATIC UINT32 GicGetEnableStatus(HWI_HANDLE_T hwiNum)
{
    UINT32 maskPos = hwiNum % INT_COUNTS_32_PER_REG;
    UINT32 enable;

    if (hwiNum < GIC_MIN_SPI_NUM) {
#ifndef LOSCFG_ARM_SIC
        UINT32 cpuId = GicrCpuIdGet();
        enable = ((GIC_REG_32(GICR_ISENABLER0(cpuId + GICR_CLUSTER_CPU_ID_OFFSET)) >> maskPos) & 1U);
#else
        enable = SicrGetEnableStatus(hwiNum);
#endif
    } else {
        enable = ((GIC_REG_32(GICD_ISENABLER(hwiNum >> BIT_SHIFT_5)) >> maskPos) & 1U);
    }
    return enable;
}

STATIC UINT32 GicGetPendingStatus(HWI_HANDLE_T hwiNum)
{
    UINT32 maskPos = hwiNum % INT_COUNTS_32_PER_REG;
    UINT32 pending;

    if (hwiNum < GIC_MIN_SPI_NUM) {
#ifndef LOSCFG_ARM_SIC
        UINT32 cpuId = GicrCpuIdGet();
        pending = ((GIC_REG_32(GICR_ISPENDR0(cpuId)) >> maskPos) & 1U);
#else
        pending = SicrGetPendingStatus(hwiNum);
#endif
    } else {
        pending = ((GIC_REG_32(GICD_ISPENDR(hwiNum >> BIT_SHIFT_5)) >> maskPos) & 1U);
    }
    return pending;
}

STATIC UINT32 GicGetPrioStatus(HWI_HANDLE_T hwiNum)
{
    UINT32 pri;
    UINT32 pos = hwiNum >> BIT_SHIFT_2; /* one irq have the 8-bit interrupt priority field */
    UINT32 priReg;
    UINT32 cpu;

    cpu = GicrCpuIdGet();
    if (hwiNum >= GIC_MIN_SPI_NUM) {
        priReg = GIC_REG_32(GICD_IPRIORITYR(pos));
    } else {
        priReg = GIC_REG_32(GICR_IPRIORITYR(cpu, pos));
    }
    pri = (priReg >> ((hwiNum % INT_COUNTS_4_PER_REG) * GIC_PRIORITY_OFFSET)) & GIC_PRIORITY_MASK;
    return pri >> PRIORITY_SHIFT;
}

STATIC UINT32 HalIrqStatusGet(HWI_HANDLE_T hwiNum, HwiStatus *status)
{
    if (!HWI_NUM_VALID(hwiNum)) {
        return LOS_ERRNO_HWI_NUM_INVALID;
    }

    GIC_LPI_CHECK_RETURN(hwiNum);

    status->pri = GicGetPrioStatus(hwiNum);
    status->enable = GicGetEnableStatus(hwiNum);
    status->pending = GicGetPendingStatus(hwiNum);
    status->affinity = GicGetAffinity(hwiNum);

    return LOS_OK;
}
#endif

STATIC const HwiControllerOps g_gicv3Ops = {
    .triggerIrq         = HalIrqTrigger,
    .clearIrq           = HalIrqClear,
    .enableIrq          = HalIrqUnmask,
    .disableIrq         = HalIrqMask,
    .setIrqPriority     = HalIrqSetPrio,
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

STATIC VOID HalGicdInit(VOID)
{
    UINT32 i;
    UINT32 reg;

    /* disable distributor */
    reg = GIC_REG_32(GICD_CTLR);
#if defined(LOSCFG_ARCH_SECURE_MONITOR_MODE) || defined(LOSCFG_ARCH_SECURE_OS_MODE)
    reg &= ~(CTLR_ENALBE_G0 | CTLR_ENABLE_G1NS | CTLR_ENABLE_G1S);
#else /* LOSCFG_ARCH_NON_SECURE_OS_MODE */
    reg &= ~(CTLR_ENALBE_G1 | CTLR_ENABLE_G1A);
#endif
    GIC_REG_32(GICD_CTLR) = reg;
    GicWaitForRwp(GICD_CTLR);
    ISB();

    /* set external interrupts to be level triggered, active low. */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_16_PER_REG) {
        GIC_REG_32(GICD_ICFGR(i >> BIT_SHIFT_4)) = 0;
    }

    /* config distributer, mask and clear all spis, set group x */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_32_PER_REG) {
        GIC_REG_32(GICD_ICENABLER(i >> BIT_SHIFT_5)) = 0xffffffff;
        GIC_REG_32(GICD_ICPENDR(i >> BIT_SHIFT_5)) = 0xffffffff;
        GicdSetGroup(i);
    }

    /* set spi priority as default */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i++) {
        GicdSetPmr(i, GICD_INT_DEF_PRI);
    }

    GicWaitForRwp(GICD_CTLR);

    /* disable all interrupts. */
    for (i = 0; i < LOSCFG_PLATFORM_HWI_LIMIT; i += INT_COUNTS_32_PER_REG) {
        GIC_REG_32(GICD_ICENABLER(i >> BIT_SHIFT_5)) = 0xffffffff;
    }

    /* enable distributor with ARE, group 1 enabled */
#if defined(LOSCFG_ARCH_SECURE_MONITOR_MODE)
    GIC_REG_32(GICD_CTLR) = CTLR_ENALBE_G0 | CTLR_ENABLE_G1NS | CTLR_ARE_S;
#elif defined(LOSCFG_ARCH_SECURE_OS_MODE)
    GIC_REG_32(GICD_CTLR) = CTLR_ENABLE_G1NS | CTLR_ENABLE_G1S | CTLR_ARE_S | CTLR_ARE_NS;
#else /* LOSCFG_ARCH_NON_SECURE_OS_MODE */
    GIC_REG_32(GICD_CTLR) = CTLR_ENABLE_G1A | CTLR_ARE_NS;
#endif

    /* set spi to boot cpu only. ARE must be enabled */
    for (i = GIC_MIN_SPI_NUM; i < LOSCFG_PLATFORM_HWI_LIMIT; i++) {
        (VOID)HalIrqSetAffinity(i, (0x1U << ArchCurrCpuid()));
    }
}

STATIC GIC_PROC_FUNC g_gicdInitHook = HalGicdInit;
STATIC VOID GicdHookProc(VOID)
{
    if (g_gicdInitHook != NULL) {
        g_gicdInitHook();
    }
}

VOID HalSetGicdHook(GIC_PROC_FUNC gicdHook)
{
    g_gicdInitHook = gicdHook;
}

GIC_PROC_FUNC HalGetGicdHook(VOID)
{
    return g_gicdInitHook;
}

#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
STATIC GIC_PROC_FUNC g_gicLpiInitHook = NULL;
STATIC VOID GicLpiHookProc(VOID)
{
    if (g_gicLpiInitHook != NULL) {
        g_gicLpiInitHook();
    }
}

VOID HalSetGicLpiHook(GIC_PROC_FUNC gicLpiHook)
{
    g_gicLpiInitHook = gicLpiHook;
}

GIC_PROC_FUNC HalGetGicLpiHook(VOID)
{
    return g_gicLpiInitHook;
}
#endif

VOID HalIrqInit(VOID)
{
    GicdHookProc();
    HalIrqInitPercpu();

#ifdef LOSCFG_ARM_GIC_LPI_ENABLE
    GicLpiHookProc();
#endif
    /* register interrupt controller's operations */
    OsHwiControllerReg(&g_gicv3Ops);
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
