/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "securec.h"
#include "ux_common_log.h"
#include "ux_errcode.h"
#ifdef CONFIG_SUPPORT_BLE
#include "ux_bt_ble.h"
#include "bs_ble_service_config.h"
#endif
#ifdef CONFIG_SUPPORT_BR
#include "ux_bt_br.h"
#endif
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
#include "ux_bt_a2dp.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
#include "ux_bt_a2dp_sink.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
#include "ux_bt_a2dp_src.h"
#endif
#ifdef CONFIG_SUPPORT_AVRCP
#include "ux_bt_avrcp.h"
#endif
#ifdef CONFIG_SUPPORT_HFP_HF
#include "ux_bt_hal_sco.h"
#endif
#include "adapter_bt_br.h"
#include "bts_device_manager.h"
#include "ux_event_def.h"
#include "ux_api_fwk.h"
#include "ux_bt_ble.h"
#include "ux_bt_br.h"
#include "ux_device_manager.h"
#include "ux_api_ble.h"
#include "ux_api_bt.h"
#include "ux_bt.h"

typedef errcode_t (*bs_pair_handle)(const bs_addr_t *addr);
bs_pair_handle bs_pair_func_set[UX_API_BT_PROTOCOL_MAX] = {
#ifdef CONFIG_SUPPORT_BR
    [UX_API_BT_PROTOCOL_BR] = bs_br_start_pair,
#endif
#ifdef CONFIG_SUPPORT_BLE
    [UX_API_BT_PROTOCOL_BLE] = bs_ble_start_pair,
#endif
};
static bool g_is_multiple_devices_open = true;
void ux_bt_switch_multiple_devices(bool enable)
{
    g_is_multiple_devices_open = enable;
}

bool ux_bt_is_multiple_device_open(void)
{
    return g_is_multiple_devices_open;
}

void ux_api_bt_switch_multiple_devices(bool enable)
{
    ux_bt_switch_multiple_devices(enable);
}

bool ux_api_bt_is_multiple_device_open(void)
{
    return ux_bt_is_multiple_device_open();
}

const ux_api_bt_local_info_t* ux_api_bt_get_local_info(ux_api_bt_protocol_t protocol)
{
    if (protocol >= UX_API_BT_PROTOCOL_MAX) {
        ux_log_err("[ux_api_bt_get_local_info] invalid param, protocol=%d.\n", protocol);
        return NULL;
    }
    typedef ux_api_bt_local_info_t* (*ux_bt_get_local_info_handle)(void);
    static ux_bt_get_local_info_handle s_get_local_info_handle_set[UX_API_BT_PROTOCOL_MAX] = {
#ifdef CONFIG_SUPPORT_BR
        [UX_API_BT_PROTOCOL_BR] = ux_bt_br_get_local_info,
#endif
#ifdef CONFIG_SUPPORT_BLE
        [UX_API_BT_PROTOCOL_BLE] = ux_bt_ble_get_local_info,
#endif
    };
    if (s_get_local_info_handle_set[protocol] == NULL) {
        ux_log_err("[ux_api_bt_get_local_info] not support, protocol=%d.\n", protocol);
        return NULL;
    }
    return s_get_local_info_handle_set[protocol]();
}

ux_api_bt_device_info_t* ux_bt_get_device_info_writable(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol,
    uint16_t *device_num)
{
    if (role >= UX_API_BT_AS_MAX || protocol >= UX_API_BT_PROTOCOL_MAX || device_num == NULL) {
        ux_log_err("[ux_bt_get_device_info_writable] invalid param, role=%d, protocol=%d.\n", role, protocol);
        return NULL;
    }
    static ux_api_bt_device_info_t s_ux_device_info_set[] = {
        {
            .role = UX_API_BT_AS_CENTRAL,
            .protocol = UX_API_BT_PROTOCOL_BR,
        },
        {
            .role = UX_API_BT_AS_PERIPHERAL,
            .protocol = UX_API_BT_PROTOCOL_BR,
        },
        {
            .role = UX_API_BT_AS_CENTRAL,
            .protocol = UX_API_BT_PROTOCOL_BLE,
        },
        {
            .role = UX_API_BT_AS_PERIPHERAL,
            .protocol = UX_API_BT_PROTOCOL_BLE,
        },
    };
    for (uint8_t i = 0; i < sizeof(s_ux_device_info_set) / sizeof(s_ux_device_info_set[0]); i++) {
        if (s_ux_device_info_set[i].role != role || s_ux_device_info_set[i].protocol != protocol) {
            continue;
        }
        /* 若同时连接2个及以上耳机, 可修改为二维数组 */
        *device_num = sizeof(s_ux_device_info_set[i]) / sizeof(s_ux_device_info_set[i]);
        return &s_ux_device_info_set[i];
    }
    return NULL;
}

const ux_api_bt_device_info_t* ux_api_bt_get_device_info(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol,
    uint16_t *device_num)
{
    if (role >= UX_API_BT_AS_MAX || protocol >= UX_API_BT_PROTOCOL_MAX || device_num == NULL) {
        ux_log_err("[ux_api_bt_get_device_info] invalid param, role=%d, protocol=%d.\n", role, protocol);
        return NULL;
    }
    return ux_bt_get_device_info_writable(role, protocol, device_num);
}

const ux_api_bt_device_info_t* ux_api_bt_get_device_info_by_addr(ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    if (addr == NULL || protocol >= UX_API_BT_PROTOCOL_MAX) {
        ux_log_err("[ux_api_bt_audio_get_info] invalid param, protocol=%d.\n", protocol);
        return NULL;
    }
    ux_api_bt_device_info_t *dev_info = NULL;
    uint16_t dev_num = 0;
    for (ux_api_bt_role_t role = UX_API_BT_AS_CENTRAL; role < UX_API_BT_AS_MAX; role++) {
        for (ux_api_bt_protocol_t protocol = UX_API_BT_PROTOCOL_BR; protocol < UX_API_BT_PROTOCOL_MAX; protocol++) {
            dev_info = ux_bt_get_device_info_writable(role, protocol, &dev_num);
            if (dev_info == NULL) {
                continue;
            }
            if ((memcmp(&dev_info->addr, addr, sizeof(dev_info->addr)) == 0) && dev_info->protocol == protocol) {
                return dev_info;
            }
        }
    }
    return NULL;
}

errcode_t ux_api_bt_get_device_info_by_id(uint8_t device_id, ux_api_bt_device_info_t *device_info)
{
    if (device_info == NULL) {
        ux_log_err("[ux_api_bt_get_device_info_by_id] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_bg_device_info_t* inner_dev_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), device_id);
    if (inner_dev_info == NULL) {
        ux_log_err("[ux_api_bt_get_device_info_by_id] no this device %hhu.\n", device_id);
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    int err = memcpy_s(&device_info->addr, sizeof(ux_api_bt_addr_t), &inner_dev_info->addr, sizeof(ux_api_bt_addr_t));
    if (err) {
        ux_log_err("[ux_api_bt_get_device_info_by_id] memcpy_s addr err.\n");
    }
    device_info->device_class = inner_dev_info->device_class;
    err = memcpy_s(device_info->name, sizeof(device_info->name), inner_dev_info->name, sizeof(inner_dev_info->name));
    if (err) {
        ux_log_err("[ux_api_bt_get_device_info_by_id] memcpy_s name err.\n");
    }
    device_info->protocol = inner_dev_info->protocol;
    device_info->role = inner_dev_info->role;
    device_info->acl_state = inner_dev_info->connect_state;
    return UX_ERR_SUCCESS;
}
static void notify_delete_weakest_rssi_device(const ux_bg_device_info_t *min_rssi_node)
{
    bs_br_discovery_result_t del_device_info = {0};
    int err = memcpy_s(&del_device_info.addr, sizeof(del_device_info.addr),
        &min_rssi_node->addr, sizeof(min_rssi_node->addr));
    if (err != EOK) {
        ux_log_err("[notify_delete_weakest_rssi_device] memcpy_s addr fail err=0x%x.\n", err);
    }
    err = strcpy_s((char*)del_device_info.name, sizeof(del_device_info.name),
        min_rssi_node->name);
    if (err != EOK) {
        ux_log_err("[notify_delete_weakest_rssi_device] strcpy_s name fail err=0x%x.\n", err);
    }
    del_device_info.device_class = min_rssi_node->device_class;
    del_device_info.rssi = min_rssi_node->rssi;
    bool need_to_del = true;
    
    ux_adapter_bt_scan_complete_process(&del_device_info, need_to_del);
}

static void notify_add_new_device(const ux_bg_device_info_t *new_device)
{
    bs_br_discovery_result_t del_device_info = {0};
    int err = memcpy_s(&del_device_info.addr, sizeof(del_device_info.addr),
        &new_device->addr, sizeof(new_device->addr));
    if (err != EOK) {
        ux_log_err("[notify_add_new_device] memcpy_s addr fail err=0x%x.\n", err);
    }
    err = strcpy_s((char*)del_device_info.name, sizeof(new_device->name), (char*)new_device->name);
    if (err != EOK) {
        ux_log_err("[notify_add_new_device] strcpy_s name fail err=0x%x.\n", err);
    }
    del_device_info.device_class = new_device->device_class;
    del_device_info.rssi = new_device->rssi;
    bool need_to_del = false;
    ux_adapter_bt_scan_complete_process(&del_device_info, need_to_del);
}

void replace_weakest_rssi_device(ux_bg_device_list_t *device_list, ux_bg_device_info_t *new_device)
{
    ux_bg_device_info_t *cur_node =  device_list->head;
    ux_bg_device_info_t *min_rssi_node = device_list->head;
    // 找到最小的节点
    while (cur_node != NULL) {
        if (cur_node->rssi < min_rssi_node->rssi) {
            min_rssi_node = cur_node;
        }
        cur_node = (ux_bg_device_info_t*)cur_node->next;
    }
    if (new_device->rssi < min_rssi_node->rssi) {
        // 若待插入设备rssi比最小的还小, 不插入
        ux_log_info("[replace_weakest_rssi_device] throw low rssi, name:%s, addr:****%02x%02x%02x%02x.\n",
            new_device->name, new_device->addr.addr[BG_ADDR_INDEX_3], new_device->addr.addr[BG_ADDR_INDEX_2],
            new_device->addr.addr[BG_ADDR_INDEX_1], new_device->addr.addr[BG_ADDR_INDEX_0]);
        return;
    } else {
        notify_delete_weakest_rssi_device(min_rssi_node);
        // 替换最小的节点
        new_device->next = min_rssi_node->next;
        int err = memcpy_s(min_rssi_node, sizeof(ux_bg_device_info_t), new_device, sizeof(ux_bg_device_info_t));
        if (err != EOK) {
            ux_log_err("[replace_weakest_rssi_device] memcpy_s min_rssi_node fail err=0x%x.\n", err);
        }
        notify_add_new_device(new_device);
    }
}

/* 在链表新增结点, 当数量超过上限, 替换rssi最小的，当数量不超过上限, 在末尾插入 */
void ux_bt_insert_device_with_rssi_filter(ux_bg_device_list_t *device_list, ux_bg_device_info_t *new_device)
{
    if (device_list == NULL || new_device == NULL) {
        ux_log_err("[ux_bg_insert_device_with_rssi_filter] nullptr.\n");
        return;
    }
    if (device_list->device_num >= UX_BG_MAX_DEVICE_NUM) {
        replace_weakest_rssi_device(device_list, new_device);
    } else {
        /* 在尾部插入 */
        ux_bg_device_info_t *new_node = ux_bg_append_blank_device_info(device_list);
        if (new_node == NULL) {
            return;
        }
        int err = memcpy_s(new_node, sizeof(ux_bg_device_info_t), new_device, sizeof(ux_bg_device_info_t));
        if (err != EOK) {
            ux_log_err("[ux_bg_insert_device_with_rssi_prune] memcpy_s ux_bg_device_info_t fail err=0x%x.\n", err);
        }
        new_node->next = NULL;
        notify_add_new_device(new_device);
    }
}

errcode_t ux_bt_get_addr_by_device_id(uint16_t dev_id, ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_bt_get_addr_by_device_id] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    bs_addr_t *bs_addr = bs_get_addr_by_device_id((uint8_t)dev_id);
    if (bs_addr == NULL) {
        ux_log_err("[ux_bt_get_addr_by_device_id] no this device %hhu.\n", dev_id);
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    if (memcpy_s(addr->addr, sizeof(addr->addr), bs_addr->addr, sizeof(addr->addr)) != EOK) {
        ux_log_err("[ux_bt_get_addr_by_device_id] memcpy fail.\n");
        return UX_ERR_MEMCPY_FAILED;
    }
    addr->type = bs_addr->type;
    return UX_ERR_SUCCESS;
}

uint16_t ux_bt_find_device_id_by_addr(ux_api_bt_protocol_t protocol, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        return UX_BT_CONNECTION_UPPER_LIMIT;
    }
    bs_protocol_type_t bth_protocol = BS_PROTOCOL_BLE;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
            bth_protocol = BS_PROTOCOL_BREDR;
            break;
        case UX_API_BT_PROTOCOL_BLE:
            bth_protocol = BS_PROTOCOL_BLE;
            break;
        default:
            return UX_BT_CONNECTION_UPPER_LIMIT;
    }
    return (uint16_t)bs_find_device_id_by_addr(bth_protocol, addr->type, addr->addr);
}

static void ux_bt_heartbeat_monitor_cbk(uint32_t param)
{
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_BT_CORE_CRASH, (uint8_t*)&param, sizeof(param));
}

static errcode_t ux_bt_disconnect_gap(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    ux_bg_device_info_t* dev_info = NULL;
    dev_info = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, protocol);
    if (dev_info == NULL) {
        ux_log_err("[ux_bt_disconnect_gap] Ux can not find addr.\n");
        return UX_ERR_BG_NON_THIS_DEVICE;
    }
    /* 调用bth接口，进行断连 */
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            err = bs_br_disconnect((uint8_t)dev_info->id);
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            err = bs_ble_disconnect((uint8_t)dev_info->id);
#endif
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_disconnect_gap] fail to disconnect, protocol:%d, err:0x%x.\n", protocol, err);
        return err;
    }
    dev_info->connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTING;
    return UX_ERR_SUCCESS;
}

static errcode_t reconnect_device(ux_bg_device_info_t *dev_info)
{
    if (ux_bt_active_device_is_full(dev_info->role, dev_info->protocol)) {
        ux_log_err("[ux][reconnect_device] active dev has full.\n");
        return UX_ERR_BG_PROFILE_HAD_CONNECTED;
    }
    ux_api_bt_profile_state_t connect_state = dev_info->connect_state;
    // 重入当连接成功
    if (connect_state == UX_API_BT_PROFILE_STATE_CONNECTED || connect_state == UX_API_BT_PROFILE_STATE_CONNECTING) {
        return UX_ERR_SUCCESS;
    }
    // 已配对过的设备更新状态
    dev_info->connect_state = UX_API_BT_PROFILE_STATE_CONNECTING;
    // 建立连接
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (dev_info->protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            err = bs_br_start_pair((bs_addr_t*)&dev_info->addr);
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            err = bs_ble_start_pair((bs_addr_t*)&dev_info->addr);
#endif
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][reconnect_device] Fail to reconnect device, err:0x%x.\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

static void init_new_peripheral_device_info(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol,
    uint16_t dev_id, ux_bg_device_info_t *dev_info)
{
    if (memcpy_s(&dev_info->addr, sizeof(dev_info->addr), addr, sizeof(ux_api_bt_addr_t)) != EOK) {
        ux_log_err("[ux][append_new_peripheral_device] Fail to memcpy dev info.\n");
    }
    dev_info->connect_state = UX_API_BT_PROFILE_STATE_CONNECTING;
    dev_info->role = UX_API_BT_AS_PERIPHERAL;
    dev_info->is_valid = true;
    dev_info->protocol = protocol;
    dev_info->id = dev_id;
    dev_info->enc_state = false;
}

static ux_bg_device_info_t* append_new_peripheral_device(const ux_api_bt_addr_t *addr,
    ux_api_bt_protocol_t protocol, uint16_t dev_id)
{
    ux_bg_device_info_t *new_device = ux_bg_append_blank_device_info(ux_bg_get_paired_device_list());
    if (new_device == NULL) {
        ux_log_err("[ux][append_new_peripheral_device] Fail to append device info.\n");
        return NULL;
    }
    init_new_peripheral_device_info(addr, protocol, dev_id, new_device);
    new_device->next = NULL;
    return new_device;
}

static errcode_t pair_device_after_connect(ux_bg_device_info_t *dev_info)
{
    // 配对
    errcode_t err = UX_ERR_NOT_SUPPORT;
#ifdef CONFIG_SUPPORT_BLE
    err = bs_ble_start_pair((bs_addr_t*)&dev_info->addr);
#endif
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][pair_device_after_connect] Fail to reconnect device, err:0x%x.\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

static errcode_t pair_new_peripheral_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    if (ux_bt_active_device_is_full(UX_API_BT_AS_PERIPHERAL, protocol)) {
        ux_log_err("[ux][pair_new_peripheral_device] active dev has full.\n");
        return UX_ERR_BG_PROFILE_HAD_CONNECTED;
    }
    /* 申请dev_id */
    bs_protocol_type_t bs_protocol = BS_PROTOCOL_BREDR;
    if (protocol == UX_API_BT_PROTOCOL_BLE) {
        bs_protocol = BS_PROTOCOL_BLE;
    } else if (protocol == UX_API_BT_PROTOCOL_BR) {
        bs_protocol = BS_PROTOCOL_BREDR;
    }

    uint16_t dev_id = bs_alloc_device_id_by_addr(bs_protocol, (uint8_t)addr->type, addr->addr);
    ux_log_debug("[ux][pair_new_peripheral_device] alloc id=%d.\n", dev_id);
    if (dev_id == BS_CONNECTION_UPPER_LIMIT) {
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    ux_bg_device_info_t *dev_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
    if (dev_info == NULL) { // 新设备插表
        dev_info = append_new_peripheral_device(addr, protocol, dev_id);
        if (dev_info == NULL) {
            ux_log_err("[ux][pair_new_peripheral_device] append_new_peripheral_device fail.\n");
            return UX_ERR_MALLOC_FAILED;
        }
    } else { // 新申请的id和旧设备相同, 旧设备失效, 覆盖旧设备
        ux_log_debug("[ux][pair_new_peripheral_device]cover old dev.\n");
        init_new_peripheral_device_info(addr, protocol, dev_id, dev_info);
    }
    // 配对
    errcode_t err = UX_ERR_NOT_SUPPORT;
    if (protocol >= UX_API_BT_PROTOCOL_BR && protocol < UX_API_BT_PROTOCOL_MAX) {
        err = bs_pair_func_set[protocol]((bs_addr_t*)addr);
    }
    /* 发送连接报文失败，移除alloc的id，释放内存 */
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[pair_new_peripheral_device] Fail to start pair err0x%x.\n", err);
        ux_api_bt_remove_device(addr, protocol);
        return err;
    }
    /* 删除扫描设备列表内的设备信息 */
    ux_bg_delete_device_info_by_addr(ux_bg_get_scan_device_list(), addr, protocol);
    return UX_ERR_SUCCESS;
}

static errcode_t get_a2dp_connect_state(const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state)
{
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, 0, addr->addr);
    if (dev_id == BS_CONNECTION_UPPER_LIMIT) {
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
#ifdef CONFIG_SUPPORT_A2DP_SINK
    ux_bt_a2dp_info_t *sink_info = ux_bt_a2dp_get_sink_info();
    if (dev_id == sink_info->device_id) {
        *profile_state = sink_info->profile_state;
        return UX_ERR_SUCCESS;
    }
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
    ux_bt_a2dp_info_t *src_info = ux_bt_a2dp_get_src_info();
    if (dev_id == src_info->device_id) {
        *profile_state = src_info->profile_state;
        return UX_ERR_SUCCESS;
    }
#endif
    return UX_ERR_NOT_SUPPORT;
}

static errcode_t get_avrcp_connect_state(const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state)
{
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, 0, addr->addr);
    if (dev_id >= BS_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux][get_avrcp_connect_state] invalid param, devid=%d.\n", dev_id);
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
#ifdef CONFIG_SUPPORT_AVRCP
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    *profile_state = avrcp_info[dev_id].profile_state;
    return UX_ERR_SUCCESS;
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t get_br_connect_state(const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state)
{
    ux_bg_device_info_t *device_info = NULL;
    device_info = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (device_info == NULL) {
        ux_log_err("[ux][get_br_connect_state] Nullptr.\n");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    *profile_state = device_info->connect_state;
    return UX_ERR_SUCCESS;
}

/* 对外接口，使能bt，包含br以及ble */
errcode_t ux_api_bt_enable(ux_api_bt_protocol_t protocol)
{
    errcode_t ret = bts_register_bt_heartbeat_monitor(ux_bt_heartbeat_monitor_cbk);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_enable] Fail to register bt monitor 0x%x.\n", ret);
    }

    ret = UX_ERR_NOT_SUPPORT;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            ret = ux_bt_ble_init();
#endif
            break;
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            ret = ux_bt_br_init();
#endif
            break;
        default:
            ret = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_enable] Fail to enable, protocol:%d, err:0x%x.\n", protocol, ret);
        return ret;
    }
    return UX_ERR_SUCCESS;
}

/* 对外接口，去使能bt，包含br以及ble */
errcode_t ux_api_bt_disable(ux_api_bt_protocol_t protocol)
{
    ux_log_err("[ux_api_bt_disable]protocol=%d", protocol);
    errcode_t ret = UX_ERR_NOT_SUPPORT;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            ret = ux_bt_ble_deinit();
#endif
            break;
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            ret = ux_bt_br_deinit();
#endif
            break;
        default:
            ret = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_disable] Fail to disable, protocol:%d, err:0x%x.\n", protocol, ret);
        return ret;
    }
    return UX_ERR_SUCCESS;
}

/* 对外接口，启动discovery */
errcode_t ux_api_bt_start_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    ux_log_info("[ux_api_bt_start_discovery] role=%d, protocol=%d.\n", role, protocol);
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            err = ux_bt_br_switch_discovery(role, UX_BT_DISCOVERY_START);
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            err = ux_bt_ble_switch_discovery(role, UX_BT_DISCOVERY_START);
#endif
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    return err;
}

/* 对外接口，停止discovery */
errcode_t ux_api_bt_stop_discovery(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    ux_log_info("[ux_api_bt_stop_discovery] role=%d, protocol=%d.\n", role, protocol);
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            err = ux_bt_br_switch_discovery(role, UX_BT_DISCOVERY_STOP);
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            err = ux_bt_ble_switch_discovery(role, UX_BT_DISCOVERY_STOP);
#endif
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    return err;
}

/* 对外接口，配对设备(隐式包含了connect) */
errcode_t ux_api_bt_pair_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    if (addr == NULL || protocol >= UX_API_BT_PROTOCOL_MAX || protocol < UX_API_BT_PROTOCOL_BR) {
        ux_log_err("[ux_api_bt_pair_device] invalid param.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ux_log_info("[ux_api_bt_pair_device] addr:****%02x%02x%02x%02x, type:%u, protocol:%u.\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], addr->type, protocol);
    // 查找配对记录
    ux_bg_device_info_t *dev_info = NULL;
    dev_info = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, protocol);
    // 若存在配对记录，而且打开一键双连，配对ble
    if (dev_info != NULL && dev_info->protocol == UX_API_BT_PROTOCOL_BLE &&
        dev_info->connect_state == UX_API_BT_PROFILE_STATE_CONNECTED && ux_bt_ctkd_is_open()) {
        /* 连接未配对，在连接后配对配对 */
#ifdef CONFIG_SUPPORT_BLE
        bool flag = false;
        errcode_t err = ux_api_bt_config(UX_API_BT_CONFIG_PAIR_USE_CTKD, &flag, sizeof(bool));
        ux_log_debug("[ux_api_bt_pair_device] close ctkd, ret=0x%x.\n", err);
        return pair_device_after_connect(dev_info);
#else
        return UX_ERR_NOT_SUPPORT;
#endif
    } else if (dev_info != NULL) {
        /* 已配对，回连 */
        return reconnect_device(dev_info);
    }
    /* 新设备，配对 */
    errcode_t err = pair_new_peripheral_device(addr, protocol);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_pair_device] pair_new_peripheral_device 0x%x.\n", err);
        return err;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_api_ble_connect(const ux_api_bs_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_ble_connect] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_api_bt_addr_t bt_addr = {0};
    bt_addr.type = (uint8_t)addr->type;
    if (memcpy_s(bt_addr.addr, UX_API_BT_ADDR_LEN, addr->addr, UX_API_BT_ADDR_LEN) != EOK) {
        ux_log_err("[ux_api_ble_connect] memcpy_s fail\n");
        return UX_ERR_MEMCPY_FAILED;
    }
    errcode_t err = ux_api_bt_pair_device(&bt_addr, UX_API_BT_PROTOCOL_BLE);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_ble_connect] pair_device fail, err:0x%x.\n", err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

/* 对外接口，设备连接，主要用于发起acl及各服务的连接 */
errcode_t ux_api_bt_connect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_connect] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_connect] addr:****%02x%02x%02x%02x, type:%u, profile:%u.\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], addr->type, profile);
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (profile) {
        case UX_API_BT_PROFILE_BR:
            err = ux_api_bt_pair_device(addr, UX_API_BT_PROTOCOL_BR);
            break;
        case UX_API_BT_PROFILE_BLE:
            err = ux_api_bt_pair_device(addr, UX_API_BT_PROTOCOL_BLE);
            break;
        case UX_API_BT_PROFILE_SCO:
#if defined(CONFIG_SUPPORT_HFP_HF) || defined(CONFIG_SUPPORT_HFP_AG)
            err = ux_bt_hal_sco_create_connection(addr);
#endif
            break;
        case UX_API_BT_PROFILE_A2DP:
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
            err = ux_bt_a2dp_connect((ux_api_bt_addr_t*)addr, sizeof(ux_api_bt_addr_t));
#endif
            break;
        case UX_API_BT_PROFILE_HFP:
            break;
        case UX_API_BT_PROFILE_AVRCP:
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_connect] fail to connect, prifile:%d, err:0x%x.\n", profile, err);
        return err;
    }
    return UX_ERR_SUCCESS;
}

/* 对外接口，设备断开连接（不删除配置） */
errcode_t ux_api_bt_disconnect(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_disconnect] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_disconnect] addr:****%02x%02x%02x%02x, type:%u, profile:%u.\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0], addr->type, profile);
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (profile) {
        case UX_API_BT_PROFILE_BR:
            err = ux_bt_disconnect_gap(addr, UX_API_BT_PROTOCOL_BR);
            break;
        case UX_API_BT_PROFILE_BLE:
            err = ux_bt_disconnect_gap(addr, UX_API_BT_PROTOCOL_BLE);
            break;
        case UX_API_BT_PROFILE_SCO:
#ifdef CONFIG_SUPPORT_HFP_HF
            err = ux_bt_hal_sco_release_connection(addr);
#endif
            break;
        case UX_API_BT_PROFILE_A2DP:
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
            err = ux_bt_a2dp_disconnect((ux_api_bt_addr_t*)addr, sizeof(ux_api_bt_addr_t));
#endif
            break;
        case UX_API_BT_PROFILE_HFP:
            break;
        case UX_API_BT_PROFILE_AVRCP:
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_disconnect] fail to disconnect, prifile:%d, err:0x%x.\n", profile, err);
        return err;
    }
    return err;
}

errcode_t ux_api_bt_remove_device(const ux_api_bt_addr_t *addr, ux_api_bt_protocol_t protocol)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_disconnect] nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux][bt]ux_api_bt_remove_device start:****%02x%02x%02x%02x, addr_type[%d], profile[%d]\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], addr->type, protocol);
    uint16_t device_id = ux_bt_find_device_id_by_addr(protocol, addr);
    /* 找不到设备视为失败 */
    if (device_id == UX_BT_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_api_bt_remove_device] Can not find device.");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    errcode_t err = UX_ERR_SUCCESS;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            err = bs_br_remove_pair((uint8_t)device_id);
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            err = bs_ble_remove_pair((uint8_t)device_id);
#endif
            break;
        default:
            err = UX_ERR_INVALID_PARAMETER;
            break;
    }
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_api_bt_remove_device] Fail to remove device, protocol:%d, err:0x%x.", protocol, err);
        return err;
    }
    return err;
}

errcode_t ux_api_bt_get_profile_state(ux_api_bt_profile_t profile,
    const ux_api_bt_addr_t *addr, ux_api_bt_profile_state_t *profile_state)
{
    if (addr == NULL || profile_state == NULL) {
        ux_log_err("[ux_api_bt_get_profile_state] Nullptr.");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_get_profile_state] addr:****%02x%02x%02x%02x, type:%u, profile:%u.\n",
        addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
        addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0], addr->type, profile);
    switch (profile) {
        case UX_API_BT_PROFILE_BR:
            return get_br_connect_state(addr, profile_state);
        case UX_API_BT_PROFILE_A2DP:
            return get_a2dp_connect_state(addr, profile_state);
        case UX_API_BT_PROFILE_AVRCP:
            return get_avrcp_connect_state(addr, profile_state);
        default:
            return UX_ERR_NOT_SUPPORT;
            break;
    }
}

void ux_bt_update_cur_device_info_from_node(const ux_bg_device_info_t *device_node)
{
    ux_api_bt_device_info_t *dev_info = NULL;
    uint16_t dev_num = 0;
    dev_info = ux_bt_get_device_info_writable(device_node->role, device_node->protocol, &dev_num);
    if (dev_info == NULL || device_node == NULL) {
        ux_log_err("[ux_bt_update_cur_device_info_from_node] device not exist.\n");
        return;
    }
    /* 断连仅改变链路状态 */
    if (device_node->connect_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        dev_info->acl_state = device_node->connect_state;
        ux_log_err("[ux_bt_update_cur_device_info_from_node] device disconnect.\n");
        return;
    }
    int err = UX_ERR_SUCCESS;
    dev_info->acl_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
    dev_info->device_class = device_node->device_class;
    if (memcmp(&dev_info->addr, &device_node->addr, sizeof(device_node->addr)) != 0) {
        err =  memcpy_s(&dev_info->addr, sizeof(dev_info->addr),
            &device_node->addr, sizeof(device_node->addr));
        if (err != EOK) {
            ux_log_err("[ux][ux_bt_update_cur_device_info_from_node] memcpy_s addr fail, err=%d.\n", err);
        }
    }
    if (memcmp(dev_info->name, device_node->name, sizeof(dev_info->name)) != 0) {
        err = strcpy_s(dev_info->name, sizeof(dev_info->name), device_node->name);
        if (err != EOK) {
            ux_log_err("[ux][ux_bt_update_cur_device_info_from_node] strcpy_s name fail, err=%d.\n", err);
        }
    }
}

void ux_bt_clear_cur_device_info(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    uint16_t dev_num;
    ux_api_bt_device_info_t *dev_info = NULL;
    dev_info = ux_bt_get_device_info_writable(role, protocol, &dev_num);
    if (dev_info == NULL) {
        ux_log_err("[ux_bt_clear_cur_device_info] device not exist.\n");
        return;
    }
    memset_s(&dev_info->addr, sizeof(dev_info->addr), 0, sizeof(dev_info->addr));
    dev_info->acl_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
}

bool ux_bt_active_device_is_full(ux_api_bt_role_t role, ux_api_bt_protocol_t protocol)
{
    if (ux_api_bt_is_multiple_device_open()) {
        // 若打开了多设备, 则不限制设备数量
        return false;
    }
    ux_bg_device_list_t *device_list = ux_bg_get_paired_device_list();
    if (device_list == NULL) {
        return false;
    }
    ux_bg_device_info_t *cur_node =  device_list->head;
    // 规格约束，允许连接1个外围设备和1个中心设备
    while (cur_node != NULL) {
        ux_api_bt_profile_state_t connect_state = cur_node->connect_state;
        if (cur_node->role == role && cur_node->protocol == protocol &&
            (connect_state == UX_API_BT_PROFILE_STATE_CONNECTED)) {
            return true;
        }
        cur_node = (ux_bg_device_info_t*)cur_node->next;
    }
    return false;
}