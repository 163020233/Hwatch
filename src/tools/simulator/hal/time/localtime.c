/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: simulator source file
 * Create: 2025-10
 */

#include "localtime.h"

struct tm* localtime_r(const time_t* t, struct tm* r)
{
    if (localtime_s(r, &time) != 0) {
        return NULL;
    }
    return r;
}
