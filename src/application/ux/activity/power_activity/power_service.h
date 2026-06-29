/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux power service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-12-13 Create file. \n
 */
#ifndef POWER_SERVICE_H
#define POWER_SERVICE_H
#include "stdint.h"
#include "errcode.h"
#include "fsm_master.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void (*ux_power_service_func_t)(const uint8_t *param, uint16_t length);

typedef enum {
    POWER_ON = 0,
    POWER_ON_ACK,
    POWER_OFF,
    POWER_OFF_ACK,
    ACC_ON,
    ACC_OFF,
    ACC_OFF_ACK,
    POWER_STANDBY,
    POWER_BUTTON_KEY,
    RESET_INFO,
    POWER_OFF_TRIGGER,

    POWER_SERVICE_INDEX_MAX
} service_power_index_t;

typedef struct {
    uint8_t service_index;
    ux_power_service_func_t func;
} ux_power_service_map_t;

void ux_power_service_execute(uint8_t service_index, uint8_t *param, uint16_t length);
void ux_power_activity_init(void);
void ux_power_activity_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
