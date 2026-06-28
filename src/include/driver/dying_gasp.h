/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides DYING_GASP driver api
 */

#ifndef DYING_GASP_H
#define DYING_GASP_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_dying_gasp dying_gasp
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Definition of the dying_gasp control source，提供参考给 @ref uapi_dygp_set_ctrl 中的trim_sel参数.
 * @param  EFUSE_CONTRAL The EFUSE is used as the control source.
 * @param  REGISTER_CONTRAL Register as control source.
 * @else
 * @brief  定义dying_gasp的控制来源。
 * @param  EFUSE_CONTRAL EFUSE做为控制源.
 * @param  REGISTER_CONTRAL 寄存器做为控制源.
 * @endif
 */
typedef enum {
    DYING_GASP_EFUSE_CONTRAL = 0,
    DYING_GASP_REGISTER_CONTRAL = 1,
} trim_sel_t;

/**
 * @if Eng
 * @brief  Dying_gasp irq event callback to be called when irq callback set
    @ref uapi_dygp_register_callback is invoked.
 * @note   This callback is invoked in an interrupt context.
 * @param  index The index of Dying_gasp.
 * @else
 * @brief  Dying_gasp中断事件的回调函数，通过 @ref uapi_dygp_register_callback 注册到驱动中.
 * @note   这个函数是在中断上下文中执行的。
 * @param  index 指向Dying_gasp IP .

 * @endif
 */
typedef void (*dying_gasp_callback_t)(uint32_t index);

/**
 * @if Eng
 * @brief  Initialize Dying_gasp.
 * @param  index The index of Dying_gasp.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  初始化Dying_gasp。
 * @param  index Dying_gasp 的索引.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_init(uint32_t index);

/**
 * @if Eng
 * @brief  Deinitialize Dying_gasp.
 * @param  index The index of Dying_gasp.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  去初始化Dying_gasp。
 * @param  index Dying_gasp 的索引.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_deinit(uint32_t index);

/**
 * @if Eng
 * @brief  Dying_gasp Unregister the interrupt callback function.
 * @param  index The index of Dying_gasp.
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Dying_gasp 在指定索引下取消注册中断回调函数。
 * @param  index Dying_gasp 的索引.
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_unregister_callback(uint32_t index);

/**
 * @if Eng
 * @brief  Dying_gasp Registers the interrupt callback function under the specified index.
 * @param  index The index of Dying_gasp.
 * @param  callback Interrupt callback function.
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Dying_gasp 在指定索引下注册中断回调函数。
 * @param  index Dying_gasp 的索引.
 * @param  callback 中断回调函数.
 * @retval ERRCODE_SUCC 成功.
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_register_callback(uint32_t index, dying_gasp_callback_t callback);

/**
 * @if Eng
 * @brief  Dying_gasp Set Control Source and Set the value of trim_val.
 * @param  index The index of Dying_gasp.
 * @param  trim_sel Dying_gasp Control Source，When the control source comes from the register，
    the power-off protection program is executed according to the register configuration，and trim_val is
    used for voltage calibration，When the EFUSE is used as the control source, configurations are read
    from the EFUSE to implement power-off protection (References trim_sel_t).
 * @param  trim_val This parameter is used to calibrate the voltage when the control source is set to register
    control (unit: mv).
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Dying_gasp 设置控制源和设置trim_val的值.
 * @param  index Dying_gasp 的索引.
 * @param  trim_sel Dying_gasp 控制源，当控制源来自寄存器时就会按照寄存器配置执行掉电保护程序，同时用到trim_val进行电压校准，
    当采用EFUSE做为控制源时会从EFUSE中读取配置执行掉电保护 (参考 trim_sel_t).
 * @param  trim_val 当控制源选择为寄存器控制时用于校准电压(单位: mv).
 * @retval ERRCODE_SUCC 成功.
 * @retval Other        失败，参考 @ref errcode_t.
 * @endif
 */
errcode_t uapi_dygp_set_ctrl(uint32_t index, uint32_t trim_sel, uint32_t trim_val);

/**
 * @if Eng
 * @brief  Dying_gasp Sets the high and low levels, High voltage and low voltage are deviations from preset
    voltage thresholds.
 * @param  index The index of Dying_gasp.
 * @param  low_threshold low level value，When the voltage of the board is lower than the value of this parameter,
    an interruption is triggered and power-off protection is performed (type: enum),
    @ref dygp_low_voltage_threshold_t.
 * @param  high_threshold high level value，When the voltage of the board is higher than this value,
    the power-off protection program is enabled (type: enum), @ref dygp_high_voltage_threshold_t.
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Dying_gasp 设置高电平和低电平的值,高压和低压是偏离预设的电压阈值。
 * @param  index Dying_gasp 的索引.
 * @param  low_threshold 低电压值，当单板电压小于这一值时便触发中断，进行掉电保护 (类型: 枚举),
    @ref dygp_low_voltage_threshold_t.
 * @param  high_threshold 高电压值，当单板电压高于这一值时掉电保护程序开启 (类型: 枚举),
        @ref dygp_high_voltage_threshold_t.
 * @retval ERRCODE_SUCC 成功.
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_set_threshold(uint32_t index, uint32_t low_threshold, uint32_t high_threshold);

/**
 * @if Eng
 * @brief  Dying_gasp Setting the filtering glitch enable and setting the interval.
 * @param  index The index of Dying_gasp.
 * @param  enable Glitch filtering enable flag.
 * @param  gap The glitch interval (unit: 0.1us).
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  Dying_gasp 设置过滤毛刺使能，并过滤毛刺间隔。
 * @param  index Dying_gasp 的索引.
 * @param  enable 毛刺间隔.
 * @param  gap 毛刺间隔 (单位: 0.1us).
 * @retval ERRCODE_SUCC 成功.
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_set_filter_and_gap(uint32_t index, uint32_t enable, uint32_t gap);

/**
 * @if Eng
 * @brief  Dying_gasp Enable global reset upon power-off and set the global reset time.
 * @param  index The index of Dying_gasp.
 * @param  delay_time Delay from power-off to chip reset (unit: ms).
 * @retval ERRCODE_SUCC: Success.
 * @retval Other:        Failure. For details, see @ref errcode_t .
 * @else
 * @brief  下电全局复位使能并设置全局复位时间.
 * @param  index Dying_gasp 的索引.
 * @param  delay_time 掉电到全芯片复位的延时 (单位: ms).
 * @retval ERRCODE_SUCC 成功.
 * @retval Other        失败，参考 @ref errcode_t .
 * @endif
 */
errcode_t uapi_dygp_set_time(uint32_t index, uint32_t delay_time);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* DYING_GASP_H */