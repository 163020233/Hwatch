/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 PWM register operation api \n
 * Author: HiSilicon (Shanghai) Technologies Co., Ltd. \n
 * History: \n
 * 2022-12-9， Create file. \n
 */

#ifndef HAL_PWM_AON_V151_REGS_OP_H
#define HAL_PWM_AON_V151_REGS_OP_H

#include <stdint.h>
#include "errcode.h"
#include "hal_pwm_aon_v151_regs_def.h"
#include "pwm_aon_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pwm_aon_v151_regs_op PWM V151 Regs Operation
 * @ingroup  drivers_hal_pwm
 * @{
 */

extern uintptr_t g_pwm_aon_regs;

/**
 * @brief  Set the value of @ref pwm_aon_sel_data_t.pwm_aon_sel_i.
 * @param  [in]  group The value of @ref pwm_aon_v151_regs_t.pwm_aon_v151_sel.
 * @param  [in]  val The value of @ref pwm_aon_sel_data_t.pwm_aon_sel_i.
 */
static inline void hal_pwm_aon_sel_set_pwm_aon_sel_i(pwm_aon_v151_group_t group, uint16_t val)
{
    pwm_aon_sel_data_t sel;
    sel.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_sel;
    sel.b.pwm_aon_sel_i = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_sel = sel.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_sel_data_t.pwm_aon_sel_i.
 * @param  [in]  group The value of @ref pwm_aon_v151_regs_t.pwm_aon_v151_sel.
 * @return The value of @ref pwm_aon_sel_data_t.pwm_aon_sel_i.
 */
static inline uint16_t hal_pwm_aon_sel_get_pwm_aon_sel_i(pwm_aon_v151_group_t group)
{
    pwm_aon_sel_data_t sel;
    sel.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_sel;
    return sel.b.pwm_aon_sel_i;
}

/**
 * @brief  Set the value of @ref pwm_aon_startclrcnt_en_data_t.pwm_aon_startclrcnt_en_i.
 * @param  [in]  group The value of @ref pwm_aon_v151_regs_t.pwm_aon_v151_sel.
 * @param  [in]  val The value of @ref pwm_aon_startclrcnt_en_data_t.pwm_aon_startclrcnt_en_i.
 */
static inline void hal_pwm_aon_startclrcnt_en_set_pwm_aon_startclrcnt_en_i(pwm_aon_v151_group_t group, uint32_t val)
{
    pwm_aon_startclrcnt_en_data_t startclrcnt_en;
    startclrcnt_en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_startclrcnt_en;
    startclrcnt_en.b.pwm_aon_startclrcnt_en_i = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_startclrcnt_en = startclrcnt_en.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_startclrcnt_en_data_t.pwm_aon_startclrcnt_en_i.
 * @param  [in]  group The value of @ref pwm_aon_v151_regs_t.pwm_aon_v151_sel.
 * @return The value of @ref pwm_aon_startclrcnt_en_data_t.pwm_aon_startclrcnt_en_i.
 */
static inline uint32_t hal_pwm_aon_startclrcnt_en_get_pwm_aon_startclrcnt_en_i(pwm_aon_v151_group_t group)
{
    pwm_aon_startclrcnt_en_data_t startclrcnt_en;
    startclrcnt_en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_startclrcnt_en;
    return startclrcnt_en.b.pwm_aon_startclrcnt_en_i;
}

/**
 * @brief  Set the value of @ref pwm_aon_start_data_t.pwm_aon_start_i.
 * @param  [in]  group The value of @ref pwm_aon_v151_regs_t.pwm_aon_v151_sel.
 * @param  [in]  val The value of @ref pwm_aon_start_data_t.pwm_aon_start_i.
 */
static inline void hal_pwm_aon_start_set_pwm_aon_start_i(pwm_aon_v151_group_t group, uint32_t val)
{
    pwm_aon_start_data_t start;
    start.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_start;
    start.b.pwm_aon_start_i = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_v151_sel[group].pwm_aon_start = start.d32;
}

/**
 * @brief  Set the value of @ref pwm_aon_en_data_t.pwm_aon_clk_en.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_en_data_t.pwm_aon_clk_en.
 */
static inline void hal_pwm_aon_en_set_pwm_aon_clk_en(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_en_data_t en;
    en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_en;
    en.b.pwm_aon_clk_en = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_en = en.d32;
}

/**
 * @brief  Set the value of @ref pwm_aon_en_data_t.pwm_aon_en_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_en_data_t.pwm_aon_en_j.
 */
static inline void hal_pwm_aon_en_set_pwm_aon_en_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_en_data_t en;
    en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_en;
#ifdef CONFIG_PWM_AON_SUPPORT_CLOCK_ENABLE
    en.b.pwm_aon_clk_en = val;
#endif
    en.b.pwm_aon_en_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_en = en.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_en_data_t.pwm_aon_en_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_en_data_t.pwm_aon_en_j.
 */
static inline uint32_t hal_pwm_aon_en_get_pwm_aon_en_j(pwm_aon_channel_t channel)
{
    pwm_aon_en_data_t en;
    en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_en;
    return en.b.pwm_aon_en_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_portity_data_t.pwm_aon_portity_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_portity_data_t.pwm_aon_portity_j.
 */
static inline void hal_pwm_aon_portity_set_pwm_aon_portity_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_portity_data_t portity;
    portity.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_portity;
    portity.b.pwm_aon_portity_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_portity = portity.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_portity_data_t.pwm_aon_portity_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_portity_data_t.pwm_aon_portity_j.
 */
static inline uint32_t hal_pwm_aon_portity_get_pwm_aon_portity_j(pwm_aon_channel_t channel)
{
    pwm_aon_portity_data_t portity;
    portity.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_portity;
    return portity.b.pwm_aon_portity_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_oen_cfg_data_t.pwm_aon_oen_cfg_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_oen_cfg_data_t.pwm_aon_oen_cfg_j.
 */
static inline void hal_pwm_aon_oen_cfg_set_pwm_aon_oen_cfg_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_oen_cfg_data_t oen_cfg;
    oen_cfg.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_oen_cfg;
    oen_cfg.b.pwm_aon_oen_cfg_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_oen_cfg = oen_cfg.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_oen_cfg_data_t.pwm_aon_oen_cfg_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_oen_cfg_data_t.pwm_aon_oen_cfg_j.
 */
static inline uint32_t hal_pwm_aon_oen_cfg_get_pwm_aon_oen_cfg_j(pwm_aon_channel_t channel)
{
    pwm_aon_oen_cfg_data_t oen_cfg;
    oen_cfg.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_oen_cfg;
    return oen_cfg.b.pwm_aon_oen_cfg_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_offset_l_data_t.pwm_aon_offset_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_offset_l_data_t.pwm_aon_offset_l_j.
 */
static inline void hal_pwm_aon_offset_l_set_pwm_aon_offset_l_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_offset_l_data_t offset_l;
    offset_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_l;
    offset_l.b.pwm_aon_offset_l_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_l = offset_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_offset_l_data_t.pwm_aon_offset_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_offset_l_data_t.pwm_aon_offset_l_j.
 */
static inline uint32_t hal_pwm_aon_offset_l_get_pwm_aon_offset_l_j(pwm_aon_channel_t channel)
{
    pwm_aon_offset_l_data_t offset_l;
    offset_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_l;
    return offset_l.b.pwm_aon_offset_l_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_offset_h_data_t.pwm_aon_offset_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_offset_h_data_t.pwm_aon_offset_h_j.
 */
static inline void hal_pwm_aon_offset_h_set_pwm_aon_offset_h_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_offset_h_data_t offset_h;
    offset_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_h;
    offset_h.b.pwm_aon_offset_h_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_h = offset_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_offset_h_data_t.pwm_aon_offset_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_offset_h_data_t.pwm_aon_offset_h_j.
 */
static inline uint32_t hal_pwm_aon_offset_h_get_pwm_aon_offset_h_j(pwm_aon_channel_t channel)
{
    pwm_aon_offset_h_data_t offset_h;
    offset_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_offset_h;
    return offset_h.b.pwm_aon_offset_h_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_freq_l_data_t.pwm_aon_freq_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_freq_l_data_t.pwm_aon_freq_l_j.
 */
static inline void hal_pwm_aon_freq_l_set_pwm_aon_freq_l_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_freq_l_data_t freq_l;
    freq_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_l;
    freq_l.b.pwm_aon_freq_l_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_l = freq_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_freq_l_data_t.pwm_aon_freq_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_freq_l_data_t.pwm_aon_freq_l_j.
 */
static inline uint32_t hal_pwm_aon_freq_l_get_pwm_aon_freq_l_j(pwm_aon_channel_t channel)
{
    pwm_aon_freq_l_data_t freq_l;
    freq_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_l;
    return freq_l.b.pwm_aon_freq_l_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_freq_h_data_t.pwm_aon_freq_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_freq_h_data_t.pwm_aon_freq_h_j.
 */
static inline void hal_pwm_aon_freq_h_set_pwm_aon_freq_h_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_freq_h_data_t freq_h;
    freq_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_h;
    freq_h.b.pwm_aon_freq_h_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_h = freq_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_freq_h_data_t.pwm_aon_freq_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_freq_h_data_t.pwm_aon_freq_h_j.
 */
static inline uint32_t hal_pwm_aon_freq_h_get_pwm_aon_freq_h_j(pwm_aon_channel_t channel)
{
    pwm_aon_freq_h_data_t freq_h;
    freq_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_freq_h;
    return freq_h.b.pwm_aon_freq_h_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_duty_l_data_t.pwm_aon_duty_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_duty_l_data_t.pwm_aon_duty_l_j.
 */
static inline void hal_pwm_aon_duty_l_set_pwm_aon_duty_l_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_duty_l_data_t duty_l;
    duty_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_l;
    duty_l.b.pwm_aon_duty_l_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_l = duty_l.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_duty_l_data_t.pwm_aon_duty_l_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_duty_l_data_t.pwm_aon_duty_l_j.
 */
static inline uint32_t hal_pwm_aon_duty_l_get_pwm_aon_duty_l_j(pwm_aon_channel_t channel)
{
    pwm_aon_duty_l_data_t duty_l;
    duty_l.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_l;
    return duty_l.b.pwm_aon_duty_l_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_duty_h_data_t.pwm_aon_duty_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_duty_h_data_t.pwm_aon_duty_h_j.
 */
static inline void hal_pwm_aon_duty_h_set_pwm_aon_duty_h_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_duty_h_data_t duty_h;
    duty_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_h;
    duty_h.b.pwm_aon_duty_h_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_h = duty_h.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_duty_h_data_t.pwm_aon_duty_h_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_duty_h_data_t.pwm_aon_duty_h_j.
 */
static inline uint32_t hal_pwm_aon_duty_h_get_pwm_aon_duty_h_j(pwm_aon_channel_t channel)
{
    pwm_aon_duty_h_data_t duty_h;
    duty_h.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_duty_h;
    return duty_h.b.pwm_aon_duty_h_j;
}

/**
 * @brief  Get the value of @ref pwm_aon_periodload_flag_data_t.pwm_aon_periodload_flag_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_periodload_flag_data_t.pwm_aon_periodload_flag_j.
 */
static inline uint32_t hal_pwm_aon_periodload_flag_get_pwm_aon_periodload_flag_j(pwm_aon_channel_t channel)
{
    pwm_aon_periodload_flag_data_t periodload_flag;
    periodload_flag.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_periodload_flag;
    return periodload_flag.b.pwm_aon_periodload_flag_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_period_val_data_t.pwm_aon_period_val_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_period_val_data_t.pwm_aon_period_val_j.
 */
static inline void hal_pwm_aon_period_val_set_pwm_aon_period_val_j(pwm_aon_channel_t channel, uint16_t val)
{
    pwm_aon_period_val_data_t period_val;
    period_val.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_period_val;
    period_val.b.pwm_aon_period_val_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_period_val = period_val.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_period_val_data_t.pwm_aon_period_val_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_period_val_data_t.pwm_aon_period_val_j.
 */
static inline uint32_t hal_pwm_aon_period_val_get_pwm_aon_period_val_j(pwm_aon_channel_t channel)
{
    pwm_aon_period_val_data_t period_val;
    period_val.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_period_val;
    return period_val.b.pwm_aon_period_val_j;
}

/**
 * @brief  Get the value of @ref pwm_aon_periodcnt_data_t.pwm_aon_periodcnt_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_periodcnt_data_t.pwm_aon_periodcnt_j.
 */
static inline uint32_t hal_pwm_aon_periodcnt_get_pwm_aon_periodcnt_j(pwm_aon_channel_t channel)
{
    pwm_aon_periodcnt_data_t periodcnt;
    periodcnt.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_periodcnt;
    return periodcnt.b.pwm_aon_periodcnt_j;
}

/**
 * @brief  Set the value of @ref pwm_aon_mode_data_t.pwm_aon_mode_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @param  [in]  val The value of @ref pwm_aon_mode_data_t.pwm_aon_mode_j.
 */
static inline void hal_pwm_aon_period_val_set_pwm_aon_mode_j(pwm_aon_channel_t channel, uint32_t val)
{
    pwm_aon_mode_data_t pwm_aon_mode;
    pwm_aon_mode.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_mode;
    pwm_aon_mode.b.pwm_aon_mode_j = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_mode = pwm_aon_mode.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_mode_data_t.pwm_aon_mode_j.
 * @param  [in]  channel The value of @ref pwm_aon_v151_regs_t.pwm_aon_ip.
 * @return The value of @ref pwm_aon_mode_data_t.pwm_aon_mode_j.
 */
static inline uint32_t hal_pwm_aon_period_val_get_pwm_aon_mode_j(pwm_aon_channel_t channel)
{
    pwm_aon_mode_data_t pwm_aon_mode;
    pwm_aon_mode.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_ip[channel].pwm_aon_mode;
    return pwm_aon_mode.b.pwm_aon_mode_j;
}

/**
 * @brief  Get the value of @ref pwm_aon_abnor_state0_data_t.pwm_aon_abnor_state0.
 * @return The value of @ref pwm_aon_abnor_state0_data_t.pwm_aon_abnor_state0.
 */
static inline uint32_t hal_pwm_aon_abnor_state0_get_pwm_aon_abnor_state0(void)
{
    pwm_aon_abnor_state0_data_t abnor_state0;
    abnor_state0.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state0;
    return abnor_state0.b.pwm_aon_abnor_state0;
}

/**
 * @brief  Get the value of @ref pwm_aon_abnor_state1_data_t.pwm_aon_abnor_state1.
 * @return The value of @ref pwm_aon_abnor_state1_data_t.pwm_aon_abnor_state1.
 */
static inline uint32_t hal_pwm_aon_abnor_state1_get_pwm_aon_abnor_state1(void)
{
    pwm_aon_abnor_state1_data_t abnor_state1;
    abnor_state1.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state1;
    return abnor_state1.b.pwm_aon_abnor_state1;
}

/**
 * @brief  Get the value of @ref pwm_aon_abnor_state1_data_t.reserved16_31.
 * @return The value of @ref pwm_aon_abnor_state1_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_aon_abnor_state1_get_reserved(void)
{
    pwm_aon_abnor_state1_data_t abnor_state1;
    abnor_state1.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state1;
    return abnor_state1.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_aon_abnor_state_clr0_data_t.pwm_aon_abnor_state_clr0.
 * @param  [in]  val The value of @ref pwm_aon_abnor_state_clr0_data_t.pwm_aon_abnor_state_clr0.
 */
static inline void hal_pwm_aon_abnor_state_clr0_set_pwm_aon_abnor_state_clr0(uint32_t val)
{
    pwm_aon_abnor_state_clr0_data_t abnor_state_clr0;
    abnor_state_clr0.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr0;
    abnor_state_clr0.b.pwm_aon_abnor_state_clr0 = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr0 = abnor_state_clr0.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_abnor_state_clr0_data_t.reserved16_31.
 * @return The value of @ref pwm_aon_abnor_state_clr0_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_aon_abnor_state_clr0_get_reserved(void)
{
    pwm_aon_abnor_state_clr0_data_t abnor_state_clr0;
    abnor_state_clr0.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr0;
    return abnor_state_clr0.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_aon_abnor_state_clr1_data_t.pwm_aon_abnor_state_clr1.
 * @param  [in]  val The value of @ref pwm_aon_abnor_state_clr1_data_t.pwm_aon_abnor_state_clr1.
 */
static inline void hal_pwm_aon_abnor_state_clr1_set_pwm_aon_abnor_state_clr1(uint32_t val)
{
    pwm_aon_abnor_state_clr1_data_t abnor_state_clr1;
    abnor_state_clr1.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr1;
    abnor_state_clr1.b.pwm_aon_abnor_state_clr1 = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr1 = abnor_state_clr1.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_abnor_state_clr1_data_t.reserved16_31.
 * @return The value of @ref pwm_aon_abnor_state_clr1_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_aon_abnor_state_clr1_get_reserved(void)
{
    pwm_aon_abnor_state_clr1_data_t abnor_state_clr1;
    abnor_state_clr1.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_abnor_state_clr1;
    return abnor_state_clr1.b.reserved16_31;
}

/**
 * @brief  Set the value of @ref pwm_aon_int_mask_data_t.pwm_aon_int_mask.
 * @param  [in]  val The value of @ref pwm_aon_int_mask_data_t.pwm_aon_int_mask.
 */
static inline void hal_pwm_aon_int_mask_set_pwm_aon_int_mask(uint32_t val)
{
    pwm_aon_int_mask_data_t int_mask;
    int_mask.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_int_mask;
    int_mask.b.pwm_aon_int_mask = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_int_mask = int_mask.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_int_mask_data_t.pwm_aon_int_mask.
 * @return The value of @ref pwm_aon_int_mask_data_t.pwm_aon_int_mask.
 */
static inline uint32_t hal_pwm_aon_int_mask_get_pwm_aon_int_mask(void)
{
    pwm_aon_int_mask_data_t int_mask;
    int_mask.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_int_mask;
    return int_mask.b.pwm_aon_int_mask;
}

/**
 * @brief  Set the value of @ref pwm_aon_dma_en_data_t.pwm_aon_dma_en.
 * @param  [in]  val The value of @ref pwm_aon_dma_en_data_t.pwm_aon_dma_en.
 */
static inline void hal_pwm_aon_dma_en_set_pwm_aon_dma_en(uint32_t val)
{
    pwm_aon_dma_en_data_t dma_en;
    dma_en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_dma_en;
    dma_en.b.pwm_aon_dma_en = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_dma_en = dma_en.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_dma_en_data_t.pwm_aon_dma_en.
 * @return The value of @ref pwm_aon_dma_en_data_t.pwm_aon_dma_en.
 */
static inline uint32_t hal_pwm_aon_dma_en_get_pwm_aon_dma_en(void)
{
    pwm_aon_dma_en_data_t dma_en;
    dma_en.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_dma_en;
    return dma_en.b.pwm_aon_dma_en;
}

/**
 * @brief  Set the value of @ref pwm_aon_cfg_int_clr0_data_t.pwm_aon_cfg_int_clr0.
 * @param  [in]  val The value of @ref pwm_aon_cfg_int_clr0_data_t.pwm_aon_cfg_int_clr0.
 */
static inline void hal_pwm_aon_cfg_int_clr0_set_pwm_aon_cfg_int_clr0(uint32_t val)
{
    pwm_aon_cfg_int_clr0_data_t cfg_int_clr0;
    cfg_int_clr0.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_cfg_int_clr0;
    cfg_int_clr0.b.pwm_aon_cfg_int_clr0 = val;
    ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_cfg_int_clr0 = cfg_int_clr0.d32;
}

/**
 * @brief  Get the value of @ref pwm_aon_cfg_int_clr0_data_t.reserved16_31.
 * @return The value of @ref pwm_aon_cfg_int_clr0_data_t.reserved16_31.
 */
static inline uint32_t hal_pwm_aon_cfg_int_clr0_get_reserved(void)
{
    pwm_aon_cfg_int_clr0_data_t cfg_int_clr0;
    cfg_int_clr0.d32 = ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_cfg_int_clr0;
    return cfg_int_clr0.b.reserved16_31;
}

#ifdef CONFIG_PWM_AON_PERIODLOAD_FLAG_BIT
/**
 * @brief  Set the value of @ref pwm_aon_periodload_flag.
 * @param  [in]  val The value of @ref pwm_aon_periodload_flag.
 */
static inline uint32_t hal_pwm_aon_get_periodload_flag(void)
{
    return ((pwm_aon_v151_regs_t *)g_pwm_aon_regs)->pwm_aon_periodload_flag;
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif