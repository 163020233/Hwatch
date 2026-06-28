/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux bs audio-related functions, such as state, etc. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_API_BS_AUDIO_H
#define UX_API_BS_AUDIO_H

#include "stdint.h"
#include "errcode.h"
#include "ux_common_define.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 音乐状态枚举类型
 */
typedef enum {
    UX_API_BS_MUSIC_STATE_NONE,         /**< 无状态 */
    UX_API_BS_MUSIC_STATE_PAUSED,       /**< 暂停状态 */
    UX_API_BS_MUSIC_STATE_PLAYING,      /**< 播放状态 */
    UX_API_BS_MUSIC_STATE_STOPPED,      /**< 停止状态 */
    UX_API_BS_MUSIC_STATE_PLAY,         /**< 播放命令 */
    UX_API_BS_MUSIC_STATE_MAX           /**< 状态数量上限 */
} ux_api_bs_music_state_t;

/**
 * @brief 通话状态枚举类型
 */
typedef enum {
    UX_API_BS_CALL_STATE_CONNECT_REQUEST,   /**< 连接请求状态 */
    UX_API_BS_CALL_STATE_CONNECTED,         /**< 已连接状态 */
    UX_API_BS_CALL_STATE_DISCONNECTED,      /**< 已断开状态 */
    UX_API_BS_CALL_STATE_UNKNOWN,           /**< 未知状态 */
    UX_API_BS_CALL_STATE_END,               /**< 通话结束状态 */
    UX_API_BS_CALL_STATE_MAX                /**< 状态数量上限 */
} ux_api_bs_call_state_t;


/**
 * @brief 获取当前音乐状态
 * @return ux_api_bs_music_state_t 当前音乐状态值，参考 @ref ux_api_bs_music_state_t
 */
ux_api_bs_music_state_t ux_api_bs_get_music_state(void);

/**
 * @brief 获取当前通话状态
 * @return ux_api_bs_call_state_t 当前通话状态值，参考 @ref ux_api_bs_call_state_t
 */
ux_api_bs_call_state_t ux_api_bs_get_call_state(void);

/**
 * @brief 开始蓝牙类型音频流
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_music_stream_start(void);

/**
 * @brief 停止蓝牙类型音频流
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_music_stream_stop(void);

/**
 * @brief 开始通话类型音频流
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_call_stream_start(void);

/**
 * @brief 停止通话类型音频流
 * @return ERRCODE_SUCC 成功
 * @return Other 失败，参考 @ref errcode_t
 */
errcode_t ux_api_bs_call_stream_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
