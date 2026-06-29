/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026. All rights reserved.
 *
 * Description: PWM sample implementation \n
 *
 */

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include "uart.h"
#include "pwm.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"
#include "errcode.h"
#include "debug_print.h"
#include "driver/pinctrl.h"
#include "soc_osal.h"
#include "app_init.h"
#include "common_def.h"
#include "tcxo.h"
#include "pwm_demo.h"

void at_diting_pwm_example_cmd_register(void)
{
    uapi_at_cmd_table_register(g_at_pwm_cmd_parse_table, AT_PWM_FUNC_NUM, EXT_AT_PWM_CMD_MAX_LEN);
}

static errcode_t pwm_module_init(void)
{
    // step1: 配置PWM引脚模式
    errcode_t ret = uapi_pin_set_mode(CONFIG_PWM_PIN, CONFIG_PWM_PIN_MODE);
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm set mode failed\n");
    }
    // step2: 初始化PWM模块
    ret = uapi_pwm_init();
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm init failed, ret=%d\n", ret);
        return ret;
    }
    printf("  - pwm module initialized\n");

    // step3: 配置PWM通道初始参数
    pwm_config_t cfg_no_repeat = {
        .low_time = PWM_DUTY_20PCT,           // 20%占空比
        .high_time = PWM_DUTY_80PCT,          // 80%占空比
        .offset_time = PWM_OFFSET_0,          // 相位偏移为0
        .cycles = PWM_CYCLES_CONTINUOUS,      // 连续输出
        .repeat = false                       // 非连续模式
    };
    
    // step4: 打开PWM通道
    ret = uapi_pwm_open(CONFIG_PWM_CHANNEL, &cfg_no_repeat);
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm open channel %d failed, ret=%d\n", CONFIG_PWM_CHANNEL, ret);
        return ret;
    }
    printf("  - Channel %d opened\n", CONFIG_PWM_CHANNEL);

    // step5: 将通道添加到PWM组中
    uint8_t channel_id = CONFIG_PWM_CHANNEL;
    ret = uapi_pwm_set_group(CONFIG_PWM_GROUP_ID, &channel_id, 1);
    if (ret != ERRCODE_SUCC) {
        printf("error: PWM set group failed, ret=%d\n", ret);
        return ret;
    }
    printf("  - group %d configured with channel %d\n", CONFIG_PWM_GROUP_ID, channel_id);
    
    return ERRCODE_SUCC;
}

static errcode_t pwm_single_output(uint32_t cycle_index)
{
    errcode_t ret;
    printf("========== PWM Cycle %u Start ==========\n", cycle_index + 1);
    
    // step1: 配置新的PWM参数
    pwm_config_t pwm_cfg = {
        .low_time = (cycle_index % 2 == 0) ? PWM_DUTY_20PCT : PWM_DUTY_80PCT,
        .high_time = (cycle_index % 2 == 0) ? PWM_DUTY_80PCT : PWM_DUTY_20PCT,
        .offset_time = PWM_OFFSET_0,
        .cycles = PWM_CYCLES_CONTINUOUS,
        .repeat = true
    };
    
    printf("Expected: high_time=%u, low_time=%u\n", pwm_cfg.high_time, pwm_cfg.low_time);
    printf("Expected duty cycle: %u%%\n",
        (cycle_index % PWM_CYCLE_MODULO == 0) ? PWM_DUTY_CYCLE_HIGH : PWM_DUTY_CYCLE_LOW);

    // step2：预加载新的PWM配置
    ret = uapi_pwm_config_preload(CONFIG_PWM_GROUP_ID, CONFIG_PWM_CHANNEL, &pwm_cfg);
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm config preload failed, ret=%d\n", ret);
        return ret;
    }

    // step3: 启动PWM组输出
    ret = uapi_pwm_start_group(CONFIG_PWM_GROUP_ID);
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm start group failed, ret=%d\n", ret);
        return ret;
    }
    printf("  - pwm started\n");

    // 输出一段时间（例如500ms）
    uapi_tcxo_delay_ms(500);
    
    // step4: 停止PWM组输出
    ret = uapi_pwm_stop_group(CONFIG_PWM_GROUP_ID);
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm stop group failed, ret=%d\n", ret);
        return ret;
    }
    printf("  - pwm stopped\n");

    printf("pwm output %u completed, duty: %u/%u\n",
        cycle_index + 1,
        pwm_cfg.high_time,
        pwm_cfg.high_time + pwm_cfg.low_time);
    printf("========== PWM Cycle %u End ==========\n", cycle_index + 1);

    return ERRCODE_SUCC;
}


static void pwm_module_deinit(void)
{
    uapi_pwm_close(CONFIG_PWM_CHANNEL);
    uapi_pwm_deinit();
    printf("pwm resources cleaned up\n");
}

at_ret_t at_pwm_sample(void)
{
    errcode_t ret;
    uint32_t i;
    uint32_t success_count = 0;
    uint32_t fail_count = 0;
    printf("==========  pwm sample test start  ==========\n");
    
    // step1: 初始化PWM模块
    ret = pwm_module_init();
    if (ret != ERRCODE_SUCC) {
        printf("error: pwm module init failed,ret=%d\n", ret);
        return AT_RET_RUN_ERROR;
    }
    printf("PWM module init SUCCESS\n");
    
    // step2: 循环5次，每次改变占空比
    for (i = 0; i < PWM_CFG_CHANGE_COUNT; i++) {
        ret = pwm_single_output(i);
        if (ret == ERRCODE_SUCC) {
            success_count++;
            printf("Cycle %u: SUCCESS\n", i + 1);
        } else {
            fail_count++;
            printf("Cycle %u: FAILED (ret=%d)\n", i + 1, ret);
        }
    }
    
    // step3: 清理资源
    pwm_module_deinit();
    
    printf("========== PWM Sample Test End ==========\n");
    printf("Total: %u, Success: %u, Failed: %u\n", PWM_CFG_CHANGE_COUNT, success_count, fail_count);
    
    if (fail_count > 0) {
        return AT_RET_RUN_ERROR;
    }
    
    return AT_RET_OK;
}

