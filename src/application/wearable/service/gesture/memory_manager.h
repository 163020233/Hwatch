/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include "slide_window.h"
#include "data_preprocess.h"
#include "supp_functions.h"
#include "gesture_filter.h"

typedef enum { RESULT_ERROR = -1, RESULT_SUCCESS = 0 } ResultCode;

typedef struct {
    bool valid_flag;
    bool activation_flag;
    int pred;
} final_result;

typedef struct {
    int quit_num_long;  // 默认30
    int acc_fuse_curr_thre;  // 默认60
    int acc_fuse_prev_thre;  // 默认60
    bool trace_back_flag;  // 默认true
    bool pre_model_check_enable;  // 默认true
    float imu_gain_coeff;  // 默认0.7f
    float imu_sample_rate;  // 默认100f
    int trigger_num_thre;  // 默认3
    bool motor_flag;  // 默认false
    int trigger_thre;  // 默认20
    float seg_thre;  // 默认0.2
    int pre_check_thre;  // 默认2
    int rotate_roll_thre;  // 默认-80
    float status_thre;  // 默认-100
    bool force_clench;  // 默认false
    float roll_mov;  // 默认30
    float pitch_mov;  // 默认30
    float std_var;  // 默认0.4
    float ob_1;  // 默认2.0
    float ob_2;  // 默认0.25
    float ob_3;  // 默认2.0
    float ob_4;  // 默认0.2
    float ob_5;  // 默认2.5
    float ob_6;  // 默认0.5
    float roll_thre_diff;  // 默认20
    float pitch_thre_diff;  // 默认20
    int filter_type;  // 默认1
    float seg_thre_z;  // 默认0.2
} user_param;

typedef struct {
    float acc_mov;
    size_t capacity;
    float ax_2;
    float ay_2;
    float az_2;
    int cooldown_timer;
    int cooldown_timer_short;
    int start_index;
    int end_index;
    int prev_start;
    int prev_end;
    bool key_long;
    bool key_short;
    int timer_count;
    int initial_cooldown;
    int status;
    int prev_pred;
    bool correction_flag;
    bool activation_flag;
} general_var;

typedef struct {
    float *g_fuse;
    float *g_fuse_clip;
    DataRow *seg_data;
    DataRow *seg_data_smooth;
    float *seg_g_fuse;
    float *all_roll;
    float *all_pitch;
    float **seg_data_array;
    struct features_2 *extracted_features_2;
    float **padding_data;
    float ***AI_data;
    float ***AI_data_transfered;
    struct features *extracted_features;
} local_pointer_var;

typedef struct {
    DataManager *manager;
    SlideWindow *all_acc_fuse_clip;
    SegAlg_short *seg_alg_short;
    SegAlg *seg_alg;
    AbsoluteAlg *abs_alg;
    RelativeAlg *rlt_alg;
    GestureFilterEngine *filter_engine;
} global_pointer_var;

typedef struct {
    DataRow *seg_data;
    DataRow *seg_data_smooth;
    float **seg_data_array;
    float **padding_data;
    float ***AI_data;
    float ***AI_data_transfered;
    struct features *extracted_features;
    struct features_2 *extracted_features_2;
    int seg_data_nums;
    int seg_data_smooth_nums;
} ProcessingData;

extern final_result *g_result;
extern general_var *g_var_gen;
extern local_pointer_var *g_pointer_local;
extern global_pointer_var *g_pointer_global;

void free_all_var();
ResultCode allocate_all_memory(general_var *var_gen, local_pointer_var *pointer_local,
                               global_pointer_var *pointer_global);
void free_processing_data(ProcessingData *proc_data);

#ifdef SPORTS_HEALTH_NPU
bool AllocateNpuBuffers(void);
void FreeNpuBuffers(void);
#endif

#endif  //  MEMORY_MANAGER_H