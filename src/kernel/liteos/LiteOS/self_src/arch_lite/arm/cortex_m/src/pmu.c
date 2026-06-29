/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Perf Armv8m Pmu Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-06-09
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

#include "perf_pmu_pri.h"
#include "los_hwi.h"
#include "arch/pmu.h"

#define IRQ_CONTEXT_PCOFFSET     24

STATIC HwPmu g_armPmu;

STATIC INLINE BOOL PmuEnvCheck(VOID)
{
    /* check the Data Watchpoint and Trace(DWT) unit is enable */
    if ((GET_UINT32(DWT_CTRL) & DWT_ENABLE_MASK) > 0) {
        return TRUE;
    }
    return FALSE;
}

STATIC INLINE UINT32 PmuCountersNum(VOID)
{
    /* Number of counters implemented in addition to the cycle counter */
    return GET_UINT32(PMU_TYPE) & TYPE_N_MASK;
}

STATIC INLINE VOID PmuDisableCnt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    WRITE_UINT32(PMU_CNT2BIT(counter), PMU_CNTENCLR);
}

STATIC INLINE VOID PmuEnableCnt(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    WRITE_UINT32(PMU_CNT2BIT(counter), PMU_CNTENSET);
}

STATIC INLINE VOID PmuBindEvt2Cnt(UINT32 index, UINT32 value)
{
    UINT32 counter = PMU_IDX2CNT(index);
    WRITE_UINT32(value, PMU_EVTYPERn(counter));
}

STATIC INLINE VOID PmuClearOverflowStatus(UINT32 index)
{
    UINT32 counter = PMU_IDX2CNT(index);
    WRITE_UINT32(PMU_CNT2BIT(counter), PMU_OVSCLR);
}

STATIC INLINE UINT32 PmuGetOverflowStatus(VOID)
{
    return GET_UINT32(PMU_OVSCLR);
}

STATIC INLINE VOID PmuResetOverflowStatus(UINT32 value)
{
    WRITE_UINT32(value, PMU_OVSCLR);
}

STATIC INLINE UINT32 PmuGetAndResetOfs(VOID)
{
    UINT32 value = PmuGetOverflowStatus();
    PmuResetOverflowStatus(value);
    return value;
}

STATIC INLINE VOID PmuEnableCntInterrupt(UINT32 index)
{
    UINT32 value = PMU_IDX2CNT(index);
    WRITE_UINT32(PMU_CNT2BIT(value), PMU_INTENSET);
}

STATIC INLINE VOID PmuDisableCntInterrupt(UINT32 index)
{
    UINT32 value = PMU_IDX2CNT(index);
    WRITE_UINT32(PMU_CNT2BIT(value), PMU_INTENCLR);
}

STATIC INLINE VOID PmuEnableMonitor(BOOL canPendforUnpri)
{
    UINT32 value;

    READ_UINT32(value, DEMCR_REG);
    value &= ~(DEMCR_MON_EN_MASK | DEMCR_UMON_EN_MASK);
    value |= DEMCR_MON_EN_MASK;
    value |= DEMCR_MON_TRCENA_MASK;
    if (canPendforUnpri) {
        value |= DEMCR_UMON_EN_MASK;
    }
    WRITE_UINT32(value, DEMCR_REG);
}

STATIC INLINE VOID PmuDisableMonitor(VOID)
{
    UINT32 value;

    READ_UINT32(value, DEMCR_REG);
    value &= ~(DEMCR_MON_EN_MASK | DEMCR_UMON_EN_MASK | DEMCR_MON_TRCENA_MASK);
    WRITE_UINT32(value, DEMCR_REG);
}

STATIC INLINE UINT32 PmuCntOverflowed(UINT32 status, UINT32 index)
{
    return (status & PMU_CNT2BIT(PMU_IDX2CNT(index)));
}

STATIC INLINE VOID PmuSetCntPeriod(UINT32 index, UINT32 period)
{
    UINT32 counter = PMU_IDX2CNT(index);
    UINT32 regAddr;
    UINT32 value;

    if (index > PmuCountersNum()) {
        PRINT_ERR("pmu counter index invalid %u\n", counter);
        return;
    }

    if (index == PMU_IDX_CYCLE_COUNTER) {
        regAddr = PMU_CCNTR;
        value = PERIOD_CYCLE_CALC(period);
    } else {
        regAddr = PMU_EVCNTRn(counter);
        value = PERIOD_EVENT_CALC(period);
    }
    WRITE_UINT32(value, regAddr);
}

STATIC VOID PmuEnableEvent(Event *event)
{
    UINT32 cnt = event->counter;

    if (cnt > PmuCountersNum()) {
        PRINT_ERR("pmu counter index invalid %u\n", cnt);
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

    /*
     * Set event (if destined for PMNx counters)
     * We only need to set the event for the cycle counter if we
     * have the ability to perform event filtering.
     */
    if (cnt != PMU_IDX_CYCLE_COUNTER) {
        PmuBindEvt2Cnt(cnt, event->eventId & EVENT_TYPE_MASK);
    }

    PmuEnableCntInterrupt(cnt);
    PmuEnableCnt(cnt);
    LOS_IntRestore(intSave);

    PRINT_DEBUG("enabled event: %u cnt: %u\n", event->eventId, cnt);
}

STATIC VOID PmuDisableEvent(Event *event)
{
    UINT32 cnt = event->counter;

    if (cnt > PmuCountersNum()) {
        PRINT_ERR("pmu counter index invalid %u\n", cnt);
        return;
    }

    UINT32 intSave = LOS_IntLock();
    PmuDisableCnt(cnt);
    PmuDisableCntInterrupt(cnt);
    PmuClearOverflowStatus(cnt);
    LOS_IntRestore(intSave);
}

STATIC VOID PmuStartAllCnt(VOID)
{
    UINT32 value = GET_UINT32(PMU_CTRL) | CTRL_E_BIT | CTRL_FZO_BIT;

    WRITE_UINT32(value, PMU_CTRL);
    PmuEnableMonitor(FALSE);
}

STATIC VOID PmuStopAllCnt(VOID)
{
    UINT32 value = GET_UINT32(PMU_CTRL) & ~CTRL_E_BIT;
    WRITE_UINT32(value, PMU_CTRL);
    PmuDisableMonitor();
}

STATIC VOID PmuResetAllCnt(VOID)
{
    UINT32 index;
    UINT32 conterNum = PmuCountersNum();
    UINT32 value = CTRL_P_BIT | CTRL_C_BIT;

    /* The counter and interrupt enable registers are unknown at reset. */
    for (index = PMU_IDX_CYCLE_COUNTER; index <= conterNum; index++) {
        PmuDisableCnt(index);
        PmuDisableCntInterrupt(index);
        PmuClearOverflowStatus(index);
    }

    /* Initialize & Reset: C and P bits */
    WRITE_UINT32(value, PMU_CTRL);
}

STATIC VOID PmuSetEventPeriod(Event *event)
{
    if (event->period != 0) {
        PRINT_DEBUG("counter: %u, period: 0x%x\n", event->counter, event->period);
        UINT32 remain = event->period - (UINT32)(event->count % event->period);
        PmuSetCntPeriod(event->counter, remain);
    }
}

STATIC UINT64 PmuReadEventCnt(Event *event)
{
    UINT32 value = 0;
    UINT32 index = event->counter;
    UINT32 periodBase;

    if (index > PmuCountersNum()) {
        PRINT_ERR("pmu counter index invalid %u\n", index);
        return 0;
    }

    if (index == PMU_IDX_CYCLE_COUNTER) {
        READ_UINT32(value, PMU_CCNTR);
        periodBase = PERIOD_CYCLE_CALC(event->period);
    } else {
        READ_UINT32(value, PMU_EVCNTRn(PMU_IDX2CNT(index)));
        periodBase = PERIOD_EVENT_CALC(event->period);
    }

    if (value < periodBase) {
        if (PmuCntOverflowed(PmuGetAndResetOfs(), event->counter)) {
            value += event->period;
        }
    } else {
        value -= periodBase;
    }

    return (UINT64)value;
}

STATIC const UINT32 g_armMap[] = {
    [PERF_COUNT_HW_CPU_CYCLES]          = EVENT_CYCLES,
    [PERF_COUNT_HW_INSTRUCTIONS]        = EVENT_INST_RETIRED,
    [PERF_COUNT_HW_DCACHE_REFERENCES]   = EVENT_L1D_CACHE,
    [PERF_COUNT_HW_DCACHE_MISSES]       = EVENT_L1D_CACHE_REFILL,
    [PERF_COUNT_HW_ICACHE_REFERENCES]   = EVENT_L1I_CACHE,
    [PERF_COUNT_HW_ICACHE_MISSES]       = EVENT_L1I_CACHE_REFILL,
    [PERF_COUNT_HW_BRANCH_INSTRUCTIONS] = EVENT_PC_WRITE_RETIRED,
    [PERF_COUNT_HW_BRANCH_MISSES]       = EVENT_BR_MIS_PRED,
};

STATIC UINT32 PmuMapEvent(UINT32 eventType, UINT32 *result, UINT32 len, BOOL reverse)
{
    (VOID)len;
    UINT32 i;

    if (!reverse) {  /* Common event to arm real event */
        if (eventType < ARRAY_SIZE(g_armMap)) {
            result[0] = g_armMap[eventType];
            return 1;
        }
    } else { /* Real arm event to common event */
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

VOID OsPmuIrqHandler(VOID)
{
    UINT32 index = 0;
    PerfRegs regs;
    UINT32 irqStatus = 0;
    PerfEvent *events = g_armPmu.pmu.current[ArchCurrCpuid()];
    UINT32 eventNum = events->nr;

    (VOID)memset_s(&regs, sizeof(PerfRegs), 0, sizeof(PerfRegs));

    /* Get the IRQ flags */
    irqStatus = PmuGetOverflowStatus();
    if (irqStatus == 0) {
        return;
    }

    /* get task sp point and pc */
    __asm__ __volatile__("MRS %0, PSP" : "=r"(regs.fp));
    regs.pc = *(UINT32 *)(regs.fp + IRQ_CONTEXT_PCOFFSET);

    for (index = 0; index < eventNum; index++) {
        Event *event = &(events->per[index]);
        /*
         * We have a single interrupt for all counters. Check that
         * each counter has overflowed before we process it.
         */
        if (!PmuCntOverflowed(irqStatus, event->counter) || (event->period == 0)) {
            continue;
        }

        OsPerfHwPmuHandleOverflow(event, &regs);
    }
    PmuResetOverflowStatus(irqStatus);
}

UINT32 OsGetPmuMaxCounter(VOID)
{
    return PmuCountersNum() + 1;
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
    .canDivided = FALSE,
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

    if (PmuEnvCheck()) {
        PRINT_ERR("pmu coexistence with the dwt performance monitors\n");
        return LOS_NOK;
    }

    if (PmuCountersNum() == 0) {
        PRINT_ERR("pmu is unimplemented\n");
        return LOS_NOK;
    }

    ret = OsPerfHwInit(&g_armPmu);
    return ret;
}

VOID OsHwPmuDeinit(VOID)
{
    OsPerfHwDeinit(&g_armPmu);
    PmuStopAllCnt();
}
