/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: bt_dev_handle.c header file
 * Author: AuthorNameMagicTag
 * Create: 2025-11-18
 */

#ifndef BT_DEV_HANDLE_H
#define BT_DEV_HANDLE_H

#include "diag_service.h"
#include "conn_tlv_err_code.h"
#include "conn_tlv_api.h"
#include "ux_event_define.h"

/* 协议5.9.1 BT使能类型定义 */
typedef enum {
    BT_DEV_ENABLE_START = 0,
    BT_DEV_ENABLE,

    BT_DEV_ENABLE_END
} bt_dev_enable_type_t;

/* 协议5.9.2 射频单音类型定义 */
typedef enum {
    BT_DEV_RF_SINGLETONE_START = 0,
    BT_DEV_RF_SINGLETONE,

    BT_DEV_RF_SINGLETONE_END
} bt_dev_rf_singletone_type_t;

/* 协议5.9.3 射频常收类型定义 */
typedef enum {
    BT_DEV_RF_RX_START = 0,
    BT_DEV_RF_RX,

    BT_DEV_RF_RX_END
} bt_dev_rf_rx_type_t;

/* 协议5.9.4 射频常发类型定义 */
typedef enum {
    BT_DEV_RF_TX_START = 0,
    BT_DEV_RF_TX,

    BT_DEV_RF_TX_END
} bt_dev_rf_tx_type_t;

typedef uint16_t (*bt_dev_proc_fun_cb)(uint8_t *payload, uint16_t len, uint8_t type);

bt_dev_proc_fun_cb *get_bt_dev_enable_map(void);
bt_dev_proc_fun_cb *get_bt_dev_rf_singletone_map(void);
bt_dev_proc_fun_cb *get_bt_dev_rf_rx_map(void);
bt_dev_proc_fun_cb *get_bt_dev_rf_tx_map(void);

tlv_app_op_code_t ux_protocol_bt_dev_parse_process(diag_ser_data_t *msg);

uint16_t bt_dev_btenable(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t bt_dev_rf_singletone(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t bt_dev_rf_rx(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t bt_dev_rf_tx(uint8_t *payload, uint16_t len, uint8_t type);

#endif