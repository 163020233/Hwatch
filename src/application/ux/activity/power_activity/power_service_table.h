/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux power service table \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-12-13, Create file. \n
 */
#ifndef POWER_SERVICE_TABLE_H
#define POWER_SERVICE_TABLE_H

#include "stdint.h"
#include "fsm_master.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t get_power_service_index(fsm_master_state_t state, uint8_t event);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
