/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */


#ifndef FP32_FB16_H_TABLE
#define FP32_FB16_H_TABLE

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

unsigned short fitnessNvfloat2half(float m2);
void fitness_float16_fuzz(short *__restrict out, const float in);
void fitness_float32_fuzz(float *__restrict out, const short in);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif