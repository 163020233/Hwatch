/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-11， Create file. \n
 */
#ifndef UX_FWK_MSG_DEF_H
#define UX_FWK_MSG_DEF_H
#include "stdint.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UX_EVENT_MSG_PARA_LENGTH 4

typedef enum {
    UX_BT = 0,
    UX_HAID,
    UX_AUDIO,
    UX_OTA,
    UX_MASTER,
    UX_POWER,
    UX_CHARGE,
    UX_TOUCHPAD,
    UX_BUTTON,
    UX_BOX,
    UX_AT,
    UX_PAIR,
    UX_SPP,
    UX_APP,
    UX_LED,
    UX_PHONE,
    UX_DFX,
    UX_SLE,
    UX_TASK_BUTT
} ux_task_t;

typedef struct ux_event_msg_t {
    ux_task_t task_src;
    uint8_t res;
    uint16_t event;
    uint8_t ret_code;
    uint8_t para[UX_EVENT_MSG_PARA_LENGTH];
} ux_event_msg_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif