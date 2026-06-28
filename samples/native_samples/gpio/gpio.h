/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: GPIO sample header \n
 *
 */

#ifndef GPIO_SAMPLE_H
#define GPIO_SAMPLE_H
#include <stdint.h>
#include "at.h"

#define EXT_AT_GPIO_CMD_MAX_LEN   128

#define AT_GPIO_FUNC_NUM (sizeof(g_at_gpio_cmd_parse_table) / sizeof(g_at_gpio_cmd_parse_table[0]))

#define AT_LOG_MAX_LEN 512

void at_diting_gpio_example_cmd_register(void);

typedef struct {
    uint32_t para_map;
    uint32_t pin;
    uint32_t level;
} gpio_args_t;

typedef struct {
    uint32_t pin;
} gpio_intput_args_t;

typedef struct {
    uint32_t para_map;
    uint32_t pin;
    uint32_t dir;
} gpio_setdir_args_t;

at_ret_t at_gpio_output(gpio_args_t *args);
at_ret_t at_gpio_input(gpio_intput_args_t *args);
at_ret_t at_gpio_setdir(gpio_setdir_args_t *args);
at_ret_t at_gpio_toggle(gpio_args_t *args);

const at_para_parse_syntax_t g_gpio_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(gpio_args_t, pin)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gpio_args_t, level)
    },
};

const at_para_parse_syntax_t g_gpio_input_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(gpio_intput_args_t, pin)
    },
};

const at_para_parse_syntax_t g_gpio_setdir_args_syntax[] = {
    {
        .type = AT_SYNTAX_TYPE_INT,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 5,
        .offset = offsetof(gpio_setdir_args_t, pin)
    },
    {
        .type = AT_SYNTAX_TYPE_INT,
        .last = true,
        .attribute = AT_SYNTAX_ATTR_AT_MIN_VALUE | AT_SYNTAX_ATTR_AT_MAX_VALUE,
        .entry.int_range.min_val = 0,
        .entry.int_range.max_val = 1,
        .offset = offsetof(gpio_setdir_args_t, dir)
    },
};

const at_cmd_entry_t g_at_gpio_cmd_parse_table[] = {
    {
        "GPIOOUTPUT",
        0x2260,
        0,
        g_gpio_args_syntax,
        NULL,
        (at_set_func_t)at_gpio_output,
        NULL,
        NULL,
    },
    {
        "GPIOINPUT",
        0x2261,
        0,
        g_gpio_input_args_syntax,
        NULL,
        (at_set_func_t)at_gpio_input,
        NULL,
        NULL,
    },
    {
        "GPIOSETDIR",
        0x2262,
        0,
        g_gpio_setdir_args_syntax,
        NULL,
        (at_set_func_t)at_gpio_setdir,
        NULL,
        NULL,
    },
    {
        "GPIOTOGGLE",
        0x2263,
        0,
        g_gpio_args_syntax,
        NULL,
        (at_set_func_t)at_gpio_toggle,
        NULL,
        NULL,
    },
};

#endif