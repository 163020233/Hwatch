/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef PPG_INTERFACE_H
#define PPG_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*SetHeartRateCallback)(uint8_t);
extern float g_smooth_hr;
extern float g_prev_hr;
extern float g_ai_hr;

bool ppg_algo_input(int32_t value, float x, float y, float z);
bool ai_ppg_init(void);
void ai_ppg_uninit(void);
bool register_set_heart_rate_cb(SetHeartRateCallback set_heart_rate_cb);

#endif  // PPG_INTERFACE_H