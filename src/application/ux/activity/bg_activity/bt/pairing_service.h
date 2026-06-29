/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux pairing process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef PAIRING_SERVICE_H
#define PAIRING_SERVICE_H
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 对耳连接失败触发重启默认次数 */
#define DEFAULT_PEER_CONN_RESET                 0

typedef void (*pairing_serivce_fun)(const uint8_t *param, uint16_t length);

typedef struct {
    uint8_t index;
    pairing_serivce_fun fun;
} pairing_serivce_map_t;

typedef enum {
    BT_ENTER_STANDBY_LOCAL, // 本耳进入standby
    BT_ENTER_STANDBY_TWS,   // 对耳同步进入standby
    BT_ENTER_STANDBY_BUTT
}bt_enter_standby_type;

errcode_t ux_set_peer_conn_reset_num(int32_t peer_conn_reset_num);
void device_peer_conn_start(void);
errcode_t pairing_start_connect(uint32_t time);
errcode_t pairing_stop_connect(void);
void pairing_service_execute(uint8_t index, uint8_t *param, uint16_t length);
void pairing_timer_execute(uint8_t timer_id, uint8_t *param, uint16_t length);
void pairing_timer_set_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
