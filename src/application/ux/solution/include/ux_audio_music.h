/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_music.h
 * Description:  provide ux audio sle music header file.
 */

#ifndef UX_AUDIO_MUSIC_H
#define UX_AUDIO_MUSIC_H
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
} ux_audio_music_codec;

// 下行
errcode_t ux_audio_music_sink_set_param(ux_audio_music_codec *music_cfg);
errcode_t ux_audio_music_sink_start(void);
errcode_t ux_audio_music_sink_stop(void);

// 上行
errcode_t ux_audio_music_source_set_param(ux_audio_music_codec *music_cfg);
errcode_t ux_audio_music_source_start(void);
errcode_t ux_audio_music_source_stop(void);

errcode_t ux_audio_handle_play_music(void);
errcode_t ux_audio_handle_pause_music(void);


#ifdef __cplusplus
}
#endif
#endif