/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_linein.h
 * Description:  provide ux audio jack header file.
 */

#ifndef UX_AUDIO_LINEIN_H
#define UX_AUDIO_LINEIN_H
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
} ux_audio_linein_codec;

// 启动/停止音频链路
errcode_t ux_audio_linein_music_link_start(void);
errcode_t ux_audio_linein_music_link_stop(void);

// linein音频参数
errcode_t ux_audio_linein_music_link_set_param(ux_audio_linein_codec *linein_cfg);
ux_api_audio_stream_direction_t ux_audio_linein_get_stream_direction(void);
ux_api_audio_stream_type_t ux_audio_linein_get_stream_type(void);
uint32_t ux_audio_linein_get_sample_rate(void);

#ifdef __cplusplus
}
#endif
#endif