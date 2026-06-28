/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: GPIO sample header \n
 *
 */
#ifndef GPIO_SAMPLE_H
#define GPIO_SAMPLE_H
#include <stdint.h>
#include "at.h"

#define EXT_AT_TIMER_CMD_MAX_LEN 128

#define AT_TIMER_FUNC_NUM (sizeof(g_at_timer_cmd_parse_table) / sizeof(g_at_timer_cmd_parse_table[0]))

#define AT_LOG_MAX_LEN 512

// 定时器 AT 命令注册
void at_diting_timer_example_cmd_register(void);

typedef struct {
    uint32_t para_map;        // 映射参数
    uint32_t timer_pority;    // 优先级
    uint32_t timer_index_no;  // 序号
    uint32_t start_time1;     // 定时器1开始时间
    uint32_t end_time1;       // 定时器1结束时间
    uint32_t delay_time1;     // 定时器1延时时间
    uint32_t start_time2;     // 定时器1开始时间
    uint32_t end_time2;       // 定时器1结束时间
    uint32_t delay_time2;     // 定时器1延时时间
} timer_task_args_t;          // AT命令参数结构体

// AT命令入口函数
at_ret_t at_timer_sample(const timer_task_args_t *args);

static const at_para_parse_syntax_t g_timer_task_args_syntax[] = {
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,  // 定时器优先级最小值
        .entry.int_range.max_val = 7,  // 定时器优先级最大值
        .offset = offsetof(timer_task_args_t, timer_pority)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 1,
        .entry.int_range.max_val = 100000,  // 定时器序号索引值
        .offset = offsetof(timer_task_args_t, timer_index_no)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器1开始时间
        .offset = offsetof(timer_task_args_t, start_time1)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器1结束时间
        .offset = offsetof(timer_task_args_t, end_time1)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器1延时时间
        .offset = offsetof(timer_task_args_t, delay_time1)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器2开始时间
        .offset = offsetof(timer_task_args_t, start_time2)},
    {.type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器2结束时间
        .offset = offsetof(timer_task_args_t, end_time2)},
    {.type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 10000,  // 定时器2延时时间
        .offset = offsetof(timer_task_args_t, delay_time2)},
};

static const at_cmd_entry_t g_at_timer_cmd_parse_table[] = {
    // AT+TIMERTASK=1,1,0,0,1000,0,0,2000
    {
        "TIMERTASK",                     // 指令字符串 - 用户实际输入的AT命令（如"AT+GETADCMANUAL"）
        0x2264,                          // 指令编码 - 用于内部识别的唯一标识符（十六进制形式）
        0,                               // 参数数量 - 该指令支持的参数个数
        g_timer_task_args_syntax,        // 指令参数语法定义 - 指向参数格式定义的指针
        NULL,                            // 测试函数指针 - 用于AT+CMD=?测试命令的响应函数
        (at_set_func_t)at_timer_sample,  // 设置函数指针 - 执行AT+CMD=<...>时的处理函数
        NULL,                            // 查询函数指针 - 执行AT+CMD?查询命令时的处理函数
        NULL,                            // 其他功能函数指针 - 保留用于扩展功能
    },
};

#endif