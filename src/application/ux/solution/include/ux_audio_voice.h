/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_voice.h
 * Description:  provide ux audio sle voice header file.
 */

#ifndef UX_AUDIO_VOICE_H
#define UX_AUDIO_VOICE_H
#include "audio.h"
#include "audio_types.h"
#include "audio_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

/* audio stream codec config */
typedef struct {
    uint32_t sample_rate;               /* 采样率 */
    uint8_t channel_num;                /* 信道号 */
    uint16_t sample_bit_width;          /* 采样位宽 */
    codec_format_t codec_format;        /* 音频编码格式 */
} ux_audio_voice_codec;

// 上下行
errcode_t ux_audio_voice_dual_link_set_param(ux_audio_voice_codec *voice_cfg);
errcode_t ux_audio_voice_dual_link_start(void);
errcode_t ux_audio_voice_dual_link_stop(void);

// 上行
errcode_t ux_audio_voice_up_link_set_param(ux_audio_voice_codec *voice_cfg);
errcode_t ux_audio_voice_up_link_start(void);
errcode_t ux_audio_voice_up_link_stop(void);

// 下行
errcode_t ux_audio_voice_down_link_set_param(ux_audio_voice_codec *voice_cfg);
errcode_t ux_audio_voice_down_link_start(void);
errcode_t ux_audio_voice_down_link_stop(void);
#ifdef __cplusplus
}
#endif
#endif