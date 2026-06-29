/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_system_config.h
 * Description:  provide ux system config header file.
 */
#ifndef UX_SYSTEM_CONFIG_H
#define UX_SYSTEM_CONFIG_H

#include "errcode.h"
#include "stddef.h"
#include "ux_errcode.h"
#include "ux_api_system_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    char* sdk_version;
    uint8_t sdk_version_len;
} ux_system_settings_table_t;

typedef enum {
    UX_SYSTEM_LANGUAGE_START = 0,
    UX_SYSTEM_ENGLISH = UX_SYSTEM_LANGUAGE_START,
    UX_SYSTEM_CHINESE,
    UX_SYSTEM_CHINESE_TW,
    UX_SYSTEM_JAPANESE,
    UX_SYSTEM_KOREAN,
    UX_SYSTEM_FRENCH,
    UX_SYSTEM_LANGUAGE_END,
} ux_system_language_t;

typedef enum {
    UX_SYS_MAX_CONNECT_NUM_1 = 1,
    UX_SYS_MAX_CONNECT_NUM_2 = 2,
    UX_SYS_MAX_CONNECT_NUM_4 = 4,
    UX_SYS_MAX_CONNECT_NUM_8 = 8,

    UX_SYS_MAX_CONNECT_NUM_MAX,
} ux_sys_max_connect_num_t;

/**
 * @brief 上下文管理，用于获取当前正在工作的transmit mode
 * @return 返回transmit mode
 */
ux_api_sys_transmit_mode_t ux_system_cfg_get_transmit_mode(void);

/**
 * @brief 上下文管理，用于设置当前正在工作的transmit mode到上下文
 * @return 返回void
 */
void ux_system_cfg_set_transmit_mode(ux_api_sys_transmit_mode_t mode);

/**
 * @brief 上下文管理，用于获取当前状态是否正在切换transmit mode
 * @return 返回状态
 */
bool ux_sys_cfg_get_transmit_mode_change_flag(void);

/**
 * @brief 上下文管理，用于设置当前状态是否正在切换transmit mode
 * @return 返回void
 */
void ux_sys_cfg_set_transmit_mode_change_flag(bool flag);

/**
 * @brief 设置SDK版本号
 * @param device_id 设备ID
 * @param sdk_version SDK版本号
 * @param data_len 版本号长度
 * @return 返回错误码，0表示成功
 */
errcode_t ux_system_cfg_set_sdk_version(uint16_t device_id, char* sdk_version, uint16_t data_len);

/**
 * @brief 获取SDK版本号
 * @param device_id 设备ID
 * @param sdk_version 用于存储SDK版本号的缓冲区
 * @param data_len 缓冲区长度
 * @return 返回错误码，0表示成功
 */
errcode_t ux_system_cfg_get_sdk_version(uint16_t device_id, char* sdk_version, uint16_t data_len);

/**
 * @brief 用于每次上电时读取transmit mode至上下文
 * @return 返回错误码，0表示成功
 */
errcode_t ux_system_cfg_restore_transmit_mode(void);

/**
 * @brief 用于直播麦TX响应RX的transmit mode切换请求，协同切换
 * @param transmit_mode 工作模式
 * @return 返回错误码，0表示成功
 */
errcode_t ux_sys_livemic_set_transmit_mode(ux_api_sys_transmit_mode_t transmit_mode);

/**
 * @brief 用于查询最大支持的可同时连接的对端设备数量
 * @return 返回uint8_t，代表数量
 */
uint8_t ux_sys_get_max_connect_num(void);

/**
 * @brief 用于设置系统时间
 * @param calendar 系统时间
 * @return 返回错误码，0表示成功
 */
errcode_t ux_sys_set_time(calendar_t calendar);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
