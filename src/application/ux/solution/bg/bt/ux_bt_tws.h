/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bt_tws.h
 * Description:  Audio ux bt tws interface
 */

#ifndef __UX_BT_TWS_H__
#define __UX_BT_TWS_H__

#include "errcode.h"
#include "bt_service.h"
#include "ux_api_tws.h"
#include "ux_api_device.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#ifndef OUT_EAR_DETECTION_TIMEOUT
#define OUT_EAR_DETECTION_TIMEOUT 300000
#endif

typedef enum {
    HANDOVER_BY_POSITION_AND_BATTERY,
    HANDOVER_BY_POSITION,
    HANDOVER_BY_BATTERY,
    HANDOVER_BY_LOW_BATTERY,
} bt_handover_type;

typedef enum {
    WEAR_DETECTION_FOR_OUT_EAR,
    WEAR_DETECTION_FOR_IN_EAR,
    WEAR_DETECTION_FOR_CLOSE_BOX,
    WEAR_DETECTION_FOR_MUSIC_PLAY,
    WEAR_DETECTION_FOR_MUSIC_PAUSED,
    WEAR_DETECTION_FOR_CALL_ACTIVE,
    WEAR_DETECTION_FOR_SWITCH_CHANGE,
    WEAR_DETECTION_FOR_HANDOVER,
} bt_wear_detection_type;

typedef enum {
    MUSIC_PAUSED_REASON_INIT,       /* 音频暂停初始化 */
    MUSIC_PAUSED_BY_OUT_EAR,        /* 耳机出耳触发音乐暂停 */
    MUSIC_PAUSED_REASON_BUTT,
} ux_music_paused_reason;

typedef struct {
    uint8_t tws_state;                          // service_bt_acl_state
    uint8_t role_state;                         // ux_service_twslink_role
    uint8_t has_coupled_bt_earbud;              // has_coupled_bt_earbud
    uint8_t has_coupled_sle_earbud;             // has_coupled_sle_earbud
    uint8_t has_coupled_ble_earbud;             // has_coupled_ble_earbud
    uint8_t other_mobile_connect_state;         // service_bt_acl_state
} ux_bt_tws_state;

typedef struct {
    uint32_t reserved;
    uint8_t state[BD_ADDR_SIZE];
    uint8_t addr[BD_ADDR_SIZE];
} ux_bt_tws_send_state_t;

typedef struct {
    uint32_t music_paused_time;                     // 音乐出耳暂停定时器剩余时间
    uint8_t music_paused_reason;                    // 音乐暂停原因
    uint8_t wear_detection_switch;                  // 智能佩戴检测开关
    uint8_t position_state;                         // 耳机位置信息
    uint8_t wear_side;                              // 耳机佩戴左右耳信息
    uint32_t tone_vol;                              // 提示音音量
    uint8_t tone_language;                          // 提示音语言
    uint8_t anc_state;                              // ANC降噪开关
    uint8_t anc_mode;                               // ANC降噪模式
    uint8_t dual_connection_switch;                 // 双设备连接开关
    uint32_t pga_gain;                              // PGA音量增益值
    uint8_t battery_percent;                        // 电量百分比信息
    uint8_t charge_state;                           // 充电状态信息
    uint8_t device_reconn_type;                     // 耳机回连类型
    uint8_t connect_num;                            // 耳机连接数量
    uint8_t bt_pairing_state;                       // 蓝牙协议栈配对连接状态
    uint8_t sle_pairing_state;                      // 蓝牙协议栈配对连接状态
    uint8_t combo_pairing_state;                    // 双模协议栈配对连接状态
    uint8_t connected_protocol_type;                // 已连接的协议类型
    uint8_t src_role;                               // 信息源的tws角色
} __attribute__((__packed__)) ux_tws_sync_info_t;

ux_bt_tws_state *ux_bt_get_tws_state(void);
void ux_bt_tws_state_init(void);
void ux_bt_main_state_change_handle(uint8_t old_state, uint8_t new_state);

errcode_t ux_bt_set_audio_channel(ux_api_device_audio_channel_t audio_channel);
ux_api_device_audio_channel_t ux_bt_get_audio_channel(void);
errcode_t ux_bt_update_audio_channel(ux_api_device_audio_channel_t peer_audio_channel);
void ux_bt_update_mono_mode(void);

void ux_bt_send_tws_battery_info(void);
void ux_bt_send_tws_position_info(uint8_t index);
void ux_bt_send_tws_wear_detection_switch(void);
void ux_bt_send_tws_dual_connection_switch(void);
void ux_bt_send_tws_music_paused_state(void);
void ux_bt_send_tws_language_info(void);
void ux_bt_send_tws_pga_gain_info(void);
void ux_bt_send_tws_tone_vol_info(void);
void ux_bt_send_tws_local_addr_info(void);
void ux_bt_send_tws_connect_state(uint8_t index);
void ux_bt_sync_tws_info(void);
errcode_t ux_bt_tws_send_state_to_peer(uint8_t type, uint8_t *data, uint16_t data_len);

errcode_t ux_bt_wear_in(void);
errcode_t ux_bt_wear_out(void);
errcode_t ux_bt_enter_box(void);
errcode_t ux_bt_exit_box(void);

errcode_t ux_generate_tws_addresses(ux_bd_addr *master_addr, ux_bd_addr *slave_addr, uint8_t type);

void ux_bt_common_music_paused_state_clear(void);
void ux_bt_common_out_ear_detection_timer_start(uint32_t time);
void ux_bt_common_handle_wear_detection(bt_wear_detection_type wear_detection_type);
void ux_bt_common_handle_dual_connection(void);
void ux_bt_common_handle_low_latency(void);
void ux_bt_common_handle_handover(bt_handover_type handover_type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

