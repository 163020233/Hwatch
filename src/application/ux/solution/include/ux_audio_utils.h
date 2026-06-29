/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * File          ux_audio_recorder_service.h
 * Description:  provide ux audio recorder header file.
 */

#ifndef UX_AUDIO_UTILS_H
#define UX_AUDIO_UTILS_H

#include "errcode.h"
#include "audio_base_type.h"
#include "audio_types.h"
#include "feature_config.h"
#ifndef DEMO_WATCH
#include "key_struct.h"
#endif
#include "ux_api_audio_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
typedef errcode_t (*ux_audio_sync_msg_handler)(uint16_t dev_id, uint8_t *data, uint16_t data_len);
AudioStreamType get_audio_stream_type(ux_api_audio_stream_type_t type);
AudioLinkDirection get_audio_stream_direction(ux_api_audio_stream_direction_t direction);
AudioCodecFormat get_audio_stream_codec_format(codec_format_t format);
AudioParamCmd get_audio_param_cmd(codec_format_t format);
uint16_t get_nv_id(ux_api_audio_stream_type_t type, ux_api_audio_stream_direction_t direction);
errcode_t ux_audio_utils_get_volume(ux_api_audio_stream_type_t type, ux_api_audio_stream_direction_t direction,
    int32_t* volume);
errcode_t ux_audio_utils_set_volume(ux_api_audio_stream_type_t type, ux_api_audio_stream_direction_t direction,
    int32_t volume);
errcode_t ux_audio_info_restore(void);
errcode_t ux_audio_info_store(void);
errcode_t ux_audio_sync_audio_state_2_host(uint8_t *data, uint16_t data_len,
    ux_audio_sync_msg_handler handler);
#ifdef CONFIG_SUPPORT_CUSTOM_VOLUME_LEVELS
audio_downlink_volume_level_t ux_audio_get_volume_levels(void);
void ux_audio_set_volume_levels(audio_downlink_volume_level_t volume_levels);
#endif

#ifdef __cplusplus
#if __cplusplus
}

#endif
#endif
#endif
