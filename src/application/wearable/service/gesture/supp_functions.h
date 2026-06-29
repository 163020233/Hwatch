/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <stdbool.h>

struct GestureResultData {
    int pred;
    bool valid_flag;
};

struct features {
    float width;
    float acc_fuse_clip_width;
    float max_g_fuse_1st;
    float max_g_fuse_2nd;
};

struct features_2 {
    float mean_1;
    float mean_2;
    float mean_3;
};

struct GestureResultData *post_check(int pred, float all_roll[], int array_size, int data_seg_flag_short, float width,
                                     int status, bool acc_max_diff_flag, int ob_flag);

struct features *feature_extract(float seg_g_fuse[], int array_size, float g_fuse_mean);

struct features_2 *feature_extract_2(float **seg_data, int row);

float **padding_zeros(float **seg_data, int row, int col);

bool check_amp(float seg_g_fuse[], int array_size);

int get_argmax(float softmax_data[], int array_size);

void free_2d_pointer(float **data, int rows);

bool check_sum(float **seg_data, int row, int status);

#endif
