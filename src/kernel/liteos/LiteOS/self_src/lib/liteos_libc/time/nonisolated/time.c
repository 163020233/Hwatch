/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2013-2020. All rights reserved.
 * Description: Time file
 * Author: Huawei LiteOS Team
 * Create: 2013-01-01
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

#if defined(__LITEOS__)
#include "time_pri.h"
#include "limits.h"
#include "sys/times.h"
#if !defined(LOSCFG_AARCH64) && !defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
#include "time64.h"
#endif
#include "errno.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/time.h"
#include "stdint.h"
#include "los_sys.h"
#include "los_tick_pri.h"
#include "los_tick.h"
#else
#include "limits.h"
#include "sys/time.h"
#include "time.h"
#include "../../include/posix_adapt.h"
#include "drb_atomic.h"
#endif

const char __utc[] = "UTC";

#define SECSPERMIN           60     /* second per minute */
#define TIME_ZONE_MAX        720    /* UTC-12:00, the first time zone */
#define TIME_ZONE_MIN        (-840) /* UTC+14:00, the last time zone */

/*
 * Do a time package defined return. This requires the error code
 * to be placed in errno, and if it is non-zero, -1 returned as the
 * result of the function. This also gives us a place to put any
 * generic tidyup handling needed for things like signal delivery and
 * cancellation.
 */
#define TIME_RETURN(err) do { \
    INT32 retVal = 0;         \
    if ((err) != 0) {         \
        retVal = -1;          \
        errno = (err);        \
    }                         \
    return retVal;            \
} while (0)

#if defined(__LITEOS__)
#define OS_SEC_L4_DATA
#define OS_SEC_L4_TEXT
#define DEFAULT_VISIBILITY
#if defined (LOSCFG_AARCH64) || defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
/*
 * This two structures originally didn't exit,
 * liteos adds to support 64bit interfaces on 32bit platform,
 * in 64bit platform, timeval64 define to timeval which is platform adaptive.
 */
#define timeval64 timeval
#define timespec64 timespec
#endif

#define TV_SEC_MAX ((INT_MAX / 1000000L) - 2)
#define TV_SEC_MIN ((INT_MIN / 1000000L) + 2)

#define TIME_TICK_PER_SECOND     LOSCFG_BASE_CORE_TICK_PER_SECOND

#define TIME_LOCK_INIT(lock)      SPIN_LOCK_INIT(lock)
#define TIME_LOCK(state)          LOS_SpinLockSave(&g_timeLock, &(state))
#define TIME_UNLOCK(state)        LOS_SpinUnlockRestore(&g_timeLock, (state))

#define TIME_GET_CURR_NANO_SEC()  LOS_CurrNanosec()
#define TIME_TASK_DELAY(ticks)    LOS_TaskDelay(ticks)
#else /* __LITEOS__ */
#define TIME_TICK_PER_SECOND     g_tickModInfo.tickPerSecond

OS_SEC_L4_DATA struct SreSpinLock g_timeLock = {0};
#define TIME_LOCK(state)          (state) = DRB_SplIrqLock(&g_timeLock)
#define TIME_UNLOCK(state)        DRB_SplIrqUnlock(&g_timeLock, (state))
OS_SEC_L4_DATA struct SreSpinLock g_gmTimeLock = {0};
#define GMTIME_LOCK(state)        (state) = DRB_SplIrqLock(&g_gmTimeLock)
#define GMTIME_UNLOCK(state)      DRB_SplIrqUnlock(&g_gmTimeLock, (state))

OS_SEC_L4_TEXT unsigned long long OsGetCurrNanoSec(void)
{
    unsigned long long nanos;
    unsigned long long cycle = OsCurCycleGet64();
    nanos = (cycle / g_systemClock) * (unsigned long long)OS_SYS_NS_PER_SECOND;
    nanos += cycle % g_systemClock * (unsigned long long)OS_SYS_NS_PER_SECOND / g_systemClock;

    return nanos;
}

#define TIME_GET_CURR_NANO_SEC()  OsGetCurrNanoSec()
#define TIME_TASK_DELAY(ticks)    DRB_TaskDelay(ticks)

INLINE bool ValidTimespec(const struct timespec *tp)
{
    /* Fail a NULL pointer */
    if (tp == NULL) {
        return FALSE;
    }

    /* Fail illegal nanosecond values */
    if ((tp->tv_nsec < 0) || (tp->tv_nsec >= (long)OS_SYS_NS_PER_SECOND) || \
        (tp->tv_sec < 0)) {
        return FALSE;
    }

    return TRUE;
}
#endif

#if defined(__LITEOS__)
STATIC INLINE BOOL ValidTimeval(const struct timeval *tv)
{
    /* Fail a NULL pointer */
    if (tv == NULL) {
        return FALSE;
    }

    /* Fail illegal microseconds values */
    if ((tv->tv_usec < 0) || (tv->tv_usec >= OS_SYS_US_PER_SECOND) || (tv->tv_sec < 0)) {
        return FALSE;
    }

    return TRUE;
}

#if !defined(LOSCFG_AARCH64) && !defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
STATIC INLINE BOOL ValidTimeval64(const struct timeval64 *tv)
{
    /* Fail a NULL pointer */
    if (tv == NULL) {
        return FALSE;
    }

    /* Fail illegal microseconds values */
    if ((tv->tv_usec < 0) || (tv->tv_usec >= OS_SYS_US_PER_SECOND) || (tv->tv_sec < 0)) {
        return FALSE;
    }

    return TRUE;
}
#endif

STATIC TIME_LOCK_INIT(g_timeLock);
STATIC long long g_adjTimeLeft; /* absolute value of adjtime */
STATIC INT32 g_adjDirection;    /* 1, speed up; 0, slow down; */

/* Adjust pacement, nanoseconds per SCHED_CLOCK_INTETRVAL_TICKS ticks */
#define TIME_ADJ_PACEMENT (((LOSCFG_BASE_CORE_ADJ_PER_SECOND * SCHED_CLOCK_INTETRVAL_TICKS) / LOSCFG_BASE_CORE_TICK_PER_SECOND) * OS_SYS_NS_PER_US)
#endif

#define TIME_REMAINDER(number, unit) (((UINT64)(number) * TIME_TICK_PER_SECOND) % (unit))
/* accumulative time delta from continuous modify, such as adjtime */
STATIC OS_SEC_L4_DATA struct timespec64 g_accDeltaFromAdj;
/* accumulative time delta from discontinuous modify, such as settimeofday */
STATIC OS_SEC_L4_DATA struct timespec64 g_accDeltaFromSet;

#if defined(__LITEOS__)
VOID OsAdjTime(VOID)
{
    UINT32 intSave;
    TIME_LOCK(intSave);
    if (!g_adjTimeLeft) {
        TIME_UNLOCK(intSave);
        return;
    }

    if (g_adjTimeLeft > TIME_ADJ_PACEMENT) {
        if (g_adjDirection) {
            if ((g_accDeltaFromAdj.tv_nsec + TIME_ADJ_PACEMENT) >= OS_SYS_NS_PER_SECOND) {
                g_accDeltaFromAdj.tv_sec++;
                g_accDeltaFromAdj.tv_nsec  = (g_accDeltaFromAdj.tv_nsec + TIME_ADJ_PACEMENT) % OS_SYS_NS_PER_SECOND;
            } else {
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec + TIME_ADJ_PACEMENT;
            }
        } else {
            if ((g_accDeltaFromAdj.tv_nsec - TIME_ADJ_PACEMENT) < 0) {
                g_accDeltaFromAdj.tv_sec--;
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec - TIME_ADJ_PACEMENT + OS_SYS_NS_PER_SECOND;
            } else {
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec - TIME_ADJ_PACEMENT;
            }
        }

        g_adjTimeLeft -= TIME_ADJ_PACEMENT;
    } else {
        if (g_adjDirection) {
            if ((g_accDeltaFromAdj.tv_nsec + g_adjTimeLeft) >= OS_SYS_NS_PER_SECOND) {
                g_accDeltaFromAdj.tv_sec++;
                g_accDeltaFromAdj.tv_nsec  = (g_accDeltaFromAdj.tv_nsec + g_adjTimeLeft) % OS_SYS_NS_PER_SECOND;
            } else {
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec + g_adjTimeLeft;
            }
        } else {
            if ((g_accDeltaFromAdj.tv_nsec - g_adjTimeLeft) < 0) {
                g_accDeltaFromAdj.tv_sec--;
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec - g_adjTimeLeft + OS_SYS_NS_PER_SECOND;
            } else {
                g_accDeltaFromAdj.tv_nsec  = g_accDeltaFromAdj.tv_nsec - g_adjTimeLeft;
            }
        }

        g_adjTimeLeft = 0;
    }
    TIME_UNLOCK(intSave);
}

/*
 * Function: adjtime
 * Description:  correct the time to synchronize the system clock.
 * Input:     delta - The amount of time by which the clock is to be adjusted.
 * Output: oldDelta - the amount of time remaining from any previous adjustment that has not yet been completed.
 * Return: On success, returns 0.  On failure, -1 is returned, and errno is set to indicate the error.
 */
int adjtime(const struct timeval *delta, struct timeval *oldDelta)
{
    UINT32 intSave;
    TIME_LOCK(intSave);
    /* return the amount of time remaining from any previous adjustment that has not yet been completed. */
    if (oldDelta != NULL) {
        if (g_adjDirection == 1) {
            oldDelta->tv_sec = g_adjTimeLeft / OS_SYS_NS_PER_SECOND;
            oldDelta->tv_usec = (g_adjTimeLeft % OS_SYS_NS_PER_SECOND) / OS_SYS_NS_PER_US;
        } else {
            oldDelta->tv_sec = -(g_adjTimeLeft / OS_SYS_NS_PER_SECOND);
            oldDelta->tv_usec = -((g_adjTimeLeft % OS_SYS_NS_PER_SECOND) / OS_SYS_NS_PER_US);
        }
    }

    if ((delta == NULL) || ((delta->tv_sec == 0) && (delta->tv_usec == 0))) {
        TIME_UNLOCK(intSave);
        return 0;
    }

    if ((delta->tv_usec > OS_SYS_US_PER_SECOND) || (delta->tv_usec < -OS_SYS_US_PER_SECOND)) {
        TIME_UNLOCK(intSave);
        TIME_RETURN(EINVAL);
    }

    if ((delta->tv_sec < TV_SEC_MIN) ||
        (delta->tv_sec > TV_SEC_MAX)) {
        TIME_UNLOCK(intSave);
        TIME_RETURN(EINVAL);
    }

    g_adjTimeLeft = ((INT64)delta->tv_sec * OS_SYS_NS_PER_SECOND) + (delta->tv_usec * OS_SYS_NS_PER_US);
    if (g_adjTimeLeft > 0) {
        g_adjDirection = 1;
    } else {
        g_adjDirection = 0;
        g_adjTimeLeft = -g_adjTimeLeft;
    }

    TIME_UNLOCK(intSave);
    return 0;
}
#else
OS_SEC_L4_TEXT U32 OsPosixTimeInit(void)
{
    U32 ret;

    (void)memset_s((void *)&g_accDeltaFromAdj, sizeof(struct timespec64), 0, sizeof(struct timespec64));
    (void)memset_s((void *)&g_accDeltaFromSet, sizeof(struct timespec64), 0, sizeof(struct timespec64));

    ret = DRB_SplLockInit(&g_gmTimeLock);
    if (ret != ENOERR) {
        return ret;
    }

    return DRB_SplLockInit(&g_timeLock);
}

OS_SEC_L4_TEXT void OsTimeGetHwTime(struct timespec *hwTime)
{
    U64 cycle = OsCurCycleGet64();
    U64 nowNsec = (cycle / g_systemClock) * OS_SYS_NS_PER_SECOND +
        (cycle % g_systemClock) * OS_SYS_NS_PER_SECOND / g_systemClock;

    hwTime->tv_sec = (S64)(nowNsec / OS_SYS_NS_PER_SECOND);
    hwTime->tv_nsec = (S64)(nowNsec % OS_SYS_NS_PER_SECOND);
}

OS_SEC_L4_TEXT void OsTimeGetRealTime(struct timespec *realTime)
{
    uintptr_t intSave;
    struct timespec hwTime = {0};

    OsTimeGetHwTime(&hwTime);

    intSave = OsIntLock();
    realTime->tv_nsec = hwTime.tv_nsec + g_accDeltaFromSet.tv_nsec + g_accDeltaFromAdj.tv_nsec;
    realTime->tv_sec = hwTime.tv_sec + g_accDeltaFromSet.tv_sec + g_accDeltaFromAdj.tv_sec +
        (time_t)(realTime->tv_nsec >= OS_SYS_NS_PER_SECOND);
    realTime->tv_nsec %= OS_SYS_NS_PER_SECOND;
    OsIntRestore(intSave);
}

OS_SEC_L4_TEXT int OsTimeOut2Ticks(const struct timespec *time, U32 *ticks)
{
    struct timespec curTime;
    S64 timeOutNs;
    U64 timeOut;

    if ((time->tv_nsec < 0) || (time->tv_nsec >= OS_SYS_NS_PER_SECOND) ||
        (time->tv_sec < 0) || (time->tv_sec > OS_MAX_S32)) {
        return EINVAL;
    }

    OsTimeGetRealTime(&curTime);

    timeOutNs = (S64)(time->tv_sec - curTime.tv_sec) * OS_SYS_NS_PER_SECOND + (time->tv_nsec - curTime.tv_nsec);
    if (timeOutNs <= 0) {
        return ETIMEDOUT;
    }

    timeOut = (U64)(time->tv_sec - curTime.tv_sec) * OsSysGetTickPerSecond();
    if (time->tv_nsec > curTime.tv_nsec) {
        timeOut += ((U64)(time->tv_nsec - curTime.tv_nsec) * OsSysGetTickPerSecond() - 1) / OS_SYS_NS_PER_SECOND + 1;
    } else {
        timeOut -= ((U64)(curTime.tv_nsec - time->tv_nsec) * OsSysGetTickPerSecond()) / OS_SYS_NS_PER_SECOND;
    }
    // 因为tick误差在ms级，为了确保延时时间满足要求，向上加一
    timeOut = timeOut + 1;
    *ticks = timeOut >= OS_WAIT_FOREVER ? OS_WAIT_FOREVER : (U32)timeOut;

    return ENOERR;
}
#endif

#if defined(__LITEOS__)
STATIC INLINE struct timespec64 OsTimeSpecAdd(const struct timespec64 t1, const struct timespec64 t2)
#else
INLINE struct timespec64 OsTimeSpecAdd(const struct timespec64 t1, const struct timespec64 t2)
#endif
{
    struct timespec64 ret = {0};

    ret.tv_sec = t1.tv_sec + t2.tv_sec;
    ret.tv_nsec = t1.tv_nsec + t2.tv_nsec;
    if (ret.tv_nsec >= OS_SYS_NS_PER_SECOND) {
        ret.tv_sec += 1;
        ret.tv_nsec -= OS_SYS_NS_PER_SECOND;
    } else if (ret.tv_nsec < 0L) {
        ret.tv_sec -= 1;
        ret.tv_nsec += OS_SYS_NS_PER_SECOND;
    }

    return ret;
}

#if defined(__LITEOS__)
STATIC INLINE struct timespec64 OsTimeSpecSub(const struct timespec64 t1, const struct timespec64 t2)
{
    struct timespec64 ret = {0};

    ret.tv_sec = t1.tv_sec - t2.tv_sec;
    ret.tv_nsec = t1.tv_nsec - t2.tv_nsec;
    if (ret.tv_nsec < 0) {
        ret.tv_sec -= 1;
        ret.tv_nsec += OS_SYS_NS_PER_SECOND;
    }

    return ret;
}

STATIC INT32 OsSetTimeOfDay(const struct timeval64 *tv)
{
    UINT64 nowNsec;
    UINT32 intSave;
    struct timespec64 setTime = {0};
    struct timespec64 hwTime = {0};
    struct timespec64 realTime = {0};
    struct timespec64 tmp = {0};

    TIME_LOCK(intSave);
    nowNsec = LOS_CurrNanosec();
    setTime.tv_sec = tv->tv_sec;
    setTime.tv_nsec = tv->tv_usec * OS_SYS_NS_PER_US;
    hwTime.tv_sec = nowNsec / OS_SYS_NS_PER_SECOND;
    hwTime.tv_nsec = (INT64)(nowNsec -  hwTime.tv_sec * OS_SYS_NS_PER_SECOND);

    /* stop on-going continuous adjusement */
    if (g_adjTimeLeft) {
        g_adjTimeLeft = 0;
    }
    realTime = OsTimeSpecAdd(hwTime, g_accDeltaFromAdj);
    realTime = OsTimeSpecAdd(realTime, g_accDeltaFromSet);

    tmp = OsTimeSpecSub(setTime, realTime);
    g_accDeltaFromSet = OsTimeSpecAdd(g_accDeltaFromSet, tmp);

    TIME_UNLOCK(intSave);

    return 0;
}

int __settimeofday(const struct timeval *tv, const struct timezone *tz)
{
    struct timeval64 stTimeVal64 = {0};

    if (!ValidTimeval(tv)) {
        TIME_RETURN(EINVAL);
    }

    if (tz != NULL) {
        if ((tz->tz_minuteswest >= TIME_ZONE_MIN) &&
            (tz->tz_minuteswest <= TIME_ZONE_MAX)) {
            timezone = tz->tz_minuteswest * SECSPERMIN;
        } else {
            errno = EINVAL;
            return -1;
        }
    }

    stTimeVal64.tv_sec = tv->tv_sec;
    stTimeVal64.tv_usec = tv->tv_usec;

    return OsSetTimeOfDay(&stTimeVal64);
}

weak_alias(__settimeofday, settimeofday);

#if !defined(LOSCFG_AARCH64) && !defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
int settimeofday64(const struct timeval64 *tv, const struct timezone *tz)
{
    (VOID)tz;
    if (!ValidTimeval64(tv)) {
        TIME_RETURN(EINVAL);
    }

    return OsSetTimeOfDay(tv);
}
#endif

int setlocalseconds(int seconds)
{
    struct timeval tv = {0};

    tv.tv_sec = seconds;
    tv.tv_usec = 0;

    return settimeofday(&tv, NULL);
}
#endif

STATIC OS_SEC_L4_TEXT INT32 OsGettimeOfDay(struct timeval64 *tv, const struct timezone *tz)
{
    UINT64 nowNsec;
#if defined(__LITEOS__)
    UINT32 intSave;
#else
    uintptr_t intSave;
#endif

    (VOID)tz;
    struct timespec64 hwTime = {0};
    struct timespec64 realTime = {0};

    TIME_LOCK(intSave);
    nowNsec = TIME_GET_CURR_NANO_SEC();
    hwTime.tv_sec = nowNsec / OS_SYS_NS_PER_SECOND;
    hwTime.tv_nsec = (INT64)(nowNsec - (hwTime.tv_sec * OS_SYS_NS_PER_SECOND));
    realTime = OsTimeSpecAdd(hwTime, g_accDeltaFromAdj);
    realTime = OsTimeSpecAdd(realTime, g_accDeltaFromSet);
    TIME_UNLOCK(intSave);

    tv->tv_sec = realTime.tv_sec;
    tv->tv_usec = realTime.tv_nsec / OS_SYS_NS_PER_US;

    if (tv->tv_sec < 0) {
        TIME_RETURN(EINVAL);
    }
    return 0;
}

#if defined(__LITEOS__)
#if !defined(LOSCFG_AARCH64) && !defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
int gettimeofday64(struct timeval64 *tv, struct timezone *tz)
{
    if (tv == NULL) {
        TIME_RETURN(EINVAL);
    }

    return OsGettimeOfDay(tv, tz);
}
#endif

int __gettimeofday(struct timeval *tv, void *tz)
{
    struct timeval64 stTimeVal64 = {0};

    if (tv == NULL) {
        TIME_RETURN(EINVAL);
    }

    if (OsGettimeOfDay(&stTimeVal64, tz) == -1) {
        return -1;
    }

#if defined(LOSCFG_AARCH64) || defined(LOSCFG_LIB_LIBC_COMPAT_WIDTH64)
    tv->tv_sec = stTimeVal64.tv_sec;
    tv->tv_usec = stTimeVal64.tv_usec;
#else
    if (stTimeVal64.tv_sec > (long long)LONG_MAX) {
        return -1;
    }
    tv->tv_sec = (time_t)stTimeVal64.tv_sec;
    tv->tv_usec = (suseconds_t)stTimeVal64.tv_usec;
#endif

    return 0;
}
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT int __gettimeofday(struct timeval *restrict tv, void *restrict tz)
{
    struct timeval64 stTimeVal64 = {0};

    if (tv == NULL || tz == NULL) {
        errno = EINVAL;
        return 0;
    }

    if (OsGettimeOfDay(&stTimeVal64, tz) == -1) {
        return 0;
    }

    if (stTimeVal64.tv_sec > (long long)LONG_MAX) {
        errno = EINVAL;
        return 0;
    }
    tv->tv_sec = (time_t)stTimeVal64.tv_sec;
    tv->tv_usec = (suseconds_t)stTimeVal64.tv_usec;

    return 0;
}
#endif

weak_alias(__gettimeofday, gettimeofday);

#if defined(__LITEOS__)
int clock_settime(clockid_t clockId, const struct timespec *tp)
{
    struct timeval tv = {0};

    if (clockId != CLOCK_REALTIME) {
        TIME_RETURN(EINVAL);
    }

    if (!ValidTimespec(tp)) {
        TIME_RETURN(EINVAL);
    }

    tv.tv_sec = tp->tv_sec;
    tv.tv_usec = tp->tv_nsec / OS_SYS_NS_PER_US;
    return settimeofday(&tv, NULL);
}
#endif

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int clock_gettime(clockid_t clockId, struct timespec *tp)
{
#if defined(__LITEOS__)
    UINT32 intSave;
#else
    uintptr_t intSave;
#endif
    UINT64 nowNsec;
    struct timespec64 tmp = {0};
    struct timespec64 hwTime = {0};

    if ((clockId > MAX_CLOCKS) || (clockId < CLOCK_REALTIME)) {
        goto ERROUT;
    }

    if (tp == NULL) {
        goto ERROUT;
    }

    TIME_LOCK(intSave);
    nowNsec = TIME_GET_CURR_NANO_SEC();
    hwTime.tv_sec = (INT64)(nowNsec / OS_SYS_NS_PER_SECOND);
    hwTime.tv_nsec = (INT64)(nowNsec - hwTime.tv_sec * OS_SYS_NS_PER_SECOND);

    switch (clockId) {
        case CLOCK_MONOTONIC_RAW:
            tp->tv_sec = (time_t)hwTime.tv_sec;
            tp->tv_nsec = (long)hwTime.tv_nsec;
            break;
        case CLOCK_MONOTONIC:
            tmp = OsTimeSpecAdd(hwTime, g_accDeltaFromAdj);
            tp->tv_sec = (time_t)tmp.tv_sec;
            tp->tv_nsec = (long)tmp.tv_nsec;
            break;
        case CLOCK_REALTIME:
            tmp = OsTimeSpecAdd(hwTime, g_accDeltaFromAdj);
            tmp = OsTimeSpecAdd(tmp, g_accDeltaFromSet);
            tp->tv_sec = (time_t)tmp.tv_sec;
            tp->tv_nsec = (long)tmp.tv_nsec;
            break;
        default:
            TIME_UNLOCK(intSave);
            goto ERROUT;
    }

    TIME_UNLOCK(intSave);
    return 0;

ERROUT:
    TIME_RETURN(EINVAL);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int clock_getres(clockid_t clockId, struct timespec *tp)
{
    if (clockId != CLOCK_REALTIME) {
        TIME_RETURN(EINVAL);
    }

    if (tp == NULL) {
        TIME_RETURN(EINVAL);
    }

    /* the accessible rtc resolution */
    tp->tv_nsec = OS_SYS_NS_PER_US; /* the precision of clock_gettime is 1us */
    tp->tv_sec = 0;

    TIME_RETURN(0);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    UINT64 nanosec;
    UINT64 ticks = 0;

    (VOID)rmtp;

    if (!ValidTimespec(rqtp)) {
        errno = EINVAL;
        return -1;
    }

    nanosec = (UINT64)rqtp->tv_sec * OS_SYS_NS_PER_SECOND + rqtp->tv_nsec;
    if (nanosec != 0) {
        ticks = ((UINT64)nanosec * TIME_TICK_PER_SECOND) / OS_SYS_NS_PER_SECOND;
        if (ticks > UINT32_MAX) {
            errno = EINVAL;
            return -1;
        }
        if (TIME_REMAINDER(nanosec, OS_SYS_NS_PER_SECOND) && (ticks < UINT32_MAX)) {
            ticks += 1;
        }
    }

    if (TIME_TASK_DELAY((UINT32)ticks) == ENOERR) {
        return 0;
    }
    errno = EPERM;
    return -1;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT int usleep(unsigned useconds)
{
    struct timespec specTime = { 0 };
    UINT64 nanoseconds = (UINT64)useconds * OS_SYS_NS_PER_US;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    return nanosleep(&specTime, NULL);
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT unsigned int sleep(unsigned int seconds)
{
    struct timespec specTime = { 0 };
    int ret;
    UINT64 nanoseconds = (UINT64)seconds * OS_SYS_NS_PER_SECOND;

    specTime.tv_sec = (time_t)(nanoseconds / OS_SYS_NS_PER_SECOND);
    specTime.tv_nsec = (long)(nanoseconds % OS_SYS_NS_PER_SECOND);
    ret = nanosleep(&specTime, NULL);
    if (ret == ENOERR) {
        return 0;
    }
    return seconds;
}

#if defined(__LITEOS__)
clock_t __clock(VOID)
{
    UINT64 nowNsec;
    nowNsec = LOS_CurrNanosec();
    return (clock_t)(nowNsec / (OS_SYS_NS_PER_SECOND / CLOCKS_PER_SEC));
}
weak_alias(__clock, clock);

clock_t times(struct tms *buf)
{
    clock_t clockTick;

    clockTick = (clock_t)LOS_TickCountGet();
    if (buf != NULL) {
        buf->tms_cstime = clockTick;
        buf->tms_cutime = clockTick;
        buf->tms_stime  = clockTick;
        buf->tms_utime  = clockTick;
    }

    return clockTick;
}

int clock_nanosleep(clockid_t clk, int flags,
                    const struct timespec *req, struct timespec *rem)
{
    switch (clk) {
        case CLOCK_REALTIME:
            if (flags == 0) {
                /* we only support the realtime clock currently */
                return nanosleep(req, rem);
            }
            /* fallthrough */
        case CLOCK_MONOTONIC_COARSE:
        case CLOCK_REALTIME_COARSE:
        case CLOCK_MONOTONIC_RAW:
        case CLOCK_MONOTONIC:
        case CLOCK_PROCESS_CPUTIME_ID:
        case CLOCK_BOOTTIME:
        /* The following macros are defined in the time.h */
#ifdef CLOCK_REALTIME_ALARM
        case CLOCK_REALTIME_ALARM:
#endif
#ifdef CLOCK_BOOTTIME_ALARM
        case CLOCK_BOOTTIME_ALARM:
#endif
#ifdef CLOCK_SGI_CYCLE
        case CLOCK_SGI_CYCLE:
#endif
#ifdef CLOCK_TAI
        case CLOCK_TAI:
#endif
            if (flags == 0 || flags == TIMER_ABSTIME) {
                return ENOTSUP;
            }
            /* fallthrough */
        case CLOCK_THREAD_CPUTIME_ID:
        default:
            return EINVAL;
    }
}
#else
DEFAULT_VISIBILITY OS_SEC_L4_TEXT time_t time(time_t* t)
{
    struct timespec ts;

    clock_gettime(CLOCK_REALTIME, &ts);
    if (t != NULL) {
        *t = ts.tv_sec;
    }
    return ts.tv_sec;
}

/* 2000-03-01 00:00:00 (为简化计算，以这个时间为base。2000年正好为400整闰年，这天为周三) */
#define LEAP_BASE (946684800LL + 86400 * (31 + 29))
#define WEEK_BASE (3LL)
#define MON_BASE  (2LL)

#define YEARS_PER_CENTURY (100LL)
#define YEARS_PER_4C  (YEARS_PER_CENTURY * 4)

#define LEAPS_PER_CENTURY (25LL)
#define CENTURYS_PER_LEAP (4LL)
#define YEARS_PER_LEAP    (4LL)

#define MONS_PER_YEAR (12LL)

#define DAYS_PER_YEAR (365LL)
#define DAYS_PER_WEEK (7LL)
#define DAYS_PER_4Y   (DAYS_PER_YEAR * 4   + 1)
#define DAYS_PER_100Y (DAYS_PER_YEAR * 100 + 24)
#define DAYS_PER_400Y (DAYS_PER_YEAR * 400 + 97)

#define SECS_PER_YEARS (31622400LL) // 366天
#define SECS_PER_DAY   (86400LL)
#define SECS_PER_HOUR  (3600LL)
#define SECS_PER_MIN   (60LL)
#define MINS_PER_HOUR  (60LL)

#define DAYS_31        (31)
#define DAYS_30        (30)
#define DAYS_29        (29)
#define DAYS_28        (28)

INLINE int OsGetDayInWeek(long long days)
{
    int dayInWeek = (WEEK_BASE + days) % DAYS_PER_WEEK;
    if (dayInWeek < 0) {
        dayInWeek += DAYS_PER_WEEK;
    }

    return dayInWeek;
}

INLINE int OsGetCCycles(int remDays)
{
    int cCycles = remDays / DAYS_PER_100Y;
    if (cCycles == CENTURYS_PER_LEAP) {
        cCycles--;
    }

    return cCycles;
}

INLINE int OsGetQCycles(int remDays)
{
    int qCycles = remDays / DAYS_PER_4Y;
    if (qCycles == LEAPS_PER_CENTURY) {
        qCycles--;
    }

    return qCycles;
}

INLINE int OsGetRemYears(int remDays)
{
    int remYears = remDays / DAYS_PER_YEAR;
    if (remYears == YEARS_PER_LEAP) {
        remYears--;
    }

    return remYears;
}

INLINE int OsGetDayInYear(int remDays, int leaps)
{
    int dayInYear = remDays + DAYS_31 + DAYS_28 + leaps; // 加上1、2月
    if (dayInYear >= DAYS_PER_YEAR + leaps) {
        dayInYear -= DAYS_PER_YEAR + leaps;
    }

    return dayInYear;
}

static OS_SEC_L4_TEXT long long OsGetYears(long long days, int *remDaysOut, int *dayInYear)
{
    int leaps;
    int remYears, remDays;
    int qcCycles, cCycles, qCycles;

    qcCycles = days / DAYS_PER_400Y;
    remDays = days % DAYS_PER_400Y;
    if (remDays < 0) {
        remDays += DAYS_PER_400Y;
        qcCycles--;
    }

    cCycles = OsGetCCycles(remDays);
    remDays -= cCycles * DAYS_PER_100Y;

    qCycles = OsGetQCycles(remDays);
    remDays -= qCycles * DAYS_PER_4Y;

    remYears = OsGetRemYears(remDays);
    remDays -= remYears * DAYS_PER_YEAR;

    leaps = (remYears == 0) && ((qCycles != 0) || (cCycles == 0));
    *dayInYear = OsGetDayInYear(remDays, leaps);
    *remDaysOut = remDays;

    return remYears + YEARS_PER_LEAP * qCycles + YEARS_PER_CENTURY * cCycles + YEARS_PER_4C * qcCycles;
}

OS_SEC_L4_TEXT int OsTransSecsToTm(long long s, struct tm *tm)
{
    long long days, secs, years;
    int remDays, remSecs;
    int months;
    int dayInWeek, dayInYear;
    static const int DAYS_IN_MONTH[] = { // 从3月开始计算
        DAYS_31, DAYS_30, DAYS_31, DAYS_30,
        DAYS_31, DAYS_31, DAYS_30, DAYS_31,
        DAYS_30, DAYS_31, DAYS_31, DAYS_29
    };

    if (s < INT_MIN * SECS_PER_YEARS || s > INT_MAX * SECS_PER_YEARS) {
        return -1;
    }

    secs = s - LEAP_BASE;
    days = secs / SECS_PER_DAY;
    remSecs = secs % SECS_PER_DAY;
    if (remSecs < 0) {
        remSecs += SECS_PER_DAY;
        days--;
    }

    dayInWeek = OsGetDayInWeek(days);

    years = OsGetYears(days, &remDays, &dayInYear);

    for (months = 0; DAYS_IN_MONTH[months] <= remDays; months++) {
        remDays -= DAYS_IN_MONTH[months];
    }

    if (months >= (MONS_PER_YEAR - MON_BASE)) {
        months -= MONS_PER_YEAR;
        years++;
    }

    if (years + YEARS_PER_CENTURY > INT_MAX || years + YEARS_PER_CENTURY < INT_MIN) {
        return -1;
    }

    tm->tm_year = (int)years + YEARS_PER_CENTURY;
    tm->tm_mon = months + MON_BASE;
    tm->tm_mday = remDays + 1;
    tm->tm_wday = dayInWeek;
    tm->tm_yday = dayInYear;

    tm->tm_hour = remSecs / SECS_PER_HOUR;
    tm->tm_min = remSecs / SECS_PER_MIN % MINS_PER_HOUR;
    tm->tm_sec = remSecs % SECS_PER_MIN;

    return 0;
}

DEFAULT_VISIBILITY OS_SEC_L4_TEXT struct tm *gmtime_r(const time_t *restrict in_time, struct tm *restrict tm)
{
    uintptr_t intSave;

    if (in_time == NULL || tm == NULL) {
        errno = EINVAL;
        return NULL;
    }

    GMTIME_LOCK(intSave);
    if (OsTransSecsToTm(*in_time, tm) < 0) {
        GMTIME_UNLOCK(intSave);
        errno = EOVERFLOW;
        return NULL;
    }
    tm->tm_isdst = 0;
#ifdef __TM_GMTOFF
    tm->tm_gmtoff = 0;
#endif
#ifdef __TM_ZONE
    tm->tm_zone = __utc;
#endif
    GMTIME_UNLOCK(intSave);
    return tm;
}
#endif
