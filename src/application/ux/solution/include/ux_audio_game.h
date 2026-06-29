/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_game.h
 * Description:  provide ux audio sle game header file.
 */

#ifndef UX_AUDIO_GAME_H
#define UX_AUDIO_GAME_H
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
} ux_audio_game_codec;

ux_api_audio_stream_direction_t ux_audio_game_get_stream_direction(void);
ux_api_audio_stream_type_t ux_audio_game_get_stream_type(void);
uint32_t ux_audio_game_get_sample_rate(void);
// 上行
errcode_t ux_audio_game_up_link_set_param(ux_audio_game_codec *game_cfg);
errcode_t ux_audio_game_up_link_start(void);
errcode_t ux_audio_game_up_link_stop(void);

// 下行
errcode_t ux_audio_game_down_link_set_param(ux_audio_game_codec *game_cfg);
errcode_t ux_audio_game_down_link_start(void);
errcode_t ux_audio_game_down_link_stop(void);

// UAC音频参数
void ux_audio_game_set_mic_sample_rate(uint32_t sample_rate);
void ux_audio_game_set_mic_bit_width(uint16_t bit_width);
void ux_audio_game_set_speaker_sample_rate(uint32_t sample_rate);
errcode_t ux_audio_game_increase_volume(void);
errcode_t ux_audio_game_decrease_volume(void);

#ifdef __cplusplus
}
#endif
#endif