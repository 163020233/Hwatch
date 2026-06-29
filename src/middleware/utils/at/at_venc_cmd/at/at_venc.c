/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At venc function \n
 *
 */

#include <string.h>

#include "at_cmd.h"
#include "at_venc_cmd_register.h"
#include "at_venc_cmd_table.h"
#include "debug_print.h"
#ifdef SUPPORT_VENC_TEST
#include "test_venc.h"
#endif
#include "td_base.h"
#include "at_venc.h"

#include "drv_venc_dfx.h"
#include "drv_venc_intf.h"

#define venc_assert_ret(condition, value)                             \
    do {                                                              \
        if (!(condition)) {                                           \
            printf("[%s %d]assert Error \r\n", __func__, __LINE__);   \
            return value;                                             \
        }                                                             \
    } while (0)

#define venc_assert_goto(cond, tag)                                   \
    do {                                                              \
        if (!(cond)) {                                                \
            printf("[%s %d]assert Error \r\n", __func__, __LINE__);   \
            goto tag;                                                 \
        }                                                             \
    } while (0)

#define AT_VENC_FUNC_NUM (sizeof(at_venc_cmd_parse_table) / sizeof(at_venc_cmd_parse_table[0]))
void los_at_venc_cmd_register(void)
{
    print_str("los_at_venc_cmd_register EXCUTE\r\n");
    uapi_at_venc_register_cmd(at_venc_cmd_parse_table, AT_VENC_FUNC_NUM);
}


static td_void venc_proc_help(td_void)
{
    printf("example wstp     : AT+VENC=venc_proc,cat,venc \r\n");
    printf("                 : AT+VENC=venc_proc,echo,print,0,venc \r\n");
    printf("example testsuit : a venc_proc cat venc \r\n");
    printf("                 : a venc_proc echo print 0 > venc \r\n");
}

static td_s32 venc_proc(td_s32 argc_tmp, td_char *argv_tmp[])
{
    td_s32 ret;
    td_s32 i;
    td_u32 j;
    td_u32 num;
    td_u32 is_find = 0;
    td_char *venc_proc_dir[] = { "venc", "venc_res", "venc_rc", "venc_jpeg", "venc_h264", "venc_h265" };
    td_s32 argc = argc_tmp - 1; // skip argv[0]: venc_proc
    td_char **argv = argv_tmp + 1; // skip argv[0]: venc_proc

    venc_assert_goto((argc >= 0x2 && argc <= 0x5), out);

    num = sizeof(venc_proc_dir) / sizeof(venc_proc_dir[0]);
    for (i = 0; i < argc && (is_find == 0); i++) {
        if (argv[i] == TD_NULL) {
            continue;
        }
        for (j = 0; j < num; j++) {
            if (strcmp(argv[i], venc_proc_dir[j]) == 0) {
                is_find = 1;
                break;
            }
        }
    }

    venc_assert_goto(is_find == 1, out);
    venc_assert_goto(j < num, out);
    venc_assert_ret(uapi_venc_module_init() == EXT_SUCCESS, EXT_FAILURE);

    if (strncmp(argv[0], "echo", strlen("echo")) == 0) {
        ret = uapi_venc_dfx_proc_write(argc - 1, (const char **)(&argv[1]));
    } else if (strncmp(argv[0], "cat", strlen("cat")) == 0) {
        ret = uapi_venc_dfx_proc_read(1, (const char **)(&venc_proc_dir[j]));
    } else {
        ret = EXT_FAILURE;
    }

    uapi_venc_module_exit();

    venc_assert_goto(ret == EXT_SUCCESS, out);

    return EXT_SUCCESS;

out:
    venc_proc_help();

    return EXT_FAILURE;
}

at_ret_t venc_process(const venc_para_t *arg)
{
    td_s32 i;
    td_s32 ret = EXT_FAILURE;
    char *argv[] = {arg->para1, arg->para2, arg->para3, arg->para4, arg->para5, arg->para6};
    td_s32 argc = sizeof(argv) / sizeof(char *);
    td_s32 anum = argc;

    PRINT("venc_process enter \r\n");
    for (i = 0; i < argc; i++) {
        if (argv[i] == TD_NULL) {
            anum--;
        }
    }

    if (strcmp(argv[0], "venc_proc") == 0) {
        ret = venc_proc(anum, argv);
        return AT_RET_OK;
    }

#ifdef SUPPORT_VENC_TEST
    ret = test_venc_process(sizeof(argv) / sizeof(char *), argv);
#endif

    return (ret == EXT_SUCCESS) ? AT_RET_OK : AT_RET_PROC_CMD_FUNC_MISSING;
}
