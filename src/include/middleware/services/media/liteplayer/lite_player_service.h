/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: lite player wrapper
 * Author: Media Software Group
 * Create: 2024-10-16
 */

#ifndef LITE_PLAYER_SAERVICE_H
#define LITE_PLAYER_SAERVICE_H

#include <stdint.h>
#include "lite_player.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t toneId;            /*!< @if Eng ID of the currently played announcement.
                                  @else   当前播放提示音ID @endif */
    char filePath[PLAYER_SOURCE_PATH_MAX];  /*!< @if Eng Path of the current file to be played.
                                            @else   当前播放文件路径 @endif */
    ToggleConfig notifyToggleConfig;     /*!< @if Eng Synchronization configuration information of the prompt tone.
                                         @else   提示音同步配置信息 @endif */
    AudioStreamType streamType;   /*!< @if Eng Type of the currently played stream.
                                  @else   当前播放的流类型 @endif */
    AudioSession sessionID;       /*!< @if Eng session ID.
                                  @else   会话ID @endif */
    PlaybackPriority priority;   /*!< @if Eng Priority of the current playback source.
                                  @else   当前播放源的优先级 @endif */
    LitePlayerEeventFunc eventFunc;  /*!< @if Eng Callback function for local playback.
                                  @else   本地播放的回调函数 @endif */
    const void *cookie;           /*!< @if Eng Context required by the callback function for local playback.
                                  @else   本地播放的回调函数需要的上下文 @endif */
    bool singleLoop;             /*!< @if Eng Is the current playback source set to loop
                                  @else   是否是单曲循环 @endif */
    bool syncTone;              /*!< @if Eng Is the alert tone synchronized
                                  @else   是否是同步提示音 @endif */
    int32_t volumeMixWeight;    /*!< @if Eng Mix ratio of music and call follow-up prompt tones
                                  @else   音乐和通话的跟当前提示音的混音比例 @endif */
    int32_t maxInterruptCount;  /*!< @if Eng Maximum number of times that the current.
                                  @else   当前提示音最大被打断次数 @endif */
    bool immediateCompleteCB;   /*!< @if Eng Whether to immediately call back the playback completion event after the playback is complete.
                                  @else   是否播放完成立即回调播放完成事件 @endif */
    bool fastSourceSwitching;   /*!< @if Eng Whether to support fast source switching.
                                  @else   是否快速切源 @endif */
    int32_t volume;             /*!< @if Eng the volume of the current playback source.
                                  @else   当前播放源的音量 @endif */
} LitePlayerInfo;

/**
 * @if Eng
 * @brief anable the playing service, create a playing channel, and start playing.
 *
 * @param [out] handle Indicates the playing instance created.
 * @param [in] playInfo Indicates the parameters required for creating a playing service.
 * @return Returns <b>0</b> Indicates that the start playing service is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开启播放服务，创建播放通道，开始播放。
 *
 * @param [out] handle 创建的播放实例。
 * @param [in] playInfo 创建播放业务需要的参数信息。
 * @return 返回 <b>0</b>开始播放成功；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerServiceStart(LitePlayerHandle *handle, const LitePlayerInfo *playInfo);

/**
 * @brief stops the playing service and destroys the entire playing channel and resources.
 *
 * @param [in] handle Indicates the player instance.[in]
 * @return Returns <b>0</b> Indicates that the stop playing service is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止播放服务，销毁整个播放通道和资源。
 *
 * @param [in] handle 播放器实例。【输入】
 * @return 返回<b>0</b>停止播放服务成功；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerServiceStop(const LitePlayerHandle handle);

#ifdef __cplusplus
}
#endif

#endif
