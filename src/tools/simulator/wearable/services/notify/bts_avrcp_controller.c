/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024. All rights reserved.
 * Description: smart voice service.
 */
#include <string.h>
#include <stdio.h>
#include "bts_br_gap.h"
#include "bts_avrcp_target.h"
#include "bts_avrcp_controller.h"
#include "bt_audio.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_a2dp_source.h"
#include "bts_a2dp_sink.h"
#include "bts_spp.h"

errcode_t avrcp_ct_set_absolute_volume(const bd_addr_t *bd_addr, uint8_t volume)
{
    return 0;
}

bool bt_is_acl_connected(const bd_addr_t *addr)
{
    return true;
}

errcode_t gap_disconnect_remote_device(const bd_addr_t *addr)
{
    return 0;
}

errcode_t gap_br_set_bt_scan_mode(uint8_t mode)
{
    return true;
}

errcode_t gap_br_set_inquiry_paramters(uint32_t device_class, uint8_t max_num)
{
    return 0;
}

errcode_t gap_br_start_discovery(void)
{
    return true;
}

errcode_t gap_br_cancel_discovery(void)
{
    return true;
}

errcode_t gap_remove_pair(const bd_addr_t *addr)
{
    return true;
}

bool is_bt_discovering(int transport)
{
    return 0;
}

errcode_t avrcp_tg_notify_volume_changed(uint8_t volume)
{
}

errcode_t avrcp_tg_notify_playback_status_changed(uint8_t play_status)
{
}

errcode_t avrcp_tg_notify_track_changed(uint64_t identifier)
{
}

errcode_t avrcp_tg_notify_track_reached_end(void)
{
}

errcode_t avrcp_tg_notify_track_reached_start(void)
{
}

errcode_t avrcp_tg_notify_playback_pos_changed(uint32_t playback_pos)
{
}

errcode_t avrcp_tg_notify_now_playing_content_changed(void)
{
}

errcode_t avrcp_tg_notify_available_players_changed(void)
{
}

errcode_t avrcp_tg_notify_addressed_players_changed(uint16_t player_id, uint16_t uid_counter)
{
}

errcode_t avrcp_tg_notify_uid_changed(uint16_t uid_counter)
{
}

errcode_t avrcp_ct_register_notification(const bd_addr_t *bd_addr, uint8_t event_id, uint32_t interval)
{
    return 0;
}

errcode_t avrcp_ct_get_supported_events(const bd_addr_t *bd_addr)
{
    return 0;
}

errcode_t avrcp_tg_register_callbacks(avrcp_tg_callbacks_t *func)
{
    return 0;
}

errcode_t avrcp_ct_register_callbacks(avrcp_ct_callbacks_t *func)
{
    return 0;
}

void bt_avrcp_tg_register_audio_cbk(bt_avrcp_tg_bts_cbk *func)
{
}

errcode_t avrcp_ct_connect(const bd_addr_t *bd_addr)
{
    return 0;
}

errcode_t enable_bt_stack(uint8_t transport)
{
    return ERRCODE_SUCC;
}

errcode_t disable_bt_stack(uint8_t transport)
{
    return ERRCODE_SUCC;
}

errcode_t bluetooth_set_local_addr(const bd_addr_t *addr)
{
    return ERRCODE_SUCC;
}

errcode_t bluetooth_get_local_addr(bd_addr_t *addr)
{
    unsigned char tmp[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    return ERRCODE_FAIL;
}

errcode_t bluetooth_get_local_name(unsigned char *local_name, unsigned char *length)
{
    unsigned char* tmp = (unsigned char*)"WatchDemo";
    return ERRCODE_FAIL;
}

errcode_t bluetooth_set_local_name(const unsigned char *local_name, unsigned char length)
{
    return ERRCODE_SUCC;
}

bool bluetooth_factory_reset(void)
{
    return ERRCODE_SUCC;
}

int gap_br_get_scan_mode(void)
{
    return GAP_SCAN_MODE_CONNECTABLE;
}

bool gap_get_paired_devices_num(unsigned int *number)
{
    number = 0;
    return true;
}

bool gap_get_paired_devices_list(gap_paired_device_info_t *dev_info, int *number)
{
    number = 0;
    dev_info = NULL;
    return true;
}

int gap_get_pair_state(const bd_addr_t *addr)
{
    return GAP_PAIR_NONE;
}

bool gap_remove_all_pairs(void)
{
    return true;
}

errcode_t gap_connect_remote_device(const bd_addr_t *addr)
{
    return ERRCODE_SUCC;
}

bool gap_get_device_name(const bd_addr_t *addr, unsigned char *remote_name, unsigned char *length)
{
    return true;
}

int gap_get_device_class(const bd_addr_t *addr)
{
    return 0;
}

errcode_t gap_reconnect_media_device(const bd_addr_t *addr)
{
    return ERRCODE_SUCC;
}

errcode_t gap_bt_set_flow_limit(uint16_t flow_limit)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_set_sec_param(gap_ble_sec_params_t *params)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_register_callbacks(gap_ble_callbacks_t *func)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_connect_param_update(gap_conn_param_update_t *params)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_pair_remote_device(const bd_addr_t *addr)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_stop_adv(uint8_t adv_id)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_set_adv_param(uint8_t adv_id, const gap_ble_adv_params_t *param)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_set_local_appearance(uint16_t appearance)
{
    return ERRCODE_SUCC;
}

errcode_t gap_ble_start_adv(uint8_t adv_id)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_register_callbacks(gatts_callbacks_t *func)
{
    return ERRCODE_SUCC;
}

errcode_t gattc_register_callbacks(gatts_callbacks_t *func)
{
    return ERRCODE_SUCC;
}

errcode_t gattc_register_client(bt_uuid_t *uuid, uint8_t *clientId)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_register_server(bt_uuid_t *app_uuid, uint8_t *server_id)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_add_service(uint8_t server_id, bt_uuid_t *service_uuid, bool is_primary)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_add_characteristic(uint8_t server_id, uint16_t service_handle, gatts_add_chara_info_t *character)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_add_descriptor(uint8_t server_id, uint16_t service_handle, gatts_add_desc_info_t *descriptor)
{
    return ERRCODE_SUCC;
}

errcode_t gatts_start_service(uint8_t server_id, uint16_t service_handle)
{
    return ERRCODE_SUCC;
}

bd_addr_t a2dp_snk_get_active_device(void)
{
    bd_addr_t empty = {};
    return empty;
}

int spp_server_accept(int server_id)
{
    return 0;
}