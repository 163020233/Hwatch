/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provide ux audio common uapi \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */
 
#ifndef UX_API_AUDIO_COMMON_H
#define UX_API_AUDIO_COMMON_H
 
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 音频流类型枚举
 */
typedef enum {
    UX_API_AUDIO_STREAM_TYPE_START,                     /**< 流类型起始值 */
    UX_API_AUDIO_STREAM_TYPE_MUSIC_A2DP_SINK = UX_API_AUDIO_STREAM_TYPE_START,   /**< 蓝牙音乐A2DP SINK端 */
    UX_API_AUDIO_STREAM_TYPE_MUSIC_A2DP_SOURCE,         /**< 蓝牙音乐A2DP SOURCE端 */
    UX_API_AUDIO_STREAM_TYPE_MUSIC_NEARLINK,            /**< 星闪音乐 */
    UX_API_AUDIO_STREAM_TYPE_CALL_HFP_SCO,              /**< 蓝牙通话HFP SCO端 */
    UX_API_AUDIO_STREAM_TYPE_CALL_NEARLINK,             /**< 星闪通话 */
    UX_API_AUDIO_STREAM_TYPE_LIVE_MIC,                  /**< 直播麦 */
    UX_API_AUDIO_STREAM_TYPE_VOICE_RECORD,              /**< 语音采集录制 */
    UX_API_AUDIO_STREAM_TYPE_MUSIC_LOCAL,               /**< 本地音乐 */
    UX_API_AUDIO_STREAM_TYPE_TONE,                      /**< 提示音 */
    UX_API_AUDIO_STREAM_TYPE_RING,                      /**< 带外铃声 */
    UX_API_AUDIO_STREAM_TYPE_UAC_UP,                    /**< UAC上行 */
    UX_API_AUDIO_STREAM_TYPE_UAC_DOWN,                  /**< UAC下行 */
    UX_API_AUDIO_STREAM_TYPE_GAME_UP,                   /**< dongle上行 */
    UX_API_AUDIO_STREAM_TYPE_GAME_DOWN,                 /**< dongle下行 */
    UX_API_AUDIO_STREAM_TYPE_MUSIC_LINEIN,              /**< 3.5mm音频输入 */
    UX_API_AUDIO_STREAM_TYPE_END,                       /**< 流类型结束值 */
    UX_API_AUDIO_STREAM_TYPE_INVALID                    /**< 无效流类型 */
} ux_api_audio_stream_type_t;

/**
 * @brief 音频流方向枚举
 */
typedef enum {
    UX_API_AUDIO_STREAM_DIRECTION_DOWN,    /**< 下行流 */
    UX_API_AUDIO_STREAM_DIRECTION_UP,      /**< 上行流 */
    UX_API_AUDIO_STREAM_DIRECTION_DOWN_UP, /**< 上下行流 */
    UX_API_AUDIO_STREAM_DIRECTION_BUTT
} ux_api_audio_stream_direction_t;

/**
 * @brief 音频流控制结构体
 */
typedef struct {
    ux_api_audio_stream_type_t      stream_type;           /**< 音频类型，必选 */
    ux_api_audio_stream_direction_t volume_direction;      /**< 音量调整方向，必选 */
} ux_api_audio_stream_control_t;

/**
 * @brief 音频音量信息结构体
 */
typedef struct {
    ux_api_audio_stream_control_t stream_control; /**< 音频流控制 */
    int32_t volume;                               /**< 音量值 */
} ux_api_audio_volume_info_t;

/**
 * @brief 音频静音信息结构体
 */
typedef struct {
    ux_api_audio_stream_control_t stream_control; /**< 音频流控制 */
    bool mute;                                    /**< 静音状态 */
} ux_api_audio_mute_info_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
