/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef GESTURE_MODEL_H
#define GESTURE_MODEL_H

#include "xacl_fmk_types.h"
#include "utils.h"
#ifdef __cplusplus
extern "C" {
#endif
#define GESTURE_INPUT_NUM 2
#define GESTURE_OUTPUT_NUM 1
#define GESTURE_INPUT1_ROWS 3
#define GESTURE_INPUT1_COLS 13
#define GESTURE_INPUT1_DEPTH 149
#define GESTURE_INPUT1_COUNT (GESTURE_INPUT1_ROWS * GESTURE_INPUT1_COLS * GESTURE_INPUT1_DEPTH)  // 元素个数
#define GESTURE_INPUT1_SIZE (GESTURE_INPUT1_COUNT * sizeof(short))  // 字节数
#define GESTURE_INPUT2_COUNT 8
#define GESTURE_INPUT2_SIZE (GESTURE_INPUT2_COUNT * sizeof(short))
#define GESTURE_OUTPUT_COUNT 6
#define GESTURE_OUTPUT_SIZE (GESTURE_OUTPUT_COUNT * sizeof(short))

GestureResult gestureModelInit(const char *modelPath);
GestureResult gestureModelExecute(void *input1, void *input2);
GestureResult gestureModelCopyOutput(void *outputPoint);
void fp32ToFp16Gesture(const float *fp32, short *fp16, uint16_t size);
void convert3Dto1DAndFp16(float ***src, short *dst);
void fp16ToFp32Gesture(const short *fp16, float *fp32, uint16_t size);

void gestureModelDestroy(void);

#ifdef __cplusplus
}
#endif
#endif  // GESTURE_MODEL_H