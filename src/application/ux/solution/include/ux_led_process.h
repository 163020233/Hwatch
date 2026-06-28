/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux led service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-08， Create file. \n
 */
#ifndef UX_LED_PROCESS_H
#define UX_LED_PROCESS_H

#include <stdlib.h>
#include "errcode.h"
#include "ux_errcode.h"
#include "pinctrl_porting.h"
#include "ux_gpio_led.h"
#include "ux_pwm_led.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_api_led_init(void);
errcode_t ux_api_led_deinit(void);
void ux_led_process_init(void);
errcode_t ux_led_process_deinit(void);
errcode_t ux_led_process_led_type(uint8_t flag, uint16_t led_type);
errcode_t ux_led_on_all_led_type(void);
errcode_t ux_led_off_all_led_type(void);
void ux_api_led_register_cust_inst(ux_led_customer_funcs* instance);
ux_led_customer_funcs* ux_led_customer_get_instance(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif