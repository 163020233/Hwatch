/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 * Description: At nfc cmd table \n
 *
 * Date: 2025-09-19 \n
 */

#ifndef AT_NFC_CMD_TALBE_H
#define AT_NFC_CMD_TALBE_H

#include "at.h"
#include "nfc_hal_mxxce.h"

typedef struct {
    uint32_t para_map;
    uint32_t start_mode;
} nfc_cfg_start_mode_args;

typedef struct {
    uint32_t para_map;
    uint32_t card_index;
} nfc_cfg_card_index_args;

typedef struct {
    uint32_t        para_map;
    const uint8_t   *data;          /* HEX数据内容 */
    uint32_t        data_length;    /* HEX数据长度，用户无需配置 */
} nfc_hex_args;

at_ret_t at_nfc_reboot(void);
at_ret_t at_nfc_reset(void);
at_ret_t at_nfc_business_start(nfc_cfg_start_mode_args *args);
at_ret_t at_nfc_business_stop(void);
at_ret_t at_nfc_mxx_ce_switch_card(nfc_cfg_card_index_args *args);
at_ret_t at_nfc_open_log(void);
at_ret_t at_nfc_close_log(void);
at_ret_t at_nfc_send_nci_cmd(nfc_hex_args *args);

const at_para_parse_syntax_t nfc_start_mode_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 6,
        .offset = offsetof(nfc_cfg_start_mode_args, start_mode)
    },
};

const at_para_parse_syntax_t nfc_card_index_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = MAX_MXX_CARD_NUM,
        .offset = offsetof(nfc_cfg_card_index_args, card_index)
    },
};

const at_para_parse_syntax_t nfc_hex_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_OCTET_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_ADD_LENGTH,
        .offset = offsetof(nfc_hex_args, data),
        .entry.octet_string.max_length = AT_CMD_MAX_LENGTH,
        .entry.octet_string.length_field_offset = offsetof(nfc_hex_args, data_length)
    },
};

const at_cmd_entry_t g_at_nfc_cmd_parse_table[] = {
    {
        "NFCREBOOT",  // AT+NFCREBOOT
        0x1000,
        0,
        NULL,
        at_nfc_reboot,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "NFCRESET",  // AT+NFCRESET
        0x1001,
        0,
        NULL,
        at_nfc_reset,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "NFCBUSINESSSTART",  // AT+NFCBUSINESSSTART=?
        0x1002,
        0,
        nfc_start_mode_syntax,
        NULL,
        (at_set_func_t)at_nfc_business_start,
        NULL,
        NULL,
    },
    {
        "NFCBUSINESSSTOP",  // AT+NFCBUSINESSSTOP
        0x1003,
        0,
        NULL,
        at_nfc_business_stop,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "NFCMXXCESWITCHCARD",  // AT+NFCMXXCESWITCHCARD
        0x1004,
        0,
        nfc_card_index_syntax,
        NULL,
        (at_set_func_t)at_nfc_mxx_ce_switch_card,
        NULL,
        NULL,
    },
    {
        "NFCOPENLOG",  // AT+NFCOPENLOG
        0x1005,
        0,
        NULL,
        at_nfc_open_log,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "NFCCLOSELOG",  // AT+NFCCLOSELOG
        0x1006,
        0,
        NULL,
        at_nfc_close_log,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "NFCSENDNCICMD",  // AT+NFCSENDNCICMD=XX,XX,XX,...,XX
        0x1007,
        0,
        nfc_hex_syntax,
        NULL,
        (at_set_func_t)at_nfc_send_nci_cmd,
        NULL,
        NULL,
    },
};

#endif  /* AT_NFC_CMD_AT_CMD_TALBE_H */