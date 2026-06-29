/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: Adapte Time HeadFile
 * Author: Huawei LiteOS Team
 * Create: 2025-03-01
 */
#ifndef _ADAPT_TIME_H
#define _ADAPT_TIME_H

#include "time_pri.h"
#include "limits.h"
#include "sys/times.h"
#include "errno.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/time.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

clock_t __clock(VOID);
int __clock_gettime(clockid_t, struct timespec *);
int __gettimeofday(struct timeval *tv, void *tz);
int __settimeofday(const struct timeval *tv, const struct timezone *tz);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
