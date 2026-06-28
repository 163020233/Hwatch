/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux power service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-08-26， Create file. \n
 */
#include "ux_common_log.h"
#include "ux_errcode.h"
#include "key_id.h"
#include "common.h"
#include "common_def.h"
#include "timer_manager.h"
#include "platform_types.h"
#include "nv_storage.h"
#include "property.h"
#include "ux_power_customer.h"
#include "ux_event_define.h"
#include "ux_api_fwk.h"
#include "calendar.h"
#include "calendar_porting.h"
#include "ux_system_manager.h"
#include "ux_config.h"
#include "ux_api_fwk.h"
#include "ux_status_define.h"
#include "ux_power.h"
#include "power_mgmt.h"

#define BATTERY_PERCENTAGE_STUB 100

void ux_adapter_power_off_process(void)
{
    if (get_working_state() == STATUS_ON_WORKING) {
        acc_type_t acc_type = ACC_OFF_ALL;
        ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_ACC_OFF, (uint8_t *)(&acc_type), sizeof(acc_type_t));
    } else {
        uapi_sys_shipmode(0);
    }
}

void ux_adapter_power_reset_process(void)
{
    hal_reboot_chip();
}

/* 重启 */
errcode_t ux_adapter_power_reset(void)
{
    hal_reboot_chip();
    return UX_ERR_SUCCESS;
}

/* 下电，ship模式 */
errcode_t ux_adapter_power_shipmode(void)
{
    uapi_sys_shipmode(0);
    return UX_ERR_SUCCESS;
}

/* 充电控制相关 */
errcode_t ux_adapter_power_get_percentage(uint32_t *val)
{
    *val = BATTERY_PERCENTAGE_STUB;
    return UX_ERR_SUCCESS;
}

errcode_t ux_adapter_power_get_charge_state(ux_adapter_power_charge_t *charge_state)
{
    *charge_state = UX_ADAPTER_POWER_CHARGE_MAX;
    return UX_ERR_SUCCESS;
}

errcode_t ux_adapter_power_register_charge_cb(charge_state_cb callback)
{
    unused(callback);
    return UX_ERR_SUCCESS;
}

errcode_t ux_adapter_register_watchdog_irq_cb(watchdog_dump_callback_t callback)
{
    unused(callback);
    return UX_ERR_SUCCESS;
}

errcode_t ux_adapter_power_save_timestamp(void)
{
    return UX_ERR_SUCCESS;
}

errcode_t ux_power_pm_stage_switch(ux_power_pm_stage_state_t state)
{
    unused(state);
    return UX_ERR_SUCCESS;
}

/* 独立上电 */
errcode_t ux_adapter_power_on(ux_power_cores_t core)
{
    switch (core) {
        case UX_POWER_CORE_BT:
            return uapi_pmu_control(PMU_BT_SUB, PMU_POWER_ON_AND_RUN);
        default:
            return UX_ERR_INVALID_PARAMETER;
    }
}

/* 独立下电 */
errcode_t ux_adapter_power_off(ux_power_cores_t core)
{
    switch (core) {
        case UX_POWER_CORE_BT:
            return uapi_pmu_control(PMU_BT_SUB, PMU_POWER_OFF);

        default:
            return UX_ERR_INVALID_PARAMETER;
    }
}