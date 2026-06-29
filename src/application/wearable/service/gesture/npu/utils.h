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
GestureResult gestureInitModelEnv(void);
GestureResult gestureDestroyModelEnv(void);
GestureResult gestureInitResource(GestureModelInfo *modelInfo);
GestureResult gestureLoadModelFromFile(GestureModelInfo *modelInfo, const char *modelPath);
GestureResult gestureCreateInput(GestureModelInfo *modelInfo, void **inputAddr, uint32_t *inputSizeArr,
                                 uint32_t inputNumbers);
GestureResult gestureCreateOutput(GestureModelInfo *modelInfo, void **outputAddr, uint32_t *outputSizeArr,
                                  uint32_t outputNumbers);
GestureResult gestureExecute(GestureModelInfo *modelInfo);
GestureResult gestureDestroyResource(GestureModelInfo *modelInfo);
void gestureModelReset(GestureModelInfo *modelInfo);

#ifdef __cplusplus
}
#endif
#endif  // UTILS_H
