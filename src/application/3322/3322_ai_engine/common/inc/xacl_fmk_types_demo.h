/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef XACL_FMK_TYPES_DEMO_H
#define XACL_FMK_TYPES_DEMO_H

#include "acl/acl.h"
#define SAFE_ACLRT_FREE(ptr, name) \
    do { \
        if ((ptr) != NULL) { \
            if (aclrtFree(ptr) != ACL_SUCCESS) { \
                printf("Model " name " free failed\n"); \
            } \
            (ptr) = NULL; \
        } \
    } while (0)

typedef enum Result {
    SUCCESS = 0,
    FAILED = 1
} Result;

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
} ModelInfo;

#endif /* XACL_FMK_TYPES_DEMO_H */