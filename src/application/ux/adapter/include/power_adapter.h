/**
  Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
*
* Description: Provide property interface that base on KV \n
*/

#ifndef POWER_HAL_H
#define POWER_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <errcode.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    POWER_GET_DATA_NONE = 0,
    POWER_GET_DATA_VOLTAGE,
    POWER_GET_DATA_SOC,
    POWER_GET_DATA_BOX_STATUS,
    POWER_GET_DATA_ALL,
    POWER_GET_DATA_MAX
} power_get_specific_data;

void power_get_real_data(int32_t data_type, void *data);

#ifdef __cplusplus
}
#endif

#endif
