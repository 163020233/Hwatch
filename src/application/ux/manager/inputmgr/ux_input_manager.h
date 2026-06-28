/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux input manager header.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_INPUT_MANAGER_H
#define UX_INPUT_MANAGER_H

#include "stdint.h"
#include "errcode.h"
#include "ux_jack_customer.h"
#include "ux_api_jack.h"
#include "ux_api_input_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define UX_INPUT_MANAGER_BUTTON_1000MS 1000

typedef enum {
    UX_INPUT_BUTTON1_CLICK_1,
    UX_INPUT_BUTTON1_CLICK_2,
    UX_INPUT_BUTTON1_CLICK_3,
    UX_INPUT_BUTTON2_CLICK_1,
    UX_INPUT_BUTTON2_CLICK_2,
    UX_INPUT_BUTTON2_CLICK_3,
    UX_INPUT_BUTTON_LONG_PRESS_1S,
    UX_INPUT_BUTTON_LONG_PRESS_3S,
    UX_INPUT_BUTTON_EVENT_MAX,
} ux_input_manager_button_event_t;

typedef enum {
    UX_INPUT_CONTROL_MUSIC_PAUSE,
    UX_INPUT_CONTROL_MUSIC_PLAY,
    UX_INPUT_CONTROL_MUSIC_MAX,
} ux_input_manager_music_control_t;

ux_api_input_manager_button_info_t ux_input_manager_get_button_previous_behavior(void);

errcode_t ux_input_manager_dispatch_jack_event(ux_api_jack_state_t *event);
errcode_t ux_input_manager_dispatch_button_event(ux_api_input_manager_button_info_t *info);
errcode_t ux_input_manager_dispatch_box_event(ux_api_input_manager_box_info_t *info);
errcode_t ux_input_manager_dispatch_sensor_event(ux_api_input_manager_sensor_event_t *event);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
