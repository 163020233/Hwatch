/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux manager at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-21， Create file. \n
 */
#ifndef UX_AT_MANAGER_H
#define UX_AT_MANAGER_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_interface_define.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_MAX_QUERY_EARSIDE_LENGTH 20
#define UX_MAX_QUERY_BT_LENGTH 35
#define UX_MAX_QUERY_TWO_BT_LENGTH 70
#define UX_MAX_QUERY_LED_LENGTH 30
#define UX_MAX_QUERY_ROLE_LENGTH 30
#define UX_MAX_QUERY_ADDR_LENGTH 30
#define UX_MAX_QUERY_NAME_LENGTH 80
#define UX_MAX_QUERY_BLE_NAME_LENGTH 80
#define UX_MAX_QUERY_BLE_EXPAND_VAL_LENGTH 100
#define UX_MAX_QUERY_STATE_LENGTH 30
#define UX_MAX_QUERY_INFO_LENGTH 128
#define UX_MAX_QUERY_DATA_LENGTH 256
#define UX_MAX_QUERY_BUTTON_LENGTH 50
#define UX_MAX_QUERY_WORKMODE_LENGTH 20
#define UX_MAX_QUERY_DEFAULT_LENGTH 20

#define UX_MAX_PARA_LENGTH (UX_MAX_QUERY_DATA_LENGTH + 30)
#define LANGUAGE_LENGTH 10
#define DUTSIG_LENGTH 10
#define TONE_VOL_LENGTH 10
#define PROFILE_STATE_LENGTH 30
#define UX_RESULT_DATA_LENGTH 30
#define BATTERY_MIN_VALUE 1
#define BATTERY_MAX_VALUE 100
#define UX_AT_PARA_MAX_LENGTH 56
#define UX_AT_CMD_MAX 56
#define UX_TEST_CMD_LEN 8
#define DECIMAL_BASE 10

#define UX_PLAYBACK_SOURCE_PATH "source"
#define UX_PLAYBACK_START_PLAY "start"
#define UX_PLAYBACK_PAUSE_PLAY "pause"
#define UX_PLAYBACK_RESUME_PLAY "resume"
#define UX_PLAYBACK_STOP_PLAY "stop"
#define UX_PLAYBACK_GET_PLAY_STATE "state"


#define SWITCH_ON_TYPE "on"
#define SWITCH_OFF_TYPE "off"

#define FAIL_TYPE "fail"
#define IDLE_TYPE "idle"
#define INIT_TYPE "init"

#define HAID_DOING_TYPE "doing"
#define HAID_FITTING_TYPE "fitting"
#define HAID_MEDICAL_TYPE "medical"

#define PLAY_PAUSE_MUSIC_TYPE "play_pause"
#define PLAY_MUSIC_TYPE "play"
#define PLAY_PRE_MUSIC_TYPE "play-"
#define PLAY_NEXT_MUSIC_TYPE "play+"
#define PAUSE_MUSIC_TYPE "pause"
#define STOP_MUSIC_TYPE "stop"
#define MUSIC_MUTE_TYPE "mute"
#define DISABLE_MUSIC_ATTR "disable_attr"
#define ENABLE_MUSIC_ATTR "enable_attr"

#define A2DP_TYPE "a2dp"
#define HFP_TYPE "hfp"

#define INCOMING_CALL_TYPE "incoming"
#define DIALING_CALL_TYPE "dialing"
#define RINGING_CALL_TYPE "ringing"
#define ANSWER_CALL_TYPE "answer"
#define HANGUP_CALL_TYPE "hangup"
#define REJECT_CALL_TYPE "reject"

#define MASTER_POWEROFF_STATE_STR "master_idle"
#define MASTER_SOUND_STATE_STR "master_sound"
#define MASTER_HAID_STATE_STR "master_haid"
#define MASTER_RECOVER_STATE_STR "master_recover"
#define MASTER_OTA_STATE_STR "master_ota"
#define MASTER_DUT_STATE_STR "master_dut"
#define MASTER_STATE_BUTT_STR "unknown"

#define CALL_IDLE_STATE_STR "call_idle"
#define INCOMING_STATE_STR "incoming_call"
#define DIALING_STATE_STR "ongoing_call"
#define RINGING_STATE_STR "ringing_call"
#define ANSWER_STATE_STR "answer_phone"
#define PLAY_MUSIC_STATE_STR "play_music"
#define STOP_MUSIC_STATE_STR "stop_music"
#define PAUSE_MUSIC_STATE_STR "pause_music"
#define PLAY_MUSIC_MUTE_STATE_STR "play_music_mute"
#define CALL_STATE_BUTT_STR "unknown"

#define VOLUME_UP_TYPE "volume+"
#define VOLUME_DOWN_TYPE "volume-"

#define VOICEASST_TYPE "voiceassistant"

#define SHORT_TOUCH_TYPE "short_touch"
#define LONG_TOUCH_TYPE "long_touch"
#define SLIP_UP_TYPE "slip+"
#define SLIP_DOWN_TYPE "slip-"

#define POSITION_OUTBOX "outbox"
#define POSITION_INBOX_TYPE "inbox"

#define PAIR_EAR "ear"
#define PAIR_EAR_DISCOVER "ear_discover"
#define PAIR_UNLINK "peer_unlink"
#define PAIR_LINK "peer_link"
#define PAIR_DEVICE "device"
#define PAIR_DISCONNECT "disconnect"
#define PAIR_START_CONNECT "start_connect"
#define PAIR_STOP_CONNECT "stop_connect"
#define PAIR_BT_STOP_PAIR "stop_pair"
#define PAIR_BLE_EXPAND "ble_expand"
#define PAIR_BLE_EXPAND_END "ble_expand_end"
#define PAIR_BLE_START_DISCOVER "ble_start_discover"
#define PAIR_BLE_STOP_DISCOVER "ble_stop_discover"
#define PAIR_BLE_DISCONNECT "ble_disconnect"
#define PAIR_CLEAR "clear"
#define PAIR_INFO "info"
#define PAIR_BLE "ble"
#define PAIR_ISO "iso"
#define PAIR_STATE "pair_state"
#define TRX_MODE "trx"

#define RX_START_REC "rx_start_rec"
#define RX_STOP_REC "rx_stop_rec"

#define UX_AT_BOX_COVER "cover"
#define UX_AT_COVER_OPEN "open"
#define UX_AT_COVER_CLOSE "close"
#define UX_AT_BOX_DEVICE_POS "position"
#define UX_AT_DEVICE_OUT_BOX "outbox"
#define UX_AT_DEVICE_IN_BOX "inbox"

#define UX_BOX_BATTERY "bat"

#define BUTTON_PWR "0"
#define BUTTON_1 "1"
#define BUTTON_2 "2"
#define BUTTON_3 "3"
#define BUTTON_MODE_1 "1"
#define BUTTON_MODE_2 "2"
#define BUTTON_MODE_3 "3"
#define BUTTON_MODE_4 "4"
#define BUTTON_MODE_5 "5"
#define BUTTON_MODE_6 "6"
#define BUTTON_MODE_10 "10"

#define WEAR_TYPE "wear"
#define WEAR_IN "in"
#define WEAR_IN_LEFT "in_left"
#define WEAR_IN_RIGHT "in_right"
#define WEAR_OUT "out"
#define WEAR_CONTROL_TYPE "wear_control"
#define WEAR_CONTROL_IN_TYPE "in"
#define WEAR_CONTROL_OUT_TYPE "out"
#define WEAR_CONTROL_SENSOR_TYPE "sensor"
#define POS_TYPE "pos"
#define RESET_TYPE "reset"
#define WEAR_DETECTION_TYPE "wear_detection"
#define LOW_LATENCY_TYPE "low_latency"
#define DUAL_CONNECTION_TYPE "dual_connection"
#define TRY_HANDOVER "try_handover"

#define OUT_BOX_TYPE "outbox"
#define IN_BOX_TYPE "inbox"
#define RECOVER_TYPE "recover"
#define WORK_TYPE "work"

#define WORK_DUT_TYPE "dut"
#define WORK_OTA_TYPE "ota"
#define WORK_RECOVER_TYPE "recover"

#define BT_ADDR "btaddr"
#define BLE_ADDR "bleaddr"
#define SLE_ADDR "sleaddr"
#define PEER_ADDR "peeraddr"
#define BT_NAME "name"
#define BLE_NAME "blename"
#define BLE_EXPAND_VAL "bleexpandval"
#define EAR_SIDE "earside"
#define BT_ROLE "role"
#define POLOR_MODE "polor_mode"
#define TONE_VOL   "volume"
#define TONE_WORK_STATE   "work"
#define TONE_OUTBAND_RING   "outband_ring"
#define HAID_VOLUME "volume"
#define HAID_MODE "mode"
#define HAID_WORK_TYPE "worktype"

#define RIGHT_EAR_STR "R"
#define LEFT_EAR_STR "L"

#define CONNECTED "connected"
#define CONNECTING "connecting"
#define DISCONNECTED "disconnected"
#define UX_UNKNOWN "unknown"

#define START_PAIR "start_pair"
#define STOP_PAIR "stop_pair"
#define START_CONNECT "start_connect"
#define STOP_CONNECT "stop_connect"
#define START_DISCOVERY "start_discovery"
#define STOP_DISCOVERY "stop_discovery"
#define START_DISCONNECT "start_disconnect"
#define UX_OFF_WORKING "off_working"

#define MASTER_ROLE "master"
#define SLAVE_ROLE "slave"
#define RECORDER_32_BIT_FLOAT "32f"
#define RECORDER_48000_SAMPLE_RATE "48000"
#define RECORDER_32_BIT_INT "32i"
#define RECORDER_24_BIT_INT "24i"
#define LANGUAGE_TYPE "language"
#define LANGUAGE_CHN "cn"
#define LANGUAGE_ENG "en"

#define OFFWORKING_AT_INVALID "\r\nOFF WORKING STATUS, AT INVALID!\r\n"
#define ONWORKING_AT_INVALID "\r\nON WORKING STATUS, AT INVALID!\r\n"
#define OTA_AT_INVALID "\r\n OTA STATUS, AT INVALID!\r\n"
#define NOT_OUTBOX_AT_INVALID "\r\nNOT OUTBOX STATUS, AT INVALID!\r\n"
#define SEA "sea"

#define HAID_OFFSET_TWO 2
#define HAID_OFSET_ONE 1
#define EQUIP_HAID_FIT_CMD_LEN 9
#define EQUIP_HAID_FIT_CMD_IN 1
#define EQUIP_HAID_FIT_CMD_OUT 2
#define EQUIP_HAID_MODE_SAVE_CMD_LEN 9
#define EQUIP_HAID_MODE_NO_SAVE 1
#define EQUIP_HAID_MODE_SAVE 2
#define EQUIP_HAID_POWER_DELAY_CMD_LEN 10
#define EQUIP_HAID_POWER_DELAY_VALUE_SHIFT 8

typedef at_ret_t (*cmd_handle_t)(const char *para);
typedef at_ret_t (*two_para_cmd_handle_t)(const char *para1, const char *para2);
typedef at_ret_t (*three_para_cmd_handle_t)(const char *para1, const char *para2, const char *para3);
typedef at_ret_t (*query_handle_t)(void);
typedef at_ret_t (*audio_cmd_handle_t)(const char *type, const char *value);
typedef at_ret_t (*audio_query_handle_t)(const char *type);

typedef struct {
    const char *cmd;
    cmd_handle_t cmd_handle;
} ux_cmd_t;

typedef struct {
    const char *cmd;
    two_para_cmd_handle_t cmd_handle;
} ux_two_para_cmd_t;

typedef struct {
    const char *cmd;
    three_para_cmd_handle_t cmd_handle;
} ux_three_para_cmd_t;

typedef struct {
    const char *audio_cmd;
    audio_cmd_handle_t cmd_handle;
} audio_cmd_t;

typedef struct {
    const char *cmd;
    uint8_t event_id;
} cmd_event_t;

typedef struct {
    char *button;
    char *type;
    char *mode;
    uint8_t event_id;
} button_event_t;

typedef struct {
    const char *cmd;
    query_handle_t query_handle;
} cmd_query_t;

typedef struct {
    const char *audio_cmd;
    audio_query_handle_t query_handle;
} audio_query_t;

typedef struct {
    const char *cmd;
    uint8_t event_type;
    uint8_t event_id;
} cmd_active_t;

typedef struct {
    const char *led_cmd;
    two_para_cmd_handle_t cmd_handle;
} led_cmd_t;

typedef struct {
    uint32_t para_map;
    const char *para1;
} ux_at_para1_args_t;

typedef struct {
    uint32_t para_map;
    const char *para1;
    const char *para2;
} ux_at_para2_args_t;

typedef struct {
    uint32_t para_map;
    const char *para1;
    const char *para2;
    const char *para3;
} ux_at_para3_args_t;

typedef struct {
    uint32_t para_map;
    const char *para1;
    const char *para2;
    const char *para3;
    const char *para4;
} ux_at_para4_args_t;

typedef struct {
    uint32_t para_map;
    const char *para1;
    const char *para2;
    const char *para3;
    const char *para4;
    const char *para5;
} ux_at_para5_args_t;

typedef at_ret_t (*at_2para_cmd_handle_t)(const ux_at_para2_args_t *args);
typedef at_ret_t (*at_5para_cmd_handle_t)(const ux_at_para5_args_t *args);

typedef struct {
    const char *cmd;
    at_2para_cmd_handle_t cmd_handle;
} ux_at_2para_cmd_pair_t;

typedef struct {
    const char *cmd;
    at_5para_cmd_handle_t cmd_handle;
} ux_at_5para_cmd_pair_t;

errcode_t ux_at_cmd_init(void);
at_ret_t at_power_set_process(const ux_at_para4_args_t *args);
at_ret_t at_power_read_process(void);
at_ret_t at_power_query_process(const ux_at_para4_args_t *args);
at_ret_t at_audio_cfg_set_process(const ux_at_para3_args_t *args);
at_ret_t at_audio_cfg_query_process(const ux_at_para3_args_t *args);
at_ret_t at_mic_cfg_set_process(const ux_at_para2_args_t *args);
at_ret_t at_button_set_process(const ux_at_para3_args_t *args);
at_ret_t at_button_read_process(void);
at_ret_t at_ux_power_set_process(const ux_at_para4_args_t *args);
at_ret_t at_ux_power_query_process(const ux_at_para2_args_t *args);
at_ret_t at_ux_device_ctrl_set_process(const ux_at_para2_args_t *args);
at_ret_t at_ux_led_set_process(const ux_at_para3_args_t *args);
at_ret_t at_button_query_process(const ux_at_para3_args_t *args);
at_ret_t at_tone_set_process(const ux_at_para2_args_t *args);
at_ret_t at_tone_query_process(const ux_at_para2_args_t *args);
at_ret_t at_ux_tone_set_process(const ux_at_para1_args_t *args);
at_ret_t at_sound_read_process(const ux_at_para1_args_t *args);
at_ret_t at_call_set_process(const ux_at_para2_args_t *args);
at_ret_t at_call_read_process(void);
at_ret_t at_music_set_process(const ux_at_para1_args_t *args);
at_ret_t at_music_read_process(void);
at_ret_t parse_tone_vol_para_process(const char *para);
at_ret_t parse_tone_work_state_para_process(const char *para);
at_ret_t parse_tone_outband_ring_process(const char *para);
at_ret_t parse_play_tone_para_process(const char *para);
at_ret_t query_tone_vol(void);
at_ret_t at_profile_query_process(const ux_at_para2_args_t *args);
at_ret_t get_string_info(char *result, uint16_t result_size, uint32_t *offset, const char *prefix,
                         ux_at_callback_t at_callback, ux_at_cmd_callback_t at_cmd_callback);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
