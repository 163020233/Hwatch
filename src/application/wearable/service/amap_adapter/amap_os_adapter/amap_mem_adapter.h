/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Create Date : 2025-11
 */

#ifndef AMAP_MEM_ADAPTER
#define AMAP_MEM_ADAPTER

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void amap_mem_free(void *ptr);
void amap_fast_mem_free(void *ptr);
void amap_fast_mem_free_type(void *ptr, int type);
void *amap_mem_malloc(size_t size);
void *amap_mem_calloc(size_t count, size_t size);
void *amap_mem_realloc(void *ptr, size_t size);
// type 内存类型，0表示tile缓存内存，1表示tile绘制内存
void *amap_fast_mem_malloc(size_t size, int type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif  // AMAP_MEM_ADAPTER