/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "pwm_porting.h"
#include "soc_osal.h"
#include "common_def.h"
#include "hal_pwm_v151.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "pm_clocks.h"

#define BUS_CLOCK_TIME_32M      32000000UL
static const uintptr_t g_pwm_base_addr =  (uintptr_t)PWM_BASE_ADDR;

uintptr_t pwm_porting_base_addr_get(void)
{
    return g_pwm_base_addr;
}

static int pwm_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);
    hal_pwm_v151_irq_handler();
    return 0;
}

void pwm_port_register_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_request((uintptr_t)PWM_ABNOR_IRQN, (osal_irq_handler)pwm_handler, NULL, NULL, NULL);
    osal_irq_request((uintptr_t)PWM_CFG_IRQN, (osal_irq_handler)pwm_handler, NULL, NULL, NULL);
    osal_irq_enable((uintptr_t)PWM_ABNOR_IRQN);
    osal_irq_enable((uintptr_t)PWM_CFG_IRQN);
}

void pwm_port_unregister_irq(pwm_channel_t channel)
{
    unused(channel);
    osal_irq_disable((uintptr_t)PWM_ABNOR_IRQN);
    osal_irq_disable((uintptr_t)PWM_CFG_IRQN);
    osal_irq_free((uintptr_t)PWM_ABNOR_IRQN, NULL);
    osal_irq_free((uintptr_t)PWM_CFG_IRQN, NULL);
}

void pwm_irq_lock(uint8_t channel)
{
    unused(channel);
    osal_irq_lock();
}

void pwm_irq_unlock(uint8_t channel)
{
    unused(channel);
    osal_irq_unlock();
}

void pwm_port_clock_enable(bool on)
{
    if (on) {
        hal_clocks_app_perips_config(HAL_CLOCKS_APP_PWM_CLK, TURN_ON);
    } else {
        hal_clocks_app_perips_config(HAL_CLOCKS_APP_PWM_CLK, TURN_OFF);
    }
}

uint32_t pwm_port_get_clock_value(pwm_channel_t channel)
{
    if (channel >= PWM_MAX_NUMBER) {
        return 0;
    }
    return BUS_CLOCK_TIME_32M;
}

errcode_t pwm_port_param_check(const pwm_config_t *cfg)
{
    if (0xffffffff - cfg->high_time < cfg->low_time ||
        cfg->high_time + cfg->low_time < cfg->offset_time) {
        return ERRCODE_PWM_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}
