/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef GESTURE_FILTER_H
#define GESTURE_FILTER_H

#include <stdint.h>
#include <stdbool.h>

#define SOS_SECTIONS 2

typedef struct {
    float z[2];
} SOSState;

typedef struct {
    SOSState x_state[SOS_SECTIONS];
    SOSState y_state[SOS_SECTIONS];
    SOSState z_state[SOS_SECTIONS];
} GestureFilterEngine;

GestureFilterEngine *gesture_filter_create(void);

void gesture_filter_destroy(GestureFilterEngine *engine);

void gesture_filter_apply(GestureFilterEngine *engine, float raw_x, float raw_y, float raw_z, float *out_x,
                          float *out_y, float *out_z);

bool clean_gesture_data(float **data_array, int n);
void gesture_filter_module_reset(void);

#endif  // GESTURE_FILTER_H