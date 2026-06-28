/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide managing earphone instance service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef UX_SYSTEM_MANAGER_H
#define UX_SYSTEM_MANAGER_H
#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "ux_system_attr.h"
#include "ux_status_define.h"
#include "ux_fwk_msg.h"
#ifdef DEPENDENCY_AUDIO
#include "recorder_service_interface.h"
#endif
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define DEFAULT_DB_VAL 8
#define DEFAULT_REC_BIT 32
#define LED_LIGHT_SW_ON 0x01
#define LED_LIGHT_SW_OFF 0X00

#define BATTERY_CHANGE_INTERVAL 5   /* 电量变化间隔 */
#define BATTERY_INFO_MIN_VALUE  1   /* 电量有效值最小值 */
#define BATTERY_INFO_MAX_VALUE  100 /* 电量有效值最大值 */

typedef void (*cmd_handler_t)(uint16_t dev_id, const bt_control_msg_t *msg);
typedef struct {
    bt_control_cmd_t cmd;
    cmd_handler_t handler;
} cmd_handler_entry_t;

bool is_sound_state(void);
void set_call_state(call_state_t state);
call_state_t get_call_state(void);

void set_haid_state(haid_state_t state);
haid_state_t get_haid_state(void);

void record_audio_time(void);
void record_calling_time(void);
void record_config_time(void);
void init_service_time(void);
ux_service_time_count_t *get_service_time(void);

uint16_t get_time_switch_aidmode(void);
uint16_t get_delay_open_aidmode(void);
uint8_t get_audio_aid_mix(void);

void set_wearing_status(uint8_t state);
uint8_t get_wearing_status(void);

void set_tws_role_state(uint8_t state);
uint8_t get_tws_role_state(void);

void set_real_charge_state(uint8_t state);
uint8_t get_real_charge_state(void);

void set_battery_percent(uint8_t state);
uint8_t get_battery_percent(void);

void set_peer_battery_percent(uint8_t percent);
uint8_t get_peer_battery_percent(void);

void set_device_position_state(uint8_t state);
uint8_t get_device_position_state(void);

void set_peer_ear_state(uint8_t state);
uint8_t get_peer_ear_state(void);

void set_peer_ear_spp_state(uint8_t state);
uint8_t get_peer_ear_spp_state(void);

void set_bt_standby_state(uint8_t state);
uint8_t get_bt_standby_state(void);

void init_device_pairing_failed_nums(void);
void record_device_pairing_failed_nums(void);
void set_device_pairing_failed_nums(uint8_t nums);
uint8_t get_device_pairing_failed_nums(void);

void init_peer_pair_nums(void);
void record_peer_pair_nums(void);
uint8_t get_peer_pair_nums(void);

void init_device_reconn_times(void);
void record_device_reconn_times(void);
uint8_t get_device_reconn_times(void);

void init_device_reconn_duration(void);
void set_device_reconn_duration(uint32_t duration);
uint32_t get_device_reconn_duration(void);
void set_device_reconnect_state(bool state);
bool is_device_reconnect_state(void);

void set_switch_pairing_state(bool state);
bool get_switch_pairing_state(void);

uint8_t get_touch_config_status(void);

errcode_t set_wear_control_state(uint8_t state);
uint8_t get_wear_control_state(void);

void set_device_reconn_type(uint8_t type);
uint8_t get_device_reconn_type(void);
void set_peer_device_reconn_type(uint8_t type);
uint8_t get_peer_device_reconn_type(void);

void set_device_pair_state(uint8_t state);
uint8_t get_device_pair_state(void);

void set_bt_device_pair_state(uint8_t state);
uint8_t get_bt_device_pair_state(void);
void set_sle_device_pair_state(uint8_t state);
uint8_t get_sle_device_pair_state(void);

void set_connected_protocol_type(uint8_t state); /* 记录当前和耳机建连的协议类型 */
uint8_t get_connected_protocol_type(void); /* 获取当前或最近耳机建连的协议类型 */

uint8_t get_peer_connected_protocol_type(void); /* 记录对耳当前和耳机建连的协议类型 */
void set_peer_connected_protocol_type(uint8_t type); /* 获取对耳当前或最近耳机建连的协议类型 */

void set_combo_pairing_state(uint8_t state);
uint8_t get_combo_pairing_state(void);
void set_bt_pairing_state(uint8_t state);
uint8_t get_bt_pairing_state(void);
void set_sle_pairing_state(uint8_t state);
uint8_t get_sle_pairing_state(void);
void set_ble_pairing_state(uint8_t state);
uint8_t get_ble_pairing_state(void);

void set_peer_bt_pairing_state(uint8_t state); /* 设置对耳蓝牙是否处于配对或回连中 */
uint8_t get_peer_bt_pairing_state(void); /* 获取对耳蓝牙是否处于配对或回连中 */
void set_peer_sle_pairing_state(uint8_t state); /* 设置对耳星闪是否处于配对或回连中 */
uint8_t get_peer_sle_pairing_state(void); /* 获取对耳星闪是否处于配对或回连中 */
void set_peer_combo_pairing_state(uint8_t state); /* 设置对耳双模是否处于配对或回连中 */
uint8_t get_peer_combo_pairing_state(void); /* 获取对耳双模是否处于配对或回连中 */

void set_peer_device_pair_state(uint8_t state); /* 设置对耳的设备连接状态 */
uint8_t get_peer_device_pair_state(void);       /* 查询对耳的设备连接状态 */

void set_peer_device_connect_nums(uint8_t num);   /* 设置对耳的设备连接数量 */
uint8_t get_peer_device_connect_nums(void);       /* 查询对耳的设备连接数量 */

uint8_t get_device_connect_nums(void);            /* 查询当前耳机的设备连接数量 */

void set_peer_charge_state(uint8_t state);
uint8_t get_peer_charge_state(void);

void set_slave_trigger_dev_pair_state(uint8_t state); /* 设置按键触发配对状态 */
uint8_t get_slave_trigger_dev_pair_state(void);       /* 查询按键触发配对状态 */

void set_lowbattery_state(uint8_t state);
uint8_t get_lowbattery_state(void);

void set_cover_state(uint8_t state);
uint8_t get_cover_state(void);

void set_button_state(uint8_t state);
uint8_t get_button_state(void);

void set_touchpad_state(uint8_t state);
uint8_t get_touchpad_state(void);

uint16_t get_delay_wear_aidmode(void);

void set_ble_conn_state(uint8_t state);
uint8_t get_ble_conn_state(void);

void set_call_sound_state(uint8_t state);
uint8_t get_call_sound_state(void);

void set_working_state(uint8_t state);
uint8_t get_working_state(void);
uint8_t get_pre_working_state(void);

void set_bt_working_state(uint8_t state);
uint8_t get_bt_working_state(void);

errcode_t set_play_mode(uint8_t play_mode);
uint8_t get_play_mode(void);
void update_play_mode(void);

void set_haid_working_state(uint8_t state);
uint8_t get_haid_working_state(void);
bool is_off_working_state(void);
void working_state_init(void);

void set_led_state(uint8_t state);
uint8_t get_led_state(void);
void set_tone_state(uint8_t state);
uint8_t get_tone_state(void);
void set_notifica_state_finish(notifica_work_state_t state);

void set_storage_power_state(uint8_t state);
uint8_t get_storage_power_state(void);

void set_power_on_haid_delay_state(bool state);
bool is_power_on_haid_delay_state(void);

void set_box_battery_percent(uint8_t percent);
uint8_t get_box_battery_percent(void);

void set_ota_state(uint8_t state);
uint8_t get_ota_state(void);

void set_dev_time(uint8_t *time, uint8_t time_length);
uint8_t *get_dev_time(void);

void device_acc_on_init(void);
void ux_device_power_on_init(void);
void ux_device_power_off_init(void);

void set_device_acc_on_state(bool is_acc_on);
bool get_device_acc_on_state(void);

void write_service_time_to_nv(void);
void update_delay_wear_sensor_aidmode(void);
void update_delay_wear_in_aidmode(void);
void update_touch_config_status(void);
void update_audio_aid_mix(void);
void update_delay_open_aidmode(void);
void update_time_switch_aidmode(void);
void update_service_time(void);
void update_control_wearing_status(void);
void update_button_config(void);
ux_button_config_t *get_button_config(void);

void set_box_charge_state(uint8_t percent);
uint8_t get_box_charge_state(void);

void default_nv_value_init(void);
void set_ftm_mode(ftm_state_t state);
uint8_t get_ftm_mode(void);

void set_power_off_mode(uint8_t mode);
uint8_t get_power_off_mode(void);

void set_power_on_mode(uint8_t mode);
uint8_t get_power_on_mode(void);

void set_recover_type(uint8_t type);
uint8_t get_recover_type(void);

void set_bt_init_state(bool state);
bool is_bt_init_state(void);

uint8_t ux_get_ble_expand_end_val(void);
uint8_t ux_get_ble_expand_type(void);
uint8_t ux_get_ble_expand_val_max_len(void);
uint8_t ux_get_bt_name_max_len(void);
uint8_t ux_get_ble_name_max_len(void);

void set_device_disconnect_reason(uint8_t state);
uint8_t get_device_disconnect_reason(void);

void set_device_reconn_phase(uint8_t phase);
uint8_t get_device_reconn_phase(void);

uint8_t get_tone_enable_status(void);
void update_tone_enable_status(void);

uint8_t get_wear_detection_switch(void);
errcode_t set_wear_detection_switch(uint8_t new_switch);
void update_wear_detection_switch(void);
errcode_t set_local_wear_detection_switch(uint8_t new_switch);

void init_low_latency_switch(void);
uint8_t get_low_latency_switch(void);
errcode_t set_low_latency_switch(uint8_t new_switch);
void update_low_latency_switch(void);

void update_bs_device_name(void);

uint8_t get_dual_connection_switch(void);
errcode_t set_dual_connection_switch(uint8_t new_switch);
void update_dual_connection_switch(void);
errcode_t set_local_dual_connection_switch(uint8_t new_switch);

uint8_t get_tws_ear_side(void);
errcode_t set_tws_ear_side(uint8_t ear_side);
void update_tws_ear_side(void);

uint8_t get_tws_pair_state(void);
errcode_t set_tws_pair_state(uint8_t state);
void update_tws_pair_state(void);

void set_scene_mode_state(uint16_t state);
uint16_t get_scene_mode_state(void);

void set_mute_state(mute_state_t state);
mute_state_t get_mute_state(void);

void set_wear_in_haid_delay_state(bool state);
bool is_wear_in_haid_delay_state(void);

void set_bt_power_off_mode(uint8_t mode);
uint8_t get_bt_power_off_mode(void);

void update_scene_mode_status(void);
uint16_t get_scene_mode_state(void);

void ux_device_state_init(void);
void update_scene_mode_status(void);

errcode_t set_recorder_bit_width(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t set_tx_connect_state(uint8_t id, uint8_t state);

device_source_t get_source_by_id(uint8_t id);
uint8_t get_recorder_state(void);
void set_recorder_state(uint8_t state);

uint32_t get_led_key(uint8_t event_type, uint8_t event_id, uint16_t light_flag);

bool set_haid_enable_state(uint8_t state);
uint8_t get_haid_enable_state(void);

bool write_haid_enable_state_to_nv(uint8_t state);
bool ux_sys_mgr_check_power_on_state(void);
bool ux_sys_mgr_check_local_device_id(uint16_t device_id);
bool is_set_haid_afc_calibrate_valid(uint8_t calibrate_time);
uint8_t get_ux_bg_pdl_num(void);
void set_connect_dongle_state(bool connect_dongle);
bool get_connect_dongle_state(void);
void ux_system_info_restore(void);

ux_system_info_t *ux_get_system_info(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
