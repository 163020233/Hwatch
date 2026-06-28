/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_a2dp.h
 * Description:  provide ux audio a2dp header file.
 */

#ifndef UX_AUDIO_A2DP_H
#define UX_AUDIO_A2DP_H
#include "audio.h"
#include "audio_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

void ux_audio_a2dp_init(void);
void ux_audio_a2dp_deinit(void);
errcode_t ux_audio_a2dp_sink_set_param(ux_audio_stream_codec *a2dp_cfg);
errcode_t ux_audio_a2dp_sink_start(void);
errcode_t ux_audio_a2dp_sink_stop(void);
errcode_t ux_audio_music_volume_sync(AudioStreamType stream_type, audio_volume_set_struct *volume_set);
uint8_t audio_manager_get_max_music_volume_value(void);

#ifdef __cplusplus
}
#endif
#endif