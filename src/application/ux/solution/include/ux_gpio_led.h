/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux gpio led \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-08， Create file. \n
 */
#ifndef UX_GPIO_LED_H
#define UX_GPIO_LED_H

#include <stdlib.h>
#include "errcode.h"
#include "ux_errcode.h"
#include "pinctrl.h"
#include "ux_cust_led_config.h"
#include "ux_api_led.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

pin_t ux_gpio_led_find_pin_by_color(ux_api_led_color_type_t color);
errcode_t ux_gpio_led_light_on(pin_t pin);
errcode_t ux_gpio_led_light_off(pin_t pin);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif