/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd manager at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_MANAGER_H
#define UX_CMD_MANAGER_H
#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BUF_LEN_12 12
#define BUF_LEN_20 20
#define BUF_LEN_32 32
#define BUF_LEN_64 64
#define BUF_LEN_128 128
#define BASE16 16

errcode_t ux_cmd_play_tone_by_id(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_tone_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_tone_work_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_outband_ring_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_mic_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_max_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_db_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_loopback_on(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_loopback_off(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_tone_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_mic_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_max_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_db_vol(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_avrcp_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_hfp_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_a2dp_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_hfp_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_mode(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_mix_vol_range(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_set_work_type(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_mode(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_volume(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_haid_get_mix_vol_range(uint8_t *data, uint16_t data_len);
bool bt_peer_addr_is_invalid(void);
bool ux_cmd_check_device_id_valid(uint16_t device_id);
errcode_t ux_cmd_manager_wear_in(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_wear_out(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_inbox(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_outbox(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_power_toggle(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_suspend_bs(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_resume_bs(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_pair_bs(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_reconnect_bs(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_manager_disconnect_bs(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_uac_music_play_or_pause(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_uac_music_prev(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_uac_music_next(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_uac_music_volume_up(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_uac_music_volume_down(uint16_t device_id, uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
