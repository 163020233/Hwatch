/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: ux hearing tlv api
 * Author: AuthorNameMagicTag
 * Create: 2023-02-24
 */

#ifndef HAID_TLV_H
#define HAID_TLV_H

#include "stdint.h"
#include "stdbool.h"
#include "conn_tlv_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 协议5.3.1 type类型定义 */
typedef enum {
    HAID_FIT_START_STOP_TYPE = 1,
} haid_fit_start_stop_type_t;

/* 协议5.3.2 type类型定义 */
typedef enum {
    HAID_FIT_GET_NOISE_TYPE = 1,
} haid_fit_get_noise_type_t;

/* 协议5.3.3 type类型定义 */
typedef enum {
    HAID_FIT_LEFT_EAR_FERQ_TYPE = 1,
    HAID_FIT_RIGHT_EAR_FERQ_TYPE,
    HAID_FIT_VOL_TYPE,
    HAID_FIT_TEST_TIME_TYPE,
    HAID_FIT_HL_LEVEL_TYPE
} haid_fit_chn_type_t;

/* 协议5.3.4 type类型定义 */
typedef enum {
    HAID_FIT_CHANGE_MODE_TYPE = 1,
    HAID_FIT_ENABLE_MODE_TYPE,
} haid_fit_change_mode_type_t;

/* 协议5.3.8 type类型定义 */
typedef enum {
    HAID_FIT_AUTO_DETECT_TYPE = 1,
} haid_fit_auto_detect_type_t;

/* 协议5.3.9 type类型定义 */
typedef enum {
    HAID_FIT_RARAM_SAVE_TYPE = 1,
} haid_fit_save_param_type_t;

/* 协议5.3.10 type类型定义 */
typedef enum {
    HAID_FIT_SET_MAX_VOL_RANGE_TYPE = 1,
    HAID_FIT_SET_DEFAULT_VOL_RANGE_TYPE = 2,
    HAID_FIT_SET_VOL_LIST_TYPE = 3
} haid_fit_set_mode_vol_type_t;

/* 协议5.3.12 type类型定义 */
typedef enum {
    HAID_FIT_MODE_COUNT_TYPE = 1,
    HAID_FIT_MODE_ORDER_TYPE = 2,
    HAID_FIT_MODE_TIME_SUM_TYPE = 3
} haid_fit_mode_statistic_type_t;

/* 协议5.3.13 type类型定义 */
typedef enum {
    HAID_FIT_DEV_CFG_SWITCH_TYPE = 1
} haid_fit_dev_cfg_switch_type_t;

/* 协议5.3.14 type类型定义 */
typedef enum {
    HAID_FIT_MODE_SAVE_SWITCH_TYPE = 1,
    HAID_FIT_VOL_SAVE_SWITCH_TYPE = 2
} haid_fit_mode_vol_save_type_t;

/* 协议5.3.15 type类型定义 */
typedef enum {
    HAID_FIT_RPT_PHONE_STATE_TYPE = 1,
} haid_fit_rpt_phone_state_type_t;

/* 协议5.3.16 type类型定义 */
typedef enum {
    HAID_FIT_QUERY_MODE_TYPE = 1,
    HAID_QUERY_MIX_MODE_TYPE = 2,
    HAID_SET_A2DP_ENABLE_TYPE = 3,
    HAID_SET_HFP_ENABLE_TYPE = 4,
    HAID_SET_SCENE_ENABLE_TYPE = 5,
    HAID_QUERY_MAX_VOL_RANGE_TYPE = 6,
    HAID_QUERY_DEFAULT_VOL_RANGE_TYPE = 7,
    HAID_QUERY_VOL_LIST_TYPE = 8,
    HAID_QUERY_HAID_ENABLE_TYPE = 9,
    HAID_QUERY_AFC_CALIBRATE_ENABLE_TYPE = 10,

    HAID_BUSINESS_QUERY_TYPE_END
} haid_fit_query_mode_type_t;

/* 协议5.3.17 type类型定义 */
typedef enum {
    HAID_FIT_A2DP_WDRC_TYPE = 1,
    HAID_FIT_HFP_WDRC_TYPE = 2
} haid_fit_wdrc_switch_type_t;

int32_t haid_tlv_decode_bytes_to_nodes(diag_ser_data_t *msg, tlv_node_list_t **node_list);
bool haid_tlv_mode_order_statistic(diag_ser_header_t *header);
bool haid_tlv_encode_sum_statistic(diag_ser_header_t *header);
bool haid_tlv_encode_rpt_phone_state(uint8_t state);
bool haid_tlv_encode_noise(diag_ser_header_t *header, int32_t noise);

uint16_t fill_haid_mode(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_mix_mode(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_a2dp_enable_mode(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_hfp_enable_mode(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_scene_mode(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_vol_max_range(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_vol_default_range(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_vol_list(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_haid_enable(uint8_t *payload, uint16_t len, uint8_t type);
uint16_t fill_haid_afc_calibrate_status(uint8_t *payload, uint16_t len, uint8_t type);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
