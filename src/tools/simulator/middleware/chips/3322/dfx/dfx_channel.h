/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: simulator module empty implement
 * Create: 2025-07-25
 */
 
#ifndef DFX_CHANNEL_H
#define DFX_CHANNEL_H
#include <stdint.h>
#include "errcode.h"
#include "bts_gatt_server.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


typedef struct {
    uint8_t peer_addr;        /*!< @if Eng peer addr.
                                   @else   地址 @endif */
    uint8_t pad[3];             /*!< @if Eng pad.
                                     @else   预留字段 @endif */
} diag_option_t;

void diag_uart_rx_proc(uint8_t *buffer, uint16_t length);
errcode_t diag_register_channel(void);
errcode_t diag_cmd_uart_config(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
errcode_t diag_cmd_set_offline_log(uint16_t cmd_id, void *cmd_param, uint16_t cmd_param_size, diag_option_t *option);
void diag_gatt_write_req_cbk(uint8_t server_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
    errcode_t status);
void diag_gatt_set_handle(uint8_t server_id, uint16_t conn_id, uint16_t handle);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif