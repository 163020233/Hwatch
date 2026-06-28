/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: shell main
 * Author: audio
 */

#ifdef SAP_SHELL_SUPPORT
#include <shell.h>
#include <xtensa/hal.h>
#include "audio_type.h"
#include "platform.h"
#include "securec.h"

#ifndef SAP_IPC_LOG_SUPPORT
#include "serial_dw.h"
#endif

/**************************************FILE DEFINES****************************/
/* DEFINE VIRTUAL KEYS:VIRTUAL KEYS ARE COMBINED BY:0X1b 0xXX CODE */
#define CN_VIRTUAL_KEY_TD_NULL      (0x000000) /* TD_NULL */
#define CN_VIRTUAL_KEY_ARROWU       (0X41001B) /* UP */
#define CN_VIRTUAL_KEY_ARROWD       (0X42001B) /* DOWN */
#define CN_VIRTUAL_KEY_ARROWR       (0X43001B) /* RIGHT */
#define CN_VIRTUAL_KEY_ARROWL       (0X44001B) /* LEFT */
/* DEFINE SOME SPECIAL KEYS */
#define CN_KEY_LF                   (0X00000D) /* ENTER */
#define CN_KEY_CR                   (0X00000A) /* NEW */
#define CN_KEY_TAB                  (0X000009) /* TAB */
#define CN_KEY_BS                   (0X000008) /* BACK */
#define CN_KEY_ES                   (0X00001B) /* ESPACE */
/* DEFINES FOR THE COMMAND CACHE BUFFER */
#define CN_CMDLEN_MAX  128 /* THE COMMAND MAXLENGTH COULD CACHED */
#define CN_CMD_CACHE   4   /* THE COMMAND CACHED DEPTH */
/* DEFINES FOR THE SHELL SERVER TASK STACK SIZE */

#ifndef  CONFIG_SHELL_TASKSTACK
#define  CONFIG_SHELL_TASKSTACK   0x1000
#endif

#define VIRTUAL_MAGIC_OFFSET 8
#define KEY_CODE_OFFSET 16

/**************************************FILE DATA STRUCTURE*********************/
/* data structure used for cache the command */
struct shell_buffer_t {
    td_char     curcmd[CN_CMDLEN_MAX];               /* current command edited */
    td_uchar    curoffset;                           /* position to be edit */
    td_char     tab[CN_CMD_CACHE][CN_CMDLEN_MAX];    /* command cache table */
    td_uchar    taboffset;                           /* position to be cached in table */
};

typedef td_void (*key_cmd_func)(td_void);

typedef struct  {
    td_u32 vkmask;
    key_cmd_func func;
} shell_server_func;

/**************************************FILE VARS*******************************/
/* define your global variables here */
static td_char *gs_welcome_info = "\r\n*** WELCOME SHELL COMMAND ***\n";

/**************************************FUNCTION IMPLEMENT**********************/
/*******************************************************************************
function     :function used to get td_char from the input device
instruction  :you could reimplement by redirection,make sure it is block mode
*******************************************************************************/
static td_s32 shell_get_char(td_void)
{
#ifdef SAP_IPC_LOG_SUPPORT
    return EOF;
#else
    return serial_getc();
#endif /* SAP_IPC_LOG_SUPPORT */
}
/*******************************************************************************
function     :function used to put td_char to the output device
instruction  :you could reimplement by redirection
*******************************************************************************/
static td_void shell_put_char(td_s32 ch)
{
#ifdef SAP_IPC_LOG_SUPPORT
    audio_unused(ch);
#else
    serial_putc((td_char)ch);
#endif /* SAP_IPC_LOG_SUPPORT */
}

/*******************************************************************************
function     :function used to put a string
instruction  :you could reimplement by redirection
*******************************************************************************/
static td_void  shell_put_string(const td_char *str)
{
    td_s32 i;
    td_s32 len;

    len = strlen(str);
    for (i = 0; i < len; i++) {
        shell_put_char(*str);
        str++;
    }
}

/*******************************************************************************
function     :function used to prtd_s32 the path index
instruction  :if you has a getcwd function,you could modify this
*******************************************************************************/
static td_void shell_put_index(td_void)
{
    if (xthal_get_prid() == 0) {
        shell_put_string("\n\rdsp0:/>");
    } else {
        shell_put_string("\n\rdsp1:/>");
    }
}

/*******************************************************************************
function     :function used to delete a string in the user display
*******************************************************************************/
static td_void shell_put_backspace(td_s32 times)
{
    td_s32 i;
    td_char *str = "\b \b";

    for (i = 0; i < times; i++) {
        shell_put_string(str);
    }
}

/*******************************************************************************
function     :function used to put a space to display
*******************************************************************************/
static td_void shell_put_space(td_s32 times)
{
    td_s32 i;

    for (i = 0; i < times; i++) {
        shell_put_char(' ');
    }
}
/*******************************************************************************
function     :function used to move cursor right
*******************************************************************************/
static td_void  shell_moves_cursor_right(td_s32 times, td_u32 vk)
{
    td_s32 i;
    td_char ch;

    for (i = 0; i < times; i++) {
        ch = (td_char)(vk & 0xff);
        shell_put_char(ch);

        ch = (td_char)((vk >> VIRTUAL_MAGIC_OFFSET) & 0xff);
        shell_put_char(ch);

        ch = (td_char)((vk >> KEY_CODE_OFFSET) & 0xff);
        shell_put_char(ch);
    }
}
/*******************************************************************************
function     :function used ring the terminal's bell
*******************************************************************************/
static td_void shell_bell(td_void)
{
    shell_put_char('\a');
}
/*******************************************************************************
function     :function used to move cursor left
*******************************************************************************/
static td_void  shell_moves_cursor_left(td_s32 times)
{
    td_s32 i;

    for (i = 0; i < times; i++) {
        shell_put_char('\b');
    }
}

/*******************************************************************************
function     :function used cache the current command
instruction  :check if it has been cached.
*******************************************************************************/
static td_void shell_cachecmd(struct shell_buffer_t *tab)
{
    td_s32 i;
    td_s32 ret;
    td_s32 offset;

    for (i = 0; i < CN_CMD_CACHE; i++) {
        if (0 == strcmp(tab->curcmd, tab->tab[i])) {
            break;
        }
    }

    if (i == CN_CMD_CACHE) {
        offset = tab->taboffset;
        (td_void)memset_s(tab->tab[offset], CN_CMDLEN_MAX, 0, CN_CMDLEN_MAX);
        ret = strncpy_s(tab->tab[offset], CN_CMDLEN_MAX, tab->curcmd, CN_CMDLEN_MAX);
        if (ret != EOK) {
            sap_err_log_fun(strncpy_s, ret);
            return;
        }
        offset = (offset + 1) % CN_CMD_CACHE;
        tab->taboffset = offset;
    }

    return;
}

/*******************************************************************************
function     :insert string at the cursor's position
instruction  :check if it has been cached.
*******************************************************************************/
static td_void shell_insert_string(struct shell_buffer_t *tab, const td_char *str)
{
    td_s32 i;
    td_s32 ret;
    td_char *cursor = tab->curcmd + tab->curoffset;
    td_s32 cursor_len = strlen(cursor);
    td_s32 str_len = strlen(str);

    shell_put_string(str);
    shell_put_string(cursor);
    shell_moves_cursor_left(cursor_len);

    tab->curoffset += str_len;
    cursor = tab->curcmd + strlen(tab->curcmd) + str_len;
    *(cursor--) = 0;

    for (i = 0; i < cursor_len; i++) {
        *cursor = *(cursor - str_len);
        cursor--;
    }

    cursor = cursor - (str_len - 1);
    ret = strncpy_s(cursor, CN_CMDLEN_MAX - (td_uintptr_t)(cursor - tab->curcmd), str, str_len);
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return;
    }
}

/*******************************************************************************
function     :this is the  shell server task entry
instruction  :we get td_character from the input device each time,cached it in the
              current command line,if enter key received, then do command execute;
              if other virtual key received, then do the specified action.
*******************************************************************************/
static td_u32  vk     = CN_VIRTUAL_KEY_TD_NULL;
static td_u32  vkmask = CN_VIRTUAL_KEY_TD_NULL;
static struct shell_buffer_t shell_cmd_cache;

static td_s32 shell_server_init(td_void)
{
    audio_func_enter();

    shell_cmd_init();
    /* initialize the buffer */
    (td_void)memset_s(&shell_cmd_cache, sizeof(shell_cmd_cache), 0, sizeof(shell_cmd_cache));
    shell_put_string(gs_welcome_info);     /*  put the welcome information */
    shell_put_index();                     /*  do initialize */

    audio_func_exit();
    return AUDIO_SUCCESS;
}

static td_void key_execute_arrow_left(td_void)
{
    if (shell_cmd_cache.curoffset > 0) {
        shell_cmd_cache.curoffset--;
        /* push back the left cursor and make the terminal display know what has happened */
        shell_moves_cursor_left(1);
    } else {
        shell_bell();
    }
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_arrow_right(td_void)
{
    td_s32 len = strlen(shell_cmd_cache.curcmd);
    if (shell_cmd_cache.curoffset < len) {
        shell_cmd_cache.curoffset++;
        /* push back the right cursor and make the terminal display know what has happened */
        shell_moves_cursor_right(1, vk);
    } else {
        shell_bell();
    }
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_arrow_up(td_void)
{
    /*
     * first,we should clear what we has get,do the reset current command
     * first moves to the left,then putsnxt to the next to the right,then back
     */
    td_s32 ret;
    td_uchar offset;
    td_s32 len = strlen(shell_cmd_cache.curcmd);
    if (shell_cmd_cache.curoffset < len) {
        shell_put_space(len - shell_cmd_cache.curoffset);
        shell_cmd_cache.curoffset = len;
    }
    shell_put_backspace(len);
    (td_void)memset_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, 0, CN_CMDLEN_MAX);
    shell_cmd_cache.curoffset = 0;
    /* then copy the previous command to current and echo all the info */
    offset = (shell_cmd_cache.taboffset + CN_CMD_CACHE - 1) % CN_CMD_CACHE;
    ret = strncpy_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, shell_cmd_cache.tab[offset], CN_CMDLEN_MAX);
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return;
    }

    shell_cmd_cache.taboffset = offset;
    shell_cmd_cache.curoffset = strlen(shell_cmd_cache.curcmd);
    /* OK,now puts all the current td_character to the terminal */
    shell_put_string(shell_cmd_cache.curcmd);
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_arrow_down(td_void)
{
    /*
     * first,we should clear what we has get,do the reset current command
     * first moves to the right,then backspace all the input
     */
    td_s32 ret;
    td_uchar offset;
    td_s32 len = strlen(shell_cmd_cache.curcmd);
    if (shell_cmd_cache.curoffset < len) {
        shell_put_space(len - shell_cmd_cache.curoffset);
        shell_cmd_cache.curoffset = len;
    }
    shell_put_backspace(len);
    (td_void)memset_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, 0, CN_CMDLEN_MAX);
    shell_cmd_cache.curoffset = 0;
    /* then copy the next command to current */
    offset = (shell_cmd_cache.taboffset + CN_CMD_CACHE + 1) % CN_CMD_CACHE;
    ret = strncpy_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, shell_cmd_cache.tab[offset], CN_CMDLEN_MAX);
    if (ret != EOK) {
        sap_err_log_fun(strncpy_s, ret);
        return;
    }

    shell_cmd_cache.taboffset = offset;
    shell_cmd_cache.curoffset = strlen(shell_cmd_cache.curcmd);
    /* OK,now puts all the current td_character to the terminal */
    shell_put_string(shell_cmd_cache.curcmd);
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_tab(td_void)
{
    td_s32 i;
    const struct shell_tab_matches *matches;

    /* should search the command we has installed if some index has get */
    if (shell_cmd_cache.curoffset > 0) {
        td_char cursor = shell_cmd_cache.curcmd[shell_cmd_cache.curoffset];
        shell_cmd_cache.curcmd[shell_cmd_cache.curoffset] = 0;
        matches = shell_cmd_index((const td_char*)shell_cmd_cache.curcmd);
        shell_cmd_cache.curcmd[shell_cmd_cache.curoffset] = cursor;
        if (matches->len == 1) {
            /* only one cmd matched, insert the matched cmd at current cursor */
            shell_insert_string(&shell_cmd_cache, matches->matches[0] + shell_cmd_cache.curoffset);
        } else if (matches->len > 1) {
            shell_log("\n\r");
            for (i = 0; i < matches->len; i++) {
                shell_put_string(matches->matches[i]);
                shell_put_char('\t');
            }
            shell_log("\n\r");
            shell_put_index();
            shell_put_string(shell_cmd_cache.curcmd);
            shell_moves_cursor_left(strlen(shell_cmd_cache.curcmd) - shell_cmd_cache.curoffset);
        } else {
            shell_bell();
        }
    } else {
        shell_bell();
    }
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_enter(td_void)
{
    if (strlen(shell_cmd_cache.curcmd) != 0) {
        shell_log("\n\r");
        /*
         * copy the current to the history cache if the current command is not none
         * must do before the execute,execute will split the string
         */
        shell_cachecmd(&shell_cmd_cache);
        shell_cmd_execute(shell_cmd_cache.curcmd);  /* execute the command */
        (td_void)memset_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, 0, CN_CMDLEN_MAX);
        shell_cmd_cache.curoffset = 0;
    }
    shell_put_index();
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_cr(td_void)
{
    if (strlen(shell_cmd_cache.curcmd) != 0) {
        shell_log("\n\r");
        /*
         * copy the current to the history cache if the current command is not none
         * must do before the execute,execute will split the string
         */
        shell_cachecmd(&shell_cmd_cache);
        shell_cmd_execute(shell_cmd_cache.curcmd);  /* execute the command */
        (td_void)memset_s(shell_cmd_cache.curcmd, CN_CMDLEN_MAX, 0, CN_CMDLEN_MAX);
        shell_cmd_cache.curoffset = 0;
    }
    shell_put_index();
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_back_space(td_void)
{
    td_s32 ret;

    /* should delete the current td_character,move all the following td_character 1 position to before */
    if (shell_cmd_cache.curoffset > 0) {
        td_char *substr = shell_cmd_cache.curcmd + shell_cmd_cache.curoffset;
        td_s32 len = strlen(substr);
        shell_put_char('\b');
        shell_put_string(substr);
        shell_put_char(' ');
        shell_moves_cursor_left(len + 1);

        ret = strcpy_s(substr - 1, len + 1, substr);
        if (ret != EOK) {
            sap_err_log_fun(strcpy_s, ret);
            return;
        }

        shell_cmd_cache.curoffset--;
    } else {
        shell_bell();
    }
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;
}

static td_void key_execute_escape(td_void)
{
    vk = CN_KEY_ES;
    vkmask = CN_KEY_ES;
}

static const shell_server_func g_shell_server_func[] = {
    {CN_VIRTUAL_KEY_ARROWL, key_execute_arrow_left},
    {CN_VIRTUAL_KEY_ARROWR, key_execute_arrow_right},
    {CN_VIRTUAL_KEY_ARROWU, key_execute_arrow_up},
    {CN_VIRTUAL_KEY_ARROWD, key_execute_arrow_down},
    {CN_KEY_TAB, key_execute_tab},
    {CN_KEY_LF, key_execute_enter},
    {CN_KEY_CR, key_execute_cr},
    {CN_KEY_BS, key_execute_back_space},
    {CN_KEY_ES, key_execute_escape},
};

static td_void shell_server_entry(td_void)
{
    td_u32 i;
    td_s32 ch = shell_get_char();
    if ((ch == '\0') || (ch == EOF) || (ch == 0xFF)) {
        return;
    }

    if ((vk & 0xFF) != 0)  {
        if (((vk >> VIRTUAL_MAGIC_OFFSET) & 0xff) == CN_VIRTUAL_KEY_TD_NULL)  {
            vk |= (ch << VIRTUAL_MAGIC_OFFSET); /* 8:offset */
            return;
        } else {
            vk |= (ch << KEY_CODE_OFFSET); /* this is the vk code */
            vkmask |= (ch << KEY_CODE_OFFSET);
        }
    } else {
        vkmask  = ch;
    }

    for (i = 0; i < sizeof(g_shell_server_func) / sizeof(g_shell_server_func[0]); i++) {
        if (vkmask == g_shell_server_func[i].vkmask) {
            g_shell_server_func[i].func();
            return;
        }
    }

    /* push the td_character to the buffer until its full and the '\n' comes */
    if (shell_cmd_cache.curoffset < (CN_CMDLEN_MAX - 1)) {
        td_char *p = (td_char *)&ch;
        if (*p == 0) {
            /*  this architecture is big-endian */
            *p = (td_char)ch;
        }
        shell_insert_string(&shell_cmd_cache, p);
    } else {
        shell_bell();
    }
    /* flush the vk */
    vk = CN_VIRTUAL_KEY_TD_NULL;
    vkmask = CN_VIRTUAL_KEY_TD_NULL;

    return;
}

static platform_driver g_shell_driver = {
    .module_id = 0,
    .init = shell_server_init,
    .deinit = TD_NULL,
    .process = shell_server_entry,
};
module_driver(g_shell_driver);

#endif
