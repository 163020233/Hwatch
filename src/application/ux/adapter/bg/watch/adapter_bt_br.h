/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux BG service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-20， Create file. \n
 */
#ifndef ADAPTER_BT_BR_H_
#define ADAPTER_BT_BR_H_
#include "bs_br_service_api.h"
#include "bs_ble_service_api.h"
#include "bs_br_call.h"
#include "bts_def.h"
#include "ux_api_bt_control.h"
#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_BT_BR_CONNECTED,
    UX_BT_BR_DISCONNECTED,
    UX_BT_BLE_CONNECTED,
    UX_BT_BLE_DISCONNECTED,
} ux_bt_connect_state_t;

typedef struct {
    unsigned char deviceName[BS_BR_DEVICE_NAME_MAX_LEN]; /* Name of the device record, must be in UTF-8 */
    unsigned char addr[BS_ADDR_LEN];
    uint32_t device_class; // 设备类型
    int8_t rssi; // 蓝牙信号强弱
    bool need_to_delete; // 若需要上层删除设备，为true
} ux_bt_scan_msg_t;

typedef struct {
    unsigned char deviceName[BS_BR_DEVICE_NAME_MAX_LEN]; /* Name of the device record, must be in UTF-8 */
    ux_api_bt_addr_t addr;
    uint32_t device_class; // 设备类型
    uint8_t state;
    uint32_t reason;
} ux_bt_connect_msg_t;

typedef struct {
    bd_addr_t bd_addr;
    uint8_t hfp_state;
} ux_bt_hfp_msg_t;

typedef struct {
    bd_addr_t bd_addr;
    uint8_t sco_state;
} ux_bt_sco_msg_t;

typedef struct {
    bd_addr_t bd_addr;
    bs_br_call_state_t state;
    bool outgoing;
    uint8_t number_len;
    uint8_t number[BR_CALL_PHONENUM_MAX_DIGITS];
} ux_bt_call_msg_t;

typedef struct {
    bd_addr_t bd_addr;
    uint8_t a2dp_state;
} ux_bt_a2dp_msg_t;

typedef struct {
    bd_addr_t bd_addr;
    ux_api_bt_control_play_t play_control;
} ux_bt_avrcp_play_control_msg_t;

void ux_adapter_bt_scan_complete_process(const bs_br_discovery_result_t *scan_result, bool need_to_delete);
void ux_adapter_bt_connect_process(int8_t *name, ux_api_bt_addr_t *addr, uint32_t reason,
    uint8_t state, uint32_t device_class);
void ux_adapter_bt_disconnect_process(int8_t *name, ux_api_bt_addr_t *addr, uint32_t reason,
    uint8_t state, uint32_t device_class);
void ux_adapter_bt_enable_process(bs_protocol_type_t protocol);
void ux_adapter_bt_call_state_process(bd_addr_t *bd_addr, uint8_t call_state, char* number,
    uint8_t number_len, bool outgoing);
void ux_adapter_bt_sco_state_process(bd_addr_t *bd_addr, uint8_t sco_state);
void ux_adapter_bt_hfp_state_process(bd_addr_t *bd_addr, uint8_t hfp_state);
void ux_adapter_bt_a2dp_src_state_process(ux_api_bt_addr_t *bd_addr, uint8_t a2dp_state);
void ux_adapter_bt_avrcp_tg_play_control_process(ux_api_bt_addr_t *bd_addr, ux_api_bt_control_play_t play_control);
void ux_adapter_bt_avrcp_tg_volume_control_process(bd_addr_t *bd_addr, uint8_t avrcp_volume);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif