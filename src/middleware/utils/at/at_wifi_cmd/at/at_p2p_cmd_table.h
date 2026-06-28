/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2024. All rights reserved.
 * Description: example of using P2P APIs to pair P2P devices.
 */

#ifndef __AT_P2P_H__
#define __AT_P2P_H__

#include <td_base.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct {
    uint32_t                para_map;
    int32_t                 para1; /* Range: 0..32 */
} p2p_find_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *dev_name; /* Optional.   Length: 32 */
    int32_t                 wps_method; /* Optional. */
    int32_t                 listen_channel; /* Optional.Range: 1..14 */
    int32_t                 oper_channel; /* Optional.Range: 1..14 */
} p2pusercfg_args_t;

typedef struct {
    uint32_t                para_map;
    int32_t                 discoverd;  /* Range: 0..32 */
} p2p_peers_args_t;

typedef struct {
    uint32_t                para_map;
    const uint8_t           *peer_mac; /*    Length: 32 */
    int32_t                 go_intent; /* Range: 0..15 */
} p2pconn_args_t;

at_ret_t cmd_p2p_start(void);
at_ret_t cmd_wpa_p2p_find(const p2p_find_args_t *args);
at_ret_t cmd_wpa_p2p_stop(void);
at_ret_t cmd_wpa_p2p_stop_find(void);
at_ret_t cmd_wpa_p2p_peers(void);
/* AT Command */
at_ret_t cmd_wpa_p2p_connect(const p2pconn_args_t *args);

/* AT Command */
at_ret_t cmd_wpa_p2p_user_config(const p2pusercfg_args_t *args);

static const at_para_parse_syntax_t p2pfind_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE | AT_SYNTAX_ATTR_OPTIONAL,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 120,
        .offset = offsetof(p2p_find_args_t, para1)
    },
};

const int32_t p2pusercfg_wps_method_values[] = {
    0, 1, 2
};

const at_para_parse_syntax_t p2pusercfg_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(p2pusercfg_args_t, dev_name)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, p2pusercfg_wps_method_values},
        .offset = offsetof(p2pusercfg_args_t, wps_method)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 14,
        .offset = offsetof(p2pusercfg_args_t, listen_channel)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 14,
        .offset = offsetof(p2pusercfg_args_t, oper_channel)
    },
};

const at_para_parse_syntax_t p2pconn_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length = 32,
        .offset = offsetof(p2pconn_args_t, peer_mac)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 15,
        .offset = offsetof(p2pconn_args_t, go_intent)
    },
};

static const at_cmd_entry_t at_p2p_cmd_parse_table[] = {
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "P2PFIND",
        8,
        0,
        p2pfind_syntax,
        (at_set_func_t)NULL,
        (at_set_func_t)cmd_wpa_p2p_find,
        NULL,
        NULL,
    },
    {
        "P2PUSERCFG",
        11,
        0,
        p2pusercfg_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_p2p_user_config,
        NULL,
        NULL,
    },
    {
        "STOPP2P",
        8,
        0,
        NULL,
        (at_set_func_t)cmd_wpa_p2p_stop,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "P2PSTOPFIND",
        12,
        0,
        NULL,
        (at_set_func_t)cmd_wpa_p2p_stop_find,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "P2PPEERS",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_wpa_p2p_peers,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "P2PCONN",
        8,
        0,
        p2pconn_syntax,
        NULL,
        (at_set_func_t)cmd_wpa_p2p_connect,
        NULL,
        NULL,
    },
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
    {
        "STARTP2P",
        9,
        0,
        NULL,
        (at_set_func_t)cmd_p2p_start,
        (at_set_func_t)NULL,
        NULL,
        NULL,
    },
};

td_void los_at_p2p_cmd_register(td_void);

#ifdef __cplusplus
#if __cplusplus
    }
#endif
#endif

#endif /* end of at_p2p_cmd_table.h */
