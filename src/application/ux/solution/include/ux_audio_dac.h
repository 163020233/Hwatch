/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_dac.h
 * Description:  provide ux audio dac header file.
 */

#ifndef UX_AUDIO_DAC_H
#define UX_AUDIO_DAC_H
#include "feature_config.h"
#include "errcode.h"
#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

void ux_audio_dac_init(void);
void ux_audio_dac_deinit(void);
errcode_t ux_audio_dac_start(AudioStreamType stream_type);
errcode_t ux_audio_dac_stop(AudioStreamType stream_type);
#ifdef CONFIG_SUPPORT_JACK
AudioStreamType ux_audio_get_dac_stream_type(void);
errcode_t ux_audio_jack_connect_dac(AudioStreamType stream_type);
#endif

#ifdef __cplusplus
}
#endif
#endif
