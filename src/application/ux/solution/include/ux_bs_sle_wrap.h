/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle_wrap.h
 * Description:  provide ux bg sle header file.
 */
#ifndef UX_BG_SLE_WRAP_H
#define UX_BG_SLE_WRAP_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_api_bs.h"
#include "ux_api_bs_connection.h"
#include "ux_bs_generic.h"
#include "key_struct.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_BS_STREAM_STATE_STARTING,   // 开启中
    UX_BS_STREAM_STATE_STREAMING,  // 传输中
    UX_BS_STREAM_STATE_STOPED,     // 已停止
    UX_BS_STREAM_STATE_CLOSED,     // 已关闭
} ux_bs_stream_state_t;

typedef enum {
    UX_BS_AUDIO_TYPE_TELE = 0x00,     //  通话类型的音频流
    UX_BS_AUDIO_TYPE_MUSIC = 0x01,    //  媒体类型的音频流
    UX_BS_AUDIO_TYPE_UAC_UP = 0x02,   //  UAC上行流
    UX_BS_AUDIO_TYPE_UAC_DOWN = 0x03, //  UAC下行流
} ux_bs_audio_type_t;

typedef enum {
    UX_BS_AUDIO_ABILITY_NONE,           // 对端不支持音频流
    UX_BS_AUDIO_ABILITY_ONLY_SINK,      // 对端仅支持音宿，即下行
    UX_BS_AUDIO_ABILITY_ONLY_SRC,       // 对端仅支持音源，即上行
    UX_BS_AUDIO_ABILITY_BOTH,           // 对端音宿音源均支持，即上下行

    UX_BS_AUDIO_ABILITY_MAX,
} ux_bs_peer_audio_ability_t;

typedef struct {
    uint8_t bg_addr[UX_BG_ADDR_LEN];  // 星闪/蓝牙地址
    uint8_t flag;                     // 标记当前槽位是否被占用
} ux_air_pairing_session_t;

typedef struct {
    errcode_t (*bs_sle_init)(void);
    errcode_t (*bs_sle_deinit)(void);
    errcode_t (*bs_sle_enable)(void);
    errcode_t (*bs_sle_disable)(void);
    errcode_t (*bs_sle_start_pair)(const ux_bs_addr_t *bs_addr);  // 启动配对模式
    errcode_t (*bs_sle_stop_pair)(void);  // 停止配对模式，用于直播麦等允许一次配对多个设备的产品形态主动停止配对
    errcode_t (*bs_sle_remove_paired)(ux_bs_obj_t obj);  // 清除单个配对信息
    errcode_t (*bs_sle_clear_paired)(void);              // 清空所有配对信息
    errcode_t (*bs_sle_start_discovery)(ux_api_bs_pairing_mode_t method);
    errcode_t (*bs_sle_stop_discovery)(void);
    errcode_t (*bs_sle_disconnect_device)(ux_bs_obj_t obj);
    errcode_t (*bs_sle_start_auto_connect)(void);
    errcode_t (*bs_sle_stop_auto_connect)(void);
    errcode_t (*bs_sle_disconnect)(void);
    errcode_t (*bs_sle_set_reconn_phase)(void);
    errcode_t (*bs_sle_set_config)(void);
    errcode_t (*bs_sle_config_device_name)(void);
    errcode_t (*bs_sle_stream_reply)(bool accept, uint8_t type);
    errcode_t (*bs_sle_update_connect_state)(uint8_t connect_num);
    errcode_t (*bs_sle_set_battery_level)(uint8_t battery);
    errcode_t (*bs_sle_toggle_music_play)(uint8_t *param, uint16_t len);
    errcode_t (*bs_sle_call_control)(ux_bs_call_proc_type type, uint8_t *param, uint16_t len);
    errcode_t (*bs_sle_music_control)(ux_bs_music_proc_type type, uint8_t *param, uint16_t len);
    errcode_t (*bs_sle_voice_assistant_control)(ux_bs_voice_assistant_proc_type type, uint8_t *param, uint16_t len);
    errcode_t (*bs_sle_sound_control)(ux_bs_sound_proc_type type, uint8_t *param, uint16_t len, uint8_t volume);
    errcode_t (*bs_sle_stream_create)(ux_bs_obj_t obj, ux_bs_audio_type_t type);
    errcode_t (*bs_sle_stream_destroy)(ux_bs_obj_t obj, ux_bs_audio_type_t type);
    errcode_t (*bs_sle_audio_enable)(void);
    errcode_t (*bs_sle_audio_disable)(void);
    errcode_t (*bs_sle_confirm)(void);
    errcode_t (*bs_sle_try_handover)(void);
    errcode_t (*bs_sle_get_obj)(ux_bs_obj_t *obj);
    errcode_t (*bs_sle_set_volume)(ux_bs_obj_t obj, ux_bs_audio_type_t type, uint8_t volume);
} ux_bs_sle_wrap_funcs;

errcode_t ux_bs_sle_init(void);
errcode_t ux_bs_sle_deinit(void);
errcode_t ux_bs_sle_enable(void);
errcode_t ux_bs_sle_disable(void);
errcode_t ux_bs_sle_start_pair(const ux_bs_addr_t *bs_addr);
errcode_t ux_bs_sle_stop_pair(void);
errcode_t ux_bs_sle_remove_paired(ux_bs_obj_t obj);  // 清除单个配对信息
errcode_t ux_bs_sle_clear_paired(void);              // 清空所有配对信息
errcode_t ux_bs_sle_start_discovery(ux_api_bs_pairing_mode_t method);
errcode_t ux_bs_sle_stop_discovery(void);
errcode_t ux_bs_sle_disconnect_device(ux_bs_obj_t obj);
errcode_t ux_bs_sle_start_auto_connect(void);
errcode_t ux_bs_sle_stop_auto_connect(void);
errcode_t ux_bs_sle_disconnect(void);
errcode_t ux_bs_sle_set_reconn_phase(void);
errcode_t ux_bs_sle_set_battery_level(uint8_t battery);
errcode_t ux_bs_sle_set_config(void);
errcode_t ux_bs_sle_config_device_name(void);
errcode_t ux_bs_sle_stream_reply(bool accept, uint8_t type);
errcode_t ux_bs_sle_update_connect_state(uint8_t connect_num);
errcode_t ux_bs_sle_toggle_music_play(uint8_t *param, uint16_t len);
errcode_t ux_bs_sle_call_control(ux_bs_call_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_sle_music_control(ux_bs_music_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_sle_voice_assistant_control(ux_bs_voice_assistant_proc_type type, uint8_t *param, uint16_t len);
errcode_t ux_bs_sle_sound_control(ux_bs_sound_proc_type type, uint8_t *param, uint16_t len, uint8_t volume);
errcode_t ux_bs_sle_stream_create(ux_bs_obj_t obj, ux_bs_audio_type_t type);
errcode_t ux_bs_sle_stream_destroy(ux_bs_obj_t obj, ux_bs_audio_type_t type);
errcode_t ux_bs_sle_audio_enable(void);
errcode_t ux_bs_sle_audio_disable(void);
errcode_t ux_bs_sle_confirm(void);
errcode_t ux_bs_sle_get_obj(ux_bs_obj_t *obj);
errcode_t ux_bs_sle_try_handover(void);
errcode_t ux_bs_sle_set_volume(ux_bs_obj_t obj, ux_bs_audio_type_t type, uint8_t volume);
#if defined(MELODY_DEMO_SOUNDBAR) || defined(MELODY_DEMO_DONGLE)

ux_api_bs_pairing_mode_t ux_bs_grant_get_sle_pairing_strategy(void);
errcode_t ux_bs_grant_set_sle_pairing_strategy(ux_api_bs_pairing_mode_t strategy);
errcode_t ux_bs_grant_clear_pair_session(void);
#endif

void ux_bs_sle_register_terminal_funcs(ux_bs_sle_wrap_funcs *funcs);
void ux_bs_sle_register_grant_funcs(ux_bs_sle_wrap_funcs *funcs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif