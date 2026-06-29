/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux cmd system header \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-22， Create file. \n
 */
#ifndef UX_CMD_SYSTEM_H
#define UX_CMD_SYSTEM_H

#include <stdint.h>
#include "errcode.h"
#include "ux_system_config.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

errcode_t ux_cmd_set_sys_time(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_sys_time(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_language(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_language(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_get_sdk_version(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_notify_sdk_version(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_set_file_format(uint16_t device_id, uint8_t *data, uint16_t data_len);

errcode_t ux_cmd_sync_system_config(void);
void ux_cmd_system_register(void);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif