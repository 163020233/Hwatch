/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_state_common.h
 * Description:  Audio ux common state
 */
#ifndef __UX_STATE_COMMON_H__
#define __UX_STATE_COMMON_H__

#include <std_def.h>
#include <los_typedef.h>
#include "ux_types.h"
#include "service_stack_adapter_base.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uint8_t hfp_vol;                    // call volume
    uint8_t avrcp_vol;                  // music volume
    uint8_t mic_role;                   // audio_mic_role
    bool is_mute;                       // mute state
    bool is_enable_attr;                // music attr state
    uint8_t music_stream_codec;         // music stream codec
    uint8_t voice_stream_codec;         // call stream codec
    uint8_t audio_channel;              // 声道
    uint8_t other_audio_channel;        // 对耳声道
} ux_audio_config;

ux_audio_config *ux_get_audio_config(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

