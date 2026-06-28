/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: hearing service message handle
 * Author: AuthorNameMagicTag
 * Create: 2023-02-24
 */

#ifndef HAID_SERVICE_HANDLE_H
#define HAID_SERVICE_HANDLE_H

#include "stdint.h"
#include "diag_service.h"
#include "haid_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SYNC_TYPE_OFFSET 0  // 助听同步信息的ux_bt_sync_type_t偏移

// 需要同步的数据类型，如有扩展需求，可以在此添加
// 在同步信息结构体中，该枚举放在首地址
// 在同步信息回调tws_sync_haid_param_response_cb中，param[0]为该枚举
typedef enum {
    UX_BS_SYNC_TYPE_HAID_MODE,
    UX_BS_SYNC_TYPE_HAID_MAX,
} ux_bt_sync_type_t;

// 助听模式同步信息
typedef struct {
    ux_bt_sync_type_t sync_type; // 该枚举位置不可更改，在tws_sync_haid_param_response_cb中用param[0]的方式访问
    uint8_t haid_mode;           // 助听模式, @ref HaidScene
} ux_bt_haid_mode_sync_info_t;

void haid_service_handle_aid_in(uint8_t *data, uint16_t data_len);
void haid_service_handle_aid_out(uint8_t *data, uint16_t data_len);
void haid_service_handle_vol_up(uint8_t *data, uint16_t data_len);
void haid_service_handle_vol_down(uint8_t *data, uint16_t data_len);
void haid_service_handle_power_on(uint8_t *data, uint16_t data_len);
void haid_service_handle_power_off(uint8_t *data, uint16_t data_len);
void haid_service_handle_ble_disconnect(uint8_t *data, uint16_t data_len);
void haid_service_handle_phone_state_report(uint8_t *data, uint16_t data_len);
void haid_service_handle_recover(uint8_t *data, uint16_t data_len);
void haid_service_handle_afc_calibrate_start(uint8_t *data, uint16_t data_len);
void haid_service_handle_afc_calibrate_stop(uint8_t *data, uint16_t data_len);
void haid_service_handle_afc_calibrate_end(uint8_t *data, uint16_t data_len);
void ux_haid_set_local_mode(HaidScene next_mode);
uint8_t haid_service_afc_stop(void);
uint8_t haid_service_afc_start(uint8_t *data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#endif
