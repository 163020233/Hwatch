/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm sleep port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#ifndef PM_SLEEP_PORTING_H
#define PM_SLEEP_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include <pm_cpu_porting.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_sleep_port PM sleep port
 * @ingroup  middleware_chips_pm
 * @{
 */

#ifndef CONFIG_PM_LIGHT_SLEEP_THRESHOLD_MS
#define CONFIG_PM_LIGHT_SLEEP_THRESHOLD_MS 2
#endif

#ifndef CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS
#define CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS 10
#endif

void pm_port_start_tickless(void);
void pm_port_stop_tickless(uint32_t sleep_ms);
uint32_t pm_port_get_sleep_ms(void);
void pm_port_allow_deepsleep(bool allow);
void pm_port_enter_wfi(void);
void pm_port_start_wakeup_timer(uint32_t sleep_ms);
void pm_port_lightsleep_config(void);
void pm_port_light_wakeup_config(void);
void pm_port_deepsleep_config(void);
void pm_port_deep_wakeup_config(void);
void pm_wakeup_wait_time_config(uint16_t level);
void pm_sleep_wait_time_config(void);
void pm_auto_cg_config(bool on);
void pm_wakeup_int_config(void);
void pm_auto_pg_config(void);
void pm_wakeup_mode_config(void);

uint16_t pm_port_get_sleep_event_status(void);
uint16_t pm_port_get_wakeup_event_status(void);
#if defined(PM_SLEEP_DEBUG_ENABLE) && (PM_SLEEP_DEBUG_ENABLE == 1)
void pm_port_set_start_time(void)
#endif

#define PM_GET_SLEEP_EVENT_STATUS   pm_port_get_sleep_event_status()
#define PM_GET_WKUP_EVENT_STATUS  pm_port_get_wakeup_event_status()

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

