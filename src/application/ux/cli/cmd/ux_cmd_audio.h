/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd audio service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_AUDIO_H
#define UX_CMD_AUDIO_H

#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_set_volume_level(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_volume_level(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_volume_level(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_audio_energy_value(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_eq_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_eq_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_eq_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_low_cut_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_low_cut_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_low_cut_state(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_mute_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_mute_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_mute_state(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_nr_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_nr_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_nr_mode(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_recorder_remain_time(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_remain_time(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_display_recorder_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_state(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_sample_rate(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_sample_rate(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_sample_rate(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_file_split_duration(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_split_duration(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_file_split_duration(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_file_split_size(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_split_size(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_file_split_size(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_bit_width(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_bit_width(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_bit_width(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_file_path(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_recorder_file_path(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_file_path(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_recorder_alg_bypass(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_notify_recorder_alg_bypass(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_music_mute(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_call_mute(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_music_mute_state(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_call_mute_state(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_audio_answer_call(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_hangup_call(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_reject_call(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_call_volumeup(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_call_volumedown(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_play_pause_music(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_play_music(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_pause_music(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_prev(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_next(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_volumeup(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_volumedown(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_attr_enable(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_music_attr_disable(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_audio_set_eq(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_audio_mute_toggle(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_audio_jack_in(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_jack_out(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_linein_music_start(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_audio_linein_music_stop(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_sync_audio_state_2_master(void);
void ux_cmd_audio_register(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
