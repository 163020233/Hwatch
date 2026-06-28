/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef HEART_RATE_MODEL_H
#define HEART_RATE_MODEL_H

#include "xacl_fmk_types.h"
#include "utils.h"
#ifdef __cplusplus
extern "C" {
#endif
#define HEART_RATE_INPUT_NUM 2
#define HEART_RATE_OUTPUT_NUM 1
#define HEART_RATE_INPUT1_SIZE 600
#define HEART_RATE_INPUT2_SIZE 6
#define HEART_RATE_OUTPUT_SIZE 2

HeartRateResult heartRateModelInit(const char *modelPath);
HeartRateResult heartRateModelExecute(void *input1, void *input2);
HeartRateResult heartRateModelCopyOutput(void *outputPoint);
void fp32ToFp16(const float *fp32, short *fp16, uint16_t size);
void fp16ToFp32(const short *fp16, float *fp32, uint16_t size);
void heartRateModelDestroy(void);

#ifdef __cplusplus
}
#endif
#endif  // HEART_RATE_MODEL_H