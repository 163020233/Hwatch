/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description: Provide ux power cmd \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-07-22， Create file. \n
 */
#ifndef UX_CMD_POWER_H
#define UX_CMD_POWER_H

#include "ux_power.h"
#include "key_struct.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
    ux_api_power_off_reason_t reason; // 关机原因
    uint32_t time;                    // 延迟时间
} ux_cmd_power_off_t;

void ux_cmd_sync_battery_2_master(void);
errcode_t ux_cmd_get_auto_power_off(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_get_auto_power_off_setting(ux_auto_power_off_t *setting, uint16_t data_len);
errcode_t ux_cmd_get_charge_status(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_get_battery_percentage(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_set_auto_power_off(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_on(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_off(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_startup(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_shutdown(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_on_with_box(uint16_t device_id, uint8_t *data, uint16_t data_len);
errcode_t ux_cmd_power_off_with_box(uint16_t device_id, uint8_t *data, uint16_t data_len);
void ux_cmd_power_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
