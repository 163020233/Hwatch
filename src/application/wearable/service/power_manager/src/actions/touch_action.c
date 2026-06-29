/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: action touch func.
 * Author:
 * Create:
 */

#include "power_display_service.h"
#include "display_action.h"
#include "input_feature.h"
#include "common_def.h"

int32_t action_touch_execute_entry(uint16_t action_bitmap, uint16_t new_state, event_type_t type, uint32_t event)
{
    unused(type);
    unused(event);

    screen_context_t *sc = power_display_get_screen_context();
    if (new_state == sc->current_state) {
        return 0;
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_NORMAL_WORK_MODE)) != 0) {
        input_set_workmode(APP_TOUCH, INPUT_DEV_NORMAL_MODE, INPUT_SET_MODE_SYN);
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_SLEEP_MODE)) != 0) {
        input_set_workmode(APP_TOUCH, INPUT_DEV_SLEEP_MODE, INPUT_SET_MODE_SYN);
    }

    if ((action_bitmap & GET_ACTION_MASK(SET_TP_TO_STANDBY_MODE)) != 0) {
        input_set_workmode(APP_TOUCH, INPUT_DEV_STANDBY_MODE, INPUT_SET_MODE_SYN);
    }
    return 0;
}