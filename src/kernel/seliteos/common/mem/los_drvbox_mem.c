/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2025. All rights reserved.
 * Description: drvbox memory management functions.
 *
 * Create: 2024-07-23
*/

#include "los_memory.h"
#include "los_memory_pri.h"
#include "los_mem.h"

LITE_OS_SEC_DRVBOX_TEXT UINT32 los_drvbox_mem_init(VOID)
{
    UINT32 ret;
    unsigned char *mem_pool = (unsigned char *)(uintptr_t)&__drvbox_heap_start;
    UINT32 mem_pool_size = (UINT32)((uintptr_t)&__drvbox_heap_size);
    ret = LOS_MemInit(mem_pool, mem_pool_size);
    return ret;
}

LITE_OS_SEC_DRVBOX_TEXT UINT32 los_drvbox_mem_deinit(VOID)
{
    UINT32 ret;
    unsigned char *mem_pool = (unsigned char *)(uintptr_t)&__drvbox_heap_start;
    ret = LOS_MemDeInit(mem_pool);
    return ret;
}

LITE_OS_SEC_DRVBOX_TEXT VOID* los_drvbox_malloc(INT32 size)
{
    unsigned char *mem_pool = (unsigned char *)(uintptr_t)&__drvbox_heap_start;
    VOID *ptr = LOS_MemAlloc(mem_pool, size);
    return ptr;
}

LITE_OS_SEC_DRVBOX_TEXT VOID los_drvbox_free(VOID *ptr)
{
    UINT32 ret;
    unsigned char *mem_pool = (unsigned char *)(uintptr_t)&__drvbox_heap_start;

    if (ptr == NULL) {
        return;
    }
    ret = LOS_MemFree(mem_pool, ptr);
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_MemFree failed, ret = %d\n", ret);
    }
}

#ifdef LOSCFG_MEM_LEAKCHECK
LITE_OS_SEC_DRVBOX_TEXT VOID los_drvbox_mem_info_print(VOID)
{
    unsigned char *mem_pool = (unsigned char *)(uintptr_t)&__drvbox_heap_start;
    OsMemInfoPrint(mem_pool);
}
#else
LITE_OS_SEC_DRVBOX_TEXT VOID los_drvbox_mem_info_print(VOID)
{
    PRINT_ERR("unsupport!\r\n");
}
#endif
