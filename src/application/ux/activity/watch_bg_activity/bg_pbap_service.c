/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#include "ux_common_log.h"
#include "timer_manager.h"
#include "ux_bt_pbap.h"
static void delete_all_temp_local_file(void)
{
    /* 删除所有临时文件, 失败不阻塞 */
    bool is_temp = true;
    ux_bt_pbap_delete_local_file(UX_API_BT_PBAP_FILE_PHONE_BOOK, is_temp);
    ux_bt_pbap_delete_local_file(UX_API_BT_PBAP_FILE_CALL_LOG, is_temp);
}

static void pbap_connected_process(ux_bt_pbap_info_t *cur_pbap_info)
{
    ux_api_bt_pbap_file_type_t pbap_file = UX_API_BT_PBAP_FILE_MAX;
    if (cur_pbap_info->phone_book_recevice_state == UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL) {
        pbap_file = UX_API_BT_PBAP_FILE_PHONE_BOOK;
    } else if (cur_pbap_info->call_log_recevice_state == UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL) {
        pbap_file = UX_API_BT_PBAP_FILE_CALL_LOG;
    } else {
        ux_log_err("[pbap_connect_process] not need to pull pbap file.\n");
        return;
    }
    delete_all_temp_local_file();
    errcode_t err = ux_bt_pbap_pull_remote_file(cur_pbap_info->dev_id, pbap_file);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[pbap_connect_process] ux_bt_pbap_pull_remote_file fail 0x%x.\n", err);
        return;
    }
}

void bg_pbap_service_profile_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_pbap_service_profile_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_pbap_info_t)) {
        ux_log_err("[bg_pbap_service_profile_state_change] Invalid len %d.\n", param_len);
        return;
    }

    ux_bt_pbap_info_t *profile_state_msg = (ux_bt_pbap_info_t*)param;
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();
    switch (profile_state_msg->profile_state) {
        case UX_API_BT_PROFILE_STATE_CONNECTING:
            break;
        case UX_API_BT_PROFILE_STATE_CONNECTED:
            pbap_connected_process(cur_pbap_info);
            break;
        case UX_API_BT_PROFILE_STATE_DISCONNECTING:
            break;
        case UX_API_BT_PROFILE_STATE_DISCONNECTED:
            break;
        default:
            return;
    }
    /* 更新连接状态 */
    ux_bt_pbap_mutex_lock();
    cur_pbap_info->profile_state = profile_state_msg->profile_state;
    ux_bt_pbap_mutex_unlock();
}

static void rename_pbap_file(uint16_t dev_id, ux_api_bt_pbap_file_type_t file_type)
{
    errcode_t err = UX_ERR_SUCCESS;
    bool is_temp = true;
    int ret = UX_ERR_SUCCESS;
    
    ux_bt_pbap_mutex_lock();
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            ret = rename(UX_BT_PBAP_PHONE_BOOK_TEMP_NAME, UX_BT_PBAP_PHONE_BOOK_NAME);
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            ret = rename(UX_BT_PBAP_PHONE_CALL_LOG_TEMP_NAME, UX_BT_PBAP_PHONE_CALL_LOG_NAME);
            break;
        default:
            ret = UX_ERR_INVALID_PARAMETER;
            break;
    }
    ux_bt_pbap_mutex_unlock();
    if (ret != UX_ERR_SUCCESS) {
        err = ux_bt_pbap_delete_local_file(file_type, is_temp);
        if (err != UX_ERR_SUCCESS) {
            ux_log_err("[rename_pbap_file] fail to rename file: %d, ret:%x.\n", file_type, ret);
        }
        return;
    }
}

void bg_pbap_service_finish_receive_file(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_pbap_service_finish_receive_file] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_pbap_receive_result_t)) {
        ux_log_err("[bg_pbap_service_finish_receive_file] Invalid len %d.\n", param_len);
        return;
    }
    ux_bt_pbap_receive_result_t *receive_result_msg = (ux_bt_pbap_receive_result_t*)param;
    bool is_temp = true;
    int ret = UX_ERR_SUCCESS;
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();

    /* 文件传输结束, 销毁定时器 */

    /* 文件传输成功, 替换原文件, 删除临时文件 */
    if (receive_result_msg->ret == UX_ERR_SUCCESS) {
        rename_pbap_file(receive_result_msg->dev_id, receive_result_msg->file_type);
    } else {
        /* 传输失败, 删除临时文件 */
        err = ux_bt_pbap_delete_local_file(receive_result_msg->file_type, is_temp);
        if (err != UX_ERR_SUCCESS) {
            ux_log_err("[bg_pbap_service_finish_receive_file] fail to rename file, ret:0x%x.\n", ret);
        }
    }

    if (cur_pbap_info->phone_book_recevice_state == UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL) {
        /* 需要拉phone book */
        err = ux_bt_pbap_pull_remote_file(receive_result_msg->dev_id, UX_API_BT_PBAP_FILE_PHONE_BOOK);
    } else if (cur_pbap_info->call_log_recevice_state == UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL) {
        /* 需要拉call log */
        err = ux_bt_pbap_pull_remote_file(receive_result_msg->dev_id, UX_API_BT_PBAP_FILE_CALL_LOG);
    } else {
        /* 无需要拉取的文件, 断开pbap连接 */
        cur_pbap_info->profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTING;
        err = ux_bt_pbap_disconnect(receive_result_msg->dev_id);
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_pbap_service_finish_receive_file] fail to pull pbap file, ret:0x%x.\n", ret);
    }
}