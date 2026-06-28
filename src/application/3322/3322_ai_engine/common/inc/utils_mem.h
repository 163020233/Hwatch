/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef UTILS_MEM_H_
#define UTILS_MEM_H_

#include "securec.h"
#include "memory_config.h"
#include "osal_addr.h"
#define BOUNDARY 8

static inline int demo_malloc(void **devPtr, size_t size)
{
    if (devPtr == NULL || size == 0) {
        return -1;
    }
    uintptr_t pool_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_BSP);
    *devPtr = osal_pool_mem_alloc_align((void *)pool_addr, size, BOUNDARY);
    if (*devPtr == NULL) {
        return -1;
    }
    return 0;
}

static inline int demo_zalloc(void **devPtr, size_t size)
{
    if (demo_malloc(devPtr, size) == 0) {
        return memset_s(*devPtr, size, 0, size);
    }
    return -1;
}

static inline void demo_free(void *devPtr)
{
    uintptr_t pool_addr = uapi_sys_get_memory_pool_addr(MEM_POOL_BSP);
    osal_pool_mem_free((void *)pool_addr, devPtr);
}

#endif /* UTILS_MEM_H_ */