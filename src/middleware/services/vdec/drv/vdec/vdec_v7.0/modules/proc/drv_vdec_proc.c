/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2019-2020. All rights reserved.
 * Description: decoder
 * Author: AuthorNameMagicTag
 * Create: 2019-04-22
 */

#include "drv_vdec_proc.h"
#include "drv_vdec.h"
#include "drv_vdec_osal.h"
#include "drv_vdec_sdk.h"
#include "securec.h"
#ifdef NO_UART_DEBUG
#include "diag_log.h"
#endif

typedef struct {
    td_s32 cur_chan;
    td_s32 bypass_chan;
    td_s32 cycle_mode_chan;
    td_s32 low_power_test_chan;
} vdec_proc_entry;

static vdec_proc_entry g_vdec_proc_entry = {
    .cur_chan = 0,
    .bypass_chan = -1,
    .cycle_mode_chan = -1,
    .low_power_test_chan = -1,
};

#define VDEC_PROC_BUFFER_SIZE 200
static osal_proc_entry *g_proc_vdec_entry = TD_NULL;

STATIC vdec_proc_entry* vdec_proc_get_entry(td_void)
{
    return &g_vdec_proc_entry;
}

STATIC td_u32 vdec_str_to_val_get_index(const td_char *str)
{
    td_u32 i;

    i = (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) ? 2 : 0; /* 2 :a number */

    return i;
}

STATIC td_u32 vdec_str_to_val_get_weight(const td_char *str)
{
    td_u32 weight;

    weight = (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) ? 16 : 10; /* 10 16 :a number */

    return weight;
}

STATIC td_s32 vdec_string_to_value(td_char *str, td_u32 str_len, td_u32 *data)
{
    td_u32 i, dat, weight;
    td_s32 d;

    if (str_len < 10) { /* 10 :a number */
        return EXT_FAILURE;
    }

    dat = 0;
    i = vdec_str_to_val_get_index(str);
    weight = vdec_str_to_val_get_weight(str);

    for (; i < 10; i++) {   /* 10 :a number */
        if (str[i] < 0x20) {
            break;
        } else if (weight == 16 && str[i] >= 'a' && str[i] <= 'f') {    /* 16 :a number */
            d = str[i] - 'a' + 10;  /* 10 :a number */
        } else if (weight == 16 && str[i] >= 'A' && str[i] <= 'F') {    /* 16 :a number */
            d = str[i] - 'A' + 10;  /* 10 :a number */
        } else if (str[i] >= '0' && str[i] <= '9') {
            d = str[i] - '0';
        } else {
            return EXT_FAILURE;
        }

        if (dat * weight < 0xFFFFFFFF) { /* 0xFFFFFFFF:td_u32 limit */
            dat = dat * weight + (td_u32)d;
        } else {
            vdec_print_err("%s %d overflow!\n", __func__, __LINE__);
            return EXT_FAILURE;
        }
    }

    *data = dat;

    return EXT_SUCCESS;
}

STATIC td_void vdec_proc_help_print(td_void)
{
    osal_printk("=================================vdec_help=================================\r\n");
    osal_printk(VDEC_HELP_FMT_H, "command", "para", "target", "comment");
    osal_printk(VDEC_HELP_FMT_L, VDEC_PROC_CMD_SET_CHAN,   "0", "vdec", "set chan proc alive",    "para(chn_id)");
    osal_printk(VDEC_HELP_FMT_L, VDEC_PROC_CMD_SET_BYPASS, "0", "vdec", "set chan bypass",        "para(chn_id)");
    osal_printk(VDEC_HELP_FMT_L, VDEC_PROC_CMD_SET_CYCLE,  "0", "vdec", "set chan cyclic output", "para(chn_id)");
#if defined(VDEC_DEBUG_SUPPORT)
    osal_printk(VDEC_HELP_FMT_L, VDEC_PROC_CMD_SET_MOCK,   "0", "vdec", "set chan as mock vdec",  "para(chn_id)");
#endif
    osal_printk("===========================================================================\r\n");
}

STATIC td_s32 vdec_proc_set_chan(td_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_s32 ret;
    td_u32 value = 0;
    vdec_proc_entry *entry = vdec_proc_get_entry();

    vdec_unused(argc);
    vdec_unused(private);
    ret = vdec_string_to_value(argv[1], PROC_CMD_SINGEL_LENGTH_MAX, &value); /* 1: arg idx */
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec_string_to_value err\n");
        return EXT_FAILURE;
    }
    vdec_check_ret(value < VDEC_MAX_CHAN_NUM);

    entry->cur_chan = (td_s32)value;
    vdec_print_alert("set proc chan %d \r\n", entry->cur_chan);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_proc_set_bypass(td_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_s32 ret;
    td_u32 bypass_chan = 0;
    vdec_proc_entry *entry = vdec_proc_get_entry();

    vdec_unused(argc);
    vdec_unused(private);
    ret = vdec_string_to_value(argv[1], PROC_CMD_SINGEL_LENGTH_MAX, &bypass_chan); /* 1: arg idx */
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec_string_to_value err\n");
        return EXT_FAILURE;
    }
    vdec_check_ret(bypass_chan < VDEC_MAX_CHAN_NUM || bypass_chan == 0xFFFFFFFF);

    entry->bypass_chan = (td_s32)bypass_chan;
    vdec_print_alert("set chan %d bypass \r\n", entry->bypass_chan);

    return EXT_SUCCESS;
}

STATIC td_s32 vdec_proc_set_cycle(td_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_s32 ret;
    td_u32 cycle_mode_chan = 0;
    vdec_proc_entry *entry = vdec_proc_get_entry();

    vdec_unused(argc);
    vdec_unused(private);
    ret = vdec_string_to_value(argv[1], PROC_CMD_SINGEL_LENGTH_MAX, &cycle_mode_chan); /* 1: arg idx */
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec_string_to_value err\n");
        return EXT_FAILURE;
    }
    vdec_check_ret(cycle_mode_chan < VDEC_MAX_CHAN_NUM || cycle_mode_chan == 0xFFFFFFFF);

    entry->cycle_mode_chan = (td_s32)cycle_mode_chan;
    vdec_print_alert("set chan %d in cycle mode \r\n", entry->cycle_mode_chan);

    return EXT_SUCCESS;
}

#if defined(VDEC_DEBUG_SUPPORT)
STATIC td_s32 vdec_proc_set_mock(td_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    td_s32 ret;
    td_u32 low_power_test_chan = 0;
    vdec_proc_entry *entry = vdec_proc_get_entry();

    vdec_unused(argc);
    vdec_unused(private);
    ret = vdec_string_to_value(argv[1], PROC_CMD_SINGEL_LENGTH_MAX, &low_power_test_chan); /* 1: arg idx */
    if (ret != EXT_SUCCESS) {
        vdec_print_err("vdec_string_to_value err\n");
        return EXT_FAILURE;
    }
    vdec_check_ret(low_power_test_chan < VDEC_MAX_CHAN_NUM || low_power_test_chan == 0xFFFFFFFF);

    entry->low_power_test_chan = (td_s32)low_power_test_chan;
    vdec_print_alert("set chan %d in low power test mode \r\n", entry->low_power_test_chan);

    return EXT_SUCCESS;
}
#endif

STATIC td_s32 vdec_proc_print_help(td_u32 argc, char (*argv)[PROC_CMD_SINGEL_LENGTH_MAX], td_void *private)
{
    vdec_unused(argc);
    vdec_unused(argv);
    vdec_unused(private);
    vdec_proc_help_print();

    return EXT_SUCCESS;
}

STATIC osal_proc_cmd g_vdec_proc_cmd_tbl[] = {
    { VDEC_PROC_CMD_HELP, vdec_proc_print_help },
    { VDEC_PROC_CMD_SET_CHAN, vdec_proc_set_chan },
    { VDEC_PROC_CMD_SET_BYPASS, vdec_proc_set_bypass },
    { VDEC_PROC_CMD_SET_CYCLE, vdec_proc_set_cycle },
#if defined(VDEC_DEBUG_SUPPORT)
    { VDEC_PROC_CMD_SET_MOCK, vdec_proc_set_mock },
#endif
};

td_s32 vdec_proc_get_cmd(td_void **cmd_list, td_u32 *cmd_cnt)
{
    td_u32 cnt;

    if ((cmd_list == TD_NULL) || (cmd_cnt == TD_NULL)) {
        vdec_print_err("args is null\n");
        return EXT_FAILURE;
    }

    *cmd_list = g_vdec_proc_cmd_tbl;
    cnt = (td_u32)sizeof(g_vdec_proc_cmd_tbl) / (td_u32)sizeof(osal_proc_cmd);
    *cmd_cnt = (td_u32)cnt;

    return EXT_SUCCESS;
}

td_s32 vdec_proc_read(td_void *buf, td_u32 len, td_u32 *used)
{
    vdec_proc_entry *entry = vdec_proc_get_entry();

    vdec_drv_proc_dump(buf, len, used, VP_D_D, "cur_proc_chan", entry->cur_chan, "bypass_chan_id", entry->bypass_chan);
    vdec_drv_proc_dump(buf, len, used, VP_D_D, "cycle_mode_chan", entry->cycle_mode_chan, "low_power_test_chan",
        entry->low_power_test_chan);
    drv_vdec_read_proc(buf, len, used, entry->cur_chan);

    return EXT_SUCCESS;
}

td_s32 vdec_drv_create_proc(td_char *name, fn_vdec_proc_read read_fn, osal_proc_cmd *cmd_list, td_u32 cmd_cnt)
{
    if (g_proc_vdec_entry != TD_NULL) {
        printf("%s g_proc_vdec_entry is ready!\n", __func__);
        return EXT_SUCCESS;
    }

    g_proc_vdec_entry = osal_vmalloc(sizeof(struct osal_proc_dir_entry));
    if (g_proc_vdec_entry == TD_NULL) {
        printf("%s osal_kmalloc failed!\n", __func__);
        return EXT_FAILURE;
    }
    (void)memset_s(g_proc_vdec_entry, sizeof(struct osal_proc_dir_entry), 0, sizeof(struct osal_proc_dir_entry));

    if (strncpy_s(g_proc_vdec_entry->name, OSAL_PROC_NAME_LENGTH, name, strlen(name)) != EOK) {
        printf("%s strncpy_s failed!\n", __func__);
        osal_vfree(g_proc_vdec_entry);
        return EXT_FAILURE;
    }

    g_proc_vdec_entry->read = (fn_vdec_proc_read)read_fn;
    g_proc_vdec_entry->cmd_cnt = cmd_cnt;
    g_proc_vdec_entry->cmd_list = cmd_list;

    return EXT_SUCCESS;
}

td_void vdec_drv_destroy_proc(td_char *name)
{
    vdec_unused(name);
    if (g_proc_vdec_entry != TD_NULL) {
        osal_vfree(g_proc_vdec_entry);
        g_proc_vdec_entry = TD_NULL;
    }
}

td_void vdec_drv_proc_dump(td_void *page, td_u32 page_count, td_u32 *used_bytes, const td_char *format, ...)
{
    td_s32 ret;
    va_list arg_list;
    td_char str[VDEC_PROC_BUFFER_SIZE] = { 0 };

    vdec_unused(page);
    vdec_unused(page_count);
    va_start(arg_list, format);
    ret = vsnprintf_s(str, VDEC_PROC_BUFFER_SIZE, VDEC_PROC_BUFFER_SIZE - 1, format, arg_list);
    if (ret < 0) {
        printf("%s sec func err page_count %d used_bytes %u\n", __func__, VDEC_PROC_BUFFER_SIZE, *used_bytes);
        va_end(arg_list);
        return;
    }
    (*used_bytes) += (td_u32)ret;
    va_end(arg_list);

#ifdef NO_UART_DEBUG
    uapi_diag_error_log(0, "%s", str);
#else
    printf("%s", str);
#endif
}

td_s32 vdec_drv_proc_read(td_s32 argc, const td_char *argv[])
{
    struct osal_proc_dir_entry *sproc = g_proc_vdec_entry;
    td_s32 ret;

    if (argc < 1 || argv[0] == TD_NULL) {
        printf("%s argc = %#x argv[0] = %s\n", __func__, argc, (argv[0] == TD_NULL) ? "TD_NULL" : argv[0]);
        return EXT_FAILURE;
    }

    if (sproc != TD_NULL) {
        sproc->seqfile = sproc;
        ret = sproc->read(sproc);
        if (ret != EXT_SUCCESS) {
            printf("%s proc = %s read err\n", __func__, argv[0]);
            return EXT_FAILURE;
        }
    } else {
        printf("%s proc = %s is not register\n", __func__, argv[0]);
        return EXT_FAILURE;
    }

    return EXT_SUCCESS;
}

td_s32 vdec_drv_proc_write(td_s32 argc, const td_char *argv[])
{
    td_u32 i;
    td_u32 argc_in = 2;
    td_s32 ret;
    struct osal_proc_dir_entry *sproc = g_proc_vdec_entry;
    char argv_in[PROC_CMD_NUM_MAX][PROC_CMD_SINGEL_LENGTH_MAX];

    if (argc < 3 || /* 3 num */
        argv[0] == TD_NULL ||
        argv[1] == TD_NULL ||
        argv[2] == TD_NULL) { /* 2 num */
        printf("%s argc = %#x argv[0] = %s argv[1] = %s argv[2] = %s\n", __func__, argc,
            (argv[0] == TD_NULL) ? "TD_NULL" : argv[0],
            (argv[1] == TD_NULL) ? "TD_NULL" : argv[1],
            (argv[2] == TD_NULL) ? "TD_NULL" : argv[2]); /* 2 num */
        return EXT_FAILURE;
    }

    for (i = 0; i < argc_in; i++) {
        ret = strcpy_s(argv_in[i], PROC_CMD_SINGEL_LENGTH_MAX, argv[i]);
        vdec_check_ret(ret == EOK);
    }

    if (sproc == TD_NULL) {
        printf("%s proc = %s is not register\n", __func__, argv[2]); /* 2 num */
        return EXT_FAILURE;
    }

    for (i = 0; i < sproc->cmd_cnt; i++) {
        if (strcmp(argv[0], sproc->cmd_list[i].name) == 0) {
            ret = sproc->cmd_list[i].handler(argc_in, argv_in, TD_NULL);
            if (ret != EXT_SUCCESS) {
                printf("%s %s %s %s write err\n", __func__, argv[0], argv[1], argv[2]); /* 2 num */
            }
            break;
        }
    }

    return EXT_SUCCESS;
}

td_u32 vdec_drv_proc_get_bypass(td_s32 chan_id)
{
    vdec_proc_entry *entry = vdec_proc_get_entry();

    if (chan_id == entry->bypass_chan) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_u32 vdec_drv_proc_get_cycle_mode(td_s32 chan_id)
{
    vdec_proc_entry *entry = vdec_proc_get_entry();

    if (chan_id == entry->cycle_mode_chan) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

td_u32 vdec_drv_proc_get_low_power_test_mode(td_s32 chan_id)
{
    vdec_proc_entry *entry = vdec_proc_get_entry();

    if (chan_id == entry->low_power_test_chan) {
        return TD_TRUE;
    } else {
        return TD_FALSE;
    }
}

