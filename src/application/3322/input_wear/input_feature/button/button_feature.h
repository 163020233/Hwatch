/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: button application header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08, Create file. \n
 */
#ifndef BUTTON_FREATURE_H
#define BUTTON_FREATURE_H

#include "stdint.h"
#include "product.h"

/**
 * @defgroup BUTTON_APP_
 * @ingroup  BUTTON_APP
 * @{
 */

#if (CONFIG_SUPPORT_BUTTON_DOUBLE_CLICK == YES)
#define BUTTON_CLICK_THRESHOLD 400
#else
#define BUTTON_CLICK_THRESHOLD 0
#endif

#define BUTTON_APP_TIMER_INTERVAL 20
#define BUTTON_INVALID_CLICK_TIMES 8
#if (CONFIG_SUPPORT_BUTTON_DOUBLE_CLICK == YES)
#define BUTTON_INVILID_CLICK_TIMES 8
#else
#define BUTTON_INVILID_CLICK_TIMES 0
#endif
#define BUTTON_DEFAULT_MAX_CLICK_TIMES 2
#define BUTTON_PRESS_TIME_1S 1000
#define BUTTON_PRESS_TIME_9S 9000
#define BUTTON_INVALID_PRESS_TIME (BUTTON_PRESS_TIME_9S + BUTTON_APP_TIMER_INTERVAL)

#define TOUCH_VETO_SLEEP_MS 16

typedef enum {
    /* power key */
    BUTTON_PWR_START = 0,
    BUTTON_PWR_PRESS,
    BUTTON_PWR_CLICKUP,
    BUTTON_PWR_CLICKUP_2,
    BUTTON_PWR_LONGPRESS_1S,
    BUTTON_PWR_LONGPRESS_2S,
    BUTTON_PWR_LONGPRESS_3S,
    BUTTON_PWR_LONGPRESS_4S,
    BUTTON_PWR_LONGPRESS_5S,
    BUTTON_PWR_LONGPRESS_6S,
    BUTTON_PWR_LONGPRESS_7S,

    BUTTON_PWR_LONGPRESS_8S,
    BUTTON_PWR_LONGPRESS_9S,
    BUTTON_PWR_LONGPRESS_1S_OVER,
    BUTTON_PWR_LONGPRESS_2S_OVER,
    BUTTON_PWR_LONGPRESS_3S_OVER,
    BUTTON_PWR_LONGPRESS_4S_OVER,
    BUTTON_PWR_LONGPRESS_5S_OVER,
    BUTTON_PWR_LONGPRESS_6S_OVER,
    BUTTON_PWR_LONGPRESS_7S_OVER,
    BUTTON_PWR_LONGPRESS_8S_OVER,

    BUTTON_PWR_LONGPRESS_9S_OVER,
    BUTTON_PWR_INVALID,

    /* function key */
    BUTTON1_START,
    BUTTON1_PRESS,
    BUTTON1_CLICKUP,
    BUTTON1_CLICKUP_2,
    BUTTON1_LONGPRESS_1S,
    BUTTON1_LONGPRESS_2S,
    BUTTON1_LONGPRESS_3S,
    BUTTON1_LONGPRESS_4S,
    BUTTON1_LONGPRESS_5S,
    BUTTON1_LONGPRESS_6S,
    BUTTON1_LONGPRESS_7S,
    BUTTON1_LONGPRESS_8S,
    BUTTON1_LONGPRESS_9S,
    BUTTON1_LONGPRESS_1S_OVER,
    BUTTON1_LONGPRESS_2S_OVER,
    BUTTON1_LONGPRESS_3S_OVER,
    BUTTON1_LONGPRESS_4S_OVER,
    BUTTON1_LONGPRESS_5S_OVER,
    BUTTON1_LONGPRESS_6S_OVER,
    BUTTON1_LONGPRESS_7S_OVER,
    BUTTON1_LONGPRESS_8S_OVER,
    BUTTON1_LONGPRESS_9S_OVER,
    BUTTON1_INVALID,

    /* function key */
    BUTTON2_START,
    BUTTON2_PRESS,
    BUTTON2_CLICKUP,
    BUTTON2_CLICKUP_2,
    BUTTON2_LONGPRESS_1S,
    BUTTON2_LONGPRESS_2S,
    BUTTON2_LONGPRESS_3S,
    BUTTON2_LONGPRESS_4S,
    BUTTON2_LONGPRESS_5S,
    BUTTON2_LONGPRESS_6S,
    BUTTON2_LONGPRESS_7S,
    BUTTON2_LONGPRESS_8S,
    BUTTON2_LONGPRESS_9S,
    BUTTON2_LONGPRESS_1S_OVER,
    BUTTON2_LONGPRESS_2S_OVER,
    BUTTON2_LONGPRESS_3S_OVER,
    BUTTON2_LONGPRESS_4S_OVER,
    BUTTON2_LONGPRESS_5S_OVER,
    BUTTON2_LONGPRESS_6S_OVER,
    BUTTON2_LONGPRESS_7S_OVER,
    BUTTON2_LONGPRESS_8S_OVER,
    BUTTON2_LONGPRESS_9S_OVER,
    BUTTON2_INVALID,

    BUTTON_BUTT
} event_button_t;

typedef enum {
    SERVICE_RELEASE_EVT,
    SERVICE_PREESS_EVT,
    SERVICE_LONGPRESS_EVT,
} service_event_button_t;
/**
 * @if Eng
 * @brief  Button task message.
 * @else
 * @brief  按键任务消息。
 * @endif
 */
typedef struct {
    common_notify_mail_t notify;
} button_task_mail_t;

typedef void (*report_common_event_cb)(void);

typedef enum {
    APP_KEY_POWER,
    APP_KEY_FUNC,
    APP_KEY_UNKNOWN,
} input_dev_key_id_t;

typedef void (*event_callback_func)(input_dev_report_data_t *data);
/**
 * @if Eng
 * @brief  Button driver event.
 * @else
 * @brief  按键驱动事件枚举
 * @endif
 */
enum {
    BUTTON_DRIVER_EVENT_PRESS,
    BUTTON_DRIVER_EVENT_RELEASE,
};

typedef void (*touch_callback)(void);

uint32_t button_app_init(void);
uint64_t get_button_queue_id(void);
uint32_t uapi_button_register_event_cb(report_common_event_cb callback, event_button_t event);
uint32_t uapi_button_unregister_event_cb(event_button_t event);
uint32_t button_register_report_event_func(app_notify_service_t service_type, input_report_event_cb callback);
uint32_t button_unregister_report_event_func(app_notify_service_t service_type);
/**
 * @}
 */

#endif
