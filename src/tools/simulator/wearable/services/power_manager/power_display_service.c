/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 * Description: Power display service.
 * Author: HiSilicon
 */

#include "power_display_service.h"

#define DISPLAY_AUTO_POWER_OFF_PERIOD_MS 5000
#define DISPLAY_DEFAULT_BRIGHTNESS 50
#define POWER_DISPLAY_QUEUE_MAX_SIZE 64
#define POWER_DISPLAY_MAX_SIZE 0x10
#define POWER_DISPLAY_MAX_SET_TIME (1000 * 60 * 60)
#define POWER_DISPLAY_ID_LEN sizeof(uint32_t)

typedef errcode_t (*message_handle)(uint32_t message_type, uint32_t message_value);

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

#ifdef SUPPORT_OHOSFWK
void power_display_turnon_handle_ext(void *data);
void power_display_turnoff_handle_ext(void *data);
void power_display_set_screenoff_timeout_handle_ext(void *data);
void power_display_set_screenoff_switch_handle_ext(void *data);
void power_display_set_keepon_timeout_handle_ext(void *data);
void power_display_set_brightness_handle_ext(void *data);
void power_display_gui_cmd_handle_ext(void *data);
void power_display_timer_cmd_handle_ext(void *data);
#endif

static unsigned long g_power_display_queue_id = 0xFFFFFFFF;
static uint32_t g_power_recv_msg_num = 0;
bool g_discard_all_events = false;
static uint32_t g_power_tp_press_num = false;

static unsigned long power_display_get_msg_queue_id(void)
{
    return g_power_display_queue_id;
}

/* 发送消息接口 */
static errcode_t power_display_msg_write(uint32_t msg_id, uint8_t *msg, uint16_t msg_len, bool wait)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_turn_on_screen(void)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_turn_off_screen(void)
{
    return ERRCODE_SUCC;
}

static screen_state_t power_display_get_screen_state(void)
{
    return g_screen_context.current_state;
}

static errcode_t power_display_set_screen_auto_off_timeout(uint32_t timeout)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_auto_timeout_function(bool enable)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_keepon_timeout(uint32_t time)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_set_brightness_mode(brightness_mode_t mode)
{
    return ERRCODE_SUCC;
}

static brightness_mode_t power_display_get_brightness_mode(void)
{
    return g_screen_context.brightness_mode;
}

static errcode_t power_display_set_brightness(uint8_t brightness)
{
    return ERRCODE_SUCC;
}

static uint8_t power_display_get_brightness(void)
{
    return g_screen_context.brightness_value;
}

static errcode_t power_display_register_filter_rule(event_type_t type, execute_rule rule_func)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_key_event_cb(event_callback_func callbackFunc)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_touch_event_cb(event_callback_func callbackFunc)
{
    return ERRCODE_SUCC;
}

static errcode_t power_display_register_rotate_event_cb(event_callback_func callbackFunc)
{
    return ERRCODE_SUCC;
}

static void power_display_key_event_cb(uint32_t event, uint32_t event_value)
{
    return;
}

static void power_display_touch_event_cb(uint32_t event, uint32_t event_value)
{
    return;
}

static errcode_t power_display_timeout_event_proc(uint32_t event)
{
    return ERRCODE_SUCC;
}

static void power_display_save_gui_event_stamp(input_action_t action, event_type_t event_type)
{
    return;
}

screen_context_t *power_display_get_screen_context(void)
{
    return &g_screen_context;
}

static errcode_t power_display_execute_filter_rules(event_type_t event_type, uint32_t event)
{
    return ERRCODE_SUCC;
}

errcode_t power_display_turnon_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_turnon_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_turnoff_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_turnoff_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_set_screenoff_timeout_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_screenoff_timeout_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_set_screenoff_switch_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_screenoff_switch_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_set_keepon_timeout_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_keepon_timeout_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_set_brightness_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_set_brightness_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_set_brightness_mode_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

errcode_t power_display_gui_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_gui_cmd_handle_ext(void *data)
{
    return;
}
#endif

errcode_t power_display_other_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

errcode_t power_display_timer_cmd_handle(uint32_t message_type, uint32_t message_value)
{
    return ERRCODE_SUCC;
}

#ifdef SUPPORT_OHOSFWK
void power_display_timer_cmd_handle_ext(void *data)
{
    return;
}
#endif

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
    .register_key_event_cb = power_display_register_key_event_cb,
    .register_touch_event_cb = power_display_register_touch_event_cb,
    .register_rotate_event_cb = power_display_register_rotate_event_cb,
    .screen_timeout_func = power_display_timeout_event_proc,
};

const power_display_svr_api_t *power_display_svr_get_api(void)
{
    return &g_power_display_svr_api;
}

static const message_handle g_power_manager_msg_handler[INVALID_CMD_TYPE] = {
    power_display_turnon_handle,
    power_display_turnoff_handle,
    power_display_set_screenoff_timeout_handle,
    power_display_set_screenoff_switch_handle,
    power_display_set_keepon_timeout_handle,
    power_display_set_brightness_handle,
    power_display_set_brightness_mode_handle,
    power_display_gui_cmd_handle,
    power_display_other_cmd_handle,
    power_display_timer_cmd_handle,
};

static int32_t power_display_msg_proc(uint32_t msg_id, uint8_t *data, uint16_t size)
{
    return ERRCODE_SUCC;
}

errcode_t power_display_msg_thread(void *data)
{
    return ERRCODE_SUCC;
}

errcode_t power_display_task_init(void)
{
    return ERRCODE_SUCC;
}

errcode_t uapi_power_display_init(void)
{
    return ERRCODE_SUCC;
}

void power_display_gui_enable_aod(bool enable)
{
    g_screen_context.enable_aod = enable;
}
