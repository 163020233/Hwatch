/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux memory manager header.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_HAL_MEMORY_H
#define UX_HAL_MEMORY_H

#define UX_MEM_DEBUG_SUPPORT

#include "stddef.h"
#include "stdint.h"
#include "errcode.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    UX_MEM_DEFAULT = 0,
    UX_MEM_MAIL = 1,
    UX_MEM_BOX,
    UX_MEM_AUDIO,
    UX_MEM_USB,
    UX_MEM_PROTOCOL,
    UX_MEM_BT,
    UX_MEM_SLE,
    UX_MEM_SPP,
    UX_MEM_BLE,
    UX_MEM_BG,
    UX_MEM_COMMU,
    UX_MEM_PAIR,
    UX_MEM_CMD_BT,
    UX_MEM_DEVICE_MGR,
    UX_MEM_CMD_SYS,
    UX_MEM_AT,
    UX_MEM_SYNC_COMMU,
    UX_MEM_CMD_EVENT,
    UX_MEM_LED,
    UX_MEM_ANC,
    UX_MEM_HAID,
    UX_MEM_TOOLS,
    UX_MEM_BROADCAST,
    UX_MEM_MAX
} ux_mem_module_t;

/**
 * 初始化内存管理器
 */
void ux_hal_memory_init(void);

/**
 * 去初始化内存管理器
 */
void ux_hal_memory_deinit(void);

/**
 * 获取内存
 * @param module 内存模块
 * @param size 需要获取的内存大小
 * @return 返回获取的内存地址
 */
void *ux_hal_memory_acquire(ux_mem_module_t module, size_t size);

/**
 * 释放内存
 * @param module 内存模块
 * @param memory 需要释放的内存地址
 */
void ux_hal_memory_release(ux_mem_module_t module, void *memory);

/**
 * 获取内存使用信息
 * @param used 用于存储内存使用情况的指针
 * @param len 用于存储内存模块数量的指针
 */
void ux_hal_memory_get_used_info(uint16_t **used, uint16_t *len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
