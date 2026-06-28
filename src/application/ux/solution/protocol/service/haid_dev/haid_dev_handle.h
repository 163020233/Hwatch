/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: dev_haid_handle.c header file
 * Author: AuthorNameMagicTag
 * Create: 2023-05-29
 */

#ifndef HAID_DEV_HANDLE_H
#define HAID_DEV_HANDLE_H

#include "stdint.h"
#include "diag_service.h"
#include "conn_tlv_api.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/* 协议5.3.5 type类型定义 */
typedef enum {
    HAID_FIT_MIXED_MODE_TYPE = 1,
} haid_fit_mixed_audio_type_t;

/* 协议5.3.6 type类型定义 */
typedef enum {
    HAID_FIT_BT_TO_AID_TIME_DELAY_TYPE = 1,
} haid_fit_bt_to_aid_delay_type_t;

/* 协议5.3.7 type类型定义 */
typedef enum {
    HAID_FIT_POWER_ON_TIME_DELAY_TYPE = 1,
} haid_fit_power_on_delay_type_t;

/* 协议5.3.18 type类型定义 */
typedef enum {
    HAID_FIT_SET_MEDICAL_TEST_TYPE = 1,
    HAID_FIT_GET_MEDICAL_TEST_TYPE = 2,
} haid_fit_medical_test_type_t;

/* 协议5.3.19 type类型定义 */
typedef enum {
    HAID_FIT_SET_HAID_ENABLE_TYPE = 1,
    HAID_SET_AFC_CALIBRATION_TYPE = 2,
} haid_fit_haid_enable_type_t;

/* 协议5.3.20 type类型定义 */
typedef enum {
    HAID_FIT_AFC_CALIBRATION_RPT_TYPE = 1,
} haid_fit_haid_afc_type_t;

bool haid_tlv_encode_content_req(diag_ser_header_t *header, uint16_t tlv_len, tlv_node_list_t *node, uint8_t cmd_id);
bool dev_haid_set_medical_mode(uint8_t mode);
bool dev_haid_set_haid_enable_state(uint8_t state);
/* 啸叫校准状态上报处理函数 */
bool haid_afc_calibrate_rpt_proc(uint8_t state);
void dev_haid_msg_handle_register(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
