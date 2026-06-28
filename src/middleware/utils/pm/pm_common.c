/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides pm common source \n
 *
 * History: \n
 * 2025-07-22, Create file. \n
 */

#include "pm_common.h"
#include "osal_addr.h"

__attribute__((weak)) void *pm_kmalloc(uint32_t len)
{
    return (void *)osal_kmalloc(len, 0);
}

__attribute__((weak)) void *pm_kzalloc(uint32_t len)
{
    return (void *)osal_kzalloc(len, 0);
}

__attribute__((weak)) void pm_free(void *addr)
{
    osal_kfree(addr);
}