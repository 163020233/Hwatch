/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 PWM register \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef HAL_PWM_AON_V151_REGS_DEF_H
#define HAL_PWM_AON_V151_REGS_DEF_H

#include <stdint.h>
#include "pwm_aon_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pwm_aon_v151_regs_def PWM V151 Regs Definition
 * @ingroup  drivers_hal_pwm
 * @{
 */

#define BIT16        16

/**
 * @brief  This union represents the bit fields in the pwm_aon_sel register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_sel_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_sel_i             : 16;      /*!< Grouped pwm selection signals, up to ten groups,
                                                       each group corresponding to 10bit pwm_aon_sel, each
                                                       bit corresponding to one way pwm. */
        uint32_t reserved16_31         : 16;
    } b;
} pwm_aon_sel_data_t;


/**
 * @brief  This union represents the bit fields in the pwm_aon_startclrcnt_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_startclrcnt_en_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_startclrcnt_en_i  : 1;       /*!< It will clear pwm clk cnt(internal data, invisible) & refresh
                                                       new cfg to pwm registers when pwm_aon_start_i = 1. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_startclrcnt_en_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_start register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_start_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_start_i           : 1;       /*!< pwm configuration effective register. When set to 1,
                                                       the previously assigned pwm-related registers take effect.
                                                       Each group corresponds to 1bit pwm_aon_start.  */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_start_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_en_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_en_j              : 8;       /*!< The pwm function is enabled, each pwm channel corresponds to
                                                       one pwm_aon_en.
                                                       -0: pwm off, pwm_aon_out output is continuously 0.
                                                       -1: pwm enable on. */
        uint32_t pwm_aon_clk_en            : 8;       /*!< The pwm clock is enabled, each pwm channel corresponds to
                                                       one pwm_aon_clk_en.
                                                       -0: pwm off, pwm clock is continuously 0.
                                                       -1: pwm clock on. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_en_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_portity register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_portity_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_portity_j         : 1;       /*!< Pwm output polarity configuration.
                                                       -0: forward polarity.
                                                       -1: Reverse polarity. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_portity_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_oen_cfg register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_oen_cfg_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_oen_cfg_j         : 1;       /*!< Pwm invalid level high resistance enable configuration.
                                                       -0: Invalid level follows polarity and other configurations.
                                                       -1: Invalid level Pwm output high resistance. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_oen_cfg_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_offset_low register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_offset_l_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_offset_l_j        : 16;      /*!< The low 16 bits of the pwm phase control counter controls
                                                       the position of the pwm at 1 in a cycle. For PWM,
                                                       the value range is 1 to 65535. Each channel of PWM
                                                       corresponds to an offset parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_offset_l_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_offset_high register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_offset_h_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_offset_h_j        : 16;      /*!< The high 16 bits of the pwm phase control counter,
                                                       +1 after the pwm_aon_offset_l count is full, controls
                                                       the position of the pwm at 1 in a cycle. For PWM,
                                                       the value range is 1 to 65535. Each channel of PWM
                                                       corresponds to an offset parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_offset_h_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_freq_low register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_freq_l_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_freq_l_j          : 16;      /*!< The low 16 bits of the pwm frequency control count
                                                       means the number of times the PWM clock is divided,
                                                       ranging from 1 to 65535. Each channel of pwm
                                                       corresponds to a freq parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_freq_l_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_freq_high register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_freq_h_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_freq_h_j          : 16;      /*!< The high 16 bits of the pwm frequency control count,
                                                       +1 after the pwm_aon_freq_l count is full,
                                                       means the frequency division of the PWM clock,
                                                       the range is 1 to 65535. Each channel of pwm
                                                       corresponds to one freq parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_freq_h_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_duty_low register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_duty_l_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_duty_l_j          : 16;      /*!< The pwm duty cycle controls the low 16 bits of the count value,
                                                       ranging from 1 to 65535.
                                                       The duty cycle is the ratio of pwm_aon_duty to pwm_aon_freq.
                                                       Each pwm corresponds to a duty parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_duty_l_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_duty_high register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_duty_h_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_duty_h_j          : 16;      /*!< The high 16 bits of the pwm duty control counter,
                                                       +1 after pwm_aon_duty_l is full, in the range 1 to 65535.
                                                       The duty cycle is the ratio of pwm_aon_duty to pwm_aon_freq.
                                                       Each pwm corresponds to a duty parameter. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_duty_h_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_periodload_flag register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_periodload_flag_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_periodload_flag_j : 1;       /*!< The flag register in which new parameters can be configured
                                                       when pwm smoothly switches to the next cycle
                                                       at the end of the cycle. The flag is generated at the end
                                                       of the cycle count. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_periodload_flag_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_period_value register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_period_val_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_period_val_j      : 16;      /*!< Pwm output pulse value, non-zero trigger pwm output
                                                       pulse counting function, the number of output pulses
                                                       than this value will load new parameters to continue counting. */
        uint32_t reserved1_31          : 16;
    } b;
} pwm_aon_period_val_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_periodcnt register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_periodcnt_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_periodcnt_j       : 16;      /*!< Pwm output pulse count value. */
        uint32_t reserved16_31         : 16;
    } b;
} pwm_aon_periodcnt_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_mode register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_mode_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_mode_j            : 1;       /*!< Pwm out mode select.
                                                       -0: Pwm works in continuous out mode.
                                                       -1: Pwm works in discontinuous output mode.
                                                       After the PWM_AON_STARTCLRCNT_EN and PWM_AON_START registers are valid,
                                                       the PWM outputs pwm_aon_period_val cycles of waveforms. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_mode_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_abnor_state0 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_abnor_state0_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_abnor_state0      : 16;      /*!< The bit[15:0] is a configuration exception for Pwm
                                                       multi-channel use, corresponding to 1 bit per pwm channel. */
        uint32_t reserved16_31         : 16;
    } b;
} pwm_aon_abnor_state0_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_abnor_state1 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_abnor_state1_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_abnor_state1      : 16;      /*!< The bit[15:0] for Pwm count value configuration exception,
                                                       1 bit per pwm channel. */
        uint32_t reserved16_31         : 16;      /*!< Reserved. */
    } b;
} pwm_aon_abnor_state1_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_abnor_state_clr0 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_abnor_state_clr0_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_abnor_state_clr0  : 16;      /*!< Pwm exception clear register, corresponding to
                                                       register pwm_aon_abnor_state0. */
        uint32_t reserved16_31         : 16;      /*!< Reserved. */
    } b;
} pwm_aon_abnor_state_clr0_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_abnor_state_clr1 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_abnor_state_clr1_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_abnor_state_clr1  : 16;      /*!< Pwm exception clear register, corresponding to
                                                       register pwm_aon_abnor_state1. */
        uint32_t reserved16_31         : 16;      /*!< Reserved. */
    } b;
} pwm_aon_abnor_state_clr1_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_int_mask register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_int_mask_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_int_mask          : 2;       /*!< pwm interrupt mask register.
                                                       -0: mask;
                                                       -1: no mask, default mask.
                                                       bit[1]:step mode end of cycle cycle interrupt mask.
                                                       bit[0]:abnormal interrupt mask. */
        uint32_t reserved2_31          : 30;
    } b;
} pwm_aon_int_mask_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_dma_en register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_dma_en_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_dma_en            : 1;       /*!< dma function enable
                                                       -1: Enabled.
                                                       -0: not enabled. */
        uint32_t reserved1_31          : 31;
    } b;
} pwm_aon_dma_en_data_t;

/**
 * @brief  This union represents the bit fields in the pwm_aon_cfg_int_clr0 register.
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union pwm_aon_cfg_int_clr0_data {
    uint32_t d32;                                 /*!< Raw register data. */
    struct {
        uint32_t pwm_aon_cfg_int_clr0      : 16;      /*!< Pwm step mode end-of-cycle interrupt clear register.
                                                       Corresponds to the pwm_aon_cfg_int interrupt
                                                       and pwm_aon_periodload_flag_j registers. */
        uint32_t reserved16_31         : 16;      /*!< Reserved. */
    } b;
} pwm_aon_cfg_int_clr0_data_t;

/**
 * @brief  Registers associated with the group of PWM.
 */
typedef struct pwm_aon_v151_sel {
    volatile uint32_t pwm_aon_sel;                     /*!< pwm_aon_sel.             <i>Offset: 000h+10h*i</i>. */
    volatile uint32_t pwm_aon_startclrcnt_en;          /*!< pwm_aon_startclrcnt_en.  <i>Offset: 004h+10h*i</i>. */
    volatile uint32_t pwm_aon_start;                   /*!< pwm_aon_start.           <i>Offset: 008h+10h*i</i>. */
    volatile uint32_t reserved;                    /*!< reserved.            <i>Offset: 00Ch+10h*i</i>. */
} pwm_aon_v151_sel_t;

/**
 * @brief  Registers associated with the channel of PWM.
 */
typedef struct pwm_aon_ip {
    volatile uint32_t pwm_aon_en;                      /*!< pwm_aon_en.               <i>Offset: 100h+40h*j</i>. */
    volatile uint32_t pwm_aon_portity;                 /*!< pwm_aon_portity.          <i>Offset: 104h+40h*j</i>. */
    volatile uint32_t pwm_aon_oen_cfg;                 /*!< pwm_aon_oen_cfg.          <i>Offset: 108h+40h*j</i>. */
    volatile uint32_t pwm_aon_offset_l;                /*!< pwm_aon_offset_l.         <i>Offset: 10Ch+40h*j</i>. */
    volatile uint32_t pwm_aon_offset_h;                /*!< pwm_aon_offset_h.         <i>Offset: 110h+40h*j</i>. */
    volatile uint32_t pwm_aon_freq_l;                  /*!< pwm_aon_freq_l.           <i>Offset: 114h+40h*j</i>. */
    volatile uint32_t pwm_aon_freq_h;                  /*!< pwm_aon_freq_h.           <i>Offset: 118h+40h*j</i>. */
    volatile uint32_t pwm_aon_duty_l;                  /*!< pwm_aon_duty_l.           <i>Offset: 11Ch+40h*j</i>. */
    volatile uint32_t pwm_aon_duty_h;                  /*!< pwm_aon_duty_h.           <i>Offset: 120h+40h*j</i>. */
    volatile uint32_t pwm_aon_periodload_flag;         /*!< pwm_aon_periodload_flag.  <i>Offset: 124h+40h*j</i>. */
    volatile uint32_t pwm_aon_period_val;              /*!< pwm_aon_period_val.       <i>Offset: 128h+40h*j</i>. */
    volatile uint32_t pwm_aon_periodcnt;               /*!< pwm_aon_periodcnt.        <i>Offset: 12Ch+40h*j</i>. */
    volatile uint32_t pwm_aon_mode;                    /*!< pwm_aon_out_mode.         <i>Offset: 130h+40h*j</i>. */
    volatile uint32_t reserved1;                   /*!< reserved1.            <i>Offset: 134h+40h*j</i>. */
    volatile uint32_t reserved2;                   /*!< reserved2.            <i>Offset: 138h+40h*j</i>. */
    volatile uint32_t reserved3;                   /*!< reserved3.            <i>Offset: 13Ch+40h*j</i>. */
} pwm_aon_ip_t;

typedef struct pwm_aon_v151_regs {
    volatile pwm_aon_v151_sel_t pwm_aon_v151_sel[CONFIG_PWM_AON_GROUP_NUM];      /*!< PWM GROUP.           <i>Offset: 000h</i>. */
    volatile pwm_aon_v151_sel_t pwm_aon_v151_sel_reserv[BIT16 - CONFIG_PWM_AON_GROUP_NUM];
    volatile pwm_aon_ip_t pwm_aon_ip[CONFIG_PWM_AON_CHANNEL_NUM];                /*!< PWM CHANNEL.         <i>Offset: 100h</i>. */
    volatile pwm_aon_ip_t pwm_aon_ip_reserv1[BIT16 - CONFIG_PWM_AON_CHANNEL_NUM];
    volatile uint32_t pwm_aon_abnor_state0;                              /*!< pwm_aon_abnor_state0.    <i>Offset: 500h</i>. */
    volatile uint32_t pwm_aon_abnor_state1;                              /*!< pwm_aon_abnor_state1.    <i>Offset: 504h</i>. */
    volatile uint32_t pwm_aon_abnor_state_clr0;                          /*!< pwm_aon_abnor_state_clr0.<i>Offset: 508h</i>. */
    volatile uint32_t pwm_aon_abnor_state_clr1;                          /*!< pwm_aon_abnor_state_clr1.<i>Offset: 50Ch</i>. */
    volatile uint32_t pwm_aon_int_mask;                                  /*!< pwm_aon_int_mask.        <i>Offset: 510h</i>. */
    volatile uint32_t pwm_aon_dma_en;                                    /*!< pwm_aon_dma_en.          <i>Offset: 514h</i>. */
    volatile uint32_t pwm_aon_cfg_int_clr0;                              /*!< pwm_aon_cfg_int_clr0.    <i>Offset: 518h</i>. */
#ifdef CONFIG_PWM_AON_PERIODLOAD_FLAG_BIT
    volatile uint32_t pwm_aon_periodload_flag;                           /*!< pwm_aon_periodload_flag. <i>Offset: 51Ch</i>. */
#endif
} pwm_aon_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif