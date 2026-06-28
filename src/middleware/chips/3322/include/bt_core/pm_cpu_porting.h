/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm core adapter header file
 */

#ifndef PM_CPU_PORTING_H
#define PM_CPU_PORTING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_chips_pm_sleep_port PM cpu port
 * @ingroup  middleware_chips_pm
 * @{
 */


void lowpower_cpu_suspend(void);

void lowpower_cpu_resume(void);

void pm_port_cpu_suspend(void);

void pm_port_cpu_resume(void);

void uapi_pm_enable_lp_auto_eng(bool en);

bool uapi_pm_get_lp_auto_eng(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif