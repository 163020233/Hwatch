/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 * Description: add media player function \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-06-24, Create file. \n
 */
#ifndef UX_PLAYER_CONTROL_H
#define UX_PLAYER_CONTROL_H

#include "errcode.h"
#include "audio.h"
#include "ux_api_player.h"

#ifdef __cplusplus
extern "C" {
#endif

ux_api_player_state_t ux_get_player_state(void);
void ux_player_msg_handle(uint8_t cmd_type, void *data);
errcode_t ux_start_play_process(const audio_notification_file *music_file);
void ux_player_stop_play_process(void);

#ifdef __cplusplus
}
#endif
#endif