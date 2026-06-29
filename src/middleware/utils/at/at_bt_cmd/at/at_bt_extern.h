/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At bt productline \n
 *
 */

#ifndef AT_BT_EXTERN_H
#define AT_BT_EXTERN_H

int ble_at_cmd_factory_register_callbacks(void);
int sle_at_cmd_factory_register_cbks(void);
int enable_sle(void);

/* SLE管理 */
int sle_at_cmd_enable(int argc, char *argv[]);
int gle_at_cmd_set_adv(int argc, char *argv[]);
int gle_at_cmd_enable_adv(int argc, char *argv[]);
int sle_at_cmd_sdk_set_dd_param(int argc, char *argv[]);
int sle_at_cmd_sdk_set_dd_data(int argc, char *argv[]);
int sle_at_cmd_sdk_start_dd(int argc, char *argv[]);
int sle_at_cmd_sdk_stop_dd(int argc, char *argv[]);
int gle_at_cmd_enable_scan(int argc, char *argv[]);
int sle_at_cmd_sdk_conn_remote(int argc, char *argv[]);
int sle_at_cmd_sdk_disconn_remote(int argc, char *argv[]);
int sle_at_cmd_sdk_pair_remote(int argc, char *argv[]);
int sle_at_cmd_sdk_remove_pair_remote(int argc, char *argv[]);
int sle_at_cmd_sdk_get_paired_devices_num(int argc, char *argv[]);
int sle_at_cmd_sdk_get_paired_devices(int argc, char *argv[]);
int sle_at_cmd_sdk_get_pair_state(int argc, char *argv[]);
int gle_at_cmd_set_phy(int argc, char *argv[]);
int sle_at_cmd_sdk_update_conn_param(int argc, char *argv[]);

/* SLE数传 */
/* 服务端 */
int sle_at_cmd_ssaps_register_server(int argc, char *argv[]);
int sle_at_cmd_ssaps_add_service(int argc, char *argv[]);
int sle_at_cmd_ssaps_add_property(int argc, char *argv[]);
int sle_at_cmd_ssaps_add_descriptor(int argc, char *argv[]);
int sle_at_cmd_ssaps_start_service(int argc, char *argv[]);
int sle_at_cmd_ssaps_register_cbks(int argc, char *argv[]);
/* 客户端 */
int sle_at_cmd_ssapc_register_cbks(int argc, char *argv[]);
int sle_at_cmd_ssapc_write_req(int argc, char *argv[]);
int sle_at_cmd_ssapc_read_req(int argc, char *argv[]);
int sle_at_cmd_ssapc_find_structure(int argc, char *argv[]);
int gle_at_cmd_dd_register_cbks(int argc, char *argv[]);
int gle_at_cmd_cm_register_cbks(int argc, char *argv[]);

/* sle sample 数传指令 */
int sle_at_cmd_sample_uuid_client(int argc, char *argv[]);
int sle_at_cmd_sample_start_scan(int argc, char *argv[]);
int sle_at_cmd_uuid_server_init(int argc, char *argv[]);
int sle_at_cmd_uuid_server_send_data_by_uuid(int argc, char *argv[]);
int sle_at_cmd_uuid_server_send_data_by_handle(int argc, char *argv[]);

/* BLE管理 */
int uapi_ble_conn_gap_start_adv(void);
int uapi_ble_conn_gap_stop_adv(void);
int ble_at_cmd_gap_ble_set_adv_param(int argc, char **argv);
int ble_at_cmd_gap_ble_set_adv_data(int argc, char **argv);
int ble_at_cmd_gap_enable_adv(int argc, char **argv);
int ble_at_cmd_gap_disable_adv(int argc, char **argv);
int ble_at_cmd_gap_ble_conn_param_update(int argc, char **argv);
int bt_at_cmd_ble_disconnect(int argc, char *argv[]);
int bt_at_cmd_ble_enter_scan(int argc, char *argv[]);
int bt_at_cmd_ble_connect_peer(int argc, char *argv[]);
int ble_at_cmd_gap_enable_ble(int argc, char *argv[]);
int ble_at_cmd_hid_mouse_init(int argc, char *argv[]);

/* BLE扫描 */
int ble_at_cmd_gap_ble_set_scan_parameters(int argc, char *argv[]);
int ble_at_cmd_gap_start_scan(int argc, char *argv[]);
int ble_at_cmd_gap_stop_scan(int argc, char *argv[]);

/* HID服务 */
int ble_at_cmd_hid_keyboard_init(int argc, char *argv[]);
int ble_at_cmd_hid_keyboard_server_send_input_report(int argc, char *argv[]);
int ble_at_cmd_hid_keyboard_server_send_input_report_by_uuid(int argc, char *argv[]);
int ble_at_cmd_hid_mouse_server_send_input_report(int argc, char *argv[]);
int ble_at_cmd_hid_mouse_server_send_input_report_by_uuid(int argc, char *argv[]);

/* BLE数传 */
int bt_at_cmd_high_mouse_input(int argc, char *argv[]);
int bt_at_cmd_query_paired_device_info(int argc, char *argv[]);
int bt_at_cmd_set_product_type(int argc, char *argv[]);


int ble_at_cmd_gap_ble_get_local_name(int argc, char **argv);
int ble_at_cmd_gap_ble_set_local_name(int argc, char **argv);
int ble_at_cmd_gap_ble_get_local_addr(int argc, char **argv);
int ble_at_cmd_gap_ble_set_local_addr(int argc, char **argv);

/* CHBA */
errcode_t chba_netdev_init(int argc, char *argv[]);
errcode_t sle_chba_main_thread(int argc, char *argv[]);
#endif