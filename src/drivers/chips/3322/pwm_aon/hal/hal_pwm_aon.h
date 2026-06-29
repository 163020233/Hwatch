/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL PWM AON  \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */
#ifndef HAL_PWM_AON_H
#define HAL_PWM_AON_H

#include <stdint.h>
#include "errcode.h"
#include "pwm_aon_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pwm_aon_api PWM
 * @ingroup  drivers_hal_pwm_aon
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of PWM AON  actions.
 * @else
 * @brief  PWM动作模式定义。
 * @endif
 */
typedef enum {
    PWM_AON_ACTION_START,
    PWM_AON_ACTION_STOP,
    PWM_AON_ACTION_REFRESH,
    PWM_AON_ACTION_CONTINUE_SET,
    PWM_AON_ACTION_CONTINUE_CLR
} pwm_aon_action_t;

/**
 * @if Eng
 * @brief  Definition of the set low/high level time ID of hal PWM.
 * @else
 * @brief  PWM设置高低电平时间ID定义。
 * @endif
 */
typedef enum hal_pwm_aon_set_time_id {
    PWM_AON_SET_LOW_TIME = 0,       /*!< @if Eng Set PWM AON  low level time.
                                     @else   设置PWM低电平时间。 @endif */
    PWM_AON_SET_HIGH_TIME,          /*!< @if Eng Set  PWM AON  high level time.
                                     @else   设置PWM高电平时间。 @endif */
    PWM_AON_SET_OFFSET_TIME,        /*!< @if Eng PWM AON  offset time.
                                     @else   PWM相位。 @endif */
    PWM_AON_SET_TIEM_MAX,
    PWM_AON_SET_TIME_ID_INVALID = 0xFF
} hal_pwm_aon_set_time_id_t;

 /**
 * @if Eng
 * @brief  Definition of hal PWM AON  callback type.
 * @param  [in]  channel PWM AON  device.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM AON  hal层回调类型定义。
 * @param  [in]  channel PWM设备。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*hal_pwm_aon_callback_t)(uint8_t channel);

 /**
 * @if Eng
 * @brief  Set the PWM AON  low/high level time.
 * @param  [in]  id Low/high level time ID.
 * @param  [in]  channel PWM AON  device.
 * @param  [in]  time  The value to set the low/high time of PWM.
 * @else
 * @brief  设置PWM高低电平时间。
 * @param  [in]  id 高低电平时间ID。
 * @param  [in]  channel PWM设备。
 * @param  [in]  time  用于设置PWM高低电平时间的值。
 * @endif
 */
typedef void (*hal_pwm_aon_set_time_t)(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time);

/**
 * @if Eng
 * @brief  Initialize device for hal PWM.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层PWM的初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_pwm_aon_init(void);

/**
 * @if Eng
 * @brief  Deinitialize device for hal PWM.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  HAL层PWM的去初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
void hal_pwm_aon_deinit(void);

 /**
 * @if Eng
 * @brief  Set the PWM AON  low/high level time.
 * @param  [in]  id Low/high level time ID.
 * @param  [in]  channel PWM AON  device.
 * @param  [in]  time  The value to set the low/high time of PWM.
 * @else
 * @brief  设置PWM高低电平时间。
 * @param  [in]  id 高低电平时间ID。
 * @param  [in]  channel PWM设备。
 * @param  [in]  time  用于设置PWM高低电平时间的值。
 * @endif
 */
void hal_pwm_aon_set_time(hal_pwm_aon_set_time_id_t id, pwm_aon_channel_t channel, uint32_t time);

 /**
 * @if Eng
 * @brief  Set the PWM AON  repeat cycles.
 * @param  [in]  channel PWM AON  device.
 * @param  [in]  cycles  The value to set repeat cycles.
 * @else
 * @brief  设置PWM重复周期。
 * @param  [in]  channel PWM设备。
 * @param  [in]  cycles  重复周期值。
 * @endif
 */
void hal_pwm_aon_set_cycles(pwm_aon_channel_t channel, uint16_t cycles);

 /**
 * @if Eng
 * @brief  Set the PWM AON  action.
 * @param  [in]  channel PWM AON  device.
 * @param  [in]  action  PWM AON  action.
 * @else
 * @brief  设置PWM动作模式。
 * @param  [in]  channel PWM设备。
 * @param  [in]  action  PWM动作模式。
 * @endif
 */
void hal_pwm_aon_set_action(uint8_t channel, pwm_aon_action_t action);

 /**
 * @if Eng
 * @brief  Clear the PWM AON  interrupt.
 * @param  [in]  channel PWM AON  device.
 * @else
 * @brief  清除PWM中断。
 * @param  [in]  channel PWM设备。
 * @endif
 */
void hal_pwm_aon_intr_clear(pwm_aon_channel_t channel);

 /**
 * @if Eng
 * @brief  Register a callback asociated with a PWM AON  interrupt cause.
 * @param  [in]  channel PWM AON  device.
 * @param  [in]  callback  The interrupt callback to register.
 * @else
 * @brief  注册与PWM中断原因关联的回调。
 * @param  [in]  channel PWM设备。
 * @param  [in]  callback  寄存器的中断回调。
 * @endif
 */
void hal_pwm_aon_register_callback(pwm_aon_channel_t channel, hal_pwm_aon_callback_t callback);

 /**
 * @if Eng
 * @brief  Unregister a previously registered callback for a PWM AON  device.
 * @param  [in]  channel PWM AON  device.
 * @else
 * @brief  为PWM设备注销先前注册的回调。
 * @param  [in]  channel PWM设备。
 * @endif
 */
void hal_pwm_aon_unregister_callback(pwm_aon_channel_t channel);

#if defined(CONFIG_PWM_AON_USING_V151) || defined(CONFIG_PWM_AON_USING_V152)
 /**
 * @if Eng
 * @brief  Assigning channels to PWM AON  groups, one group can have more than one channel,
 *         the same channel cannot be in different groups at the same time.
 * @param  [in]  group PWM AON  group.
 * @param  [in]  channel_id PWM AON  device.
 * @else
 * @brief  为PWM组分配通道，一个组可以有多个通道，同一个通道不能同时位于不同的组里。
 * @param  [in]  group   PWM组。
 * @param  [in]  channel_id PWM设备。
 * @endif
 */
void hal_pwm_aon_set_group(pwm_aon_v151_group_t group, uint16_t channel_id);

#if defined(CONFIG_PWM_AON_PRELOAD)
  /**
 * @if Eng
 * @brief  config pwm_aon preload param.
 * @param  [in]  group PWM AON  group.
 * @else
 * @brief  为PWM配置预加载参数。
 * @param  [in]  group   PWM组。
 * @endif
 */
void hal_pwm_aon_config_preload(uint8_t group);
#endif /* CONFIG_PWM_AON_PRELOAD */
#endif /* CONFIG_PWM_AON_USING_V151 */

/**
 * @if Eng
 * @brief  Init the pwm_aon which will set the base address of registers.
 * @retval ERRCODE_SUCC   Success.
 * @retval Other          Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化PWM，设置寄存器的基地址。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_pwm_aon_regs_init(void);

/**
 * @if Eng
 * @brief  Deinit the hal_drv_pwm_aon which will clear the base address of registers has been
 *         set by @ref hal_pwm_aon_regs_init.
 * @else
 * @brief  去初始化，然后清除在 @ref hal_pwm_aon_regs_init 中设置的寄存器地址。
 * @endif
 */
void hal_pwm_aon_regs_deinit(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif