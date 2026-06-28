/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux power manager service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-12， Create file. \n
 */
#ifndef UX_POWER_MANAGER_H
#define UX_POWER_MANAGER_H

#include <stdint.h>
#include "errcode.h"
#include "ux_api_fwk.h"
#include "ux_service_define.h"
#include "ux_status_define.h"
#ifndef DEMO_WATCH
#include "ux_api_powermgr.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef errcode_t (*powermgr_state_migrate_action)(void);
typedef errcode_t (*powermgr_state_change_handler)(working_state_t state);

typedef struct {
    uint8_t index;
    powermgr_state_migrate_action fun;
} powermgr_state_migrate_action_map_t;

typedef struct {
    working_state_t state;
    powermgr_state_change_handler handler;
} ux_powermgr_service_context_t;

void ux_powermgr_init(void);
void ux_powermgr_deinit(void);
#ifndef DEMO_WATCH
void ux_powermgr_set_power_state(ux_api_powermgr_state_t state);
ux_api_powermgr_state_t ux_powermgr_get_power_state(void);
#endif
errcode_t ux_powermgr_register_state_change_handler(ux_service_t service,
    powermgr_state_change_handler handler);
errcode_t ux_powermgr_state_change_notify(working_state_t state);
errcode_t ux_powermgr_set_service_working_status(ux_service_t service, working_state_t state);
errcode_t ux_powermgr_get_service_working_status(ux_service_t service, working_state_t* state);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
