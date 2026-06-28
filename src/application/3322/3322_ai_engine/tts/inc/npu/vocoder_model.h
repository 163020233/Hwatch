/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef VOCODER_MODEL_H
#define VOCODER_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define VOCODER_INPUT_NUM 1
#define VOCODER_OUTPUT_NUM 1

Result vocoderModelInit(const char *modelPath);
Result vocoderModelExecute(void *modelInputData);
void vocoderModelGetOutput(void **outputPoint);
Result vocoderModelCopyOutput(void *outputPoint);
Result vocoderModelWriteOutput(const char *outputFilePath);
void vocoderModelDestroy(void);
#ifdef __cplusplus
}
#endif
#endif  // VOCODER_MODEL_H