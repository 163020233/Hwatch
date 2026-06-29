/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "display_type.h"
#include "gralloc_c_api.h"
#define AMAP_LOG_LEVEL 1
#include "amap_log_adapter.h"

enum AmapMemType {
    AMAP_MEM_TILE_CACHE = 0,
    AMAP_MEM_TILE_DRAW = 1,
    AMAP_MEM_GPU_BUFFER = 3,
};

void amap_mem_free(void *ptr)
{
    if (ptr) {
        free(ptr);
        AMAP_LOGI("amap_mem_free");
    }
}

void amap_fast_mem_free(void *ptr)
{
    if (ptr) {
        GrallocFreeBuffer(ptr);
        AMAP_LOGW("UNEXPECTED: amap_fast_mem_free called!");
    }
}

void amap_fast_mem_free_type(void *ptr, int type)
{
    if (type == AMAP_MEM_TILE_CACHE || type == AMAP_MEM_GPU_BUFFER) {
        GrallocFreeBuffer(ptr);
        AMAP_LOGI("free gralloc: type=%d", type);
    } else {
        free(ptr);
        AMAP_LOGI("free malloc: type=%d", type);
    }
}

void *amap_mem_malloc(size_t size)
{
    void *ptr = malloc(size);
    AMAP_LOGI("amap_mem_malloc size=%zu", size);
    return ptr;
}

void *amap_mem_calloc(size_t count, size_t size)
{
    AMAP_LOGI("amap_mem_calloc ptrsize = %zu", size);
    return calloc(count, size);
}

void *amap_mem_realloc(void *ptr, size_t size)
{
    void *newPtr = realloc(ptr, size);
    if (newPtr == NULL) {
        AMAP_LOGE("amap_mem_realloc failed size=%zu", size);
        return NULL;
    }
    AMAP_LOGI("amap_mem_realloc size=%zu", size);
    return newPtr;
}

static unsigned int GetGrallocFlag(int type)
{
    switch (type) {
        case AMAP_MEM_GPU_BUFFER:
            return HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_BUFMAP;
        case AMAP_MEM_TILE_CACHE:
            return HBM_USE_ASSIGN_SIZE | HBM_USE_MEM_JPEG_DATA;
        default:
            return 0;
    }
}

void *amap_fast_mem_malloc(size_t size, int type)
{
    unsigned int flag = GetGrallocFlag(type);
    void *buf;
    if (flag != 0) {
        buf = (void *)GrallocAllocBufferBySize(size, flag);
        AMAP_LOGI("GrallocAllocBufferBySize size=%zu", size);
    } else {
        buf = malloc(size);
        AMAP_LOGI("fast_mem_malloc(gralloc path) size=%zu type=%d", size, type);
    }
    if (buf == NULL) {
        AMAP_LOGE("fast_mem_malloc failed, size=%zu, type=%d", size, type);
    }
    return buf;
}