/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2023. All rights reserved.
 * Description: source file for shell cmd
 * Author: audio
 */

/**********************************README***************************************
 * 1, we will load the command table in shell_cmd_init
 * 2, uptil now, we support the keil,gcc,you could add more compiler here
 *    make sure you get the method to define a section and know its addr and size
*******************************************************************************/

/**************************************FILE INCLIUDES**************************/

#if SAP_SHELL_SUPPORT
#include <string.h>
#include <shell.h>
#include <link_misc.h>
#include "securec.h"

typedef td_s32 (*fn_shell_cmdentry)(td_s32 argc, const td_char *argv[]);

/**************************************FILE DEFINES****************************/
#define CN_SHELL_ARGS       64     /* at most could support 64 parameters for command */
#define CN_SHELL_VER_MAJOR  1
#define CN_SHELL_VER_MINOR  0

/**************************************FILE DATA STRUCTURE*********************/
struct shell_cb_t {
    struct shell_item_t *s;      /* static tabs */
    td_s32               s_num;  /* static number */
    struct shell_item_t *d;      /* dynamic list */
    td_s32               d_num;  /* dynamic number; */
};

/**************************************FILE VARS*******************************/
static struct shell_cb_t gs_shell_cb; /* shell controller here */

static const td_char *gs_os_shell_type[EN_OSSHELL_LAST] = {
    "CMD", \
    "VAR",
}; /* this will be used in show the shell type */
static struct shell_tab_matches tab_matches;
/**************************************FILE FUNCTIONS**************************/
/* export functions */
td_s32   shell_cmd_execute(td_char *param);            /* execute the command */
td_s32   shell_cmd_init(td_void);                      /* do the command table load */
const struct shell_tab_matches *shell_cmd_index(const td_char *index);  /* find the most like command */

static td_s32 shell_cmd_help(td_s32 argc, const td_char *argv[])
{
    td_s32 i;
    struct shell_item_t *item;

    (td_void)argc;
    (td_void)argv;
    shell_log("%-20s%-5s%-4s%-10s%-20s\n\r", "Name", "Type", "Len", "RunAddr", "Description");
    for (i = 0; i < gs_shell_cb.s_num; i++) {
        item = &(gs_shell_cb.s[i]);
        shell_log("%-20s%-5s%-4x%08x  %-30s\n\r", \
                  item->name, gs_os_shell_type[item->type % EN_OSSHELL_LAST], \
                  item->len, (td_u32)item->addr, item->help);
    }
    for (i = 0; i < gs_shell_cb.d_num; i++) {
        item = &(gs_shell_cb.d[i]);
        shell_log("%-2x  %-16s%-5s%-4x%08x  %-30s\n\r", \
                  i, item->name, gs_os_shell_type[item->type % EN_OSSHELL_LAST], \
                  item->len, (td_u32)item->addr, item->help);
    }
    return AUDIO_SUCCESS;
}

static struct shell_item_t *shell_cmd_match(const td_char *name)
{
    td_s32 i;
    struct shell_item_t *item = TD_NULL;
    struct shell_item_t *ret  = TD_NULL;

    /* search the static tab first,if not found then find the dynamic tab */
    for (i = 0; i < gs_shell_cb.s_num; i++) {
        item = &(gs_shell_cb.s[i]);
        if (0 == strcmp(name, item->name)) {
            ret = item;
            break;
        }
    }

    if (ret == TD_NULL) {
        for (i = 0; i < gs_shell_cb.d_num; i++) {
            item = &(gs_shell_cb.d[i]);
            if (0 == strcmp(name, item->name)) {
                ret = item;
                break;
            }
        }
    }

    return ret;
}

const struct shell_tab_matches *shell_cmd_index(const td_char *index)
{
    td_s32 i;
    struct shell_item_t *item;

    /* search the static tab first,if not found then find the dynamic tab */
    tab_matches.len = 0;
    for (i = 0; i < gs_shell_cb.s_num && tab_matches.len < MAX_TAB_MATCHES; i++) {
        item = &(gs_shell_cb.s[i]);
        if (strncmp(item->name, index, strlen(index)) == 0) {
            tab_matches.matches[tab_matches.len++] = item->name;
        }
    }

    for (i = 0; i < gs_shell_cb.d_num && tab_matches.len < MAX_TAB_MATCHES; i++) {
        item = &(gs_shell_cb.d[i]);
        if (strncmp(item->name, index, strlen(index)) == 0) {
            tab_matches.matches[tab_matches.len++] = item->name;
        }
    }

    return &tab_matches;
}

/*******************************************************************************
function     :used to execute the command specified by the string
instruction  :we will split the string with the blank space, and the first one
             will be used as the shell name to search the shell.If a shell cmd
             found then will execute the command,and then all the args will be
             passed to the function;if a data shell found, then will only do the
             "get" or set "action";attention that the first arg is the shell name
             itself!
*******************************************************************************/
td_s32 shell_cmd_execute(td_char *param)
{
    td_s32               i;
    td_u32               value;
    struct shell_item_t *item = TD_NULL;             /* match the command item */
    td_uchar            *bytes = TD_NULL;
    fn_shell_cmdentry    shell_cmd_entry;            /* command function entry */
    td_s32               ret = AUDIO_FAILURE;
    td_s32               argc = CN_SHELL_ARGS;       /* split the params to argc and argv format */
    const  td_char      *argv[CN_SHELL_ARGS] = {TD_NULL};

    string_to_arg(&argc, argv, param);    /* format the parameters */
    if (argc == 0) {
        shell_cmd_help(0, TD_NULL);       /* if no args we show system help */
        return AUDIO_SUCCESS;
    }

    item = shell_cmd_match(argv[0]); /* find the item */
    if (item == TD_NULL) {
        /* 找不到命令，shell_log("SHELL COMMAND NOT FIND:%s\n\r", argv[0]); */
        return AUDIO_FAILURE;
    }

    if (item->type == EN_OSSHELL_CMD) {
        shell_cmd_entry = (fn_shell_cmdentry)item->addr;
        shell_cmd_entry(argc, argv);
        return AUDIO_SUCCESS;
    }

    if ((argc == 3) && (0 == strcmp(argv[1], "set"))) { /* 3: argv num */
        /* deal it simple and easy here */
        value = strtol(argv[2], TD_NULL, 0); /* 2: argv idx */
        ret = memcpy_s(item->addr, item->len, &value, sizeof(value));
        if (ret != EOK) {
            sap_err_log_fun(memcpy_s, ret);
            return ret;
        }
    } else {
        bytes = item->addr;
        /* 打印内存对应的值，shell_log("(HEX):ADDR:0X%08X:", (td_u32)bytes); */
        for (i = 0; i < item->len; i++) {
            shell_log("%02x ", *bytes++);
        }
    }

    return AUDIO_SUCCESS;
}

/*******************************************************************************
function     :used to load the command table
instruction  :different compiler has different method to ge the section("oshell")
              so take care of your compiler
*******************************************************************************/
extern td_u32 _oshell_start;
extern td_u32 _oshell_end;

td_s32 shell_cmd_init(td_void)
{
    td_void  *cmd_start = TD_NULL;
    td_u32 len = 0;

    cmd_start = &_oshell_start;
    len = (td_u32)(td_uintptr_t)(&_oshell_end) - (td_u32)(td_uintptr_t)(&_oshell_start);
    if (len > 0) {
        len = len / sizeof(struct shell_item_t);
        gs_shell_cb.s = (struct shell_item_t *)cmd_start;
        gs_shell_cb.s_num = len;
    }
    return AUDIO_SUCCESS;
}

static td_s32 os_shell_version(td_s32 argc, td_char *argv)
{
    (td_void)argc;
    (td_void)argv;
    shell_log("os_shell_version:%d.%d\n\r", CN_SHELL_VER_MAJOR, CN_SHELL_VER_MINOR);
    return AUDIO_SUCCESS;
};

/* add the shell cmd */
osshell_export_cmd(os_shell_version, "version", "usage example: version");
osshell_export_cmd(shell_cmd_help, "help", "usage example: help");
#endif
