/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_sco.h
 * Description:  provide ux audio sco header file.
 */

#ifndef UX_AUDIO_SCO_H
#define UX_AUDIO_SCO_H
#include "audio.h"
#include "audio_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

void ux_audio_sco_init(void);
void ux_audio_sco_deinit(void);
errcode_t ux_audio_sco_set_param(ux_audio_stream_codec *sco_cfg);
errcode_t ux_audio_sco_set_direction(ux_api_audio_stream_direction_t direction);
errcode_t ux_audio_sco_start(void);
errcode_t ux_audio_sco_stop(void);
errcode_t ux_audio_call_volume_sync(AudioStreamType stream_type, audio_volume_set_struct *volume_set);
errcode_t ux_audio_sco_set_mic_role(audio_mic_role mic_role);
uint8_t audio_manager_get_max_call_volume_value(void);

#ifdef __cplusplus
}
#endif
#endif