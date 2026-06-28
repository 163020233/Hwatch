/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: WATCHDOG sample header \n
 *
 */

#ifndef WATCHDOG_SAMPLE_H
#define WATCHDOG_SAMPLE_H
#include <stdint.h>
#include "at.h"

#define EXT_AT_WATCHDOG_CMD_MAX_LEN 128
#define AT_LOG_MAX_LEN 512
#define AT_WATCHDOG_FUNC_NUM (sizeof(g_at_watchdog_cmd_parse_table) / sizeof(g_at_watchdog_cmd_parse_table[0]))

// 看门狗 AT 命令注册
void at_diting_watchdog_example_cmd_register(void);

typedef struct {
    uint32_t para_map;   // 参数映射表
    uint32_t kick_flag;  // 是否喂狗标志
} wd_init_args_t;        // 看么狗AT命令参数结构体

// 看门狗命令处理函数
at_ret_t at_watchdog_sample(wd_init_args_t *args);

static const at_para_parse_syntax_t g_wd_init_syntax[] = {
    {.type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range = {0, 1},  // 0:不喂狗, 1:喂狗
        .offset = offsetof(wd_init_args_t, kick_flag)},
};

static const at_cmd_entry_t g_at_watchdog_cmd_parse_table[] = {
    // AT+WDINIT
    {
        "WDINIT",          // 指令字符串 - 用户实际输入的AT命令（如"AT+GETADCMANUAL"）
        0x2260,            // 指令编码 - 用于内部识别的唯一标识符（十六进制形式）
        0,                 // 参数数量 - 该指令支持的参数个数
        g_wd_init_syntax,  // 指令参数语法定义 - 指向参数格式定义的指针
        NULL,              // 测试函数指针 - 用于AT+CMD=?测试命令的响应函数
        (at_set_func_t)at_watchdog_sample,  // 设置函数指针 - 执行AT+CMD=<...>时的处理函数
        NULL,                               // 查询函数指针 - 执行AT+CMD?查询命令时的处理函数
        NULL,                               // 其他功能函数指针 - 保留用于扩展功能
    },
};

#endif