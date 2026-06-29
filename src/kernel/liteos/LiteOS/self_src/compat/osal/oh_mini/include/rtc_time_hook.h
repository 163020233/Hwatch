/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: RTC Time Hook HeadFile
 * Author: Huawei LiteOS Team
 * Create : 2025-03-15
 */

#ifndef _RTC_TIME_HOOK_H
#define _RTC_TIME_HOOK_H

#include "los_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * RTC interfaces for improving the accuracy of the time, need implement and
 * register by user, implementation method is as follows:
 *
 * UINT64 RtcGetTickHook(VOID)
 * brief: supplement expanded ticks
 * input: VOID
 * return: UINT64 ticks
 *
 * INT32 RtcGetTimeHook(UINT64 *usec)
 * brief: get RTC time
 * output: UINT64 *usec, pointer to get the time, value is counted in microsecond(us)
 * return: non-0 if error, 0 if success.
 *
 * INT32 RtcSetTimeHook(UINT64 msec, UINT64 *usec)
 * brief: set RTC time
 * input: UINT64 msec, the time to be set counted in millisecond(ms).
 * input: UINT64 *usec, the time to be set counted in microsecond(us)
 * return: non-0 if error, 0 if success.
 *
 * INT32 RtcGetTimezoneHook(INT32 *tz)
 * brief: get RTC timezone
 * output: INT32 *tz, pointer to get the timezone, value is counted in second
 * return: non-0 if error, 0 if success.
 *
 * INT32 RtcSetTimezoneHook(INT32 tz)
 * brief: set RTC timezone
 * input: INT32 tz, the timezone to be set, value is counted in second
 * return: non-0 if error, 0 if success.
 */
struct RtcTimeHook {
    UINT64  (*RtcGetTickHook)(VOID);
    INT32   (*RtcGetTimeHook)(UINT64 *usec);
    INT32   (*RtcSetTimeHook)(UINT64 msec, UINT64 *usec);
    INT32   (*RtcGetTimezoneHook)(INT32 *tz);
    INT32   (*RtcSetTimezoneHook)(INT32 tz);
};

VOID LOS_RtcHookRegister(struct RtcTimeHook *cfg);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
