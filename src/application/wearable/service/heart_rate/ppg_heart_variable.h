/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef PPG_HEART_VARIABLE_H
#define PPG_HEART_VARIABLE_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TOLERANCE 0.02
#define SQI_LEN 4
#define FILTER_ORDER 6
#define SIG_ARRAY_SIZE 125
#define DESIGN_MATRIX_SIZE (2 * SIG_ARRAY_SIZE)
#define L_EXT_ARRAY_SIZE (3 * (FILTER_ORDER + 1))
#define EXT_ARRAY_SIZE (2 * L_EXT_ARRAY_SIZE + SIG_ARRAY_SIZE)
#define QUADRATIC 2

#define INPUT1_BATCH 1
#define INPUT1_CHANNEL 2
#define INPUT1_LENGTH 150

#define INPUT2_SIZE 3

#define MAXPOOL_ROWS 2
#define MAXPOOL_COLS 13
#define POOL_SIZE 5

#define CON1_OUT_ROW 2
#define CON1_OUT_COL 68
#define CON2_OUT_ROW 2
#define CON2_OUT_COL 3

#define FC1_ROWS 3
#define FC1_COLS 6
#define FC2_ROWS 1
#define FC2_COLS 6
#define FC_OUTPUT_COLS 1

#define MAXFACTORS 32

#define RETURN_FALSE_IF_NULL(...)                                     \
    do {                                                              \
        const void *ptrs[] = {__VA_ARGS__};                           \
        for (size_t i = 0; i < sizeof(ptrs) / sizeof(ptrs[0]); ++i) { \
            if (ptrs[i] == NULL) {                                    \
                return false;                                         \
            }                                                         \
        }                                                             \
    } while (0)

#define SAFE_FREE(ptr)   \
    if ((ptr) != NULL) { \
        free((ptr));     \
        (ptr) = NULL;    \
    }

struct fft_complex {
    float r;
    float i;
};

struct fft_configuration {
    int nfft;
    int factors[2 * MAXFACTORS];
    struct fft_complex twiddles[1];
};

typedef struct fft_complex fft_cpx;
typedef struct fft_configuration fft_cfg;

extern const uint16_t g_sig_array_size;
extern const uint16_t g_fft_data_size;
extern const uint16_t g_preprocessed_data_size;
extern const uint16_t g_fft_size;

extern float *g_ppg_data_buf;
extern float *g_ppg_movement_x_buffer;
extern float *g_ppg_movement_y_buffer;
extern float *g_ppg_movement_z_buffer;
extern float *g_movement;
extern float *g_ppg_scaled;
extern float *g_preprocessed_data;
extern float *g_ppg_detrend;
extern float *g_ppg_detrend_filtered;
extern float *g_extended_buffer;
extern float *g_temp_buffer;
extern float *g_newdata;
extern float *g_design_matrix;
extern float *g_ppg_signal1_window_sqi;
extern float *g_corrs;
extern float *g_temp_ppg;
extern float *g_fft_data;
extern float *g_fft_temp;
extern fft_cfg *g_cfg;
extern fft_cpx *g_fin;
extern fft_cpx *g_fout;
#ifdef SPORTS_HEALTH_NPU
// 使用 npu 时用到的变量
extern short *g_npu_input_1;
extern short *g_npu_input_2;
#else
// 使用 mcu 时用到的变量
extern float ***g_mcu_input_1;
extern float *g_mcu_input_2;
extern float ***g_maxpooling_out;
extern float **g_con_output_1_ppg;
extern float **g_con_output_2_ppg;
extern float **g_output_reshaped_ppg;
extern float *g_fc1_output_ppg;
extern float *g_vstack_output_ppg;
extern float *g_fc2_output_ppg;
#endif

struct Movement {
    float *movement_x;
    float *movement_y;
    float *movement_z;
};

#endif  // PPG_HEART_VARIABLE_H