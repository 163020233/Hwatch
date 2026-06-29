/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef AM_MODEL_H
#define AM_MODEL_H

#include "utils_demo.h"
#ifdef __cplusplus
extern "C" {
#endif
#define AM_INPUT_NUM 2
#define AM_OUTPUT_NUM 2

Result amModelInit(const char *modelPath);
Result amModelExecute(void *phonemeData, void *phonemeMaskData);
void amModelGetOutput(void **repeatitionPoint, void **melPoint);
Result amModelCopyOutput(void *repeatitionPoint, void *melPoint);
Result amModelWriteOutput(const char *repeatitionFilePath, const char *melFilePath);
void amModelDestroy(void);
#ifdef __cplusplus
}
#endif
#endif  // AM_MODEL_H