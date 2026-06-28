/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef ASRCTC_MODEL_H
#define ASRCTC_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define ASRCTC_MODEL_INPUT_SIZE_SPEECH 5600
#define ASRCTC_MODEL_INPUT_SIZE_POSEMB 8192
#define ASRCTC_MODEL_OUTPUT_SIZE 20624


Result asrctcModelInit(const char *modelPath, const char *modelOffsetPath);
Result asrctcModelExecute(void *speechData, void *posEmbData);
void asrctcModelGetOutput(void **outputPoint);
Result asrctcModelCopyOutput(void *outputPoint);
Result asrctcModelWriteOutput(const char *outputFilePath);
void asrctcModelDestroy(void);
Result asrctcModelCnnCacheReset(void);
#ifdef __cplusplus
}
#endif
#endif  // ASRCTC_MODEL_H