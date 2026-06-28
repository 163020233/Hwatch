/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux master service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef MASTER_SERVICE_H
#define MASTER_SERVICE_H

#include "stdbool.h"
#include "stdint.h"
#include "fsm_master.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*master_serivce_fun)(const uint8_t *param, uint16_t length);
typedef void (*app_notify_fun)(void);

typedef struct {
    uint8_t index;
    master_serivce_fun fun;
} master_serivce_map_t;

typedef struct {
    uint8_t index;
    app_notify_fun fun;
} app_fun_map_t;

typedef enum {
    POWER_ON = 0,
    POWER_ON_ACK,
    POWER_OFF,
    POWER_OFF_ACK,
    POWER_ON_NOTIFICA,
    POWER_OFF_NOTIFICA,
    ACC_ON,
    ACC_OFF,
    ACC_OFF_ACK,
    BT_CONFIG_ACK,
    POWER_OFF_CB,

    CALL_COMING,
    CALL_ANSWER,
    CALL_HANGUP,
    CALL_REJECT,
    CALL_VOLUMEUP,
    CALL_VOLUMEDOWN,
    CALL_ANSWER_PHONE,
    CALL_HANGUP_PHONE,
    CALL_REJECT_PHONE,
    CALL_DIALING,
    CALL_RINGING,
    SOUND_VOICE_START,
    SOUND_VOICE_STOP,

    MUSIC_PLAY,
    MUSIC_STOP,
    MUSIC_PLAY_PRE,
    MUSIC_PAUSE,
    MUSIC_PLAY_NEXT,
    MUSIC_VOLUMEUP,
    MUSIC_VOLUMEDOWN,
    MUSIC_PLAY_PHONE,
    MUSIC_PAUSE_PHONE,
    MUSIC_ATTR_DISABLE,
    MUSIC_ATTR_ENABLE,
    VOICE_ASSISTANT,
    SOUND_MUTE,
    SOUND_UNMUTE,
    SOUND_MUSIC_START,
    SOUND_MUSIC_STOP,
    HAID_FIT_START,
    HAID_FIT_START_RESP,
    HAID_FIT_STOP,
    HAID_FIT_STOP_RESP,
    HAID_START,
    HAID_START_RESP,
    HAID_STOP,
    HAID_STOP_RESP,
    HAID_VOLUMEUP,
    HAID_VOLUMEDOWN,
    HAID_CHANGE_MODE,
    HAID_CHANGE_MODE_RESP,

    PLAY_TONE,
    BLE_CONN,
    PHONE_ACTIVE,
    EARPHONE_ACTIVE,
    WEAR_CONTROL,
    WEAR_INFO,
    POSITION_INFO,
    APP_INFO,

    RECOVER_START,
    RECOVER_END,
    OTA_ANSWER,
    OTA_FAIL,
    OTA_MESSAGE_GET,
    OTA_SUCC,
    ENTER_DUT,
    RESET_INFO,
    ENTER_MEDICAL,
    EXIT_MEDICAL,
    SOUND_MUTE_HAID_CTRL,
    SOUND_UNMUTE_HAID_CTRL,
    AFC_START,
    AFC_START_RESP,
    AFC_END,
    AFC_STOP,
    AFC_STOP_RESP,

    MASTER_BUTT = 0xA0
} service_master_index_t;

typedef enum {
    JACK_PLUGIN_WORKING_STATE_ON,
    JACK_PLUGIN_WORKING_STATE_OFF,
    JACK_PLUGIN_WORKING_STATE_INVALID,
} jack_plugin_working_state_t;

void master_service_execute(uint8_t index, uint8_t *param, uint16_t length);
uint8_t get_master_service_index(fsm_master_state_t state,  uint8_t event);
void master_timer_set_init(void);
void recover_enable_process(void);
bool is_invalid_opt(void);
void notify_bt_config(void);
/* 佩戴检测主动上报接口 */
bool dev_tlv_encode_rpt_wear_status(uint8_t wear_status);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
