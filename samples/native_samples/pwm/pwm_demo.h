/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: PWM sample header \n
 *
 */

#ifndef PWM_DEMO_H
#define PWM_DEMO_H
#include <stdint.h>
#include "at.h"

#define EXT_AT_PWM_CMD_MAX_LEN   128

#define AT_PWM_FUNC_NUM (sizeof(g_at_pwm_cmd_parse_table) / sizeof(g_at_pwm_cmd_parse_table[0]))

#define PWM_FREQ_1KHZ           32000      // 1KHz对应的时钟周期数
#define PWM_DUTY_80PCT          25600      // 80%占空比
#define PWM_DUTY_20PCT          6400       // 20%占空比
#define PWM_OFFSET_0            0          // 相位偏移
#define PWM_CYCLES_CONTINUOUS   0          // 0表示连续输出
#define PWM_CFG_CHANGE_COUNT    5          // 占空比变化次数
#define CONFIG_PWM_PIN_MODE     1          // PWM功能对应的引脚复用模式
#define CONFIG_PWM_CHANNEL      0          // PWM通道号
#define CONFIG_PWM_GROUP_ID     0          // PWM组ID
#define CONFIG_PWM_PIN          34         // PWM输出引脚
#define PWM_CYCLE_MODULO        2
#define PWM_DUTY_CYCLE_HIGH     80
#define PWM_DUTY_CYCLE_LOW      20
void at_diting_pwm_example_cmd_register(void);
at_ret_t at_pwm_sample(void);

static const at_cmd_entry_t g_at_pwm_cmd_parse_table[] = {
    // AT+PWMSAMPLE=0
    {
        "PWMSAMPLE",
        0x2264,
        0,
        NULL,
        NULL,
        (at_set_func_t)at_pwm_sample,
        NULL,
        NULL,
    },
};

#endif