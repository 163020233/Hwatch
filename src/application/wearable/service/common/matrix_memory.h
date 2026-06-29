/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef MATRIX_MEMORY_H
#define MATRIX_MEMORY_H

// 2D matrix
float **allocate_matrix_2D(int d1, int d2);
void free_matrix_2D(float ***matrix, int rows);

// 3D matrix
float ***allocate_matrix_3D(int d1, int d2, int d3);
void free_matrix_3D(float ****matrix, int rows, int cols);

#endif  // MATRIX_MEMORY_H
