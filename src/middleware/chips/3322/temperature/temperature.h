/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-12
 */

#ifndef TEMPERATURE_PORT_H
#define TEMPERATURE_PORT_H
#include "stdint.h"
#include "stdlib.h"

/**
 * @if Eng
 * @brief  get temperature from comram
 * @param  [in out]  temperature
 * @return return val, 0:success, other:fail.
 * @endif
 */
int32_t uapi_get_temperature(int32_t *temperature);

/**
 * @if Eng
 * @brief  set temperature to comram
 * @param  temperature data
 * @endif
 */
void uapi_set_temperature(int32_t data);

/**
 * @if Eng
 * @brief  temperature adapt init
 * @endif
 */
void uapi_temperature_adapt_init(void);
#endif