/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS Perf Hardware Pmu Module Implementation
 * Author: Huawei LiteOS Team
 * Create: 2020-07-29
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

#include "los_perf_pri.h"
#include "perf_pmu_pri.h"
#include "arch/perf.h"

#define OS_PERF_CODE_MAX                    2       /* 1 to N event map, this option limits the max N */

STATIC Pmu *g_perfHw = NULL;

STATIC CHAR *g_eventName[PERF_COUNT_HW_MAX] = {
    [PERF_COUNT_HW_CPU_CYCLES]              = "cycles",
    [PERF_COUNT_HW_INSTRUCTIONS]            = "instructions",
    [PERF_COUNT_HW_ICACHE_REFERENCES]       = "icache",
    [PERF_COUNT_HW_ICACHE_MISSES]           = "icache-misses",
    [PERF_COUNT_HW_DCACHE_REFERENCES]       = "dcache",
    [PERF_COUNT_HW_DCACHE_MISSES]           = "dcache-misses",
    [PERF_COUNT_HW_BRANCH_INSTRUCTIONS]     = "branches",
    [PERF_COUNT_HW_BRANCH_MISSES]           = "branches-misses",
};

typedef struct {
    UINT32 maxCounter;              /* max counter id */
    UINT32 cycleCounter;            /* cycle counter id */
    UINT32 cycleCode;               /* cycle event code */
#ifdef LOSCFG_ARCH_RISCV32
    UINT32 instretCounter;          /* instruction counter code */
    UINT32 instretCode;             /* instruction event code */
#endif
    UINT32 currCounter;
} CounterInfo;

STATIC UINT32 OsPerfCounterInfoInit(CounterInfo *info)
{
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);

    info->currCounter = OsGetPmuCounter0();
    info->maxCounter = OsGetPmuMaxCounter();
    info->cycleCounter = OsGetPmuCycleCounter();

    (VOID)hwPmu->mapEvent(PERF_COUNT_HW_CPU_CYCLES, &info->cycleCode, 1, PERF_EVENT_TO_CODE);
    if (info->cycleCode == PERF_HW_INVALID_EVENT_TYPE) {
        return LOS_NOK;
    }

#ifdef LOSCFG_ARCH_RISCV32
    info->instretCounter = OsGetPmuInstretCounter();
    (VOID)hwPmu->mapEvent(PERF_COUNT_HW_INSTRUCTIONS, &info->instretCode, 1, PERF_EVENT_TO_CODE);
    if (info->instretCode == PERF_HW_INVALID_EVENT_TYPE) {
        return LOS_NOK;
    }
#endif

    return LOS_OK;
}

STATIC UINT32 OsPerfHwPreConfigEvents(PerfEvent *events, enum PerfEventType type,
                                      const PerfEventConfig *eventsCfg)
{
    UINT32 writeIdx = 0;
    UINT32 codesLen;
    UINT32 codes[OS_PERF_CODE_MAX];
    UINT32 eventNum = OS_MIN(eventsCfg->eventsNr, PERF_MAX_EVENT);
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);

    for (UINT32 i = 0; i < eventNum; i++) {
        if (type == PERF_EVENT_TYPE_HW) { /* do map */
            codesLen = hwPmu->mapEvent(eventsCfg->events[i].eventId, codes, OS_PERF_CODE_MAX,
                                       PERF_EVENT_TO_CODE);
        } else { /* RAW */
            codes[0] = eventsCfg->events[i].eventId;
            codesLen = 1;
        }

        for (UINT32 j = 0; j < codesLen; j++) {
            if (writeIdx >= PERF_MAX_EVENT) {
                PRINT_ERR("Actual event config number exceeded max event size %d\n", PERF_MAX_EVENT);
                return LOS_NOK;
            }
            if (codes[j] == PERF_HW_INVALID_EVENT_TYPE) {
                return LOS_NOK;
            }
            events->per[writeIdx].eventId = codes[j];
            events->per[writeIdx].period = eventsCfg->events[i].period;
            writeIdx++;
        }
    }
    events->nr = (UINT8)writeIdx;

    hwPmu->cntDivided = events->cntDivided & hwPmu->canDivided;

    return LOS_OK;
}

STATIC UINT32 OsPerfHwCounterGet(CounterInfo *counters, UINT32 *counterId, UINT32 eventId)
{
    if (eventId == counters->cycleCode) {
        *counterId = counters->cycleCounter;
        return LOS_OK;
    }
#ifdef LOSCFG_ARCH_RISCV32
    if (eventId == counters->instretCode) {
        *counterId = counters->instretCounter;
        return LOS_OK;
    }
#endif
    if (counters->currCounter < counters->maxCounter) {
        *counterId = counters->currCounter++;
        return LOS_OK;
    }

    PRINT_ERR("max events: %u excluding cycle event\n", (counters->maxCounter - 1));

    return LOS_NOK;
}

STATIC UINT32 OsPerfHwPeriodValid(const CounterInfo *counters, UINT32 eventId, UINT32 period)
{
    if (eventId == counters->cycleCode) {
        if (!VALID_CYCLE_PERIOD(period)) {
            PRINT_ERR("Config period: 0x%x invalid, should be in (%#x, %#x)\n", period,
                PERIOD_CYCLE_CALC(CYCLE_PERIOD_UPPER_BOUND), PERIOD_CYCLE_CALC(CYCLE_PERIOD_LOWER_BOUND + 1));
            return LOS_NOK;
        }
    } else {
        if (!VALID_EVENT_PERIOD(period)) {
            PRINT_ERR("Config period: 0x%x invalid, should be in (%#x, %#x)\n", period,
                PERIOD_EVENT_CALC(EVENT_PERIOD_UPPER_BOUND), PERIOD_EVENT_CALC(EVENT_PERIOD_LOWER_BOUND + 1));
            return LOS_NOK;
        }
    }

    return LOS_OK;
}

STATIC UINT32 OsPerHwConfigEvents(PerfEvent *events)
{
    UINT32 ret;
    CounterInfo counters;
    UINT32 counterId;
    Event *event = NULL;

    ret = OsPerfCounterInfoInit(&counters);
    if (ret != LOS_OK) {
        return ret;
    }

    for (UINT32 i = 0; i < events->nr; i++) {
        event = &events->per[i];
        ret = OsPerfHwPeriodValid(&counters, event->eventId, event->period);
        if (ret != LOS_OK) {
            return ret;
        }

        ret = OsPerfHwCounterGet(&counters, &counterId, event->eventId);
        if (ret != LOS_OK) {
            return ret;
        }

        event->counter = counterId;

        PRINT_DEBUG("Perf Config %u eventId = 0x%x, counter = 0x%x, period = 0x%x\n",
            i, event->eventId, event->counter, event->period);
    }

    return LOS_OK;
}

/**
 * 1.If config event is PERF_EVENT_TYPE_HW, then map it to the real eventId first, otherwise use the configured
 * eventId directly.
 * 2.Find available counter for each event.
 * 3.Decide whether this hardware pmu need prescaler (once every 64 cycle counts).
 */
STATIC UINT32 OsPerfHwConfig(PerfEvent *events, const PerfEventConfig *eventsCfg)
{
    UINT32 ret;

    ret = OsPerfHwPreConfigEvents(events, g_perfHw->type, eventsCfg);
    if (ret != LOS_OK) {
        return ret;
    }
    ret = OsPerHwConfigEvents(events);
    if (ret != LOS_OK) {
        return ret;
    }

    return LOS_OK;
}

STATIC UINT32 OsPerfHwStart(PerfEvent *events, BOOL restart)
{
    (VOID)restart;
    
    UINT32 i;
    Event *event = NULL;
    UINT32 eventNum = events->nr;
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);

    hwPmu->clear();

    for (i = 0; i < eventNum; i++) {
        event = &(events->per[i]);
        hwPmu->setPeriod(event);
        hwPmu->enable(event);
    }

    PRINT_DEBUG("starting pmu...\n");
    hwPmu->start();
    return LOS_OK;
}

STATIC VOID OsPerfHwUpdateCycleCounter(PerfEvent *events)
{
    UINT32 i;
    UINT32 eventId;
    UINT32 eventNum = events->nr;
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);
    if (hwPmu == NULL) {
        return;
    }

    for (i = 0; i < eventNum; i++) {
        Event *event = &(events->per[i]);
        if (g_perfHw->type == PERF_EVENT_TYPE_HW) {
            (VOID)hwPmu->mapEvent(event->eventId, &eventId, 1, PERF_CODE_TO_EVENT);
            if ((eventId == PERF_COUNT_HW_CPU_CYCLES) && (hwPmu->cntDivided == TRUE)) {
                event->count = event->count << 6; /* CCNT counts every 64th cpu cycle */
            }
        }
    }
}

STATIC UINT32 OsPerfHwStop(PerfEvent *events, BOOL update)
{
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);

    PRINT_DEBUG("stopping pmu...\n");
    hwPmu->stop();
    OsPerfHwReadAllCounter(events);
    if (update) {
        OsPerfHwUpdateCycleCounter(events);
    }

    return LOS_OK;
}

STATIC CHAR *OsPerfGetEventName(Event *event)
{
    UINT32 eventId;
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);
    (VOID)hwPmu->mapEvent(event->eventId, &eventId, 1, PERF_CODE_TO_EVENT);
    if (eventId < PERF_COUNT_HW_MAX) {
        return g_eventName[eventId];
    } else {
        return "unknown";
    }
}

STATIC UINT64 IncrementUpdate(Event *event, UINT64 newRegCount)
{
    UINT64 prevRegCount = event->prevCount;
    event->prevCount = newRegCount % event->period;
    return newRegCount - prevRegCount;
}

VOID OsPerfHwReadAllCounter(PerfEvent *events)
{
    UINT32 i;
    UINT64 increment;
    Event *event = NULL;
    HwPmu *hwPmu = NULL;

    if (events == NULL) {
        return;
    }
    if (g_perfHw == NULL) {
        return;
    }
    hwPmu = GET_HW_PMU(g_perfHw);
    if ((hwPmu == NULL) || (hwPmu->readCnt == NULL)) {
        return;
    }

    UINT32 eventNum = events->nr;
    for (i = 0; i < eventNum; i++) {
        event = &(events->per[i]);
        increment = IncrementUpdate(event, hwPmu->readCnt(event));
        PRINT_DEBUG("perf increment value = 0x%llx\n", increment);
        event->count += increment;
    }
}

UINT32 OsPerfHwInit(HwPmu *hwPmu)
{
    UINT32 ret;
    if (hwPmu == NULL) {
        return LOS_NOK;
    }

    hwPmu->pmu.type    = PERF_EVENT_TYPE_HW;
    hwPmu->pmu.config  = OsPerfHwConfig;
    hwPmu->pmu.start   = OsPerfHwStart;
    hwPmu->pmu.stop    = OsPerfHwStop;
    hwPmu->pmu.getName = OsPerfGetEventName;

    ret = OsPerfPmuRegister(&hwPmu->pmu);

    g_perfHw = OsPerfPmuGet(PERF_EVENT_TYPE_HW);
    return ret;
}

VOID OsPerfHwDeinit(HwPmu *hwPmu)
{
    g_perfHw = NULL;
    OsPerfPmuRm(PERF_EVENT_TYPE_HW);
    (void)memset((void *)&hwPmu->pmu, 0, sizeof(Pmu));
}

/* Handle pmu overflow and update event counts.
 * This function shall not used if hardware interrupt not supported
 * (eg. riscv) due to count calculation can goes wrong.
 */
VOID OsPerfHwPmuHandleOverflow(Event *event, PerfRegs *regs)
{
    HwPmu *hwPmu = GET_HW_PMU(g_perfHw);

    UINT32 increment = (UINT32)IncrementUpdate(event, event->period);
    OsPerfUpdateEventCount(event, increment);
    OsPerfHandleOverFlow(event, regs);
    hwPmu->setPeriod(event); /* reload event counter */
}
