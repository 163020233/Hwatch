/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef DATA_SEGMENTATION
#define DATA_SEGMENTATION
#include <stdbool.h>

typedef struct {
    int trigger_thres;
    int trigger_num;
    int trigger_cap;
    int trigger_offset;

    int quit_num;
    int quit_cap;
    int quit_offset;

    int status;
    int cooldown_timer;
    int trigger_cnt;
    int quit_cnt;

    int start_idx;
    int end_idx;

    int seg_ready_flag;
    int count;
} SegAlg;

typedef struct {
    int trigger_thres;
    int trigger_num;
    int trigger_cap;
    int trigger_offset;

    int quit_num;
    int quit_cap;
    int quit_offset;

    int status;
    int cooldown_timer;
    int trigger_cnt;
    int quit_cnt;

    int start_idx;
    int end_idx;

    int seg_ready_flag;
    int count;
} SegAlg_short;

SegAlg *SegAlg_create(void);

void SegAlg_destroy(SegAlg *self);

void SegAlg_reset_counter(SegAlg *self);

int SegAlg_IMU_seg_main(SegAlg *self, float acc_fuse_curr, float acc_fuse_prev, int cooldown_timer, bool key_long);

SegAlg_short *SegAlg_short_create(void);

void SegAlg_short_destroy(SegAlg_short *self);

void SegAlg_short_reset_counter(SegAlg_short *self);

int SegAlg_short_IMU_seg_main(SegAlg_short *self, float acc_fuse_curr, float acc_fuse_prev, int cooldown_timer,
                              bool key_short, int initial_cooldown);

#endif