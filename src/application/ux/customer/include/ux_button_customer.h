/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 * Description: Provides ux peripheral event \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-04， Create file. \n
 */

#ifndef UX_BUTTON_CUSTOMER_H
#define UX_BUTTON_CUSTOMER_H

#include "stdint.h"
#include "fsm_master.h"
#include "ux_api_msg.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* 需要增加按钮在此配置 */
typedef enum {
    UX_BUTTON_PWR,
    UX_BUTTON_1,
    UX_BUTTON_2,
    UX_BUTTON_3,
    UX_BUTTON_4,
    UX_BUTTON_5,

    UX_BUTTON_MAX
} ux_api_button_id_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
