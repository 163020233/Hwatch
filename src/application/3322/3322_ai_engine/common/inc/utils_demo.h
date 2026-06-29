/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef UTILS_DEMO_H
#define UTILS_DEMO_H

#include "xacl_fmk_types_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
Result initModelEnv(void);
Result destroyModelEnv(void);
Result initResourceDemo(ModelInfo *modelInfo);
Result loadModelFromFileDemo(ModelInfo *modelInfo, const char *modelPath);
Result createModelDescDemo(ModelInfo *modelInfo, const char *modelPath);
Result createInputDemo(ModelInfo *modelInfo, void **inputAddr, const uint32_t *inputSizeArr, const uint32_t inputNumbers);
Result createOutputDemo(ModelInfo *modelInfo, void **outputAddr, const uint32_t *outputSizeArr, const uint32_t outputNumbers);
Result executeDemo(ModelInfo *modelInfo);
Result destroyResourceDemo(ModelInfo *modelInfo);
void modelReset(ModelInfo *modelInfo);
Result modelWriteOutput(const char *outputFilePath, uint32_t outputFileSize, void *modelOutput);
#ifdef __cplusplus
}
#endif
#endif  /* UTILS_DEMO_H */