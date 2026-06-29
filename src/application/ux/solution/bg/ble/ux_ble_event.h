/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * File          ux_ble_event.h
 * Description:  ux ble event callback process
 */

#ifndef UX_BLE_EVENT_H
#define UX_BLE_EVENT_H
#include <stdint.h>
#include "errcode.h"
#include "ux_common_define.h"
#include "bt_service.h"
#include "bs_api_stru.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define BLE_DISCONNECT_BY_HANDOVER               0xFE    /* 表示BLE因主切副断连 */

typedef struct {
    uint16_t dev_id;
    uint8_t conn_state;
    uint8_t encrypt_state;
    uint8_t reserved;
    uint32_t reason;
} bs_ble_connect_state_msg_param_t;

typedef struct {
    uint8_t acl_state;
    uint8_t reasoncode;
    uint16_t reserved2;
} bs_ble_stack_state_msg_param_t;

typedef struct {
    uint8_t handover_state;    /* bs_ble_acl_switch_state_t */
    uint8_t reserved[UX_RESERVED_LEN_THR];
} bs_ble_handover_state_msg_param_t;

/* 处理BLE上报的事件消息
 * @param[in] msg_id 事件消息ID
 * @param[in] param 事件参数指针（类型需与msg_id对应）
 * @return 处理结果错误码
 */
errcode_t ux_bs_ble_event_callback(service_to_ux_msg msg_id, void *param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

