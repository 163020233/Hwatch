/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provides ux audio tone interface. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-06, Create file. \n
 */
#ifndef UX_AUDIO_TONE_SERVICE_H
#define UX_AUDIO_TONE_SERVICE_H

#include "errcode.h"
#include "audio.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    bool anc_state;
    ux_api_audio_anc_mode_t anc_mode;
} anc_info_t;

void ux_tone_complete(void *tone_player_handle);
void ux_play_tone_process(uint8_t tone_id);
void ux_tone_msg_handle(uint8_t cmd_type, void *data);
char *ux_get_tone_file_name(int16_t tone_id);
errcode_t ux_start_tone_process(const audio_notification_file *tone_file);
void ux_stop_tone_process(void *tone_player_handle, bool is_active_stop);
void ux_stop_all_tone_process(void);
void ux_notify_stop_all_tone(void);
void ux_tone_init(void);
void ux_tone_deinit(void);
anc_info_t* ux_audio_tone_get_anc_state(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif