/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef CTC_MODEL_H
#define CTC_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define CTC_MODEL_INPUT_SIZE 15360
#define CTC_MODEL_OUTPUT_SIZE 309360

Result ctcModelInit(const char *modelPath);
Result ctcModelExecute(void *inputData);
void ctcModelGetOutput(void **outputPoint);
Result ctcModelCopyOutput(void *outputPoint);
Result ctcModelWriteOutput(const char *outputFilePath);
void ctcModelDestroy(void);
#ifdef __cplusplus
}
#endif
#endif  // CTC_MODEL_H