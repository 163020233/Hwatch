/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 */

#ifndef IMU_PREPROCESS_H
#define IMU_PREPROCESS_H

typedef struct {
    float Qw;
    float Qx;
    float Qy;
    float Qz;
} AbsoluteAlg;

AbsoluteAlg *AbsoluteAlg_create(void);

void AbsoluteAlg_destroy(AbsoluteAlg *self);

void AbsoluteAlg_attitude_from_acc(AbsoluteAlg *self, float ax, float ay, float az, float *roll, float *pitch);

typedef struct {
    float roll;
    float pitch;
} RelativeAlg;

RelativeAlg *RelativeAlg_create(void);

void RelativeAlg_destroy(RelativeAlg *self);

void RelativeAlg_reset(RelativeAlg *self);

void RelativeAlg_attitude_from_acc(RelativeAlg *self, float ax, float ay, float az, float *roll, float *pitch,
                                   float *yaw);

#endif