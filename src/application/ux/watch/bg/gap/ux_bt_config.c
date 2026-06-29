/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux bg config service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-23， Create file. \n
 */
#include <stdio.h>
#include <string.h>
#include <securec.h>
#include "key_id.h"
#include "stdbool.h"
#include "std_def.h"
#include "nv.h"
#include "property.h"
#include "bs_ble_service_config.h"
#ifdef CONFIG_SUPPORT_BR
#include "bs_br_service_config.h"
#endif
#ifdef CONFIG_SUPPORT_BLE
#include "bs_ble_service_config.h"
#endif
#include "ux_errcode.h"
#include "ux_common_define.h"
#include "ux_utils.h"
#include "ux_common_log.h"
#include "ux_device_manager.h"
#include "ux_cmd_bs.h"
#include "ux_event_define.h"
#include "ux_feature_config.h"
#include "ux_bt.h"
#include "ux_api_bt.h"
#include "ux_bg_config.h"

#define BUF_LEN_12 12
#define NUMBER_2 2
#define NO_ADDR "no addr"
#define MAX_BLE_INTERVAL 3200
#define MIN_BLE_INTERVAL 6
#define MIN_BLE_SLAVE_LATENCY 0
#define MAX_BLE_SLAVE_LATENCY 499
#define MIN_BLE_TIMEOUT_MULTIPLIER 10
#define MAX_BLE_TIMEOUT_MULTIPLIER 3200

typedef errcode_t (*ux_bt_config_handle)(void *param, uint16_t len);
typedef struct {
    ux_api_bt_config_t config;
    ux_bt_config_handle func;
} ux_bt_config_func_t;

static bool g_ctkd_is_open = false;

static void ux_bt_set_ctkd_status(bool flag)
{
    g_ctkd_is_open = flag;
}

bool ux_bt_ctkd_is_open(void)
{
    return g_ctkd_is_open;
}

errcode_t ux_bg_load_addr(uint16_t key, uint8_t *addr, uint16_t addr_len)
{
    if (addr == NULL || addr_len == 0 || addr_len > UX_BD_ADDR_SIZE) {
        return ERRCODE_FAIL;
    }

    errcode_t err = uapi_nv_read(key, addr_len, &addr_len, addr);
    if (err != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    /* 切换为小端地址 */
    uint8_t *start = addr;
    uint8_t *end = addr + UX_BD_ADDR_SIZE - 1;
    while (start < end) {
        uint8_t temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    char addr_buff[BUF_LEN_12 + 1] = {
        '\0',
    };
    if (snprintf_s((char *)addr_buff, sizeof(addr_buff), BUF_LEN_12, "%02X%02X%02X%02X%02X%02X", addr[BG_ADDR_INDEX_5],
                   addr[BG_ADDR_INDEX_4], addr[BG_ADDR_INDEX_3], addr[BG_ADDR_INDEX_2], addr[BG_ADDR_INDEX_1],
                   addr[BG_ADDR_INDEX_0]) < 0) {
        ux_log_err("bg_read_addr snprintf_s failed \r\n");
        return UX_ERR_SPRINTF_FAILED;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_bg_str_addr_to_dec(const char *str_addr, uint16_t str_len, ux_bd_addr little_endian_addr,
                                ux_bd_addr big_endian_addr)
{
    if (str_addr == NULL || str_len != BUF_LEN_12) {
        return ERRCODE_FAIL;
    }

    for (uint8_t i = 0; i < UX_BD_ADDR_SIZE; i++) {
        // 2 is because handles two char at a time
        if ((ux_utils_hex_char_to_dec(str_addr[UX_CHAR_NUM * i]) < 0) ||
            (ux_utils_hex_char_to_dec(str_addr[UX_CHAR_NUM * i + 1]) < 0)) {
            return ERRCODE_FAIL;
        }
        /* 大端序 */
        big_endian_addr[i] = (unsigned char)((uint8_t)ux_utils_hex_char_to_dec(str_addr[UX_CHAR_NUM * i]) * UX_BASE16 +
                                             (uint8_t)ux_utils_hex_char_to_dec(str_addr[UX_CHAR_NUM * i + 1]));
        /* 小端序 */
        little_endian_addr[UX_BD_ADDR_SIZE - 1 - i] = big_endian_addr[i];
    }
#ifdef SUPPORT_BT_ADDR_VERIFY
    if (bt_addr_is_invalid(little_endian_addr, UX_BD_ADDR_SIZE)) {
        return ERRCODE_INVALID_PARAM;
    }
#endif
    return ERRCODE_SUCC;
}

/* 对外接口，设置蓝牙地址 */
errcode_t ux_bt_set_addr(uint8_t *addr, uint16_t len)
{
    if (addr == NULL || len != UX_BD_ADDR_SIZE) {
        ux_log_err("[ux_api_bt_set_addr] NULL!\n");
        return ERRCODE_FAIL;
    }

    for (int i = 0; i < UX_BD_ADDR_SIZE / NUMBER_2; i++) {
        uint8_t temp = addr[i];
        addr[i] = addr[UX_BD_ADDR_SIZE - 1 - i];
        addr[UX_BD_ADDR_SIZE - 1 - i] = temp;
    }

    return property_set_non_upgrade_bytes(NV_ID_BT_FACTORY_ADDR, addr, UX_BD_ADDR_SIZE);
}

errcode_t ux_bs_set_local_addr(const ux_work_protocol_t protocol, uint8_t *addr, uint16_t len)
{
    if (protocol >= UX_WORK_PROTOCOL_MAX) {
        ux_log_err("[ux_bs_set_local_addr] protocol invalid = %d\n", protocol);
        return ERRCODE_INVALID_PARAM;
    }

    if (addr == NULL || len != BUF_LEN_12) {
        ux_log_err("[ux_bt_set_local_addr] NULL!, len = %d\n", len);
        return ERRCODE_FAIL;
    }
    ux_bd_addr little_endian_addr = { 0 };
    ux_bd_addr big_endian_addr = { 0 };
    if (ux_bg_str_addr_to_dec((char *)addr, len, little_endian_addr, big_endian_addr) != ERRCODE_SUCC) {
        ux_log_err("[ux_bt_set_local_addr] Fail to ux_bg_str_addr_to_dec!\n");
        return ERRCODE_FAIL;
    }

    return ux_bt_set_addr(big_endian_addr, UX_BD_ADDR_SIZE);
}

/* 对外接口，设置蓝牙名称 */
errcode_t ux_bt_set_name(uint8_t *name, uint16_t len)
{
    if (name == NULL || len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    if (len >= BS_BR_DEVICE_NAME_MAX_LEN) {
        return ERRCODE_NOT_SUPPORT;
    }
    /* BT_NAME is non_upgrade */
    errcode_t res = property_set_non_upgrade_bytes(NV_ID_UX_BT_NAME, name, len + 1);
    if (res != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

static errcode_t config_local_addr(void *param, uint16_t len)
{
    if (len != UX_BG_ADDR_LEN) {
        return UX_ERR_INVALID_LENGTH;
    }
    return ux_bt_set_addr(param, len);
}

static errcode_t config_local_name(void *param, uint16_t len)
{
    if (len > BS_BR_DEVICE_NAME_MAX_LEN) {
        return UX_ERR_INVALID_LENGTH;
    }
    return ux_bt_set_name(param, len);
}

static errcode_t config_ble_adv_data(void *param, uint16_t len)
{
#ifdef CONFIG_SUPPORT_BLE
    return bs_config(BLE_CONFIG_ADV_DATA, param, len);
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t config_ble_adv_scan_rsp(void *param, uint16_t len)
{
#ifdef CONFIG_SUPPORT_BLE
    return bs_config(BLE_CONFIG_ADV_SCAN_RSP, param, len);
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t config_connect_parameter(void *param, uint16_t len)
{
    if (len != sizeof(ux_api_bt_conn_param_t)) {
        return UX_ERR_INVALID_LENGTH;
    }
    ux_api_bt_conn_param_t *ux_config = (ux_api_bt_conn_param_t *)param;
    uint16_t device_id =
        (uint16_t)bs_find_device_id_by_addr(BS_PROTOCOL_BLE, (uint8_t)ux_config->addr.type, ux_config->addr.addr);
    if (device_id == BS_CONNECTION_UPPER_LIMIT) {
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    if ((ux_config->interval_min < MIN_BLE_INTERVAL || ux_config->interval_min > MAX_BLE_INTERVAL) ||
        (ux_config->interval_max < MIN_BLE_INTERVAL || ux_config->interval_max > MAX_BLE_INTERVAL) ||
        (ux_config->slave_latency < MIN_BLE_SLAVE_LATENCY || ux_config->slave_latency > MAX_BLE_SLAVE_LATENCY) ||
        (ux_config->timeout_multiplier < MIN_BLE_TIMEOUT_MULTIPLIER ||
         ux_config->timeout_multiplier > MAX_BLE_TIMEOUT_MULTIPLIER)) {
            return UX_ERR_INVALID_PARAMETER;
    }
#ifdef CONFIG_SUPPORT_BLE
    bs_ble_conn_param_update_t bth_config = {
        .dev_id = device_id,
        .interval_max = ux_config->interval_max,
        .interval_min = ux_config->interval_min,
        .slave_latency = ux_config->slave_latency,
        .timeout_multiplier = ux_config->timeout_multiplier,
    };
    return bs_config(BLE_CONFIG_CONNECT_UPDATE, &bth_config, sizeof(bs_ble_conn_param_update_t));
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t config_connect_phy(void *param, uint16_t len)
{
    if (len != sizeof(ux_api_bt_phy_t)) {
        return UX_ERR_INVALID_LENGTH;
    }
    ux_api_bt_phy_t *ux_config = (ux_api_bt_phy_t *)param;
    uint16_t device_id =
        (uint16_t)bs_find_device_id_by_addr(BS_PROTOCOL_BLE, (uint8_t)ux_config->addr.type, ux_config->addr.addr);
    if (device_id == BS_CONNECTION_UPPER_LIMIT) {
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
#ifdef CONFIG_SUPPORT_BLE
    bs_ble_set_phy_t bth_config = {
        .dev_id = device_id,
        .all_phys = ux_config->all_phys,
        .phy_options = ux_config->phy_options,
        .rx_phys = ux_config->rx_phys,
        .tx_phys = ux_config->tx_phys,
    };
    return bs_config(BLE_CONFIG_CONNECT_PHY, &bth_config, sizeof(bs_ble_set_phy_t));
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t config_switch_ctkd(void *param, uint16_t len)
{
    if (len != sizeof(bool)) {
        return UX_ERR_INVALID_LENGTH;
    }
#ifdef CONFIG_SUPPORT_BLE
    bs_ble_config_enable_t flag = BS_BLE_DISABLE;
    if (*((bool *)param) == false) {
        ux_bt_set_ctkd_status(false);
        flag = BS_BLE_DISABLE;
    } else {
        ux_bt_set_ctkd_status(true);
        flag = BS_BLE_ENABLE;
    }
    return bs_config(BLE_CONFIG_PAIR_USE_CTDK, &flag, sizeof(bs_ble_config_enable_t));
#else
    ux_bt_set_ctkd_status(false);
    return UX_ERR_NOT_SUPPORT;
#endif
}

static errcode_t config_profile_auto_connect(void *param, uint16_t len)
{
    if (len != sizeof(ux_api_bt_auto_connect_profile_t)) {
        return UX_ERR_INVALID_LENGTH;
    }
    ux_api_bt_auto_connect_profile_t *ux_config = (ux_api_bt_auto_connect_profile_t *)param;
#ifdef CONFIG_SUPPORT_BR
    bs_br_config_connect_profile_t bth_config = {
        .a2dp_snk = ux_config->a2dp_snk,
        .a2dp_src = ux_config->a2dp_src,
        .avrcp_ct = ux_config->avrcp_ct,
        .avrcp_tg = ux_config->avrcp_tg,
        .hfp_ag = ux_config->hfp_ag,
        .hfp_hf = ux_config->hfp_hf,
    };
    return bs_config(BR_CONFIG_PROFILE_CONNECT, &bth_config, sizeof(bs_br_config_connect_profile_t));
#else
    return UX_ERR_NOT_SUPPORT;
#endif
}

ux_bt_config_func_t g_ux_bt_config_func_table[] = {
    {
        UX_API_BT_CONFIG_LOCAL_ADDR,
        config_local_addr,
    },
    {
        UX_API_BT_CONFIG_LOCAL_NAME,
        config_local_name,
    },
    {
        UX_API_BT_CONFIG_ADV_DATA,
        config_ble_adv_data,
    },
    {
        UX_API_BT_CONFIG_ADV_SCAN_RSP,
        config_ble_adv_scan_rsp,
    },
    {
        UX_API_BT_CONFIG_CONNECT_PARAM,
        config_connect_parameter,
    },
    {
        UX_API_BT_CONFIG_CONNECT_PHY,
        config_connect_phy,
    },
    {
        UX_API_BT_CONFIG_PAIR_USE_CTKD,
        config_switch_ctkd,
    },
    {
        UX_API_BT_CONFIG_PROFILE_CONNECT,
        config_profile_auto_connect,
    },
};

errcode_t ux_api_bt_config(ux_api_bt_config_t config, void *param, uint16_t len)
{
    /* 参数校验 */
    if (param == NULL) {
        return UX_ERR_NULL_PTR;
    }
    for (uint8_t i = 0; i < arry_num(g_ux_bt_config_func_table); i++) {
        if (g_ux_bt_config_func_table[i].config == config) {
            return g_ux_bt_config_func_table[i].func(param, len);
        }
    }
    return UX_ERR_INVALID_PARAMETER;
}

/* 对外接口，获取蓝牙名称 */
errcode_t ux_api_bt_get_name(uint8_t *name, uint16_t len)
{
    if (name == NULL || len == 0) {
        ux_log_err("[ux_api_bt_get_name] invalid param!\n");
        return ERRCODE_INVALID_PARAM;
    }
    if (memset_s(name, len, 0, len) != EOK) {
        return ERRCODE_MEMSET;
    }

    if (property_get_bytes(NV_ID_UX_BT_NAME, (uint8_t *)name, len) == NULL) {
        if (strcpy_s((char *)name, len, ux_get_default_bt_name()) != EOK) {
            ux_log_err("fail to strcpy_s");
            return ERRCODE_FAIL;
        }
    }
    return ERRCODE_SUCC;
}