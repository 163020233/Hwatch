/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "audio_manager_service.h"
#include "errcode.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_types.h"
#include "ux_audio_a2dp_sink.h"

static bool g_a2dp_sink_stream_is_active = false;
static bool sink_stream_is_active(void)
{
    return g_a2dp_sink_stream_is_active;
}

static void set_sink_stream_active_state(bool state)
{
    g_a2dp_sink_stream_is_active = state;
}
errcode_t ux_audio_a2dp_sink_set_param(AudioFormatConfig *codec)
{
    AudioFormatConfig config = {
        .audioFormat = codec->audioFormat,
        .channelCount = codec->channelCount,
        .sampleRate = codec->sampleRate,
    };
    int32_t err = UX_ERR_SUCCESS;
    err = AudioManagerA2dpSinkSetParam(config);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_a2dp_sink_set_param] Fail to set a2dp sink codec 0x%x.\n", err);
        return UX_ERR_AUDIO_SET_CODEC_FAIL;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_audio_a2dp_sink_start(void)
{
    if (sink_stream_is_active()) {
        return UX_ERR_SUCCESS;
    }
    int32_t err = UX_ERR_SUCCESS;
    err = AudioManagerA2dpSinkCreate();
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_a2dp_sink_start] Fail to create a2dp sink stream 0x%x.\n", err);
        return UX_ERR_AUDIO_START_STREAM_FAIL;
    }
    set_sink_stream_active_state(true);
    return UX_ERR_SUCCESS;
}

errcode_t ux_audio_a2dp_sink_stop(void)
{
    if (!sink_stream_is_active()) {
        return UX_ERR_SUCCESS;
    }
    int32_t err = UX_ERR_SUCCESS;
    err = AudioManagerA2dpSinkDestroy();
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_a2dp_sink_stop] Fail to destroy a2dp sink stream 0x%x.\n", err);
        return UX_ERR_AUDIO_STOP_STREAM_FAIL;
    }
    set_sink_stream_active_state(false);
    return UX_ERR_SUCCESS;
}

int32_t ux_audio_a2dp_sink_get_volume(void)
{
    return AudioManagerGetVolume(AUDIO_STREAM_A2DP_MUSIC);
}