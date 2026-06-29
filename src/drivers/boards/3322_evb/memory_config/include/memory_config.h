/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:  2020-01
 */
#ifndef MEMORY_CONFIG_H
#define MEMORY_CONFIG_H

#include <stdint.h>
#include <stdbool.h>
#include "memory_config_common.h"

typedef enum {
    MEM_POOL_BSP    = 0,
    MEM_POOL_FS     = 1,
    MEM_POOL_UI     = 2,
    MEM_POOL_OHOS   = 3,
    MEM_POOL_VDEC   = 4,
    MEM_POOL_BGH    = 5,
    MEM_POOL_VENC   = 6,
    MEM_POOL_VICAP  = 7,
    MEM_POOL_NPU_RAM = 8,
    MEM_POOL_MAX,
} memory_pool_type;

typedef struct mem_pool {
    memory_pool_type id;
    uint32_t start_addr;
    uint32_t size;
    bool cacheable;
    bool need_init;
} memory_pool;

uint32_t uapi_sys_get_memory_pool_addr(uint8_t sub_mod_id);

bool uapi_sys_get_memory_pool_cache_attr(uint8_t sub_mod_id);

uint32_t uapi_sys_get_memory_pool_length(uint8_t sub_mod_id);

bool uapi_sys_get_memory_pool_need_init(uint8_t sub_mod_id);

#endif
