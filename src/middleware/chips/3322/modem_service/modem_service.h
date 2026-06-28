/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: modem_service
 * Create: 2026
 */

#ifndef MODEM_SERVICE_H
#define MODEM_SERVICE_H

#include "ril_interface.h"

#define AT_CMD_TIMEOUT 5000
#define WIFI_SSID_MAX_LEN 32
#define WIFI_ADDR_LEN 6

typedef enum {
    SIM_STATUS_REMOVED = 0,
    SIM_STATUS_READY,
    SIM_STATUS_PIN_BLOCKED,
    SIM_STATUS_PUK_BLOCKED,
    UNKNOWN_STATUS,
} sim_status_t;

typedef enum {
    REG_STATUS_NOT_REGISTERED = 0,
    REG_STATUS_REGISTERED_HOME,
    REG_STATUS_UNKNOWN,
} network_reg_status_t;

typedef enum {
    FLIGHT_MODE_MINIMUM = 0,
    FLIGHT_MODE_FULL,
    FLIGHT_MODE_PSM,
    FLIGHT_MODE_UNKNOWN,
} flight_mode_status_t;

typedef enum {
    IMS_REG_STATUS_NOT_REGISTERED = 0,
    IMS_REG_STATUS_REGISTERED,
    IMS_REG_STATUS_UNKNOWN,
} ims_reg_status_t;

typedef enum {
    CALL_STATE_ACTIVE = 0,
    CALL_STATE_CALL_ON_HOLD,
    CALL_STATE_OUTGOING_CALL,
    CALL_STATE_RINGTONE,
    CALL_STATE_INCOMING_CALL,
    CALL_STATE_INACTIVE,
} call_state_t;

typedef enum {
    CAT1_REG_STATUS_NOT_REGISTERED = 0,
    CAT1_REG_STATUS_REGISTERED_HOME,
    CAT1_REG_STATUS_SEARCHING,
    CAT1_REG_STATUS_DENIED,
    CAT1_REG_STATUS_UNKNOWN,
    CAT1_REG_STATUS_REGISTERED_ROAMING,
    CAT1_REG_STATUS_EMERGENCY_ONLY,
} cat1_network_reg_status_t;

typedef struct {
    char mcc[4]; // 移动国家码
    char mnc[4]; // 移动网络码
} modem_operator_info;

typedef struct {
    int8_t rssi;
    uint8_t ssid[WIFI_SSID_MAX_LEN + 1];
    uint8_t bssid[WIFI_ADDR_LEN];
    uint8_t channel_num;
} modem_wifi_scan_result_t;

typedef struct {
    uint32_t cur_rate;     // 当前速率 单位 Byte/s
    uint32_t next_rate;    // 预测速率 单位 Byte/s
} modem_net_rate_info_t;

typedef int32_t (*event_callback_t)(uint8_t* data, size_t len);

int32_t uapi_modem_get_sim_status(sim_status_t *status);

int32_t uapi_modem_get_network_reg(network_reg_status_t *status);

int32_t uapi_modem_get_operator_info(modem_operator_info *operator_info);

int32_t uapi_modem_get_signal_info(soc_ue_cell_radio_info *signal_info);

int32_t uapi_modem_get_flight_mode_status(flight_mode_status_t *status);

int32_t uapi_modem_get_utc_time(time_t* utc_time, int32_t *tz_minutes);

int32_t uapi_modem_get_ims_reg_status(ims_reg_status_t *status);

int32_t uapi_modem_get_volte_status(call_state_t *status);

int32_t uapi_modem_get_cat1_network_status(cat1_network_reg_status_t *status);

/* 异步上报接口 */
int32_t uapi_modem_rate_report_start(uint32_t period, event_callback_t callback);

int32_t uapi_modem_rate_report_stop(void);

int32_t uapi_modem_wifi_scan(event_callback_t callback);

#endif