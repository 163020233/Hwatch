/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef DATA_STACK_H
#define DATA_STACK_H

#include <stddef.h>  // for size_t
#include <stdbool.h>

typedef struct {
    float ax;
    float ay;
    float az;
    float ppg;
} DataRow;

typedef struct DataManager DataManager;

DataManager *data_manager_create(size_t capacity);

void data_manager_process(DataManager *manager, float ax_f, float ay_f, float az_f, float ax, float ay, float az,
                          float ppg, float ax_2, float ay_2, float az_2);

void data_manager_destroy(DataManager *manager);

size_t data_manager_get_buffer1_count(const DataManager *manager);
const DataRow *data_manager_get_buffer1_row(const DataManager *manager, size_t index);

size_t data_manager_get_buffer2_count(const DataManager *manager);

size_t data_manager_get_clip_buffer_count(const DataManager *manager);
const DataRow *data_manager_get_clip_buffer_row(const DataManager *manager, size_t index);

DataRow *data_manager_copy_buffer2_slice(const DataManager *manager, size_t logical_start, size_t logical_end,
                                         int *out_num_rows);

DataRow *data_manager_copy_buffer1_slice(const DataManager *manager, size_t logical_start, size_t logical_end,
                                         int *out_num_rows);

bool data_manager_calculate_means_in_range(const DataManager *manager, size_t logical_start, size_t logical_end,
                                           float *out_mean_ax, float *out_mean_ay, float *out_mean_az);

void find_max(const DataRow *seg_data, float *max_x, float *max_y, float *max_z, int end_index);

void find_diff_max(const DataRow *seg_data, float *max_x_diff, float *max_y_diff, float *max_z_diff, int end_index);

#endif  // DATA_MANAGER_H
