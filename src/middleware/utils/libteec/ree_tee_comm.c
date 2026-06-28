/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * filename:ree_tee_comm.c
 * Author: @CompanyNameTag
 * Create:2025-06-13
 * Description: ree tee communication entry API
 */

#include "ree_tee_comm.h"
#include "soc_osal.h"
#include "debug_print.h"
#include "los_memory.h"
#include "los_hwi.h"
#include "seliteos_memory_config.h"

uint32_t ree_tee_comm_entry(uint32_t cmd, void *arg);

static osal_atomic g_comm_mutex_inited = {0};
static osal_mutex g_ree_tee_comm_mutex = {0};

static void ree_tee_comm_mutex_init(void)
{
    osal_mutex_init(&g_ree_tee_comm_mutex);
}

uint32_t ree_tee_comm_unify_entry(uint32_t cmd, void *arg)
{
    uint32_t ret;

    if (osal_in_interrupt() == 1) {
        return ree_tee_comm_entry(cmd, arg);
    }

    if (osal_atomic_read(&g_comm_mutex_inited) == 0) {
        ree_tee_comm_mutex_init();
        osal_atomic_inc_return(&g_comm_mutex_inited);
        PRINT("ree_tee_comm_mutex inited!\n");
    }

    ret = (uint32_t)osal_mutex_lock(&g_ree_tee_comm_mutex);
    if (ret != OSAL_SUCCESS) {
        PRINT("osal_mutex_lock failed! ret = %#x!\n", ret);
        return ret;
    }

    ret = ree_tee_comm_entry(cmd, arg);
    osal_mutex_unlock(&g_ree_tee_comm_mutex);

    return ret;
}

void ree_tee_shared_memory_init(void)
{
    (void)osal_pool_mem_init((UINT8 *)(uintptr_t)SHARED_DATA_REE_START, SHARED_DATA_REE_ALLOC_SIZE);
}

void* ree_tee_shared_memory_alloc(uint32_t size)
{
    return osal_pool_mem_alloc((void *)(uintptr_t)SHARED_DATA_REE_START, size);
}

void ree_tee_shared_memory_free(void *buffer)
{
    (void)osal_pool_mem_free((void *)(uintptr_t)SHARED_DATA_REE_START, buffer);
}
