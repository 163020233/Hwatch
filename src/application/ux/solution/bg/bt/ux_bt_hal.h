/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bt_hal.h
 * Description:  ux bt hal interface
 */

#ifndef __UX_BT_HAL_H__
#define __UX_BT_HAL_H__
#include <stdint.h>
#include "errcode.h"
#include "bs_api_stru.h"
#include "ux_bs_generic.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define UX_BS_BT_STREAM_ACCEPT_FLAGS 1
#define MELODY_LEFT                  "melody_l"
#define MELODY_RIGHT                 "melody_r"

typedef enum {
    UX_BS_BT_CALL_DATE_LINK_CREATE = 0,      /* 请求创建通话数据传输链路 */
    UX_BS_BT_CALL_DATE_LINK_RELEASE,         /* 请求释放通话数据传输链路 */
    UX_BS_BT_CALL_DATE_LINK_MAX,             /* 无效枚举 */
} ux_bs_br_cal_data_link_action_t;

typedef enum {
    UX_BS_BT_MUSIC_CONTROL_PLAY = 0,      /* 请求播放媒体音频 */
    UX_BS_BT_MUSIC_CONTROL_PAUSE,         /* 请求暂停媒体音频 */
    UX_BS_BT_MUSIC_CONTROL_MAX,           /* 无效枚举 */
} ux_bs_br_music_play_action_t;

typedef enum {
    UX_BS_BT_VOICE_AUDIO_STREAM,           /* 通话类型的音频流 */
    UX_BS_BT_MUSIC_AUDIO_STREAM,           /* 媒体类型的音频流 */
} ux_bs_br_audio_stream_type_t;

typedef struct {
    uint8_t (*get_state)(void);        // 获取当前配对状态
    void (*set_state)(uint8_t state);  // 设置配对状态
} ux_pairing_ops_t;

errcode_t ux_bs_br_init(void);
errcode_t ux_bt_hal_set_bt_config(void);
errcode_t ux_bt_hal_set_battery_level(uint8_t battery);
errcode_t ux_bt_hal_set_dual_connection(void);
errcode_t ux_bt_hal_set_low_latency(void);
errcode_t ux_bt_hal_set_reconn_phase(void);
errcode_t ux_bs_br_enable(void);
errcode_t ux_bt_hal_start_pair(void);
errcode_t ux_bt_hal_start_connect(void);
errcode_t ux_bt_hal_stop_pair(void);
errcode_t ux_bt_hal_stop_connect(void);
errcode_t ux_bt_hal_disconnect(void);
errcode_t ux_bt_hal_stream_replay(bool accept, uint8_t type);
errcode_t ux_bt_hal_a2dp_req_replay(service_ack_audio_a2dp_s *param);
errcode_t ux_bt_hal_sco_req_replay(service_ack_audio_sco_s *param);
errcode_t ux_bs_br_power_off_prepare(void);
errcode_t ux_bs_br_power_off(void);
errcode_t ux_bs_br_try_handover(void);
errcode_t ux_bs_br_discovery_peer(void);
errcode_t ux_bt_hal_send_data(uint16_t conn_id, uint8_t *data, uint16_t data_len);
errcode_t ux_bs_br_update_connect_state(uint8_t connect_num);
errcode_t bs_update_connect_state(uint8_t connect_num, const ux_pairing_ops_t *ops);
errcode_t ux_bt_hal_tone_sync(uint8_t tone_type, uint8_t is_start_toggle);
errcode_t ux_bt_hal_tone_sync_with_param(uint8_t tone_type, uint8_t is_start_toggle, uint8_t param1, uint8_t param2);
errcode_t ux_bt_hal_config_device_name(void);

errcode_t ux_bs_bt_call_date_link_control(ux_bs_br_cal_data_link_action_t action);
errcode_t ux_bs_bt_music_play_control(ux_bs_br_music_play_action_t action);

errcode_t ux_bs_bt_toggle_music_play(uint8_t *param, uint16_t len);
errcode_t ux_bs_bt_call_control(ux_bs_call_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_bt_music_control(ux_bs_music_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_bt_voice_assistant_control(ux_bs_voice_assistant_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_bt_sound_control(ux_bs_sound_proc_type type, uint8_t *param, uint16_t len, uint8_t volume);
errcode_t ux_bs_bt_set_audio_channel(ux_api_device_audio_channel_t audio_channel);
errcode_t ux_bs_bt_clear_pair_info(void);
errcode_t ux_bs_bt_sync_peer_vol(int32_t type, int32_t volume, int32_t flag);
errcode_t ux_bs_bt_sync_haid_info(uint8_t *param, uint8_t param_len);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

