/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: ux spp service
 */

#ifndef UX_SPP_SERVER_H
#define UX_SPP_SERVER_H

#include "errcode.h"

#define SPP_SERVER_UUID_2_BYTE_LEN    2
#define SPP_SERVER_UUID_16_BYTE_LEN   16
#define SPP_SERVER_NAME_LEN           15
#define SPP_SERVER_INVALID_CHANNEL    0
#define SPP_SERVER_INVALID_DEV_ID     0
#define SPP_SERVER_INVALID_PORT       0

/* 服务状态 */
typedef enum {
    UX_SPP_SERVICE_STATUS_DISABLED,            /* port profile服务未使能 */
    UX_SPP_SERVICE_STATUS_STARTING,            /* port profile服务注册中 */
    UX_SPP_SERVICE_STATUS_STARTED,             /* port profile服务已注册 */
} ux_spp_service_status;

/* server端port生成方式 */
typedef enum {
    UX_SPP_SERVICE_AUTO_GENERATE = 0,        /* 自动生成 */
    UX_SPP_SERVICE_SPECIFY_GENERATE,         /* 指定为固定port号 */
    UX_SPP_SERVICE_GENERATE_MAX,
} ux_spp_server_create_type;

typedef enum {
    UX_SPP_CONNECTING_STATE = 0x01,    /*!< @if Eng status of connecting. @else 正在连接。 @endif */
    UX_SPP_CONNECTED_STATE = 0x02,     /*!< @if Eng status of connected. @else 已连接。 @endif */
    UX_SPP_DISCONNECTING_STATE = 0x03, /*!< @if Eng status of disconnecting. @else 正在断连。 @endif */
    UX_SPP_DISCONNECTED_STATE = 0x04   /*!< @if Eng status of disconnected. @else 已断连。 @endif */
} ux_spp_connect_state_t;

errcode_t ux_spp_service_disconnect(void);

errcode_t ux_spp_service_destroy(void);

errcode_t ux_spp_service_init(void);

errcode_t ux_spp_service_deinit(void);

uint8_t get_ux_spp_service_enable_state(void);            /* 获取spp服务基础状态, 是否已启动服务 */
bool get_ux_spp_service_connect_state(uint16_t port);  /* 获取spp服务端口号 */
uint16_t get_ux_spp_port_number(void);
errcode_t spp_service_send_data(uint8_t *data, uint16_t data_len);
uint16_t get_diag_spp_port_number(void);
#endif /* UX_SPP_SERVER_H */
