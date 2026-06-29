/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: factory test at cmd \n
 *
 */

#ifndef BT_FT_ATCMD_H
#define BT_FT_ATCMD_H

#include <errcode.h>
#include <feature_config.h>
#include "bts_def.h"
#include "at.h"

#if defined(CONFIG_SUPPORT_FACTORY_TEST_SLE) && CONFIG_SUPPORT_FACTORY_TEST_SLE == 1
typedef struct {
    uint32_t para_map;
    int32_t tx_freq;
    int32_t tx_power;
    int32_t test_data_len;
    int32_t pk_payload_type;
    int32_t tx_phy;
    int32_t tx_format;
    int32_t tx_rate;
    int32_t tx_pilot_ratio;
    int32_t tx_polar_r;
    int32_t tx_interval;
} sle_rf_tx_param_args_t;

typedef struct {
    uint32_t para_map;
    int32_t rx_freq;
    int32_t rx_phy;
    int32_t rx_format;
    int32_t rx_pilot_ratio;
    int32_t rx_interval;
} sle_rf_rx_param_args_t;

typedef struct {
    uint32_t para_map;
    int32_t rf_freq;
    int32_t rf_mode;
} ble_rf_st_param_args_t; // BLE单音

typedef struct {
    uint32_t para_map;
    int32_t open;
} bt_sigle_test_param_args_t;

typedef struct {
    uint32_t para_map;
    uint8_t freq_idx;
    uint8_t test_data_len;
    uint8_t payload_type;
    uint8_t phy;
    uint8_t cte_len;
    uint8_t cte_type;
    uint8_t switch_pattern_len;
    uint8_t *ant_ids;
    int8_t power_level;
} ble_rf_tx_v2_param_args_t; // BLE TX V2

/* 以下为BR产测 */
typedef struct {
    uint32_t para_map;
    uint16_t test_data_len;
    const uint8_t *addr;
    uint8_t tx_freq;
    uint8_t module_type;
    uint8_t code_type;
    uint8_t payload_type;
    uint8_t power_level;
    uint8_t tx_channel;
} br_rf_tx_param_args_t;

typedef struct {
    uint32_t para_map;
    uint16_t test_data_len;
    const uint8_t *addr;
    uint8_t rx_freq;
    uint8_t module_type;
    uint8_t code_type;
    uint8_t rx_channel;
} br_rf_rx_param_args_t;
/* BR产测end */

errcode_t bt_factory_open_single_test(void);    // 在include仓的头文件中增加接口后删除
errcode_t bt_factory_close_single_test(void);    // 在include仓的头文件中增加接口后删除
errcode_t bt_ft_atcmd_init(void);
at_ret_t bt_at_br_register_callback_cmd(void);
#endif
#endif
