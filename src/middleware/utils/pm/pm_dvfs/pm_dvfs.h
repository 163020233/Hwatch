/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PM DVFS Public Header \n
 *              Simplified: only 2 public APIs, no request structures.
 *
 * History: \n
 * 2026-06-11, Create file. \n
 */

#ifndef PM_DVFS_H
#define PM_DVFS_H

#include "common_def.h"
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/**
 * @defgroup drivers_driver_pm_dvfs PM DVFS
 * @ingroup  drivers_driver_pm
 * @{
 */

/*==============================================================================
 * Public API
 *============================================================================*/

/**
 * @brief Initialize PM DVFS module.
 *        Starts 100ms CPU sampling timer.
 * @return ERRCODE_SUCC on success, error code otherwise.
 */
errcode_t uapi_pm_dvfs_init(void);

/**
 * @brief Set business boost frequency.
 *
 * @param [in] freq_hz Target frequency in Hz. 0 means clear boost.
 * @param [in] timeout_ms Timeout in milliseconds. 0 means never expires.
 * @return ERRCODE_SUCC on success, error code otherwise.
 * @note Only one boost is supported at a time; second call overwrites the first.
 *       Boost is bounded by current hardware frequency from pm_stage_switch.
 */
errcode_t uapi_pm_boost_set(uint32_t freq_hz, uint32_t timeout_ms);

/**
 * @brief Hook called by pm_stage_switch() at its end.
 *        CONFIG_PM_DVFS_ENABLE=0: empty function (no impact on original system).
 */
void pm_dvfs_on_scene_changed(void);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
