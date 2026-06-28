/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef PPG_PREPROCESSING_H
#define PPG_PREPROCESSING_H

#include <stdbool.h>
#include <stdint.h>

extern bool g_print_control;
extern int8_t g_cooldown_timer;
extern uint16_t g_time_num;
extern uint16_t g_prev_num;
extern float g_last_filter_hr;
extern int16_t g_last_filter_timestamp;

typedef struct Movement Movement;

bool process_data(const float *data, const Movement *buf, float *ai_hr, float *prev_hr, float *smooth_hr);

#endif  // PPG_PREPROCESSING_H