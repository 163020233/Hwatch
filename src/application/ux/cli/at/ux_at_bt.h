/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux bt at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#ifndef UX_AT_BT_H
#define UX_AT_BT_H
#include "stdint.h"
#include "errcode.h"
#include "at.h"
#include "ux_at_manager.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define BLE_DISCONNECT                      "ble_disconnect"

at_ret_t parse_btaddr_para_process(const char *para);
at_ret_t parse_peeraddr_para_process(const char *para);
at_ret_t parse_bleaddr_para_process(const char *para);
at_ret_t parse_name_para_process(const char *para);
at_ret_t parse_ble_name_para_process(const char *para);
at_ret_t parse_ble_expand_val_para_process(const char *para);
at_ret_t parse_earside_para_process(const char *para);
at_ret_t parse_ble_disconnect_para_process(const char *para);
at_ret_t query_bt_addr(void);
/* 查询 peeraddr */
at_ret_t query_peer_addr(void);
/* 查询 bleaddr */
at_ret_t query_ble_addr(void);
/* 查询 name */
at_ret_t query_bt_name(void);
/* 查询 ble_expend_val */
at_ret_t query_ble_expand_val(void);
/* 查询 blename */
at_ret_t query_ble_name(void);
/* 查询 earside */
at_ret_t query_ear_side(void);
/* 查询 role */
at_ret_t query_tws_role(void);
at_ret_t at_btcfg_read_name_common(char *name, uint16_t bt_name_str_max_len,
    char *blename, uint16_t ble_name_str_max_len);
at_ret_t query_bt_info_pair(void);
#ifdef DEMO_WATCH
at_ret_t at_btcfg_set_process(const ux_at_para5_args_t *args);
#else
at_ret_t at_btcfg_set_process(const ux_at_para2_args_t *args);
#endif
at_ret_t at_btcfg_read_process(void);
at_ret_t at_btcfg_query_process(const ux_at_para2_args_t *args);

at_ret_t at_pair_set_process(const ux_at_para2_args_t *args);
at_ret_t at_pair_read_process(void);
at_ret_t at_pair_query_process(const ux_at_para2_args_t *args);
at_ret_t ux_get_device_bt_cfg_string(char *result, uint16_t result_size);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
