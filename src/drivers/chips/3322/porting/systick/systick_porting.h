/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick port template \n
 *
 * History: \n
 * 2022-07-30， Create file. \n
 */
#ifndef SYSTICK_PORTING_H
#define SYSTICK_PORTING_H

#include <stdint.h>
#include "platform_core.h"
#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_systick Systick
 * @ingroup  drivers_port
 * @{
 */

#define SYSTICK_SPEED               32768

#define ms_to_systick(ms)           ((((uint64_t)(ms) * SYSTICK_SPEED) / 1000) + 1)

#define HAL_SYSTICK_SPEED           SYSTICK_SPEED
#define MS_PER_S                    1000
#define US_PER_MS                   1000

#ifdef CONFIG_SYSTICK_TEST_HIGHPRECISION
#define SYSTICK_ERR_PPM    ((double)200 / (double)1000000)
#define SYSTICK_USERR_INTF 70
#endif

#if defined(CONFIG_ENABLE_RC_CALIBRATION)
uint64_t convert_s_2_count(uint64_t s);
uint64_t convert_count_2_s(uint64_t count);
uint64_t convert_ms_2_count(uint64_t ms);
uint64_t convert_count_2_ms(uint64_t count);
uint64_t convert_us_2_count(uint64_t us);
uint64_t convert_count_2_us(uint64_t count);
uint64_t systick_get_time_us(void);
#else
#define convert_s_2_count(s)        ((s) * HAL_SYSTICK_SPEED)
#define convert_count_2_s(count)    ((count) / HAL_SYSTICK_SPEED)
#define convert_ms_2_count(ms)      ((ms) * HAL_SYSTICK_SPEED / MS_PER_S)
#define convert_count_2_ms(count)   ((count) * MS_PER_S / HAL_SYSTICK_SPEED)
#define convert_us_2_count(us)      ((us) * HAL_SYSTICK_SPEED / (MS_PER_S * US_PER_MS))
#define convert_count_2_us(count)   ((count) * (MS_PER_S * US_PER_MS) / HAL_SYSTICK_SPEED)
#endif

/**
 * @if Eng
 * @brief  Get the base address of a specified systick.
 * @return The base address of specified systick.
 * @else
 * @brief  获取指定SYSTICK的基地址。
 * @return 指定SYSTICK的基地址。
 * @endif
 */
uintptr_t systick_porting_base_addr_get(void);

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_systick module.
 * @else
 * @brief  将hal funcs对象注册到hal_systick模块中
 * @endif
 */
void systick_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_systick module.
 * @else
 * @brief  从hal_systick模块注销hal funcs对象
 * @endif
 */
void systick_port_unregister_hal_funcs(void);

/**
 * @if Eng
 * @brief  Cali XCLK.
 * @else
 * @brief  修正XCLK时钟
 * @endif
 */
void systick_port_cali_xclk(void);

errcode_t uapi_ulp_systick_count_clear(void);

uint64_t uapi_ulp_systick_get_count(void);

uint64_t uapi_ulp_systick_get_ms(void);
uint64_t uapi_ulp_systick_get_us(void);

void systick_cail_update(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
