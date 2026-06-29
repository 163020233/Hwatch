/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_sle_port.h
 * Description:  provide ux bg sle port header file.
 */
#ifndef UX_BS_SLE_PORT_H
#define UX_BS_SLE_PORT_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_api_communication.h"
#include "sle_ssap_stru.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BS_SLE_PORT_MAX                   2
#define UX_BS_SLE_BIND_DEV_INVALID           0xff
#define UX_BS_SLE_SERVICE_UUID_LEN           16      /* SLE协议栈默认UUID长度 */
#define UX_BS_SLE_STANDARD_SERVICE_UUID_LEN  2      /* SLE协议栈标准服务uuid长度 */
#define UX_BS_MANUFACTURER_ID                0xbe   /* 制造商ID */
#define UX_BS_SLE_PORT_MSG_LEN_MAX           8
#define UX_BS_SLE_CUST_UUID_HIGH             2
#define UX_BS_SLE_CUST_UUID_LOW              1
#define UX_BS_BYTE_NUM                       8
#define UX_BS_SLE_CHANNEL_MAX                6 /* 底层数传总最大通道数 */
#define UX_BS_SLE_PER_PORT_CHANNEL_MAX       3 /* 底层数传每个端口最大通道数 */

typedef enum {
    UX_BS_SLE_PORT_ROLE_NONE = 0X00,
    UX_BS_SLE_PORT_ROLE_LEFT_EAR = 0x01,
    UX_BS_SLE_PORT_ROLE_RIGHT_EAR = 0x02,
    UX_BS_SLE_PORT_ROLE_DONGLE = 0x03,
    UX_BS_SLE_PORT_ROLE_MAX = 0x04,
} ux_bs_sle_port_role_t;

typedef enum {
    UX_BS_SLE_PORT_STATE_NONE = 0X00,
    UX_BS_SLE_PORT_STATE_ENABLE = 0x01,
    UX_BS_SLE_PORT_STATE_DISABLE = 0x02,
    UX_BS_SLE_PORT_STATE_CONNECTING = 0x03,
    UX_BS_SLE_PORT_STATE_CONNECTED = 0x04,
    UX_BS_SLE_PORT_STATE_DISCONNECTING = 0x05,
    UX_BS_SLE_PORT_STATE_DISCONNECTED = 0x06,
    UX_BS_SLE_PORT_STATE_MAX = 0x07,
} ux_bs_sle_port_state_t;

typedef struct {
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t bind_device;
    ux_bs_sle_port_state_t state;
    sle_uuid_t uuid;
} ux_bs_sle_data_port_ctx_t;

/* 数传通道通信协议定义 */
typedef struct {
    ux_api_commu_cmd_t cmd;                     /* 命令类型 */
    uint16_t device_id;                         /* 源设备ID */
    uint8_t param[UX_BS_SLE_PORT_MSG_LEN_MAX];  /* 命令参数 */
    uint16_t param_len;                         /* 命令参数长度 */
    uint16_t command;                           /* 命令 */
    ux_bs_sle_port_role_t port_role;            /* port 角色 */
} ux_bs_sle_data_port_payload_t;

typedef void (*ux_bs_sle_port_data_receive_handler)(uint16_t device_id, uint8_t *data, uint32_t data_len);

typedef enum {
    UX_BS_SLE_PORT_OPT_BY_UUID,  /* 基于UUID操作 */
    UX_BS_SLE_PORT_OPT_BY_PORT,  /* 基于PORT ID操作 */
} ux_bs_sle_port_operate_type;

typedef struct {
    errcode_t (*bs_sle_data_port_enable)(void);
    errcode_t (*bs_sle_data_port_disable)(ux_bs_sle_port_operate_type operate_type);
    errcode_t (*bs_sle_data_port_send_data)(uint16_t device_id, uint8_t *data, uint16_t data_len);
    errcode_t (*bs_sle_data_port_register_receive_handler)(ux_bs_sle_port_data_receive_handler handler);
    errcode_t (*bs_sle_data_port_get_device_list)(uint16_t *device_list, uint8_t list_len, uint8_t *output_len);
    errcode_t (*bs_sle_data_port_connect)(uint16_t device_id);
    errcode_t (*bs_sle_data_port_disconnect)(uint16_t device_id);
} ux_bs_sle_port_wrap_funcs;

errcode_t ux_bs_sle_port_enable(void);
errcode_t ux_bs_sle_port_disable(ux_bs_sle_port_operate_type operate_type);
errcode_t ux_bs_sle_port_send_data(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_bs_sle_port_register_receive_handler(ux_bs_sle_port_data_receive_handler handler);
errcode_t ux_bs_sle_port_connect(uint16_t device_id);
errcode_t ux_bs_sle_port_disconnect(uint16_t device_id);
errcode_t ux_bs_sle_port_get_device_list(uint16_t *device_list, uint8_t list_len, uint8_t *output_len);

void ux_bs_sle_port_set_uuid(sle_uuid_t *uuid_buf, uint16_t service_uuid);
void ux_bs_sle_port_print_uuid(sle_uuid_t *uuid_buf);
void ux_bs_sle_port_register_grant_funcs(ux_bs_sle_port_wrap_funcs *funcs);
void ux_bs_sle_port_register_term_funcs(ux_bs_sle_port_wrap_funcs *funcs);
void ux_bs_sle_set_device_id(uint16_t device_id);
uint16_t ux_bs_sle_get_device_id(void);


typedef enum {
    UX_BS_SLE_PORT_UUID1  = 0x99F1, /* 预留内取1个左/右声道数传 */
    UX_BS_SLE_PORT_UUID2  = 0x99F2, /* 预留内取1个左/右声道数传 */
} ux_bs_sle_service_port_uuid;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UX_BS_SLE_PORT_H */