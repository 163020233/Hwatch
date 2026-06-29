/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef AI_PREPROCESS_H
#define AI_PREPROCESS_H

#define WIN_LEN 12
#define STEP_SIZE 1
#define SLOW_LEN 200
#define FAST_LEN 3
#define PADDED_LEN (2 * WIN_LEN)
#define TRANSFORM_SIZE (WIN_LEN + 1)
#define PI 3.14159265

float ***preprocess_multi_bin(float **data_win, int slow_len, int fast_len);

float ***create_transformed_3d(float ***input_data, int a, int b, int c);

void free_transformed_data_3d(float ***data, int dim1, int dim2);

#endif