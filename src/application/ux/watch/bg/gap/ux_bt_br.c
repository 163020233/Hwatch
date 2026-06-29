/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux bg br \n
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
#include "bs_br_service_acquire.h"
#include "bs_device_mgr.h"
#include "osal_mutex.h"
#include "ux_bg_config.h"
#include "ux_bt.h"
#include "ux_device_manager.h"
#include "bs_br_service_config.h"
#include "bts_br_gap.h"
#include "property.h"
#include "ux_api_bt.h"
#include "bs_br_service_api.h"
#include "bts_br_gap.h"
#include "bts_def.h"
#include "ux_api_fwk.h"
#include "ux_event_def.h"
#include "ux_hal_memory.h"
#ifdef CONFIG_SUPPORT_HFP_HF
#include "ux_bt_call_hf.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
#include "ux_bt_a2dp_sink.h"
#endif
#ifdef CONFIG_SUPPORT_AVRCP
#include "ux_bt_avrcp.h"
#endif
#ifdef CONFIG_SUPPORT_PBAP
#include "ux_bt_pbap.h"
#endif
#include "adapter_bt_br.h"
#include "ux_spp_service.h"
#include "ux_bt_br.h"

#define BT_STR_END_LEN   1
#define BT_PAIRED_DEVICES_MAX 4
#define BR_DEVICE_TYPE_WATCH 0x600704
#define BR_DEVICE_TYPE_PHONE 0x200
#define UX_BT_DISCONNECT_REASON_AUTH_FAIL 0X05
static osal_mutex g_br_stack_info_mutex;

// 自动起蓝牙服务的配置
bs_br_config_support_profile_t g_bt_service_role_set = {
#ifdef CONFIG_SUPPORT_HFP_HF
    .hfp_hf = 0,
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
    .a2dp_snk = 1,
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
    .a2dp_src = 1,
#endif
#ifdef CONFIG_SUPPORT_AVRCP
    .avrcp_ct = 1,
    .avrcp_tg = 1,
#endif
#ifdef CONFIG_SUPPORT_PBAP
    .pbap = 1,
#endif
};

// 自动连接服务的配置
bs_br_config_connect_profile_t g_bt_service_auto_connect_set = {
#ifdef CONFIG_SUPPORT_AUTO_CONNECT_HFP_HF
    .hfp_hf = 1,
#endif
#ifdef CONFIG_SUPPORT_AUTO_CONNECT_A2DP_SRC
    .a2dp_src = 1,
#endif
#ifdef CONFIG_SUPPORT_AUTO_CONNECT_A2DP_SINK
    .a2dp_snk = 1,
#endif
#ifdef CONFIG_SUPPORT_AUTO_CONNECT_AVRCP
    .avrcp_ct = 1,
    .avrcp_tg = 1,
#endif
#ifdef CONFIG_SUPPORT_PBAP
    .pbap = 0,
#endif
};

static ux_bt_stack_info_t g_br_stack_info = {
    .adv_status = UX_BT_DISCOVERY_STOP,
    .scan_status = UX_BT_DISCOVERY_STOP,
    .stack_status = UX_BT_STACK_DEINITED,
};

ux_bt_stack_info_t *ux_bt_br_get_stack_info(void)
{
    return &g_br_stack_info;
}

ux_api_bt_local_info_t* ux_bt_br_get_local_info(void)
{
    static ux_api_bt_local_info_t s_ux_br_local_info = {0};
    errcode_t err = UX_ERR_SUCCESS;
    if (property_get_bytes(NV_ID_UX_BT_NAME, (uint8_t *)s_ux_br_local_info.name,
        sizeof(s_ux_br_local_info.name)) == NULL) {
        err = (errcode_t)strcpy_s(s_ux_br_local_info.name, sizeof(s_ux_br_local_info.name), ux_get_default_bt_name());
        if (err != EOK) {
            ux_log_err("[ux_bt_br_get_local_info] get_name ret=%d.\n", err);
        }
    }
    err = ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, s_ux_br_local_info.addr.addr, sizeof(s_ux_br_local_info.addr.addr));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_br_get_local_info] load addr ret=0x%x.\n", err);
    }
    if (ux_bt_br_get_stack_info()->stack_status == UX_BT_STACK_INITIED) {
        s_ux_br_local_info.enable = true;
    } else {
        s_ux_br_local_info.enable = false;
    }
    return &s_ux_br_local_info;
}

static void update_cur_device_info_from_nv(const bs_br_acquire_t *nv_device_info)
{
    ux_api_bt_device_info_t *dev_info = NULL;
    uint16_t dev_num;
    if ((nv_device_info->bond_device_info.class_of_device & BR_DEVICE_TYPE_PHONE) == BR_DEVICE_TYPE_PHONE) {
        dev_info = ux_bt_get_device_info_writable(UX_API_BT_AS_CENTRAL, UX_API_BT_PROTOCOL_BR, &dev_num);
    } else {
        dev_info = ux_bt_get_device_info_writable(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR, &dev_num);
    }
    dev_info->acl_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
    dev_info->device_class = nv_device_info->bond_device_info.class_of_device;
    dev_info->addr.type = UX_API_BT_ADDR_TYPE_PUBLIC;
    int err =  memcpy_s(dev_info->addr.addr, sizeof(dev_info->addr.addr),
        nv_device_info->bond_device_info.addr, sizeof(nv_device_info->bond_device_info.addr));
    if (err != EOK) {
        ux_log_err("[ux][update_cur_device_info_from_nv] memcpy_s addr fail, err=%d.\n", err);
    }
    err = strcpy_s(dev_info->name, sizeof(dev_info->name), (char*)nv_device_info->bond_device_info.name);
    if (err != EOK) {
        ux_log_err("[ux][update_cur_device_info_from_nv] strcpy_s name fail, err=%d.\n", err);
    }
}

// 从nv存储中恢复配对设备信息
static void load_paired_devices_from_storage(void)
{
    uint16_t list_len = BT_PAIRED_DEVICES_MAX;
    bs_br_acquire_t *dev_infos = (bs_br_acquire_t *)malloc(sizeof(bs_br_acquire_t) * list_len);
    if (dev_infos == NULL) {
        ux_log_err("[ux][bg][load_paired_devices_from_storage]dev_infos malloc error!\n");
        return;
    }
    errcode_t ret = bs_acquire(BR_ACQUIRE_REMOTE_BOND_LIST, dev_infos, &list_len);
    if (ret != 0) {
        ux_log_err("[ux][bg][load_paired_devices_from_storage]bs_acquire erro ret = %x\n", ret);
        goto clean;
    }

    for (int i = 0; i < list_len && i < BT_PAIRED_DEVICES_MAX; i++) {
        update_cur_device_info_from_nv(&dev_infos[i]);
        ux_bg_device_info_t *find_device_res = ux_bg_append_blank_device_info(ux_bg_get_paired_device_list());
        if (find_device_res == NULL) {
            ux_log_err("[ux][bg][load_paired_devices_from_storage ] append to paired device list fail!\n");
            goto clean;
        }
        ux_log_info("[ux][bg][load_paired_devices_from_storage] addr:****%02x%02x%02x%02x, device_class=%d.\n",
            dev_infos[i].bond_device_info.addr[BG_ADDR_INDEX_3], dev_infos[i].bond_device_info.addr[BG_ADDR_INDEX_2],
            dev_infos[i].bond_device_info.addr[BG_ADDR_INDEX_1], dev_infos[i].bond_device_info.addr[BG_ADDR_INDEX_0],
            (uint8_t)dev_infos[i].bond_device_info.class_of_device);
        find_device_res->role = (((dev_infos[i].bond_device_info.class_of_device) & BR_DEVICE_TYPE_PHONE) == 0) ?
            UX_API_BT_AS_PERIPHERAL : UX_API_BT_AS_CENTRAL;
        find_device_res->protocol = UX_API_BT_PROTOCOL_BR;
        find_device_res->id = (uint8_t)dev_infos[i].bond_device_info.dev_id;
        find_device_res->device_class = (uint8_t)dev_infos[i].bond_device_info.class_of_device;
        find_device_res->name[BS_BR_DEVICE_NAME_MAX_LEN - 1] = 0;
        find_device_res->is_valid = true;
        find_device_res->enc_state = false;
        if (strcpy_s(find_device_res->name, BS_BR_DEVICE_NAME_MAX_LEN,
            (const char *)(dev_infos[i].bond_device_info.name))) {
            ux_log_err("[ux][bg][load_paired_devices_from_storage ] strcpy_s fail!\n");
            goto clean;
        }
        if (memcpy_s(find_device_res->addr.addr, UX_API_BT_ADDR_LEN, dev_infos[i].bond_device_info.addr, BS_ADDR_LEN)) {
            ux_log_err("[ux][bg][load_paired_devices_from_storage ] memcpy_s fail!\n");
            goto clean;
        }
        find_device_res->addr.type = UX_API_BT_ADDR_TYPE_PUBLIC;
        find_device_res->connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
        ux_log_info("[ux][bg][load_paired_devices_from_storage]load paired devices[%s] !\n", find_device_res->name);
    }
    ux_log_info("[ux][bg][load_paired_devices_from_storage]load paired devices list success!");
    goto clean;
clean:
    free(dev_infos);
    return;
}

static void process_br_enable_result(void)
{
    ux_bt_stack_info_t *br_info = ux_bt_br_get_stack_info();
    osal_mutex_lock(&g_br_stack_info_mutex);
    br_info->stack_status = UX_BT_STACK_INITIED;
    osal_mutex_unlock(&g_br_stack_info_mutex);

    errcode_t ret = UX_ERR_SUCCESS;
    ret = bs_br_start_auto_conn();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][process_br_enable_result] Fail to start auto connect.\n");
    }
    load_paired_devices_from_storage(); // 开机后从存储中恢复出已配对设备的列表
    /* 初始化服务 */
    typedef errcode_t (*init_profile_handle)(void);
    init_profile_handle init_handle_set[] = {
#ifdef CONFIG_SUPPORT_HFP_HF
        ux_bt_hf_init,
#endif
#ifdef  CONFIG_SUPPORT_AVRCP
        ux_bt_avrcp_init,
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
        ux_bt_a2dp_init,
#endif
#ifdef CONFIG_SUPPORT_UX_SPP
        ux_spp_service_init, // ux的数传通道初始化
#endif
#ifdef CONFIG_SUPPORT_PBAP
        ux_bt_pbap_init,
#endif
    };
    for (uint8_t i = 0; i < sizeof(init_handle_set) / sizeof(init_handle_set[0]); i++) {
        ret = init_handle_set[i]();
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux][process_br_enable_result]Fail to init, idx=%d, ret=0x%x.\n", i, ret);
        }
    }
    // 协议栈启动后启动广播
    ret = ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][bg][process_br_enable_result] Fail to start br adv 0x%x.\n", ret);
    }
    ux_log_info("[ux][process_br_enable_result] br and related service start success.\n");
    ux_adapter_bt_enable_process(BS_PROTOCOL_BREDR);
}

static void process_br_disable_result(void)
{
    ux_bt_stack_info_t *br_info = ux_bt_br_get_stack_info();
    osal_mutex_lock(&g_br_stack_info_mutex);
    br_info->stack_status = UX_BT_STACK_DEINITED;
    osal_mutex_unlock(&g_br_stack_info_mutex);

    osal_mutex_destroy(&g_br_stack_info_mutex);
    g_br_stack_info_mutex.mutex = NULL;
    ux_log_info("[ux][process_br_disable_result] br and related service stop success.\n");
    ux_api_bt_protocol_t protocol = UX_API_BT_PROTOCOL_BR;
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_BT_DISABLE_CB, (uint8_t*)&protocol, sizeof(protocol));
}

static void process_br_stack_state_change_result(uint8_t state)
{
    switch (state) {
        case BR_STATE_ENABLED:
            process_br_enable_result();
            break;
        case BR_STATE_DISABLED:
            process_br_disable_result();
            break;
        default:
            break;
    }
}

static void br_connected_common_process(ux_bg_device_info_t *dev_info,
    bs_br_encrypt_state_t encrypt_state, uint8_t conn_state, uint32_t reason)
{
    ux_log_info("[ux][br_connected_common_process] old_state=%d, new_state=%d, encrypt=%d, reason=%u, cod=0x%x.\n",
        dev_info->connect_state, conn_state, encrypt_state, reason, dev_info->device_class);
    dev_info->connect_state = UX_API_BT_PROFILE_STATE_CONNECTED;
    dev_info->enc_state = ((encrypt_state == BR_ENCRYPT_SUCCESS) ? true : false);
    if (encrypt_state == BR_ENCRYPT_SUCCESS) {
        // 加密成功, 上报消息
        if ((dev_info->device_class & BR_DEVICE_TYPE_PHONE) != 0) {
            /* 连上手机后，停止被发现以避免被其他手机连上 */
            ux_api_bt_stop_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR);
            ux_log_info("[ux][bt]phone connected and stop iscan pscan adv");
        }
        ux_adapter_bt_connect_process((int8_t *)(dev_info->name), &dev_info->addr, reason,
            UX_BT_BR_CONNECTED, dev_info->device_class);
    } else if (encrypt_state == BR_ENCRYPT_FAILED) {
        ux_adapter_bt_connect_process((int8_t *)(dev_info->name), &dev_info->addr, reason,
            UX_BT_BR_DISCONNECTED, dev_info->device_class);
        // 加密失败，移除设备
        ux_api_bt_remove_device(&dev_info->addr, UX_API_BT_PROTOCOL_BR);
        ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR);
    }
}

static void br_disconnected_common_process(ux_bg_device_info_t *dev_info,
    bs_br_encrypt_state_t encrypt_state, uint8_t conn_state, uint32_t reason)
{
    ux_log_info("[ux][br_connected_common_process] old_state=%d, new_state=%d, encrypt=%d, reason=%u, cod=0x%x.\n",
        dev_info->connect_state, conn_state, encrypt_state, reason, dev_info->device_class);
    if (dev_info->connect_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        return;
    }
    if (dev_info->enc_state != true) {
        // 未加密成功的br设备断连不上报
        ux_log_err("[ux][br_connected_common_process] not encrypt, delete device info.\n");
        ux_bg_delete_device_info_by_id(ux_bg_get_paired_device_list(), dev_info->id);
        return;
    }
    if ((dev_info->device_class & BR_DEVICE_TYPE_PHONE) != 0) {
        /* 断连后，再次打开广播，以避免设备无法被发现 */
        ux_api_bt_start_discovery(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR);
        ux_log_info("[ux][bt]phone disconnected and start iscan pscan adv");
    }
    dev_info->connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
    ux_adapter_bt_disconnect_process((int8_t *)(dev_info->name), &dev_info->addr, reason,
        UX_BT_BR_DISCONNECTED, dev_info->device_class);
}

static ux_api_bt_role_t get_role_by_device_class(uint32_t device_class)
{
    if ((device_class & BR_DEVICE_TYPE_PHONE) == BR_DEVICE_TYPE_PHONE) {
        return UX_API_BT_AS_CENTRAL;
    }
    return UX_API_BT_AS_PERIPHERAL;
}

static bool is_inactive_br_central_device(const ux_bg_device_info_t *node, const uint8_t *param, uint16_t len)
{
    unused(param);
    unused(len);
    if (node->role == UX_API_BT_AS_CENTRAL && node->protocol == UX_API_BT_PROTOCOL_BR &&
        node->connect_state == UX_API_BT_PROFILE_STATE_DISCONNECTED) {
        return true;
    }
    return false;
}

static void remove_inactive_br_central_device(void)
{
    ux_bg_device_info_t *old_dev = NULL;
    old_dev = ux_bg_get_device_info(ux_bg_get_paired_device_list(), is_inactive_br_central_device, NULL, 0);
    if (old_dev == NULL) {
        ux_log_info("[ux][bg][process_new_device_connected] old_dev is nullptr!\n");
        return;
    }
    bs_br_remove_pair(old_dev->id);
}

static ux_bg_device_info_t *process_new_device_connected(uint16_t dev_id,
    const bs_br_remote_device_info_t *dev_info)
{
    ux_log_info("[ux][bt][process_new_device_connected] dev_id=%u, device_class=%u, addr:%02x%02x%02x******.\n",
        dev_id, dev_info->class_of_device, dev_info->addr[BG_ADDR_INDEX_0], dev_info->addr[BG_ADDR_INDEX_1],
        dev_info->addr[BG_ADDR_INDEX_2]);
    ux_bg_device_info_t *new_device = ux_bg_append_blank_device_info(ux_bg_get_paired_device_list());
    if (new_device == NULL) {
        ux_log_err("[ux][bg][process_new_device_connected] new_device is nullptr!\n");
        return NULL;
    }
#ifdef DEMO_WATCH
    new_device->role = get_role_by_device_class(dev_info->class_of_device);
#else
    new_device->role = UX_API_BT_AS_CENTRAL;
#endif
    new_device->protocol = UX_API_BT_PROTOCOL_BR;
    new_device->id = dev_id;
    new_device->device_class = dev_info->class_of_device;  // 填充device class上报
    new_device->is_valid = true;
    new_device->connect_state = UX_API_BT_PROFILE_STATE_CONNECTING;
    new_device->enc_state = false;
    /* ble地址可变，在连接后确定，修改地址 */
    new_device->addr.type = UX_API_BT_ADDR_TYPE_PUBLIC;
    if (memcpy_s(new_device->addr.addr, sizeof(new_device->addr.addr), dev_info->addr, sizeof(dev_info->addr)) != EOK) {
        ux_log_err("[ux][bg][process_new_device_connected] Fail to memcpy device addr.\n");
    }

    if (strcpy_s(new_device->name, BS_BR_DEVICE_NAME_MAX_LEN, (const void *)(dev_info->name)) < 0) {
        ux_log_err("[ux][bg][process_new_device_connected] Fail to strcpy_s device name.\r\n");
    }
    if (new_device->role == UX_API_BT_AS_CENTRAL) {
        remove_inactive_br_central_device();
    }
    return new_device;
}

static void process_disconnected_after_remove(uint16_t dev_id)
{
    errcode_t err = bs_br_remove_pair(dev_id);
    ux_log_info("[ux][bt][process_disconnected_after_remove] dev_id=%u, err=0x%x.\n", dev_id, err);
    ux_bg_delete_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
}

static void process_device_connected(ux_bg_device_info_t *ux_device_info, const bs_br_remote_device_info_t *dev_info)
{
    ux_log_info("[ux][bt][process_device_connected] name:%s, addr:%02x%02x%02x******.\n",
        dev_info->name, dev_info->addr[BG_ADDR_INDEX_0], dev_info->addr[BG_ADDR_INDEX_1],
        dev_info->addr[BG_ADDR_INDEX_2]);
    /* 地址可变，在连接后确定，修改地址 */
    ux_device_info->addr.type = UX_API_BT_ADDR_TYPE_PUBLIC;
    if (memcpy_s(ux_device_info->addr.addr, sizeof(ux_device_info->addr.addr),
        dev_info->addr, sizeof(dev_info->addr)) != EOK) {
        ux_log_err("[ux][bg][process_new_device_connected] Fail to memcpy device addr.\n");
    }
    /* 名称可变，修改名称 */
    if (strcpy_s(ux_device_info->name, BS_BR_DEVICE_NAME_MAX_LEN, (const void *)(dev_info->name)) < 0) {
        ux_log_err("[ux][bg][process_new_device_connected] Fail to strcpy_s device name.\r\n");
    }
    ux_device_info->device_class = dev_info->class_of_device;
    ux_device_info->protocol = UX_API_BT_PROTOCOL_BR;
#ifdef DEMO_WATCH
    ux_device_info->role = get_role_by_device_class(dev_info->class_of_device);
#endif
}

static bool central_device_already_full(uint16_t dev_id, uint8_t enc_state, const bs_br_remote_device_info_t *dev_info)
{
    ux_api_bt_role_t role = get_role_by_device_class(dev_info->class_of_device);
    if (role != UX_API_BT_AS_CENTRAL) {
        return false;
    }
    if (ux_bt_active_device_is_full(role, UX_API_BT_PROTOCOL_BR) && enc_state == BR_ENCRYPT_NONE) {
        bs_br_disconnect(dev_id);
        ux_log_err("[ux][bg][process_new_device_connected] device already connected!\n");
        return true;
    }
    return false;
}

static void disconnect_reason_process(const ux_bg_device_info_t *device_info, uint32_t reason)
{
    if (reason == UX_BT_DISCONNECT_REASON_AUTH_FAIL) {
        ux_log_info("[ux][process_br_connect_state_changed_result] auth fail.\n");
        ux_bg_delete_device_info_by_addr(ux_bg_get_paired_device_list(), &device_info->addr, UX_API_BT_PROTOCOL_BR);
    }
}

static void process_br_connect_state_changed_result(uint16_t dev_id, uint8_t conn_state,
                                                    uint8_t enc_state, uint32_t reason)
{
    ux_log_info("[ux][br_conn_state_changed][dev_id=%d][state=%d][enc_state=%d][reason=0x%x].\n",
        dev_id, conn_state, enc_state, reason);
    errcode_t ret = UX_ERR_SUCCESS;
    ux_bg_device_info_t *find_device_res = NULL;
    find_device_res = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
    if (conn_state == BR_CONNECT_CONNECTED) {
        bs_br_remote_device_info_t dev_info = {0};
        ret = bs_br_get_remote_device_info(dev_id, &dev_info);
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[process_br_connect_state_changed_result] gap_br_get_remote_device_info 0x%x.\n", ret);
            return;
        }
        if (central_device_already_full(dev_id, conn_state, &dev_info)) {
            return;
        }
        if (find_device_res == NULL) {
            // 新的中心设备连接
            find_device_res = process_new_device_connected(dev_id, &dev_info);
            if (find_device_res == NULL) {
                ux_log_err("[process_br_connect_state_changed_result] malloc fail.\n");
                return;
            }
            br_connected_common_process(find_device_res, enc_state, conn_state, reason);
        } else {
            // 设备重连
            process_device_connected(find_device_res, &dev_info);
            br_connected_common_process(find_device_res, enc_state, conn_state, reason);
        }
        /* 更新最新的设备信息 */
        ux_bt_update_cur_device_info_from_node(find_device_res);
    } else if (conn_state == BR_CONNECT_DISCONNECTED) {
        if (find_device_res == NULL) {
            ux_log_err("[process_br_connect_state_changed_result] device not exist.\n");
            return;
        } else if (find_device_res->is_valid == false) {
            // 设备移除或断连后的disconnect回调
            ux_log_info("[process_br_connect_state_changed_result] disconnect after remove, enc=%d, valid=%d.\n",
                find_device_res->enc_state, find_device_res->is_valid);
            ux_bt_clear_cur_device_info(find_device_res->role, find_device_res->protocol);
            br_disconnected_common_process(find_device_res, enc_state, conn_state, reason);
            process_disconnected_after_remove(find_device_res->id);
            return;
        } else {
            // 设备断连
            ux_bt_update_cur_device_info_from_node(find_device_res);
            br_disconnected_common_process(find_device_res, enc_state, conn_state, reason);
            disconnect_reason_process(find_device_res, reason);
        }
    }
}

static void process_br_pair_result(uint16_t dev_id, const bs_br_pair_info_t *pair_info)
{
    unused(pair_info);
    ux_log_info("[ux][process_br_pair_result] dev_id: %d\n", dev_id);
}

static void process_br_discover_result(const bs_br_discovery_result_t *discovery_result)
{
    errcode_t ret = UX_ERR_SUCCESS;
    /* 手表场景下，不上报手机的扫描消息 */
#ifdef DEMO_WATCH
    if ((discovery_result->device_class & BR_DEVICE_TYPE_PHONE) != 0) {
        ux_log_debug("watch discover mobile phone");
        return;
    }
#endif
    ux_log_info("[ux][process_br_discover_result] discover divice, name=%s, device_class=0x%x, rssi=%d.\n",
        discovery_result->name, discovery_result->device_class, discovery_result->rssi);
    ux_bg_device_info_t* find_device_res = NULL;
    /* 新设备插表，旧设备不处理 */
    find_device_res = ux_bg_find_device_info_by_addr(ux_bg_get_scan_device_list(),
        (ux_api_bt_addr_t*)&discovery_result->addr, UX_API_BT_PROTOCOL_BR);
    if (find_device_res == NULL) {
        ux_bg_device_info_t *dev_info = ux_hal_memory_acquire(UX_MEM_BT, sizeof(ux_bg_device_info_t));
        if (dev_info == NULL) {
            ux_log_err("[process_br_discover_result] Fail to ux_hal_memory_acquire dev_info.\n");
            return;
        }
        if (memcpy_s(&dev_info->addr, sizeof(dev_info->addr),
            &discovery_result->addr, sizeof(discovery_result->addr)) != EOK) {
            ux_log_err("[process_br_discover_result] Fail to memcpy addr: 0x%x\n", ret);
        }
        if (strcpy_s(dev_info->name, arry_num(dev_info->name), (char*)discovery_result->name) != EOK) {
            ux_log_err("[process_br_discover_result] Fail to strcpy name.\n");
        }
        dev_info->connect_state = UX_API_BT_PROFILE_STATE_MAX;
        dev_info->id = MIN_SCAN_ID + ux_bg_get_scan_device_count();
        dev_info->protocol = UX_API_BT_PROTOCOL_BR;
        dev_info->device_class = discovery_result->device_class;
        dev_info->role = UX_API_BT_AS_PERIPHERAL;
        dev_info->rssi = discovery_result->rssi;
        ux_bg_set_scan_device_count(ux_bg_get_scan_device_count() + 1);
        ux_bt_insert_device_with_rssi_filter(ux_bg_get_scan_device_list(), dev_info);
        ux_hal_memory_release(UX_MEM_BT, dev_info);
    } else {
        // 更新rssi等信息
        find_device_res->rssi = discovery_result->rssi;
        find_device_res->device_class = discovery_result->device_class;
        if (memcmp(find_device_res->name, (char*)discovery_result->name, sizeof(find_device_res->name)) != 0) {
            if (strcpy_s(find_device_res->name, arry_num(find_device_res->name),
                (char*)discovery_result->name) != EOK) {
                ux_log_err("[process_br_discover_result] Fail to strcpy name.\n");
            }
        }
        bool need_to_del = false;
        ux_adapter_bt_scan_complete_process(discovery_result, need_to_del);
    }
}

static void process_br_remove_device_result(uint16_t dev_id)
{
    bs_addr_t *addr = bs_get_addr_by_device_id(dev_id);
    if (addr != NULL) {
        ux_log_info("process br remove device success addr: ****%02x%02x%02x%02x\n",
            addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2],
            addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    }
    ux_bg_device_info_t *cur_dev = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
    if (cur_dev == NULL) {
        ux_log_err("[process_br_remove_device_result] device not exist.\n");
        return;
    } else if (cur_dev->connect_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_info("[process_br_remove_device_result] removed when connected.\n");
        cur_dev->is_valid = false;
    } else {
        // 不处于连接态，直接移除
        ux_log_info("[process_br_remove_device_result] removed when disconnected, connect state:%d.\n",
            cur_dev->connect_state);
        ux_bt_clear_cur_device_info(cur_dev->role, cur_dev->protocol);
        ux_bg_delete_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
    }
}

static void config_br_service(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    bs_config(BR_CONFIG_PROFILE_SUPPORT, &g_bt_service_role_set, 0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][config_br_service] Fail to config br service 0x%x.", err);
        return;
    }
    bs_config(BR_CONFIG_PROFILE_CONNECT, &g_bt_service_auto_connect_set, 0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux][config_br_service] Fail to config br service auto connect 0x%x.", err);
        return;
    }
}

static void config_br_param(void)
{
    errcode_t ret = UX_ERR_SUCCESS;
    char bt_name[BS_BR_DEVICE_NAME_MAX_LEN] = {0};
    ux_bd_addr bt_add = {0};

    ret = ux_api_bt_get_name((uint8_t *)bt_name, BS_BR_DEVICE_NAME_MAX_LEN);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[config_br_param] Fail to load bt name from nv 0x%x.\n", ret);
        return;
    }

    uint32_t name_len = (strlen(bt_name) > BS_BR_DEVICE_NAME_MAX_LEN) ? BS_BR_DEVICE_NAME_MAX_LEN : strlen(bt_name);
    ret = bs_config(BR_CONFIG_LOCAL_NAME, bt_name, (uint16_t)name_len);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[config_br_param] Fail to set bt name 0x%x\n", ret);
        return;
    }

    ret = ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, bt_add, arry_num(bt_add));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[config_br_param] Fail to load bt addr from nv 0x%x\n", ret);
        return;
    }
    ret = bs_config(BR_CONFIG_LOCAL_ADDR, bt_add, arry_num(bt_add));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[config_br_param] Fail to set bt addr 0x%x.\n", ret);
        return;
    }

    /* 设备类型为手表 */
    uint32_t local_device_class = BR_DEVICE_TYPE_WATCH;
    ret = bs_config(BR_CONFIG_LOCAL_DEV_CLASS, &local_device_class, sizeof(local_device_class));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[config_br_param] Fail to set local device class 0x%x\n", ret);
        return;
    }
}

void process_br_remote_name_changed_cb(uint16_t dev_id, const uint8_t *name, uint8_t length)
{
    if (name == NULL || dev_id >= BS_CONNECTION_UPPER_LIMIT || length > BS_BR_DEVICE_NAME_MAX_LEN) {
        ux_log_err("[process_br_remote_name_changed_cb] Invalid param, dev_id=%d, name:%s, length=%d.\n",
            dev_id, name, length);
        return;
    }
    ux_log_info("[process_br_remote_name_changed_cb] dev_id=%d, name:%s.\n", dev_id, name);
    ux_bg_device_info_t *dev_info = ux_bg_find_device_info_by_id(ux_bg_get_paired_device_list(), dev_id);
    if (dev_info == NULL) {
        ux_log_err("[process_br_remote_name_changed_cb] device not exist.\n");
        return;
    }
    int err = strcpy_s(dev_info->name, sizeof(dev_info->name), (char*)name);
    if (err != EOK) {
        ux_log_err("[process_br_remote_name_changed_cb] strcpy_s name fail 0x%x.\n", err);
    }
}

errcode_t ux_bt_br_init(void)
{
    static bs_br_callbacks_t ux_br_cb = {
        .stack_state_cb = process_br_stack_state_change_result,
        .connect_request_cb = NULL,
        .connect_state_cb = process_br_connect_state_changed_result,
        .pair_complete_cb = process_br_pair_result,
        .discovery_result_cb = process_br_discover_result,
        .pair_remove_cb = process_br_remove_device_result,
        .remote_name_changed_cb = process_br_remote_name_changed_cb,
    };
    errcode_t ret = UX_ERR_SUCCESS;
    if (osal_mutex_init(&g_br_stack_info_mutex) != 0) {
        ux_log_err("[ux_bt_br_init] Fail to intit g_br_stack_info_mutex\n");
        ret = UX_ERR_MUTEX_INIT_FAILED;
        goto out;
    }
    config_br_param();
    config_br_service(); // 配置支持的蓝牙服务
    ret = bs_br_register_callbacks(0, &ux_br_cb);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_br_init] Fail to register cb: 0x%x!\n", ret);
        goto out;
    }
    ux_bt_stack_info_t *br_info = ux_bt_br_get_stack_info();
    osal_mutex_lock(&g_br_stack_info_mutex);
    br_info->stack_status = UX_BT_STACK_INITING;
    osal_mutex_unlock(&g_br_stack_info_mutex);
    ret = bs_br_enable();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_br_init] Fail to enable br: 0x%x!\n", ret);
        goto out;
    }
    return ret;
out:
    osal_mutex_destroy(&g_br_stack_info_mutex);
    g_br_stack_info_mutex.mutex = NULL;
    return ret;
}

errcode_t ux_bt_br_deinit(void)
{
    /* 去初始化服务 */
    typedef errcode_t (*deinit_profile_handle)(void);
    static deinit_profile_handle deinit_handle_set[] = {
#ifdef CONFIG_SUPPORT_HFP_HF
        ux_bt_hf_deinit,
#endif
#if defined(CONFIG_SUPPORT_AUTO_CONNECT_A2DP_SINK) || defined(CONFIG_SUPPORT_AUTO_CONNECT_A2DP_SRC)
        ux_bt_a2dp_deinit,
#endif
#ifdef CONFIG_SUPPORT_AVRCP
        ux_bt_avrcp_deinit,
#endif
#ifdef CONFIG_SUPPORT_UX_SPP
        ux_spp_service_deinit,
#endif
    };
    errcode_t ret = UX_ERR_SUCCESS;
    for (uint8_t i = 0; i < sizeof(deinit_handle_set) / sizeof(deinit_handle_set[0]); i++) {
        ret = deinit_handle_set[i]();
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux_bt_br_deinit]Fail to deinit, idx=%d, ret=0x%x.\n", i, ret);
        }
    }
    /* 通知gap断连 */
    ux_bg_device_info_t *cur_node = ux_bg_get_paired_device_list()->head;
    while (cur_node != NULL) {
        if (cur_node->connect_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
            process_br_connect_state_changed_result(cur_node->id, BR_CONNECT_DISCONNECTED, 0, 0);
        }
        cur_node = cur_node->next;
    }
    ux_bg_clear_device_list(ux_bg_get_paired_device_list());
    ux_bg_clear_device_list(ux_bg_get_scan_device_list());
    /* 更新状态 */
    ux_bt_stack_info_t *br_info = ux_bt_br_get_stack_info();
    osal_mutex_lock(&g_br_stack_info_mutex);
    br_info->stack_status = UX_BT_STACK_DEINITING;
    osal_mutex_unlock(&g_br_stack_info_mutex);
    /* 去使能协议栈 */
    ret = bs_br_disable();
    if (ret != UX_ERR_SUCCESS) {
        osal_mutex_destroy(&g_br_stack_info_mutex);
        g_br_stack_info_mutex.mutex = NULL;
        ux_log_err("Fail to disable br ret=0x%x.\n", ret);
    }

    ret = bs_br_stop_auto_conn();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("Fail to stop auto connect.\n");
    }

    return ret;
}

errcode_t ux_bt_br_switch_discovery(ux_api_bt_role_t role, ux_bt_discovery_state_t discovery_switch)
{
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_stack_info_t *br_stack_info = ux_bt_br_get_stack_info();
    ux_bt_discovery_state_t *discovery_state = NULL;
    if (role == UX_API_BT_AS_CENTRAL && discovery_switch == UX_BT_DISCOVERY_START) {
        /* br开扫描 */
        err = bs_br_start_discovery(BR_DISCOVERY_CENTRAL);
        discovery_state = &br_stack_info->scan_status;
    } else if (role == UX_API_BT_AS_CENTRAL && discovery_switch == UX_BT_DISCOVERY_STOP) {
        /* br关扫描 */
        err = bs_br_stop_discovery(BR_DISCOVERY_CENTRAL);
        discovery_state = &br_stack_info->scan_status;
    } else if (role == UX_API_BT_AS_PERIPHERAL && discovery_switch == UX_BT_DISCOVERY_START) {
        /* br开广播 */
        err = bs_br_start_discovery(BR_DISCOVERY_PERIPHERAL);
        discovery_state = &br_stack_info->adv_status;
    } else if (role == UX_API_BT_AS_PERIPHERAL && discovery_switch == UX_BT_DISCOVERY_STOP) {
        /* br关广播 */
        err = bs_br_stop_discovery(BR_DISCOVERY_PERIPHERAL);
        discovery_state = &br_stack_info->adv_status;
    } else {
        err = UX_ERR_INVALID_PARAMETER;
    }

    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_switch_br_discovery] fail to switch br discovery, role:%d, discovery_state:%d, err:0x%x",
            role, discovery_switch, err);
        return err;
    }
    /* discovery切换成功，刷新状态 */
    *discovery_state = discovery_switch;
    return UX_ERR_SUCCESS;
}