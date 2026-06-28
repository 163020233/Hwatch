/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux watch bg  header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-17， Create file. \n
 */
#ifndef UX_BT_H
#define UX_BT_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "ux_device_manager.h"
#include "ux_api_bt.h"
#include "bs_device_mgr.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define PROTOCOL_MAX_CONNECT_NUM 2
#define BT_ADDR_LEN 6
#define MIN_SCAN_ID 100
#define UX_BT_CONNECTION_UPPER_LIMIT 4

typedef enum {
    UX_BT_STACK_DEINITED,
    UX_BT_STACK_DEINITING,
    UX_BT_STACK_INITING,
    UX_BT_STACK_INITIED,
    UX_BT_STACK_MAX,
} ux_bt_stack_state_t;

typedef enum {
    UX_BT_DISCOVERY_START = 0,
    UX_BT_DISCOVERY_STOP,
    UX_BT_DISCOVERY_MAX,
} ux_bt_discovery_state_t;

typedef struct {
    ux_bt_stack_state_t stack_status;
    ux_bt_discovery_state_t scan_status;
    ux_bt_discovery_state_t adv_status;
} ux_bt_stack_info_t;

/* 处理activity事件时使用 */
typedef struct {
    ux_api_bt_role_t role;
    ux_api_bt_protocol_t protocol;
    ux_api_bt_addr_t addr;
    uint8_t id;
} ux_bg_event_config_t;

errcode_t ux_bt_get_addr_by_device_id(uint16_t dev_id, ux_api_bt_addr_t *addr);
uint16_t ux_bt_find_device_id_by_addr(ux_api_bt_protocol_t protocol, const ux_api_bt_addr_t *addr);
void ux_bt_insert_device_with_rssi_filter(ux_bg_device_list_t *device_list, ux_bg_device_info_t *new_device);
void ux_bt_update_cur_device_info_from_node(const ux_bg_device_info_t *device_node);
void ux_bt_clear_cur_device_info(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);
ux_api_bt_device_info_t* ux_bt_get_device_info_writable(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol,
    uint16_t *device_num);
bool ux_bt_active_device_is_full(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol);
void ux_bt_switch_multiple_devices(bool enable);
bool ux_bt_is_multiple_device_open(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
