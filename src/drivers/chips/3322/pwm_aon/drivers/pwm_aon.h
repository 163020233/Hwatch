/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides PWM AON  driver api \n
 *
 * History: \n
 * 2022-09-16， Create file. \n
 */

#ifndef PWM_AON_H
#define PWM_AON_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pwm PWM AON
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of PWM AON  configuration.
 * @else
 * @brief  PWM配置类型定义。
 * @endif
 */
typedef struct pwm_aon_config {
    uint32_t low_time;               /*!< @if Eng PWM AON  working clock cycle count low level part,
                                                  PWM AON  work frequency @ref uapi_pwm_aon_get_frequency().
                                                  If the PWM AON  operating cycle is Tus,
                                                  the actual low-level time = low_time * Tus.
                                          @else   PWM工作时钟周期计数个数低电平部分，
                                                  频率参考 @ref uapi_pwm_aon_get_frequency()。
                                                  如果PWM工作周期为Tus, 实际低电平时间 = low_time * Tus @endif */
    uint32_t high_time;              /*!< @if Eng PWM AON  working clock cycle count high level part,
                                                  PWM AON  work frequency @ref uapi_pwm_aon_get_frequency().
                                                  If the PWM AON  operating cycle is Tus,
                                                  the actual high-level time = high_time * Tus.
                                          @else   PWM工作时钟周期计数个数高电平部分，
                                                  频率参考 @ref uapi_pwm_aon_get_frequency()。
                                                  如果PWM工作周期为Tus, 实际高电平时间 = high_time * Tus @endif */
    uint32_t offset_time;            /*!< @if Eng PWM AON  offset time.
                                          @else   PWM相位。  @endif */
    uint16_t cycles;                 /*!< @if Eng PWM AON  cycles, range:0~32767 (15bit).
                                          @else   PWM重复周期，范围：0~32767 (15bit)。  @endif */
    bool repeat;                     /*!< @if Eng Flag to indicate PWM AON  should output continuously.
                                          @else   指示PWM应连续输出的标志。  @endif */
} pwm_aon_config_t;

 /**
 * @if Eng
 * @brief  Definition of PWM AON  callback type.
 * @param  [in]  channel PWM AON  device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM AON  回调类型定义。
 * @param  [in]  channel PWM设备。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
typedef errcode_t (*pwm_aon_callback_t)(uint8_t channel);

/**
 * @if Eng
 * @brief  Initialize the PWM AON .
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化PWM。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the PWM AON .
 * @else
 * @brief  去初始化PWM。
 * @endif
 */
void uapi_pwm_aon_deinit(void);

/**
 * @if Eng
 * @brief  Open a PWM AON  device with the specified configuration.
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @param  [in]  cfg Config set to the PWM AON  device. For details, see @ref pwm_aon_config_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  打开具有指定配置的PWM通道。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @param  [in]  cfg PWM设备的配置信息，参考 @ref pwm_aon_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_open(uint8_t channel, const pwm_aon_config_t *cfg);

/**
 * @if Eng
 * @brief  Close the specified PWM AON .
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  关闭指定的PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_close(uint8_t channel);

/**
 * @if Eng
 * @brief  Start up PWM AON .
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开始PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_start(uint8_t channel);

/**
 * @if Eng
 * @brief  Get PWM AON  work frequency.
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval PWM AON  work frequency, the unit is HZ.
 * @else
 * @brief  获取PWM工作频率。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval PWM工作频率，单位为HZ。
 * @endif
 */
uint32_t uapi_pwm_aon_get_frequency(uint8_t channel);

#if defined(CONFIG_PWM_AON_USING_V150)
/**
 * @if Eng
 * @brief  Stop a running PWM AON .
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止正在运行的PWM。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_stop(uint8_t channel);

/**
 * @if Eng
 * @brief  Update the duty ratio on a opened PWM AON .
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @param  [in]  low_time PWM AON  working clock cycle count low level part, PWM AON  work freq @ref uapi_pwm_aon_get_frequency().
                          If the PWM AON  operating cycle is Tus, the actual low-level time = low_time * Tus.
 * @param  [in]  high_time PWM AON  working clock cycle count high level part, PWM AON  work freq @ref uapi_pwm_aon_get_frequency().
                           If the PWM AON  operating cycle is Tus, the actual high-level time = high_time * Tus.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新打开的PWM上的占空比。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @param  [in]  low_time PWM工作时钟周期计数个数低电平部分，频率参考 @ref uapi_pwm_aon_get_frequency()。
                          如果PWM工作周期为Tus, 实际低电平时间 = low_time * Tus。
 * @param  [in]  high_time PWM工作时钟周期计数个数高电平部分，频率参考 @ref uapi_pwm_aon_get_frequency()。
                           如果PWM工作周期为Tus, 实际高电平时间 = high_time * Tus。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_update_duty_ratio(uint8_t channel, uint32_t low_time, uint32_t high_time);
#endif /* CONFIG_PWM_AON_USING_V150 */

/**
 * @if Eng
 * @brief  PWM AON  Interrupt Service Routine.
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM中断服务例程。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_isr(uint8_t channel);

/**
 * @if Eng
 * @brief  Register an interrupt callback for PWM AON .
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @param  [in]  callback Callback to register.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  为PWM注册中断回调。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @param  [in]  callback 回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_register_interrupt(uint8_t channel, pwm_aon_callback_t callback);

/**
 * @if Eng
 * @brief  Unregister an interrupt callback.
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去注册中断回调。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_unregister_interrupt(uint8_t channel);

#if defined(CONFIG_PWM_AON_USING_V151) || defined(CONFIG_PWM_AON_USING_V152)
/**
 * @if Eng
 * @brief  Grouping the channels for PWM AON  channels into one group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_AON_GROUP_NUM.
 * @param  [in]  channel_set Set of channels to be grouped.
 * @param  [in]  channel_set_len Length of the channel set.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  为PWM通道分组。
 * @param  [in]  group PWM组。
 * @param  [in]  channel_set 进行分组设置的通道集合。
 * @param  [in]  channel_set_len 进行分组设置的通道集合长度。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_set_group(uint8_t group, const uint8_t *channel_set, uint32_t channel_set_len);

/**
 * @if Eng
 * @brief  Clear PWM AON  channel of the group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_AON_GROUP_NUM.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清理PWM通道分组。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_clear_group(uint8_t group);

/**
 * @if Eng
 * @brief  Start up PWM AON  of a group.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_AON_GROUP_NUM.
 *                          corresponds to a corresponding channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动指定分组的PWM。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_start_group(uint8_t group);

/**
 * @if Eng
 * @brief  Stop PWM AON  of a group
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_AON_GROUP_NUM.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止指定分组的PWM。
 * @param  [in]  group PWM组。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_stop_group(uint8_t group);

/**
 * @if Eng
 * @brief  Update a PWM AON  device with the specified configuration.
 * @param  [in]  channel PWM AON  channel to use. For details, see @ref pwm_aon_channel_t.
 * @param  [in]  cfg Config set to the PWM AON  device. For details, see @ref pwm_aon_config_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  更新指定PWM通道的配置。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @param  [in]  cfg PWM设备的配置信息，参考 @ref pwm_aon_config_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_update_cfg(uint8_t channel, const pwm_aon_config_t *cfg);
#if defined(CONFIG_PWM_AON_PRELOAD)
/**
 * @if Eng
 * @brief  PWM AON  preload. When the previous PWM AON  configuration is completed and configuration are auto loaded.
 * @param  [in]  group The group ID which can be from 0 to CONFIG_PWM_AON_GROUP_NUM.
 * @param  [in]  channel PWM AON  channel. For details, see @ref pwm_aon_channel_t.
 * @param  [in]  cfg Config set to the PWM AON  device.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  PWM预配置，当上一个PWM配置完成时，此配置会自动加载。
 * @param  [in]  group PWM组。
 * @param  [in]  channel PWM通道，参考 @ref pwm_aon_channel_t 。
 * @param  [in]  cfg PWM设备的配置信息。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_config_preload(uint8_t group, uint8_t channel, const pwm_aon_config_t *cfg);
#endif /* CONFIG_PWM_AON_PRELOAD */
#endif /* CONFIG_PWM_AON_USING_V151 */

#if defined(CONFIG_PWM_AON_SUPPORT_LPM)
/**
 * @if Eng
 * @brief  PWM AON  power lower suspend.
 * @param  [in]  arg Argument for suspend.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  挂起所有PWM通道
 * @param  [in]  arg 挂起所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  PWM AON  power lower resume.
 * @param  [in]  arg Argument for resume.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  恢复所有PWM通道
 * @param  [in]  arg 恢复所需要的参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_aon_resume(uintptr_t arg);
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
