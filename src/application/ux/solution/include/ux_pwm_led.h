/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux pwm led \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-08， Create file. \n
 */
#ifndef UX_PWM_LED_H
#define UX_PWM_LED_H

#include <stdlib.h>
#include "errcode.h"
#include "ux_cust_led_config.h"
#include "ux_api_led.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_pwm_led_init(void);
void ux_pwm_led_deinit(void);
errcode_t ux_pwm_led_light_off(uint8_t channel);
errcode_t ux_pwm_led_light_on(uint8_t channel);
uint8_t ux_pwm_led_find_channel_by_color(ux_api_led_color_type_t color);
void ux_pwm_led_set_breathing_mode(uint8_t channel, ux_led_breathing_mode_t breathing_mode);
errcode_t ux_led_set_brightness(ux_api_led_color_type_t color, uint8_t level);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif