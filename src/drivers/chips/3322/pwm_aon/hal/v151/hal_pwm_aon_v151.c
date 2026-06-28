/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL pwm \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#include "common_def.h"
#include "osal_debug.h"
#include "hal_pwm_aon.h"
#include "tcxo.h"
#include "hal_pwm_aon_v151.h"

#define PWM_AON_INT_MASK_BIT 3
#define PWM_AON_LEN_HIGH_16BIT_RIGHT_SHIFT 16

#define PWM_AON_START_INTERVAL_US 1
#define PWM_AON_WAIT_EN_US 0

#define PWM_AON_CLK_32K_START_INTERVAL_US 200
#define PWM_AON_CLK_32K_WAIT_EN_US 50

#define PWM_AON_CLK_32K 32768

static hal_pwm_aon_callback_t g_hal_pwm_aon_v151_callback[CONFIG_PWM_AON_CHANNEL_NUM] = { NULL };
static uint32_t g_freq_time = 0;
static bool g_repeat_flag = 0;

#if !defined(PWM_AON_SUPPORT_CONTINUOUS_MODE)
static uint32_t hal_pwm_aon_get_start_interval_us(void)
{
    uint32_t clk = pwm_aon_port_get_clock_value(PWM_AON_0);
    return clk == PWM_AON_CLK_32K ? PWM_AON_CLK_32K_START_INTERVAL_US : PWM_AON_START_INTERVAL_US;
}
#endif

static uint32_t hal_pwm_aon_get_wait_en_us(void)
{
    uint32_t clk = pwm_aon_port_get_clock_value(PWM_AON_0);
    return clk == PWM_AON_CLK_32K ? PWM_AON_CLK_32K_WAIT_EN_US : PWM_AON_WAIT_EN_US;
}

#pragma weak hal_pwm_aon_init = hal_pwm_aon_v151_init
errcode_t hal_pwm_aon_v151_init(void)
{
    if (hal_pwm_aon_regs_init() != ERRCODE_SUCC) {
        return ERRCODE_PWM_REG_ADDR_INVALID;
    }

    /* Remove interrupt mask */
    hal_pwm_aon_int_mask_set_pwm_aon_int_mask(PWM_AON_INT_MASK_BIT);
    return ERRCODE_SUCC;
}

#pragma weak hal_pwm_aon_deinit = hal_pwm_aon_v151_deinit
void hal_pwm_aon_v151_deinit(void)
{
    hal_pwm_aon_regs_deinit();
}

static void hal_pwm_aon_v151_set_low_time(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time)
{
    unused(id);
    unused(channel);
    g_freq_time = 0;
    g_freq_time += time;
}

static void hal_pwm_aon_v151_set_high_time(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time)
{
    unused(id);
    g_freq_time += time;
    hal_pwm_aon_duty_l_set_pwm_aon_duty_l_j(channel, time);
#ifdef CONFIG_PWM_AON_SUPPORT_TIME_32BIT
    hal_pwm_aon_duty_h_set_pwm_aon_duty_h_j(channel, time >> PWM_AON_LEN_HIGH_16BIT_RIGHT_SHIFT);
#endif /* CONFIG_PWM_AON_SUPPORT_TIME_32BIT */
}

static void hal_pwm_aon_v151_set_offset_time(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time)
{
    unused(id);

    /* set offset time */
    hal_pwm_aon_offset_l_set_pwm_aon_offset_l_j(channel, time);

    /* set freq time */
    hal_pwm_aon_freq_l_set_pwm_aon_freq_l_j(channel, g_freq_time);
#ifdef CONFIG_PWM_AON_SUPPORT_TIME_32BIT
    hal_pwm_aon_offset_h_set_pwm_aon_offset_h_j(channel, time >> PWM_AON_LEN_HIGH_16BIT_RIGHT_SHIFT);
    hal_pwm_aon_freq_h_set_pwm_aon_freq_h_j(channel, g_freq_time >> PWM_AON_LEN_HIGH_16BIT_RIGHT_SHIFT);
#endif /* CONFIG_PWM_AON_SUPPORT_TIME_32BIT */
    g_freq_time = 0;
}

static hal_pwm_aon_set_time_t g_hal_pwm_aon_set_func_array[PWM_AON_SET_TIEM_MAX] = {
    hal_pwm_aon_v151_set_low_time,
    hal_pwm_aon_v151_set_high_time,
    hal_pwm_aon_v151_set_offset_time,
};

#pragma weak hal_pwm_aon_set_time = hal_pwm_aon_v151_set_time
void hal_pwm_aon_v151_set_time(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time)
{
    g_hal_pwm_aon_set_func_array[id](id, channel, time);
}

#pragma weak hal_pwm_aon_set_group = hal_pwm_aon_v151_set_group
void hal_pwm_aon_v151_set_group(pwm_aon_v151_group_t group, uint16_t channel_id)
{
    /* Each bit in channel_id corresponds to one pwm channel */
    hal_pwm_aon_sel_set_pwm_aon_sel_i(group, channel_id);
}

static void hal_pwm_aon_v151_set_group_en(pwm_aon_v151_group_t group, bool en)
{
    uint16_t channel_id = hal_pwm_aon_sel_get_pwm_aon_sel_i(group);
    for (uint8_t i = 0; i < CONFIG_PWM_AON_CHANNEL_NUM; i++) {
        if ((channel_id & (1 << i)) != 0) {
            hal_pwm_aon_en_set_pwm_aon_en_j(i, en);
            uapi_tcxo_delay_us(hal_pwm_aon_get_wait_en_us());
        }
    }
#ifdef CONFIG_PWM_AON_IP_VERSION_V151_PRO
    hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(group, 0);
#else
    hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(group, 1);
#endif
    hal_pwm_aon_start_set_pwm_aon_start_i(group, 1);
#ifdef PWM_AON_STOP_WITH_CLK_EN_ASYN
    for (uint8_t i = 0; i < CONFIG_PWM_AON_CHANNEL_NUM; i++) {
        if ((channel_id & (1 << i)) != 0) {
            hal_pwm_aon_en_set_pwm_aon_clk_en(i, en);
        }
    }
#endif
#if !defined(PWM_AON_SUPPORT_CONTINUOUS_MODE)
#if !defined(CONFIG_PWM_AON_PRELOAD)
    if (g_repeat_flag == false) {
        for (uint8_t i = 0; i < CONFIG_PWM_AON_CHANNEL_NUM; i++) {
            if ((channel_id & (1 << i)) > 0 && hal_pwm_aon_period_val_get_pwm_aon_period_val_j(i) > 0) {
                hal_pwm_aon_en_set_pwm_aon_en_j(i, 0);
#ifdef PWM_AON_STOP_WITH_CLK_EN_ASYN
                hal_pwm_aon_en_set_pwm_aon_clk_en(i, en);
#endif
            }
        }
        uapi_tcxo_delay_us(hal_pwm_aon_get_start_interval_us());
        hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(group, 0);
        hal_pwm_aon_start_set_pwm_aon_start_i(group, 1);
    }
#else
    uapi_tcxo_delay_us(hal_pwm_aon_get_start_interval_us());
    hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(group, 0);
#endif /* CONFIG_PWM_AON_PRELOAD */
#endif
}

#if defined(CONFIG_PWM_AON_PRELOAD)
void hal_pwm_aon_config_preload(uint8_t group)
{
    hal_pwm_aon_start_set_pwm_aon_start_i(group, 1);
}
#endif /* CONFIG_PWM_AON_PRELOAD */

static void hal_pwm_aon_v151_start(pwm_aon_v151_group_t group)
{
    hal_pwm_aon_v151_set_group_en(group, true);
}

static void hal_pwm_aon_v151_stop(pwm_aon_v151_group_t group)
{
    hal_pwm_aon_v151_set_group_en(group, false);
}

static void hal_pwm_aon_v151_refresh(pwm_aon_v151_group_t group)
{
    hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(group, 1);
    hal_pwm_aon_start_set_pwm_aon_start_i(group, 1);
}

#pragma weak hal_pwm_aon_set_action = hal_pwm_aon_v151_set_action
void hal_pwm_aon_v151_set_action(uint8_t channel, pwm_aon_action_t action)
{
    if (action == PWM_AON_ACTION_START) {
        hal_pwm_aon_v151_start((pwm_aon_v151_group_t)channel);
    } else if (action == PWM_AON_ACTION_STOP) {
        hal_pwm_aon_v151_stop((pwm_aon_v151_group_t)channel);
    } else if (action == PWM_AON_ACTION_CONTINUE_CLR) {
        g_repeat_flag = false;
        hal_pwm_aon_period_val_set_pwm_aon_mode_j(channel, 1);
    } else if (action == PWM_AON_ACTION_CONTINUE_SET) {
        g_repeat_flag = true;
        hal_pwm_aon_period_val_set_pwm_aon_mode_j(channel, 0);
    } else if (action == PWM_AON_ACTION_REFRESH) {
        hal_pwm_aon_v151_refresh(channel);
    }
}

#pragma weak hal_pwm_aon_set_cycles = hal_pwm_aon_v151_set_cycles
void hal_pwm_aon_v151_set_cycles(pwm_aon_channel_t channel, uint16_t cycles)
{
    hal_pwm_aon_period_val_set_pwm_aon_period_val_j(channel, cycles);
}

#pragma weak hal_pwm_aon_intr_clear = hal_pwm_aon_v151_intr_clear
void hal_pwm_aon_v151_intr_clear(pwm_aon_channel_t channel)
{
    uint32_t abnor_state0 = hal_pwm_aon_abnor_state0_get_pwm_aon_abnor_state0();
    uint32_t abnor_state1 = hal_pwm_aon_abnor_state1_get_pwm_aon_abnor_state1();
#ifdef CONFIG_PWM_AON_PERIODLOAD_FLAG_BIT
    uint32_t flag = (hal_pwm_aon_get_periodload_flag() >> channel) & 1;
#else
    uint32_t flag = hal_pwm_aon_periodload_flag_get_pwm_aon_periodload_flag_j(channel);
#endif

    hal_pwm_aon_abnor_state_clr0_set_pwm_aon_abnor_state_clr0(abnor_state0);
    hal_pwm_aon_abnor_state_clr1_set_pwm_aon_abnor_state_clr1(abnor_state1);

    if (flag != 0) {
        uint32_t cfg_int_clr0 = (bit((uint32_t)channel));
        hal_pwm_aon_cfg_int_clr0_set_pwm_aon_cfg_int_clr0(cfg_int_clr0);
    }
}

#pragma weak hal_pwm_aon_register_callback = hal_pwm_aon_v151_register_callback
void hal_pwm_aon_v151_register_callback(pwm_aon_channel_t channel, hal_pwm_aon_callback_t callback)
{
    g_hal_pwm_aon_v151_callback[channel] = callback;
}

#pragma weak hal_pwm_aon_unregister_callback = hal_pwm_aon_v151_unregister_callback
void hal_pwm_aon_v151_unregister_callback(pwm_aon_channel_t channel)
{
    g_hal_pwm_aon_v151_callback[channel] = NULL;
}

void hal_pwm_aon_v151_irq_handler(void)
{
    uint32_t abnor_state0 = hal_pwm_aon_abnor_state0_get_pwm_aon_abnor_state0();
    uint32_t abnor_state1 = hal_pwm_aon_abnor_state1_get_pwm_aon_abnor_state1();
#ifdef CONFIG_PWM_AON_PERIODLOAD_FLAG_BIT
    uint32_t periodload_flags = hal_pwm_aon_get_periodload_flag();
#endif
    bool int_flag = false;
    for (uint8_t ch = 0; ch < CONFIG_PWM_AON_CHANNEL_NUM; ch++) {
        int_flag = false;
        if ((((abnor_state0 >> ch) & 1) == 1)) {
            osal_printk("pwm %d group cfg err\r\n", ch);
            int_flag = true;
        }
        if ((((abnor_state1 >> ch) & 1) == 1)) {
            osal_printk("pwm %d time cfg err\r\n", ch);
            int_flag = true;
        }
#ifdef CONFIG_PWM_AON_PERIODLOAD_FLAG_BIT
        bool periodload_flag = (periodload_flags >> ch) & 1;
#else
        bool periodload_flag = hal_pwm_aon_periodload_flag_get_pwm_aon_periodload_flag_j(ch);
#endif
        if (periodload_flag) {
            if (g_hal_pwm_aon_v151_callback[(pwm_aon_channel_t)ch] != NULL) {
                g_hal_pwm_aon_v151_callback[(pwm_aon_channel_t)ch]((pwm_aon_channel_t)ch);
            }
            int_flag = true;
        }
        if (int_flag) {
            hal_pwm_aon_v151_intr_clear((pwm_aon_channel_t)ch);
        }
    }
}
