/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "bg_gap_service.h"
#include "ux_common_log.h"
#include "securec.h"
#include "ux_event_define.h"
#include "ux_bt_pbap.h"
#include "ux_api_bt.h"
/*
 * 蓝牙中的GAP（通用访问配置文件，Generic Access Profile）是蓝牙技术的核心规范，
 * 用于定义设备如何发现、连接、建立安全连接以及与其它设备进行交互。
 * GAP规定了蓝牙设备的共性功能，并对设备进行角色划分（如中央设备、外围设备等），
 * 以实现设备间的基础通信能力，是所有蓝牙应用的基石。
 */

void bg_gap_service_enable(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bg_event_config_t)) {
        ux_log_err("[ux][bg_gap_service_enable] Null ptr!\n");
        return;
    }

    ux_bg_event_config_t *enable_cfg = (ux_bg_event_config_t*)param;
    errcode_t ret = UX_ERR_SUCCESS;
    ret = ux_api_bt_enable(enable_cfg->protocol);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[bg_response_power_on] Fail to init bt stack, protocol:%d, ret:0x%x!\n", enable_cfg->protocol, ret);
        return;
    }
}

void bg_gap_service_disable(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bg_event_config_t)) {
        ux_log_err("[ux][bg_gap_service_disable] Null ptr!\n");
        return;
    }

    ux_bg_event_config_t *enable_cfg = (ux_bg_event_config_t*)param;
    errcode_t ret = ux_api_bt_disable(enable_cfg->protocol);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[bg_gap_service_disable] Fail to deinit bt stack 0x%x!\n", ret);
        return;
    }
}

void bg_gap_service_disable_cb(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_api_bt_protocol_t)) {
        ux_log_err("[ux][bg_gap_service_disable_cb] invalid param, len=%d!\n", param_len);
        return;
    }
    ux_api_bt_protocol_t protocol = *(ux_api_bt_protocol_t*)param;
    ux_log_info("[ux][bg_gap_service_disable_cb] protocol=%d.\n", protocol);
    if (protocol == UX_API_BT_PROTOCOL_BR) {
#ifdef CONFIG_SUPPORT_PBAP
        ux_bt_pbap_deinit();
#endif
    }
}

void bg_gap_service_start_discovery(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bg_event_config_t)) {
        ux_log_err("[ux][bg_gap_service_start_discovery] null point!\n");
        return;
    }
    ux_bg_event_config_t *discovery_cfg = (ux_bg_event_config_t*)param;
    errcode_t ret = UX_ERR_INVALID_PARAMETER;
    ret = ux_api_bt_start_discovery(discovery_cfg->role, discovery_cfg->protocol);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bg_gap_service_start_discovery] Fail to start discovery 0x%x.", ret);
        return;
    }
    return;
}

void bg_gap_service_stop_discovery(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bg_event_config_t)) {
        ux_log_err("[ux][bg_gap_service_stop_discovery] null point!\n");
        return;
    }
    ux_bg_event_config_t *discovery_cfg = (ux_bg_event_config_t*)param;
    errcode_t ret = ux_api_bt_stop_discovery(discovery_cfg->role, discovery_cfg->protocol);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bg_gap_service_stop_discovery] Fail to stop discovery 0x%x.", ret);
        return;
    }
    return;
}

void bg_gap_service_start_pair(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(uint16_t)) {
        ux_log_err("[bg_gap_service_start_pair] Nullptr.\n");
        return;
    }
    uint16_t *dev_id = (uint16_t*)param;
    errcode_t ret = UX_ERR_FAILURE;
    ux_bg_device_info_t *scan_info = ux_bg_find_device_info_by_id(ux_bg_get_scan_device_list(), *dev_id);
    if (scan_info != NULL) {
        /* 扫描列表有设备, 尝试配对 */
        ret = ux_api_bt_pair_device(&scan_info->addr, scan_info->protocol);
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux][bg_gap_service_start_pair] pair fail ret:0x%x.\n", ret);
            return;
        }
    } else {
        /* 扫描列表无设备，尝试回连 */
        ux_bg_device_info_t *pair_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), *dev_id);
        if (pair_info == NULL) {
            ux_log_err("[bg_gap_service_start_pair] no match device.\n");
            return;
        }
        ret = ux_api_bt_pair_device(&pair_info->addr, pair_info->protocol);
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux][bg_gap_service_start_pair] reconnect fail ret:0x%x.\n", ret);
            return;
        }
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[bg_gap_service_start_pair] Fail to start pair.\n");
        return;
    }
    return;
}

void bg_gap_service_disconnect(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(uint16_t)) {
        ux_log_err("[bg_gap_service_disconnect] invalid param.\n");
        return;
    }

    uint16_t *dev_id = (uint16_t*)param;
    ux_bg_device_info_t *pair_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), *dev_id);
    if (pair_info == NULL) {
        ux_log_err("[bg_gap_service_start_pair] no match device.\n");
        return;
    }
    errcode_t err = UX_ERR_SUCCESS;
    if (pair_info->protocol == UX_API_BT_PROTOCOL_BR) {
        err = ux_api_bt_disconnect(UX_API_BT_PROFILE_BR, &pair_info->addr);
    } else if (pair_info->protocol == UX_API_BT_PROTOCOL_BLE) {
        err = ux_api_bt_disconnect(UX_API_BT_PROFILE_BLE, &pair_info->addr);
    } else {
        err = UX_ERR_INVALID_PARAMETER;
    }
    /* 调用solution接口，进行断连 */
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_gap_service_disconnect] ux_api_bt_disconnect failed 0x%x.\n", err);
        return;
    }
    return;
}

void bg_gap_service_remove_device(uint8_t *param, uint16_t param_len)
{
    if (param_len != sizeof(uint16_t) || param == NULL) {
        ux_log_err("[bg_gap_service_remove_device] invalid param %d.\n", param_len);
        return;
    }
    uint16_t *dev_id = (uint16_t*)param;
    ux_bg_device_info_t *pair_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), *dev_id);
    if (pair_info == NULL) {
        ux_log_err("[bg_gap_service_start_pair] no match device.\n");
        return;
    }
    errcode_t ret = ux_api_bt_remove_device(&pair_info->addr, pair_info->protocol);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[bg_gap_service_remove_device] Fail to remove device info 0x%x.", ret);
        return;
    }
    return;
}