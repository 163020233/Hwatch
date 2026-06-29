/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Create Date : 2025-08
 */

#ifndef XACL_FMK_TYPES_H
#define XACL_FMK_TYPES_H

#include <stdbool.h>
#include "acl/acl.h"

#define SAFE_ACLRT_FREE(ptr, name)                      \
    do {                                                \
        if ((ptr) != NULL) {                            \
            if (aclrtFree(ptr) != ACL_SUCCESS) {        \
                printf("Model " name " free failed\n"); \
            }                                           \
            (ptr) = NULL;                               \
        }                                               \
    } while (0)

typedef enum GestureResult { GESTURE_SUCCESS = 0, GESTURE_FAILED = 1 } GestureResult;

typedef struct {
    uint32_t modelId;
    aclrtContext context;
    aclrtStream modelStream;
    aclmdlDataset *input;
    aclmdlDataset *output;
    void *weight;
    size_t weightSize;
    void *workspace;
    size_t workSize;
    bool modelLoaded;
} GestureModelInfo;

#endif /* XACL_FMK_TYPES_H */