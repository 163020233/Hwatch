/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd usb service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-29， Create file. \n
 */
#ifndef UX_CMD_USB_H
#define UX_CMD_USB_H
#include <stdint.h>
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_set_hid_event(uint16_t device_id, uint8_t *data, uint16_t data_len);
void ux_cmd_usb_register(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
