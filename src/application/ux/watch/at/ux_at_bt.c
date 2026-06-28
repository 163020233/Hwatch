/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux dut at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-24， Create file. \n
 */
#include <dut.h>
#include <at_product.h>
#include "key_id.h"
#include "ux_bg_config.h"
#include "ux_api_fwk.h"
#include "ux_common_log.h"
#include "ux_event_def.h"
#include "debug_print.h"
#include "ux_at_manager.h"
#include "ux_at_interface.h"
#include "ux_errcode.h"
#include "ux_api_bt.h"
#include "ux_bt_br.h"
#include "ux_bt_ble.h"
#include "ux_spp_service.h"
#include "ux_api_msg.h"

#ifndef ENDL
#define ENDL '\0'
#endif

#define at_uart_printf PRINT /* 仅在此文件中重定向，后续3322引入at_dut后切换 */
#define STR_BG_ADDR_LEN 12
#define BS_BLE_DEVICE_NAME_MAX_LEN 32

typedef struct {
    ux_api_bt_profile_state_t em_connect_state;
    char *str_connect_state;
} enum_str_connect_state_pair_t;

typedef struct {
    ux_bt_stack_state_t em_stack_state;
    char *str_stack_state;
} enum_str_stack_state_pair_t;

typedef struct {
    ux_api_bt_protocol_t em_protocol;
    char *str_protocol;
} enum_str_protocol_pair_t;

static enum_str_connect_state_pair_t g_enum_str_connect_state_map[] = {
    {.em_connect_state = UX_API_BT_PROFILE_STATE_CONNECTING, .str_connect_state = "connecting"},
    {.em_connect_state = UX_API_BT_PROFILE_STATE_CONNECTED,     .str_connect_state = "connected"},
    {.em_connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTING, .str_connect_state = "disconnecting"},
    {.em_connect_state = UX_API_BT_PROFILE_STATE_DISCONNECTED,  .str_connect_state = "disconnected"},
};

static enum_str_stack_state_pair_t g_enum_str_stack_state_map[] = {
    {.em_stack_state = UX_BT_STACK_DEINITED,  .str_stack_state = "disable"},
    {.em_stack_state = UX_BT_STACK_INITING,   .str_stack_state = "enabling"},
    {.em_stack_state = UX_BT_STACK_DEINITING, .str_stack_state = "disabling"},
    {.em_stack_state = UX_BT_STACK_INITIED,   .str_stack_state = "enable"},
};

static enum_str_protocol_pair_t g_enum_str_protocol_map[] = {
    {.em_protocol = UX_API_BT_PROTOCOL_BR, .str_protocol = "BREDR"},
    {.em_protocol = UX_API_BT_PROTOCOL_BLE,   .str_protocol = "BLE"},
};

static at_ret_t execute_cmd_handle(const ux_at_5para_cmd_pair_t *cmd_set, uint8_t cmd_set_size,
    const char *cmd, const ux_at_para5_args_t *args)
{
    if (cmd == NULL || cmd_set == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    for (uint32_t i = 0; i < cmd_set_size; i++) {
        if (strcasecmp(cmd, cmd_set[i].cmd) == 0) {
            return cmd_set[i].cmd_handle(args);
        }
    }
    return AT_RET_SYNTAX_ERROR;
}

/* bt相关AT命令的实现 */
static at_ret_t parse_bt_addr_para_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    if (ux_bs_set_local_addr(UX_WORK_PROTOCOL_BT, (uint8_t*)args->para2,
        (uint16_t)strlen(args->para2)) != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t parse_bt_name_para_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }

    errcode_t ret = ux_api_bt_config(UX_API_BT_CONFIG_LOCAL_NAME, (uint8_t*)args->para2, (uint16_t)strlen(args->para2));
    if (ret != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t parse_bt_pair_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char *endptr = NULL;
    long target_num = strtol(args->para2, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || target_num < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    uint16_t dev_id = (uint16_t)target_num;

    ux_api_comm_mail_t mail = {0};
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_START_PAIR;
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&dev_id, sizeof(dev_id));
}

static at_ret_t parse_bt_disconnect_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char *endptr = NULL;
    long target_num = strtol(args->para2, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || target_num < 0 || target_num >= BS_CONNECTION_UPPER_LIMIT) {
        return AT_RET_SYNTAX_ERROR;
    }
    uint16_t dev_id = (uint16_t)target_num;
    ux_api_comm_mail_t mail = {0};
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_DISCONNECTED;
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&dev_id, sizeof(dev_id));
}

static at_ret_t parse_bt_scan_cmd(const ux_at_para5_args_t *args)
{
    if (args->para1 == NULL || args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_bg_event_config_t scan_cfg = {0};
    ux_api_comm_mail_t mail = {0};
    scan_cfg.role = UX_API_BT_AS_CENTRAL;
    mail.event = UX_EVENT_TYPE_BG;
    if (strcasecmp(args->para1, "ble") == 0) {
        scan_cfg.protocol = UX_API_BT_PROTOCOL_BLE;
    } else if (strcasecmp(args->para1, "br") == 0) {
        scan_cfg.protocol = UX_API_BT_PROTOCOL_BR;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    if (strcasecmp(args->para3, "start") == 0) {
        mail.sub_event = UX_BG_START_DISCOVERY;
    } else if (strcasecmp(args->para3, "stop") == 0) {
        mail.sub_event = UX_BG_STOP_DISCOVERY;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&scan_cfg, sizeof(scan_cfg));
}

static at_ret_t parse_bt_adv_cmd(const ux_at_para5_args_t *args)
{
    if (args->para1 == NULL || args->para3 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_bg_event_config_t adv_cfg = {0};
    ux_api_comm_mail_t mail = {0};
    adv_cfg.role = UX_API_BT_AS_PERIPHERAL;
    mail.event = UX_EVENT_TYPE_BG;
    if (strcasecmp(args->para1, "ble") == 0) {
        adv_cfg.protocol = UX_API_BT_PROTOCOL_BLE;
    } else if (strcasecmp(args->para1, "br") == 0) {
        adv_cfg.protocol = UX_API_BT_PROTOCOL_BR;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    if (strcasecmp(args->para3, "start") == 0) {
        mail.sub_event = UX_BG_START_DISCOVERY;
    } else if (strcasecmp(args->para3, "stop") == 0) {
        mail.sub_event = UX_BG_STOP_DISCOVERY;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&adv_cfg, sizeof(adv_cfg));
}

static at_ret_t parse_bt_enable_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_api_comm_mail_t mail = {0};
    ux_bg_event_config_t enable_cfg = {0};
    if (strcasecmp("ble", args->para1) == 0) {
        enable_cfg.protocol = UX_API_BT_PROTOCOL_BLE;
    } else if (strcasecmp("br", args->para1) == 0) {
        enable_cfg.protocol = UX_API_BT_PROTOCOL_BR;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_ENABLE;
    
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&enable_cfg, sizeof(enable_cfg));
}

static at_ret_t parse_bt_disable_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_api_comm_mail_t mail = {0};
    ux_bg_event_config_t enable_cfg = {0};
    if (strcasecmp("ble", args->para1) == 0) {
        enable_cfg.protocol = UX_API_BT_PROTOCOL_BLE;
    } else if (strcasecmp("br", args->para1) == 0) {
        enable_cfg.protocol = UX_API_BT_PROTOCOL_BR;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_DISABLE;
    
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&enable_cfg, sizeof(enable_cfg));
}

static at_ret_t parse_bt_remove_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL) {
        return  AT_RET_SYNTAX_ERROR;
    }
    char *endptr = NULL;
    long target_num = strtol(args->para2, &endptr, 0);
    if (endptr == NULL || *endptr != ENDL || target_num < 0 || target_num >= BS_CONNECTION_UPPER_LIMIT) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_api_comm_mail_t mail = {0};
    mail.event = UX_EVENT_TYPE_BG;
    mail.sub_event = UX_BG_REMOVE_DEVICE;
    uint16_t dev_id = (uint16_t)target_num;
    return ux_api_fwk_push_mail(&mail, (uint8_t*)&dev_id, sizeof(dev_id));
}

static at_ret_t parse_bt_ctkd_cmd(const ux_at_para5_args_t *args)
{
    bool flag = false;
    if (strcasecmp("open", args->para2) == 0) {
        flag = true;
    } else if (strcasecmp("close", args->para2) == 0) {
        flag = false;
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    errcode_t err = ux_api_bt_config(UX_API_BT_CONFIG_PAIR_USE_CTKD, &flag, sizeof(bool));
    if (err != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return err;
}

static at_ret_t parse_bt_interval_cmd(const ux_at_para5_args_t *args)
{
    char *end = NULL;
    long temp = 0;
    if (args->para2 == NULL || args->para3 == NULL || args->para4 == NULL || args->para5 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_api_bt_conn_param_t config = {0};
    temp = strtol(args->para2, &end, 0);
    if (end == args->para2 || temp < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.interval_min = (uint16_t)temp;

    temp = strtol(args->para3, &end, 0);
    if (end == args->para3 || temp < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.interval_max = (uint16_t)temp;

    temp = strtol(args->para4, &end, 0);
    if (end == args->para4 || temp < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.slave_latency = (uint16_t)temp;

    temp = strtol(args->para5, &end, 0);
    if (end == args->para5 || temp < 0) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.timeout_multiplier = (uint16_t)temp;

    if (ux_bt_get_addr_by_device_id(0, &config.addr) != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    errcode_t err = ux_api_bt_config(UX_API_BT_CONFIG_CONNECT_PARAM, &config, sizeof(ux_api_bt_conn_param_t));
    if (err != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return err;
}

static at_ret_t parse_bt_phy_cmd(const ux_at_para5_args_t *args)
{
    if (args->para2 == NULL || args->para3 == NULL || args->para4 == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    char *end = NULL;
    long temp = 0;
    ux_api_bt_phy_t config = {0};
    /* 地址 */
    temp = strtol(args->para2, &end, 0);
    if (end == NULL || *end != ENDL || temp < 0 || temp >= BS_CONNECTION_UPPER_LIMIT) {
        return AT_RET_SYNTAX_ERROR;
    }
    uint16_t dev_id = (uint16_t)temp;
    if (ux_bt_get_addr_by_device_id(dev_id, &config.addr) != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    /* tx */
    temp = (uint8_t)strtol(args->para3, &end, 0);
    if (end == args->para3) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.tx_phys = (uint8_t)temp;
    /* rx */
    temp = (uint8_t)strtol(args->para4, &end, 0);
    if (end == args->para4) {
        return AT_RET_SYNTAX_ERROR;
    }
    config.rx_phys = (uint8_t)temp;

    errcode_t err = ux_api_bt_config(UX_API_BT_CONFIG_CONNECT_PHY, &config, sizeof(ux_api_bt_phy_t));
    if (err != UX_ERR_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return err;
}

static at_ret_t parse_ble_para_cmd(const ux_at_para5_args_t *args)
{
    ux_at_5para_cmd_pair_t ble_cmd[] = {
        {"enable", parse_bt_enable_cmd},
        {"disable", parse_bt_disable_cmd},
        {"scan", parse_bt_scan_cmd},
        {"adv", parse_bt_adv_cmd},
    };
    return execute_cmd_handle(ble_cmd, arry_num(ble_cmd), args->para2, (ux_at_para5_args_t*)args);
}

static at_ret_t parse_br_para_cmd(const ux_at_para5_args_t *args)
{
    ux_at_5para_cmd_pair_t br_cmd[] = {
        {"enable", parse_bt_enable_cmd},
        {"disable", parse_bt_disable_cmd},
        {"scan", parse_bt_scan_cmd},
        {"adv", parse_bt_adv_cmd},
    };
    return execute_cmd_handle(br_cmd, arry_num(br_cmd), args->para2, (ux_at_para5_args_t*)args);
}

static at_ret_t parse_multiple_devices_cmd(const ux_at_para5_args_t *args)
{
    if (strcasecmp(args->para2, "open") == 0) {
        ux_api_bt_switch_multiple_devices(true);
    } else if (strcasecmp(args->para2, "close") == 0) {
        ux_api_bt_switch_multiple_devices(false);
    } else {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}
at_ret_t at_btcfg_set_process(const ux_at_para5_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ux_at_para5_args_t *bt_args = (ux_at_para5_args_t *)args;
    ux_at_5para_cmd_pair_t btcfg_cmd[] = {
        {"addr", parse_bt_addr_para_cmd},
        {"name", parse_bt_name_para_cmd},
        {"ble", parse_ble_para_cmd},
        {"br", parse_br_para_cmd},
        {"pair", parse_bt_pair_cmd},
        {"disconnect", parse_bt_disconnect_cmd},
        {"remove", parse_bt_remove_cmd},
        {"ctkd", parse_bt_ctkd_cmd},
        {"interval", parse_bt_interval_cmd},
        {"phy", parse_bt_phy_cmd},
        {"multiple_devices", parse_multiple_devices_cmd},
    };
    return execute_cmd_handle(btcfg_cmd, arry_num(btcfg_cmd), bt_args->para1, (ux_at_para5_args_t*)args);
}

static void print_device_info(ux_bg_device_info_t *device_info)
{
    uint8_t idx = 0;
    uint8_t *addr = device_info->addr.addr;

    at_uart_printf("^name: %s\n", device_info->name);
    at_uart_printf(" id: %d\n", device_info->id);
    at_uart_printf(" addr:  ****%02x%02x%02x%02x\n",
        addr[BG_ADDR_INDEX_3], addr[BG_ADDR_INDEX_2], addr[BG_ADDR_INDEX_1], addr[BG_ADDR_INDEX_0]);
    at_uart_printf(" addr type: %d\n", device_info->addr.type);
    at_uart_printf(" rssi: %d\n", device_info->rssi);
    for (idx = 0; idx < arry_num(g_enum_str_protocol_map); idx++) {
        if (g_enum_str_protocol_map[idx].em_protocol == device_info->protocol) {
            at_uart_printf(" protocol: %s\n", g_enum_str_protocol_map[idx].str_protocol);
            break;
        }
    }
    if (idx == arry_num(g_enum_str_protocol_map)) {
        at_uart_printf(" protocol: unknown\n");
    }

    if (device_info->role == UX_API_BT_AS_PERIPHERAL) {
        at_uart_printf(" role: peripheral\n");
    } else if (device_info->role == UX_API_BT_AS_CENTRAL) {
        at_uart_printf(" role: central\n");
    } else {
        at_uart_printf(" role: unknown\n");
    }

    for (idx = 0; idx < arry_num(g_enum_str_connect_state_map); idx++) {
        if (g_enum_str_connect_state_map[idx].em_connect_state == device_info->connect_state) {
            at_uart_printf(" connect state: %s\n", g_enum_str_connect_state_map[idx].str_connect_state);
            break;
        }
    }
    if (idx == arry_num(g_enum_str_protocol_map)) {
        at_uart_printf(" connect state: unknown\n");
    }
}

#if CONFIG_SUPPORT_UX_SPP
static void show_spp_status(void)
{
    uint16_t ux_spp_port = get_ux_spp_port_number();
    if (get_ux_spp_service_connect_state(ux_spp_port)) {
        at_uart_printf("ux spp connect-state: connected\n");
    } else {
        at_uart_printf("ux spp connect-state: disconnected\n");
    }
    if (get_ux_spp_service_connect_state(DIAG_SPP_DEFAULT_PORT)) {
        at_uart_printf("diag spp connect-state: connected\n");
    } else {
        at_uart_printf("diag spp connect-state: disconnected\n");
    }
}
#endif

static void print_stack_info(ux_api_bt_protocol_t protocol)
{
    ux_bt_stack_info_t *stack_info = NULL;
    switch (protocol) {
        case UX_API_BT_PROTOCOL_BR:
#ifdef CONFIG_SUPPORT_BR
            stack_info = ux_bt_br_get_stack_info();
#endif
            break;
        case UX_API_BT_PROTOCOL_BLE:
#ifdef CONFIG_SUPPORT_BLE
            stack_info = ux_bt_ble_get_stack_info();
#endif
            break;
        default:
            return;
    }
    uint8_t i = 0;
    /* 打印协议栈使能状态 */
    for (i = 0; i < arry_num(g_enum_str_stack_state_map); i++) {
        if (g_enum_str_stack_state_map[i].em_stack_state == stack_info->stack_status) {
            at_uart_printf(" stack state: %s\n", g_enum_str_stack_state_map[i].str_stack_state);
            break;
        }
    }
    if (i == arry_num(g_enum_str_stack_state_map)) {
        at_uart_printf(" stack state: unknown\n");
    }

    if (stack_info->scan_status == UX_BT_DISCOVERY_STOP) {
        at_uart_printf(" scan: stop\n");
    } else if (stack_info->scan_status == UX_BT_DISCOVERY_START) {
        at_uart_printf(" scan: start \n");
    } else {
        at_uart_printf(" scan: unknown\n");
    }
    
    /* 打印协议广播状态 */
    if (stack_info->adv_status == UX_BT_DISCOVERY_STOP) {
        at_uart_printf(" adv: stop\n");
    } else if (stack_info->adv_status == UX_BT_DISCOVERY_START) {
        at_uart_printf(" adv: start \n");
    } else {
        at_uart_printf(" adv: unknown\n");
    }

#if CONFIG_SUPPORT_UX_SPP
    if (protocol == UX_API_BT_PROTOCOL_BR) {
        show_spp_status();
    }
#endif
}

at_ret_t at_btcfg_read_process(void)
{
    ux_bg_device_info_t *scan_device_list = ux_bg_get_scan_device_list()->head;
    ux_bg_device_info_t *paired_device_list = ux_bg_get_paired_device_list()->head;
    char bt_name[BS_BR_DEVICE_NAME_MAX_LEN] = {0};
    uint8_t addr[UX_API_BT_ADDR_LEN];

    if (ux_api_bt_get_name((uint8_t *)bt_name, BS_BLE_DEVICE_NAME_MAX_LEN) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }
    if (ux_bg_load_addr(NV_ID_BT_FACTORY_ADDR, addr, UX_API_BT_ADDR_LEN) != ERRCODE_SUCC) {
        return AT_RET_SYNTAX_ERROR;
    }

    at_uart_printf("bt name:%s\n", bt_name);
    at_uart_printf("bt addr:  ****%02x%02x%02x%02x\n",
        addr[BG_ADDR_INDEX_3], addr[BG_ADDR_INDEX_2], addr[BG_ADDR_INDEX_1], addr[BG_ADDR_INDEX_0]);
    at_uart_printf("multiple_devices:%s\r\n", ux_api_bt_is_multiple_device_open() ? "open" : "close");
    at_uart_printf("========br stack state========\n");
    print_stack_info(UX_API_BT_PROTOCOL_BR);
    at_uart_printf("========ble stack state========\n");
    print_stack_info(UX_API_BT_PROTOCOL_BLE);
    at_uart_printf("========paired list========\n");
    while (paired_device_list != NULL) {
        print_device_info(paired_device_list);
        paired_device_list = (ux_bg_device_info_t*)paired_device_list->next;
    }
    at_uart_printf("========scan list========\n");
    while (scan_device_list != NULL) {
        print_device_info(scan_device_list);
        scan_device_list = (ux_bg_device_info_t*)scan_device_list->next;
    }
    return AT_RET_OK;
}