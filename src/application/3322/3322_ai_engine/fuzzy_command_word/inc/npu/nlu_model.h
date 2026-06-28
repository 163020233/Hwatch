/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef NLU_MODEL_H
#define NLU_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define NLU_MODEL_INPUT_SIZE_SEQ 100
#define NLU_MODEL_INPUT_SIZE_SEQ_LEN 2
#define NLU_MODEL_OUTPUT_SIZE 212
#define NLU_MODEL_OUTPUT_SIZE_SLOT 11450

Result nluModelInit(const char *modelPath);
Result nluModelExecute(void *seqInputData, int32_t seqLengthData);
void nluModelGetOutput(void **outputPoint);
Result nluModelCopyOutput(void *outputPoint);
Result nluModelWriteOutput(const char *outputFilePath);
void nluModelGetOutputSlot(void **outputPoint);
Result nluModelCopyOutputSlot(void *outputPoint);
Result nluModelWriteOutputSlot(const char *outputFilePath);
void nluModelDestroy(void);
#ifdef __cplusplus
}
#endif
#endif  // NLU_MODEL_H