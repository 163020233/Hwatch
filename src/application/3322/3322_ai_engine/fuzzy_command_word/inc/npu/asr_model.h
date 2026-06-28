/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef ASR_MODEL_H
#define ASR_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define ASR_MODEL_INPUT_SIZE_SPEECH 5600
#define ASR_MODEL_INPUT_SIZE_POSEMB 8192
#define ASR_MODEL_OUTPUT_SIZE 1024

Result asrModelSetCnnCacheMergeNum(uint32_t mergeNum);
Result asrModelInit(const char *modelPath, const char *modelOffsetPath);
Result asrModelExecute(void *speechData, void *posEmbData);
void asrModelGetOutput(void **outputPoint);
Result asrModelCopyOutput(void *outputPoint);
Result asrModelWriteOutput(const char *outputFilePath);
void asrModelDestroy(void);
Result asrModelOutputReset(void);
#ifdef __cplusplus
}
#endif
#endif  // ASR_MODEL_H