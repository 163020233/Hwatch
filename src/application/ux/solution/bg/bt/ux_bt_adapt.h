/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_bt_adapt.h
 * Description:  ux bt event register callback interface
 */

#ifndef UX_BT_ADAPT_H
#define UX_BT_ADAPT_H
#include <stdint.h>
#include "errcode.h"
#include "bs_api_stru.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

errcode_t ux_bs_br_register_event_callback(void);
/* 双模场景复用 */
void ux_bt_peer_data_received_callback(uint8_t *data, uint16_t data_len);
void ux_bt_tws_warn_tone_response_cb(bs_warn_tone_sync_info_t *param);

void ux_bt_tws_active_device_report_cb(bs_tws_active_device_t *param);
void ux_bt_mobile_single_pdl_report_cb(bt_nv_mobile_single_pdl_data_s *param);
void ux_bt_mobile_connect_state_report_cb(service_to_ux_mobile_connect_state_msg_param_s *param);
void ux_bt_mobile_order_report_cb(bt_nv_mobile_conn_order_s *param);
void ux_bt_mobile_multi_pdl_report_cb(bt_nv_mobile_multi_pdl_data_s *param);
void ux_bt_mobile_reminder_pdl_report_cb(bt_nv_mobile_multi_pdl_data_s *param);
void ux_bt_main_report_cb(service_to_ux_bt_main_state_msg_param_s *param);
void ux_bt_a2dp_codec_report_cb(service_to_ux_a2dp_codec_setting_msg_param_s *param);
void ux_bt_sco_codec_report_cb(service_to_ux_sco_codec_setting_msg_param_s *param);
void ux_bt_master_connect_state_report_cb(service_to_ux_mobile_connect_state_msg_param_s *param);
void ux_bt_a2dp_state_report_cb(service_to_ux_a2dp_state_msg_param_s *param);
void ux_bt_sco_state_report_cb(service_to_ux_sco_state_msg_param_s *param);
void ux_bt_voice_assistant_state_report_cb(bs_tws_voice_assistant_state_param_t *param);
void ux_bt_tws_call_volume_report_cb(bs_call_volume_update_msg_param_t *param);
void ux_bt_tws_music_volume_report_cb(bs_music_volume_update_msg_param_t *param);
void ux_bt_tws_local_ring_report_cb(bs_tws_play_local_ring_param_t *param);
void ux_bt_tws_peer_addr_report_cb(bs_tws_discover_result_t *param);
void ux_bt_tws_audio_scene_report_cb(bs_audio_scene_info_t *param);
void ux_bt_tws_music_attr_report_cb(bs_music_media_element_attribute_t *param);
void ux_bt_tws_local_volume_report_cb(bs_local_volume_update_event_param_t *param);
void ux_bt_haid_info_sync_cb(bs_haid_param_sync_info_t *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif

