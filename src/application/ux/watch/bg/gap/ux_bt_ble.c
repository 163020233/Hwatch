/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux bg ble\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"
#include "ux_errcode.h"
#include "errcode.h"
#include "securec.h"
#include "nv.h"
#include "ux_bt.h"
#include "ux_feature_config.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "common_def.h"
#include "bs_device_mgr.h"
#include "bs_ble_service_api.h"
#include "ux_bg_config.h"
#include "osal_mutex.h"
#include "bs_ble_service_config.h"
#include "ux_api_bt.h"
#include "ux_ble_service.h"
#include "bts_le_gap.h"
#include "ux_api_fwk.h"
#include "ux_api_msg.h"
#include "ux_event_def.h"
#include "adapter_bt_br.h"
#include "ux_hal_memory.h"
#include "property.h"
#ifdef JS_ENABLE
#include "dfx_channel.h"
#endif
#include "ux_bt_ble.h"
#define BT_STR_END_LEN   1
#define BS_BLE_DEVICE_NAME_MAX_LEN 32
static osal_mutex g_ble_stack_info_mutex;
static ux_bt_stack_info_t g_ble_stack_info = {
    .adv_status = UX_BT_DISCOVERY_STOP,
    .scan_status = UX_BT_DISCOVERY_STOP,
    .stack_status = UX_BT_STACK_DEINITED,
};

ux_bt_stack_info_t *ux_bt_ble_get_stack_info(void)
{
    return &g_ble_stack_info;
}

ux_api_bt_local_info_t* ux_bt_ble_get_local_info(void)
{
    static ux_api_bt_local_info_t s_ux_ble_local_info = {0};
    errcode_t err = UX_ERR_SUCCESS;
    if (property_get_bytes(NV_ID_UX_BT_NAME, (uint8_t *)s_ux_ble_local_info.name,
        sizeof(s_ux_ble_local_info.name)) == NULL) {
        err = (errcode_t)strcpy_s(s_ux_ble_local_info.name, sizeof(s_ux_ble_local_info.name), ux_get_default_bt_name());
        if (err != EOK) {
            ux_log_err("[ux_bt_ble_get_local_info] get_name ret=0x%x.\n", err);
        }
    }
    err = ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, s_ux_ble_local_info.addr.addr, sizeof(s_ux_ble_local_info.addr.addr));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_ble_get_local_info] load addr ret=0x%x.\n", err);
    }
    if (g_ble_stack_info.stack_status == UX_BT_STACK_INITIED) {
        s_ux_ble_local_info.enable = true;
    } else {
        s_ux_ble_local_info.enable = false;
    }
    return &s_ux_ble_local_info;
}

static void process_ble_enable_result(void)
{
    errcode_t ret = UX_ERR_SUCCESS;
    ux_bt_stack_info_t *ble_info = ux_bt_ble_get_stack_info();
    osal_mutex_lock(&g_ble_stack_info_mutex);
    ble_info->stack_status = UX_BT_STACK_INITIED;
    osal_mutex_unlock(&g_ble_stack_info_mutex);

    char bt_name[BS_BLE_DEVICE_NAME_MAX_LEN] = {0};
    ux_bd_addr bt_add = {0};

    /* 从nv中读取名字，并设置到蓝牙配置中 */
    ret = ux_api_bt_get_name((uint8_t *)bt_name, BS_BLE_DEVICE_NAME_MAX_LEN);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[process_ble_enable_result] Fail to load bt name: 0x%x!\n", ret);
        return;
    }
    ret = bs_config(BLE_CONFIG_LOCAL_NAME, bt_name, (uint16_t)strlen(bt_name));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[process_ble_enable_result] Fail to config bt name: 0x%x!\n", ret);
        return;
    }

    ret = ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, bt_add, arry_num(bt_add));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[process_ble_enable_result] Fail to load bt addr: 0x%x!\n", ret);
        return;
    }
    ret = bs_config(BLE_CONFIG_LOCAL_ADDR, bt_add, arry_num(bt_add));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[process_ble_enable_result] Fail to config bt addr: 0x%x!\n", ret);
        return;
    }
    gap_ble_sec_params_t sec_param = {
        .bondable = (uint8_t)true,
        .io_capability = GAP_BLE_IO_CAPABILITY_NOINPUTNOOUTPUT,
        .sc_enable = (uint8_t)true,
        .sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL3,
        .privacy = GAP_BLE_PRIVACY_DISABLE,
    };
    gap_ble_set_sec_param(&sec_param);
    /* 设置ble广播设备类型 */
    gap_ble_set_local_appearance(GAP_BLE_APPEARANCE_TYPE_GENERIC_WATCH);
    /* 并启动广播 */
    if (ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BLE) != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bg][process_ble_enable_result] Fail to start ble adv 0x%x.\n", ret);
    }
#ifdef CONFIG_SUPPORT_UX_GATT
    /* 初始化手机与耳机间BLE数传服务 */
    ux_bs_ble_service_init();
#ifdef JS_ENABLE
    diag_channel_register_bt_gatt_init();
#endif
#endif
    ux_log_info("[ux][process_ble_enable_result]ble enable success");
    ux_adapter_bt_enable_process(BS_PROTOCOL_BLE);
}

/* ble使能失败，修改改状态，并且销毁锁 */
static void process_ble_disable_result(void)
{
    ux_bt_stack_info_t *ble_info = ux_bt_ble_get_stack_info();
    osal_mutex_lock(&g_ble_stack_info_mutex);
    ble_info->stack_status = UX_BT_STACK_DEINITED;
    osal_mutex_unlock(&g_ble_stack_info_mutex);
    osal_mutex_destroy(&g_ble_stack_info_mutex);
    g_ble_stack_info_mutex.mutex = NULL;
    ux_log_info("[ux][process_ble_disable_result]ble disable success");
}

/* 协议栈发生状态切换，分别处理 */
static void process_ble_stack_state_change_result(uint8_t state)
{
    ux_log_info("ble enable state: %d.\n", state);
    switch (state) {
        case BLE_STATE_ENABLED:
            process_ble_enable_result();
            break;
        case BLE_STATE_DISABLED:
            process_ble_disable_result();
            break;
        default:
            break;
    }
}

static bool is_inactive_ble_central_device(const ux_bg_device_info_t *node, const uint8_t *param, uint16_t len)
{
    unused(param);
    unused(len);
    if (node->role == UX_API_BT_AS_CENTRAL && node->protocol == UX_API_BT_PROTOCOL_BLE &&
        node->connect_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        return true;
    }
    return false;
}

static void remove_inactive_ble_central_device(void)
{
    ux_bg_device_info_t *old_dev = NULL;
    old_dev = ux_bg_get_device_info(ux_bg_get_paired_device_list(), is_inactive_ble_central_device, NULL, 0);
    if (old_dev == NULL) {
        ux_log_info("[ux][bg][process_new_device_connected] old_dev is nullptr!\n");
        return;
    }
    errcode_t err = bs_ble_remove_pair(old_dev->id);
    ux_log_debug("[ux][bg][process_new_device_connected] remove id: %s, err:0x%x.\n", old_dev->id, err);
}

static ux_bg_device_info_t* append_new_ble_central_device(uint16_t dev_id)
{
    if (ux_bt_active_device_is_full(UX_API_BT_AS_CENTRAL, UX_API_BT_PROTOCOL_BLE)) {
        ux_log_err("[ux][bg][process_new_device_connected] device had connected!\n");
        bs_ble_disconnect(dev_id);
        return NULL;
    }
    remove_inactive_ble_central_device();
    // 地址类型可变，由随机变成固定
    bs_addr_t *bs_addr = bs_get_addr_by_device_id((uint8_t)dev_id);
    if (bs_addr == NULL) {
        ux_log_err("[ux][bg][update_ble_dev_info] Btc fail to check add.\n");
        return NULL;
    }
    ux_bg_device_info_t *new_dev = ux_bg_append_blank_device_info(ux_bg_get_paired_device_list());
    if (new_dev == NULL) {
        ux_log_err("[ux][bg][append_new_ble_central_device] new_dev is nullptr!\r\n");
        return NULL;
    }
    /* 先填充默认设备名，等待bth改接口 */
    if (snprintf_s(new_dev->name, BS_BLE_DEVICE_NAME_MAX_LEN, BS_BLE_DEVICE_NAME_MAX_LEN - 1,
        "default_device_%hu", dev_id) < 0) {
        ux_log_err("[ux][bg][append_new_ble_central_device] Fail to snprintf device name.\r\n");
    }
    new_dev->role = UX_API_BT_AS_CENTRAL;
    new_dev->protocol = UX_API_BT_PROTOCOL_BLE;
    new_dev->id = dev_id;
    new_dev->connect_state = UX_API_BT_PROFILE_STATE_CONNECTED;
    new_dev->is_valid = true;
    if (memcpy_s(&new_dev->addr.addr, UX_API_BT_ADDR_LEN, bs_addr->addr, BS_ADDR_LEN) != EOK) {
        ux_log_err("[ux][bg][append_new_ble_central_device]Fail to memcpy device addr.");
    }
    new_dev->addr.type = bs_addr->type;
    new_dev->enc_state = false;
    return new_dev;
}

static errcode_t update_ble_dev_info(ux_bg_device_info_t *dev_info, const bs_ble_connection_result_t *conn_result)
{
    if (ux_bt_active_device_is_full(dev_info->role, UX_API_BT_PROTOCOL_BLE) &&
        conn_result->encrypt_state == BLE_ENCRYPT_NONE && conn_result->conn_state == BLE_CONNECT_CONNECTED) {
        bs_ble_disconnect(conn_result->dev_id);
        ux_log_err("[ux][bg][process_new_device_connected] device had connected!\n");
        return UX_ERR_BG_PROFILE_HAD_CONNECTED;
    }
    dev_info->id = conn_result->dev_id;
    /* 找到设备id对应的地址和protocol, ble地址可变，在配对后确定，修改地址 */
    bs_addr_t *bs_addr = bs_get_addr_by_device_id((uint8_t)conn_result->dev_id);
    if (bs_addr == NULL) {
        ux_log_err("[ux][bg][update_ble_dev_info] Btc fail to check add.\n");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    // 地址类型可变，由随机变成固定
    if (memcpy_s(&dev_info->addr.addr, UX_API_BT_ADDR_LEN, bs_addr->addr, BS_ADDR_LEN) != EOK) {
        ux_log_err("[ux][bg][update_ble_dev_info]Fail to memcpy device addr");
    }
    dev_info->addr.type = bs_addr->type;
    if (conn_result->conn_state == BLE_CONNECT_CONNECTED) {
        dev_info->connect_state = UX_API_BT_PROFILE_STATE_CONNECTED;
    } else {
        dev_info->connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
    }
    return UX_ERR_SUCCESS;
}

static void notify_ble_connect_state(bs_ble_connection_result_t *result, ux_bg_device_info_t *device_node)
{
    /* 未加密时上报连接状态, 上层进行一键双连 */
    if (result->conn_state == BLE_CONNECT_CONNECTED && result->encrypt_state == BLE_ENCRYPT_NONE) {
        device_node->connect_state = UX_API_BT_PROFILE_STATE_CONNECTED;
        ux_adapter_bt_connect_process((int8_t *)(device_node->name), &device_node->addr, result->reason,
            UX_BT_BLE_CONNECTED, device_node->device_class);
    } else if (result->conn_state == BLE_CONNECT_DISCONNECTED &&
               device_node->connect_state != UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        device_node->connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
        ux_adapter_bt_disconnect_process((int8_t *)(device_node->name), &device_node->addr, result->reason,
            UX_BT_BLE_DISCONNECTED, device_node->device_class);
    }
}

static void ctkd_pair(bs_ble_connection_result_t *result)
{
    if (ux_bt_ctkd_is_open()) {
        ux_api_comm_mail_t mail = {0};
        mail.event = UX_EVENT_TYPE_BG;
        mail.sub_event = UX_BG_START_PAIR;
        ux_api_fwk_push_mail(&mail, (uint8_t*)&result->dev_id, sizeof(result->dev_id));
    }
}
static void start_ble_adv(bs_ble_local_role_t role, bs_ble_conn_state_t conn_state)
{
    if (role != BLE_SVC_LOCAL_PERIPHERAL) {
        return;
    }
    switch (conn_state) {
        case BLE_CONNECT_CONNECTED:
            ux_bt_ble_switch_discovery(UX_API_BT_AS_PERIPHERAL, UX_BT_DISCOVERY_STOP);
            break;
        case BLE_CONNECT_DISCONNECTED:
            ux_bt_ble_switch_discovery(UX_API_BT_AS_PERIPHERAL, UX_BT_DISCOVERY_START);
            break;
        default:
            break;
    }
}

static void delete_invalid_ble_device(bs_ble_connection_result_t *result, ux_bg_device_info_t *device_node)
{
    uint16_t dev_id = device_node->id;
    ux_log_debug("[ux][bt][process_ble_connect_state_changed_result] delete device %d.\n", dev_id);
    notify_ble_connect_state(result, device_node);
    ux_bt_clear_cur_device_info(device_node->role, device_node->protocol);
    ux_bg_delete_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
}
/* connect连接状态发生改变时的处理 */
static void process_ble_connect_state_changed_result(bs_ble_connection_result_t *result)
{
    if (result == NULL) {
        ux_log_err("[ux][bt][process_ble_connect_state_changed_result] input para NULL!\n");
        return;
    }
    ux_log_info("[ux][ble_conn_state_changed][dev_id=%d][state=%d][enc_state=%d][reason=0x%x]",
        result->dev_id, result->conn_state, result->encrypt_state, result->reason);
    ux_bg_device_info_t *find_device_res = NULL;
    find_device_res = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), result->dev_id);
    /*
     * 若本机作为中心设备，在主动发起连接时，对端已经插入设备列表
     * 若无法在设备列表中查到，本机作为外围设备被连接
     * 设备角色只在首次查表或主动发起连接时更新
    */
    start_ble_adv(result->local_role, result->conn_state);
    if (result->conn_state == BLE_CONNECT_CONNECTED) {
        if (find_device_res == NULL) {
            /* 新的外围设备连接 */
            find_device_res = append_new_ble_central_device(result->dev_id);
            if (find_device_res == NULL) {
                ux_log_err("[ux][process_ble_connect_state_changed_result] Fail to append new device.\n");
                return;
            }
            ux_bt_update_cur_device_info_from_node(find_device_res);
        } else {
            /* 已有的设备回连 */
            if (update_ble_dev_info(find_device_res, result) != UX_ERR_SUCCESS) {
                ux_log_err("[ux][process_ble_connect_state_changed_result] Fail to update device.\n");
                return;
            }
            ux_bt_update_cur_device_info_from_node(find_device_res);
        }
        find_device_res->enc_state = ((result->encrypt_state == BLE_ENCRYPT_SUCCESS) ? true : false);
        /* 先连接后配对，若打开一键双连, 主动发起配对, 为避免时序问题，必须在连接状态置为connected的时候发送事件 */
        ctkd_pair(result);
    } else if (result->conn_state == BLE_CONNECT_DISCONNECTED) {
        if (find_device_res == NULL) {
            /* 异常, 不存在的设备断连 */
            ux_log_err("[ux][bt][process_ble_connect_state_changed_result] No this device.\n");
            return;
        } else {
            if (find_device_res->is_valid == false || find_device_res->enc_state == false) {
                /* 已删除或未加密的设备断连 */
                delete_invalid_ble_device(result, find_device_res);
                return;
            } else {
                /* 已加密的设备断连 */
                update_ble_dev_info(find_device_res, result);
                ux_bt_update_cur_device_info_from_node(find_device_res);
            }
        }
    } else {
        /* 异常, 未知状态 */
        ux_log_err("[ux][bt][process_ble_connect_state_changed_result] invalid state=%d.\n", result->conn_state);
        return;
    }
    notify_ble_connect_state(result, find_device_res);
}

/* 配对成功 */
static void process_ble_pair_result(uint16_t dev_id, const bs_addr_t *addr, const bs_ble_pair_info_report_t *pair_info)
{
    unused(pair_info);
    ux_log_info("[ux][bt]process_ble_pair_result success: ****%02x%02x%02x%02x, dev_id[%d]\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], dev_id);
}

/* 处理ble discovery的扫描结果 */
static void process_ble_discover_result(const bs_ble_discovery_result_t *discovery_result,
    const uint8_t *data, uint16_t len)
{
    unused(len);
    unused(data);
    if (discovery_result == NULL) {
        ux_log_info("[process_ble_discover_result] Nullptr\n");
        return;
    }
    ux_bg_device_info_t* find_res = NULL;
    /* 新设备插表，已插表设备不处理 */
    find_res = ux_bg_find_device_info_by_addr(ux_bg_get_scan_device_list(),
        (ux_api_bt_addr_t*)&discovery_result->addr, UX_API_BT_PROTOCOL_BLE);
    if (find_res == NULL) {
        ux_bg_device_info_t *new_dev = ux_hal_memory_acquire(UX_MEM_BT, sizeof(ux_bg_device_info_t));
        if (new_dev == NULL) {
            ux_log_err("[process_ble_discover_result] ux_hal_memory_acquire new_dev fail.\n");
            return;
        }
        if (memcpy_s(&new_dev->addr, sizeof(ux_api_bt_addr_t), &discovery_result->addr, sizeof(bs_addr_t))) {
            ux_log_err("[process_ble_discover_result] memcpy_s addr fail.\n");
        }
        if (strcpy_s(new_dev->name, sizeof(find_res->name), (char*)discovery_result->name) != EOK) {
            ux_log_err("[process_ble_discover_result] sprintf_s name fail.\n");
        }
        new_dev->connect_state = UX_API_BT_PROFILE_STATE_MAX;
        new_dev->id = (uint16_t)(MIN_SCAN_ID + ux_bg_get_scan_device_count());
        new_dev->protocol = UX_API_BT_PROTOCOL_BLE;
        new_dev->role = UX_API_BT_AS_PERIPHERAL;
        new_dev->rssi = discovery_result->rssi;
        ux_bg_set_scan_device_count(ux_bg_get_scan_device_count() + 1);
        ux_bt_insert_device_with_rssi_filter(ux_bg_get_scan_device_list(), new_dev);
        ux_hal_memory_release(UX_MEM_BT, new_dev);
    } else {
        find_res->rssi = discovery_result->rssi;
        if (strcpy_s(find_res->name, arry_num(find_res->name), (char*)discovery_result->name) != EOK) {
            ux_log_err("[process_br_discover_result] Fail to strcpy name.\n");
            return;
        }
    }
}

static void process_ble_remove_device_result(const bs_addr_t *addr, errcode_t status)
{
    if (addr == NULL) {
        ux_log_err("[ux][process_ble_remove_device_result] nullptr.\n");
        return;
    }
    ux_log_info("[ux][bt]process_ble_remove_device_result status[%d]: ****%02x%02x%02x%02x\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], status);

    osal_mutex_lock(&g_ble_stack_info_mutex);
    ux_bg_device_info_t *dev_info = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(),
        (ux_api_bt_addr_t*)addr, UX_API_BT_PROTOCOL_BLE);
    if (dev_info == NULL) {
        ux_log_err("[ux][process_ble_remove_device_result] Can not find device.\n");
        return;
    } else if (dev_info->connect_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        dev_info->is_valid = false;
    } else {
        // 不处于连接态，直接移除
        ux_log_info("[process_br_remove_device_result] removed when disconnected, connect state:%d.\n",
            dev_info->connect_state);
        ux_bt_clear_cur_device_info(dev_info->role, dev_info->protocol);
        ux_bg_delete_device_info_by_id(ux_bg_get_paired_device_list(), dev_info->id);
    }
    osal_mutex_unlock(&g_ble_stack_info_mutex);
}


errcode_t ux_bt_ble_init(void)
{
    static bs_ble_callbacks_t ux_ble_cb = {
        .stack_state_cbk = process_ble_stack_state_change_result,
        .connect_state_cbk = process_ble_connect_state_changed_result,
        .pair_complete_cbk = process_ble_pair_result,
        .discover_result_cbk = process_ble_discover_result,
        .pair_remove_cbk = process_ble_remove_device_result,
    };
    errcode_t ret = UX_ERR_SUCCESS;

    if (osal_mutex_init(&g_ble_stack_info_mutex) != 0) {
        ux_log_err("[ux_bt_ble_init] Fail to init stack info mutexn");
        ret =  UX_ERR_MUTEX_INIT_FAILED;
        goto out;
    }

    ret = bs_ble_register_callbacks(0, &ux_ble_cb);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_ble_init] Fail to register cb: 0x%x!\n", ret);
        goto out;
    }
    ux_bt_stack_info_t *ble_info = ux_bt_ble_get_stack_info();
    osal_mutex_lock(&g_ble_stack_info_mutex);
    ble_info->stack_status = UX_BT_STACK_INITING;
    osal_mutex_unlock(&g_ble_stack_info_mutex);
    ret = bs_ble_enable();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_ble_init] Fail to enable ble: 0x%x!\n", ret);
        goto out;
    }

    return ret;
out:
    osal_mutex_destroy(&g_ble_stack_info_mutex);
    g_ble_stack_info_mutex.mutex = NULL;
    return ret;
}

errcode_t ux_bt_ble_deinit(void)
{
    ux_bg_clear_device_list(ux_bg_get_paired_device_list());
    ux_bg_clear_device_list(ux_bg_get_scan_device_list());
    errcode_t ret = UX_ERR_SUCCESS;
    ux_bt_stack_info_t *ble_info = ux_bt_ble_get_stack_info();
    osal_mutex_lock(&g_ble_stack_info_mutex);
    ble_info->stack_status = UX_BT_STACK_DEINITING;
    osal_mutex_unlock(&g_ble_stack_info_mutex);
    ret = bs_ble_disable();
    if (ret != UX_ERR_SUCCESS) {
        osal_mutex_destroy(&g_ble_stack_info_mutex);
        g_ble_stack_info_mutex.mutex = NULL;
        ux_log_err("Fail to disable ble.\n");
    }

    ret = bs_ble_stop_auto_conn();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("Fail to stop auto connect.\n");
    }
    return ret;
}

errcode_t ux_bt_ble_switch_discovery(ux_api_bt_role_t role, ux_bt_discovery_state_t discovery_switch)
{
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_stack_info_t *ble_stack_info = ux_bt_ble_get_stack_info();
    ux_bt_discovery_state_t *discovery_state = NULL;
    if (role == UX_API_BT_AS_CENTRAL && discovery_switch == UX_BT_DISCOVERY_START) {
        /* ble开扫描 */
        err = bs_ble_start_discovery(BLE_DISCOVERY_CENTRAL);
        discovery_state = &ble_stack_info->scan_status;
    } else if (role == UX_API_BT_AS_CENTRAL && discovery_switch == UX_BT_DISCOVERY_STOP) {
        /* ble关扫描 */
        err = bs_ble_stop_discovery(BLE_DISCOVERY_CENTRAL);
        discovery_state = &ble_stack_info->scan_status;
    } else if (role == UX_API_BT_AS_PERIPHERAL && discovery_switch == UX_BT_DISCOVERY_START) {
        /* ble开广播 */
        err = bs_ble_start_discovery(BLE_DISCOVERY_PERIPHERAL);
        discovery_state = &ble_stack_info->adv_status;
    } else if (role == UX_API_BT_AS_PERIPHERAL && discovery_switch == UX_BT_DISCOVERY_STOP) {
        /* ble关广播 */
        err = bs_ble_stop_discovery(BLE_DISCOVERY_PERIPHERAL);
        discovery_state = &ble_stack_info->adv_status;
    } else {
        err = UX_ERR_INVALID_PARAMETER;
    }

    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_switch_br_discovery] fail to switch ble discovery, role:%d, discovery_state:%d, err:0x%x",
            role, discovery_switch, err);
        return err;
    }
    /* discovery切换成功，刷新状态 */
    *discovery_state = discovery_switch;
    return UX_ERR_SUCCESS;
}