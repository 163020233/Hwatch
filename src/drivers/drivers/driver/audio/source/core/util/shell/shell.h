/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: shell.h
 * Author: audio
 */

#ifndef __OSSHELL_H
#define __OSSHELL_H

#include "audio_type.h"
#if SAP_SHELL_SUPPORT
#include "audio_debug.h"

#define shell_log audio_osal_print

/* this is a shell module designed for the os */
/* this is a shell type,maybe a command or the data variables */
enum en_os_shell_type {
    EN_OSSHELL_CMD = 0,
    EN_OSSHELL_VAR, /* up till now, we only support 4 bytes */
    EN_OSSHELL_LAST,
};

#define build_var_name(a,b)         a##b
#define  MAX_TAB_MATCHES			16
struct shell_tab_matches {
    const td_char *matches[MAX_TAB_MATCHES];
    td_u16 len;
};

/* this is the shell function module.the register function must have the same type */
/* uptils now, we don't care the return value */
struct shell_item_t {
    const td_char     *name;  /* point to the shell name string */
    const td_char     *help;  /* point to the shell description string */
    td_void           *addr;  /* point to the shell function or the shell data */
    td_u16             type;  /* used to  point the shell type:command or a data */
    td_u16             len;   /* used to  point the shell command or data length */
};

/*  __attribute__((__used__)) */
/*  __attribute__((__section__(".initcall"))) */
/*  this define will create  a shell command with the specified cmdname */
#define osshell_export_cmd(cmdentry, cmdname, cmdhelp)                  \
    static const struct shell_item_t build_var_name(__oshell_,cmdentry) \
    __attribute__((__used__))                                       \
    __attribute__((__section__(".oshell")))=                        \
    {                                                                   \
        .name=cmdname,                                                  \
        .help=cmdhelp,                                                  \
        .addr=(td_void *)&cmdentry,                                     \
		.type=EN_OSSHELL_CMD,                                           \
		.len = sizeof(td_void *),                                       \
    }

/*  this define will create  a create a shell data with the specified name */
#define osshell_export_var(var, varname, varhelp)                   \
    static const struct shell_item_t build_var_name(__oshell_,var)  \
    __attribute__((__used__))                                   \
    __attribute__((__section__(".oshell")))=                    \
    {                                                               \
        .name=varname,                                              \
        .help=varhelp,                                              \
        .addr=(td_void *)&var,                                      \
		.type=EN_OSSHELL_VAR,                                       \
		.len =sizeof(var),                                          \
    }

td_s32 string2int(const td_char* str, td_u32* val);
#else
#define shell_log
#define osshell_export_cmd(cmdentry, cmdname, cmdhelp)
#define osshell_export_var(var, varname, varhelp)
#define string2int
#endif  /* end for the shell_config */

td_s32 shell_cmd_execute(td_char *param);            /* execute the command */
td_s32 shell_cmd_init(td_void);                      /* do the command table load */
const struct shell_tab_matches *shell_cmd_index(const td_char *index);  /* find the most like command */

#endif /* __OSSHELL_H */
