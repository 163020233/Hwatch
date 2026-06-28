/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: query_dev_handle.c header file
 * Author: AuthorNameMagicTag
 * Create: 2025-10-13
 */

#ifndef QUERY_DEV_HANDLE_H
#define QUERY_DEV_HANDLE_H

#include "diag_service.h"
#include "conn_tlv_err_code.h"
#include "conn_tlv_api.h"
#include "ux_event_define.h"

/* 协议5.1.1 连接参数查询类型定义 */
typedef enum {
    QUERY_DEV_CONN_START = 0,
    QUERY_DEV_CONN_PROTOCAL_VER,
    QUERY_DEV_CONN_SN,
    QUERY_DEV_CONN_BT_MAC,
    QUERY_DEV_CONN_EARPHONE_ROLE = 7,
    QUERY_DEV_CONN_EAR_SIDE = 8,
    QUERY_DEV_CONN_BLE_MAC = 9,
    QUERY_DEV_CONN_BT_NAME = 10,
    QUERY_DEV_CONN_BLE_NAME = 11,

    QUERY_DEV_CONN_END
} query_dev_conn_param_type_t;

/* 协议5.1.2 设备电池电量查询类型定义 */
typedef enum {
    QUERY_DEV_BATTERY_START = 0,
    QUERY_DEV_BATTERY_PERCENT,

    QUERY_DEV_BATTERY_END
} query_dev_battery_type_t;

/* 协议5.1.4 耳机佩戴状态查询类型定义 */
typedef enum {
    QUERY_DEV_WEARING_START = 0,
    QUERY_DEV_WEARING_STATUS,
    QUERY_DEV_CONTROL_WEAR_STATUS,
    
    QUERY_DEV_WEARING_END
} query_dev_wearing_type_t;

/* 协议5.1.5 设备音量查询类型定义 */
typedef enum {
    QUERY_DEV_VOLUME_START = 0,
    QUERY_DEV_HEARINGAID_VOLUME = 1,
    QUERY_DEV_MUSIC_VOLUME,

    QUERY_DEV_VOLUME_END
} query_dev_volume_type_t;

typedef enum {
    QUERY_DEV_SDKVERSION_START = 0,
    QUERY_DEV_SDK_VERSION = 1,

    QUERY_DEV_SDKVERSION_END
} query_dev_version_type_t;

typedef enum {
    QUERY_ANC_PARAM_START = 0,
    QUERY_ANC_PARAM,
    QUERY_ANC_FB_GAIN,
    QUERY_ANC_FF0_GAIN,

    QUERY_ANC_PARAM_END
} query_anc_param_type_t;

typedef enum {
    QUERY_DEV_SECTOR_INFO_START = 0,
    QUERY_DEV_SECTOR_INFO_END
} query_dev_sector_info_type_t;

typedef struct {
    uint32_t addr;
    uint32_t size;
} dev_sector_param_t;


typedef uint16_t (*query_dev_proc_fun_cb)(uint8_t *payload, uint16_t len, uint8_t type);

query_dev_proc_fun_cb *get_query_dev_conn_param_map(void);
query_dev_proc_fun_cb *get_query_dev_battery_map(void);
query_dev_proc_fun_cb *get_query_dev_wearing_map(void);
query_dev_proc_fun_cb *get_query_dev_volume_map(void);
query_dev_proc_fun_cb *get_query_dev_sdkversion_map(void);
query_dev_proc_fun_cb *get_query_anc_param_map(void);
query_dev_proc_fun_cb *get_query_dev_sector_info_map(void);

/* 5.1 */
uint16_t query_dev_query_earphone_role(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_battery_percent(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_wearing_status(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_music_volume(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_bt_addr(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_ble_addr(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_bt_name(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_ble_name(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_ear_side(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_sdk_version(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_anc_param(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_fb_gain(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_ff0_gain(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_hearingaid_volume(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t query_dev_query_sector_info(uint8_t *payload, uint16_t len, uint8_t type);

tlv_app_op_code_t ux_protocol_query_dev_parse_process(diag_ser_data_t *msg);

#endif
