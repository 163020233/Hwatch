/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux led config \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-21， Create file. \n
 */

#ifndef UX_CUST_LED_CONFIG_H
#define UX_CUST_LED_CONFIG_H

#include "stdint.h"
#include "stdbool.h"
#include "platform_core.h"
#include "ux_api_led.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LED_TYPE_CONTROL_LED_MAX_NUM 2 /* 单个灯语最多控制的灯数量 限制只有交替闪才可控多灯 */
#define LED_LIGHT_ALWAYS  0xFFFFFFFF
#define LED_SIGNLE                             1
#define LED_DOUBLE                             2

/* LED是否处于交替闪模式 */
typedef enum {
    UX_LED_ALTERNATE_BLINK_OFF = 0,
    UX_LED_ALTERNATE_BLINK_ON,

    UX_LED_ALTERNATE_BLINK_MAX,
} ux_led_alternate_blink_mode_t;

/* LED是否处于呼吸模式 */
typedef enum {
    UX_LED_BREATHING_MODE_OFF = 0,
    UX_LED_BREATHING_MODE_ON,
} ux_led_breathing_mode_t;

/* LED是否使能 */
typedef enum {
    UX_LED_DISABLE = 0,
    UX_LED_ENABLE = 1,
    UX_LED_ENABLE_MAX
} ux_led_enable_state_t;

/* LED是否使能 */
typedef enum {
#if defined(MELODY_DEMO_SOUND_BOX)
    UX_PWM_LED_CHANNEL_1 = 0,
    UX_PWM_LED_CHANNEL_2 = 1,
#else
    UX_PWM_LED_CHANNEL_1 = 2,
    UX_PWM_LED_CHANNEL_2 = 3,
#endif
    UX_PWM_LED_CHANNEL_MAX
} ux_pwm_led_channel_t;

/* LED-GPIO口映射节点定义 */
typedef struct {
    ux_api_led_color_type_t color;
    pin_t pin;
} ux_gpio_led_node_t;

/* LED-PWM口映射节点定义 */
typedef struct {
    ux_api_led_color_type_t color;
    uint8_t channel;
} ux_pwm_led_node_t;

/* LED单灯属性定义 */
typedef struct {
    ux_api_led_color_type_t color; /* 灯颜色 */
    uint32_t on_time;          /* 单次灯亮周期 */
    uint32_t off_time;         /* 单次灯灭周期 */
} ux_led_attribute_t;

/* 灯语操作信息节点定义 */
typedef struct {
    uint8_t cnt;                                           /* 灯语对应led灯的数量 */
    uint16_t type;                                         /* 灯语类型 */
    uint32_t time;                                         /* 灯操作时间 */
    bool enable;                                           /* 灯模式使能 */
    ux_led_alternate_blink_mode_t blink_mode;              /* 交替闪模式 */
    ux_led_breathing_mode_t       breathing_mode;          /* 呼吸模式 */
    ux_led_attribute_t info[LED_TYPE_CONTROL_LED_MAX_NUM]; /* 单灯配置信息 */
} ux_led_type_config_t;

typedef struct {
    /**
    * @brief  获取GPIO-LED-pin口映射表
    * @return ux_gpio_led_node_t* LED-pin口映射结构体，@ref ux_cust_led_config.h
    */
    const ux_gpio_led_node_t* (*get_gpio_map_table)(void);

    /**
    * @brief  获取LED-GPIO映射表长度
    * @return GPIO-LED-pin口映射表长度
    */
    uint8_t (*get_gpio_map_table_len)(void);
    /**
    * @brief  获取PWM-LED-pin口映射表
    * @return ux_pwm_led_node_t* LED-pin口映射结构体，@ref ux_cust_led_config.h
    */
    const ux_pwm_led_node_t* (*get_pwm_map_table)(void);

    /**
    * @brief  获取PWM-LED-pin口映射表长度
    * @return PWM-LED-pin口映射表长度
    */
    uint8_t (*get_pwm_map_table_len)(void);

    /**
    * @brief  获取LED灯语配置表
    * @return ux_led_type_config_t* 灯语操作信息节点结构体，@ref ux_cust_led_config.h
    */
    const ux_led_type_config_t* (*get_config_table)(void);

    /**
    * @brief  获取LED灯语配置表长度
    * @return LED灯语配置表长度
    */
    uint8_t (*get_config_table_len)(void);
} ux_led_customer_funcs;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif