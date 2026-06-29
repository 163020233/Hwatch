/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio setting uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */
  
#ifndef UX_API_AUDIO_SETTING_H
#define UX_API_AUDIO_SETTING_H

#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#ifndef DEMO_WATCH
#include "ux_api_device.h"
#endif
#include "ux_api_audio_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_API_VOLUME_LEVELS_MAX 25                                    /**< 最大音量等级数 */
#define DEFAULT_UPLINK_VOLUME    12                                    /**< 默认上行音量 */
#define DEFAULT_AUDIO_STREAM_BIT_RATE 64                               /**< 默认音频流比特率 */
#define UX_API_VOLUME_FACTOR 1000                                      /**< 音量因子 */
#define UX_API_VOLUME_CONTROL_NUM 10                                   /**< 音量控制数量 */
#define MAX_VOLUME_DB_VALUE 30                                         /**< 最大音量分贝值 */
#define MIN_VOLUME_DB_VALUE (-2147483648)                              /**< 最小音量分贝值 */
/**
 * @brief 设置音频流控制
 * @param [in] control 音频流控制 @ref ux_api_audio_stream_control_t
 */
void ux_api_audio_set_stream_control(ux_api_audio_stream_control_t control);

/**
 * @brief 获取指定发射器的能量值
 * @param [in] device_id 设备ID
 * @param [out] energy 指向 int32_t 类型变量的指针，用于存储获取到的能量值
 * @return errcode_t 返回操作的状态码，表示调用成功或失败的具体原因
 */
errcode_t ux_api_audio_get_energy_value(uint16_t device_id, int32_t *energy);

/**
 * @brief 设置指定音量控制的音量（以分贝为单位）
 * @param [in] device_id 设备ID
 * @param [in] db 要设置的音量级别（以分贝为单位）
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_volume(uint16_t device_id, int32_t db);

/**
 * @brief 获取指定音量控制的当前音量（以分贝为单位）
 * @param [in] device_id 设备ID
 * @param [out] db 指向用于存储当前音量级别（以分贝为单位）的变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_volume(uint16_t device_id, int32_t *db);

/**
 * @brief 将指定音量控制静音
 * @param [in] device_id 设备ID
 * @param [in] mute 静音状态（true表示静音，false表示非静音)
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_mute_status(uint16_t device_id, bool mute);

/**
 * @brief 检查指定音量控制是否处于静音状态
 * @param [in] device_id 设备ID
 * @param [out] mute 指向用于存储静音状态（true表示静音，false表示非静音）的布尔变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_mute_status(uint16_t device_id, bool *mute);

/**
 * @brief 设置麦克风静音
 * @param [in] is_mute 静音状态（true表示静音，false表示非静音）的布尔变量
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_mic_mute(bool is_mute);

/**
 * @brief 获取麦克风静音状态
 * @param [out] is_mute 指向用于存储麦克风静音状态（true表示静音，false表示非静音）的布尔变量的指针
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_mic_mute(bool* is_mute);

/**
 * @brief 设置全局静音
 * @param [in] mute 静音状态（true表示静音，false表示非静音)
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_master_mute(bool mute);

/**
 * @brief 设置本地音频流音量
 * @param [in] info 音频音量控制结构体指针，包含音量信息和控制参数
 * @return ERRCODE_SUCC 设置成功
 * @return Other 设置失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_stream_volume(ux_api_audio_volume_info_t* info);

/**
 * @brief 获取本地音频流音量
 * @param [out] info 音频音量控制结构体指针，用于返回当前音量信息
 * @return ERRCODE_SUCC 获取成功
 * @return Other 获取失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_stream_volume(ux_api_audio_volume_info_t* info);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 设置对端设备音频流音量
 * @param [in] device_id 对端设备ID
 * @param [in] info 音频音量控制结构体指针，包含音量信息和控制参数
 * @return ERRCODE_SUCC 设置成功
 * @return Other 设置失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_remote_volume(uint16_t device_id, ux_api_audio_volume_info_t* info);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 获取对端设备音频流音量
 * @param [in] device_id 对端设备ID
 * @param [out] info 音频音量控制结构体指针，用于返回当前音量信息
 * @return ERRCODE_SUCC 获取成功
 * @return Other 获取失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_remote_volume(uint16_t device_id, ux_api_audio_volume_info_t* info);

/**
 * @brief 设置本地音频静音
 * @param [in] info 音频静音结构体指针，包含静音信息和控制参数
 * @return ERRCODE_SUCC 设置成功
 * @return Other 设置失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_stream_mute(ux_api_audio_mute_info_t* info);

/**
 * @brief 获取本地音频流静音状态
 * @param [out] info 音频静音结构体指针，包含静音信息和控制参数
 * @return ERRCODE_SUCC 获取成功
 * @return Other 获取失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_stream_mute(ux_api_audio_mute_info_t* info);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 设置对端设备音频流静音状态
 * @param [in] device_id 对端设备ID
 * @param [in] info 音频静音结构体指针，包含静音信息和控制参数
 * @return ERRCODE_SUCC 设置成功
 * @return Other 设置失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_set_remote_mute(uint16_t device_id, ux_api_audio_mute_info_t* info);

/**
 * @defgroup 支持产品 [livemic_rx]
 * @brief 获取对端设备音频流静音状态
 * @param [in] device_id 对端设备ID
 * @param [out] info 音频静音结构体指针，包含静音信息和控制参数
 * @return ERRCODE_SUCC 获取成功
 * @return Other 获取失败，参考 @ref errcode_t
 */
errcode_t ux_api_audio_get_remote_mute(uint16_t device_id, ux_api_audio_mute_info_t* info);

#ifndef DEMO_WATCH
/**
* @brief 获取通道音量值
* @param [in] channel 通道号
* @return 音量值
*/
int32_t ux_api_audio_get_volume_by_channel(ux_api_device_audio_channel_t channel);

/**
* @brief 设置对端设备通道音量值
* @param [in] volume 需要设置的音量值
* @param [in] channel 通道号
* @return 成功返回ERRCODE_SUCC，失败返回其他错误码。
*/
errcode_t ux_api_audio_set_volume_by_channel(ux_api_device_audio_channel_t channel, int32_t volume);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
