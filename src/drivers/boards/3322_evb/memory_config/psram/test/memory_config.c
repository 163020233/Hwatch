/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description:  Default memory configurations
 * Author: @CompanyNameTag
 * Create:
 */
#include "memory_config.h"
#if defined(__LITEOS__)
#include "los_memory.h"

memory_pool g_sys_mem_pool[MEM_POOL_MAX] = {
    // MEM_POOL_BSP = 0
    {
        MEM_POOL_BSP,
        (uint32_t)OS_EXTEND_MEM_ADDR,
        (uint32_t)OS_EXTEND_MEM_SIZE,
        false,
        false,
    },
    // MEM_POOL_FS = 1
    {
        MEM_POOL_FS,
        PSRAM_FS_ADDR,
        PSRAM_FS_LENGTH,
        true,
        false,
    },
    // MEM_POOL_UI = 2
    {
        MEM_POOL_UI,
        PSRAM_GUI_MEM_POOL_ADDR,
        PSRAM_GUI_MEM_POOL_SIZE,
        false,
        false,
    },
    // MEM_POOL_OHOS = 3
    {
        MEM_POOL_OHOS,
        PSRAM_HM_APP_SYS_ADDR,
        PSRAM_HM_APP_SYS_LENGTH,
        false,
        true,
    },
    // MEM_POOL_VDEC = 4
    {
        MEM_POOL_VDEC,
        (uint32_t)OS_EXTEND_MEM_ADDR,
        (uint32_t)OS_EXTEND_MEM_SIZE,
        true,
        false,
    },
    // MEM_POOL_BGH = 5
    {
        MEM_POOL_BGH,
        (uint32_t)OS_EXTEND_MEM_ADDR,
        (uint32_t)OS_EXTEND_MEM_SIZE,
        true,
        false,
    },
    // MEM_POOL_VENC = 6
    {
        MEM_POOL_VENC,
        PSRAM_VENC_MEM_POOL_ADDR,
        PSRAM_VENC_MEM_POOL_SIZE,
        false,
        true,
    },
    // MEM_POOL_VICAP = 7
    {
        MEM_POOL_VICAP,
        PSRAM_VICAP_MEM_POOL_ADDR,
        PSRAM_VICAP_MEM_POOL_SIZE,
        false,
        true,
    },
    // MEM_POOL_NPU_RAM = 8
    {
        MEM_POOL_NPU_RAM,
        (uint32_t)L2M_NPU_MEM_START,
        (uint32_t)L2M_NPU_MEM_LENGTH,
        false,
        false,
    },
};

uint32_t uapi_sys_get_memory_pool_addr(uint8_t sub_mod_id)
{
    return g_sys_mem_pool[sub_mod_id].start_addr;
}

uint32_t uapi_sys_get_memory_pool_length(uint8_t sub_mod_id)
{
    return g_sys_mem_pool[sub_mod_id].size;
}

bool uapi_sys_get_memory_pool_cache_attr(uint8_t sub_mod_id)
{
    return g_sys_mem_pool[sub_mod_id].cacheable;
}

bool uapi_sys_get_memory_pool_need_init(uint8_t sub_mod_id)
{
    return g_sys_mem_pool[sub_mod_id].need_init;
}

#endif