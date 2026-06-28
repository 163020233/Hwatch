/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: usr memory management functions.
 *
 * Create: 2024-07-23
*/

#include "los_memory.h"
#include "los_memory_pri.h"
#include "los_task_pri.h"
#include "ta_manager.h"
#include "los_mem.h"

VOID* los_usr_malloc(INT32 size)
{
    struct ta_header *ta = NULL;
    TEE_Result ret = TEE_SUCCESS;
    unsigned char *mem_pool = NULL;
    VOID *ptr = NULL;

    ret = usr_get_cur_ta(&ta);
    if (ret != TEE_SUCCESS) {
        return NULL;
    }

    mem_pool = (unsigned char *)(uintptr_t)(ta->data_addr);
    ptr = LOS_MemAlloc(mem_pool, size);

    return ptr;
}

VOID los_usr_free(VOID *ptr)
{
    struct ta_header *ta = NULL;
    unsigned char *mem_pool = NULL;
    TEE_Result ret = TEE_SUCCESS;

    if (ptr == NULL) {
        return;
    }

    ret = usr_get_cur_ta(&ta);
    if (ret != TEE_SUCCESS) {
        return;
    }

    mem_pool = (unsigned char *)(uintptr_t)(ta->data_addr);
    ret = LOS_MemFree(mem_pool, ptr);
    if (ret != LOS_OK) {
        PRINT_ERR("LOS_MemFree failed, ret = %d\n", ret);
    }
}

#ifdef LOSCFG_MEM_LEAKCHECK
VOID los_usr_mem_info_print(VOID)
{
    struct ta_header *ta = NULL;
    unsigned char *mem_pool = NULL;

    if (usr_get_cur_ta(&ta) == TEE_SUCCESS) {
        mem_pool = (unsigned char *)(uintptr_t)(ta->data_addr);
        OsMemInfoPrint(mem_pool);
    } else {
        PRINT_ERR("wrong TA\n");
    }
}
#else
VOID los_usr_mem_info_print(VOID)
{
    PRINT_ERR("unsupport!\r\n");
}
#endif
