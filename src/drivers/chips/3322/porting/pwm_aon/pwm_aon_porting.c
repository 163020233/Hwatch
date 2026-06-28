/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides pwm port \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#include "pwm_aon_porting.h"
#include "soc_osal.h"
#include "common_def.h"
#include "hal_pwm_aon_v151.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "pm_clocks.h"

#define BUS_CLOCK_TIME_32M      32000000UL
static const uintptr_t g_pwm_aon_base_addr =  (uintptr_t)AON_PWM_BASE_ADDR;

uintptr_t pwm_aon_porting_base_addr_get(void)
{
    return g_pwm_aon_base_addr;
}

static int pwm_aon_handler(int a, const void *tmp)
{
    unused(a);
    unused(tmp);
    hal_pwm_aon_v151_irq_handler();
    return 0;
}

void pwm_aon_port_register_irq(pwm_aon_channel_t channel)
{
    unused(channel);
    osal_irq_request((uintptr_t)AON_PWM_ABNOR_INT, (osal_irq_handler)pwm_aon_handler, NULL, NULL, NULL);
    osal_irq_request((uintptr_t)AON_PWM_CFG_INT, (osal_irq_handler)pwm_aon_handler, NULL, NULL, NULL);
    osal_irq_enable((uintptr_t)AON_PWM_ABNOR_INT);
    osal_irq_enable((uintptr_t)AON_PWM_CFG_INT);
}

void pwm_aon_port_unregister_irq(pwm_aon_channel_t channel)
{
    unused(channel);
    osal_irq_disable((uintptr_t)AON_PWM_ABNOR_INT);
    osal_irq_disable((uintptr_t)AON_PWM_CFG_INT);
    osal_irq_free((uintptr_t)AON_PWM_ABNOR_INT, NULL);
    osal_irq_free((uintptr_t)AON_PWM_CFG_INT, NULL);
}

void pwm_aon_irq_lock(uint8_t channel)
{
    unused(channel);
    osal_irq_lock();
}

void pwm_aon_irq_unlock(uint8_t channel)
{
    unused(channel);
    osal_irq_unlock();
}

void pwm_aon_port_clock_enable(bool on)
{
    if (on) {
        reg16_setbit(GLB_CTL_M_RB_PWM_CTRL_REG, GLB_CTL_M_RB_CFG_AON_PWM_CLKEN_OFFSET);
    } else {
        reg16_clrbit(GLB_CTL_M_RB_PWM_CTRL_REG, GLB_CTL_M_RB_CFG_AON_PWM_CLKEN_OFFSET);
    }
}

uint32_t pwm_aon_port_get_clock_value(pwm_aon_channel_t channel)
{
    if (channel >= PWM_MAX_NUMBER) {
        return 0;
    }
    return BUS_CLOCK_TIME_32M;
}

errcode_t pwm_aon_port_param_check(const pwm_aon_config_t *cfg)
{
    if (0xffffffff - cfg->high_time < cfg->low_time ||
        cfg->high_time + cfg->low_time < cfg->offset_time) {
        return ERRCODE_PWM_INVALID_PARAMETER;
    }
    return ERRCODE_SUCC;
}
