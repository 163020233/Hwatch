/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux sensor process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-13， Create file. \n
 */
#ifndef UX_SENSOR_PROCESS_H
#define UX_SENSOR_PROCESS_H

#include <stdbool.h>
#include <errcode.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_SENSOR_WEAR_STATE_IN,
    UX_SENSOR_WEAR_STATE_OUT,
    UX_SENSOR_WEAR_STATE_MAX,
} ux_sensor_wear_state_t;

typedef enum {
    UX_SENSOR_WEAR,
    UX_SENSOR_PRESS,
} ux_sensor_type_t;

ux_sensor_wear_state_t ux_sensor_get_wear_state(void);
errcode_t ux_sensor_active(ux_sensor_type_t type, bool active);
errcode_t ux_sensor_set_capsensor_compensation(bool enable);
errcode_t ux_sensor_calibrate_capsensor(void);
errcode_t ux_sensor_init(void);
errcode_t ux_sensor_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
