/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bs_event_center.h
 * Description:  provide ux bs event center header file.
 */

#ifndef UX_BS_EVENT_CENTER_H
#define UX_BS_EVENT_CENTER_H

#include <stdint.h>
#include "errcode.h"
#include "bt_service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RC_PAGE_TIMEOUT 0x04            /* 连接超时: 两个设备一直连接不上 */
#define RC_CONNECTION_TERMINATED_BY_KEY_MISSING 0x06 // 手机侧删除配对信息后耳机回连
#define RC_CONNECTION_TIMEOUT_UX 0x08   /* 超距超时: 两个设备已经连接,某个时间没有收到ack */
#define RC_CONNECTION_EXISTED 0x0B      /* 连接已存在 */
#define RC_CONNECTION_DISALLOWED 0x0C   /* 回连手机时中ux又下发主连主导致 */
#define RC_REMOTE_USER_TERMINATED_CONNECTION_UX 0x13 /* 用户主动断连 */
#define RC_REMOTE_USER_TERMINATED_DUE_TO_POWER_OFF 0x15 /* 远端关机导致断连 */
#define RC_CONNECTION_TERMINATED_BY_LOCAL_HOST  0x16 // 耳机主动断连
#define RC_CONNECTION_DEFAULT_UX 0x0  // 默认

#define SLE_RC_DISCONNECT_BY_PIN_OR_KEY_MISSING  0x05    /* pin或key丢失 */
#define SLE_RC_DISCONNECT_BY_CONNECT_TIMEOUT     0x07    /* 连接超时断链(拉锯断连) */
#define SLE_RC_DISCONNECT_BY_REMOTE_USER         0x10    /* 远端断链 */
#define SLE_RC_DISCONNECT_BY_LOCAL_HOST          0x11    /* 本端断链 */
#define SLE_RC_ESTABLISH_CONNECT_FAIL            0x1C    /* 建连异常 */
#define SLE_RC_DISCONNECT_BY_CANCEL_PAIR         0x21    /* 远端主动删除配对断连 */
#define SLE_RC_DISCONNECT_BY_POWER_OFF           0x22    /* 主动取消配时断连 */

#define SLE_PEER_DISCONNECT_BY_TIMEOUT           0x08    /* 连接超时断链 */


void state_change_notify_ux_manager(uint16_t event_type, uint16_t event, uint8_t state, uint8_t reason_code);
void ux_bt_common_sco_start(service_hfp_sco_state sco_state);
void ux_bt_common_sco_stop(service_hfp_sco_state sco_state);
void ux_bt_common_sink_a2dp_start(service_a2dp_audio_state a2dp_state);
void ux_bt_common_sink_a2dp_stop(service_a2dp_audio_state a2dp_state);

void ux_bt_record_tws_state(void *param);
void ux_bt_handle_handover_status(void *param);
void ux_bt_common_record_sco_state(void *param);
void ux_bt_common_record_a2dp_state(void *param);
void ux_bt_common_work_state_rsp(void *param);
void ux_bt_common_record_pairing_mode_state(void *param);
void ux_bt_handle_tws_role_msg(void *param);
void ux_bt_common_record_mobile_connect_state(void *param);
void ux_bt_common_record_main_state(void *param);
void ux_bt_common_update_hfp_volume(void *param);
void ux_bt_common_update_avrcp_volume(void *param);

errcode_t ux_bt_record_peer_position(void *param);
errcode_t ux_bt_record_peer_battery(void *param);
errcode_t ux_bt_record_peer_wear_detection_switch(void *param);
errcode_t ux_bt_record_peer_music_paused_state(void *param);
errcode_t ux_bt_record_peer_dual_connection_switch(void *param);
errcode_t ux_bt_record_peer_language(void *param);
errcode_t ux_bt_record_peer_tone_vol(void *param);
errcode_t ux_bt_record_peer_addr_info(void *param);
errcode_t ux_bt_record_peer_connect_state(void *param);
errcode_t ux_bt_record_peer_anc_state(void *param);
errcode_t ux_bt_record_peer_anc_mode(void *param);
errcode_t ux_bt_record_peer_pga_gain(void *param);
errcode_t ux_bt_record_peer_sync_info(void *param);

#ifdef __cplusplus
}
#endif
#endif // UX_BS_EVENT_CENTER_H
