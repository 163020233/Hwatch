/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-12
 */

#include "temperature.h"
#include "memory_config_shared.h"
#include "product_evb_standard.h"

static int32_t *g_temperature_data = (int32_t *)(uintptr_t)TEMPERATURE_DATA_REGION_START;

int32_t uapi_get_temperature(int32_t *temperature)
{
    if (*g_temperature_data == 0xff) {
        return -1;
    }
    *temperature = *g_temperature_data;
    return 0;
}

void uapi_set_temperature(int32_t data)
{
    *g_temperature_data = data;
}

void uapi_temperature_adapt_init(void)
{
    *g_temperature_data = 0xff;
}