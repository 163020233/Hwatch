/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux sle activity process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef SLE_SERVICE_H
#define SLE_SERVICE_H
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef void (*ux_sle_serivce_func)(const uint8_t *param, uint16_t length);

typedef struct {
    uint8_t service_index;
    ux_sle_serivce_func func;
} ux_sle_serivce_map_t;

typedef enum {
    SLE_PAIRING_START,
    SLE_PAIRING_STOP,
    SLE_PAIRED,
    SLE_CONNECTED,
    SLE_DISCONNECTED,
    SLE_STREAM_REQUEST,
    SLE_TRANSMIT_MODE_CHANGE,

    SERVICE_MAX
} ux_sle_service_index_t;

void ux_sle_service_execute(uint8_t ux_sle_service_execute, uint8_t *param, uint16_t length);
void ux_sle_activity_init(void);
void ux_sle_activity_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
