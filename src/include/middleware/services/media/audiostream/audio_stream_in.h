/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
* Description: audio stream interfaces
* Author: Media Software Group
* Create: 2023-05-24
*/

#ifndef AUDIO_STREAM_IN_H
#define AUDIO_STREAM_IN_H

#include "audio_base_type.h"
#include "audio_debug_info.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief Enumerates the time base of timespec. Different timing methods are supported.
 */
typedef enum {
    /** Monotonically increasing time, excluding the system sleep time */
    MONOTONIC = 0,
    /** Monotonically increasing time, including the system sleep time */
    BOOTTIME = 1
} Timebase;

typedef void *AudioStreamInHandle;

/**
 * @if Eng
 * @brief Create uplink stream handle.
 *
 * @param [out] handle Indicates uplink stream handle.
 * @return Returns <b>0</b> if the creation is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 创建上行流句柄。
 *
 * @param [out] handle 表示上行流句柄。
 * @return 如果创建成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInCreate(AudioStreamInHandle *handle);

/**
 * @if Eng
 * @brief uplink audio stream initialization.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [in] config Indicates Configuration parameters for initializing the uplink audio stream.
 * @param [in] cbk Indicates Callback function for the uplink audio stream.
 * @param [in] caller Indicates Context required by the callback function for the uplink audio stream.
 * @return Returns <b>0</b> If initialization is successful; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 上行音频流初始化。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [in] config 指示初始化上行音频流的配置参数。
 * @param [in] cbk 指示上行音频流的回调函数。
 * @param [in] caller 指示回调函数所需的上下文。
 * @return 如果初始化成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInInit(const AudioStreamInHandle handle, const CapturerInputConfig *config,
    CapturerCallback cbk, const void *caller);

/**
 * @if Eng
 * @brief Obtain the device ID for the upstream audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] deviceId Indicates device id.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取上行音频流的设备ID。
 *
 * @param [in] handle 表示上行流句柄。
 * @param [out] deviceId 表示设备ID。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInGetCurrentDeviceId(const AudioStreamInHandle handle, int32_t *deviceId);

/**
 * @if Eng
 * @brief Get the data channel ID for the uplink audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] channelId Indicates data channel ID.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取上行音频流的数据通道ID。
 *
 * @param [in] handle 表示上行流句柄。
 * @param [out] channelId 表示数据通道ID。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInGetCurrentChannelId(const AudioStreamInHandle handle, uint32_t *channelId);

/**
 * @if Eng
 * @brief Data channel ID for binding the upstream audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] backendId Indicates data channel ID. [out]
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 用于绑定上游音频流的数据通道ID。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [out] backendId 指示数据通道ID。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInAttachBackend(const AudioStreamInHandle handle, uint32_t backendId);

/**
 * @if Eng
 * @brief Get current time location information.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] position Indicates current time location.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取当前时间位置信息。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [out] position 指示当前时间位置。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInGetPosition(const AudioStreamInHandle handle, int64_t *position);

/**
 * @if Eng
 * @brief Start the uplink audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开始上行音频流。
 *
 * @param[in]  handle 指示上行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInStart(const AudioStreamInHandle handle);

/**
 * @if Eng
 * @brief stop the uplink audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止上行音频流。
 *
 * @param [in] handle 指示上行流句柄。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInStop(const AudioStreamInHandle handle);

/**
 * @if Eng
 * @brief Get audio data from the uplink audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] buffer Indicates audio data buffer.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 从上行音频流获取音频数据。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [out] buffer 指示音频数据缓冲区。
 * @return 如果成功，返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInObtainBuffer(const AudioStreamInHandle handle, const CapturerBuffer *buffer);

/**
 * @if Eng
 * @brief Set the upstream audio stream configuration parameters.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [in] keyValueList Indicates Parameter key-value pair string pointer.
 * @param [in] len Indicates Parameter key-value pair string length.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 设置上行音频流配置参数。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [in] keyValueList 指示参数键值对字符串指针。
 * @param [in] len 指示参数键值对字符串长度。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInSetParam(const AudioStreamInHandle handle, const char *keyValueList, uint32_t len);

/**
 * @if Eng
 * @brief Get upstream audio stream audio parameters.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] keyValueList Indicates Parameter key-value pair string pointer.
 * @param [in] len Indicates Parameter key-value pair string length.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取上行音频流音频参数。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [out] keyValueList 指示参数键值对字符串指针。
 * @param [in] len 指示参数键值对字符串长度。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInGetParam(const AudioStreamInHandle handle, char *keyValueList, uint32_t len);

/**
 * @if Eng
 * @brief Release the uplink audio stream..
 *
 * @param [in] handle Indicates uplink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 释放上行音频流。
 *
 * @param [in] handle 指示上行流句柄。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInRelease(const AudioStreamInHandle handle);

/**
 * @if Eng
 * @brief destory the uplink audio stream..
 *
 * @param [in] handle Indicates uplink stream handle.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 销毁上行音频流。
 *
 * @param [in] handle 指示上行流句柄。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInDestroy(const AudioStreamInHandle handle);

/**
 * @if Eng
 * @brief Obtain dump information about the uplink audio stream.
 *
 * @param [in] handle Indicates uplink stream handle.
 * @param [out] info Indicates Dump information for downlink audio streams.
 * @return Returns <b>0</b> If successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 获取上行音频流的转储信息。
 *
 * @param [in] handle 指示上行流句柄。
 * @param [out] info 指示上行音频流的转储信息。
 * @return 成功时返回 <b>0</b>；否则失败。
 * @since 1.0
 * @version 1.0
 * @endif
 */
errcode_t AudioStreamInDumpInfo(const AudioStreamInHandle handle, StreamDebugInfo *info);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

