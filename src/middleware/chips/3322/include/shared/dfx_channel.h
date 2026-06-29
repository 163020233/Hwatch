/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: dfx uart channel
 * This file should be changed only infrequently and with great care.
 */
#ifndef DFX_CHANNEL_H
#define DFX_CHANNEL_H
#include <stdint.h>
#include "errcode.h"
#include "diag.h"
#include "transmit.h"
#ifndef JS_ENABLE
#include "bts_gatt_server.h"
#else
#include "bs_ble_gatt_server.h"
#include "msg_center_protocol.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


void diag_uart_rx_proc(uint8_t *buffer, uint16_t length);
errcode_t diag_register_channel(void);
errcode_t diag_cmd_uart_config(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_channel_uart_tx(uint8_t *data, uint16_t len);
#ifdef JS_ENABLE
void diag_gatt_write_req_cbk(uint16_t conn_id, uint8_t server_id, bs_gatts_req_write_cb_t *write_cb_para,
    errcode_t status);
void diag_gatt_set_handle(uint8_t server_id, uint16_t conn_id, uint16_t handle, uint8_t inst_id);
void diag_channel_register_bt_gatt_init(void);
#else
void diag_gatt_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status);
void diag_gatt_set_handle(uint8_t server_id, uint16_t conn_id, uint16_t handle);
void diag_channel_register_bt_gatt_init(uint32_t service_uuid);
void diag_channel_register_bt_callback(void);
#endif
void diag_metrics_rssi_notify(int rssi);
void diag_channel_register_br_port_callback(bt_uuid_t *uuid, uint16_t diag_port);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
