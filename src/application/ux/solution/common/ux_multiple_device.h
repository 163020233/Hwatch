/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2021. All rights reserved.
 * File          ux_multiple_device.h
 * Description:  Audio ux multiple device interface
 */
#ifndef UX_MULTIPLE_DEVICE_H
#define UX_MULTIPLE_DEVICE_H

#include "errcode.h"
#include "ux_bt_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct multiple_dev_info_node {
    struct multiple_dev_info_node *prev;
    struct multiple_dev_info_node *next;
    bd_addr connect_addr;
    bool is_has_ble_addr;
    uint8_t protocol;
    bd_addr ble_addr;
    ux_bt_common_dev_state state;
} ux_multiple_dev_info_node;

typedef struct {
    uint8_t main_state;
    uint8_t node_length;
    uint8_t incomming_switch_flag;
    bd_addr main_connect_addr;
    bd_addr pre_main_connect_addr;
    ux_multiple_dev_info_node *head_node;
} ux_multiple_dev_info_node_head;

uint8_t ux_multiple_get_main_state(void);
void ux_multiple_set_main_state(uint8_t state);
uint8_t ux_bt_get_mobile_connect_state(void);

ux_multiple_dev_info_node* ux_multiple_find_node(uint8_t *addr);
uint8_t ux_multiple_next_state(uint8_t *para, uint8_t State);
errcode_t ux_multiple_get_main_state_addr(uint8_t *out_addr, uint16_t addr_len);
uint8_t ux_multiple_get_main_state_protocol(void);
errcode_t ux_multiple_get_other_device_addr(uint8_t *out_addr, uint16_t addr_len);
uint8_t ux_multiple_get_connect_num(void);
uint8_t ux_multiple_get_connect_num_by_protocol(uint8_t protocol);
int32_t ux_multiple_insert_new_node(service_to_ux_mobile_connect_state_msg_param_s *parameter);
void ux_multiple_clear_all_nodes(void);
void ux_multiple_update_work_mode(uint8_t *para);
void ux_multiple_switch_active_dev(uint8_t *addr, uint16_t addr_len);
ux_bt_common_dev_state* ux_multiple_get_common_state(uint8_t *para);
bool ux_multiple_node_exists(const service_to_ux_mobile_connect_state_msg_param_s *param);
int32_t ux_multiple_get_mobile_addr_by_state(uint8_t *out_addr, uint16_t addr_len, uint8_t state);
void ux_multiple_update_tws_work_mode(void *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
