/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef SLIDE_WINDOW_H
#define SLIDE_WINDOW_H
#include <stdbool.h>

typedef struct {
    float *data;
    int count;
} SlideWindow;

bool window_init(SlideWindow *window);

SlideWindow *window_create(void);

void window_destroy(SlideWindow *window);

bool window_append(SlideWindow *window, float value);

float window_get_max_in_slice(const SlideWindow *window, int start_index, int end_index);

bool pre_model_check(const SlideWindow *window, int start_index, int end_index);

bool variation_check(const SlideWindow *window, int start_index, int end_index);

#endif