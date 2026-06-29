/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: auddio recorder wrapper
 * Author: Media Software Group
 * Create: 2025-03-10
 */

#ifndef AUDIO_RECORDER_SERVICE_H
#define AUDIO_RECORDER_SERVICE_H

#include <stdint.h>
#include "audio_recorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RECORDER_PATH_MAX 260

typedef struct {
    char outDir[RECORDER_PATH_MAX];
    AudioSourceType inputSource;
    AudioCodecFormat audioFormat;
    int32_t sampleRate;
    int32_t channelCount;
    AudioStreamType streamType;
    AudioBitWidth bitWidth;
    AudioSession sessionID;
    bool isSeaClose;
    RecorderCallback callback;
    void *cookie;
    uint32_t splitFileDuration;
    uint64_t splitFileSize;
    bool enableOverwriteFiles; /*!< @if Eng The earliest file can be deleted when the recording space is full.
                                        @else   支持录像空间满时删除最早的文件 @endif */
    uint32_t storageWarningThresholdPercent; /*!< @if Eng Percentage of warning threshold for storage space.
                                        @else   存储空间预警阈值百分比 @endif */
    uint32_t frameLength; /*!< @if Eng Audio frame length{@link AudioFrameLength}.
                                        @else   音频帧长{@link AudioFrameLength} @endif */
} AudioRecordInfo;

/**
 * @if Eng
 * @brief enable the recording service, create a recording channel, and start recording.
 *
 * @param [out] handle Indicates the recording instance created.
 * @param [in] recordInfo Indicates the parameters required for creating a recording service.
 * @return Returns <b>0</b> Indicates that the start recording service is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 开启录音服务，创建录音通路，开始录音
 *
 * @param [out] handle 创建的录制实例。
 * @param [in] recordInfo 创建录音业务需要的参数。
 * @return 返回 <b>0</b> 启动录音服务成功；否则失败。
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioRecorderServiceStart(AudioRecorderHandle *handle, const AudioRecordInfo *recordInfo);

/**
 * @if Eng
 * @brief stops the recording service and destroys the entire recording channel and resources.
 *
 * @param [in] handle Indicates the recorder instance.
 * @return Returns <b>0</b> Indicates that the stop recording service is successful.; otherwise is failed.
 * @since 1.0
 * @version 1.0
 * @else
 * @brief 停止录制服务，销毁整个录制通道和资源。
 *
 * @param [in] handle 录制实例。
 * @return 返回 <b>0</b>停止录音成功；否则失败。
 * @自1.0起
 * @版本1.0
 */
errcode_t AudioRecorderServiceStop(const AudioRecorderHandle handle);

#ifdef __cplusplus
}
#endif

#endif