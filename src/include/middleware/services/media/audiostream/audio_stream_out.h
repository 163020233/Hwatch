/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2023-05-24
*/

#ifndef AUDIO_STREAM_OUT_H
#define AUDIO_STREAM_OUT_H

#include "audio_base_type.h"
#include "audio_debug_info.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef void *AudioStreamOutHandle;

/**
 * @if Eng
 * @brief Create downlink  stream handle.
 *
 * @param [out] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> if the creation is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 创建下行流句柄。
 *
 * @param [out] handle 表示下行流句柄。
 * @return 如果创建成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutCreate(AudioStreamOutHandle *handle);

/**
 * @if Eng
 * @brief downlink audio stream initialization.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [in] config Indicates Configuration parameters for initializing the downlink audio stream.
 * @param [in] cbk Indicates Callback function for the downlink audio stream.
 * @param [in] caller Indicates Context required by the callback function for the downlink audio stream.
 * @return Returns <b>0</b> If initialization is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 下行音频流初始化。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [in] config 指示初始化下行音频流的配置参数。
 * @param [in] cbk 指示下行音频流的回调函数。
 * @param [in] caller 指示回调函数所需的上下文。
 * @return 如果初始化成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutInit(const AudioStreamOutHandle handle, const AudioRendererConfig *config,
    RendererCallback cbk, const void *caller);
/**
 * @if Eng
 * @brief Start the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开始下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutPlay(const AudioStreamOutHandle handle);

/**
 * @if Eng
 * @brief Pause the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 暂停下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutPause(const AudioStreamOutHandle handle);

/**
 * @if Eng
 * @brief Stop the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutStop(const AudioStreamOutHandle handle);

/**
 * @if Eng
 * @brief Release the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 释放下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutRelease(const AudioStreamOutHandle handle);

/**
 * @if Eng
 * @brief Obtain the device ID for the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] deviceId Indicates device id.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取下行音频流的设备ID。
 *
 * @param [in] handle 表示下行流句柄。
 * @param [out] deviceId 表示设备ID。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutGetCurrentDeviceId(const AudioStreamOutHandle handle, int32_t *deviceId);

/**
 * @if Eng
 * @brief Get the data channel ID for the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] channelId Indicates data channel ID.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取下行音频流的数据通道ID。
 *
 * @param [in] handle 表示下行流句柄。
 * @param [out] channelId 表示数据通道ID。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutGetCurrentChannelId(const AudioStreamOutHandle handle, uint32_t *channelId);

/**
 * @if Eng
 * @brief Data channel ID for binding the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] frontendId Indicates data channel ID.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 用于绑定下行音频流的数据通道ID。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [out] frontendId 指示数据通道ID。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutAttachFrontend(const AudioStreamOutHandle handle, uint32_t frontendId);

/**
 * @if Eng
 * @brief Write audio data to the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [in] buffer Indicates audio data buffer.
 * @param [in] userSize Indicates audio data buffer length.
 * @param [in] writtenSize Indicates the length of the data written successfully.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 将音频数据写入下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [in] buffer 指示音频数据缓冲区。
 * @param [in] userSize 指示音频数据缓冲区长度。
 * @param [in] writtenSize 指示成功写入的数据长度。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutStreamWrite(const AudioStreamOutHandle handle, const uint8_t *buffer,
    uint64_t userSize, uint64_t *writtenSize);

/**
 * @if Eng
 * @brief Get current time location information.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] position Indicates current time location.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取当前时间位置信息。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [out] position 指示当前时间位置。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutGetPosition(const AudioStreamOutHandle handle, int64_t *position);

/**
 * @if Eng
 * @brief Obtain dump information about the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] info Indicates Dump information for downlink audio streams.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取下行音频流的转储信息。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [out] info 指示下行音频流的转储信息。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutDumpInfo(const AudioStreamOutHandle handle, StreamDebugInfo *info);

/**
 * @if Eng
 * @brief Set the downlink audio stream configuration parameters.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [in] keyValueList Indicates Parameter key-value pair string pointer.
 * @param [in] len Indicates Parameter key-value pair string length.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置下行音频流配置参数。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [in] keyValueList 指示参数键值对字符串指针。
 * @param [in] len 指示参数键值对字符串长度。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutSetParam(const AudioStreamOutHandle handle, const char *keyValueList, uint32_t len);

/**
 * @if Eng
 * @brief Get downlink audio stream audio parameters.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [out] keyValueList Indicates Parameter key-value pair string pointer.
 * @param [in] len Indicates Parameter key-value pair string length.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取下行音频流音频参数。
 *
 * @param [in] handle 指示下行流句柄。
 * @param [out] keyValueList 指示参数键值对字符串指针。
 * @param [in] len 指示参数键值对字符串长度。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutGetParam(const AudioStreamOutHandle handle, char *keyValueList, uint32_t len);

/**
 * @if Eng
 * @brief set the synchronization clock switchover parameters of the downlink audio stream.
 *
 * @param [in] handle Indicates downlink stream handle.
 * @param [in] type Indicates Synchronous clock type.
 * @param [in] toggleConfig Indicates Synchronous clock switchover parameters.
 * @return Returns <b>0</b> If successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置下行音频流同步时钟切换参数。
 *
 * @param [in] handle 下行码流句柄。
 * @param [in] type 表示同步时钟类型。
 * @param [in] toggleConfig 同步时钟切换参数。
 * @return 如果成功，则返回<b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutSetToggleConfig(const AudioStreamOutHandle handle,
    ToggleConfigType type, ToggleConfig toggleConfig);

/**
 * @if Eng
 * @brief destory the downlink audio stream..
 *
 * @param [in] handle Indicates downlink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 销毁下行音频流。
 *
 * @param [in] handle 指示下行流句柄。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamOutDestroy(const AudioStreamOutHandle handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif  // AUDIO_STREAM_OUT_H
