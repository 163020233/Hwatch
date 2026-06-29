/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef STFT_H
#define STFT_H

#define STFT_WIN_LEN 12
#define STFT_STEP_SIZE 1
#define SLOW_LEN 200
#define FAST_LEN 3
#define PADDED_LEN (2 * STFT_WIN_LEN)
#define FFT_SIZE (STFT_WIN_LEN + 1)
#define PI 3.14159265

/**
 * @brief 对输入的二维雷达数据进行STFT（短时傅里叶变换）
 * * @param data_win  输入的二维数据，类型为 float**
 * @param slow_len  输入数据的行数（慢时间维度）
 * @param fast_len  输入数据的列数（快时间维度）
 * @return float*** 返回一个三维数组，表示STFT的结果。维度为 [FFT_SIZE][stft_win_num][fast_len]。
 * 如果内存分配失败，返回 NULL。
 */
float ***radar_stft_multi_bin(float **data_win, int slow_len, int fast_len);

/**
 * @brief 创建一个新的经过变换和切片的float***三维数据结构。
 *
 * @details
 * 此函数接受一个维度为 (a, b, c) 的 float*** 输入，
 * 然后在内部动态分配内存，创建一个新的维度为 (c, a, b - 40) 的 float*** 结构，
 * 并将变换后的数据填入其中。
 *
 * 变换规则: output[k][i][j_sliced] = input[i][j_sliced + 20][k]
 *
 * @param input_data  输入的源数据，类型为 float***。
 * @param a           输入数据的第一个维度。
 * @param b           输入数据的第二个维度。
 * @param c           输入数据的第三个维度。
 * @return            成功时返回指向新创建的 float*** 数据的指针。
 * 如果内存分配失败或输入参数无效，则返回 NULL。
 *
 * @warning 调用者必须负责释放返回的指针。建议使用配套的 free_stft_data_3d 函数。
 */
float ***create_transformed_stft_3d(float ***input_data, int a, int b, int c);

/**
 * @brief 安全地释放由 create_transformed_stft_3d 创建的 float*** 结构。
 *
 * @param data   要释放的 float*** 指针。
 * @param dim1   要释放的数据的第一个维度 。
 * @param dim2   要释放的数据的第二个维度 。
 */
void free_stft_data_3d(float ***data, int dim1, int dim2);

#endif