/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef GESTURE_ALGORITHM_H
#define GESTURE_ALGORITHM_H

#include <stdint.h>
#include "memory_manager.h"

extern final_result *g_result;
typedef void (*GestureCallback)(bool, bool, int);

user_param get_user_param(void);
void set_user_param(user_param param);
bool algorithm_stk_param_init(void);
void algorithm_stk_input(float x, float y, float z);
bool register_gesture_cb(GestureCallback gesture_cb);
void algorithm_stk_param_uninit(void);

#endif