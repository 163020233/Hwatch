/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: cmsis empty implement
 * Create: 2025-07-28
 */

#include "cmsis_os.h"

osStatus_t osDelay(uint32_t ticks)
{
    return 0;
}

osStatus_t osTimerDelete(osTimerId_t timer_id)
{
    return 0;
}

osTimerId_t osTimerNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr)
{
    return 0;
}

osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks)
{
    return 0;
}

uint64_t osMs2Tick(uint64_t ms)
{
    return 0;
}