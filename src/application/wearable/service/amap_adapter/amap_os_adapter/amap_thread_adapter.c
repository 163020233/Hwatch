/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#include <stdio.h>
#include <stdint.h>
#include <osal_task.h>
#include <pthread.h>

uint64_t amap_get_thread_id(void)
{
    return osal_get_current_tid();
}
