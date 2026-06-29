/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: malloc function.
 * Author: HiSilicon
 * History:
 * 2018-10-15， Create file.
 */
#include "chip_mem.h"
#include "securec.h"
#include "memory_config.h"
#include "irmalloc.h"

#define g_hal_sectors_ram_heap_start        IRMALLOC_HEAP_USE_DTCM_ORIGIN
#define g_hal_sectors_ram_heap_size         IRMALLOC_HEAP_USE_DTCM_LENGTH

int irmalloc_init_default(void)
{
    // Give irmalloc the heap to allocate from
    return irmalloc_new_pool((unsigned char *)g_hal_sectors_ram_heap_start, (uint32_t)g_hal_sectors_ram_heap_size);
}

/**
 * Add an additional memory pool to allocate from; the irmalloc implementation is not obliged to use this memory
 * @param start is the start memory address of the new pool
 * @param size is number of bytes in the new memory pool
 */
int irmalloc_new_pool(unsigned char *start, size_t size)
{
    int ret = conn_mem_init_pool(start, (unsigned int)size);
    if (ret != 0) {
        panic(PANIC_IRMALLOC_FAIL, __LINE__);
    }
    return ret;
}

static void *irmalloc_internal(size_t size)
{
    void *mem = conn_mem_alloc_enhanced((unsigned long)(uintptr_t)__builtin_return_address(0), (uint32_t)size);
    if (mem == NULL) {
        panic(PANIC_IRMALLOC_FAIL, __LINE__);
    }

    return mem;
}

void *irmalloc(size_t size)
{
    void *mem = irmalloc_internal(size);
    return mem;
}

void *irmalloc_panicking(size_t size)
{
    void *mem = irmalloc_internal(size);
    if (mem == NULL) {
        panic(PANIC_IRMALLOC_FAIL, (uint32_t)size);
    }

    return mem;
}

void *irzalloc(size_t size)
{
    void *buf = irmalloc_internal(size);
    if (buf != NULL) {
        if (memset_s(buf, size, 0, size) != EOK) {
            return (void *)NULL;
        }
    }
    return buf;
}

void *irrealloc(void *buf, size_t size)
{
    void *mem = conn_mem_realloc_enhanced((unsigned long)(uintptr_t)__builtin_return_address(0), buf, (uint32_t)size);
    if (mem == NULL) {
        panic(PANIC_IRMALLOC_FAIL, __LINE__);
    }

    return mem;
}

void *irrealloc_panicking(void *buf, size_t size)
{
    void *mem = conn_mem_realloc_enhanced((unsigned long)(uintptr_t)__builtin_return_address(0), buf, (uint32_t)size);
    if (mem == NULL) {
        panic(PANIC_IRMALLOC_FAIL, (uint32_t)size);
    }

    return mem;
}

void irfree(void *buf)
{
    if (buf == NULL) {
        return;
    }

    conn_mem_free_enhanced(buf);
}

void *irmalloc_dup(const void *orig, size_t size)
{
    if (orig == NULL) {
        return (void *)NULL;
    }

    void *ret = irmalloc_internal(size);
    if (ret == NULL) {
        return (void *)NULL;
    }

    if (memcpy_s(ret, size, orig, size) != EOK) {
        return (void *)NULL;
    }

    return ret;
}