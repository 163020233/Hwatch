/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux led uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */


#ifndef UX_API_BOX_H
#define UX_API_BOX_H

#include "stdint.h"
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @brief 设备位置枚举
 */
typedef enum {
    UX_API_BOX_DEVICE_POSITION_IN = 0,      /**< 设备在盒子内 */
    UX_API_BOX_DEVICE_POSITION_OUT,         /**< 设备在盒子外 */

    UX_API_BOX_DEVICE_POSITION_MAX          /**< 最大值 */
} ux_api_box_device_position_t;

/**
 * @brief 盒盖状态枚举
 */
typedef enum {
    UX_API_BOX_COVER_STATUS_CLOSE = 0,      /**< 盒盖关闭 */
    UX_API_BOX_COVER_STATUS_OPEN,           /**< 盒盖打开 */

    UX_API_BOX_COVER_STATUS_MAX             /**< 最大值 */
} ux_api_box_cover_state_t;

/**
 * @brief 盒盖动作枚举
 */
typedef enum {
    UX_API_BOX_COVER_ACTION_CLOSE = 0,     /**< 关闭盒盖 */
    UX_API_BOX_COVER_ACTION_OPEN,          /**< 打开盒盖 */
    UX_API_BOX_COVER_ACTION_MAX            /**< 盒盖动作最大值 */
} ux_api_box_cover_action_t;

/**
 * @brief ux_box模块设置设备相对盒子的位置
 * @param [in] position 设备相对盒子的位置，分为设备在盒子里、设备在盒子外面、无效位置
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_box_set_device_position(ux_api_box_device_position_t position);

/**
 * @brief ux_box模块获取设备相对盒子的位置
 * @param [out] position 设备相对盒子的位置，分为设备在盒子里、设备在盒子外面、无效位置
 * @return errcode_t 是否获取设备相对盒子的位置成功;ERRCODE_SUCC: 成功;其他: 失败
 */
errcode_t ux_api_box_get_device_position(ux_api_box_device_position_t *position);

/**
 * @brief ux_box模块设置盒盖状态
 * @param [in] state 盒盖状态，分为开盖、关盖、无效状态
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_box_set_cover_status(ux_api_box_cover_state_t state);

/**
 * @brief ux_box模块获取盒盖状态
 * @param [out] state 盒盖状态，分为打开、关闭、无效状态
 * @return errcode_t 是否获取盒盖状态成功;ERRCODE_SUCC: 成功;其他: 失败
 */
errcode_t ux_api_box_get_cover_status(ux_api_box_cover_state_t *state);

/**
 * @brief 获取盒子电池电量百分比
 * @return uint8_t 电池电量百分比值
 */
uint8_t ux_api_box_get_battery_percent(void);

/**
 * @brief 触发盒盖动作
 * @param [in] action 盒盖动作类型, @ref ux_api_box_cover_action_t
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_box_trigger_cover_action(ux_api_box_cover_action_t action);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
