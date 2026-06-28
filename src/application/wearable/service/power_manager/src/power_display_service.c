/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Power display service.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "cmsis_os2.h"
#include "common_def.h"
#include "systick.h"
#include "display_filter.h"
#include "input_manager.h"
#include "soc_lcd_api.h"
#include "touch_def.h"
#include "touch_feature.h"
#include "securec.h"
#include "input_feature.h"
#include "display_state_machine.h"
#include "graphic_service_wrapper.h"

#define DISPLAY_AUTO_POWER_OFF_PERIOD_MS 5000
#define DISPLAY_DEFAULT_BRIGHTNESS 50
#define POWER_DISPLAY_MAX_SET_TIME (1000 * 60 * 60)
#define POWER_DISPLAY_ONE_MINUTE (1000 * 60)

uint32_t button_register_report_event_func(app_notify_service_t service_type, input_report_event_cb callback);

/**
 * @brief post data定义
 */
typedef struct pms_post_data {
    uint32_t type;
    uint32_t event;
} pms_post_data_t;

static screen_context_t g_screen_context = {
    .screen_off_timeout = DISPLAY_AUTO_POWER_OFF_PERIOD_MS,
    .keep_on_timeout = 0,
    .current_state = SCREEN_OFF,
    .brightness_mode = DISPLAY_DEFAULT_BRIGHTNESS,
    .brightness_value = 50,
    .time_out_enable = true,
    .last_gui_event_stamp = 0,
    .press_state = 0,
    .enable_aod = false,
};

static uint32_t g_power_tp_press_num = false;
event_filter_t *g_event_filter_map[EVENT_TYPE_SIZE] = { NULL };

static void power_display_save_gui_event_stamp(input_action_t action, event_type_t event_type)
{
    UNUSED(event_type);
    if ((action == TURN_ON_SCREEN) && (event_type == GUI_EVENT)) {
        g_screen_context.last_gui_event_stamp = uapi_systick_get_ms();
    }
}
static void power_display_execute_filter_rules_handle(void *data)
{
    pms_post_data_t *pms_data = (pms_post_data_t *)data;
    event_type_t event_type = pms_data->type;
    uint32_t event = pms_data->event;
    free(data);
    if (event_type >= EVENT_TYPE_SIZE) {
        return;
    }
    POWER_DISPLAY_PRINT_DEBUG(0, "type:%u, event:%u.", event_type, event);
    event_filter_t *filter = g_event_filter_map[event_type];
    screen_context_t *sc = power_display_get_screen_context();
    input_action_t ac = KEEP_CURRENT_STATE;
    if (filter == NULL) {
        return;
    }
    if (filter->system_rule != NULL) {
        ac = filter->system_rule(sc, event);
    }
    if (ac == KEEP_CURRENT_STATE) {
        return;
    }
    power_display_save_gui_event_stamp(ac, event_type);
    power_display_execute_state_change_action(event_type, ac, event);
    return;
}

static errcode_t power_display_execute_filter_rules(event_type_t event_type, uint32_t event)
{
    pms_post_data_t *data = malloc(sizeof(pms_post_data_t));
    if (data != NULL) {
        data->type = event_type;
        data->event = event;
        PostGraphicEvent(power_display_execute_filter_rules_handle, data);
        return ERRCODE_SUCC;
    }
    return ERRCODE_FAIL;
}

static errcode_t power_display_turn_on_screen(void)
{
    uint32_t keyid = FAKE;
    keyid += KEY_SHORT_PRESS;
    return power_display_execute_filter_rules(GUI_EVENT, keyid);
}

static errcode_t power_display_turn_off_screen(void)
{
    uint32_t keyid = FAKE;
    keyid += COVER_SCREEN;
    return power_display_execute_filter_rules(GUI_EVENT, keyid);
}

static screen_state_t power_display_get_screen_state(void)
{
    return g_screen_context.current_state;
}

static errcode_t power_display_set_screen_auto_off_timeout(uint32_t timeout)
{
    if (g_screen_context.time_out_enable == false) {
        return ERRCODE_FAIL;
    }
    if ((timeout < DISPLAY_AUTO_POWER_OFF_PERIOD_MS) || (timeout > POWER_DISPLAY_MAX_SET_TIME)) {
        POWER_DISPLAY_PRINT_ERR(0, "Setting the screen timeout needs to be within this range[%u(s),%u(min)].\r\n ",
            DISPLAY_AUTO_POWER_OFF_PERIOD_MS, POWER_DISPLAY_MAX_SET_TIME / POWER_DISPLAY_ONE_MINUTE);
        return ERRCODE_FAIL;
    }
    g_screen_context.screen_off_timeout = timeout;
    return power_display_execute_filter_rules(TIMER_EVENT, SET_DEFAULT_TIMEOUT);
}

static errcode_t power_display_set_auto_timeout_function(bool enable)
{
    g_screen_context.time_out_enable = enable;
    if (enable) {
        return power_display_execute_filter_rules(TIMER_EVENT, SET_DEFAULT_TIMEOUT);
    }
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_keepon_timeout(uint32_t time)
{
    if (g_screen_context.keep_on_timeout != 0 && time != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "current has been set %u,need to wait for timeout or set 0 to cancel. \r\n",
            g_screen_context.keep_on_timeout);
        return ERRCODE_FAIL;
    }
    uint32_t real_time = time > POWER_DISPLAY_MAX_SET_TIME ? POWER_DISPLAY_MAX_SET_TIME : time;
    g_screen_context.keep_on_timeout = real_time;
    return power_display_execute_filter_rules(TIMER_EVENT, SET_KEEP_ON_TIMEOUT);
}

static errcode_t power_display_set_brightness_mode(brightness_mode_t mode)
{
    g_screen_context.brightness_mode = mode;
    /* 当前没有适配环境光，不支持自动调节 */
    return ERRCODE_SUCC;
}

static brightness_mode_t power_display_get_brightness_mode(void)
{
    return g_screen_context.brightness_mode;
}

static errcode_t power_display_set_brightness(uint16_t brightness)
{
    ext_errno ret = uapi_lcd_set_brightness(brightness);
    if (ret != 0) {
        POWER_DISPLAY_PRINT_ERR(0, "power_display_set_brightness fail, ret:%u.", ret);
        return ERRCODE_FAIL;
    }
    g_screen_context.brightness_value = brightness;
    return ERRCODE_SUCC;
}

static uint16_t power_display_get_brightness(void)
{
    return g_screen_context.brightness_value;
}

static errcode_t power_display_register_filter_rule(event_type_t type, execute_rule rule_func)
{
    if (type >= EVENT_TYPE_SIZE) {
        return ERRCODE_FAIL;
    }
    if (g_event_filter_map[type] == NULL) {
        return ERRCODE_FAIL;
    }
    g_event_filter_map[type]->product_rule = rule_func;
    return ERRCODE_SUCC;
}

static errcode_t power_display_timeout_event_proc(uint32_t event)
{
    if (event == KEEP_ON_TIMER_TIME_OUT) {
        g_screen_context.keep_on_timeout = 0;
    }
    return power_display_execute_filter_rules(TIMER_EVENT, event);
}

screen_context_t *power_display_get_screen_context(void)
{
    return &g_screen_context;
}

static const power_display_svr_api_t g_power_display_svr_api = {
    .turn_on_screen = power_display_turn_on_screen,
    .turn_off_screen = power_display_turn_off_screen,
    .get_screen_state = power_display_get_screen_state,
    .set_screen_auto_off_timeout = power_display_set_screen_auto_off_timeout,
    .set_auto_timeout_function = power_display_set_auto_timeout_function,
    .set_screen_set_keepon_timeout = power_display_set_keepon_timeout,
    .set_brightness_mode = power_display_set_brightness_mode,
    .get_brightness_mode = power_display_get_brightness_mode,
    .set_brightness = power_display_set_brightness,
    .get_brightness = power_display_get_brightness,
    .register_filter_rule = power_display_register_filter_rule,
    .register_key_event_cb = NULL,
    .register_touch_event_cb = NULL,
    .register_rotate_event_cb = NULL,
    .screen_timeout_func = power_display_timeout_event_proc,
};

const power_display_svr_api_t *power_display_svr_get_api(void)
{
    return &g_power_display_svr_api;
}

static void power_display_touch_event_cb(input_dev_report_data_t *data)
{
    uint32_t event_info = TOUCH;
    if (data->state == MC_TP_PRESS || data->state == MC_TP_MOVE) {
        event_info += MC_TP_PRESS;
        g_power_tp_press_num++;
    } else if (data->state == MC_TP_RELEASE) {
        event_info += MC_TP_RELEASE;
        g_power_tp_press_num = 0;
    } else {
        return;
    }
    if (g_power_tp_press_num > 1) {
        return;
    }
    power_display_execute_filter_rules(GUI_EVENT, event_info);
}

static void power_display_key_event_cb(input_dev_report_data_t *data)
{
    UNUSED(data);
    power_display_execute_filter_rules(GUI_EVENT, KEY);
}

errcode_t uapi_power_display_init(void)
{
    g_event_filter_map[GUI_EVENT] = display_get_gui_filter();
    g_event_filter_map[TIMER_EVENT] = display_get_timer_filter();
    g_event_filter_map[SLICE_EVENT] = display_get_slice_filter();
    touch_register_report_event_cb(APP_PMS, power_display_touch_event_cb);
    button_register_report_event_func(APP_PMS, power_display_key_event_cb);
    return ERRCODE_SUCC;
}

void power_display_gui_enable_aod(bool enable)
{
    g_screen_context.enable_aod = enable;
}
