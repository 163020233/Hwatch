/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef PPG_MATH_UTILS_H
#define PPG_MATH_UTILS_H

#include <float.h>
#include <stdint.h>
#include <stdbool.h>

void scale_data(const float *sig, float *std);

float correlation_coefficient(const float *x, const float *y, uint16_t n);
float calculate_rel_diff(const float *arr, float mean, uint16_t size);
float calculate_mean(const float *arr, uint16_t size);
float get_max_value(const float *arr, uint16_t size);
float calculate_std(const float *arr, uint16_t size);
bool calculate_signaltonoise(const float *arr, uint16_t size, float *vals);
bool calculate_high_order_moment(
    const float *a_zero_mean, uint16_t size, const uint16_t *n_list, uint16_t index, float *result);
void matrix_transpose_multiply(const float *A, const float *B, float *C, uint16_t rows);
void compute_ATA(const float *A, float ATA[2][2], uint16_t rows);
bool matrix_2x2_inverse(const float A[2][2], float inv[2][2]);
void compute_residuals(float *data, float *A, float *coef, uint16_t n_rows, uint16_t n_cols);
bool rfft(float *x, float *out);

#endif  // PPG_MATH_UTILS_H
