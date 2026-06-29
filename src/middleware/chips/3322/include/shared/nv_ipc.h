/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: NV IPC interface
 */

#ifndef NV_IPC_H
#define NV_IPC_H

#include <stdint.h>
#include <memory_config_shared.h>

#define BT_CAL_NV_SAVE_ADDR IP_STATE_REGION_START

uint16_t nv_ipc_get_xotrim(void);
void nv_ipc_set_xotrim(uint16_t value);
void nv_ipc_reset_xotrim(void);
#endif