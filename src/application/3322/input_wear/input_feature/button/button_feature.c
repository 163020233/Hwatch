/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: button application source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-05-08, Create file. \n
 */
#include "input_feature.h"
#include "button.h"
#include "soc_diag_util.h"
#include "cmsis_os2.h"
#include "pm_definition.h"
#include "tcxo.h"
#include "button_feature.h"
#if defined(ENABLE_UIKIT)
#include "input_manager.h"
#endif
#include "soc_lcd_api.h"

/**
 * @if Eng
 * @brief  Button state enum.
 * @else
 * @brief  按键状态机枚举
 * @endif
 */
typedef enum {
    BUTTON_STATE_NONE = 0, /* !< @if Eng Default state.
                                @else   默认状态  @endif */
    BUTTON_PRESS_STATUS,   /* !< @if Eng Pressed state.
                                @else   按下状态  @endif */
    BUTTON_RELEASE_STATUS, /* !< @if Eng Released state.
                               @else   松开状态  @endif */
    BUTTON_LONGPRESS_1S_STATUS,
    BUTTON_LONGPRESS_2S_STATUS,
    BUTTON_LONGPRESS_3S_STATUS,
    BUTTON_LONGPRESS_4S_STATUS,
    BUTTON_LONGPRESS_5S_STATUS,
    BUTTON_LONGPRESS_6S_STATUS,
    BUTTON_LONGPRESS_7S_STATUS,
    BUTTON_LONGPRESS_8S_STATUS,
    BUTTON_LONGPRESS_9S_STATUS,
} button_state_t;

/**
 * @if Eng
 * @brief  Button report state enum.
 * @else
 * @brief  按键上报状态机枚举
 * @endif
 */
typedef enum {
    BUTTON_PRESS,
    BUTTON_CLICK,               /* !< @if Eng Click one time.
                                     @else   按下一次  @endif */
    BUTTON_DOUBLE_CLICK,        /* !< @if Eng Click double time.
                                     @else   连续按下两次  @endif */
    BUTTON_OTHER_CLICK,         /* !< @if Eng Click other time.
                                     @else   其他连按次数  @endif */
    BUTTON_CLICK_TIMES_INVALID, /* !< @if Eng Click times invilid.
                                     @else   异常连击次数  @endif */
    BUTTON_LONGPRESS_1S,
    BUTTON_LONGPRESS_2S,
    BUTTON_LONGPRESS_3S,
    BUTTON_LONGPRESS_4S,
    BUTTON_LONGPRESS_5S,
    BUTTON_LONGPRESS_6S,
    BUTTON_LONGPRESS_7S,
    BUTTON_LONGPRESS_8S,
    BUTTON_LONGPRESS_9S,
    BUTTON_LONGPRESS_1S_OVER,
    BUTTON_LONGPRESS_2S_OVER,
    BUTTON_LONGPRESS_3S_OVER,
    BUTTON_LONGPRESS_4S_OVER,
    BUTTON_LONGPRESS_5S_OVER,
    BUTTON_LONGPRESS_6S_OVER,
    BUTTON_LONGPRESS_7S_OVER,
    BUTTON_LONGPRESS_8S_OVER,
    BUTTON_LONGPRESS_9S_OVER,
    BUTTON_LONGPRESS_INVALID, /* !< @if Eng Click times invilid.
                                   @else   异常长按时间  @endif */
    BUTTON_STATE_MAX
} button_report_state_t;

typedef struct {
    button_state_t state;
    uint32_t lp_start_time;
    uint32_t click_start_time;
    uint64_t end_time;
    uint32_t click_end_time;
    uint8_t click_cnt;
    bool is_clicked;
} button_app_t;
button_app_t g_button[BUTTON_MAX] = {0};
osTimerId_t g_button_app_timer[BUTTON_MAX] = {0};
button_id_t g_curr_button_id;
button_state_t g_curr_button_state[BUTTON_MAX];
bool g_app_timer_is_running = true;
report_common_event_cb g_button_common_event_cb[BUTTON_BUTT] = {0};
button_report_state_t g_button_report_state_record[BUTTON_MAX] = {0};
button_state_t g_button_state_record[BUTTON_MAX] = {0};
uint32_t g_button_press_time[BUTTON_MAX] = {0};
static uint32_t g_button_init_flag = 0;

button_map_t g_brandy_button_pin_map[BUTTON_PIN_MAP_MAX] = {
    { PIN_NONE, BUTTON_PWR }, // power key位置须固定
    { PIN_NONE, BUTTON_1 },
    { PIN_NONE, BUTTON_2 },
    { PIN_NONE, BUTTON_3 },
    { PIN_NONE, BUTTON_4 },
    { PIN_NONE, BUTTON_5 },
    { PIN_NONE, BUTTON_6 },
    { PIN_NONE, BUTTON_7 },
};

event_callback_func g_button_cb_ar[APP_SERVICE_MAX] = {NULL};

#if defined(ENABLE_UIKIT)
uint32_t button_register_report_event_cb(report_event_cb callback)
{
    return button_register_report_event_func(APP_UI, (input_report_event_cb)callback);
}
#endif

// start for test
uint32_t uapi_button_register_event_cb(report_common_event_cb callback, event_button_t event)
{
    if (g_button_common_event_cb[event] != NULL) {
        return ERRCODE_FAIL;
    }

    if ((callback == NULL) || (event < BUTTON_PWR_PRESS) || (event > BUTTON_BUTT)) {
        return ERRCODE_FAIL;
    }

    g_button_common_event_cb[event] = callback;
    return ERRCODE_SUCC;
}

uint32_t uapi_button_unregister_event_cb(event_button_t event)
{
    if (g_button_common_event_cb[event] == NULL) {
        return ERRCODE_SUCC;
    }

    if ((event < BUTTON_PWR_PRESS) || (event > BUTTON_BUTT)) {
        return ERRCODE_FAIL;
    }

    g_button_common_event_cb[event] = NULL;
    return ERRCODE_SUCC;
}
// end for test

static inline void button_event_cb(app_notify_service_t service, input_dev_report_data_t *data)
{
    if (g_button_cb_ar[service] == NULL) {
        return;
    }
    g_button_cb_ar[service](data);
}

static void button_report_event(event_button_t event)
{
    input_dev_report_data_t data;
    data.type = APP_KEY;
    data.keyId = APP_KEY_POWER;
    switch (event) {
        case BUTTON_PWR_PRESS:
            data.state = SERVICE_PREESS_EVT;
            ext_lcd_status status = uapi_lcd_get_status();
            if (status.disp_status == LCD_DISPLAY_OFF) {
                button_event_cb(APP_PMS, &data);
                return;
            }
            button_event_cb(APP_PMS, &data);
            button_event_cb(APP_UI, &data);
            break;
        case BUTTON_PWR_CLICKUP:
            data.state = SERVICE_RELEASE_EVT;
            button_event_cb(APP_UI, &data);
            break;
        case BUTTON_PWR_LONGPRESS_3S:
            data.state = SERVICE_LONGPRESS_EVT;
            button_event_cb(APP_UI, &data);
            break;
        default:
            break;
    }
    return;
}

static void button_send_msg_to_app(uint8_t id, button_press_state_t state)
{
    button_task_mail_t mail = { 0 };
    if (state == PRESSED) {
        mail.notify.comm.event = BUTTON_DRIVER_EVENT_PRESS;
    } else if (state == RELEASED) {
        mail.notify.comm.event = BUTTON_DRIVER_EVENT_RELEASE;
    }

    mail.notify.data[0] = (uint32_t)id;
    mail.notify.comm.app = APP_BUTTON;
    if (osal_msg_queue_write_copy(get_app_queue_id(), (void *)&mail, sizeof(common_notify_mail_t), 0) != OSAL_SUCCESS) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "[button][driver] button state %d send failed\n", (uint32_t)state);
    }
}

static button_state_t button_get_state(button_id_t id)
{
    return g_button[id].state;
}

static void button_set_state(button_id_t id, button_state_t state)
{
    g_button[id].state = state;
}

static event_button_t button_event_get(button_id_t id, button_report_state_t state)
{
    event_button_t event = BUTTON_PWR_START;
    if (id == BUTTON_1) {
        event = BUTTON1_START;
    }

    if (id == BUTTON_2) {
        event = BUTTON2_START;
    }

    if ((state == BUTTON_PRESS) || (state == BUTTON_CLICK) || (state == BUTTON_DOUBLE_CLICK)) {
        return (event + state + 1);
    } else {
        return (event + state - 1);
    }
}

static void button_send_event_to_service(button_id_t id, button_report_state_t state)
{
    cmd_msg_t msg;
    event_button_t event = BUTTON_BUTT;
    event = button_event_get(id, state);
    button_report_event(event);
    INPUT_FEATURE_PRINT_DEBUG(0, "event:%u.  id:%d ", event, id);
    if (g_button_common_event_cb[event] != NULL) {
        g_button_common_event_cb[event]();
    }
}

static void button_report_long_press_over_state(button_id_t id, button_state_t state)
{
    unused(id);
    switch (state) {
        case BUTTON_LONGPRESS_1S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_1S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_2S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_2S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_3S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_3S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_4S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_4S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_5S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_5S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_6S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_6S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_7S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_7S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_8S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_8S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        case BUTTON_LONGPRESS_9S_STATUS:
            button_send_event_to_service(id, BUTTON_LONGPRESS_9S_OVER);
            button_set_state(id, BUTTON_STATE_NONE);
            break;
        default:
            break;
    }
}

static void button_report_click_state(button_id_t id)
{
    if (g_curr_button_state[id] == BUTTON_RELEASE_STATUS && button_get_state(id) == BUTTON_RELEASE_STATUS) {
        if (g_button[id].is_clicked == false) {
            /* The first click, reset all statistics */
            g_button[id].is_clicked = true;
            g_button[id].click_cnt = 0;
            g_button[id].click_start_time = g_button[id].end_time;
        } else if (g_button[id].is_clicked == true) {
            /* continue click */
            g_button[id].click_cnt++;
            g_button[id].click_start_time = g_button[id].end_time;
        }
        /* click invilid */
        if (g_button[id].click_cnt + 1 > BUTTON_INVILID_CLICK_TIMES) {
#if (CONFIG_SUPPORT_BUTTON_DOUBLE_CLICK == YES)
            button_send_event_to_service(id, BUTTON_DOUBLE_CLICK);
#else
            button_send_event_to_service(id, BUTTON_CLICK);
#endif
            osTimerStop(g_button_app_timer[id]);
            g_button[id].is_clicked = 0;
            g_button[id].click_cnt = 0;
            button_set_state(id, BUTTON_STATE_NONE);
        }
    }
    /* report click */
    if (button_get_state(id) == BUTTON_RELEASE_STATUS) {
        if (g_button[id].end_time - g_button[id].click_start_time >= BUTTON_CLICK_THRESHOLD) {
            /* click more than two times */
            if (g_button[id].click_cnt + 1 >= BUTTON_DEFAULT_MAX_CLICK_TIMES) {
#if (CONFIG_SUPPORT_BUTTON_DOUBLE_CLICK == YES)
                button_send_event_to_service(id, BUTTON_DOUBLE_CLICK);
#else
                button_send_event_to_service(id, BUTTON_CLICK);
#endif
            } else {
                button_send_event_to_service(id, BUTTON_CLICK);
            }
            g_button[id].is_clicked = 0;
            g_button[id].click_cnt = 0;
            button_set_state(id, BUTTON_STATE_NONE);
            osTimerStop(g_button_app_timer[id]);
        }
    }
}

static void button_released_state_handler(button_id_t id)
{
    if (g_curr_button_state[id] == BUTTON_PRESS_STATUS) {
        return;
    }
    if (g_curr_button_state[id] == BUTTON_RELEASE_STATUS) {
        button_report_long_press_over_state(id, button_get_state(id));
        if (button_get_state(id) == BUTTON_STATE_NONE) {
            osTimerStop(g_button_app_timer[id]);
        }
        g_button[id].lp_start_time = g_button[id].end_time;
        if (button_get_state(id) == BUTTON_PRESS_STATUS) {
            button_set_state(id, BUTTON_RELEASE_STATUS);
        }
        g_button_report_state_record[id] = 0;
        g_button_state_record[id] = 0;
        g_button_press_time[id] = 0;
    }
    button_report_click_state(id);
}

static void button_pressed_state_handler(button_id_t id)
{
    if (g_curr_button_state[id] == BUTTON_RELEASE_STATUS) {
        return;
    }
    if (g_curr_button_state[id] == BUTTON_PRESS_STATUS) {
        g_button[id].lp_start_time = g_button[id].end_time;
        if (button_get_state(id) == BUTTON_STATE_NONE || button_get_state(id) == BUTTON_RELEASE_STATUS) {
            button_set_state(id, BUTTON_PRESS_STATUS);
        }
    }
    if (button_get_state(id) == BUTTON_PRESS_STATUS) {
        /* PRESS after PRESS must be long press */
        if (g_button[id].end_time - g_button[id].lp_start_time >= BUTTON_PRESS_TIME_1S) {
            button_send_event_to_service(id, BUTTON_LONGPRESS_1S);
            button_set_state(id, BUTTON_LONGPRESS_1S_STATUS);
            g_button_report_state_record[id] = BUTTON_LONGPRESS_1S;
            g_button_state_record[id] = BUTTON_LONGPRESS_1S_STATUS;
            g_button_press_time[id] = BUTTON_PRESS_TIME_1S + BUTTON_PRESS_TIME_1S;
        }
    }
    if ((button_get_state(id) == g_button_state_record[id]) && (button_get_state(id) > BUTTON_RELEASE_STATUS)) {
        if ((g_button[id].end_time - g_button[id].lp_start_time >= g_button_press_time[id]) &&
            (g_button[id].end_time - g_button[id].lp_start_time <= BUTTON_INVALID_PRESS_TIME)) {
            g_button_state_record[id] = g_button_state_record[id] + 1;
            g_button_press_time[id] = g_button_press_time[id] + BUTTON_PRESS_TIME_1S;
            button_send_event_to_service(id, ++g_button_report_state_record[id]);
            button_set_state(id, button_get_state(id) + 1);
        }
    }
    if ((g_button[id].end_time - g_button[id].lp_start_time > BUTTON_INVALID_PRESS_TIME) &&
        (g_curr_button_state[id] == BUTTON_PRESS_STATUS)) {
        osTimerStop(g_button_app_timer[id]);
    }
}

static void button_process(button_id_t id)
{
    g_button[id].end_time = uapi_tcxo_get_ms();
    button_released_state_handler(id);
    button_pressed_state_handler(id);
    g_curr_button_state[id] = BUTTON_STATE_NONE;
}

static void button_app_timer_func(button_id_t id)
{
    button_process(id);
}

uint32_t button_register_report_event_func(app_notify_service_t service_type, input_report_event_cb callback)
{
    if (callback == NULL || service_type >= APP_SERVICE_MAX) {
        return ERRCODE_FAIL;
    }
    g_button_cb_ar[service_type] = callback;
    return ERRCODE_SUCC;
}

uint32_t button_unregister_report_event_func(app_notify_service_t service_type)
{
    if (service_type >= APP_SERVICE_MAX) {
        return ERRCODE_FAIL;
    }
    g_button_cb_ar[service_type] = NULL;
    return ERRCODE_SUCC;
}

static void button_driver_event_handler(common_notify_mail_t *mptr)
{
    g_curr_button_id = (button_id_t)(mptr->data[0]);
    button_id_t id = g_curr_button_id;
    INPUT_FEATURE_PRINT_DEBUG(APP_BUTTON, "[button] event:%u. id:%d", (uint32_t)mptr->comm.event, id);
    switch (mptr->comm.event) {
        case BUTTON_DRIVER_EVENT_PRESS:
            button_send_event_to_service(id, BUTTON_PRESS);
            g_curr_button_state[id] = BUTTON_PRESS_STATUS;
            break;
        case BUTTON_DRIVER_EVENT_RELEASE:
            g_curr_button_state[id] = BUTTON_RELEASE_STATUS;
            break;
        default:
            break;
    }
    button_process(id);
    if (mptr->comm.event == BUTTON_DRIVER_EVENT_PRESS) {
        osTimerStart(g_button_app_timer[id], BUTTON_APP_TIMER_INTERVAL);
    }

    return;
}

uint32_t button_init(void)
{
    if (g_button_init_flag) {
        return ERRCODE_SUCC;
    }
    input_register_service_cb(APP_BUTTON, button_driver_event_handler);
    uapi_button_register(g_brandy_button_pin_map);
    uapi_button_init();
    uapi_button_send_msg_register(button_send_msg_to_app);

    for (uint8_t i = 0; i < BUTTON_PIN_MAP_MAX; i++) {
        if ((g_brandy_button_pin_map[i].button == BUTTON_PWR) || (g_brandy_button_pin_map[i].pin != PIN_NONE)) {
            g_button_app_timer[i] = osTimerNew((osTimerFunc_t)button_app_timer_func,
                                               osTimerPeriodic,
                                               (void*)(uintptr_t)i,
                                               NULL);
        }
    }

    g_button_init_flag = 1;
    return ERRCODE_SUCC;
}

static uint32_t button_deinit(void)
{
    if (g_button_init_flag == 0) {
        return ERRCODE_SUCC;
    }
    g_button_init_flag = 0;

    for (uint8_t i = 0; i < BUTTON_PIN_MAP_MAX; i++) {
        if ((g_brandy_button_pin_map[i].button == BUTTON_PWR) || (g_brandy_button_pin_map[i].pin != PIN_NONE)) {
            osTimerDelete(g_button_app_timer[i]);
        }
    }

    uapi_button_deinit();
    return ERRCODE_SUCC;
}

static input_dev_info_t g_button_feature_info = {
    .dev_name = "Button",
    .dev_type = APP_BUTTON,
    .dev_ops = {
        .input_dev_init = button_init,
        .input_dev_deinit = button_deinit,
        .input_dev_register_service_cb = button_register_report_event_func,
        .input_dev_unregister_service_cb = button_unregister_report_event_func,
            },
};
