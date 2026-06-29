/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux pairing process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */
#ifndef PROTOCOL_SERVICE_H
#define PROTOCOL_SERVICE_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*protocol_serivce_func)(const uint8_t *param, uint16_t length);

typedef struct {
    uint8_t index;
    protocol_serivce_func func;
} protocol_serivce_map_t;

typedef struct {
    uint8_t        report_id;
    uint8_t        module_id;
    uint8_t        cmd_id;
    uint8_t        tlv[0];
} protocol_hid_frame_t;

typedef enum {
    PROTOCOL_DIAG_MSG,
    PROTOCOL_HID_MSG,
    PROTOCOL_OTA_MSG,
    PROTOCOL_CUSTOMER_MSG,

    PROTOCOL_MAX
} protocol_service_index_t;

void protocol_activity_init(void);
errcode_t protocol_msg_process(uint8_t event, uint8_t *param, uint16_t param_len);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
