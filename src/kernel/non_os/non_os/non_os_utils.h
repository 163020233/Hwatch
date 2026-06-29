/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description: NON OS Utils
 * Author:
 * Create:  2018-10-15
 */

#ifndef NON_OS_UTILS_H
#define NON_OS_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/* mem pool hook function */
typedef struct {
    void  *(*mem_pool_alloc)(uint32_t pool_id, uint32_t size);
    bool   (*mem_pool_free)(uint32_t pool_id,  void *addr);
} mem_pool_hook_ops;

void mem_pool_ops_hook_reg(const mem_pool_hook_ops *ops);

void *mem_pool_malloc_hook(uint32_t pool_id, uint32_t size);

bool mem_pool_mfree_hook(uint32_t pool_id, void *addr);

/* mem dfx hook function */
typedef struct {
    uint32_t (*mem_alloc_proc)(void *addr, uint32_t size, uintptr_t caller_ra, uint32_t task_id);
    uint32_t (*mem_free_proc)(void *addr);
} mem_dfx_hook_ops;

void mem_dfx_ops_hook_reg(const mem_dfx_hook_ops *Ops);

void mem_alloc_dfx_hook(void *addr, uint32_t size, uintptr_t caller_ra);

void mem_free_dfx_hook(void *addr);

#endif
