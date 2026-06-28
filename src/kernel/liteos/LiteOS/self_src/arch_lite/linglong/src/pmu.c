/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: LiteOS Perf LingLong Pmu Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2022-07-18
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
#include "arch/regs.h"
#include "los_hwi.h"
#include "linglong_ic.h"
#include "perf_pmu_pri.h"

#define PMU_CYCLE_COUNTER_ID            0
#define PMU_COUNTER0_ID                 1
#define PMU_INVALID_EVENT_TYPE          0

#define OCD_DPCTL_ACT                   (0x1U << 7)             /* Controls OCP-PM Interrupt Generating */
#define OCD_DPCTL_MAN_DIS               (0x1U << 30)            /* Manual Disable PMU */
#define OCD_DPCTL_MAN_EN                (0x1U << 31)            /* Manual enable PMU */

#define OCD_ALL_COUNTER_MASK            ((0x1U << PMU_MAX_COUNTER_NUM) - 1)
#define OCD_DPCLR_CLRCNTX_MASK          OCD_ALL_COUNTER_MASK
#define OCD_DPSTAT_OVFX_MASK            OCD_ALL_COUNTER_MASK

typedef struct {
    UINT32 len;
    const UINT32 *map;
} PMU_MAP_ITEM;

#define PMU_HW_EVENT_DECLARE(hwId, ...)                             \
const UINT32 g_map_##hwId[] = {__VA_ARGS__};                        \
const PMU_MAP_ITEM g_pmuItem_##hwId = {                             \
    .len = (UINT32)ARRAY_SIZE(g_map_##hwId),                        \
    .map = g_map_##hwId,                                            \
}                                                                   \

#define PMU_HW_EVENT_REF(hwId)                    (&g_pmuItem_##hwId)

PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_CPU_CYCLES, PMU_EVENT_CORE_CLOCK);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_INSTRUCTIONS, PMU_EVENT_VLIW_EXECUTED);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_DCACHE_REFERENCES, \
    PMU_EVENT_DCACHE_CACHEABLE_LOAD_A, PMU_EVENT_DCACHE_CACHEABLE_LOAD_B);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_DCACHE_MISSES, PMU_EVENT_DCACHE_MISS_A, PMU_EVENT_DCACHE_MISS_B);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_ICACHE_REFERENCES, PMU_EVENT_ICACHE_CACHEABLE_ACCESS);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_ICACHE_MISSES, PMU_EVENT_ICACHE_MISS);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_BRANCH_INSTRUCTIONS, PMU_EVENT_PREDICTABLE_COF_INSTRUCTIONS);
PMU_HW_EVENT_DECLARE(PERF_COUNT_HW_BRANCH_MISSES, PMU_EVENT_MISPREDICTED_COF_INSTRUCTIONS);

STATIC const PMU_MAP_ITEM * const g_linglongMap[] = {
    [PERF_COUNT_HW_CPU_CYCLES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_CPU_CYCLES),
    [PERF_COUNT_HW_INSTRUCTIONS] = PMU_HW_EVENT_REF(PERF_COUNT_HW_INSTRUCTIONS),
    [PERF_COUNT_HW_DCACHE_REFERENCES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_DCACHE_REFERENCES),
    [PERF_COUNT_HW_DCACHE_MISSES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_DCACHE_MISSES),
    [PERF_COUNT_HW_ICACHE_REFERENCES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_ICACHE_REFERENCES),
    [PERF_COUNT_HW_ICACHE_MISSES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_ICACHE_MISSES),
    [PERF_COUNT_HW_BRANCH_INSTRUCTIONS] = PMU_HW_EVENT_REF(PERF_COUNT_HW_BRANCH_INSTRUCTIONS),
    [PERF_COUNT_HW_BRANCH_MISSES] = PMU_HW_EVENT_REF(PERF_COUNT_HW_BRANCH_MISSES),
};

STATIC UINT32 PmuCheckParam(Event *event)
{
    if (event->counter >= PMU_MAX_COUNTER_NUM) {
        return LOS_NOK;
    }

    if (event->period == 0) {
        return LOS_NOK;
    }

    return LOS_OK;
}

STATIC VOID PmuEnableEvent(Event *event)
{
    if (PmuCheckParam(event) != LOS_OK) {
        PRINT_ERR("PMU enable failed, invalid param\n");
        return;
    }

    UINT32 intSave = LOS_IntLock();
    WRITE_REG(OCD_DPESEL(event->counter), event->eventId);
    LOS_IntRestore(intSave);
}

STATIC VOID PmuDisableEvent(Event *event)
{
    if (PmuCheckParam(event) != LOS_OK) {
        PRINT_ERR("PMU disable failed, invalid param\n");
        return;
    }

    UINT32 intSave = LOS_IntLock();
    WRITE_REG(OCD_DPESEL(event->counter), PMU_INVALID_EVENT_TYPE);
    LOS_IntRestore(intSave);
}

STATIC VOID PmuStartAllCnt(VOID)
{
    (VOID)LOS_HwiEnable(LLIC_OCD_OV_INT_ID);

    UINT32 dpctl = READ_REG(OCD_DPCTL);
    dpctl |= OCD_DPCTL_ACT;     /* enable interrupt */
    dpctl |= OCD_DPCTL_MAN_EN;  /* enable pmu */
    WRITE_REG(OCD_DPCTL, dpctl);
}

STATIC VOID PmuStopAllCnt(VOID)
{
    UINT32 dpctl = READ_REG(OCD_DPCTL);
    dpctl &= ~OCD_DPCTL_ACT;     /* disable interrupt */
    dpctl |= OCD_DPCTL_MAN_DIS;  /* disable pmu */
    WRITE_REG(OCD_DPCTL, dpctl);

    (VOID)LOS_HwiDisable(LLIC_OCD_OV_INT_ID);
}

STATIC VOID PmuResetAllCnt(VOID)
{
    WRITE_REG(OCD_DPCLR, OCD_DPCLR_CLRCNTX_MASK);
}

STATIC VOID PmuSetEventPeriod(Event *event)
{
    if (PmuCheckParam(event) != LOS_OK) {
        PRINT_ERR("PMU set period failed, invalid param\n");
        return;
    }
    UINT32 remain = event->period - (UINT32)(event->count % event->period);
    WRITE_REG(OCD_DPCNT(event->counter), PERIOD_EVENT_CALC(remain));
}

STATIC INLINE BOOL PmuCntOverflowed(UINT32 dpstat, UINT32 counter)
{
    return ((dpstat & (0x1U << counter)) != 0);
}

STATIC INLINE BOOL PmuOverflowed(UINT32 dpstat)
{
    return ((dpstat & OCD_DPSTAT_OVFX_MASK) != 0);
}

STATIC UINT64 PmuReadEventCnt(Event *event)
{
    if (PmuCheckParam(event) != LOS_OK) {
        PRINT_ERR("PMU read event count failed, invalid param\n");
        return 0;
    }

    UINT32 value = READ_REG(OCD_DPCNT(event->counter));
    UINT32 dpstat = READ_REG(OCD_DPSTAT);

    if (value < PERIOD_EVENT_CALC(event->period)) {
        if (PmuCntOverflowed(dpstat, event->counter) == TRUE) {
            value += event->period;
        }
    } else {
        value -= PERIOD_EVENT_CALC(event->period);
    }

    return (UINT64)value;
}

STATIC INLINE UINT32 PmuMapRealEvent(UINT32 eventType, UINT32 *result, UINT32 len)
{
    (VOID)len;

    UINT32 event = PERF_HW_INVALID_EVENT_TYPE;
    for (UINT32 i = 0; i < ARRAY_SIZE(g_linglongMap); i++) {
        for (UINT32 j = 0; j < g_linglongMap[i]->len; j++) {
            if (g_linglongMap[i]->map[j] == eventType) {
                event = i;
            }
        }
    }

    result[0] = event;
    return 1;
}

STATIC INLINE UINT32 PmuMapCommEvent(UINT32 eventType, UINT32 *result, UINT32 len)
{
    UINT32 i;
    if (eventType < ARRAY_SIZE(g_linglongMap)) {
        for (i = 0; i < OS_MIN(len, g_linglongMap[eventType]->len); i++) {
            result[i] = g_linglongMap[eventType]->map[i];
        }
        return i;
    }
    result[0] = PERF_HW_INVALID_EVENT_TYPE;
    return 1;
}

STATIC UINT32 PmuMapEvent(UINT32 eventType, UINT32 *result, UINT32 len, BOOL reverse)
{
    if (!reverse) {  /* Common event to arm real event */
        return PmuMapCommEvent(eventType, result, len);
    } else {
        return PmuMapRealEvent(eventType, result, len);
    }
}

UINT32 OsGetPmuCounter0(VOID)
{
    return PMU_COUNTER0_ID;
}

UINT32 OsGetPmuMaxCounter(VOID)
{
    return PMU_MAX_COUNTER_NUM;
}

UINT32 OsGetPmuCycleCounter(VOID)
{
    return PMU_CYCLE_COUNTER_ID;
}

STATIC HwPmu g_linglongPmu = {
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

STATIC VOID PmuIrqHandler(VOID)
{
    UINT32 index;
    UINT32 dpstat;
    PerfRegs regs = {0};
    Event *event = NULL;
    PerfEvent *events = g_linglongPmu.pmu.current[ArchCurrCpuid()];
    UINT32 eventNum = events->nr;

    /* Get and reset the IRQ flags */
    dpstat = READ_REG(OCD_DPSTAT);
    if (!PmuOverflowed(dpstat)) {
        return;
    }

    OsPerfFetchIrqRegs(&regs);

    PmuStopAllCnt();

    for (index = 0; index < eventNum; index++) {
        event = &(events->per[index]);
        /*
         * We have a single interrupt for all counters. Check that
         * each counter has overflowed before we process it.
         */
        if (!PmuCntOverflowed(dpstat, event->counter) || (event->period == 0)) {
            continue;
        }

        OsPerfHwPmuHandleOverflow(event, &regs);
    }
    WRITE_REG(OCD_DPSTAT, dpstat);  /* clear all handled counters */
    PmuStartAllCnt();
}

UINT32 OsHwPmuInit(VOID)
{
    UINT32 ret;

    ret = LOS_HwiCreate(LLIC_OCD_OV_INT_ID, 0, 0, PmuIrqHandler, NULL);
    if (ret != LOS_OK) {
        return ret;
    }

    ret = OsPerfHwInit(&g_linglongPmu);
    return ret;
}

VOID OsHwPmuDeinit(VOID)
{
    UINT32 ret;
    OsPerfHwDeinit(&g_linglongPmu);
    ret = LOS_HwiDelete(LLIC_OCD_OV_INT_ID, NULL);
    if (ret != LOS_OK) {
        PRINT_ERR("Failed to delete pmu interrupt, code %u\n", ret);
    }
}
