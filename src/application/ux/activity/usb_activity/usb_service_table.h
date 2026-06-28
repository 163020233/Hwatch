/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux usb service table \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#ifndef USB_SERVICE_TABLE_H
#define USB_SERVICE_TABLE_H

#include "stdint.h"
#include "fsm_master.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

uint8_t get_usb_service_index(fsm_master_state_t state, uint8_t event);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
