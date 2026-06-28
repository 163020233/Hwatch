/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: lite player
 * Author: Media Software Group
 * Create: 2024-09-15
 */

#ifndef LITE_PLAYER_H
#define LITE_PLAYER_H

#include <stdint.h>
#include "audio_base_type.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif
#define PLAYER_SOURCE_PATH_MAX 260

typedef void* LitePlayerHandle;

typedef enum {
    LITE_PLAY_STATUS_ERR = 0, /*!< @if Eng error status.
                                  @else   错误状态 @endif */
    LITE_PLAY_STATUS_IDLE = 1 << 0, /*!< @if Eng idle status.
                                        @else   就绪状态 @endif */
    LITE_PLAY_STATUS_INIT = 1 << 1, /*!< @if Eng init status.
                                        @else   初始化状态 @endif */
    LITE_PLAY_STATUS_PREPARING = 1 << 2,/*!< @if Eng Prepareing status.
                                        @else   准备中状态 @endif */
    LITE_PLAY_STATUS_PREPARED = 1 << 3, /*!< @if Eng Prepared status.
                                        @else   准备状态 @endif */
    LITE_PLAY_STATUS_PLAY = 1 << 4, /*!< @if Eng play start status.
                                        @else   播放开始状态 @endif */
    LITE_PLAY_STATUS_PAUSE = 1 << 5, /*!< @if Eng play pause status.
                                        @else   播放暂停状态 @endif */
    LITE_PLAY_STATUS_STOPPED = 1 << 6, /*!< @if Eng play stop status.
                                        @else   播放停止状态 @endif */
    LITE_PLAY_STATUS_COMPLETE = 1 << 7,  /*!< @if Eng play complete status.
                                        @else   播放完成状态 @endif */
    LITE_PLAY_STATUS_EOS = 1 << 8,      /*!< @if Eng play complete status.
                                        @else   播放结束等待退出状态 @endif */
} LitePlayerStates;

typedef enum {
    LITE_PLAY_EVENT_EOF = 0, /*!< @if Eng playback end event.
                                  @else   播放结束事件 @endif */
    LITE_PLAY_EVENT_COMPLETE, /*!< @if Eng playback complete event.
                                  @else   当前文件播放完成事件 @endif */
    LITE_PLAY_EVENT_ERROR,  /*!< @if Eng playback error event.
                                  @else   播放错误事件 @endif */
    LITE_PLAY_EVENT_START,  /*!< @if Eng playback start event.
                                  @else   播放开始事件 @endif */
    LITE_PLAY_EVENT_VOLUME_CHANGE,  /*!< @if Eng volume changed event.
                                  @else   音量变化事件 @endif */
    LITE_PLAY_EVENT_UPDATE_TOGGLE,  /*!< @if Eng Update the toggle parameter.
                                  @else   更新toggle参数 @endif */
    LITE_PLAY_EVENT_BUTT
} LitePlayerEvent;

typedef enum {
    LITE_PLAY_PRIORITY_INVALID = -1,
    LITE_PLAY_PRIORITY_0,
    LITE_PLAY_PRIORITY_1,
    LITE_PLAY_PRIORITY_2,
    LITE_PLAY_PRIORITY_3,
    LITE_PLAY_PRIORITY_4,
    LITE_PLAY_PRIORITY_5,
    LITE_PLAY_PRIORITY_MAX,
} PlaybackPriority;

typedef struct {
    int64_t fileSize;
    int64_t duration;
    float frameRate;
    int32_t bitrate;
    uint32_t audioChannelCnt;
    uint32_t sampleRate;
    uint16_t sampleFmt;
    AudioCodecFormat audioFormat;
    char *album;
    char *title;
    char *artist;
    char *composer;
    char *author;
} LitePlayerFileInfo;

typedef struct {
    char filePath[PLAYER_SOURCE_PATH_MAX];  /*!< @if Eng Path of the current file to be played.
                                  @else   当前播放文件路径 @endif */
    int64_t duration;       /*!< @if Eng Total duration of playing files. The default value is - 1.
                                  @else   播放文件总时长，默认值是-1 @endif */
    bool isSingleLoop;      /*!< @if Eng Indicates whether the song is cyclic..
                                  @else   是否是单曲循环 @endif */
    uint64_t playStartTime;  /*!< @if Eng Playback start time.Unit: us
                                  @else   起播的耗时，单位：ms @endif */
    uint64_t playStopTime;    /*!< @if Eng Time required for exiting the playback.Unit: us
                                  @else   退出播放的耗时，单位：ms @endif */
    uint64_t seekTime;        /*!< @if Eng Time required for seeking.Unit: us
                                  @else   播放跳转的耗时，单位：ms @endif */
    uint32_t sendSuccessFrames;  /*!< @if Eng Number of frames that have been sent successfully.
                                  @else   已经送成功的帧数 @endif */
    uint32_t threadSleepTime;   /*!< @if Eng Thread sleep time.Unit: ms
                                  @else   线程休眠时间，单位：ms @endif */
    bool hasRenderAudioEos;      /*!< @if Eng Whether data has been sent.
                                  @else   是否已经送数据完成 @endif */
    uint32_t sendDataFailTimes;   /*!< @if Eng Number of times that single frame data fails to be sent.
                                  @else   送单帧数据失败次数 @endif */
    uint32_t renderTime;        /*!< @if Eng Rendering time consumption.Unit: us
                                   @else   渲染的耗时，单位：us @endif */
    uint32_t pauseTime;         /*!< @if Eng Pause time consumption.Unit: us
                                   @else   暂停的耗时，单位：us @endif */
    uint32_t resumeTime;        /*!< @if Eng Resume time consumption.Unit: us
                                   @else   恢复的耗时，单位：us @endif */
} LitePlayerDumpInfo;

typedef struct {
    uint32_t toneId;            /*!< @if Eng ID of the currently played announcement.
                                  @else   当前播放提示音ID @endif */
    const char *filePath;       /*!< @if Eng Path of the current file to be played.
                                  @else   当前播放文件路径 @endif */
    uint32_t filePathLen;       /*!< @if Eng Current path length of the playing file.
                                  @else   当前播放文件路径长度 @endif */
    int32_t priority;           /*!< @if Eng Priority of the current playback source    .
                                  @else   当前播放源的优先级 @endif */
    bool singleLoop;            /*!< @if Eng Is the current playback source set to loop
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
} LitePlayerDataSourceInfo;

/**
 * @if Eng
 * @brief lite player callback function.
 *
 * Sets the callback event reported when the playback is successful or abnormal.
 *
 * @param [in] enEvent Indicates the Callback Event Type, see {@link LitePlayerEvent}.
 * @param [in] data Indicates the Event return value.
 * @param [in] cookie Indicates the Context transferred by the callback function.
 * @return void.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 轻量播放器回调函数。
 *
 * 设置播放成功或异常时上报的回调事件。
 *
 * @param [in] enEvent 回调事件类型，参考{@link LitePlayerEvent}。
 * @param [in] data 事件返回值。
 * @param [in] cookie 回调函数传入的Context。
 * @return void.
 * @自1.0起
 * @版本1.0
 * @endif
 */
typedef void (*LitePlayerEeventFunc)(uint32_t enEvent, const void *data, const void *cookie);

/**
 * @if Eng
 * @brief create player.
 *
 * @param [out] handle Indicates the lite player instance.
 * @return Returns <b>0</b> if the LitePlayer is successfully created; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 创建播放器。
 *
 * @param [out] handle 轻量播放器实例。
 * @return 如果创建成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerCreate(LitePlayerHandle *handle);

/**
 * @if Eng
 * @brief Sets the toggle config for the player.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] toggleConfig Indicates the toggle info.
 * @return Returns <b>0</b> if the setting is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置播放器的切换配置。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] toggleConfig 同步起播定时器信息{@link ToggleConfig}。
 * @return如果设置成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetToggleConfig(const LitePlayerHandle handle, const ToggleConfig *toggleConfig);

/**
 * @if Eng
 * @brief Sets the playback source for the player. The corresponding source can be the file descriptor (FD) of the
 * local file, local file URI.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] info Indicates Information related to the playback source. Currently, only local file URIs are supported.
 * {@link LitePlayerSetDataSource}。
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置播放器的播放源。对应的源可以是的文件描述符(FD)
 * 本地文件，本地文件URI。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] info 播放源相关的信息。目前仅支持本地文件URI。参考{@link LitePlayerDataSourceInfo}。
 * @return 如果设置成功返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetDataSource(const LitePlayerHandle handle, const LitePlayerDataSourceInfo *info);

/**
 * @if Eng
 * @brief Prepares the playback environment and buffers media data.
 *
 * This function must be called after {@link LitePlayerSetDataSource}.
 *
 * @param [in] handle Indicates the lite player instance.
 * @return Returns <b>0</b> if the playback environment is prepared and media data is buffered;
 * otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 准备播放环境，缓存媒体数据。
 *
 * 该函数必须在{@link LitePlayerSetDataSource}之后调用。
 *
 * @param [in] handle 轻量播放器实例。
 * @return 如果播放环境已准备好且媒体数据已缓存，则返回<b>0</b>；
 * 否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerPrepare(const LitePlayerHandle handle);

/**
 * @if Eng
 * @brief set sessionId.
 *
 * sets the session ID after the LitePlayerPrepare is be called.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] sessionId Indicates the sessionId.
 * @return Returns <b>0</b> Indicates that the setting is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置会话ID
 *
 * 设置调用LitePlayerPrepare后的会话ID。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] sessionId 指定sessionId。
 * @return 返回 <b>0</b>表示设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetSessionId(const LitePlayerHandle handle, uint32_t sessionId);

/**
 * @if Eng
 * @brief set stream type.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] streamType Indicates the stream type. {@link AudioStreamType}.
 * @return Returns <b>0</b> if the stream type is set successfully; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置流类型。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] streamType 流类型。{@link AudioStreamType}。
 * @return 如果设置流类型成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetStreamType(const LitePlayerHandle handle, AudioStreamType streamType);

/**
 * @if Eng
 * @brief Starts or resumes playback.
 *
 * This function must be called after {@link LitePlayerPrepare}.
 * If the player state is <b>Prepared</b>, this function is called to start playback.
 * If the player state is <b>Playback paused</b>, this function is called to resume playback.
 *
 * @param [in] handle Indicates the lite player instance.
 * @return Returns <b>0</b> if the playback starts or resumes; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开始或恢复播放。
 *
 * 该函数必须在{@link LitePlayerPrepare}之后调用。
 * 如果播放器状态为<b>Prepared</b>，则调用此函数开始播放。
 * 如果播放器状态为<b>Playback paused</b>，则调用此函数恢复播放。
 *
 * @param [in] handle 轻量播放器实例。
 * @return 如果开始播放或继续播放，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerPlay(const LitePlayerHandle handle);

/**
 * @if Eng
 * @brief Stops playback.
 *
 * @param [in] handle Indicates the lite player instance.
 * @return Returns <b>0</b> if the playback is stopped; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止播放。
 *
 * @param [in] handle 轻量播放器实例。
 * @return如果停止播放，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerStop(const LitePlayerHandle handle);

/**
 * @if Eng
 * @brief Pauses playback.
 *
 * @param [in] handle Indicates the lite player instance.
 * @return Returns <b>0</b> if the playback is paused; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 暂停播放。
 *
 * @param [in] handle 轻量播放器实例。
 * @return如果暂停播放，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerPause(const LitePlayerHandle handle);

/**
 * @if Eng
 * @brief Play the seek program.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] mSeconds Indicates The time position that needs to be sought.
 * @return Returns <b>0</b> if The seek operation is successful. ; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 播放seek。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] mSeconds 需要seek到的时间位置
 * @return如果seek成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSeek(const LitePlayerHandle handle, int64_t mSeconds);

/**
 * @if Eng
 * @brief Obtains the information about the current file to be played.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [out] fileInfo Indicates the playing file information.
 * @return Returns <b>0</b> If the information about the currently played file is obtained successfully;
 * otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取当前播放文件信息。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [out] fileInfo 播放文件信息。
 * @return 如果获取当前播放文件信息成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerGetFileInfo(const LitePlayerHandle handle, LitePlayerFileInfo *fileInfo);

/**
 * @if Eng
 * @brief Obtains the dump information about the current playback..
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [out] dumpInfo Indicates the Dump information about the current playback.
 * @return Returns <b>0</b> if The dump information is obtained successfully.
 * otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取当前播放dump信息。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [out] dumpInfo 获取的当前播放dump信息。
 * @return 如果获取当前播放dump信息成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerGetDumpInfo(const LitePlayerHandle handle, LitePlayerDumpInfo *dumpInfo);

/**
 * @if Eng
 * @brief Registers the lite player callback function..
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] eventFunc Indicates the callback function pointer.
 * @param [in] cookie Indicates the Context transferred by the callback function.
 * @return Returns <b>0</b> if the playback callback function is successfully set; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 注册轻量播放器回调函数。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] eventFunc 回调函数指针。
 * @param [in] cookie 回调函数传入的Context。
 * @return 如果设置播放回调函数成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetPlayerCallback(const LitePlayerHandle handle,
    LitePlayerEeventFunc eventFunc, const void *cookie);

/**
 * @if Eng
 * @brief Sets the volume of the lite player.
 *
 * This function can be used during playback or pause.
 * The volume needs to be set to a DB value that is 1000 times magnified.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] volume Indicates the target volume of the audio channel to set.
 * @return Returns <b>0</b> Indicates that the volume is set successfully.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置轻量播放器的音量。
 *
 * 此函数可以在播放或暂停时使用。
 * 音量需要设置为放大1000倍的DB值。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] volume 设置声道的目标音量。
 * @return 返回 <b>0</b>表示音量设置成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerSetVolume(const LitePlayerHandle handle, int32_t volume);

/**
 * @if Eng
 * @brief Gets the volume of the lite player.
 *
 * This function can be used during playback or pause.
 * The obtained volume is amplified by 1000 times.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [out] volume Indicates the target volume of the audio channel to get.
 * @return Returns <b>0</b> Indicates that the volume is get successfully.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取轻量播放器的音量。
 *
 * 此函数可以在播放或暂停时使用。
 * 获取的音量是放大了1000倍的DB值。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [out] volume 获取声道的目标音量。
 * @return 返回 <b>0</b>表示音量获取成功。；否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerGetVolume(const LitePlayerHandle handle, int32_t *volume);

/**
 * @if Eng
 * @brief Sets loop playback.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [in] singleLoop Specifies whether to enable loop playback.
 * The value <b>true</b> means to enable loop playback, and <b>false</b> means to disable loop playback.
 * @return Returns <b>0</b> if the single loop is set successfully; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置循环播放。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [in] singleLoop 是否开启循环播放。<b>true</b>表示启用循环播放。
 * 和<b>false</b>表示关闭循环播放。
 * @return 如果单循环设置成功，则返回<b>0</b>，否则返回失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerEnableSingleLooping(const LitePlayerHandle handle, bool singleLoop);

/**
 * @if Eng
 * @brief Obtains the player state.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [out] states Indicates the player state. For details, see {@link LitePlayerStates}.
 * @return Returns <b>0</b> Indicates that the player state is obtained; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief获取播放器状态。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [out] states 表示播放器状态。详细信息请参见{@link LitePlayerStates}。
 * @return 返回<b>0</b>表示获取播放器状态，否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerGetCurPlayState(const LitePlayerHandle handle, uint32_t *states);

/**
 * @if Eng
 * @brief Obtains the playback position, accurate to millisecond.
 *
 * @param [in] handle Indicates the lite player instance.
 * @param [out] position Indicates the playback position.
 * @return Returns <b>0</b> Indicates that the playback position is obtained; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取播放位置，精确到毫秒。
 *
 * @param [in] handle 轻量播放器实例。
 * @param [out] position 播放位置。
 * @return 返回<b>0</b>表示获取播放位置，否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerGetCurPosition(const LitePlayerHandle handle, int64_t *position);

/**
 * @if Eng
 * @brief destroy lite player.
 *
 * @param [in] handle Indicates the lite player instance.
 * @return Returns <b>0</b> Indicates that the handle is deleted successfully; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 销毁轻量播放器。
 *
 * @param [in] handle 轻量播放器实例。
 * @return 返回 <b>0</b>表示删除句柄成功，否则失败。
 * @自1.0起
 * @版本1.0
 * @endif
 */
errcode_t LitePlayerDestroy(const LitePlayerHandle handle);

#ifdef __cplusplus
}
#endif

#endif
