/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: factory test at cmd \n
 *
 */
#include "bt_ft_atcmd.h"
#include "at.h"
#include "app_at_uart.h"
#include "sle_errcode.h"
#include "sle_device_manager.h"
#include "sle_factory_manager.h"
#include "bts_factory.h"
#include "bts_br_factory.h"
#include "bts_br_gap.h"
#include "bts_device_manager.h"
#include "at_bt_productline.h"
#if defined(CONFIG_SUPPORT_FACTORY_TEST_SLE) && CONFIG_SUPPORT_FACTORY_TEST_SLE == 1
#define BT_FT_ATCMD_MAX 64
#define POWER_LEVEL_LEFT_VALUE 20
#define POWER_LEVEL_RIGHT_VALUE 126

enum {
    BT_FT_ATCMD_NUM_1 = 1,
    BT_FT_ATCMD_NUM_2,
    BT_FT_ATCMD_NUM_3,
    BT_FT_ATCMD_NUM_4,
    BT_FT_ATCMD_NUM_5,
    BT_FT_ATCMD_NUM_6,
    BT_FT_ATCMD_NUM_7,
    BT_FT_ATCMD_NUM_8,
    BT_FT_ATCMD_NUM_9,
    BT_FT_ATCMD_NUM_10,
};

static errcode_t bt_at_char_to_num_16(uint8_t ch, uint8_t *num)
{
    if (ch >= '0' && ch <= '9') {
        *num = ch - '0';
        return AT_RET_OK;
    } else if (ch >= 'a' && ch <= 'f') {
        *num = ch - 'a' + BT_FT_ATCMD_NUM_10;
        return AT_RET_OK;
    } else if (ch >= 'A' && ch <= 'F') {
        *num = ch - 'A' + BT_FT_ATCMD_NUM_10;
        return AT_RET_OK;
    }
    return AT_RET_SYNTAX_ERROR;
}

static errcode_t bt_at_str_to_addr(uint8_t *str, uint8_t *addr)
{
    uint8_t *str_cpy = str;
    uint8_t len = (uint8_t)strlen((char*)str_cpy);
    uint8_t num_h;
    uint8_t num_l;

    if (len != BD_ADDR_LEN * BT_FT_ATCMD_NUM_2) {
        if (len != BD_ADDR_LEN * BT_FT_ATCMD_NUM_2 + BT_FT_ATCMD_NUM_2) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (!(str_cpy[0] == '0' && (str_cpy[1] == 'x' || str_cpy[1] == 'X'))) {
            return AT_RET_SYNTAX_ERROR;
        }
        str_cpy += BT_FT_ATCMD_NUM_2;
        len -= BT_FT_ATCMD_NUM_2;
    }
    for (uint8_t i = 0; i < len - 1; i += BT_FT_ATCMD_NUM_2) {
        if ((bt_at_char_to_num_16(str_cpy[i], &num_h) != AT_RET_OK) ||
            (bt_at_char_to_num_16(str_cpy[i + 1], &num_l) != AT_RET_OK)) {
            return AT_RET_SYNTAX_ERROR;
        }
        addr[i / BT_FT_ATCMD_NUM_2] = (num_h << BT_FT_ATCMD_NUM_4) | num_l;
    }
    return AT_RET_OK;
}

static errcode_t bt_at_fixed_str_to_data(uint16_t str_len, uint8_t *str, uint16_t data_len, uint8_t *data)
{
    uint8_t *str_cpy = str;
    uint16_t len = str_len;
    uint8_t num_h;
    uint8_t num_l;

    if (len != data_len * BT_FT_ATCMD_NUM_2) {
        if (len != data_len * BT_FT_ATCMD_NUM_2 + BT_FT_ATCMD_NUM_2) {
            return AT_RET_SYNTAX_ERROR;
        }
        if (!(str_cpy[0] == '0' && (str_cpy[1] == 'x' || str_cpy[1] == 'X'))) {
            return AT_RET_SYNTAX_ERROR;
        }
        str_cpy += BT_FT_ATCMD_NUM_2;
        len -= BT_FT_ATCMD_NUM_2;
    }
    for (uint16_t i = 0; i < len - 1; i += BT_FT_ATCMD_NUM_2) {
        if ((bt_at_char_to_num_16(str_cpy[i], &num_h) != AT_RET_OK) ||
            (bt_at_char_to_num_16(str_cpy[i + 1], &num_l) != AT_RET_OK)) {
            return AT_RET_SYNTAX_ERROR;
        }
        data[i / BT_FT_ATCMD_NUM_2] = (num_h << BT_FT_ATCMD_NUM_4) | num_l;
    }
    return AT_RET_OK;
}

static errcode_t bt_at_str_to_data(uint8_t *str, uint16_t data_len, uint8_t *data)
{
    return bt_at_fixed_str_to_data((uint16_t)strlen((char*)str), str, data_len, data);
}

const int32_t bt_ft_sle_rf_tx_power_values[] = {
    0, 1, 2, 3, 4, 5, 6
};

const int32_t bt_ft_sle_rf_tx_payload_type_values[] = {
    0, 1, 2
};

const int32_t bt_ft_sle_rf_tx_phy_values[] = {
    0, 1, 4
};

const int32_t bt_ft_sle_rf_tx_format_values[] = {
    0, 1, 2
};

const int32_t bt_ft_sle_rf_tx_rate_values[] = {
    0, 1, 2, 3
};

const int32_t bt_ft_sle_rf_tx_pilot_ratio_values[] = {
    0, 1, 2, 3
};

const int32_t bt_ft_sle_rf_tx_polar_r_values[] = {
    0, 2, 3, 4, 5, 6, 7
};

const at_para_parse_syntax_t sle_rf_tx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x14E,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {7, bt_ft_sle_rf_tx_power_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_power)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, bt_ft_sle_rf_tx_payload_type_values},
        .offset = offsetof(sle_rf_tx_param_args_t, pk_payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, bt_ft_sle_rf_tx_phy_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, bt_ft_sle_rf_tx_format_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, bt_ft_sle_rf_tx_rate_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_rate)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, bt_ft_sle_rf_tx_pilot_ratio_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {7, bt_ft_sle_rf_tx_polar_r_values},
        .offset = offsetof(sle_rf_tx_param_args_t, tx_polar_r)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 6,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_tx_param_args_t, tx_interval)
    },
};

const int32_t bt_ft_sle_rf_rx_phy_values[] = {
    0, 1, 4
};

const int32_t bt_ft_sle_rf_rx_format_values[] = {
    0, 1, 2
};

const int32_t bt_ft_sle_rf_rx_pilot_ratio_values[] = {
    0, 1, 2, 3
};

const at_para_parse_syntax_t sle_rf_rx_write_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 78,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, bt_ft_sle_rf_rx_phy_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, bt_ft_sle_rf_rx_format_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_format)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, bt_ft_sle_rf_rx_pilot_ratio_values},
        .offset = offsetof(sle_rf_rx_param_args_t, rx_pilot_ratio)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 6,
        .entry.int_range.max_val = 255,
        .offset = offsetof(sle_rf_rx_param_args_t, rx_interval)
    },
};

const at_para_parse_syntax_t ble_rf_single_tone_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 78,
        .offset = offsetof(ble_rf_st_param_args_t, rf_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(ble_rf_st_param_args_t, rf_mode)
    },
};

const at_para_parse_syntax_t bt_rf_single_test_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(bt_sigle_test_param_args_t, open)
    },
};

const int32_t ble_rf_payload_type_values_list[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};
const int32_t ble_rf_phy_values_list[] = {
    1, 2, 3, 4
};
const int32_t ble_rf_cte_type_values_list[] = {
    0, 1, 2
};

const at_para_parse_syntax_t ble_rf_tx_v2_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 39,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, freq_idx)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 255,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {8, ble_rf_payload_type_values_list},
        .offset = offsetof(ble_rf_tx_v2_param_args_t, payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {4, ble_rf_phy_values_list},
        .offset = offsetof(ble_rf_tx_v2_param_args_t, phy)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xff,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, cte_len)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, ble_rf_cte_type_values_list},
        .offset = offsetof(ble_rf_tx_v2_param_args_t, cte_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, switch_pattern_len)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.string.max_length = 22,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, ant_ids)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = -127,
        .entry.int_range.max_val = 127,
        .offset = offsetof(ble_rf_tx_v2_param_args_t, power_level)
    },
};

static void ft_sle_print_status_cbk(errcode_t status)
{
    at_uart_printf("status:%x\r\n", status);
}

static void ft_sle_rf_trx_end_callback(sle_rf_trx_end_cmp_evt_t* evt)
{
    at_uart_printf("status:%x, num_packet:%u, rssi:%d\r\n", evt->status, evt->num_packets, (int8_t)(evt->rssi));
}

static at_ret_t bt_at_enable_sle_cmd(void)
{
    enable_sle();
    return AT_RET_OK;
}

/* SLE REGISTER CALLBACK */
static at_ret_t bt_at_sle_register_callback_cmd(void)
{
    sle_factory_callbacks_t func = {0};
    char* at_reg_str = "sle factory register";
    at_uart_printf("[AT] %s cbks.\n", at_reg_str);
    func.rf_tx_start_cb = ft_sle_print_status_cbk;
    func.rf_rx_start_cb = ft_sle_print_status_cbk;
    func.rf_trx_end_cb  = ft_sle_rf_trx_end_callback;
    func.rf_reset_cb    = ft_sle_print_status_cbk;
    errcode_t ret = sle_factory_register_callbacks(&func);
    if (ret != ERRCODE_SLE_SUCCESS) {
        at_uart_printf("%s callbacks fail ret :0x%x \r\n", at_reg_str, ret);
        return AT_RET_FORWORD_ERROR;
    }
    return AT_RET_OK;
}

/* sle长发 */
static at_ret_t bt_at_sle_rf_tx_cmd(const sle_rf_tx_param_args_t *args)
{
    sle_rf_tx_start_t rf_tx_start = { 0 };
    rf_tx_start.tx_freq = (uint16_t)args->tx_freq;
    // 发送频点为16bit非负整数，当bit9置为1时，表示开启跳频功能。
    if (rf_tx_start.tx_freq > 0x4E && rf_tx_start.tx_freq < 0x100) {
        return AT_RET_CMD_PARA_ERROR;
    }
    rf_tx_start.tx_power = (uint8_t)args->tx_power;
    rf_tx_start.test_data_len = (uint16_t)args->test_data_len;
    rf_tx_start.pk_payload_type = (uint8_t)args->pk_payload_type;
    rf_tx_start.tx_phy = (uint8_t)args->tx_phy;
    rf_tx_start.tx_format = (uint8_t)args->tx_format;
    rf_tx_start.tx_rate = (uint8_t)args->tx_rate;
    rf_tx_start.tx_pilot_ratio = (uint8_t)args->tx_pilot_ratio;
    rf_tx_start.tx_polar_r = (uint8_t)args->tx_polar_r;
    rf_tx_start.tx_interval = (uint16_t)args->tx_interval;
    sle_rf_tx_start(&rf_tx_start);
    return AT_RET_OK;
}

/* sle长收 */
static at_ret_t bt_at_sle_rf_rx_cmd(const sle_rf_rx_param_args_t *args)
{
    sle_rf_rx_start_t rf_rx_start = { 0 };
    rf_rx_start.rx_freq = (uint8_t)args->rx_freq;
    rf_rx_start.rx_phy = (uint8_t)args->rx_phy;
    rf_rx_start.rx_format = (uint8_t)args->rx_format;
    rf_rx_start.rx_pilot_ratio = (uint8_t)args->rx_pilot_ratio;
    rf_rx_start.rx_interval = (uint16_t)args->rx_interval;
    sle_rf_rx_start(&rf_rx_start);
    return AT_RET_OK;
}

/* sle TRX END */
static at_ret_t bt_at_sle_rf_trxend_cmd(void)
{
    sle_rf_trx_end();
    return AT_RET_OK;
}

/* BT SINGLE TEST */
static at_ret_t bt_at_single_test_cmd(const bt_sigle_test_param_args_t *args)
{
    if (args->open) {
        bt_factory_open_single_test();
    } else {
        bt_factory_close_single_test();
    }
    return AT_RET_OK;
}

/* sle RESET */
static at_ret_t bt_at_sle_reset_cmd(void)
{
    sle_rf_reset();
    return AT_RET_OK;
}

static at_ret_t bth_at_open(void)
{
    if (enable_ble() != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    return AT_RET_OK;
}

static at_ret_t bth_at_close(void)
{
    if (disable_ble() != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BLE常发V2 */
at_ret_t bt_at_ble_rf_tx_v2_cmd(const ble_rf_tx_v2_param_args_t *args)
{
    if (args == NULL) {
        return AT_RET_SYNTAX_ERROR;
    }
    ble_rf_tx_v2_start_t rf_tx_v2_start = { 0 };
    rf_tx_v2_start.freq_idx = args->freq_idx;
    rf_tx_v2_start.test_data_len = args->test_data_len;
    rf_tx_v2_start.payload_type = args->payload_type;
    rf_tx_v2_start.phy = args->phy;
    rf_tx_v2_start.cte_len = args->cte_len;
    rf_tx_v2_start.cte_type = args->cte_type;
    rf_tx_v2_start.switch_pattern_len = args->switch_pattern_len;
    if (rf_tx_v2_start.switch_pattern_len != 0) {
        if (bt_at_str_to_data((uint8_t *)args->ant_ids, (uint16_t)args->switch_pattern_len, rf_tx_v2_start.ant_ids) != 0) {
            return AT_RET_PARSE_PARA_ERROR;
        }
    }
    if (args->power_level > POWER_LEVEL_LEFT_VALUE && args->power_level < POWER_LEVEL_RIGHT_VALUE) {
        return AT_RET_PARSE_PARA_ERROR;
    }
    rf_tx_v2_start.power_level = args->power_level;
    if (ble_factory_rf_tx_v2_start(&rf_tx_v2_start) != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_cmd_entry_t bt_ft_atcmd_sle_parse_table[] = {
    {
        "SLEENABLE",
        1, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_enable_sle_cmd,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "SLEFACCALLBACK",
        2, // ID
        0,  // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_register_callback_cmd,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "SLETX",
        3, // ID
        0, // Attribute
        sle_rf_tx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_tx_cmd,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "SLERX",
        4, // ID
        0, // Attribute
        sle_rf_rx_write_param_syntax,
        NULL,
        (at_set_func_t)bt_at_sle_rf_rx_cmd,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "SLETRXEND",
        5, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_rf_trxend_cmd,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "SLERST",
        6, // ID
        0, // Attribute
        NULL,
        (at_cmd_func_t)bt_at_sle_reset_cmd,
        NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
};

const at_cmd_entry_t bt_ft_atcmd_bt_parse_table[] = {
    {
        "BLEENABLE",
        1,
        0,
        NULL,
        bth_at_open,
        (at_set_func_t)NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "BLEDISABLE",
        2,
        0,
        NULL,
        bth_at_close,
        (at_set_func_t)NULL,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "BTSIGNAL",
        3,
        0,
        bt_rf_single_test_param_syntax,
        NULL,
        (at_set_func_t)bt_at_single_test_cmd,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "BLETXV2",
        4,
        0,
        ble_rf_tx_v2_param_syntax,
        NULL,
        (at_set_func_t)bt_at_ble_rf_tx_v2_cmd,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
};

/* 0：1M BR GFSK 3：1M EDR QPSK 6：1M EDR 8PSK */
const int32_t br_rf_module_type_values_list[] = {0, 3, 6};
/* 3：DM1 4：DH1/2DH1 8：3DH1 10：2DH3 11：DH3/3DH3 14：2DH5 15：DH5/3DH5 */
const int32_t br_rf_code_type_values_list[] = {3, 4, 8, 10, 11, 14, 15};
/* 以下为BR产测 */
const at_para_parse_syntax_t br_rf_tx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x3FD,
        .offset = offsetof(br_rf_tx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute =  AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 12,  // max 12
        .offset = offsetof(br_rf_tx_param_args_t, addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xCE,
        .offset = offsetof(br_rf_tx_param_args_t, tx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, br_rf_module_type_values_list}, // size 3
        .offset = offsetof(br_rf_tx_param_args_t, module_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {7, br_rf_code_type_values_list}, // size 7
        .offset = offsetof(br_rf_tx_param_args_t, code_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 7, // max 7
        .offset = offsetof(br_rf_tx_param_args_t, payload_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 6, // max 6
        .offset = offsetof(br_rf_tx_param_args_t, power_level)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xff,
        .offset = offsetof(br_rf_tx_param_args_t, tx_channel)
    },
};

const at_para_parse_syntax_t br_rf_rx_param_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x3FD,
        .offset = offsetof(br_rf_rx_param_args_t, test_data_len)
    },
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute =  AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 12,  // max 12
        .offset = offsetof(br_rf_rx_param_args_t, addr)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0x4E,
        .offset = offsetof(br_rf_rx_param_args_t, rx_freq)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, br_rf_module_type_values_list}, // size 3
        .offset = offsetof(br_rf_rx_param_args_t, module_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {7, br_rf_code_type_values_list}, // size 7
        .offset = offsetof(br_rf_rx_param_args_t, code_type)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 0xff,
        .offset = offsetof(br_rf_rx_param_args_t, rx_channel)
    },
};

static at_ret_t bth_at_br_open(void)
{
    if (enable_bt_stack(BT_TRANSPORT_BR_EDR) != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    return AT_RET_OK;
}

static at_ret_t bth_at_br_close(void)
{
    if (disable_bt_stack(BT_TRANSPORT_BR_EDR) != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }

    return AT_RET_OK;
}

static void ft_br_print_status_cbk(errcode_t status)
{
    at_uart_printf("status:%x\r\n", status);
}

static void ft_br_rf_trx_end_callback(br_rf_trx_end_cmp_evt_t* evt)
{
    at_uart_printf("status:%x, num_packet:%u, rssi:%d\r\n", evt->status, evt->num_packets, (int8_t)(evt->rssi));
}

at_ret_t bt_at_br_register_callback_cmd(void)
{
    br_factory_callbacks_t func = {0};
    char* at_reg_str = "br factory register";
    at_uart_printf("[AT] %s cbks.\n", at_reg_str);
    func.br_rf_tx_start_cb = ft_br_print_status_cbk;
    func.br_rf_rx_start_cb = ft_br_print_status_cbk;
    func.br_rf_trx_end_cb  = ft_br_rf_trx_end_callback;
    func.br_rf_reset_cb    = ft_br_print_status_cbk;
    errcode_t ret = br_factory_register_callbacks(&func);
    if (ret != ERRCODE_SLE_SUCCESS) {
        at_uart_printf("%s callbacks fail ret :0x%x \r\n", at_reg_str, ret);
        return AT_RET_FORWORD_ERROR;
    }
    return AT_RET_OK;
}

/* BR常发 */
at_ret_t bt_at_br_rf_tx_cmd(const br_rf_tx_param_args_t *args)
{
    br_rf_tx_start_t param = {0};
    param.test_data_len = args->test_data_len;
    if (bt_at_str_to_addr((uint8_t *)args->addr, param.addr) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
    param.tx_freq = args->tx_freq;
    // 发送频点为8bit非负整数，当最高位置为1时，表示开启跳频功能。
    if (param.tx_freq > 0x4E && param.tx_freq < 0x80) {
        return AT_RET_CMD_PARA_ERROR;
    }
    param.module_type = args->module_type;
    param.code_type = args->code_type;
    param.payload_type = args->payload_type;
    param.power_level = args->power_level;
    param.tx_channel = args->tx_channel;
    if (br_factory_rf_tx_start(&param) != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* BR常收 */
at_ret_t bt_at_br_rf_rx_cmd(const br_rf_rx_param_args_t *args)
{
    br_rf_rx_start_t param = {0};
    param.test_data_len = args->test_data_len;
    if (bt_at_str_to_addr((uint8_t *)args->addr, param.addr) != AT_RET_OK) {
        return AT_RET_SYNTAX_ERROR;
    }
    param.rx_freq = args->rx_freq;
    param.module_type = args->module_type;
    param.code_type = args->code_type;
    param.rx_channel = args->rx_channel;
    if (br_factory_rf_rx_start(&param) != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

/* 长发长收结束 */
static at_ret_t bt_at_br_trx_end_cmd(void)
{
    if (br_factory_rf_trx_end() != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

static at_ret_t bt_at_br_reset_cmd(void)
{
    if (br_factory_rf_reset() != ERRCODE_BT_SUCCESS) {
        return AT_RET_SYNTAX_ERROR;
    }
    return AT_RET_OK;
}

const at_cmd_entry_t bt_ft_atcmd_br_parse_table[] = {
    {
        "BTENABLE",
        1,
        0,
        NULL,
        bth_at_br_open,
        (at_set_func_t)NULL,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTDISABLE",
        2,
        0,
        NULL,
        bth_at_br_close,
        (at_set_func_t)NULL,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTFACCALLBACK",
        3,
        0,
        NULL,
        bt_at_br_register_callback_cmd,
        (at_set_func_t)NULL,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTTX",
        4,
        0,
        br_rf_tx_param_syntax,
        NULL,
        (at_set_func_t)bt_at_br_rf_tx_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTRX",
        5,
        0,
        br_rf_rx_param_syntax,
        NULL,
        (at_set_func_t)bt_at_br_rf_rx_cmd,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTTRXEND",
        6,
        0,
        NULL,
        bt_at_br_trx_end_cmd,
        (at_set_func_t)NULL,
        NULL,
        NULL,
        NULL,
    },
    {
        "BTRST",
        6,
        0,
        NULL,
        bt_at_br_reset_cmd,
        (at_set_func_t)NULL,
        NULL,
        NULL,
        NULL,
    },
};
/* BR产测end */

errcode_t bt_ft_atcmd_init(void)
{
    errcode_t ret;
    los_at_bt_productline_cmd_register();
    ret = uapi_at_cmd_table_register(bt_ft_atcmd_sle_parse_table,
        sizeof(bt_ft_atcmd_sle_parse_table) / sizeof(at_cmd_entry_t), BT_FT_ATCMD_MAX);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_at_cmd_table_register(bt_ft_atcmd_bt_parse_table,
        sizeof(bt_ft_atcmd_bt_parse_table) / sizeof(at_cmd_entry_t), BT_FT_ATCMD_MAX);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    ret = uapi_at_cmd_table_register(bt_ft_atcmd_br_parse_table,
        sizeof(bt_ft_atcmd_br_parse_table) / sizeof(at_cmd_entry_t), BT_FT_ATCMD_MAX);
    return ret;
}
#endif
