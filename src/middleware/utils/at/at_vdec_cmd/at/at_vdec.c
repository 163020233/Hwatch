/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At vdec function \n
 *
 */

#include <string.h>

#include "at_cmd.h"
#include "at_vdec_cmd_register.h"
#include "at_vdec_cmd_table.h"
#include "debug_print.h"
#ifdef SUPPORT_VDEC_TEST
#include "test_vdec.h"
#endif
#include "td_base.h"
#include "at_vdec.h"

#include "drv_vdec_dfx.h"
#include "drv_vdec_intf.h"

#define vdec_assert_ret(condition, value)                        \
    do {                                                              \
        if (!(condition)) {                                           \
            printf("[%s %d]assert Error \r\n", __func__, __LINE__);   \
            return value;                                             \
        }                                                             \
    } while (0)

#define vdec_assert_goto(cond, tag)                              \
    do {                                                              \
        if (!(cond)) {                                                \
            printf("[%s %d]assert Error \r\n", __func__, __LINE__);   \
            goto tag;                                                 \
        }                                                             \
    } while (0)

#define count_of(x)                             (sizeof(x) / sizeof((x)[0]))

#define AT_VDEC_FUNC_NUM (sizeof(at_vdec_cmd_parse_table) / sizeof(at_vdec_cmd_parse_table[0]))
void los_at_vdec_cmd_register(void)
{
    print_str("los_at_vdec_cmd_register EXCUTE\r\n");
    uapi_at_vdec_register_cmd(at_vdec_cmd_parse_table, AT_VDEC_FUNC_NUM);
}

static td_void vdec_vfmw_proc_help(td_void)
{
    PRINT("example wstp      : AT+VDEC=vfmw_proc,echo,print,0x3,vfmw_com \r\n");
    PRINT("                  : AT+VDEC=vfmw_proc,cat,vfmw_com \r\n");
    PRINT("example testsuit  : a vfmw_proc echo print 0x3 > vfmw_com \r\n");
    PRINT("                  : a vfmw_proc cat vfmw_com \r\n");

    PRINT("example wstp       : AT+VDEC=vdec_proc,echo,set_proc_chan,0,vdec \r\n");
    PRINT("                   : AT+VDEC=vdec_proc,cat,vdec \r\n");
    PRINT("example testsuit   : a vdec_proc echo set_proc_chan 0 > vdec \r\n");
    PRINT("                   : a vdec_proc cat vdec \r\n");
}

static td_s32 vfmw_proc(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_u32 i, j, is_find = 0;
    td_s32 argc_in = 0;
    td_char *argv_in[3] = { TD_NULL }; /* 3 is num */
    td_char *vfmw_proc_dir[] = {
        "vfmw_stm", "vfmw_syn", "vfmw_dec", "vfmw_com", "vfmw_fsp", "vfmw_kpi",
        "vfmw_pts", "vfmw_crc", "vfmw_mem", "vfmw_usr", "vfmw_ipt"
    };

    vdec_assert_ret(uapi_vdec_module_init() == EXT_SUCCESS, EXT_FAILURE);

    vdec_assert_goto((argc >= 2 && argc <= 5), out);

    for (i = 0; i < (td_u32)argc && (is_find == 0); i++) {
        if (argv[i] == TD_NULL) {
            continue;
        }
        for (j = 0; j < count_of(vfmw_proc_dir); j++) {
            if (strncmp(argv[i], vfmw_proc_dir[j], strlen(vfmw_proc_dir[j])) == 0) {
                is_find = 1;
                break;
            }
        }
    }

    vdec_assert_goto(is_find == 1, out);
    vdec_assert_goto(j < count_of(vfmw_proc_dir), out);

    if (strncmp(argv[0], "echo", strlen("echo")) == 0) {
        argv_in[0] = argv[1]; /* 1 index param 1 */
        argv_in[1] = argv[2]; /* 2 index param 2 */
        argv_in[2] = vfmw_proc_dir[j]; /* vfmw_xx 2 index */
        argc_in = 3; /* 3 is num */
        PRINT("echo %s %s > %s \r\n", argv_in[0], (argc == 3) ? " " : argv_in[1], argv_in[2]); /* 1 2 3 is num */
        ret = uapi_vfmw_dfx_proc_write(argc_in, (const char **)argv_in);
        vdec_assert_goto(ret == EXT_SUCCESS, out);
    } else if (strncmp(argv[0], "cat", strlen("cat")) == 0) {
        argv_in[0] = vfmw_proc_dir[j]; /* vfmw_xx */
        argv_in[1] = TD_NULL; /* 1 is num */
        argv_in[2] = TD_NULL; /* 2 is num */
        argc_in = 1;
        PRINT("cat %s \r\n", argv_in[0]);
        ret = uapi_vfmw_dfx_proc_read(argc_in, (const char **)argv_in);
        vdec_assert_goto(ret == EXT_SUCCESS, out);
    } else {
        goto out;
    }

    uapi_vdec_module_exit();

    return EXT_SUCCESS;
out:
    vdec_vfmw_proc_help();
    uapi_vdec_module_exit();
    return EXT_FAILURE;
}

static td_s32 vdec_proc(td_s32 argc, td_char *argv[])
{
    td_s32 ret;
    td_s32 i;
    td_s32 argc_in = 0;
    td_char *argv_in[3] = { TD_NULL }; /* 3 is num */

    vdec_assert_ret(uapi_vdec_module_init() == EXT_SUCCESS, EXT_FAILURE);

    if (argc < 2 | argc > 5) { /* 2 5 is num */
        PRINT("%s argc :%d is not arrage[2, 5] ", __func__, argc);
        goto out;
    }

    for (i = 0; i < argc; i++) {
        PRINT("arg[%d] = %s\r\n", i, argv[i]);
    }

    if (strncmp(argv[0], "echo", strlen("echo")) == 0) {
        argv_in[0] = argv[1]; /* 1 index param 1 */
        argv_in[1] = argv[2]; /* 2 index param 2 */
        argv_in[2] = "vdec";  /* vdec 2 index */
        argc_in = 3;          /* 3 is num */
        ret = uapi_vdec_dfx_proc_write(argc_in, (const char **)argv_in);
        vdec_assert_goto(ret == EXT_SUCCESS, out);
    } else if (strncmp(argv[0], "cat", strlen("cat")) == 0) {
        argv_in[0] = "vdec"; /* vdec */
        argv_in[1] = TD_NULL;
        argv_in[2] = TD_NULL; /* 2 is num */
        argc_in = 1;
        ret = uapi_vdec_dfx_proc_read(argc_in, (const char **)argv_in);
        vdec_assert_goto(ret == EXT_SUCCESS, out);
    } else {
        goto out;
    }

    uapi_vdec_module_exit();

    return EXT_SUCCESS;
out:
    vdec_vfmw_proc_help();
    uapi_vdec_module_exit();
    return EXT_FAILURE;
}

at_ret_t vdec_process(const vdec_para_t *arg)
{
    td_s32 i;
    td_s32 ret;
    td_u32 default_null_cnt = 0;
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6};
    td_s32 argc = sizeof(argv) / sizeof(char *);

    for (i = 0; i < argc; i++) {
        if (argv[i] == TD_NULL) {
            default_null_cnt++;
        }
    }

    if (strcmp(argv[0], "vdec_proc") == 0) {
        ret = vdec_proc(argc - default_null_cnt - 1, argv + 1);
        return AT_RET_OK;
    } else if (strcmp(argv[0], "vfmw_proc") == 0) {
        ret = vfmw_proc(argc - default_null_cnt - 1, argv + 1);
        return AT_RET_OK;
    } else {
        ret = EXT_FAILURE;
    }

#ifdef SUPPORT_VDEC_TEST
    PRINT("vdec_process enter \r\n");
    ret = test_vdec_process(argc, argv);
#endif
    return (ret == EXT_SUCCESS) ? AT_RET_OK : AT_RET_PROC_CMD_FUNC_MISSING;
}
