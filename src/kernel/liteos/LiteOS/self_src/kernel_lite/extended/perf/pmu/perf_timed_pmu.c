/*----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description: LiteOS Perf Timed Pmu Implementation
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
 *---------------------------------------------------------------------------*/

#include "perf_pmu_pri.h"

#define US_PER_SECOND               1000000
#define HRTIMER_DEFAULT_PERIOD_US   1000

STATIC SwPmu g_perfTimed = {0};

STATIC BOOL OsPerfTimedPeriodValid(UINT32 period)
{
    return period >= TIMER_PERIOD_LOWER_BOUND_US;
}

STATIC UINT32 OsPerfTimedStart(PerfEvent *events, BOOL restart)
{
    UINT32 cpuid = ArchCurrCpuid();
    if (cpuid != 0 || !restart) { /* only need start on one core */
        return LOS_OK;
    }

    if (hrtimer_start(&g_perfTimed.hrtimer, g_perfTimed.time, HRTIMER_MODE_REL) != 0) {
        PRINT_ERR("Hrtimer start failed\n");
        return LOS_NOK;
    }

    if (hrtimer_forward(&g_perfTimed.hrtimer, g_perfTimed.cfgTime) == 0) {
        PRINT_ERR("Hrtimer forward failed\n");
        return LOS_NOK;
    }

    g_perfTimed.time = g_perfTimed.cfgTime;
    return LOS_OK;
}

STATIC UINT32 OsPerfTimedConfig(PerfEvent *events, const PerfEventConfig *eventsCfg)
{
    UINT32 period;
    UINT32 eventNum = OS_MIN(eventsCfg->eventsNr, PERF_MAX_EVENT);
    if ((eventNum != 1) || (eventsCfg->events[0].eventId != PERF_COUNT_CPU_CLOCK)) {
        return LOS_NOK;
    }

    /* use first PERF_COUNT_CPU_CLOCK event config */
    period = eventsCfg->events[0].period;
    if (!OsPerfTimedPeriodValid(period)) {
        period = TIMER_PERIOD_LOWER_BOUND_US;
        PRINT_ERR("config period invalid, should be >= 100, use default period:%u us\n", period);
    }

    g_perfTimed.cfgTime = (union ktime) {
        .tv.sec  = period / US_PER_SECOND,
        .tv.usec = period % US_PER_SECOND
    };

    PRINT_INFO("hrtimer config period - sec:%d, usec:%d\n",
        g_perfTimed.cfgTime.tv.sec, g_perfTimed.cfgTime.tv.usec);

    events->per[0].eventId = eventsCfg->events[0].eventId;
    events->per[0].period = period;
    events->nr = (UINT8)eventNum;

    return LOS_OK;
}

STATIC UINT32 OsPerfTimedStop(PerfEvent *events, BOOL update)
{
    if (ArchCurrCpuid() != 0 || !update) { /* only need stop on one core */
        return LOS_OK;
    }

    if (hrtimer_cancel(&g_perfTimed.hrtimer) != 1) {
        PRINT_ERR("Hrtimer stop failed!\n");
        return LOS_NOK;
    }
    return LOS_OK;
}

STATIC VOID OsPerfTimedHandle(VOID)
{
    UINT32 index;
    PerfRegs regs = {0};
    Event *event = NULL;

    PerfEvent *events = g_perfTimed.pmu.current[ArchCurrCpuid()];
    UINT32 eventNum = events->nr;

    OsPerfFetchIrqRegs(&regs);

    for (index = 0; index < eventNum; index++) {
        event = &(events->per[index]);
        OsPerfUpdateEventCount(event, 1); /* eventCount += 1 every once */
        OsPerfHandleOverFlow(event, &regs);
    }
}

STATIC enum hrtimer_restart OsPerfHrtimer(struct hrtimer *hrtimer)
{
    SMP_CALL_PERF_FUNC(OsPerfTimedHandle); /* send to all cpu to collect data */
    return HRTIMER_RESTART;
}

STATIC CHAR *OsPerfGetEventName(Event *event)
{
    if (event->eventId == PERF_COUNT_CPU_CLOCK) {
        return "timed";
    } else {
        return "unknown";
    }
}

UINT32 OsTimedPmuInit(VOID)
{
    g_perfTimed.time = (union ktime) {
        .tv.sec = 0,
        .tv.usec = HRTIMER_DEFAULT_PERIOD_US,
    };

    hrtimer_init(&g_perfTimed.hrtimer, 1, HRTIMER_MODE_REL);

    if (hrtimer_create(&g_perfTimed.hrtimer, g_perfTimed.time, OsPerfHrtimer) != LOS_OK) {
        return LOS_NOK;
    }

    g_perfTimed.pmu = (Pmu) {
        .type    = PERF_EVENT_TYPE_TIMED,
        .config  = OsPerfTimedConfig,
        .start   = OsPerfTimedStart,
        .stop    = OsPerfTimedStop,
        .getName = OsPerfGetEventName,
    };

    return OsPerfPmuRegister(&g_perfTimed.pmu);
}

VOID OsTimedPmuDeinit(VOID)
{
    OsPerfPmuRm(PERF_EVENT_TYPE_TIMED);
    (VOID)hrtimer_cancel(&g_perfTimed.hrtimer);
    (void)memset((void *)&g_perfTimed, 0, sizeof(SwPmu));
}
