/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved. \n
 *
 * Description: add media player function \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2026-06-24， Create file. \n
 */
#ifndef PLAYER_SERVICE_H
#define PLAYER_SERVICE_H
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void (*ux_player_service_func)(const uint8_t *param, uint16_t length);

typedef struct {
    uint8_t service_index;
    ux_player_service_func func;
} ux_player_service_map_t;

void ux_player_service_execute(uint8_t service_index, uint8_t *param, uint16_t length);
void ux_player_activity_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
