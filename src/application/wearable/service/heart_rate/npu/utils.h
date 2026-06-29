/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef UTILS_H
#define UTILS_H

#include "xacl_fmk_types.h"
#ifdef __cplusplus
extern "C" {
#endif
HeartRateResult heartRateInitModelEnv(void);
HeartRateResult heartRateDestroyModelEnv(void);
HeartRateResult heartRateInitResource(HeartRateModelInfo *modelInfo);
HeartRateResult heartRateLoadModelFromFile(HeartRateModelInfo *modelInfo, const char *modelPath);
HeartRateResult createModelDesc(HeartRateModelInfo *modelInfo, const char *modelPath);
HeartRateResult heartRateCreateInput(
    HeartRateModelInfo *modelInfo, void **inputAddr, uint32_t *inputSizeArr, uint32_t inputNumbers);
HeartRateResult heartRateCreateOutput(
    HeartRateModelInfo *modelInfo, void **outputAddr, uint32_t *outputSizeArr, uint32_t outputNumbers);
HeartRateResult heartRateExecute(HeartRateModelInfo *modelInfo);
HeartRateResult heartRateDestroyResource(HeartRateModelInfo *modelInfo);
void heartRateModelReset(HeartRateModelInfo *modelInfo);

#ifdef __cplusplus
}
#endif
#endif  // UTILS_H
