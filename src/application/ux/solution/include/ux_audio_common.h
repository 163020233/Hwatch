/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * File          ux_audio_common.h
 * Description:  ux common func.
 */

#ifndef UX_AUDIO_COMMON_H
#define UX_AUDIO_COMMON_H

#include "audio.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MIN_DOWN_LINK_DB_VOL (-76000)
#define MAX_DOWN_LINK_DB_VOL 0
#define MIN_UP_LINK_DB_VOL 0
#define MAX_UP_LINK_DB_VOL 60000
#define UX_DEFAULT_TONE_DB_VOL (-20000)

typedef struct {
    uint8_t mic_type; /* 记录mic类型 */
    int32_t tone_vol; /* 记录提示音音量 */
    int32_t mic_vol;  /* 记录mic音量 */
    int32_t max_vol;  /* 记录最大音量 */
    int32_t db_vol;   /* 记录db音量 */
} audio_attr_t;

errcode_t ux_audio_set_local_tone_vol(int32_t tone_vol);
errcode_t ux_audio_set_tone_vol(int32_t tone_vol);
int32_t ux_audio_get_tone_vol(void);
errcode_t ux_audio_set_mic_vol(int32_t mic_vol);
int32_t ux_audio_get_mic_vol(void);
errcode_t ux_audio_set_max_vol(int32_t max_vol);
int32_t ux_audio_get_max_vol(void);
errcode_t ux_audio_set_db_vol(int32_t db_vol);
int32_t ux_audio_get_db_vol(void);
void ux_audio_set_mic_type(uint8_t mic_type);
uint8_t ux_audio_get_mic_type(void);
void ux_audio_set_voice_assistant_switch(uint8_t mode);
uint8_t ux_audio_get_voice_assistant_switch(void);
errcode_t ux_audio_set_sea_enable(bool flag);
void ux_audio_sound_mute_haid_ctrl(uint32_t ctrl);
void ux_audio_attr_init(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
