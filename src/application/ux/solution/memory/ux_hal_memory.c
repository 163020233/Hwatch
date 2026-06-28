/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux memory manager.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-03, Create file. \n
 */

#include "securec.h"
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
#include "los_memory.h"
#endif
#include "ux_common_log.h"
#include "osal_mutex.h"
#include "osal_adapt.h"
#include "ux_hal_memory.h"

#define THRESHOLD_FOR_PRINT_MEM_USED    10

#ifdef UX_MEM_DEBUG_SUPPORT
uint16_t g_mem_module_used_cnt[UX_MEM_MAX] = { 0 };
static osal_mutex g_ux_mem_mutex;
#endif

#ifdef UX_MEM_DEBUG_SUPPORT
static void ux_hal_memory_lock(void)
{
    if (g_ux_mem_mutex.mutex != NULL) {
        osal_mutex_lock(&g_ux_mem_mutex);
    }
}

static void ux_hal_memory_unlock(void)
{
    if (g_ux_mem_mutex.mutex != NULL) {
        osal_mutex_unlock(&g_ux_mem_mutex);
    }
}
#endif

/**
 * 初始化内存管理器
 */
void ux_hal_memory_init(void)
{
#ifdef UX_MEM_DEBUG_SUPPORT
    uint8_t index = 0;
    int32_t ret = osal_mutex_init(&g_ux_mem_mutex);
    if (ret != 0) {
        g_ux_mem_mutex.mutex = NULL;
    }
    for (index = 0; index < UX_MEM_MAX; index++) {
        g_mem_module_used_cnt[index] = 0;
    }
#endif
    return;
}

/**
 * 去初始化内存管理器
 */
void ux_hal_memory_deinit(void)
{
#ifdef UX_MEM_DEBUG_SUPPORT
    uint8_t index = 0;
    ux_hal_memory_lock();
    for (index = 0; index < UX_MEM_MAX; index++) {
        g_mem_module_used_cnt[index] = 0;
    }
    ux_hal_memory_unlock();
    osal_mutex_destroy(&g_ux_mem_mutex);
    g_ux_mem_mutex.mutex = NULL;
#endif
    return;
}

/**
 * 获取内存
 * @param module 内存模块
 * @param size 需要获取的内存大小
 * @return 返回获取的内存地址
 */
void *ux_hal_memory_acquire(ux_mem_module_t module, size_t size)
{
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    osal_save_caller_ra((void *)__builtin_return_address(0));
#endif
    void *memory = malloc(size);
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
    osal_reset_caller_ra();
#endif
    if (memory == NULL) {
        ux_log_err("[ux][hal][memory]acq failed");
        return NULL;
    }
#ifdef UX_MEM_DEBUG_SUPPORT
    ux_hal_memory_lock();
    if (module < UX_MEM_MAX) {
        g_mem_module_used_cnt[module]++;
        if (g_mem_module_used_cnt[module] >= THRESHOLD_FOR_PRINT_MEM_USED) {
            ux_log_debug("[ux][hal][memory] acq module:[%d] used:[%d]\n", module, g_mem_module_used_cnt[module]);
        }
    } else {
        ux_log_warn("[ux][hal][memory] acq module [%d] out of range.\n", module);
    }
    ux_hal_memory_unlock();
#endif
    return memory;
}

/**
 * 释放内存
 * @param module 内存模块
 * @param memory 需要释放的内存地址
 */
void ux_hal_memory_release(ux_mem_module_t module, void *memory)
{
    if (memory != NULL) {
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
        osal_save_caller_ra((void *)__builtin_return_address(0));
#endif
        free(memory);
#ifdef LOSCFG_MEM_DFX_SHOW_CALLER_RA
        osal_reset_caller_ra();
#endif
    } else {
        return;
    }
#ifdef UX_MEM_DEBUG_SUPPORT
    ux_hal_memory_lock();
    if (module < UX_MEM_MAX) {
        g_mem_module_used_cnt[module]--;
    }
    ux_hal_memory_unlock();
#endif
    return;
}

/**
 * 获取内存使用信息
 * @param used 用于存储内存使用情况的指针
 * @param len 用于存储内存模块数量的指针
 */
void ux_hal_memory_get_used_info(uint16_t **used, uint16_t *len)
{
#ifdef UX_MEM_DEBUG_SUPPORT
    *used = g_mem_module_used_cnt;
    *len = UX_MEM_MAX;
#else
    *used = NULL;
    *len = 0;
#endif

    return;
}
