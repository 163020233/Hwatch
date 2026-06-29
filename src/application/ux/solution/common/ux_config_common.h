/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_config_common.h
 * Description:  Audio ux common config
 */
#ifndef __UX_CONFIG_COMMON_H__
#define __UX_CONFIG_COMMON_H__

#include "ux_types.h"
#include "audio.h"
#ifndef DEMO_WATCH
#include "bt_service.h"
#include "ux_config.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define DEVICE_NAME_LEN             (64)
#define HA_SEC_WORKKEY_LEN           16

typedef enum {
    LEFT_EAR = 1,
    RIGHT_EAR = 2,
} ear_side_enum;

typedef enum {
    LANGUAGE_CHINESE = 0,
    LANGUAGE_ENGLISH = 1,
    LANGUAGE_BUTT    = 10,
} language_enum;

/* SERVICE_TO_UX_WORK_MODE_MSG */
typedef enum {
    UX_SERVICE_BT_WORK_UNKNOWN,
    UX_SERVICE_BT_WORK_MTWS,
    UX_SERVICE_BT_WORK_SNIFF_TWS,       // 协同模式(蓝牙)
    UX_SERVICE_BT_WORK_MODE_FREEMAN,    // 自由模式
    UX_SERVICE_BT_WORK_DUAL_TRANSMIT,   // 双发模式(星闪)
} ux_service_bt_work_mode;

typedef struct {
    char bt_name[DEVICE_NAME_LEN];                          // device name
    char ble_name[BLE_DEV_NAME_MAX_LEN + BS_STR_END_LEN];   // device ble name
    uint32_t bt_name_time_stamp;                            // device name timestamp
    ear_side_enum ear_side;                                 // LEFT_EAR:1; RIGHT_EAR:2;
    language_enum language;                                 // language_enum 0:zh-CN  1:en-US
    ux_bd_addr bt_addr;                                     // bt addr
    ux_bd_addr sle_addr;                                    // sle addr
    ux_bd_addr ble_addr;                                    // ble addr
    uint8_t dual_connection_switch;                         // ux_api_tws_dual_connection_switch_t
} ux_common_config;

ux_common_config* ux_get_common_config(void);
int32_t ux_common_config_init(void);

errcode_t ux_config_set_language(uint8_t new_language);
int32_t ux_config_set_local_language(uint8_t new_language);
uint8_t ux_config_get_language(void);

uint8_t ux_config_get_dual_connection(void);
void ux_common_read_factory_cfg_bt_name(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
