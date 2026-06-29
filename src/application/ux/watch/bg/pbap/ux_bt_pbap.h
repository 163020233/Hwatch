/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#ifndef UX_BT_PBAP_H
#define UX_BT_PBAP_H

#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "errcode.h"
#include "ux_bg_config.h"
#include "ux_api_pbap.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_BT_PBAP_PHONE_BOOK_NAME "user/phone_book.vcf"
#define UX_BT_PBAP_PHONE_BOOK_TEMP_NAME "user/phone_book(1).vcf"
#define UX_BT_PBAP_PHONE_CALL_LOG_NAME "user/call_log.vcf"
#define UX_BT_PBAP_PHONE_CALL_LOG_TEMP_NAME "user/call_log(1).vcf"

typedef  enum {
    UX_BT_PBAP_RECEIVE_STATE_IDLE         = 0x00, // 空闲状态
    UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL = 0x01, // 等待拉取中
    UX_BT_PBAP_RECEIVE_STATE_RECEIVING    = 0x02, // 传输中
    UX_BT_PBAP_RECEIVE_STATE_SUCCESS      = 0x04, // 传输完成
    UX_BT_PBAP_RECEIVE_STATE_FAIL         = 0x08, // 传输失败
    UX_BT_PBAP_RECEIVE_STATE_MAX          = 0xFF,
} ux_bt_pbap_receive_state_t;

typedef struct {
    uint16_t dev_id;
    ux_api_bt_profile_state_t profile_state;
    ux_bt_pbap_receive_state_t phone_book_recevice_state;
    ux_bt_pbap_receive_state_t call_log_recevice_state;
} ux_bt_pbap_info_t;

typedef struct {
    uint16_t dev_id;
    ux_api_bt_pbap_file_type_t file_type;
    uint32_t ret;
} ux_bt_pbap_receive_result_t;

errcode_t ux_bt_pbap_init(void);
errcode_t ux_bt_pbap_deinit(void);
ux_bt_pbap_info_t* ux_bt_pbap_get_info(void);
errcode_t ux_bt_pbap_connect(uint16_t dev_id);
errcode_t ux_bt_pbap_disconnect(uint16_t dev_id);
errcode_t ux_bt_pbap_pull_remote_file(uint16_t dev_id, ux_api_bt_pbap_file_type_t file_type);
errcode_t ux_bt_pbap_delete_local_file(ux_api_bt_pbap_file_type_t file_type, bool is_temp);
void ux_bt_pbap_mutex_unlock(void);
void ux_bt_pbap_mutex_lock(void);
#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif