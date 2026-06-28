/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: Provide ux power manager service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-12， Create file. \n
 */
#ifndef UX_API_SYSMGR_H
#define UX_API_SYSMGR_H
 
#include <stdint.h>
#include "errcode.h"
 
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 配对状态枚举
 */
typedef enum {
    UX_API_SYSMGR_PAIR_DISCONNECTED = 0, /**< 未配对 */
    UX_API_SYSMGR_PAIR_CONNECTED,         /**< 已配对 */
   
    UX_API_SYSMGR_PAIR_MAX
} ux_api_sysmgr_pair_state_t;

/**
 * @brief 设备角色枚举
 */
typedef enum {
    UX_API_SYSMGR_ROLE_MASTER = 0, /**< 主设备 */
    UX_API_SYSMGR_ROLE_SLAVE,      /**< 从设备 */
    UX_API_SYSMGR_ROLE_MAX,
} ux_api_sysmgr_role_t;

/**
 * @brief 工作状态枚举
 */
typedef enum {
    UX_API_SYSMGR_WORKING_STATE_OFF_WORKING = 0,      /**< 非工作状态 */
    UX_API_SYSMGR_WORKING_STATE_ON_WORKING,           /**< 工作状态 */
    UX_API_SYSMGR_WORKING_STATE_OFF_WORKING_DELAY,    /**< 延迟非工作状态 */
    UX_API_SYSMGR_WORKING_STATE_ON_WORKING_DELAY,     /**< 延迟工作状态 */
    UX_API_SYSMGR_WORKING_STATE_ON_DUT,               /**< 工厂模式 */
    UX_API_SYSMGR_WORKING_STATE_ON_OTA,               /**< 升级模式 */
    UX_API_SYSMGR_WORKING_STATE_ON_RECOVER,           /**< 恢复模式 */
    UX_API_SYSMGR_WORKING_STATE_ON_RESET,             /**< 重置模式 */
    UX_API_SYSMGR_WORKING_STATE_ACC_OFF,              /**< ACC关闭 */

    UX_API_SYSMGR_WORKING_STATE_MAX
} ux_api_sysmgr_working_state_t;

/**
 * @brief 系统状态枚举
 */
typedef enum {
    UX_API_SYSMGR_SYS_STATE_POWEROFF = 1, /**< 整机在上电后，尚未进入业务状态时的状态 */
    UX_API_SYSMGR_SYS_STATE_SOUND,        /**< 整机进入音频模式 */
    UX_API_SYSMGR_SYS_STATE_HAID,         /**< 整机进入助听模式 */
    UX_API_SYSMGR_SYS_STATE_RECOVER,      /**< 整机进入恢厂模式 */
    UX_API_SYSMGR_SYS_STATE_OTA,          /**< 整机进入升级模式 */
    UX_API_SYSMGR_SYS_STATE_DUT,          /**< 整机进入工厂模式 */
    UX_API_SYSMGR_SYS_STATE_MAX
} ux_api_sysmgr_sys_state_t;

/**
 * @brief 通话状态枚举
 */
typedef enum {
    UX_API_SYSMGR_CALL_STATE_IDLE = 0,        /**< 空闲状态 */
    UX_API_SYSMGR_CALL_STATE_INCOMING,        /**< 来电状态 */
    UX_API_SYSMGR_CALL_STATE_DIALING,         /**< 拨号状态 */
    UX_API_SYSMGR_CALL_STATE_ANSWER,          /**< 接通状态 */
    UX_API_SYSMGR_CALL_STATE_RINGING,         /**< 响铃状态 */
    UX_API_SYSMGR_CALL_STATE_PLAY_MUSIC,      /**< 播放音乐状态 */
    UX_API_SYSMGR_CALL_STATE_STOP_MUSIC = 6,  /**< 停止音乐状态 */
    UX_API_SYSMGR_CALL_STATE_PAUSE_MUSIC,     /**< 暂停音乐状态 */
    UX_API_SYSMGR_CALL_STATE_PLAY_MUSIC_MUTE, /**< 静音播放音乐状态 */

    UX_API_SYSMGR_CALL_STATE_MAX
} ux_api_sysmgr_call_state_t;

/**
 * @brief 获取对端连接状态
 * @return 返回连接状态，参考ux_api_sysmgr_pair_state_t
 */
ux_api_sysmgr_pair_state_t ux_api_sysmgr_get_peer_pair_state(void);
 
/**
 * @brief 获取tws耳机主副耳角色
 * @return 返回主副耳角色，参考ux_api_sysmgr_role_t
 */
ux_api_sysmgr_role_t ux_api_sysmgr_get_device_role(void);

/**
 * @brief 获取目标设备的主副耳角色
 * @param [in] device_id 设备ID
 * @return 返回主副耳角色，参考ux_api_sysmgr_role_t
 */
ux_api_sysmgr_role_t ux_api_sysmgr_get_target_device_role(uint16_t device_id);

/**
 * @brief 设置设备主副耳角色
 * @param [in] role 主副耳角色，参考 @ref ux_api_sysmgr_role_t
 * @return ERRCODE_SUCC 成功；Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sysmgr_set_device_role(ux_api_sysmgr_role_t role);

/**
 * @brief 设置系统管理器工作状态, 此函数用于设置系统管理器的当前工作状态。
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sysmgr_set_working_state(ux_api_sysmgr_working_state_t state);

/**
 * @brief 获取系统管理器工作状态, 此函数用于获取系统管理器的当前工作状态。
 * @return ux_api_sysmgr_working_state_t 返回系统管理器的工作状态
 */
ux_api_sysmgr_working_state_t ux_api_sysmgr_get_working_state(void);

/**
 * @brief 检查系统管理器是否处于非工作状态, 此函数用于检查系统管理器是否处于非工作状态（例如关闭或禁用状态）。
 * @return bool 返回一个布尔值，表示系统管理器是否处于非工作状态：
 *              - true: 系统管理器处于非工作状态
 *              - false: 系统管理器处于正常工作状态
 */
bool ux_api_sysmgr_is_off_working_state(void);

/**
 * @brief 设置系统状态
 * @param state 系统状态值
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_sysmgr_set_system_state(ux_api_sysmgr_sys_state_t state);

/**
 * @brief 获取系统状态
 * @return 当前系统状态值
 */
ux_api_sysmgr_sys_state_t ux_api_sysmgr_get_system_state(void);

/**
 * @brief 获取通话状态
 * @return ux_api_sysmgr_call_state_t 当前通话状态
 */
ux_api_sysmgr_call_state_t ux_api_sysmgr_get_call_state(void);

/**
 * @brief 获取蓝牙名称最大长度
 * @return uint8_t 蓝牙名称最大长度
 */
uint8_t ux_api_sysmgr_get_ble_name_max_len(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif