/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux bt media control api \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */

#ifndef UX_API_BT_AUDIO_H
#define UX_API_BT_AUDIO_H
#include "errcode.h"
#include "ux_api_bt.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 蓝牙音频流状态枚举
 */
typedef enum {
    UX_API_BT_STREAM_STATE_STARTING = 0,     /**< 音频流正在启动 */
    UX_API_BT_STREAM_STATE_STREAMING,        /**< 音频流正在传输 */
    UX_API_BT_STREAM_STATE_STOPPING,         /**< 音频流正在停止 */
    UX_API_BT_STREAM_STATE_STOPPED,          /**< 音频流已停止 */
    UX_API_BT_STREAM_STATE_MAX,              /**< 最大值 */
} ux_api_bt_stream_state_t;

/**
 * @brief 蓝牙音频链路信息结构体
 */
typedef struct {
    ux_api_bt_profile_state_t profile_state; /**< 链路状态 */
    ux_api_bt_stream_state_t stream_state;   /**< 流状态 */
    uint32_t res[8];                           /**< 预留字段 */
} ux_api_bt_audio_info_t;

/**
 * @brief 主动启动蓝牙音频流传输
 * @param [in] profile 蓝牙音频服务类型, 目前有a2dp和sco
 * @param [in] addr 设备地址，小端序
 * @param [in] len 设备地址长度,为6
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_audio_start(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);

/**
 * @brief 主动停止音频流传输
 * @param [in] profile 蓝牙音频服务类型, 目前有a2dp和sco
 * @param [in] addr 设备地址，小端序
 * @param [in] len 设备地址长度,为6
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bt_audio_stop(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);

/**
 * @brief 按地址获取获取蓝牙音频信息
 * @param [in] profile 服务类型
 * @param [in] addr 设备地址，小端序
 * @return ux_api_bt_audio_info_t* 指向音频链路信息的结构体指针
 */
const ux_api_bt_audio_info_t* ux_api_bt_audio_get_info(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */

#endif