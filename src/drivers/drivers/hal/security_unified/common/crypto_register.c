/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crypto register utils.
 *
 * Create: 2025-05-04
*/
#include "crypto_drv_common.h"

#define FILE_ID FILE_ID_COMMON_CRYPTO_REGISTER

ROM_SHARE_BSS malloc_func_t g_crypto_malloc_func;
ROM_SHARE_BSS free_func_t g_crypto_free_func;
ROM_SHARE_BSS get_cpu_type_func_t g_get_cpu_type_func;
ROM_SHARE_BSS cache_flush_func_t g_cache_flush_func;
ROM_SHARE_BSS cache_inv_func_t g_cache_inv_func;
ROM_SHARE_BSS udelay_func_t g_crypto_udelay_func;

void crypto_register_mem_func(malloc_func_t malloc_func, free_func_t free_func)
{
    g_crypto_malloc_func = malloc_func;
    g_crypto_free_func = free_func;
}

void crypto_register_get_cpu_type_func(get_cpu_type_func_t func)
{
    g_get_cpu_type_func = func;
}

void crypto_register_cache_flush_func(cache_flush_func_t func)
{
    g_cache_flush_func = func;
}

void crypto_register_cache_inv_func(cache_inv_func_t func)
{
    g_cache_inv_func = func;
}

void crypto_register_udelay_func(udelay_func_t func)
{
    g_crypto_udelay_func = func;
}

void *crypto_malloc_coherent(uint32_t size)
{
    if (g_crypto_malloc_func == NULL) {
        return NULL;
    }
    return g_crypto_malloc_func(size);
}

void crypto_free_coherent(void *ptr)
{
    if (g_crypto_free_func == NULL) {
        return;
    }
    return g_crypto_free_func(ptr);
}

crypto_cpu_type crypto_get_cpu_type(void)
{
    if (g_get_cpu_type_func == NULL) {
        return CRYPTO_CPU_TYPE_SCPU;
    }
    return g_get_cpu_type_func();
}

void crypto_cache_flush(uintptr_t base_addr, uint32_t size)
{
    if (g_cache_flush_func == NULL) {
        return;
    }
    g_cache_flush_func(base_addr, size);
}

void crypto_cache_inv(uintptr_t base_addr, uint32_t size)
{
    if (g_cache_inv_func == NULL) {
        return;
    }
    g_cache_inv_func(base_addr, size);
}

void __attribute__((weak)) crypto_udelay(uint32_t useconds)
{
    if (g_crypto_udelay_func == NULL) {
        return;
    }
    return g_crypto_udelay_func(useconds);
}