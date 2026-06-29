/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux power service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-08， Create file. \n
 */
#ifndef POWER_MGMT_H_
#define POWER_MGMT_H_

#include "pmu_sub_control.h"
#include "hal_reboot.h"
#include "ux_power.h"
#include "ux_errcode.h"

#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
    UX_ADAPTER_POWER_CHARGE_STANDBY = 0, /*!< @if Eng Standby mode.
                                                @else  待机模式   @endif */
    UX_ADAPTER_POWER_CHARGE_SHIPMODE,    /*!< @if Eng Shipmode.
                                                @else  运输模式   @endif */
    UX_ADAPTER_POWER_CHARGE_TRX_MODE,    /*!< @if Eng Transfer mode.
                                                @else  传输模式   @endif */
    UX_ADAPTER_POWER_CHARGE_NO_CHARGING, /*!< @if Eng No charging mode.
                                                @else  非充电模式   @endif */
    UX_ADAPTER_POWER_CHARGE_TC,          /*!< @if Eng Trickle charge mode.
                                                @else  涓流充电模式   @endif */
    UX_ADAPTER_POWER_CHARGE_CC,          /*!< @if Eng Constant currernt charge mode.
                                                @else  恒流充电模式   @endif */
    UX_ADAPTER_POWER_CHARGE_CV,          /*!< @if Eng Constant voltage charge mode.
                                                @else  恒压充电模式   @endif */
    UX_ADAPTER_POWER_CHARGE_DONE,        /*!< @if Eng Charge done mode.
                                                @else  充电完成模式   @endif */
    UX_ADAPTER_POWER_CHARGE_MAX,
} ux_adapter_power_charge_t;

typedef struct {
    uint64_t last_timestamp_ms;
    uint64_t last_systick;
} timestamp_nv_t;

typedef void (*charge_state_cb)(uint8_t charge_state);
typedef void (*watchdog_dump_callback_t)(uint8_t charge_state);
errcode_t ux_adapter_power_save_timestamp(void);
void ux_adapter_power_reset_process(void);
void ux_adapter_power_off_process(void);
errcode_t ux_adapter_power_reset(void);
errcode_t ux_adapter_power_shipmode(void);
errcode_t ux_adapter_power_get_percentage(uint32_t *val);
errcode_t ux_adapter_power_get_charge_state(ux_adapter_power_charge_t *charge_state);
errcode_t ux_adapter_power_register_charge_cb(charge_state_cb callback);
errcode_t ux_adapter_register_watchdog_irq_cb(watchdog_dump_callback_t callback);
errcode_t ux_adapter_power_on(ux_power_cores_t core);
errcode_t ux_adapter_power_off(ux_power_cores_t core);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif