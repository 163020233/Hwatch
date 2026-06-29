/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 HAL watchdog \n
 *
 * History: \n
 * 2022-11-26, Create file. \n
 */
#ifndef HAL_WATCHDOG_V151_H
#define HAL_WATCHDOG_V151_H

#include "hal_watchdog.h"
#include "hal_watchdog_v151_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_watchdog_v151 Watchdog V151
 * @ingroup  drivers_hal_watchdog
 * @{
 */

/**
 * @if Eng
 * @brief  Get functions of the watchdog v151.
 * @else
 * @brief  获取watchdog v151 的实例
 * @endif
 */
hal_watchdog_funcs_t *hal_watchdog_v151_funcs_get(void);

/**
 * @if Eng
 * @brief  Handler of the watchdog interrput request.
 * @param  [in]  param request param
 * @else
 * @brief  WATCHDOG中断处理函数
 * @param  [in]  param 中断请求的参数
 * @endif
 */
void hal_watchdog_v151_irq_handler(uintptr_t param);

void hal_watchdog_v151_kick(void);

void hal_watchdog_v151_disable(void);

errcode_t hal_watchdog_v151_init(void);

void hal_watchdog_v151_deinit(void);

errcode_t hal_watchdog_v151_set_attr(uint32_t timeout);

uint32_t hal_watchdog_v151_get_attr(void);

void hal_watchdog_v151_enable(hal_wdt_mode_t mode);

uint32_t hal_watchdog_v151_get_left_time(void);

void hal_register_watchdog_v151_callback(hal_watchdog_callback_t callback);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
