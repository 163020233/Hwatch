/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: simulator source file
 * Create: 2025-10
 */

#ifndef LOCAL_TIME_H
#define LOCAL_TIME_H

#include "sys/time.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct tm* localtime_r(const time_t* t, struct tm* r);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif /* LOCAL_TIME_H */
