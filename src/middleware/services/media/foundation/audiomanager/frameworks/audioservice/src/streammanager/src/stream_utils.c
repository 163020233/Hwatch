/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description: stream common
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "stream_utils.h"
#include <math.h>
#include "audio_types.h"

#define MAGNIFY_TIMES 1000.0
#define THOUSAND 1000

int32_t ConvertOutDeviceToHal(uint32_t deviceId, int32_t *halDevice)
{
    if ((deviceId & IN_FLAG) == 0) {
        return false;
    }

    uint32_t outDeviceFw[OUT_DEVICE_NUM] = {OUT_BUILTIN_SPEAKER, OUT_BLUETOOTH_SCO, OUT_BLUETOOTH_A2DP};
    uint32_t outDeviceHal[OUT_DEVICE_NUM] = {PIN_OUT_SPEAKER, PIN_OUT_HEADSET, PIN_OUT_HEADSET};

    int32_t actualDeviceNum = 0;
    uint32_t halDeviceTmp = 0;
    for (int i = 0; i < OUT_DEVICE_NUM; i++) {
        if ((deviceId & outDeviceFw[i]) != 0) {
            halDeviceTmp |= outDeviceHal[i];
            actualDeviceNum++;
        }
    }
    *halDevice = (int32_t)halDeviceTmp;
    return actualDeviceNum;
}

char *StringState(StreamState state)
{
    switch (state) {
        case IDLE: {
            return "IDLE";
        }
        case RUNNING: {
            return "RUNNING";
        }
        case PAUSE: {
            return "PAUSE";
        }
        case STOP: {
            return "STOP";
        }
        default: {
            return "INVALID STATE";
        }
    }
}

char *StringType(AudioStreamType streamType)
{
    switch (streamType) {
        case AUDIO_STREAM_ALARM: {
            return "AUDIO_STREAM_ALARM";
        }
        case AUDIO_STREAM_RING: {
            return "AUDIO_STREAM_RING";
        }
        case AUDIO_STREAM_NOTIFICATION_SYSTEM: {
            return "AUDIO_STREAM_NOTIFICATION_SYSTEM";
        }
        case AUDIO_STREAM_NOTIFICATION_PROMPT: {
            return "AUDIO_STREAM_NOTIFICATION_PROMPT";
        }
        case AUDIO_STREAM_MUSIC: {
            return "AUDIO_STREAM_MUSIC";
        }
        case AUDIO_STREAM_A2DP_MUSIC: {
            return "AUDIO_STREAM_A2DP_MUSIC";
        }
        case AUDIO_STREAM_VOICE_CALL_BT_SCO: {
            return "AUDIO_STREAM_VOICE_CALL_BT_SCO";
        }
        case AUDIO_STREAM_VOICE_CALL_VOLTE: {
            return "AUDIO_STREAM_VOICE_CALL_VOLTE";
        }
#ifdef HI3322_CHIP_V100
        case AUDIO_STREAM_VOICE_CALL_VOLTE_SPI: {
            return "AUDIO_STREAM_VOICE_CALL_VOLTE_SPI";
        }
#endif
        case AUDIO_STREAM_VOICE_RECORD: {
            return "AUDIO_STREAM_VOICE_RECORD";
        }
        default: {
            return "INVALID TYPE";
        }
    }
}

bool IsValidVolumeValue(float value)
{
    if (value < 0 || isnan(value)) {
        return false;
    }
    uint32_t tmpValue = (uint32_t)(value * MAGNIFY_TIMES);
    return tmpValue >= 0 && tmpValue <= THOUSAND;
}

