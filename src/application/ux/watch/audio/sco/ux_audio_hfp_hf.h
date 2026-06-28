/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_audio_hfp_hf.h
 * Description:  provide ux audio sco header file.
 */

#ifndef UX_AUDIO_HFP_HF_SCO_H
#define UX_AUDIO_HFP_HF_SCO_H
#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SAMPLE_RATE_8K  8000
#define SAMPLE_RATE_16K 16000

typedef struct {
    AudioCodecFormat codec;  // SCO codec type
    uint32_t sample_rate;    // SCO sample rate
    AudioBitWidth bit_width; // SCO bit width
} ux_audio_sco_codec;

errcode_t ux_audio_sco_set_param(ux_audio_sco_codec *sco_cfg);
errcode_t ux_audio_sco_start(void);
errcode_t ux_audio_sco_stop(void);
errcode_t ux_audio_call_set_speaker_volume(uint8_t volume);
#ifdef __cplusplus
}
#endif
#endif