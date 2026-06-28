/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd manager at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef UX_CMD_BT_H
#define UX_CMD_BT_H
#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_get_earside(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_tws_role(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_bt_pdinfo_process(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_device_connect_info(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_bt_connect_order(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_combo_connect_order(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_tws_connect_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_connect_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_device_connect_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_bt_device_connect_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_sle_device_connect_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_peer_pairing_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_pairing_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_pairing_state(uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_ble_name(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_name(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_bt_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_ble_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_ble_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_ble_expand_val(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_ble_expand_val(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_earside(uint8_t *data, uint16_t data_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
