/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux cmd bt at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-13， Create file. \n
 */
#include <string.h>
#include "securec.h"
#include "audio_base_type.h"
#include "autoconf.h"
#include "common_def.h"
#include "dut.h"
#include "key_id.h"
#include "nv_storage.h"
#include "watchdog.h"
#include "property.h"
#include "ux_at_manager.h"
#include "ux_types.h"
#include "ux_cmd_manager.h"
#include "ux_event_define.h"
#include "ux_config_common.h"
#include "ux_cmd_utils.h"
#include "ux_common_log.h"
#include "ux_at_interface.h"
#include "ux_common_define.h"
#include "ux_api_tws.h"
#include "ux_api_bs_connection.h"
#include "ux_api_bs_config.h"
#include "ux_utils.h"

#ifndef DEMO_WATCH
#include "ux_multiple_device.h"
#include "ux_bt_common.h"
#include "ux_system_manager.h"
#include "ux_event_define.h"
#include "ux_secure.h"
#include "ux_hal_memory.h"
#include "ux_bs_property.h"
#include "ux_bs_generic.h"
#include "ux_bs_manager.h"
#include "ux_api_sysmgr.h"
#endif

#include "ux_bg_config.h"
#include "ux_api_ble.h"
#include "ux_api_bt.h"
#include "ux_cmd_bs.h"
#include "ux_cmd_bt.h"

#define BASE16 16
#define BUF_LEN_32 32
#define PDL_PROTOCOL_TYPE_NUM 2

#ifndef DEMO_WATCH
static errcode_t ux_cmd_addr_num_to_str(ux_bd_addr addr, uint8_t *data, uint16_t data_len)
{
    int32_t ret;
    ret = snprintf_s((char *)data, data_len, data_len - 1, "%02X%02X%02X%02X%02X%02X",
                     addr[BG_ADDR_INDEX_5], addr[BG_ADDR_INDEX_4], addr[BG_ADDR_INDEX_3],
                     addr[BG_ADDR_INDEX_2], addr[BG_ADDR_INDEX_1], addr[BG_ADDR_INDEX_0]);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}
#endif

errcode_t ux_cmd_get_bt_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
#ifndef DEMO_WATCH
    ux_bd_addr addr = { 0 };
    errcode_t ret = ERRCODE_SUCC;
    ret = ux_api_bs_get_addr(UX_API_BS_PROTOCOL_BT, addr, sizeof(ux_bd_addr));
    if (ret != ERRCODE_SUCC) {
        if (data_len < (strlen(NO_ADDR) + 1)) {
            return ERRCODE_FAIL;
        }
        int32_t str_ret = sprintf_s((char *)data, data_len, "%s", NO_ADDR);
        return (str_ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
    }
    ux_bs_reversal_addr(addr);
    return ux_cmd_addr_num_to_str(addr, data, data_len);
#else
    return ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, data, data_len);
#endif
}

#ifndef DEMO_WATCH
errcode_t ux_cmd_set_ble_name(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    uint8_t ble_name_max_len = ux_get_ble_name_max_len();
    if (data_len > ble_name_max_len) {
        return ERRCODE_NOT_SUPPORT;
    }
    errcode_t res = ux_api_bs_set_name(UX_API_BS_PROTOCOL_BLE, (char*)data, data_len);
    if (res != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_ble_name(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    errcode_t ret = ux_api_bs_get_name(UX_API_BS_PROTOCOL_BLE, (char *)data, data_len);
    if (ret != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_earside(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    char *p = NULL;
    char *side = strtok_r((char *)data, "\r", &p);
    if (side == NULL) {
        return ERRCODE_FAIL;
    }

    int32_t val = (int32_t)strtol(side, NULL, 0);
    if ((val < UX_API_TWS_EAR_SIDE_LEFT) || (val > UX_API_TWS_EAR_SIDE_RIGHT)) {
        return ERRCODE_FAIL;
    }

    errcode_t ret = ux_api_tws_set_ear_side((uint8_t)val);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_earside(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    ux_api_tws_ear_side_t earside = ux_api_tws_get_ear_side();
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%u", earside);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_tws_role(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    ux_api_sysmgr_role_t tws_role = ux_api_sysmgr_get_device_role();
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s",
        ((tws_role == UX_API_SYSMGR_ROLE_MASTER) ? MASTER_ROLE : SLAVE_ROLE));
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

// 将蓝牙地址格式化为 "XX:XX:*:*:*:XX" 字符串，写入指定缓冲区
// 返回写入的字符数，失败返回 -1
static int format_bd_addr_to_buffer(char *buffer, size_t buf_size, const ux_bd_addr addr, uint8_t num)
{
    if (buffer == NULL || buf_size == 0) {
        return -1;
    }
    // 第一个设备标记为活跃设备(ActiveDevice)
    const char *prefix = (num == 1) ? "(A)" : "";
    int len = snprintf_s(buffer, buf_size, buf_size - 1, "%s%02x:%02x:*:*:*:%02x; ",
                         prefix, addr[BG_ADDR_INDEX_5], addr[BG_ADDR_INDEX_4], addr[BG_ADDR_INDEX_0]);
    // 检查是否写入失败（包括缓冲区不足）
    if (len < 0 || (size_t)len >= buf_size) {
        return -1;
    }
    return len;
}

// 在已有偏移后追加一个字符串，如果空间足够
static int append_string_if_space(char *buffer, size_t buf_size, size_t offset, const char *str)
{
    if (buffer == NULL || str == NULL || offset >= buf_size) {
        return -1;
    }
    size_t remaining = buf_size - offset;
    if (remaining <= 1) { // 至少留一个位置给 \0
        return -1;
    }
    int len = snprintf_s(buffer + offset, remaining, remaining - 1, "%s", str);
    if (len < 0) {
        return -1;
    }
    return len;
}

errcode_t ux_cmd_get_device_connect_info(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    uint16_t connected_num = ux_api_bs_get_connected_nums(ux_api_bs_get_protocol());
    ux_api_bs_addr_t bs_addr = {0};
    int offset = 0;
    if (connected_num == 0) {
        if (append_string_if_space((char *)data, data_len, 0, DISCONNECTED) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }
    if (ux_api_bs_get_multipoint_main_addr(&bs_addr) != ERRCODE_SUCC) {
        if (append_string_if_space((char *)data, data_len, 0, NO_ADDR) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }
    offset = format_bd_addr_to_buffer((char *)data, data_len, bs_addr.addr, 1);
    if (offset < 0) {
        return ERRCODE_FAIL;
    }
    if (connected_num <= 1) {
        return ERRCODE_SUCC; // 单连接，结束
    }
    if (ux_api_bs_get_multipoint_other_addr(&bs_addr) != ERRCODE_SUCC) {
        if (append_string_if_space((char *)data, data_len, (size_t)offset, NO_ADDR) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }
    int len = format_bd_addr_to_buffer((char *)data + offset, (size_t)(data_len - offset), bs_addr.addr, 2);
    if (len < 0) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t append_conn_order(char *buf, uint16_t buf_size, uint8_t pd_num, uint8_t *conn_order)
{
    if (pd_num == 0) {
        if (snprintf_s(buf, buf_size, buf_size - 1, "%s", DISCONNECTED) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }

    uint16_t written = 0;
    uint16_t remaining = buf_size - 1; // 留1字节给 '\0'
    // 倒序遍历：从最后一个有效索引开始
    for (int i = pd_num - 1; i >= 0; i--) {
        uint8_t order = conn_order[i];
        int len = snprintf_s(buf + written, remaining, remaining - 1, "(%u)", order);
        if (len < 0) {
            return ERRCODE_FAIL;
        }
        written += (uint16_t)len;
        remaining -= (uint16_t)len;
        // 防止缓冲区耗尽
        if (remaining == 0) {
            return ERRCODE_FAIL;
        }
    }
    // 最终校验：确保未越界（snprintf_s 会自动加 '\0'，但需确认写入未超限）
    if (written >= buf_size) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_bt_connect_order(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    uint8_t pd_num = ux_api_bs_get_paired_nums(UX_API_BS_PROTOCOL_BT);
    ux_api_bs_connect_order_t pdl_conn_order = {0};
    ux_api_bs_get_connect_order(&pdl_conn_order);
    return append_conn_order((char*)data, data_len, pd_num, pdl_conn_order.connect_order);
}

errcode_t ux_cmd_get_combo_connect_order(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    uint8_t pd_num = ux_bt_get_common_state()->cm_pd_data.pdl_num;
    if (pd_num == 0) {
        if (snprintf_s((char *)data, data_len, data_len - 1, "%s", DISCONNECTED) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }
    if (pd_num > SDM_COMMON_MAX_CONN_ORDER_COUNT) {
        if (snprintf_s((char *)data, data_len, data_len - 1, "%s", FAIL_TYPE) < 0) {
            return ERRCODE_FAIL;
        }
        return ERRCODE_SUCC;
    }
    uint16_t written = 0;
    uint16_t remaining = data_len - 1; // 留1字节给 '\0'
    uint8_t order = 0;
    for (int32_t i = pd_num - 1; i >= 0; i--) {
        const char *prefix = (i == pd_num - 1) ? "\r\n" : ""; // 首个数据需要换行
        const char *suffix = (pd_num > 1 && i > 0) ? "\r\n" : ""; // 最后一个数据不需要换行
        uint8_t index = ux_bt_get_common_state()->cm_pdl_conn_order.pdl_conn_order[i];
        bs_sdm_common_pdl_data_stru_t *pd_data = &ux_bt_get_common_state()->cm_pd_data.pdl_data[index];
        int offset = snprintf_s((char *)data + written, remaining, remaining - 1,
            "%s(%u)mobile addr:%02x:%02x:*:*:*:%02x; protocol:%u;%s",
            prefix, order, pd_data->addr[BG_ADDR_INDEX_5], pd_data->addr[BG_ADDR_INDEX_4], pd_data->addr[BG_ADDR_INDEX_0],
            pd_data->protocol, suffix);
        if (offset < 0) {
            return ERRCODE_FAIL;
        }
        written += (uint16_t)offset;
        remaining -= (uint16_t)offset;
        order += 1;
        // 防止缓冲区耗尽
        if (remaining == 0) {
            return ERRCODE_FAIL;
        }
    }
    // 最终校验：确保未越界（snprintf_s 会自动加 '\0'，但需确认写入未超限）
    if (written >= data_len) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_tws_connect_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s(%s)(%s)",
        (ux_api_bs_get_peer_connect_state() == UX_API_BS_CONN_STATE_CONNECTED) ? CONNECTED : DISCONNECTED,
        (ux_api_tws_get_ear_side() == UX_API_TWS_EAR_SIDE_RIGHT) ? RIGHT_EAR_STR : LEFT_EAR_STR,
        (ux_api_sysmgr_get_device_role() == UX_API_SYSMGR_ROLE_MASTER) ? MASTER_ROLE : SLAVE_ROLE);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_ble_connect_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    bool is_connected = (ux_api_ble_get_connect_state() == UX_API_BS_CONN_STATE_CONNECTED);
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s", is_connected ? CONNECTED : DISCONNECTED);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_device_connect_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s",
        (ux_api_bs_get_connected_nums(ux_api_bs_get_protocol()) == 0) ? DISCONNECTED : CONNECTED);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_bt_device_connect_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    ux_api_bs_conn_state_t conn_state = ux_api_bs_get_connect_state(UX_API_BS_PROTOCOL_BT);
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s",
        (conn_state == UX_API_BS_CONN_STATE_CONNECTED) ? CONNECTED : DISCONNECTED);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_sle_device_connect_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    ux_api_bs_conn_state_t conn_state = ux_api_bs_get_connect_state(UX_API_BS_PROTOCOL_SLE);
    int32_t ret = snprintf_s((char*)data, data_len, data_len - 1, "%s",
        (conn_state == UX_API_BS_CONN_STATE_CONNECTED) ? CONNECTED : DISCONNECTED);
    if (ret == SNPRINTF_S_ERR_NUM) {
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

static errcode_t ux_cmd_get_device_pairing_state(uint8_t *data, uint16_t data_len, bool is_peer)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    int32_t ret = 0;
    uint16_t buff_len = 0;
    buff_len = data_len - (uint16_t)ret;
    // 对耳连接状态不显示回连阶段
    ux_api_bs_device_reconn_phase_t reconn_phase = ux_api_bs_get_device_reconnect_phase();
    const char *phase_str = is_peer ? "" : (reconn_phase == STATUS_DEVICE_RECONN_TWS) ? "(tws)" : "(all)";
    ux_api_bs_device_state_t local_pairing_state = ux_api_bs_get_device_state(ux_api_bs_get_protocol());
    ux_api_bs_device_state_t peer_pairing_state = ux_api_bs_get_peer_device_state(ux_api_bs_get_protocol());
    ux_api_bs_device_state_t pairing_state = is_peer ? peer_pairing_state : local_pairing_state;
    if (pairing_state == UX_API_BS_DEVICE_STATUS_START_PAIR) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", START_PAIR);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_STOP_PAIR) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", STOP_PAIR);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_START_DISCOVERY) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", START_DISCOVERY);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_STOP_DISCOVERY) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", STOP_DISCOVERY);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_START_CONNECT) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s%s", START_CONNECT, phase_str);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_STOP_CONNECT) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", STOP_CONNECT);
    } else if (pairing_state == UX_API_BS_DEVICE_STATUS_DISCONNECT) {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", START_DISCONNECT);
    } else {
        ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, "%s", INIT_TYPE);
    }
    if (is_peer) {
        if (ret <= 0 || ret >= buff_len) {
            return ERRCODE_FAIL;
        }
        buff_len = data_len - (uint16_t)ret;
        if (ux_api_bs_get_peer_device_connect_state() == UX_API_BS_CONN_STATE_CONNECTED) {
            ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, ", %u %s",
                ux_api_bs_get_peer_device_connected_nums(), CONNECTED);
        } else {
            ret += snprintf_s((char*)data + ret, buff_len, buff_len - 1, ", %s", DISCONNECTED);
        }
    }
    return (ret <= 0 || ret >= data_len) ? ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t ux_cmd_get_peer_pairing_state(uint8_t *data, uint16_t data_len)
{
    return ux_cmd_get_device_pairing_state(data, data_len, true);
}

errcode_t ux_cmd_get_pairing_state(uint8_t *data, uint16_t data_len)
{
    return ux_cmd_get_device_pairing_state(data, data_len, false);
}

errcode_t ux_cmd_get_ble_pairing_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    int32_t ret = 0;
    ux_api_bs_pair_state_t state = ux_api_ble_get_pairing_state();
    if (state == UX_API_BS_PAIR_STATE_PAIRING) {
        ret = snprintf_s((char*)data, data_len, data_len - 1, "%s", START_DISCOVERY);
    } else if (state == UX_API_BS_PAIR_STATE_PAIRED) {
        ret = snprintf_s((char*)data, data_len, data_len - 1, "%s", STOP_DISCOVERY);
    } else {
        ret = snprintf_s((char*)data, data_len, data_len - 1, "%s", INIT_TYPE);
    }
    return (ret <= 0 || ret >= data_len) ? ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t ux_cmd_set_ble_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    return ux_api_bs_set_addr(UX_API_BS_PROTOCOL_BLE, (char*)data, data_len);
}

errcode_t ux_cmd_get_ble_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    ux_bd_addr addr = { 0 };
    errcode_t ret = ERRCODE_SUCC;
    ret = ux_api_bs_get_addr(UX_API_BS_PROTOCOL_BLE, addr, sizeof(ux_bd_addr));
    if (ret != ERRCODE_SUCC) {
        if (data_len < (strlen(NO_ADDR) + 1)) {
            return ERRCODE_FAIL;
        }
        int32_t str_ret = sprintf_s((char *)data, data_len, "%s", NO_ADDR);
        return (str_ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
    }
    ux_bs_reversal_addr(addr);
    return ux_cmd_addr_num_to_str(addr, data, data_len);
}

errcode_t ux_cmd_set_ble_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }
    return ux_api_bs_set_peer_addr(UX_API_BS_PROTOCOL_BLE, (char*)data, data_len);
}

errcode_t ux_cmd_get_ble_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    ux_bd_addr addr = { 0 };
    errcode_t ret = ERRCODE_SUCC;
    ret = ux_api_bs_get_peer_addr(UX_API_BS_PROTOCOL_BLE, addr, sizeof(ux_bd_addr));
    if (ret != ERRCODE_SUCC) {
        if (data_len < (strlen(NO_ADDR) + 1)) {
            return ERRCODE_FAIL;
        }
        int32_t str_ret = sprintf_s((char *)data, data_len, "%s", NO_ADDR);
        return (str_ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
    }
    ux_bs_reversal_addr(addr);
    return ux_cmd_addr_num_to_str(addr, data, data_len);
}

static errcode_t send_response(uint8_t *data, uint16_t data_len, const char *msg)
{
    int ret = sprintf_s((char *)data, data_len, "%s\r\n", msg);
    return (ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
}

static errcode_t format_pd_data_to_buffer(ux_api_bs_protocol_t protocol, char (*buff)[PD_NUM_LEN_MAX], uint8_t pdnum)
{
    uint16_t data_len = (uint16_t)(pdnum * sizeof(ux_api_bs_paired_device_info_t));
    ux_api_bs_paired_device_info_t *info = (ux_api_bs_paired_device_info_t *)ux_hal_memory_acquire(UX_MEM_BT, data_len);
    if (info == NULL) {
        ux_log_err("[ux][cmd][bt]paired_device_info ux_hal_memory_acquire fail \r\n");
        return ERRCODE_FAIL;
    }
    memset_s(info, data_len, 0, data_len);
    uint8_t actual_num = 0;
    if (ux_api_bs_get_paired_device_list(protocol, info, pdnum, &actual_num) != UX_ERR_SUCCESS) {
        ux_hal_memory_release(UX_MEM_BT, info);
        info = NULL;
        return ERRCODE_FAIL;
    }
    ux_log_info("[ux][cmd][bt]paired_device_info actual_num:%u\r\n", actual_num);
    for (uint8_t pdl_index = 0; pdl_index < actual_num; pdl_index++) {
        buff[pdl_index][0] = '\0';  // 清空
        const char *prefix = (pdl_index == 0) ? "\r\n" : "";
        int offset = sprintf_s(buff[pdl_index], PD_NUM_LEN_MAX - 1,
            "%s(%u)mobile addr:%02x:%02x:*:*:*:%02x; mobile name:%s;\r\n",
            prefix, pdl_index,
            info[pdl_index].addr.addr[BG_ADDR_INDEX_5], info[pdl_index].addr.addr[BG_ADDR_INDEX_4],
            info[pdl_index].addr.addr[BG_ADDR_INDEX_0], info[pdl_index].device_name);
        if (offset < 0) {
            ux_hal_memory_release(UX_MEM_BT, info);
            info = NULL;
            return ERRCODE_FAIL;
        }
    }
    ux_hal_memory_release(UX_MEM_BT, info);
    info = NULL;
    return ERRCODE_SUCC;
}

static errcode_t format_sle_and_bt_pd_data_to_buffer(char (*buff)[PD_NUM_LEN_MAX], uint8_t pdnum)
{
    uint8_t bt_num = ux_api_bs_get_paired_nums(UX_API_BS_PROTOCOL_BT);
    // 验证总条目数一致性
    if (bt_num + ux_api_bs_get_paired_nums(UX_API_BS_PROTOCOL_SLE) != pdnum) {
        ux_log_err("[ux][cmd][bt]combo pdnum mismatch: bt=%u, total=%u\n", bt_num, pdnum);
        return ERRCODE_FAIL;
    }

    uint16_t data_len = (uint16_t)(pdnum * sizeof(ux_api_bs_paired_device_info_t));
    ux_api_bs_paired_device_info_t *info = (ux_api_bs_paired_device_info_t *)ux_hal_memory_acquire(UX_MEM_BT, data_len);
    if (info == NULL) {
        ux_log_err("[ux][cmd][bt]paired_device_info ux_hal_memory_acquire fail \r\n");
        return ERRCODE_FAIL;
    }
    // 定义协议列表和对应数量
    struct { uint8_t proto; uint8_t count; const char *prefix; } protocols[] = {
        { UX_API_BS_PROTOCOL_BT, bt_num, "\r\n^bt:\r\n" },
        { UX_API_BS_PROTOCOL_SLE, pdnum - bt_num, "^sle:\r\n" }
    };
    for (int i = 0; i < PDL_PROTOCOL_TYPE_NUM; i++) {
        if (protocols[i].count == 0) { // 双模时, 单个协议栈的配对数量可能是0
            continue;
        }
        uint8_t actual = 0;
        memset_s(info, data_len, 0, data_len);
        if (ux_api_bs_get_paired_device_list(protocols[i].proto, info, protocols[i].count, &actual) != UX_ERR_SUCCESS) {
            goto exit;
        }
        for (uint8_t k = 0; k < actual; k++) {
            uint8_t buff_index = protocols[i].proto == UX_API_BS_PROTOCOL_BT ? k : bt_num + k;
            buff[buff_index][0] = '\0'; // 清空
            int offset = sprintf_s(buff[buff_index], PD_NUM_LEN_MAX - 1,
                "%s(%u)mobile addr:%02x:%02x:*:*:*:%02x; mobile name:%s;\r\n",
                (k == 0) ? protocols[i].prefix : "", k,
                info[k].addr.addr[BG_ADDR_INDEX_5], info[k].addr.addr[BG_ADDR_INDEX_4],
                info[k].addr.addr[BG_ADDR_INDEX_0], info[k].device_name);
            if (offset < 0) {
                goto exit;
            }
        }
    }
    ux_hal_memory_release(UX_MEM_BT, info);
    info = NULL;
    return ERRCODE_SUCC;
exit:
    ux_hal_memory_release(UX_MEM_BT, info);
    info = NULL;
    return ERRCODE_FAIL;
}


static errcode_t append_buffer_to_output(uint8_t *data, uint16_t data_len, char (*buff)[PD_NUM_LEN_MAX], uint8_t pdnum)
{
    errno_t err;
    size_t res_len = data_len;
    for (uint8_t i = 0; i < pdnum; i++) {
        size_t pdl_len = strlen(buff[i]);
        if (res_len < pdl_len) {
            ux_log_err("[ux][cmd]append_buffer fail, res_len=%u pdl_len=%u pdnum=%u\n", res_len, pdl_len, pdnum);
            return ERRCODE_FAIL;
        }
        err = strcat_s((char *)data, res_len, buff[i]);
        if (err != EOK) {
            ux_log_err("[ux][cmd]strcat_s fail:%d res_len=%u pdl_len=%u i=%u pdnum=%u\n",
                err, res_len, pdl_len, i, pdnum);
            return ERRCODE_FAIL;
        }
        res_len -= pdl_len;
    }
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_bt_pdinfo_process(uint8_t *data, uint16_t data_len)
{
    uint8_t pdnum = ux_api_bs_get_paired_nums(ux_api_bs_get_protocol());
    if (pdnum == 0) {
        return send_response(data, data_len, NO_PAIR);
    }
    if (pdnum > PD_NUM_MAX + PD_NUM_MAX) {
        return send_response(data, data_len, NUM_INVALID);
    }
    uint32_t buff_len = pdnum * PD_NUM_LEN_MAX;
    char (*buff)[PD_NUM_LEN_MAX] = (char(*)[PD_NUM_LEN_MAX])ux_hal_memory_acquire(UX_MEM_CMD_BT, (size_t)buff_len);
    if (!buff) {
        return send_response(data, data_len, "malloc error");
    }
    if (memset_s(buff, buff_len, 0, buff_len) != EOK) {
        ux_hal_memory_release(UX_MEM_CMD_BT, buff);
        buff = NULL;
        return send_response(data, data_len, "memset_s error");
    }
    errcode_t ret = ERRCODE_SUCC;
    ux_api_bs_protocol_t protocol = ux_api_bs_get_protocol();
    if (protocol == UX_API_BS_PROTOCOL_SLE || protocol == UX_API_BS_PROTOCOL_BT) {
        ret = format_pd_data_to_buffer(protocol, buff, pdnum);
    } else if (protocol == UX_API_BS_PROTOCOL_COMBO) {
        ret = format_sle_and_bt_pd_data_to_buffer(buff, pdnum);
    } else {
        ret = ERRCODE_FAIL;
    }
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][cmd][bt]format pd_data_to_buffer fail\n");
        ux_hal_memory_release(UX_MEM_CMD_BT, buff);
        buff = NULL;
        return ret;
    }
    ret = append_buffer_to_output(data, data_len, buff, pdnum);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][cmd][bt]append buffer_to_output fail\n");
        ux_hal_memory_release(UX_MEM_CMD_BT, buff);
        buff = NULL;
        return ret;
    }
    ux_hal_memory_release(UX_MEM_CMD_BT, buff);
    buff = NULL;
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_ble_expand_val(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    ux_api_ble_expand_data_t ble_expand = {0};
    if (data != NULL) {
        if ((data_len & 0x1) != 0) {
            return ERRCODE_INVALID_PARAM;
        }
        for (uint32_t i = 0; i < data_len / UX_CHAR_NUM && i < arry_num(ble_expand.data); i++) {
            if ((ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i]) < 0) ||
                (ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i + 1]) < 0)) {
                return ERRCODE_INVALID_PARAM;
            }
            ble_expand.data[i] = ((uint8_t)ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i]) * BASE16 +
                (uint8_t)ux_utils_hex_char_to_dec((char)data[UX_CHAR_NUM * i + 1]));
        }
        ble_expand.data_len = data_len / UX_CHAR_NUM;
    }
    return ux_api_ble_set_expand_data(&ble_expand);
}

errcode_t ux_cmd_get_ble_expand_val(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    if (data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    ux_api_ble_expand_data_t expand_val = {0};
    errcode_t ret = ux_api_ble_get_expand_data(&expand_val);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (data_len < expand_val.data_len * UX_CHAR_NUM + 1) {
        return ERRCODE_NOT_SUPPORT;
    }
    int32_t offset = 0;
    for (uint32_t i = 0; i < expand_val.data_len; i++) {
        offset += sprintf_s((char *)(data + offset), (size_t)(data_len - offset), "%02x", expand_val.data[i]);
    }

    data[offset] = 0;
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    return ux_api_bs_set_peer_addr(UX_API_BS_PROTOCOL_BT, (char*)data, data_len);
}

errcode_t ux_cmd_get_peer_addr(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    ux_bd_addr addr = { 0 };
    errcode_t ret = ERRCODE_SUCC;
    ret = ux_api_bs_get_peer_addr(UX_API_BS_PROTOCOL_BT, addr, sizeof(ux_bd_addr));
    if (ret != ERRCODE_SUCC) {
        int ret = sprintf_s((char *)data, data_len, "%s", NO_ADDR);
        return (ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
    }
    ux_bs_reversal_addr(addr);
    return ux_cmd_addr_num_to_str(addr, data, data_len);
}

#endif

