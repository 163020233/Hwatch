/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux usb process \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-12-13 Create file. \n
 */
#ifndef POWER_PROCESS_H
#define POWER_PROCESS_H
#include "stdint.h"
#include "errcode.h"
#include "fsm_master.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    USB_CHARGE_CABLE,
    USB_DATA_CABLE,
    USB_CABLE_MAX
} ux_usb_cable_t;

void ux_usb_set_cable_type(ux_usb_cable_t cable_type);
ux_usb_cable_t ux_usb_get_cable_type(void);
errcode_t ux_usb_open_msc_process(void);
void ux_usb_close_msc_process(void);
errcode_t ux_usb_open_uac_process(void);
errcode_t ux_usb_close_uac_process(void);
errcode_t ux_usb_open_hid_process(void);
void ux_usb_close_hid_process(void);
void ux_usb_detect_hid(void);
void ux_usb_start_service_process(const uint8_t connect_time);
void ux_usb_stop_service_process(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
