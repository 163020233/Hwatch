/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2025. All rights reserved. \n
 * Description: ux power customer file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#include "stddef.h"
#include "ux_config.h"
#include "ux_event_define.h"
#include "ux_power_customer.h"

#ifndef LOW_BATTERY_POWER_OFF_THRESHOLD
#define LOW_BATTERY_POWER_OFF_THRESHOLD 5
#endif

#ifndef LOW_BATTERY_WARNING_THRESHOLD
#define LOW_BATTERY_WARNING_THRESHOLD 15
#endif

#ifndef BATTERY_SOC_HEARTBEAT_TIME
#define BATTERY_SOC_HEARTBEAT_TIME 10000
#endif

#ifndef LOW_BATTERY_POWER_OFF_TIMEOUT
#define LOW_BATTERY_POWER_OFF_TIMEOUT 30000
#endif

static uint8_t get_power_off_soc(void)
{
    return LOW_BATTERY_POWER_OFF_THRESHOLD;
}

static uint8_t get_power_warning_soc(void)
{
    return LOW_BATTERY_WARNING_THRESHOLD;
}

static uint32_t get_power_monitor_time(void)
{
    return BATTERY_SOC_HEARTBEAT_TIME;
}

static uint32_t get_power_off_delay_time(void)
{
    return LOW_BATTERY_POWER_OFF_TIMEOUT;
}

ux_power_customer_funcs *ux_power_get_instance(void)
{
    static ux_power_customer_funcs func = {
        .get_power_off_soc        = get_power_off_soc,
        .get_power_warning_soc    = get_power_warning_soc,
        .get_power_monitor_time   = get_power_monitor_time,
        .get_power_off_delay_time = get_power_off_delay_time,
    };
    return &func;
}
