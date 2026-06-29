/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * File          lpc_atcmd.h
 * Description:  lpc at command
 */
#ifndef LPC_ATCMD_H
#define LPC_ATCMD_H

#include "errcode.h"
#include "at.h"
#include "dut.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    LPC_ATCMD_NUM_1,
    LPC_ATCMD_NUM_2,
    LPC_ATCMD_NUM_3,
};
typedef struct {
    uint32_t           para_mapl;
    const uint8_t      *para1;
} pm_args_t;

const char* g_pm_white_list[] = {"CMD", "NORMAL", "HAID"};

static const at_para_parse_syntax_t g_pm_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_LIST_VALUE,
        .entry.int_list = {3, g_pm_white_list},     /* 3 types CMD\NORMAL\HAID */
        .offset = offsetof(pm_args_t, para1)
    },
};

typedef struct {
    uint32_t           para_mapl;
    const uint8_t      *para1;
} lpc_args_t;

static const at_para_parse_syntax_t g_lpc_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_MAX_LENGTH,
        .entry.string.max_length  = 16,        /* lpc max length 16 */
        .offset = offsetof(lpc_args_t, para1)
    },
};

STATIC at_ret_t equip_enter_power_mode(const pm_args_t *args);
STATIC at_ret_t equip_lpc_switch_test(const lpc_args_t *args);
STATIC at_ret_t equip_lpc_status_get(const lpc_args_t *args);

const at_cmd_entry_t g_lpc_atcmd_table[] = {
    {
        "PM",
        LPC_ATCMD_NUM_1,
        DUT_CMD_LEVEL_WHITELIST,
        g_pm_syntax,
        NULL,
        equip_enter_power_mode,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        NULL,
#endif
    },
    {
        "LPC",
        LPC_ATCMD_NUM_2,
        DUT_CMD_LEVEL_WHITELIST,
        g_lpc_syntax,
        NULL,
        equip_lpc_switch_test,
        NULL,
        NULL,
#ifdef CONFIG_AT_SUPPORT_QUERY
        .query = equip_lpc_status_get,
#endif
    }
};

errcode_t lpc_equip_atcmd_set(void);

#ifdef __cplusplus
}
#endif

#endif