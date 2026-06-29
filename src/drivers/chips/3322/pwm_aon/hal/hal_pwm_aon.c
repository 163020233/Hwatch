/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL pwm_aon \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "common_def.h"
#include "hal_pwm_aon.h"

uintptr_t g_pwm_aon_regs = (uintptr_t)NULL;

errcode_t hal_pwm_aon_regs_init(void)
{
    if (pwm_aon_porting_base_addr_get() == 0) {
        return ERRCODE_PWM_REG_ADDR_INVALID;
    }

    g_pwm_aon_regs = pwm_aon_porting_base_addr_get();

    return ERRCODE_SUCC;
}

void hal_pwm_aon_regs_deinit(void)
{
    g_pwm_aon_regs = (uintptr_t)NULL;
}