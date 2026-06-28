/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bt_common.h
 * Description:  Audio ux common bt interface
 */

#ifndef __UX_BT_COMMON_H__
#define __UX_BT_COMMON_H__

#include <stdbool.h>
#include "errcode.h"
#include "ux_config_common.h"
#include "ux_event_define.h"
#include "bts_def.h"
#include "key_struct.h"
#include "ssf_persistence.h"
#include "bs_sdm_common_dpm.h"
#include "ux_bg_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PD_NUM_MAX                      (10)
#define CLOUD_TOKEN_LEN                 (32)

typedef enum {
    TWS_DISCOVER_STATE_INIT,
    TWS_DISCOVER_STATE_START,
    TWS_DISCOVER_STATE_END,
    TWS_DISCOVER_STATE_MAX,
} ux_tws_discover_state;

typedef enum {
    AUDIO_STREAM_PROTOCOL_BT,     /* 蓝牙协议上报创建音频流请求 */
    AUDIO_STREAM_PROTOCOL_SLE,    /* 星闪协议上报创建音频流请求 */
    AUDIO_STREAM_PROTOCOL_MAX,
} ux_audio_stream_protocol_type;

typedef enum {
    AUDIO_SCENE_TYPE_MUSIC,       /* 音乐模式 */
    AUDIO_SCENE_TYPE_GAME,        /* 游戏模式 */
    AUDIO_SCENE_TYPE_MAX,
} ux_audio_scene_type;

/* TWS link role */
typedef enum {
    UX_TWS_ROLE_MASTER,
    UX_TWS_ROLE_SLAVE
} ux_service_twslink_role;

typedef void (*ux_bt_event_handle_func)(void *);

typedef struct {
    uint32_t msg_id;
    ux_bt_event_handle_func func;
}ux_bt_event_handle_pair;

typedef struct {
    uint8_t main_state;                           // service_main_state
    uint8_t mobile_connect_state;                 // service_bt_acl_state
    ux_service_bt_work_mode work_mode;            // ux_service_bt_work_mode
    bd_addr addr;                                 // typedef unsigned char bd_addr[BD_ADDR_SIZE];
}ux_bt_common_dev_state;

typedef struct {
    uint8_t audio_state;                          // service_a2dp_audio_state
    uint8_t sco_state;                            // service_hfp_sco_state
    uint16_t stream_id;                           // audio stream id
    uint8_t audio_stream_protocol;                // ux_audio_stream_protocol_type
    uint8_t handover_state;                       // bs_handover_state_t
    uint8_t pairing_state;                        // bs_pairing_state_t
    uint8_t voice_assistant_state;                // bs_voice_assistant_state_t
    uint8_t bt_pd_num;                            // bt pdl num
    uint8_t pdl_conn_order[PD_NUM_MAX];           // bt reconn order
    bt_nv_data_pdl_s_t pd_data[PD_NUM_MAX];       // bt pdl
    uint8_t sle_pd_num;                           // sle pdl num
    uint8_t sle_pdl_conn_order[PD_NUM_MAX];       // sle reconn order
    bs_sle_nv_obj_data_t sle_pd_data[PD_NUM_MAX]; // bt pdl
    bs_sdm_common_pdl_data_t cm_pd_data;          // combo pdl
    bs_sdm_common_pdl_order_t cm_pdl_conn_order;  // combo reconn order
    uint8_t bt_pairing_state;                     // device_pairing_state_t
    uint8_t sle_pairing_state;                    // device_pairing_state_t
    uint8_t combo_pairing_state;                  // device_pairing_state_t
    uint8_t outband_ring_played;                  // bool
    uint8_t addr_is_wrong;                        // bool
    uint8_t pd_data_version;                      // record current pdl index
    uint8_t br_work_state;                        // bs_work_state
    uint8_t ble_work_state;                       // bs_ble_state_t
    uint16_t ble_device_id;                       // ble connected device_id
    uint8_t audio_scene_mode;                     // ux_audio_scene_type
    uint8_t adv_data_is_set;                      // bool
    uint8_t is_add_ble_end_val;                   // bool
    uint8_t music_paused_reason;                  // ux_music_paused_reason
    uint64_t music_paused_time;                   // record music paused time
}ux_bt_common_state;

ux_bt_common_state *ux_bt_get_common_state(void);
void ux_bt_common_state_init(void);

void ux_bt_set_outband_ring_played(uint8_t tmp);
uint8_t ux_bt_get_outband_ring_played(void);

uint8_t ux_bt_common_get_pdl_num(void);
uint8_t ux_bt_common_get_max_connect_num(void);
uint8_t volume_convert(uint8_t volume, uint8_t multiplier, uint8_t divisor);
uint32_t ux_get_limit_time(uint32_t time, uint32_t max_time, uint32_t min_time);

ux_audio_stream_protocol_type ux_bt_get_audio_stream_protocol(void);
errcode_t ux_bt_set_audio_stream_protocol(ux_audio_stream_protocol_type protocol);
ux_work_protocol_t ux_bt_get_work_protocol(void);
bool is_support_sle_protocol(void);
bool is_support_bt_protocol(void);

bool is_ux_manager_working(void);
void ux_bt_common_notify_manager(ux_event_master_t event);

uint8_t ux_bt_get_hfp_state(void);
uint8_t ux_bt_get_a2dp_sink_state(void);

uint8_t ux_bt_get_music_paused_reason(void);
void ux_bt_set_music_paused_reason(uint8_t paused_reason);
uint64_t ux_bt_get_music_paused_time(void);
void ux_bt_set_music_paused_time(uint64_t time);

void ux_bt_common_save_peer_addr(ux_work_protocol_t protocol, uint8_t *addr, uint16_t addr_len);

#ifdef __cplusplus
}
#endif

#endif

