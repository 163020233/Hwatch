/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Libc Time adapt.
 * Author: Huawei LiteOS Team
 * Create: 2025-03-01
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
#include "adapt_time.h"
#include "time_pri.h"
#include "limits.h"
#include "sys/times.h"
#include "errno.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/time.h"
#include "stdint.h"
#include "los_sys.h"
#include "los_tick_pri.h"
#include "los_tick.h"
#include "rtc_time_hook.h"
#include "tzdst_pri.h"
#include "asm/platform.h"
/*
 * store register rtc func
 */
STATIC struct RtcTimeHook g_rtcTimeFunc;

STATIC UINT64 g_rtcTimeBase = 0;
STATIC UINT64 g_systickBase = 0;

#define SECSPERMIN           60     /* second per minute */
#define TIME_ZONE_MAX        720    /* UTC-12:00, the last time zone */
#define TIME_ZONE_MIN        (-840) /* UTC+14:00, the first time zone */

STATIC UINT64 GetCurrentTime(VOID)
{
    UINT64 tickDelta = 0;
    UINT64 currentTick;

    if (g_rtcTimeFunc.RtcGetTickHook != NULL) {
        currentTick = g_rtcTimeFunc.RtcGetTickHook();
        if ((g_systickBase != 0) && (currentTick > g_systickBase)) {
            tickDelta = currentTick - g_systickBase;
        }
    }
    return g_rtcTimeBase + LOS_Tick2MS((UINT32)tickDelta);
}

int gettimeofday(struct timeval *tv, void *tz)
{
    if (tv != NULL) {
        if (g_rtcTimeFunc.RtcGetTimeHook != NULL) {
            INT32 rtcRet;
            UINT64 usec = 0;
            UINT64 currentTime;

            rtcRet = g_rtcTimeFunc.RtcGetTimeHook(&usec);
            if (rtcRet != 0) {
                currentTime = GetCurrentTime();
                tv->tv_sec = (time_t)(currentTime / OS_SYS_MS_PER_SECOND);
                tv->tv_usec = (long)((currentTime % OS_SYS_MS_PER_SECOND) * OS_SYS_MS_PER_SECOND);
            } else {
                tv->tv_sec = (time_t)(usec / OS_SYS_US_PER_SECOND);
                tv->tv_usec = (long)(usec % OS_SYS_US_PER_SECOND);
            }
        } else {
            if (__gettimeofday(tv, tz) < 0) {
                return -1;
            }
        }
    } else if (g_rtcTimeFunc.RtcGetTimezoneHook == NULL) {
        return -1;
    }

    if ((tz != NULL) && (g_rtcTimeFunc.RtcGetTimezoneHook != NULL)) {
        INT32 timeZone = 0;
        struct timezone *ptz = (struct timezone *)tz;
        g_rtcTimeFunc.RtcGetTimezoneHook(&timeZone);
        ptz->tz_minuteswest = timeZone / SECSPERMIN;
        ptz->tz_dsttime = 0;
    }
    return 0;
}

int settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    if ((tv == NULL) && (tz == NULL)) {
        errno = EFAULT;
        return -1;
    }

    if ((tv != NULL) && ((tv->tv_usec < 0) || (tv->tv_usec >= OS_SYS_US_PER_SECOND) || (tv->tv_sec < 0))) {
        errno = EINVAL;
        return -1;
    }

    if ((tz != NULL) && (g_rtcTimeFunc.RtcSetTimezoneHook != NULL)) {
        if ((tz->tz_minuteswest >= TIME_ZONE_MIN) && (tz->tz_minuteswest <= TIME_ZONE_MAX)) {
            timezone = tz->tz_minuteswest * SECSPERMIN;
        } else {
            errno = EINVAL;
            return -1;
        }
        (VOID)g_rtcTimeFunc.RtcSetTimezoneHook((INT32)timezone);
    }

    if (tv != NULL) {
        if (g_rtcTimeFunc.RtcSetTimeHook != NULL) {
            UINT64 usec;
            g_rtcTimeBase = (UINT64)tv->tv_sec * OS_SYS_MS_PER_SECOND + (UINT64)tv->tv_usec / OS_SYS_MS_PER_SECOND;
            usec = (UINT64)tv->tv_sec * OS_SYS_US_PER_SECOND + (UINT64)tv->tv_usec;
            if (g_rtcTimeFunc.RtcSetTimeHook(g_rtcTimeBase, &usec) < 0) {
                return -1;
            }
        } else if (__settimeofday(tv, tz) < 0) {
            return -1;
        }
    }

    if (g_rtcTimeFunc.RtcGetTickHook != NULL) {
        g_systickBase = g_rtcTimeFunc.RtcGetTickHook();
    }

    return 0;
}

clock_t clock(VOID)
{
    if (g_rtcTimeFunc.RtcGetTickHook != NULL) {
        return (clock_t)(g_rtcTimeFunc.RtcGetTickHook());
    }
    return __clock();
}

long OsGetTimeZone(VOID)
{
    long tempTimezone = 0;
    if (g_rtcTimeFunc.RtcGetTimezoneHook != NULL) {
        g_rtcTimeFunc.RtcGetTimezoneHook((INT32 *)&tempTimezone);
    } else {
        tempTimezone = timezone;
    }
    return tempTimezone;
}

time_t time(time_t *t)
{
    struct timespec ts;
    if (g_rtcTimeFunc.RtcGetTimeHook != NULL) {
        UINT64 usec = 0;
        time_t sec;
        INT32 rtcRet;

        rtcRet = g_rtcTimeFunc.RtcGetTimeHook(&usec);
        if (rtcRet != 0) {
            UINT64 currentTime;
            currentTime = GetCurrentTime();
            sec = (time_t)(currentTime / OS_SYS_MS_PER_SECOND);
        } else {
            sec = (time_t)(usec / OS_SYS_US_PER_SECOND);
        }
        if (t != NULL) {
            *t = sec;
        }
        return sec;
    }

    __clock_gettime(CLOCK_REALTIME, &ts);
    if (t != NULL) {
        *t = ts.tv_sec;
    }
    return ts.tv_sec;
}

VOID LOS_RtcHookRegister(struct RtcTimeHook *cfg)
{
    if (cfg == NULL) {
        PRINT_ERR("Invalid argument\n");
        return;
    }
    g_rtcTimeFunc.RtcGetTickHook = cfg->RtcGetTickHook;
    g_rtcTimeFunc.RtcGetTimeHook = cfg->RtcGetTimeHook;
    g_rtcTimeFunc.RtcSetTimeHook = cfg->RtcSetTimeHook;
    g_rtcTimeFunc.RtcGetTimezoneHook = cfg->RtcGetTimezoneHook;
    g_rtcTimeFunc.RtcSetTimezoneHook = cfg->RtcSetTimezoneHook;
}