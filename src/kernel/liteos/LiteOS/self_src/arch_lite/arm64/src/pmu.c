/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Perf Arm Pmu Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-10-17
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

#include "arch/pmu.h"
#include "perf_pmu_pri.h"
#include "gic_common.h"

OS_PMU_INTS(LOSCFG_KERNEL_CORE_NUM, g_pmuIrqNr);
STATIC HwPmu g_armPmu;

STATIC INLINE UINT32 PmcrRead(VOID)
{
    return (UINT32)AARCH64_SYSREG_READ(pmcr_el0);
}

STATIC INLINE VOID PmcrWrite(UINT32 value)
{
    value &= PMCR_MASK;
    AARCH64_SYSREG_WRITE(pmcr_el0, value);
    ISB();
}

STATIC INLINE UINT32 PmuOverflowed(UINT32 pmovsr)
{
    return pmovsr & PMU_OVERFLOWED_MASK;
}

STATIC INLINE UINT32 PmuCntOverflowed(UINT32 pmovsr, UINT32 index)
{
    return pmovsr & PMU_CNT2BIT(PMU_IDX2CNT(index));
}

STATIC INLINE UINT32 PmuCntValid(UINT32 index)
{
    return index <= PMU_IDX_MAX_COUNTER;
}

STATIC INLINE VOID PmuSelCnt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    AARCH64_SYSREG_WRITE(pmselr_el0, counter);
    ISB();
}

STATIC INLINE VOID PmuSetCntPeriod(UINT32 index, UINT32 period)
{
    if (!PmuCntValid(index)) {
        PRINT_ERR("CPU writing wrong counter %u\n", index);
    } else if (index == PMU_IDX_CYCLE_COUNTER) {
        const UINT64 value64 = PMU_PERIOD_MASK | period;
        AARCH64_SYSREG_WRITE(pmccntr_el0, value64);
        PRINT_DEBUG("%llx\n", AARCH64_SYSREG_READ(pmccntr_el0));
    } else {
        PmuSelCnt(index);
        AARCH64_SYSREG_WRITE(pmxevcntr_el0, period);
    }
}

STATIC INLINE VOID PmuBindEvt2Cnt(UINT32 index, UINT32 value)
{
    PmuSelCnt(index);
    value &= PMU_EVTYPE_MASK;
    AARCH64_SYSREG_WRITE(pmxevtyper_el0, value);
}

STATIC INLINE VOID PmuEnableCnt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    AARCH64_SYSREG_WRITE(pmcntenset_el0, PMU_CNT2BIT(counter));
}

STATIC INLINE VOID PmuDisableCnt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    AARCH64_SYSREG_WRITE(pmcntenclr_el0, PMU_CNT2BIT(counter));
}

STATIC INLINE VOID PmuEnableCntInterrupt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    AARCH64_SYSREG_WRITE(pmintenset_el1, PMU_CNT2BIT(counter));
}

STATIC INLINE VOID PmuDisableCntInterrupt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    AARCH64_SYSREG_WRITE(pmintenclr_el1, PMU_CNT2BIT(counter));
    ISB();
    AARCH64_SYSREG_WRITE(pmovsclr_el0, PMU_CNT2BIT(counter));
    ISB();
}

STATIC INLINE UINT32 PmuGetOverflowStatus(VOID)
{
    UINT64 flags;

    asm volatile("mrs %x0, pmovsclr_el0" : "=r" (flags));
    flags &= PMU_FLAG_MASK;
    asm volatile("msr pmovsclr_el0, %x0" :: "r" (flags));

    return (UINT32)flags; /* higher bits archtectually reserved */
}

STATIC VOID PmuEnableEvent(Event *event)
{
    UINT32 cnt = event->counter;

    if (!PmuCntValid(cnt)) {
        PRINT_ERR("CPU enabling wrong PMNC counter IRQ enable %u\n", cnt);
        return;
    }

    if (event->period == 0) {
        PRINT_INFO("event period value not valid, counter: %u\n", cnt);
        return;
    }
    /*
     * Enable counter and interrupt, and set the counter to count
     * the event that we're interested in.
     */
    UINT32 intSave = LOS_IntLock();

    PmuDisableCnt(cnt);
    PmuBindEvt2Cnt(cnt, event->eventId);
    PmuEnableCntInterrupt(cnt);
    PmuEnableCnt(cnt);

    LOS_IntRestore(intSave);

    PRINT_DEBUG("enabled event: %u cnt: %u\n", event->eventId, cnt);
}

STATIC VOID PmuDisableEvent(Event *event)
{
    UINT32 cnt = event->counter;

    if (!PmuCntValid(cnt)) {
        PRINT_ERR("CPU enabling wrong PMNC counter IRQ enable %u\n", cnt);
        return;
    }

    /*
     * Disable counter and interrupt
     */
    UINT32 intSave = LOS_IntLock();
    PmuDisableCnt(cnt);
    PmuDisableCntInterrupt(cnt);
    LOS_IntRestore(intSave);
}

STATIC INLINE VOID PmuStartAllCnt(VOID)
{
    UINT32 value = PmcrRead() | PMCR_E;
    if (g_armPmu.cntDivided == TRUE) {
        value |= PMCR_D;
    } else {
        value &= ~PMCR_D;
    }

    PmcrWrite(value);
    (VOID)LOS_HwiEnable(g_pmuIrqNr[ArchCurrCpuid()]);
}

STATIC INLINE VOID PmuStopAllCnt(VOID)
{
    /* Disable all counters */
    PmcrWrite(PmcrRead() & ~PMCR_E);

    (VOID)LOS_HwiDisable(g_pmuIrqNr[ArchCurrCpuid()]);
}

STATIC INLINE VOID PmuResetAllCnt(VOID)
{
    UINT32 index;
    for (index = PMU_IDX_CYCLE_COUNTER; index < PMU_IDX_MAX_COUNTER; index++) {
        PmuDisableCnt(index);
        PmuDisableCntInterrupt(index);
    }

    /* Initialize & Reset PMNC: C and P bits and D bits */
    UINT32 reg = PMCR_P | PMCR_C | ((g_armPmu.cntDivided == TRUE) ? PMCR_D : 0);
    PmcrWrite(reg);
}

STATIC VOID PmuSetEventPeriod(Event *event)
{
    if (event->period != 0) {
        PRINT_DEBUG("counter: %u, period: 0x%x\n", event->counter, event->period);
        UINT32 remain = event->period - (UINT32)(event->count % event->period);
        PmuSetCntPeriod(event->counter, PERIOD_EVENT_CALC(remain));
    }
}

STATIC UINT64 PmuReadEventCnt(Event *event)
{
    UINT64 value = 0;
    UINT32 index = event->counter;

    if (!PmuCntValid(index)) {
        PRINT_ERR("reading wrong counter %u\n", index);
        return value;
    } else if (index == PMU_IDX_CYCLE_COUNTER) {
        value = AARCH64_SYSREG_READ(pmccntr_el0) & PMU_READ_MASK;
    } else {
        PmuSelCnt(index);
        value = AARCH64_SYSREG_READ(pmxevcntr_el0);
    }

    if (value < PERIOD_EVENT_CALC(event->period)) {
        if (PmuCntOverflowed(PmuGetOverflowStatus(), event->counter)) {
            value += event->period;
        }
    } else {
        value -= PERIOD_EVENT_CALC(event->period);
    }

    return value;
}

STATIC const UINT32 g_armMap[] = {
    [PERF_COUNT_HW_CPU_CYCLES]          = PERF_HW_CLOCK_CYCLES,
    [PERF_COUNT_HW_INSTRUCTIONS]        = PERF_HW_INSTR_EXECUTED,
    [PERF_COUNT_HW_DCACHE_REFERENCES]   = PERF_HW_L1_DCACHE_ACCESS,
    [PERF_COUNT_HW_DCACHE_MISSES]       = PERF_HW_L1_DCACHE_REFILL,
    [PERF_COUNT_HW_ICACHE_REFERENCES]   = PERF_HW_L1_ICACHE_ACCESS,
    [PERF_COUNT_HW_ICACHE_MISSES]       = PERF_HW_L1_ICACHE_REFILL,
    [PERF_COUNT_HW_BRANCH_INSTRUCTIONS] = PERF_HW_PC_WRITE,
    [PERF_COUNT_HW_BRANCH_MISSES]       = PERF_HW_PC_BRANCH_MIS_PRED,
};

STATIC UINT32 PmuMapEvent(UINT32 eventType, UINT32 *result, UINT32 len, BOOL reverse)
{
    (VOID)len;
    if (reverse == PERF_EVENT_TO_CODE) {  /* common event to arm real event */
        if (eventType < ARRAY_SIZE(g_armMap)) {
            result[0] = g_armMap[eventType];
            return 1;
        }
    } else {        /* arm real event to common event */
        UINT32 i;
        for (i = 0; i < ARRAY_SIZE(g_armMap); i++) {
            if (g_armMap[i] == eventType) {
                result[0] = i;
                return 1;
            }
        }
    }
    result[0] = PERF_HW_INVALID_EVENT_TYPE;
    return 1;
}

STATIC VOID PmuIrqHandler(VOID)
{
    UINT32 index;
    PerfRegs regs;
    PerfEvent *events = g_armPmu.pmu.current[ArchCurrCpuid()];
    UINT32 eventNum = events->nr;

    /* Get and reset the IRQ flags */
    UINT32 pmovsr = PmuGetOverflowStatus();
    if (!PmuOverflowed(pmovsr)) {
        return;
    }

    (VOID)memset(&regs, 0, sizeof(PerfRegs));
    OsPerfFetchIrqRegs(&regs);

    PmuStopAllCnt();

    for (index = 0; index < eventNum; index++) {
        Event *event = &(events->per[index]);
        /*
         * We have a single interrupt for all counters. Check that
         * each counter has overflowed before we process it.
         */
        if (!PmuCntOverflowed(pmovsr, event->counter) || (event->period == 0)) {
            continue;
        }

        OsPerfHwPmuHandleOverflow(event, &regs);
    }
    PmuStartAllCnt();
}

UINT32 OsGetPmuMaxCounter(VOID)
{
    return PMU_IDX_MAX_COUNTER;
}

UINT32 OsGetPmuCycleCounter(VOID)
{
    return PMU_IDX_CYCLE_COUNTER;
}

UINT32 OsGetPmuCounter0(VOID)
{
    return PMU_IDX_COUNTER0;
}

STATIC HwPmu g_armPmu = {
    .canDivided = TRUE,
    .enable     = PmuEnableEvent,
    .disable    = PmuDisableEvent,
    .start      = PmuStartAllCnt,
    .stop       = PmuStopAllCnt,
    .clear      = PmuResetAllCnt,
    .setPeriod  = PmuSetEventPeriod,
    .readCnt    = PmuReadEventCnt,
    .mapEvent   = PmuMapEvent,
};

UINT32 OsHwPmuInit(VOID)
{
    UINT32 ret;
    UINT32 index;

    for (index = 0; index < LOSCFG_KERNEL_CORE_NUM; index++) {
        ret = LOS_HwiCreate(g_pmuIrqNr[index], 0, 0, PmuIrqHandler, 0);
        if (ret != LOS_OK) {
            PRINT_ERR("pmu %u irq handler register failed\n", g_pmuIrqNr[index]);
            return ret;
        }
        if (g_pmuIrqNr[index] < GIC_MIN_SPI_NUM) {
            break;
        }

#ifdef LOSCFG_KERNEL_SMP
        ret = HalIrqSetAffinity(g_pmuIrqNr[index], CPUID_TO_AFFI_MASK(index));
        if (ret != LOS_OK) {
            PRINT_ERR("The interrupt %u is invalid, irq set affinity failed.\n", g_pmuIrqNr[index]);
            return ret;
        }
#endif
    }
    ret = OsPerfHwInit(&g_armPmu);
    return ret;
}

VOID OsHwPmuDeinit(VOID)
{
    UINT32 index;

    OsPerfHwDeinit(&g_armPmu);
    for (index = 0; index < LOSCFG_KERNEL_CORE_NUM; index++) {
        (VOID)LOS_HwiDisable(g_pmuIrqNr[index]);
        (VOID)LOS_HwiDelete(g_pmuIrqNr[index], NULL);
        if (g_pmuIrqNr[index] < GIC_MIN_SPI_NUM) {
            break;
        }
    }
}
