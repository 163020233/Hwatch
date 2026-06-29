/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: At audio function \n
 *
 */

#include "at_cmd.h"
#ifdef CONFIG_SUPPORT_WIFI
#include "at_utils.h"
#endif
#include "at_audio_cmd_register.h"
#include "at_audio_cmd_table.h"
#include "debug_print.h"

#define TEST_SUITE_MAX_ARGC 16
#define TEST_SUITE_MAX_ARGV 16

#define AT_AUDIO_FUNC_NUM (sizeof(at_audio_cmd_parse_table) / sizeof(at_audio_cmd_parse_table[0]))

void los_at_audio_cmd_register(void)
{
    print_str("los_at_audio_cmd_register EXCUTE\r\n");
    uapi_at_audio_register_cmd(at_audio_cmd_parse_table, AT_AUDIO_FUNC_NUM);
}

static uint32_t audio_para_to_args(const uint8_t *para_str, int32_t *argc, char *msg_argv[])
{
    int32_t ret;
    int32_t len;
    char *p = NULL;
    char *context = NULL;

    p = strtok_s((char*)para_str, " ", &context);
    for (int32_t i = 0; p != NULL; i++) {
        len = strlen(p) + 1;

        msg_argv[i] = (char*)malloc(len);
        if (msg_argv[i] == NULL) {
            osal_printk("len = %d\r\n", len);
            osal_printk("Call malloc fail for msg_argv.\r\n");
            return ERRCODE_FAIL;
        }

        ret = memcpy_s(msg_argv[i], len, p, len);
        if (ret != EOK) {
            osal_printk("Call memcpy_s fail for msg_argv.\r\n");
            return ERRCODE_FAIL;
        }

        (*argc)++;
        if (*argc >= TEST_SUITE_MAX_ARGC) {
            osal_printk("argc = %d, TEST_SUITE_MAX_ARGC = %d\r\n", *argc, TEST_SUITE_MAX_ARGC);
            osal_printk("Param num out of range.\r\n");
            return ERRCODE_FAIL;
        }

        p = strtok_s(NULL, " ", &context);
    }

    return ERRCODE_SUCC;
}

static uint32_t audio_at_cmd_handle(const uint8_t *para, uint32_t paraLen)
{
    int32_t argc = 0;
    uint32_t ret = ERRCODE_SUCC;
    uint8_t *para_str = NULL;
    char *msg_argv[TEST_SUITE_MAX_ARGC] = {NULL};

    if (para == NULL) {
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    para_str = (uint8_t*)malloc(paraLen + 1); /* AT送过来的是命令序列，不是字符串，所以没有结束符 需要手动加上结束符 */
    if (para_str == NULL) {
        osal_printk("paraLen = %d\r\n", paraLen);
        osal_printk("Call malloc fail for para_str.\r\n");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    ret = (uint32_t)memcpy_s(para_str, paraLen, para, paraLen);
    if (ret != EOK) {
        osal_printk("Call memcpy_s fail for para_str.\r\n");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    para_str[paraLen] = '\0'; /* 给命令序列加上结束符， 方便下面做字符串处理 */

    ret = audio_para_to_args(para_str, &argc, msg_argv);
    if (ret != ERRCODE_SUCC) {
        osal_printk("Call audio_para_to_args fail.\r\n");
        ret = ERRCODE_FAIL;
        goto EXIT_PROC;
    }

    audio_execute_function(argc, msg_argv);

EXIT_PROC:
    for (int32_t i = 0; i < argc; i++) {
        if (msg_argv[i] != NULL) {
            free(msg_argv[i]);
        }
    }

    if (para_str != NULL) {
        free(para_str);
    }

    return ret;
}

at_ret_t audio_at_cmd_process(const audio_at_cmd_args_t *args)
{
    uint32_t ret;

    ret = audio_at_cmd_handle((const uint8_t *)args->para1, strlen(args->para1));
    if (ret != ERRCODE_SUCC) {
        osal_printk("Call sap_audio_at_cmd_handle fail.\r\n");
    }

    return AT_RET_OK;
}