/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-25， Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "securec.h"
#include "ux_common_log.h"
#include "ux_event_def.h"
#include "ux_api_fwk.h"
#include "timer_manager.h"
#include "bs_br_phone_book.h"
#include "ux_bt_pbap.h"

static osal_mutex g_pbap_mutex;

ux_bt_pbap_info_t g_pbap_info = {
    .profile_state = UX_API_BT_PROFILE_STATE_MAX,
    .dev_id = BS_CONNECTION_UPPER_LIMIT,
    .call_log_recevice_state = UX_BT_PBAP_RECEIVE_STATE_IDLE,
    .phone_book_recevice_state = UX_BT_PBAP_RECEIVE_STATE_IDLE,
};

const uint8_t g_ux_bth_pofile_state_map[] = {
    [UX_API_BT_PROFILE_STATE_CONNECTING]    = BR_PROFILE_STATE_CONNECTING,
    [UX_API_BT_PROFILE_STATE_CONNECTED]     = BR_PROFILE_STATE_CONNECTED,
    [UX_API_BT_PROFILE_STATE_DISCONNECTING] = BR_PROFILE_STATE_DISCONNECTING,
    [UX_API_BT_PROFILE_STATE_DISCONNECTED]  = BR_PROFILE_STATE_DISCONNECTED,
};

ux_bt_pbap_info_t* ux_bt_pbap_get_info(void)
{
    return &g_pbap_info;
}

void ux_bt_pbap_mutex_lock(void)
{
    osal_mutex_lock(&g_pbap_mutex);
}

void ux_bt_pbap_mutex_unlock(void)
{
    osal_mutex_unlock(&g_pbap_mutex);
}

static void ux_bt_pbap_phone_book_conn_state_changed_cbk(uint16_t dev_id, uint8_t conn_state)
{
    ux_log_info("[ux_bt_pbap_call_log_result_cbk] pbap profile change, dev_id=%d, conn_state=%d", dev_id, conn_state);
    if (conn_state > BR_PROFILE_STATE_DISCONNECTED || conn_state < BR_PROFILE_STATE_CONNECTING) {
        ux_log_err("[ux_bt_pbap_phone_book_conn_state_changed_cbk] Invalid state: %d", conn_state);
        return;
    }

    ux_bt_pbap_info_t profile_state_msg = {0};
    profile_state_msg.dev_id = dev_id;
    for (uint8_t i = 0; i < arry_num(g_ux_bth_pofile_state_map); i++) {
        if (g_ux_bth_pofile_state_map[i] == conn_state) {
            profile_state_msg.profile_state = i;
            break;
        }
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_PBAP_PROFILE_STATE_CHANGE,
        (uint8_t *)(&profile_state_msg), sizeof(ux_bt_pbap_info_t));
}

static void write_temp_file(ux_api_bt_pbap_file_type_t file_type, uint16_t dev_id, const uint8_t *data, uint16_t len)
{
    unused(dev_id);
    unused(len);
    FILE *phone_book_f = NULL;
    /* 打开临时文件 */
    ux_bt_pbap_mutex_lock();
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            phone_book_f = fopen(UX_BT_PBAP_PHONE_BOOK_TEMP_NAME, "a");
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            phone_book_f = fopen(UX_BT_PBAP_PHONE_CALL_LOG_TEMP_NAME, "a");
            break;
        default:
            return;
    }
    ux_bt_pbap_mutex_unlock();
    if (phone_book_f == NULL) {
        ux_log_err("[write_temp_file] Fail to open file: %d.\n", file_type);
        return;
    }
    /* 写入数据, 关闭文件 */
    ux_bt_pbap_mutex_lock();
    (void)fprintf(phone_book_f, "%s", data);
    int ret = fclose(phone_book_f);
    ux_bt_pbap_mutex_unlock();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_pbap_phone_book_recevice_cbk] Fail to close file, ret: %d.\n", ret);
    }
}

errcode_t ux_bt_pbap_delete_local_file(ux_api_bt_pbap_file_type_t file_type, bool is_temp)
{
    const char *file_name = NULL;
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            file_name = is_temp ? UX_BT_PBAP_PHONE_BOOK_TEMP_NAME : UX_BT_PBAP_PHONE_BOOK_NAME;
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            file_name = is_temp ? UX_BT_PBAP_PHONE_CALL_LOG_TEMP_NAME : UX_BT_PBAP_PHONE_CALL_LOG_NAME;
            break;
        default:
            return UX_ERR_INVALID_PARAMETER;
    }
    ux_bt_pbap_mutex_lock();
    (void)remove(file_name);
    ux_bt_pbap_mutex_unlock();
    return UX_ERR_SUCCESS;
}

static void ux_bt_pbap_phone_book_recevice_cbk(uint16_t dev_id, const uint8_t *data, uint16_t len)
{
    /**
     * 信息量过大，暂不复制副本, 直接写文件
     */
    if (data == NULL) {
        return;
    }
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();

    /* dev_id与当前dev_id不匹配 */
    if (dev_id != cur_pbap_info->dev_id) {
        return;
    }
    
    ux_bt_pbap_mutex_lock();
    cur_pbap_info->phone_book_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_RECEIVING;
    ux_bt_pbap_mutex_unlock();
    write_temp_file(UX_API_BT_PBAP_FILE_PHONE_BOOK, dev_id, data, len);
}

static void ux_bt_pbap_phone_book_result_cbk(uint16_t dev_id, errcode_t ret)
{
    ux_log_info("[ux_bt_pbap_phone_book_result_cbk] pull phone book end, dev_id=%d, ret=0x%x", dev_id, ret);
    /* dev_id与当前dev_id不匹配 */
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();
    if (dev_id != cur_pbap_info->dev_id) {
        return;
    }

    ux_bt_pbap_mutex_lock();
    if (ret != UX_ERR_SUCCESS) {
        cur_pbap_info->phone_book_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_FAIL;
    } else {
        cur_pbap_info->phone_book_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_SUCCESS;
    }
    ux_bt_pbap_mutex_unlock();

    ux_bt_pbap_receive_result_t receive_result_msg = {
        .dev_id = dev_id,
        .file_type = UX_API_BT_PBAP_FILE_PHONE_BOOK,
        .ret = ret,
    };
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_PBAP_RECEVICE_FINISH,
        (uint8_t *)(&receive_result_msg), sizeof(ux_bt_pbap_receive_result_t));
}

static void ux_bt_pbap_call_log_recevice_cbk(uint16_t dev_id, const uint8_t *data, uint16_t len)
{
    /**
     * 信息量过大，暂不复制副本, 直接写文件
     */
    if (data == NULL) {
        return;
    }
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();

    /* dev_id与当前dev_id不匹配 */
    if (dev_id != cur_pbap_info->dev_id) {
        return;
    }
    ux_bt_pbap_mutex_lock();
    cur_pbap_info->call_log_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_RECEIVING;
    ux_bt_pbap_mutex_unlock();
    write_temp_file(UX_API_BT_PBAP_FILE_CALL_LOG, dev_id, data, len);
}

static void ux_bt_pbap_call_log_result_cbk(uint16_t dev_id, errcode_t ret)
{
    ux_log_info("[ux_bt_pbap_call_log_result_cbk] pull call log end, dev_id=%d, ret=0x%x", dev_id, ret);
    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();
    /* dev_id与当前dev_id不匹配 */
    if (dev_id != cur_pbap_info->dev_id) {
        return;
    }

    /* 停止定时器 */
    ux_bt_pbap_mutex_lock();
    if (ret != UX_ERR_SUCCESS) {
        cur_pbap_info->call_log_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_FAIL;
    } else {
        cur_pbap_info->call_log_recevice_state |= UX_BT_PBAP_RECEIVE_STATE_SUCCESS;
    }
    ux_bt_pbap_mutex_unlock();

    ux_bt_pbap_receive_result_t receive_result_msg = {
        .dev_id = dev_id,
        .file_type = UX_API_BT_PBAP_FILE_CALL_LOG,
        .ret = ret,
    };
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_PBAP_RECEVICE_FINISH,
        (uint8_t *)(&receive_result_msg), sizeof(ux_bt_pbap_receive_result_t));
}

bs_br_phone_book_callbacks_t phone_book_cbks = {
    .connect_state_cbk = ux_bt_pbap_phone_book_conn_state_changed_cbk,
    .recevice_phone_book_cbk = ux_bt_pbap_phone_book_recevice_cbk,
    .phone_book_result_cbk = ux_bt_pbap_phone_book_result_cbk,
    .recevice_call_log_cbk = ux_bt_pbap_call_log_recevice_cbk,
    .call_log_result_cbk = ux_bt_pbap_call_log_result_cbk,
};

errcode_t ux_bt_pbap_init(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    if (osal_mutex_init(&g_pbap_mutex) != 0) {
        ux_log_err("[ux_bt_pbap_init] Fail to intit g_br_stack_info_mutex\n");
        err = UX_ERR_MUTEX_INIT_FAILED;
        return err;
    }

    err = bs_br_phone_book_register_callbacks(0, &phone_book_cbks);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_pbap_init] Fal to register pbap cbk 0x%x.\n", err);
        goto out;
    }

    return err;
out:
    osal_mutex_destroy(&g_pbap_mutex);
    g_pbap_mutex.mutex = NULL;
    return err;
}

errcode_t ux_bt_pbap_deinit(void)
{
    errcode_t err = bs_br_phone_book_unregister_callbacks(0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_pbap_deinit] Fal to unregister pbap cbk 0x%x.\n", err);
        return err;
    }
    ux_bt_pbap_info_t* pbap_info = ux_bt_pbap_get_info();
    if (pbap_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_bt_pbap_phone_book_conn_state_changed_cbk(pbap_info->dev_id, BR_PROFILE_STATE_DISCONNECTED);
    }
    osal_mutex_destroy(&g_pbap_mutex);
    g_pbap_mutex.mutex = NULL;
    return UX_ERR_SUCCESS;
}

errcode_t ux_bt_pbap_connect(uint16_t dev_id)
{
    return bs_br_phone_book_connect((uint8_t)dev_id);
}

errcode_t ux_bt_pbap_disconnect(uint16_t dev_id)
{
    /**
     * 定时器在开始传输时申请
     * 在结束传输后，断连前销毁
     */
    errcode_t err = UX_ERR_SUCCESS;
    err = bs_br_phone_book_disconnect((uint8_t)dev_id);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_pbap_disconnect] Fail to disconnect pbap 0x%x.\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

/**
* 建立连接后, 开始拉取远程电话簿、通话记录
* 失败不阻塞
*/
errcode_t ux_bt_pbap_pull_remote_file(uint16_t dev_id, ux_api_bt_pbap_file_type_t file_type)
{
    errcode_t err = UX_ERR_SUCCESS;
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            err = bs_br_pull_phone_book((uint8_t)dev_id, BR_PHONE_BOOK_VCARD21);
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            err = bs_br_pull_call_log((uint8_t)dev_id, BR_CALL_LOG_CCH, BR_PHONE_BOOK_VCARD21);
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_pbap_pull_remote_file] Fail to pull remote file, err: 0x%x.\n");
        return err;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_bt_pbap_open_file(FILE **vcf_file, ux_api_bt_pbap_file_type_t file_type)
{
    if (vcf_file == NULL) {
        ux_log_err("[ux_api_bt_pbap_open_file] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }

    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();
    ux_bt_pbap_receive_state_t file_state = UX_BT_PBAP_RECEIVE_STATE_MAX;
    const char* file_name = NULL;
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            file_state = cur_pbap_info->phone_book_recevice_state & UX_BT_PBAP_RECEIVE_STATE_SUCCESS;
            file_name = UX_BT_PBAP_PHONE_BOOK_NAME;
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            file_state = cur_pbap_info->call_log_recevice_state & UX_BT_PBAP_RECEIVE_STATE_SUCCESS;
            file_name = UX_BT_PBAP_PHONE_CALL_LOG_NAME;
            break;
        default:
            ux_log_err("[ux_api_bt_pbap_open_file] file_type=%d.\n", file_type);
            return UX_ERR_INVALID_PARAMETER;
    }
    /* 若文件未传输完成, 返回文件无效 */
    if (file_state != UX_BT_PBAP_RECEIVE_STATE_SUCCESS) {
        ux_log_err("[ux_api_bt_pbap_open_file] file_state=%d.\n", file_type);
        return UX_ERR_FILE_INVALID;
    }
    ux_bt_pbap_mutex_lock();
    *vcf_file = fopen(file_name, "r");
    ux_bt_pbap_mutex_unlock();
    if (*vcf_file == NULL) {
        ux_log_err("[ux_api_read_phone_book] Fail to open file.\n");
        return UX_ERR_FAIL_TO_OPEN_FILE;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_bt_pbap_close_file(FILE *vcf_file)
{
    if (vcf_file == NULL) {
        ux_log_err("[ux_api_bt_pbap_close_file] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_bt_pbap_mutex_lock();
    int ret = fclose(vcf_file);
    ux_bt_pbap_mutex_unlock();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_read_phone_book] Fail to close file.\n");
        return UX_ERR_FAIL_TO_CLOSE_FILE;
    }
    return UX_ERR_SUCCESS;
}
static errcode_t repull_pbap_file(ux_bt_pbap_info_t *cur_pbap_info, ux_bt_pbap_receive_state_t *receive_state)
{
    errcode_t err = UX_ERR_SUCCESS;
    /* 已连接，而且正在拉取，则返回success，在用户意图上视作成功 */
    if (cur_pbap_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED &&
        ((*receive_state&UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL) == UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL ||
         (*receive_state&UX_BT_PBAP_RECEIVE_STATE_RECEIVING) == UX_BT_PBAP_RECEIVE_STATE_RECEIVING)) {
        ux_log_err("[ux_api_bt_pbap_pull_remote_file] Profile state err %d.", cur_pbap_info->profile_state);
        return UX_ERR_SUCCESS;
    } else if (cur_pbap_info->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        /* 已连接，未处于拉取状态, 状态置位, 在另一个文件拉取结束后拉取 */
        *receive_state = UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL;
    } else {
        /* 断连状态, 重连, 在连接回调内拉取 */
        *receive_state = UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL;
        err = ux_bt_pbap_connect(cur_pbap_info->dev_id);
        if (err != UX_ERR_SUCCESS) {
            ux_log_err("[ux_api_bt_pbap_pull_remote_file] bs_br_phone_book_connect err 0x%x.", err);
            return err;
        }
    }
    return UX_ERR_SUCCESS;
}

static errcode_t pull_new_device_pbap_file(ux_bt_pbap_info_t *cur_pbap_info, ux_bt_pbap_receive_state_t *receive_state)
{
    errcode_t err = UX_ERR_SUCCESS;
    /* 拉取新设备的文件, 更新pbap信息, 连接后在连接回调内拉取 */
    cur_pbap_info->profile_state = UX_API_BT_PROFILE_STATE_CONNECTING;
    cur_pbap_info->call_log_recevice_state = UX_BT_PBAP_RECEIVE_STATE_IDLE;
    cur_pbap_info->phone_book_recevice_state = UX_BT_PBAP_RECEIVE_STATE_IDLE;
    *receive_state = UX_BT_PBAP_RECEIVE_STATE_WAITING_PULL;
    err = ux_bt_pbap_connect(cur_pbap_info->dev_id);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_pbap_pull_remote_file] bs_br_phone_book_connect err 0x%x.", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_bt_pbap_pull_remote_file(const ux_api_bt_addr_t *addr, ux_api_bt_pbap_file_type_t file_type)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_pbap_pull_remote_file] NuLLptr.\n");
        return UX_ERR_NULL_PTR;
    }
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, addr->type, addr->addr);
    if (dev_id == BS_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_api_bt_pbap_pull_remote_file] Device no exist\n");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }

    ux_bt_pbap_info_t *cur_pbap_info = ux_bt_pbap_get_info();
    ux_bt_pbap_receive_state_t *receive_state = NULL;
    errcode_t err = UX_ERR_SUCCESS;
    switch (file_type) {
        case UX_API_BT_PBAP_FILE_PHONE_BOOK:
            receive_state = &cur_pbap_info->phone_book_recevice_state;
            break;
        case UX_API_BT_PBAP_FILE_CALL_LOG:
            receive_state = &cur_pbap_info->call_log_recevice_state;
            break;
        default:
            ux_log_err("[ux_api_bt_pbap_pull_remote_file] file_type=%d\n", file_type);
            return UX_ERR_INVALID_PARAMETER;
    }
    if (dev_id == cur_pbap_info->dev_id) {
        /* 更新文件 */
        err = repull_pbap_file(cur_pbap_info, receive_state);
    } else {
        /* 首次拉取文件 */
        cur_pbap_info->dev_id = dev_id;
        err = pull_new_device_pbap_file(cur_pbap_info, receive_state);
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_pbap_pull_remote_file] fail to pull pbap file 0x%x\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}