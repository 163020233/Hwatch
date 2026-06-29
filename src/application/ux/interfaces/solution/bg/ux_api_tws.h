/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux tws api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-23， Create file. \n
 */

#ifndef UX_API_TWS_H
#define UX_API_TWS_H

#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BS_CONNECT_NUM_MAX   2              /* 双链接最大连接个数 */

/**
 * @brief 佩戴检测开关枚举
 */
typedef enum {
    UX_API_TWS_WEAR_DETECTION_OFF = 0,      /**< 智能佩戴检测关 */
    UX_API_TWS_WEAR_DETECTION_ON,           /**< 智能佩戴检测开 */
    UX_API_TWS_WEAR_DETECTION_BUTT,         /**< 最大值 */
} ux_api_tws_wear_detection_switch_t;

/**
 * @brief 双设备连接开关枚举
 */
typedef enum {
    UX_API_TWS_DUAL_CONNECTION_OFF = 0,     /**< 双设备连接关 */
    UX_API_TWS_DUAL_CONNECTION_ON,          /**< 双设备连接开 */
    UX_API_TWS_DUAL_CONNECTION_BUTT,        /**< 最大值 */
} ux_api_tws_dual_connection_switch_t;

/**
 * @brief 音频低时延开关枚举
 */
typedef enum {
    UX_API_TWS_LOW_LATENCY_OFF = 0,         /**< 音频低时延关 */
    UX_API_TWS_LOW_LATENCY_ON,              /**< 音频低时延开 */
    UX_API_TWS_LOW_LATENCY_BUTT,            /**< 最大值 */
} ux_api_tws_low_latency_switch_t;

/**
 * @brief 佩戴耳侧枚举
 */
typedef enum {
    UX_API_TWS_WEAR_EAR_LEFT = 0,               /**< 佩戴左耳 */
    UX_API_TWS_WEAR_EAR_RIGHT,                  /**< 佩戴右耳 */
    UX_API_TWS_WEAR_EAR_MAX,                    /**< 最大值 */
} ux_api_tws_wear_side_t;

/**
 * @brief 左右耳侧枚举
 */
typedef enum {
    UX_API_TWS_EAR_SIDE_LEFT = 1,               /**< 左耳 */
    UX_API_TWS_EAR_SIDE_RIGHT,                  /**< 右耳 */
    UX_API_TWS_EAR_SIDE_MAX,                    /**< 最大值 */
} ux_api_tws_ear_side_t;

/**
 * 佩戴状态枚举
 * 定义了TWS耳机的佩戴状态
 */
typedef enum {
    UX_API_TWS_WEAR_STATE_INVALID = 1,  /**< 无效状态 */
    UX_API_TWS_WEAR_STATE_IN,           /**< 入耳状态 */
    UX_API_TWS_WEAR_STATE_OUT,          /**< 出耳状态 */

    UX_API_TWS_WEAR_STATE_MAX,          /**< 最大值 */
} ux_api_tws_wear_state_t;

/**
 * 佩戴模式枚举
 * 定义了TWS耳机的佩戴模式
 */
typedef enum {
    UX_API_TWS_WEAR_MODE_IN = 1,     /**< 强制设置耳机持续在入耳状态 */
    UX_API_TWS_WEAR_MODE_OUT,        /**< 强制设置耳机持续不在入耳状态 */
    UX_API_TWS_WEAR_MODE_AUTO,       /**< 由设备自身的Sensor和算法检测入耳状态 */

    UX_API_TWS_WEAR_MODE_MAX            /**< 最大值 */
} ux_api_tws_wear_mode_t;

/**
 * 佩戴动作枚举
 * 定义了TWS耳机的佩戴动作
 */
typedef enum {
    UX_API_TWS_WEAR_ACTION_ON_EAR,     /**< 耳机正处于被佩戴的状态 */
    UX_API_TWS_WEAR_ACTION_OFF_EAR,    /**< 耳机被摘下 */

    UX_API_TWS_WEAR_ACTION_MAX          /**< 最大值 */
} ux_api_tws_wear_action_t;

/**
 * TWS主副切切换状态枚举类型
 * 定义了TWS（True Wireless Stereo）主副切换的各种状态
 */
typedef enum {
    UX_API_TWS_HANDOVER_STATE_NONE,           /**< 无切换状态 */
    UX_API_TWS_HANDOVER_STATE_STARTING,       /**< 切换进行中状态 */
    UX_API_TWS_HANDOVER_STATE_SUCCESS,        /**< 切换成功状态 */
    UX_API_TWS_HANDOVER_STATE_FAILED,         /**< 切换失败状态 */
    UX_API_TWS_HANDOVER_STATE_MAX             /**< 状态数量上限，用于数组边界检查 */
} ux_api_tws_handover_state_t;

/**
 * @brief 主副切
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_try_handover(void);

/**
 * @brief 获取TWS主副切换状态
 * @return ux_api_tws_handover_state_t 当前的TWS切换状态
 */
ux_api_tws_handover_state_t ux_api_tws_get_handover_state(void);

/**
 * @brief 设置 TWS 耳机的佩戴检测功能开关。
 * @param[in] new_switch 佩戴检测开关状态：
 *                      - 0：关闭佩戴检测
 *                      - 1：开启佩戴检测
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_wear_detection_switch(ux_api_tws_wear_detection_switch_t new_switch);

/**
 * @brief 获取 TWS 耳机当前的佩戴检测功能开关状态。
 * @param[out] out_switch 指向存储当前开关状态的变量：
 *                       - 0：佩戴检测已关闭
 *                       - 1：佩戴检测已开启
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_get_wear_detection_switch(uint8_t *out_switch);

/**
 * @brief 获取 TWS 耳机当前的音频低时延开关状态。
 * @param[out] out_switch 指向存储当前开关状态的变量：
 *                       - 0：音频低时延已关闭
 *                       - 1：音频低时延已开启
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_get_low_latency_switch(uint8_t *out_switch);

/**
 * @brief 设置 TWS 耳机的音频低时开关。
 * @param[in] new_switch 音频低时延开关状态：
 *                      - 0：关闭音频低时延
 *                      - 1：开启音频低时延
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_low_latency_switch(ux_api_tws_low_latency_switch_t new_switch);

/**
 * @brief 设置 TWS 耳机的双设备连接功能开关。
 * @param[in] new_switch 双设备连接开关状态：
 *                      - 0：关闭双设备连接
 *                      - 1：开启双设备连接
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_dual_connection_switch(ux_api_tws_dual_connection_switch_t new_switch);

/**
 * @brief 获取 TWS 耳机当前的双设备连接功能开关状态。
 * @param[out] out_switch 指向存储当前开关状态的变量：
 *                       - 0：双设备连接已关闭
 *                       - 1：双设备连接已开启
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_get_dual_connection_switch(uint8_t *out_switch);

/**
 * @Deprecated
 * @brief 更新 TWS 耳机佩戴位置。
 * @param[in] wear_side 佩戴位置
 *                       - UX_API_TWS_WEAR_EAR_LEFT：佩戴左耳
 *                       - UX_API_TWS_WEAR_EAR_RIGHT：佩戴右耳
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_wear_side(ux_api_tws_wear_side_t wear_side);

/**
 * @brief 设置耳机左右耳信息。
 * @param[in] ear_side 左右信息
 *                       - UX_API_TWS_EAR_SIDE_LEFT: 左耳
 *                       - UX_API_TWS_EAR_SIDE_RIGHT:右耳
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_ear_side(ux_api_tws_ear_side_t ear_side);

/**
 * @brief 获取耳侧信息, 该函数用于获取设备的耳侧信息，通常用于判断设备佩戴在左耳还是右耳
 * @return ux_api_tws_ear_side_t 耳侧信息枚举值，表示设备佩戴在左耳或右耳
 */
ux_api_tws_ear_side_t ux_api_tws_get_ear_side(void);

/**
 * @brief 获取TWS耳机佩戴模式
 * @return ux_api_tws_wear_mode_t 佩戴模式
 */
ux_api_tws_wear_mode_t ux_api_tws_get_wear_mode(void);

/**
 * @brief 设置TWS耳机佩戴模式
 * @param [in] mode 佩戴模式
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_wear_mode(ux_api_tws_wear_mode_t mode);

/**
 * @brief 设置TWS耳机佩戴动作
 * @param [in] action 佩戴动作
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_tws_set_wear_action(ux_api_tws_wear_action_t action);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
