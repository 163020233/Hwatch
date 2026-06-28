/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides pwm led header \n
 *
 * History: \n
 * 2025-04-14， Create file. \n
 */

#ifndef PWM_LED_H
#define PWM_LED_H

#include <stdbool.h>
#include "errcode.h"
#include "pwm_led_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_pwm_led PWM_LED
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of pwm led work mode.
 * @else
 * @brief  PWM_LED工作模式定义。
 * @endif
 */
typedef enum pwm_led_mode {
    PWM_LED_LOOP,                           /*!< @if Eng loop.
                                                 @else   循环。 @endif */
    PWM_LED_LOOP_INVERSE,                   /*!< @if Eng loop inverse.
                                                 @else   循环翻转。 @endif */
    PWM_LED_NO_LOOP,                        /*!< @if Eng no loop.
                                                 @else   不循环。 @endif */
} pwm_led_mode_t;

/**
 * @if Eng
 * @brief  PWM_LED light rank.
 * @else
 * @brief  PWM_LED亮度等级。
 * @endif
 */
typedef enum pwm_led_light_rank {
    PWM_LED_LIGHT_RANK_16 = 4,              /*!< @if Eng light rank 16.
                                                 @else   亮度等级16。 @endif */
    PWM_LED_LIGHT_RANK_32 = 5,              /*!< @if Eng light rank 32.
                                                 @else   亮度等级32。 @endif */
    PWM_LED_LIGHT_RANK_64 = 6,              /*!< @if Eng light rank 64.
                                                 @else   亮度等级64。 @endif */
    PWM_LED_LIGHT_RANK_128 = 7,             /*!< @if Eng light rank 128.
                                                 @else   亮度等级128。 @endif */
    PWM_LED_LIGHT_RANK_256 = 8,             /*!< @if Eng light rank 256.
                                                 @else   亮度等级256。 @endif */
    PWM_LED_LIGHT_RANK_512 = 9,             /*!< @if Eng light rank 512.
                                                 @else   亮度等级512。 @endif */
} pwm_led_light_rank_t;

/**
 * @if Eng
 * @brief  PWM_LED polarity.
 * @else
 * @brief  PWM_LED极性。
 * @endif
 */
typedef enum pwm_led_polarity {
    PWM_LED_POLARITY_POSITIVE = 0,              /*!< @if Eng positive polarity.
                                                     @else   正向极性。 @endif */
    PWM_LED_POLARITY_NEGATIVE = 1,              /*!< @if Eng negative polarity.
                                                     @else   负向极性。 @endif */
} pwm_led_polarity_t;


/**
 * @if Eng
 * @brief  Definition of PWM_LED parameters.
 * @else
 * @brief  PWM_LED参数定义。
 * @endif
 */
typedef struct pwm_led_config {
    uint8_t mode;                           /*!< @if Eng Work mode of PWM_LED.
                                                     For details, see @ref pwm_led_mode_t
                                             @else   PWM_LED工作模式。参考 @ref pwm_led_mode_t @endif */
    uint8_t light_rank;                     /*!< @if Eng light rank of PWM_LED.
                                                     For details, see @ref pwm_led_light_rank_t
                                             @else   PWM_LED亮度等级。参考 @ref pwm_led_light_rank_t @endif */
    uint8_t polarity;                       /*!< @if Eng polarity of PWM_LED.
                                                     For details, see @ref pwm_led_polarity_t
                                             @else   PWM_LED极性。参考 @ref pwm_led_polarity_t @endif */
    uint8_t start_addr;                     /*!< @if Eng Start address within the shared step table for this PWM_LED
                                                     channel, this defines the range [start_addr, end_addr] that
                                                     will be used by this channel. Multiple channels can use the same
                                                     address range whenthey need identical brightness profiles.
                                             @else   PWM_LED通道的配置点在共享步进表中的起始地址，定义本通道的地址
                                                     范围[start_addr, end_addr]，多个通道可以使用相同的地址范围，当它们
                                                     需要相同的亮度配置时。@endif */
    uint8_t end_addr;                       /*!< @if Eng End address within the shared step table for this PWM_LED
                                                     channel, this defines the range [start_addr, end_addr] that
                                                     will be used by this channel. Multiple channels can use the same
                                                     address range whenthey need identical brightness profiles.
                                             @else   PWM_LED通道的配置点在共享步进表中的结束地址，定义本通道的地址
                                                     范围[start_addr, end_addr]，多个通道可以使用相同的地址范围，当它们
                                                     需要相同的亮度配置时。@endif */
    uint8_t reserved[0x3];
    uint32_t step_size;                     /*!< @if Eng Step size of PWM_LED.
                                             @else   PWM_LED步长 @endif */
    uint32_t period;                        /*!< @if Eng Period count value for PWM_LED.
                                                     If PWM clock period is 31.25us, actual period = period * 31.25us.
                                             @else   PWM_LED工作时钟周期计数。
                                                     如果PWM工作周期为31.25us，实际周期 = period * 31.25us。 @endif */
} pwm_led_config_t;

/**
 * @if Eng
 * @brief  Initialize the PWM_LED.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化PWM_LED。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_led_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the PWM_LED.
 * @else
 * @brief  去初始化PWM_LED。
 * @endif
 */
void uapi_pwm_led_deinit(void);

/**
 * @if Eng
 * @brief  Configure a PWM_LED channel using the shared step table memory.
 * @param  [in]  channel PWM_LED channel to use.
 * @param  [in]  cfg Config set to the PWM_LED device. For details, see @ref pwm_led_config_t.
 * @param  [in]  step_table Pointer to the globally shared step table memory.
                            This same table is used by all PWM_LED channels.
                            PWM_LED driver copy entries in the table to PWM_LED inner buffer.
 * @param  [in]  step_table_len Total length of the shared step table,
                                do not exceed PWM_LED_STEP_TABLE_LENGTH.

 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  配置PWM_LED通道，使用共享步进表。
 * @param  [in]  channel PWM_LED通道。
 * @param  [in]  cfg PWM_LED设备的配置信息，参考 @ref pwm_led_config_t 。
 * @param  [in]  step_table 指向全局共享步进表内存的指针，所有PWM_LED通道都使用这个相同的表。
                            PWM_LED驱动将表中的配置项拷贝到PWM_LED内部的缓存中。
 * @param  [in]  step_table_len 共享步进表的总长度，不超过CONFIG_PWM_LED_POINT_NUM_MAX。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_led_config(uint8_t channel, const pwm_led_config_t *cfg,
                              uint16_t *step_table, uint32_t step_table_len);

/**
 * @if Eng
 * @brief  Start up PWM_LED.
 * @param  [in]  channel PWM_LED channel to use.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  开始PWM_LED。
 * @param  [in]  channel PWM_LED通道 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_led_start(uint8_t channel);

/**
 * @if Eng
 * @brief  Stop a running PWM_LED.
 * @param  [in]  channel PWM_LED channel to use.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  停止正在运行的PWM_LED。
 * @param  [in]  channel PWM_LED通道。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pwm_led_stop(uint8_t channel);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
