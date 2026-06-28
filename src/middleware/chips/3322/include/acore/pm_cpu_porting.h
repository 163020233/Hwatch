/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm core adapter header file
 */

#ifndef PM_CPU_PORTING_H
#define PM_CPU_PORTING_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum {
    POWER_HOLD_ENTER_SHIP_MODE = 0x0,
    POWER_HOLD_ENTER_POWER_DOWN = 0x1,
} power_hold_mode_t;

/**
 * @defgroup middleware_chips_pm_sleep_port PM cpu port
 * @ingroup  middleware_chips_pm
 * @{
 */

void lowpower_cpu_suspend(void);

void lowpower_cpu_resume(void);

void lowpower_cpu_resume_ree(void);

void pm_port_cpu_suspend(void);

void pm_port_cpu_resume(void);

void pm_port_enter_shipmode(unsigned int args);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif