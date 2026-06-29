/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_property.h
 * Description:  provide ux bs property header file.
 */
#ifndef UX_BS_PROPERTY_H
#define UX_BS_PROPERTY_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_errcode.h"
#include "ux_bg_config.h"
#include "ux_api_ble.h"
#include "ux_types.h"
#ifndef DEMO_WATCH
#include "key_struct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
errcode_t ux_bs_set_protocol(const ux_work_protocol_t protocol);
errcode_t ux_bs_get_protocol(ux_work_protocol_t *protocol);
#ifndef DEMO_WATCH
errcode_t ux_bs_set_role(const ux_device_role_t role);
errcode_t ux_bs_get_role(ux_device_role_t *role);

errcode_t ux_bs_set_local_addr(const ux_work_protocol_t protocol, const uint8_t *addr, uint16_t addr_len);
#endif
errcode_t ux_bs_set_addr(uint16_t key, bool verify, uint8_t *data, uint16_t data_len);
errcode_t ux_bs_get_addr(uint16_t key, uint8_t *addr, uint16_t addr_len);
void ux_bs_reversal_addr(uint8_t *addr);
errcode_t ux_bs_get_local_addr(const ux_work_protocol_t protocol, uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_set_peer_addr(const ux_work_protocol_t protocol, const uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_get_peer_addr(const ux_work_protocol_t protocol, uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_read_local_addr(uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_read_peer_addr(uint8_t *addr, uint16_t addr_len);

errcode_t ux_bs_set_ble_addr(const uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_get_ble_addr(uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_set_ble_peer_addr(const uint8_t *addr, uint16_t addr_len);
errcode_t ux_bs_get_ble_peer_addr(uint8_t *addr, uint16_t addr_len);

errcode_t ux_bs_set_sle_cds_key(const uint8_t *data, uint16_t data_len);

/*
 * 1. param 'len' must be equal to DEVICE_NAME_LEN.
 * 2. time_stamp can be set to NULL, if you don't care about it.
 * 3. once the interface return SUCCESS, 'deviceName' must be a string ending with '\0'.
 */
errcode_t ux_bs_set_device_name(uint8_t *device_name, uint16_t len);
errcode_t ux_bs_get_device_name(char *device_name, uint16_t len, uint32_t *timestamp);
errcode_t ux_bs_bt_set_device_ble_name(uint8_t *device_name, uint16_t len);
errcode_t ux_bs_bt_get_device_ble_name(char *device_name, uint16_t len);
errcode_t ux_bs_bt_set_ble_expand_val(uint8_t *data, uint16_t data_len);
errcode_t ux_bs_bt_get_ble_expand_val(uint8_t *data, uint16_t *data_len);

bool ux_bs_addr_is_invalid(const ux_bd_addr addr, uint16_t len, uint16_t key);
errcode_t ux_bs_reset_peer_addr(ux_work_protocol_t protocol);

#ifndef DEMO_WATCH
errcode_t ux_bs_bt_read_ble_expand_val(ux_api_ble_expand_data_t *expand_val);
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_PROPERTY_H