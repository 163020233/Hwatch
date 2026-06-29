/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2012-2019. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2012-04-22
 */

#include "liteos_proc.h"
#include "vfmw_osal.h"
#include "dbg.h"
#include "soc_osal.h"
#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

#define PROC_BUFFER_SIZE 200

#ifdef VDEC_PROC_SUPPORT

static td_bool g_proc_init = TD_FALSE;
static osal_mutex g_mutex_proc;
static struct osal_list_head g_list;

td_s32 liteos_proc_create(td_char *proc_name, td_void *read, td_void *cmd_list, td_u32 cmd_cnt)
{
    osal_proc_entry *sentry = TD_NULL;

    sentry = os_alloc_vir(0, proc_name, sizeof(struct osal_proc_dir_entry));
    if (sentry == TD_NULL) {
        dprint(PRN_OS, "%s osal_kmalloc failed!\n", __func__);
        return OSAL_ERR;
    }
    (void)memset_s(sentry, sizeof(struct osal_proc_dir_entry), 0, sizeof(struct osal_proc_dir_entry));

    if (strncpy_s(sentry->name, OSAL_PROC_NAME_LENGTH, proc_name, strlen(proc_name)) != EOK) {
        dprint(PRN_OS, "%s strncpy_s failed!\n", __func__);
        os_free_vir(sentry);
        return OSAL_ERR;
    }

    osal_mutex_lock(&g_mutex_proc);
    osal_list_add_tail(&(sentry->node), &g_list);
    sentry->read = (proc_read_fn)read;
    sentry->cmd_cnt = cmd_cnt;
    sentry->cmd_list = cmd_list;
    osal_mutex_unlock(&g_mutex_proc);

    return OSAL_OK;
}

td_void liteos_proc_destroy(td_char *proc_name)
{
    struct osal_proc_dir_entry *sproc = TD_NULL;
    struct osal_proc_dir_entry *sproc_tmp = TD_NULL;

    if (proc_name == TD_NULL) {
        dprint(PRN_OS, "parameter invalid!\n");
        return;
    }

    osal_mutex_lock(&g_mutex_proc);

    if (osal_list_empty(&g_list) == 1) {
        osal_mutex_unlock(&g_mutex_proc);
        return;
    }
    osal_list_for_each_entry_safe(sproc, sproc_tmp, &g_list, node) {
        if (osal_strncmp(sproc->name, proc_name, strlen(proc_name)) == 0) {
            osal_list_del(&(sproc->node));
            os_free_vir(sproc);
            sproc = TD_NULL;
            break;
        }
    }
    osal_mutex_unlock(&g_mutex_proc);
}

td_void liteos_proc_dump(td_void *page, td_s32 page_count, td_s32 *used_bytes, td_s8 from_shr,
    const td_char *format, ...)
{
    td_s32 ret;
    OS_VA_LIST arg_list;
    td_char str[PROC_BUFFER_SIZE] = { 0 };

    vfmw_unused(page);
    vfmw_unused(page_count);
    vfmw_unused(from_shr);
    OS_VA_START(arg_list, format);
    ret = vsnprintf_s(str, PROC_BUFFER_SIZE, PROC_BUFFER_SIZE - 1, format, arg_list);
    if (ret < 0) {
        dprint(PRN_ALWS, "%s sec func err page_count %d used_bytes %d\n", __func__, PROC_BUFFER_SIZE, *used_bytes);
        OS_VA_END(arg_list);
        return;
    }

    (*used_bytes) += ret;
    OS_VA_END(arg_list);

#ifdef NO_UART_DEBUG
    uapi_diag_error_log(0, "%s", str);
#else
    os_print("%s", str);
#endif
}

td_s32 liteos_proc_init(td_void)
{
    td_s32 ret;

    ret = osal_mutex_init(&g_mutex_proc);
    if (ret != 0) {
        dprint(PRN_OS, "%s ret = %#x.\n", __func__, ret);
        return OSAL_ERR;
    }
    osal_mutex_lock(&g_mutex_proc);
    OSAL_INIT_LIST_HEAD(&g_list);
    osal_mutex_unlock(&g_mutex_proc);

    g_proc_init = TD_TRUE;

    return OSAL_OK;
}

td_void liteos_proc_exit(td_void)
{
    osal_mutex_destroy(&g_mutex_proc);
    g_proc_init = TD_FALSE;

    return;
}

STATIC td_s32 liteos_proc_read(td_s32 argc, const td_char *argv[])
{
    td_s32 ret;
    td_u32 is_find = 0;
    struct osal_proc_dir_entry *sproc = TD_NULL;

    if (argc < 1 || argv[0] == TD_NULL) {
        dprint(PRN_OS, "%s argc = %#x argv[0] = %s\n", __func__, argc, (argv[0] == TD_NULL) ? "TD_NULL" : argv[0]);
        return OSAL_ERR;
    }

    osal_mutex_lock(&g_mutex_proc);
    osal_list_for_each_entry(sproc, &g_list, node) {
        if (osal_strncmp(sproc->name, argv[0], strlen(argv[0])) == 0) {
            is_find = 1;
            break;
        }
    }

    if (is_find == 1 && sproc != TD_NULL) {
        sproc->seqfile = sproc; /* just not NULL is ok */
        ret = sproc->read(sproc);
        if (ret != OSAL_OK) {
            dprint(PRN_OS, "%s proc = %s read err\r\n", __func__, argv[0]);
            osal_mutex_unlock(&g_mutex_proc);
            return OSAL_ERR;
        }
    } else {
        dprint(PRN_OS, "%s proc = %s is not register\n", __func__, argv[0]);
        osal_mutex_unlock(&g_mutex_proc);
        return OSAL_ERR;
    }

    osal_mutex_unlock(&g_mutex_proc);

    return OSAL_OK;
}

STATIC td_s32 liteos_proc_write(td_s32 argc, const td_char *argv[])
{
    struct osal_proc_dir_entry *sproc = TD_NULL;
    td_u32 is_find = 0;
    td_u32 i;
    td_u32 argc_in = 2;
    td_s32 ret;
    char argv_in[PROC_CMD_NUM_MAX][PROC_CMD_SINGEL_LENGTH_MAX];

    if (argc < 3 || argv[0] == TD_NULL || argv[1] == TD_NULL || argv[2] == TD_NULL) { /* 3 2 num */
        dprint(PRN_OS, "%s argc = %#x argv[0] = %s argv[1] = %s argv[2] = %s\n", __func__, argc,
            (argv[0] == TD_NULL) ? "TD_NULL" : argv[0],
            (argv[1] == TD_NULL) ? "TD_NULL" : argv[1],
            (argv[2] == TD_NULL) ? "TD_NULL" : argv[2]); /* 2 num */
        return OSAL_ERR;
    }

    osal_mutex_lock(&g_mutex_proc);
    osal_list_for_each_entry(sproc, &g_list, node) {
        if (osal_strncmp(sproc->name, argv[2], strlen(argv[2])) == 0) { /* 2 num */
            is_find = 1;
            break;
        }
    }

    for (i = 0; i < argc_in; i++) {
        ret = strcpy_s(argv_in[i], PROC_CMD_SINGEL_LENGTH_MAX, argv[i]);
        vfmw_check_ret(ret == EOK, OSAL_ERR);
    }

    if (is_find == 0) {
        dprint(PRN_OS, "%s proc = %s is not register\n", __func__, argv[2]); /* 2 num */
        osal_mutex_unlock(&g_mutex_proc);
        return OSAL_ERR;
    }

    for (i = 0; i < sproc->cmd_cnt; i++) {
        if (strcmp(argv[0], sproc->cmd_list[i].name) == 0) {
            ret = sproc->cmd_list[i].handler(argc_in, argv_in, TD_NULL);
            if (ret != OSAL_OK) {
                dprint(PRN_OS, "%s %s %s %s write err\n", __func__, argv[0], argv[1], argv[2]); /* 2 num */
            }
            break;
        }
    }

    if (i == sproc->cmd_cnt) {
        dprint(PRN_OS, "%s %s is not find!\n", __func__, argv[0]);
    }
    osal_mutex_unlock(&g_mutex_proc);

    return OSAL_OK;
}
#endif

td_s32 uapi_vfmw_dfx_proc_read(td_s32 argc, const td_char *argv[])
{
#ifdef VDEC_PROC_SUPPORT
    if (g_proc_init == TD_FALSE) {
        dprint(PRN_OS, "vfmw proc not init.\n");
        return OSAL_ERR;
    }

    return liteos_proc_read(argc, argv);
#else
    (td_void)(argc);
    (td_void)(argv);
    dprint(PRN_OS, "VDEC_PROC_SUPPORT is not defined.\n");
    return EXT_FAILURE;
#endif
}

td_s32 uapi_vfmw_dfx_proc_write(td_s32 argc, const td_char *argv[])
{
#ifdef VDEC_PROC_SUPPORT
    if (g_proc_init == TD_FALSE) {
        dprint(PRN_OS, "vfmw proc not init.\n");
        return OSAL_ERR;
    }

    return liteos_proc_write(argc, argv);
#else
    (td_void)(argc);
    (td_void)(argv);
    dprint(PRN_OS, "VDEC_PROC_SUPPORT is not defined.\n");
    return EXT_FAILURE;
#endif
}
