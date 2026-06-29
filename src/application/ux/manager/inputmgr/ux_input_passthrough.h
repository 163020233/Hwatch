/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux input manager header.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-06, Create file. \n
 */
#ifndef UX_INPUT_PASSTHROUGH_H
#define UX_INPUT_PASSTHROUGH_H

#include "errcode.h"
#include "ux_input_manager.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

errcode_t ux_input_manager_event_passthrough(ux_api_input_manager_sensor_event_t event);

errcode_t ux_input_manager_button_event_passthrough(ux_input_manager_button_event_t event);

errcode_t ux_input_manager_linein_event_passthrough(ux_input_manager_music_control_t event);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
#endif
