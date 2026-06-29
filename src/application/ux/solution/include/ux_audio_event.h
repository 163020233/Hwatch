/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_audio_event.h
 * Description:  Audio ux audio event interface
 */

#ifndef UX_AUDIO_EVENT_H
#define UX_AUDIO_EVENT_H

#include "ux_product_interface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifdef CONFIG_AUDIO_EFFECT_CSTM_EQ
void ux_audio_user_eq_setting_param_to_audio(void);
#endif

#ifdef CONFIG_UX_FEATURE_SOUND_TRIGGER
int32_t ux_test_handle_audio_cmd(const char *func_name, int32_t data);
void ux_audio_soundtrigger_event_callback(audio_soundtrigger_event event, audio_sound_trigger_param param);
void ux_audio_handle_accept_call(bool is_preemption);
void ux_audio_hangup_call(void);
errcode_t ux_audio_handle_play_pause_music(void);
#endif
void ux_audio_master_btaddr_callback(uint8_t *bt_addr, uint32_t len);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
