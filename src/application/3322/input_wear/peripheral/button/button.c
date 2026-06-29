/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Button driver source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-02-24, Create file. \n
 */

#include "pinctrl.h"
#include "platform_core.h"
#include "gpio.h"
#include "gpio_porting.h"
#include "debug_print.h"
#include "rtc.h"
#include "timer.h"
#include "systick.h"
#include "chip_core_irq.h"
#include "input_feature.h"
#include "pm_veto.h"
#include "pmu_ulp.h"
#include "button.h"

#define BUTTON_GPIO_NUM (BUTTON_PIN_MAP_MAX - 1)
#define BUTTON_PWREN_ID 0

/**
 * @if Eng
 * @brief  Button debounce struct
 * @else
 * @brief  按键去抖动结构体定义
 * @endif
 */
typedef struct {
    uint8_t debounced;       /* !< @if Eng Recording debounced group.
                                  @else   记录是否产生抖动  @endif */
    uint8_t press_flag;      /* !< @if Eng Press flash.
                                  @else   是否press的标志  @endif */
    uint32_t debounced_time; /* !< @if Eng Debounced time.
                                  @else   抖动计时  @endif */
} button_debounced_t;

button_debounced_t g_button_debounced[BUTTON_GPIO_NUM] = {0};
button_map_t g_button_pin_map[BUTTON_PIN_MAP_MAX] = {0};
timer_handle_t g_button_timer = { 0 };
bool g_timer_is_running = false;
timer_handle_t g_gpio_button_timer[BUTTON_GPIO_NUM] = {0};
bool g_gpio_timer_is_running[BUTTON_GPIO_NUM] = {false};
msg_send_callback_t g_msg_send_cb = NULL;
pin_t g_gpio_key_record = 0;
uint8_t g_buttom_vetoed = 0;
static void button_debounced_timer_callback(uint32_t data);
static void button_check_released_timer_callback(uint32_t data);

static const char* g_state_str[PRESSED_MAX] = {
    "BUTTON_PRESSED",
    "BUTTON_RESLASED",
};

static button_press_state_t porting_button_get_state(pin_t pin)
{
    if (pin == PIN_NONE) {
        if (uapi_pmu_ulp_get_pwrkey_press_state()) {
            return PRESSED;
        } else {
            return RELEASED;
        }
    } else {
        if (uapi_gpio_get_val(pin) == GPIO_LEVEL_LOW) {
            return PRESSED;
        } else {
            return RELEASED;
        }
    }
}

static errcode_t porting_button_irq_register(pin_t pin, osal_irq_handler thread_handler)
{
    unused(pin);
    uapi_pmu_ulp_register_irq(ULP_PWR_KEY_PRESS_IRQ, (osal_irq_handler)thread_handler);
    return ERRCODE_SUCC;
}

static errcode_t porting_button_unregister(pin_t pin)
{
    if (pin == PIN_NONE) {
        return ERRCODE_SUCC;
    } else {
        uapi_gpio_unregister_isr_func(pin);
    }
    return ERRCODE_SUCC;
}

static bool porting_button_check_int(pin_t pin)
{
    unused(pin);
    return true;
}

static errcode_t porting_button_clear_int(pin_t pin)
{
    unused(pin);
    return ERRCODE_SUCC;
}


static errcode_t porting_button_pmu_pwr_cfg(pin_t pin)
{
    unused(pin);
    return ERRCODE_SUCC;
}

static void porting_button_add_veto(void)
{
    uapi_pm_add_sleep_veto_with_symmetry(PM_ID_BUTTON);
}

static void porting_button_remove_veto(void)
{
    uapi_pm_remove_sleep_veto_with_symmetry(PM_ID_BUTTON);
}

static button_press_state_t button_get_press_state(pin_t pin)
{
    button_peripheral_api *button_api = button_port_get_api();
    return button_api->button_get_state(pin);
}

static void button_send_msg(button_id_t id, button_press_state_t state)
{
    if (g_msg_send_cb == NULL) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "button %d is %s\r\n", id, g_state_str[state]);
    } else {
        g_msg_send_cb(id, state);
    }
}

static void button_timer_start(uint32_t gpio_index)
{
    bool start = false;
    if (!g_gpio_timer_is_running[gpio_index]) {
        g_gpio_timer_is_running[gpio_index] = true;
        start = true;
    }

    if (start) {
        uapi_rtc_stop(g_gpio_button_timer[gpio_index]);
        uapi_rtc_start(g_gpio_button_timer[gpio_index], BUTTON_TIMER_INTERVAL, button_debounced_timer_callback,
            gpio_index);
    }
}

static void button_timer_start_check_released(void)
{
    bool start = false;
    if (!g_timer_is_running) {
        g_timer_is_running = true;
        start = true;
    }

    if (start) {
        uapi_rtc_stop(g_button_timer);
        uapi_rtc_start(g_button_timer, BUTTON_CHECK_RELEADSED, button_check_released_timer_callback, 0);
    }
}

static void button_debounced_handler(uint32_t data)
{
    pin_t pin = 0;
    button_peripheral_api *button_api = button_port_get_api();
    uint32_t time = (uint32_t)uapi_systick_get_ms();
    uint32_t gpio_index = data;
    uint8_t debounced = g_button_debounced[data].debounced;

    if (debounced > 0) {
        pin = g_button_pin_map[gpio_index + 1].pin;

        if ((button_get_press_state(pin) == PRESSED) && (g_button_debounced[gpio_index].press_flag == 1) &&
            (time - g_button_debounced[gpio_index].debounced_time >= DEBOUNCED_INTERVAL)) {
            /* Debounce */
            g_button_debounced[gpio_index].debounced = 0;
            g_button_debounced[gpio_index].press_flag = 1;
            return;
        }
        if ((button_get_press_state(pin) == RELEASED) && (g_button_debounced[gpio_index].press_flag == 0) &&
            (time - g_button_debounced[gpio_index].debounced_time >= DEBOUNCED_INTERVAL)) {
            /* Debounce */
            g_button_debounced[gpio_index].debounced = 0;
            g_button_debounced[gpio_index].press_flag = 0;
            button_api->button_remove_veto();
            return;
        }
        if ((button_get_press_state(pin) == RELEASED) && (g_button_debounced[gpio_index].press_flag == 1) &&
            (time - g_button_debounced[gpio_index].debounced_time >= INTERFERENCE_INTERVAL)) {
            /* Interference */
            g_button_debounced[gpio_index].debounced = 0;
            g_button_debounced[gpio_index].press_flag = 0;
            button_api->button_remove_veto();
        }
    }
}

static void button_debounced_timer_callback(uint32_t data)
{
    pin_t pin = 0;
    g_gpio_timer_is_running[data] = false;

    button_debounced_handler(data);

    if (g_button_debounced[data].debounced != 0) {
        button_timer_start(data);
    } else {
        if (g_button_debounced[data].press_flag) {
            button_send_msg(data + 1, PRESSED);
        } else {
            button_send_msg(data + 1, RELEASED);
        }
        button_peripheral_api *button_api = button_port_get_api();
        pin = g_button_pin_map[data + 1].pin;
        uapi_gpio_enable_interrupt(pin);
    }
}

static void button_check_released_timer_callback(uint32_t data)
{
    unused(data);
    g_timer_is_running = false;

    button_peripheral_api *button_api = button_port_get_api();
    button_press_state_t press_state;
    press_state = button_get_press_state(PIN_NONE);
    if (press_state == 1) {
        if (g_buttom_vetoed == 1) {
            button_api->button_remove_veto();
            g_buttom_vetoed = 0;
        }
        button_send_msg(BUTTON_PWREN_ID, RELEASED);
    } else {
        button_timer_start_check_released();
    }
}

static void button_irq_callback(pin_t pin)
{
    pin = PIN_NONE;
    // deal with pwren interrupt
    button_peripheral_api *button_api = button_port_get_api();
    button_api->button_clear_interrupt(pin);
    button_press_state_t press_state = button_get_press_state(pin);
    button_send_msg(BUTTON_PWREN_ID, press_state);
    if (g_buttom_vetoed == 0) {
        button_api->button_add_veto();
        g_buttom_vetoed = 1;
    }
    button_timer_start_check_released();
}

static void button_gpio_callback(pin_t pin, uintptr_t param)
{
    unused(param);
    uint8_t bit = 0;
    uint8_t gpio_index = 0;
    for (uint8_t index = 0; index < BUTTON_PIN_MAP_MAX; index++) {
        if (pin == g_button_pin_map[index].pin) {
            bit = index;
            break;
        }
    }

    if (bit <= 0) {
        return;
    }
    gpio_index = bit - 1;
    uapi_gpio_clear_interrupt(pin);
    uapi_gpio_disable_interrupt(pin);
    button_press_state_t press_state = button_get_press_state(pin);
    unsigned int irq_sts = osal_irq_lock();
    if (press_state == PRESSED) {
        g_button_debounced[gpio_index].press_flag = 1;
        g_button_debounced[gpio_index].debounced = 1;
        g_button_debounced[gpio_index].debounced_time = (uint32_t)uapi_systick_get_ms();
    }

    if (g_button_debounced[gpio_index].press_flag == 1 && press_state == RELEASED) {
        g_button_debounced[gpio_index].press_flag = 0;
        g_button_debounced[gpio_index].debounced = 1;
        g_button_debounced[gpio_index].debounced_time = (uint32_t)uapi_systick_get_ms();
    }

    osal_irq_restore(irq_sts);
    button_peripheral_api *button_api = button_port_get_api();
    button_api->button_add_veto();
    button_timer_start(gpio_index);
}

errcode_t uapi_button_gpio_register(pin_t pin)
{
    errcode_t ret;
    ret = uapi_gpio_unregister_isr_func(pin);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    /* GPIO set input and pull up */
    if ((uapi_pin_set_mode(pin, (pin_mode_t)HAL_PIO_FUNC_GPIO) != ERRCODE_SUCC) ||
        (uapi_pin_set_pull(pin, (pin_pull_t)HAL_PIO_PULL_UP) != ERRCODE_SUCC) ||
        (uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT) != ERRCODE_SUCC)) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "gpio config failed\r\n");
        return ERRCODE_FAIL;
    }
    ret = uapi_gpio_register_isr_func(pin, GPIO_INTERRUPT_DEDGE, button_gpio_callback);
    if (ret != ERRCODE_SUCC) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "button gpio register failed\r\n");
        return ret;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_button_deregister(pin_t pin)
{
    errcode_t ret;
    button_peripheral_api *button_api = button_port_get_api();
    ret = button_api->button_unregister_callback(pin);
    if (ret != ERRCODE_SUCC) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "button deregister failed\r\n");
        return ret;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_button_register(button_map_t *button_map)
{
    errcode_t ret = 0;
    errcode_t ret_rtc = 0;
    if (button_map == NULL) {
        return ERRCODE_FAIL;
    }

    button_peripheral_api *button_api = button_port_get_api();
    for (uint8_t i = 0; i < BUTTON_PIN_MAP_MAX; i++) {
        g_button_pin_map[i].pin = button_map[i].pin;
        g_button_pin_map[i].button = button_map[i].button;
        if (button_map[i].pin != PIN_NONE && button_map[i].button != BUTTON_MAX) {
            if (button_map[i].button == BUTTON_PWR) {
                button_api->button_pmu_pwr_cfg(true);
            }
            ret |= uapi_button_gpio_register(g_button_pin_map[i].pin);
            if (i == 0) {
                INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "button_pin_map error.\r\n");
                continue;
            }
            ret_rtc = uapi_rtc_create(0, &g_gpio_button_timer[i - 1]);
            if (ret_rtc != ERRCODE_SUCC) {
                INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "uapi_rtc_create FAILED 0x%x, index:%x", ret, i);
                return ret_rtc;
            }
        } else if (button_map[i].pin == PIN_NONE && button_map[i].button == BUTTON_PWR) {
            ret = button_api->button_register_callback(g_button_pin_map[i].pin, (osal_irq_handler)button_irq_callback);
            if (ret != ERRCODE_SUCC) {
                INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "button register failed\r\n");
                return ret;
            }
            continue;
        }
    }

    return ret;
}

void uapi_button_send_msg_register(msg_send_callback_t cb)
{
    if (cb != NULL) {
        g_msg_send_cb = cb;
    }
}

void uapi_button_deinit(void)
{
    /* Stop the time and clear the pin map */
    uapi_rtc_stop(g_button_timer);
    uapi_rtc_delete(g_button_timer);
    g_button_timer = NULL;
    for (uint32_t i = 0; i < BUTTON_GPIO_NUM; i++) {
        uapi_rtc_stop(g_gpio_button_timer[i]);
        uapi_rtc_delete(g_gpio_button_timer[i]);
        g_gpio_button_timer[i] = NULL;
    }
    for (uint8_t i = 0; i < BUTTON_PIN_MAP_MAX; i++) {
        g_button_pin_map[i].pin = PIN_NONE;
        g_button_pin_map[i].button = BUTTON_MAX;
    }
}

errcode_t uapi_button_init(void)
{
    errcode_t ret = 0;
    ret = uapi_rtc_init();

    ret |= uapi_rtc_adapter(0, RTC_0_IRQN, irq_prio(RTC_0_IRQN)); /* 1: using timer1 */
    if (ret != ERRCODE_SUCC) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "uapi_rtc_init FAILED 0x%x", ret);
        return ret;
    }
    ret = uapi_rtc_create(0, &g_button_timer);
    if (ret != ERRCODE_SUCC) {
        INPUT_FEATURE_PRINT_ERR(APP_BUTTON, "uapi_rtc_create FAILED 0x%x", ret);
        return ret;
    }

    return ERRCODE_SUCC;
}

button_peripheral_api g_button_api = {
    .button_pmu_pwr_cfg = porting_button_pmu_pwr_cfg,
    .button_check_interrupt = porting_button_check_int,
    .button_clear_interrupt = porting_button_clear_int,
    .button_get_state = porting_button_get_state,
    .button_register_callback = porting_button_irq_register,
    .button_unregister_callback = porting_button_unregister,
    .button_add_veto = porting_button_add_veto,
    .button_remove_veto = porting_button_remove_veto,
};

button_peripheral_api *button_port_get_api(void)
{
    return &g_button_api;
}