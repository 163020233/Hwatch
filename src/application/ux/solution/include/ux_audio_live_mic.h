/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_live_mic.h
 * Description:  provide ux audio live mic header file.
 */

#ifndef UX_AUDIO_LIVE_MIC_H
#define UX_AUDIO_LIVE_MIC_H
#include "audio.h"
#include "ux_api_audio_common.h"
#ifdef __cplusplus
extern "C" {
#endif

void ux_audio_live_mic_init(void);
void ux_audio_live_mic_deinit(void);
bool ux_audio_live_mic_uplink_state(void);
bool ux_audio_live_mic_downlink_state(void);

/**
 * @brief 获取指定连接号和指定方向的音频流的启停状态
 * @param [in] direction 音频通路的方向
 * @param [in] conn_id 连接号
 * @retval bool true: 当前音频通路已启动 false：未找到音频通路或当前音频通路未启动
 */
bool ux_audio_live_mic_get_path_status(ux_api_audio_stream_direction_t direction, uint16_t conn_id);

/**
 * @brief 创建直播麦上行音频通路
 * @param [in] device_id 设备ID
 * @param [in] conn_id 连接号
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_live_mic_uplink_start(uint16_t device_id, uint16_t conn_id);

/**
 * @brief 销毁直播麦上行音频通路
 * @param [in] device_id 设备ID
 * @param [in] conn_id 连接号
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_live_mic_uplink_stop(uint16_t device_id, uint16_t conn_id);

/**
 * @brief 创建直播麦下行音频通路
 * @param [in] device_id 设备ID
 * @param [in] conn_id 连接号
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_live_mic_downlink_start(uint16_t device_id, uint16_t conn_id);

/**
 * @brief 销毁直播麦下行音频通路
 * @param [in] device_id 设备ID
 * @param [in] conn_id 连接号
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_live_mic_downlink_stop(uint16_t device_id, uint16_t conn_id);

/**
 * @brief 销毁直播麦所有音频通路
 * @retval ERRCODE_SUCC 成功
 * @retval Other 失败，参考 @ref errcode_t
 */
errcode_t ux_audio_live_mic_stop_all(void);

#ifdef __cplusplus
}
#endif
#endif
