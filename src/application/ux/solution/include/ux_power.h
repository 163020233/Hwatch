/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux power solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-11， Create file. \n
 */
#ifndef UX_POWER_H
#define UX_POWER_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"
#include "securec.h"
#include "ux_api_power.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_POWER_CORE_DSP,
    UX_POWER_CORE_BT,
    UX_POWER_CORE_MAX,
} ux_power_cores_t;

typedef enum {
    UX_POWER_NORMAL,            // 设备正常上电
    UX_POWER_OFF_DELAY,         // 设备处于延时下电
    UX_POWER_RESET_DELAY,       // 设备处于延时重启
    UX_POWER_STATE_MAX
} ux_power_state_t;

typedef enum {
    UX_POWER_BT_NORMAL,             // BT核正常运行,代表此状态下可以正常处理消息
    UX_POWER_BT_OFF,                // BT核已下电
    UX_POWER_BT_OFF_DELAY,          // BT核处于延时下电
    UX_POWER_BT_ON_DOING,           // BT核上电中
    UX_POWER_BT_ABNORMAL,           // BT核异常
    UX_POWER_BT_MAX
} ux_power_b_core_t;

typedef enum {
    STATUS_RESET_INVALID = 1,
    STATUS_RESET_VALID
} reset_state_t;

typedef enum {
    UX_POWER_PM_STAGE_STATE_HIGH_PERF_ON,
    UX_POWER_PM_STAGE_STATE_HIGH_PERF_OFF,
    UX_POWER_PM_STAGE_STATE_TRANSMIT,
    UX_POWER_PM_STAGE_STATE_RECEIVE,
    UX_POWER_PM_STAGE_STATE_IDLE,
    UX_POWER_PM_STAGE_STATE_RECORDER_ON,
    UX_POWER_PM_STAGE_STATE_RECORDER_OFF,

    UX_POWER_PM_STAGE_STATE_MAX
} ux_power_pm_stage_state_t;

errcode_t ux_power_init(void);
errcode_t ux_power_deinit(void);
void ux_power_info_restore(void);
void ux_power_set_state(ux_power_state_t state);
errcode_t ux_power_get_state(ux_power_state_t *state);
void update_reset_info_to_nv(reset_state_t state);
reset_state_t ux_power_get_reset_valid_state(void);
errcode_t ux_power_set_reset_valid_state(reset_state_t reset_state);
errcode_t ux_power_set_off_reason(ux_api_power_off_reason_t reason);
void ux_power_set_actual_power_off(bool actual_power_off);
errcode_t ux_power_get_auto_power_off_setting(uint8_t* auto_power_off_enable, uint8_t len);
errcode_t ux_power_pm_stage_switch(ux_power_pm_stage_state_t state);
errcode_t ux_power_bt_on(void);
errcode_t ux_power_bt_off(uint32_t time);
errcode_t ux_power_cancel_bt_delay_off(void);
errcode_t ux_power_get_bt_core_state(ux_power_b_core_t *state);
bool ux_power_get_actual_power_off(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
