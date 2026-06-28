/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: crash data porting
 * This file should be changed only infrequently and with great care.
 */

#include "crash_data.h"
#include "dfx_adapt_layer.h"

#ifdef SUPPORT_CRASH_DATA_RAM


extern void *g_extend_heap_begin;
extern void *g_extend_heap_size;

#define OS_EXTEND_MEM_ADDR    ((void *)&g_extend_heap_begin)
#define OS_EXTEND_MEM_SIZE    ((unsigned)&g_extend_heap_size)


static const crash_data_mem_t g_bt_crash_data_info[] = {
    {"/user/exc/bt_exc_info.bin", PRESERVED_REGION_ORIGIN, PRESERVED_REGION_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/bt_exc_info.bin", APP_PRESERVED_REGION_ORIGIN, APP_PRESERVED_REGION_LENGTH,
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/bt_exc_info.bin", CH_BT_REGION_START, CH_BT_REGION_LEN, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/BT_RAM_ORIGIN.bin", BT_RAM_ORIGIN_APP_MAPPING, BT_RAM_ORIGIN_APP_MAPPING_LENGTH,
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/SHARED_MEM.bin", SHARED_MEM_START, SHARED_MEM_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/BCPU_TRACE_MEM_REGION.bin", BCPU_TRACE_MEM_REGION_START, BCPU_TRACE_MEM_REGION_LENGTH,
        CRASH_DATA_MEM_TYPE_TRACE, 0, 0},
};
static const crash_data_mem_t g_dsp_crash_data_info[] = {
    {"/user/exc/dsp_exc_info.bin", PRESERVED_REGION_ORIGIN, PRESERVED_REGION_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/dsp_exc_info.bin", APP_PRESERVED_REGION_ORIGIN, APP_PRESERVED_REGION_LENGTH,
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
};
static const crash_data_mem_t g_app_crash_data_info[] = {
    {"/user/exc/app_exc_info.bin", PRESERVED_REGION_ORIGIN, PRESERVED_REGION_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/app_exc_info.bin", APP_PRESERVED_REGION_ORIGIN, APP_PRESERVED_REGION_LENGTH,
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/app_exc_info.bin", CH_APP_REGION_START, CH_APP_REGION_LEN, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/APP_ITCM_ORIGIN.bin", (REE_ITCM_ORIGIN + 0x800), (REE_ITCM_LENGTH - 0x800),
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/APP_DTCM_ORIGIN.bin", REE_DTCM_ORIGIN, REE_DTCM_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
    {"/user/exc/SHARED_MEM.bin", SHARED_MEM_START, SHARED_MEM_LENGTH, CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
#ifndef CFG_NORFLASH_ONLY
    {"/user/exc/APP_PSRAM_HEAP.bin", (uint32_t)(uintptr_t)PSRAM_HEAP_ADDR, PSRAM_HEAP_LENGTH,
        CRASH_DATA_MEM_TYPE_SHARED, 0, 0},
#endif
    {"/user/exc/MCPU_TRACE_MEM_REGION.bin", MCPU_TRACE_MEM_REGION_START, MCPU_TRACE_MEM_REGION_LENGTH,
        CRASH_DATA_MEM_TYPE_TRACE, 0, 0},
};

crash_data_mem_t *crash_data_get_info(uint8_t core, uint32_t *num)
{
    if (core == CORES_BT_CORE) {
        *num = (uint32_t)(sizeof(g_bt_crash_data_info) / sizeof(crash_data_mem_t));
        return g_bt_crash_data_info;
    } else if (core == CORES_PROTOCOL_CORE) {
        *num = (uint32_t)(sizeof(g_dsp_crash_data_info) / sizeof(crash_data_mem_t));
        return g_dsp_crash_data_info;
    } else if (core == CORES_APPS_CORE) {
        *num = (uint32_t)(sizeof(g_app_crash_data_info) / sizeof(crash_data_mem_t));
        return g_app_crash_data_info;
    }

    *num = 0;
    return NULL;
}

crash_data_t *crash_data_get_by_core(uint8_t core)
{
    if (core == CORES_APPS_CORE) {
        return (crash_data_t *)(uintptr_t)(CH_APP_REGION_START);
    } else if (core == CORES_BT_CORE) {
        return (crash_data_t *)(uintptr_t)(CH_BT_REGION_START);
    }
    return NULL;
}

#endif /* SUPPORT_CRASH_DATA_RAM */
