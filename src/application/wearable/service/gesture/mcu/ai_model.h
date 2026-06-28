/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef AI_MODEL_H
#define AI_MODEL_H

bool ai_model_init();
void ai_model_uninit();
bool ai_model_execute(float ***input_1, float *input_2, float *output, int output_count);

#endif  // AI_MODEL_H
