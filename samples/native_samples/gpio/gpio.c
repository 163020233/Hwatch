/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: GPIO sample implementation \n
 *
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "uart.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"
#include "errcode.h"
#include "hal_gpio.h"
#include "debug_print.h"
#include "driver/pinctrl.h"
#include "driver/gpio.h"
#include "gpio.h"

void at_diting_gpio_example_cmd_register(void)
{
    uapi_at_cmd_table_register(g_at_gpio_cmd_parse_table, AT_GPIO_FUNC_NUM, EXT_AT_GPIO_CMD_MAX_LEN);
}

static errcode_t gpio_common_init(uint32_t pin)
{
    // pin初始化
    uapi_pin_init();
    // gpio初始化
    uapi_gpio_init();
    // 设置为gpio模式
    errcode_t ret = ERRCODE_SUCC;
    ret = uapi_pin_set_mode(pin, PIN_MODE_0);
    if (ret != ERRCODE_SUCC) {
        printf("gpio set mode failed\n");
        return AT_RET_RUN_ERROR;
    } else {
        printf("gpio set mode done\n");
    }

    return ret;
}

static errcode_t gpio_common_deinit(void)
{
    // pin去初始化
    uapi_pin_deinit();
    // gpio去初始化
    uapi_gpio_deinit();

    return ERRCODE_SUCC;
}

// gpio输出
at_ret_t at_gpio_output(gpio_args_t *args)
{
    printf("========== gpio output sample test start ==========\n");
    uint32_t pin = (uint32_t)args->pin;
    uint32_t level = (uint32_t)args->level;
    printf("[params] pin:%d, level: %d.\n", (uint32_t)pin, (uint32_t)level);
    // step1: pin和gpio初始化,设置为gpio模式
    errcode_t ret = gpio_common_init(pin);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step2: 设置为输出模式
    uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    printf("gpio set_dir done\n");
    // step3: 输出电平
    ret = uapi_gpio_set_val(pin, level);
    if (ret != ERRCODE_SUCC) {
        printf("gpio operate failed , ret = 0x%x\n", ret);
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    gpio_level_t read_level;
    // step4: 读取电平
    read_level = uapi_gpio_get_val(pin);
    // step5: 判断输出是否成功
    if (read_level == (gpio_level_t)level) {
        printf("gpio output success;read level is %d\n", read_level);
    } else {
        printf("gpio output failed;read level is %d\n", read_level);
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    printf("gpio output done\n");
    // step6: 去初始化
    gpio_common_deinit();

    return AT_RET_OK;
}

// gpio输入
at_ret_t at_gpio_input(gpio_intput_args_t *args)
{
    printf("========== gpio input sample test start ==========\n");
    uint32_t pin = (uint32_t)args->pin;
    printf("[params] pin:%d.\n", pin);
    // step1: pin和gpio初始化,设置为gpio模式
    errcode_t ret = gpio_common_init(pin);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step2: 设置pin脚的输入使能
    uapi_pin_set_ie(pin, PIN_IE_ENABLE);
    printf("pin set ie enable\n");
    gpio_level_t gpio_level;
    // step3: 读取电平
    gpio_level = uapi_gpio_get_val(pin);
    // step4: 判断输入的是高电平/低电平
    if (gpio_level == GPIO_LEVEL_HIGH) {
        printf("gpio input level is high; level:1\n");
    } else {
        printf("gpio input level is low; level:0\n");
    }
    // step5: 关闭输入使能
    uapi_pin_set_ie(pin, PIN_IE_DISABLE);
    printf("pin set ie disable\n");
    printf("gpio input done\n");
    // step6: 去初始化
    gpio_common_deinit();

    return AT_RET_OK;
}

// gpio设置方向
at_ret_t at_gpio_setdir(gpio_setdir_args_t *args)
{
    printf("========== gpio setdir sample test start ==========\n");
    uint32_t pin = (uint32_t)args->pin;
    uint32_t dir = (uint32_t)args->dir;
    printf("[params] pin:%d, dir: %d.\n", (uint32_t)pin, (uint32_t)dir);
    // step1: pin和gpio初始化,设置为gpio模式
    errcode_t ret = gpio_common_init(pin);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step2: 设置方向
    if (dir == 1) {
        ret = uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
        printf("gpio set output\n");
    } else if (dir == 0) {
        ret = uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT);
        printf("gpio set input\n");
    } else {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    if (ret != ERRCODE_SUCC) {
        printf("gpio operate failed , ret = 0x%x\n", ret);
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    printf("gpio setdir done\n");
    // step6: 去初始化
    gpio_common_deinit();

    return AT_RET_OK;
}

// gpio电平反转
at_ret_t at_gpio_toggle(gpio_args_t *args)
{
    printf("========== gpio toggle sample test start ==========\n");
    uint32_t pin = (uint32_t)args->pin;
    uint32_t level = (uint32_t)args->level;
    printf("[params] pin:%d, level: %d.\n", (uint32_t)pin, (uint32_t)level);
    // step1: pin和gpio初始化,设置为gpio模式
    errcode_t ret = gpio_common_init(pin);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step2: 设置为输出模式
    ret = uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step3: 设置指定GPIO电平状态
    if (level == 1) {
        uapi_gpio_set_val(pin, GPIO_LEVEL_HIGH);
        printf("gpio set level_high\n");
    } else if (level == 0) {
        uapi_gpio_set_val(pin, GPIO_LEVEL_LOW);
        printf("gpio set level_low\n");
    } else {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step4: 电平反转
    ret = uapi_gpio_toggle(pin);
    if (ret != ERRCODE_SUCC) {
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    } else {
        printf("gpio toggle done\n");
    }
    gpio_level_t gpio_level;
    // step5: 读取电平
    gpio_level = uapi_gpio_get_val(pin);
    // step6: 判断反转是否成功
    if (gpio_level != (gpio_level_t)level) {
        printf("gpio toggle success;read level is %d\n", gpio_level);
    } else {
        printf("gpio toggle failed;read level is %d\n", gpio_level);
        gpio_common_deinit();
        return AT_RET_RUN_ERROR;
    }
    // step6: 去初始化
    gpio_common_deinit();

    return AT_RET_OK;
}