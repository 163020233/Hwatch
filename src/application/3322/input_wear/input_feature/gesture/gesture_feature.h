/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: gesture application source file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-31, Create file. \n
 */

#ifndef GESTURE_APP_H
#define GESTURE_APP_H
#include "input_feature.h"

typedef struct {
    uint16_t valid_flag;
    uint16_t activation_flag;
    int32_t pred;
} gesture_input_result;

void gesture_send_msg_to_app(uint16_t valid_flag, uint16_t activation_flag, int32_t pred);
void gesture_service_callback(common_notify_mail_t *notify_msg);
#endif
