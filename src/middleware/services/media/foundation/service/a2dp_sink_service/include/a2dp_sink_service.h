/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio manager c wrapper header
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#ifndef A2DP_SINK_SERVICE_H
#define A2DP_SINK_SERVICE_H

#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    AudioCodecFormat audioFormat;
    uint32_t sampleRate;
    uint32_t channelCount;
} AudioFormatConfig;

/**
 * @brief Sets audio effect algorithm parameters.
 *
 * @param effectType Audio effect algorithm type, For details,
    see the {uapi_aef_type} enumeration in the {soc_uapi_aef.h} file.
 * @param param Indicates Algorithm parameter pointer.
 * @param length Indicates Algorithm parameter length.
 * @return Returns {0} If the audio effect algorithm parameters are set successfully; returns {-1}
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerA2dpSinkSetAefParam(int32_t effectType, const char *param, uint32_t length);

/**
 * @brief Sets the audio effect algorithm switch of the A2DP sink channel.
 *
 * <p>Before playing audio, an application sends an interruption request to other applications for
 * pausing audio playback or decreasing the volume, preventing audio conflicts.
 * @param effectType Audio effect algorithm type, For details,
    see the {uapi_aef_type} enumeration in the {soc_uapi_aef.h} file.
 * @param enable
 * @return Returns {0} If the audio effect is set successfully; returns {-1}
 * @since 1.0
 * @version 1.0
 */
int32_t AudioManagerA2dpSinkSetAefEnable(int32_t effectType, bool enable);

int32_t AudioManagerA2dpSinkSetParam(AudioFormatConfig config);

int32_t AudioManagerA2dpSinkCreate(void);

int32_t AudioManagerA2dpSinkDestroy(void);

void AudioManagerA2dpSinkRegistCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MANAGER_C_WRAPPER_H */
