/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL pinctrl \n
 *
 * History: \n
 * 2022-08-29, Create file. \n
 */
#ifndef HAL_PINCTRL_BRANDY_H
#define HAL_PINCTRL_BRANDY_H

#include "hal_pinctrl.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_pinctrl_3322 Pinctrl 3322
 * @ingroup  drivers_hal_pinctrl
 * @{
 */

/**
 * @brief Get configuration functions of pins.
 * @return Configuration functions of pins. see @ref hal_pin_funcs_t
 */
hal_pin_funcs_t *hal_pin_3322_funcs_get(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif