/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef DATA_PREPROCESS_H
#define DATA_PREPROCESS_H
#include "data_stack.h"
#include "data_segmentation.h"
#include "imu_preprocess.h"

int data_pre_check_short(SegAlg_short *seg, DataManager *manager, RelativeAlg *algo2);
int data_pre_check(SegAlg *seg, DataManager *manager, RelativeAlg *algo2);
int check_roll_val(DataRow *seg_acc_data, int seg_acc_data_rows, float roll_s, float pitch_s, float yaw_s,
                   bool *valid_flag, int *status, float *min_roll, float *all_roll, float *all_pitch);

#endif