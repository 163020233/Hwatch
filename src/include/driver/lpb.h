/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 *
 * Description: Provides LPB driver api \n
 *
 * History: \n
 * 2025-03-25, Create file. \n
 */
#ifndef LPB_H
#define LPB_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "hal_lpb.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_driver_lpb Lpb
 * @ingroup  drivers_driver
 * @{
 */

/**
 * @if Eng
 * @brief  Initialize the Lpb.
 * @else
 * @brief  初始化Lpb。
 * @endif
 */
void uapi_lpb_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the LPB.
 * @return void
 * @else
 * @brief  去初始化LPB。
 * @return void
 * @endif
 */
void uapi_lpb_deinit(void);

/**
 * @if Eng
 * @brief  Connect a trigger to a function by LPB.
 * @param  [in] trigger Trigger type. For details, see @ref lpb_trigger_t.
 * @param  [in] func Function to be connected. For details, see @ref lpb_func_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  通过LPB将触发器与功能连接。
 * @param  [in] trigger 触发器类型，参考 @ref lpb_trigger_t。
 * @param  [in] func 需要连接的功能，参考 @ref lpb_func_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_connect_trig_func(lpb_trigger_t trigger, lpb_func_t func);

/**
 * @if Eng
 * @brief  Disconnect a trigger and a function by LPB.
 * @param  [in] func Function to be disconnected. For details, see @ref lpb_func_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  断开触发器和功能的LPB连接。
 * @param  [in] func 需要断开连接的功能，参考 @ref lpb_func_t。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_disconnect_trig_func(lpb_func_t func);

/**
 * @if Eng
 * @brief  Stop the LPB monitoring associated with the specified identifier.
 * @param  [in] id Monitor ID. For details, see @ref lpb_monitor_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  LPB停止指定标识符的监控。
 * @param  [in] id 监控模块ID， 可选0或者1。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_stop_ch_monitor(lpb_monitor_t id);

/**
 * @if Eng
 * @brief  Get the count when the LPB function is triggered.
 * @param  [in] id Monitor ID. For details, see @ref lpb_monitor_t.
 * @return uint16_t\tThe number of monitored channels.
 * @else
 * @brief  获取LPB功能被触发的计数值。
 * @param  [in] id 监控模块ID， 可选0或者1。
 * @return uint16_t\t已监控通道数量。
 * @endif
 */
uint16_t uapi_lpb_get_monited_ch_cnt(lpb_monitor_t id);

/**
 * @if Eng
 * @brief  Clear the count from the monitored LPB channel.
 * @param  [in] id Monitor ID. For details, see @ref lpb_monitor_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  清除被监控的LPB通道计数值。
 * @param  [in] id 监控模块ID， 可选0或者1。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_clr_monited_ch_cnt(lpb_monitor_t id);

/**
 * @if Eng
 * @brief  Start monitoring the times of the triggered LPB function.
 * @param  [in] func Function to be connected. For details, see @ref lpb_func_t.
 * @param  [in] id Monitor ID. For details, see @ref lpb_monitor_t.
 * @param  [in] threshold Threshold value for monitoring.
 * @param  [in] callback When the callback is not NULL, the monitoring callback will be invoked to notify the user
           whenever the number of times func is triggered equals the threshold. When the callback is NULL, the
           threshold configuration does not take effect, and the function only enables the counting feature.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  启动监控LPB功能被触发次数。
 * @param  [in] func 需要连接的功能，参考 @ref lpb_func_t。
 * @param  [in] id 监控模块ID， 可选0或者1。
 * @param  [in] threshold 监控阈值。
 * @param  [in] callback callback不为NULL时，每当func被触发的次数等于阈值，将调用监控回调callback通知用户；当callback为NULL
           时，threshold配置不生效，该函数仅打开计数功能。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_start_monitor(lpb_func_t func, lpb_monitor_t id, uint16_t threshold,
    lpb_report_callback_t callback);

/**
 * @if Eng
 * @brief  Initialize the LPB peripheral.
 * @param  [in] per_id Peripheral ID to initialize. For details, see @ref lpb_per_id_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  初始化LPB外设。
 * @param  [in] per_id 需要初始化的外设ID，参考 @ref lpb_per_id_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_peripheral_init(lpb_per_id_t per_id);

/**
 * @if Eng
 * @brief  Deinitialize the LPB peripheral.
 * @param  [in] per_id Peripheral ID to deinitialize. For details, see @ref lpb_per_id_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  去初始化外设。
 * @param  [in] per_id 需要去初始化的外设ID，参考 @ref lpb_per_id_t
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_peripheral_deinit(lpb_per_id_t per_id);

/**
 * @if Eng
 * @brief  Configure the function of LPB peripheral.
 * @param  [in] event Event type for configuration. For details, see @ref per_type_t.
 * @param  [in] params Address of the config params.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  LPB外设功能配置。
 * @param  [in] event 配置的事件类型，参考 @ref per_type_t。
 * @param  [in] params 配置参数的地址。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t。
 * @endif
 */
errcode_t uapi_lpb_peripheral_config(per_type_t event, uintptr_t params);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif