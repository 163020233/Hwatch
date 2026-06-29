/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux audio setting\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-01， Create file. \n
 */
#ifndef UX_AUDIO_SETTING_H
#define UX_AUDIO_SETTING_H

#include "errcode.h"
#include "osal_list.h"
#include "ux_api_audio_common.h"
#include "ux_api_audio_setting.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CB_VOLUME_MIN (-25600)
#define CORRECTION_VALUE (25600.0)
#define LINEAR_VOLUME_MAX (255)
#define CB_TO_LINEAR_RATE (256)
#define ROUNDING_OFFSET 0.5
#define VOLUME_CURVE_EXPONENT 2.5
#define CHANNEL_VOLUME_MAX (255)
#define CHANNEL_VOLUME_MIN (0)

typedef struct volume_manager_t {
    ux_api_audio_stream_control_t volume_control;
    uint16_t current_volume_level;
    int32_t current_volume_db;
    int32_t max_volume_db;
    int32_t min_volume_db;
    int16_t step;
    bool is_muted;
    int32_t* volume_levels;
    uint16_t volume_count;
    struct osal_list_head node;
} volume_manager_t;

errcode_t ux_audio_volume_init(ux_api_audio_stream_control_t *info, int32_t *volume_levels, uint16_t volume_count);
errcode_t ux_audio_set_volume(int32_t db);
errcode_t ux_audio_get_volume(int32_t *db);
errcode_t ux_audio_set_volume_level(uint16_t level);
errcode_t ux_audio_set_mute(bool mute);
errcode_t ux_audio_get_mute_state(bool *mute);
errcode_t ux_audio_set_rate(uint32_t rate);
errcode_t ux_audio_set_autorate_param(const uint8_t *param, uint16_t length);

errcode_t ux_audio_set_stream_volume(ux_api_audio_volume_info_t* info);
errcode_t ux_audio_get_stream_volume(ux_api_audio_volume_info_t* info);
errcode_t ux_audio_register_volume_manager(volume_manager_t* volume_mgr);
volume_manager_t* ux_audio_get_volume_manager(ux_api_audio_stream_control_t* info);
uint8_t ux_audio_volume_cb_to_linear(int32_t cb);
void ux_audio_remote_volume_init(void);
void ux_audio_setting_init(void);
void ux_audio_setting_deinit(void);
errcode_t ux_audio_set_stream_mute(ux_api_audio_mute_info_t* info);
errcode_t ux_audio_get_stream_mute(ux_api_audio_mute_info_t* info);
errcode_t ux_audio_set_default_volume(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif