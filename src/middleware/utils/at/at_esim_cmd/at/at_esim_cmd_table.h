/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved. \n
 *
 * Description: esim \n
 */

#if !defined(AT_ESIM_CMD_TABLE_H)
#define AT_ESIM_CMD_TABLE_H

#include "at.h"
#include "at_esim.h"

typedef struct {
    uint32_t para_map;
    char *parm1;
} esim_at_param;

at_ret_t at_esim_mcu_get_info(void);

// LPA 相关接口
at_ret_t at_esim_mcu_set_active_code(esim_at_param *args);
at_ret_t at_esim_mcu_set_iccid(esim_at_param *args);
at_ret_t at_esim_init(void);
at_ret_t at_esim_mcu_reset(void);
at_ret_t at_esim_mcu_download(void);
at_ret_t at_esim_mcu_notification(void);
at_ret_t at_esim_mcu_disable_profile(esim_at_param *args);
at_ret_t at_esim_mcu_enable_profile(esim_at_param *args);
at_ret_t at_esim_mcu_delete_profile(void);
at_ret_t at_esim_mcu_edit_profile(esim_at_param *args);
at_ret_t at_esim_mcu_get_profile(esim_at_param *args);
at_ret_t at_esim_mcu_get_profile_list(esim_at_param *args);

// 机卡绑定相关接口
at_ret_t at_esim_mcu_get_status(void);
at_ret_t at_esim_mcu_esim_write_ppk(void);
at_ret_t at_esim_mcu_bind_card(void);
at_ret_t at_esim_mcu_verify_card(void);
at_ret_t at_esim_mcu_unlock_bind(void);
at_ret_t at_esim_mcu_bind_main(void);
at_ret_t at_esim_nv_test(void);

const at_para_parse_syntax_t esim_syntax[] = {
    {.type = AT_SYNTAX_TYPE_STRING,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_OPTIONAL | AT_SYNTAX_ATTR_MAX_LENGTH | AT_SYNTAX_ATTR_FIX_CASE,
        .entry.string.max_length = 256,
        .offset = offsetof(esim_at_param, parm1)},
};

const at_cmd_entry_t at_esim_cmd_parse_table[] = {
    {
        // AT+ESIMGETINFO
        "ESIMGETINFO",
        0xE001,
        0,
        NULL,
        at_esim_mcu_get_info,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMINIT
        "ESIMINIT",
        0xE002,
        0,
        NULL,
        at_esim_init,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMGETLIST
        "ESIMGETLIST",
        0xE003,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_get_profile_list,
        NULL,
        NULL,
    },
    {
        // AT+ESIMGETPROFILE
        "ESIMGETPROFILE",
        0xE004,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_get_profile,
        NULL,
        NULL,
    },
    {
        // AT+ESIMEDITPROFILE
        "ESIMEDITPROFILE",
        0xE005,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_edit_profile,
        NULL,
        NULL,
    },
    {
        // AT+ESIMDELETEPROFILE
        "ESIMDELETEPROFILE",
        0xE006,
        0,
        NULL,
        at_esim_mcu_delete_profile,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMENABLE
        "ESIMENABLE",
        0xE007,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_enable_profile,
        NULL,
        NULL,
    },
    {
        // AT+ESIMDISABLE
        "ESIMDISABLE",
        0xE008,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_disable_profile,
        NULL,
        NULL,
    },
    {
        // AT+ESIMNOTIFICATION
        "ESIMNOTIFICATION",
        0xE009,
        0,
        NULL,
        at_esim_mcu_notification,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMDOWNLOAD
        "ESIMDOWNLOAD",
        0xE00A,
        0,
        NULL,
        at_esim_mcu_download,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMRESET
        "ESIMRESET",
        0xE00B,
        0,
        NULL,
        at_esim_mcu_reset,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMBINDSTATUS
        "ESIMBINDSTATUS",
        0xE00C,
        0,
        NULL,
        at_esim_mcu_get_status,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMWRITEPPK
        "ESIMWRITEPPK",
        0xE00D,
        0,
        NULL,
        at_esim_mcu_esim_write_ppk,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMBIND
        "ESIMBIND",
        0xE00E,
        0,
        NULL,
        at_esim_mcu_bind_card,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMVERIFY
        "ESIMVERIFY",
        0xE00F,
        0,
        NULL,
        at_esim_mcu_verify_card,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMUNLOCK
        "ESIMUNLOCK",
        0xE020,
        0,
        NULL,
        at_esim_mcu_unlock_bind,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMSETICCID
        "ESIMSETICCID",
        0xE021,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_set_iccid,
        NULL,
        NULL,
    },
    {
        // AT+ESIMSETACCODE
        "ESIMSETACCODE",
        0xE022,
        0,
        esim_syntax,
        NULL,
        at_esim_mcu_set_active_code,
        NULL,
        NULL,
    },
    {
        // AT+ESIMBINDMAIN
        "ESIMBINDMAIN",
        0xE023,
        0,
        NULL,
        at_esim_mcu_bind_main,
        NULL,
        NULL,
        NULL,
    },
    {
        // AT+ESIMNVTEST
        "ESIMNVTEST",
        0xE024,
        0,
        NULL,
        at_esim_nv_test,
        NULL,
        NULL,
        NULL,
    },
};

#endif /* AT_ESIM_CMD_TABLE_H */