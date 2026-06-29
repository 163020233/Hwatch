/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: PM DVFS Internal Header \n
 *              Simplified: no request table declarations.
 *
 * History: \n
 * 2026-06-11, Create file. \n
 */

#ifndef PM_DVFS_INTERNAL_H
#define PM_DVFS_INTERNAL_H

#include "pm_dvfs.h"
#include "pm_dvfs_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*==============================================================================
 * Governor Internal Functions (not for external use)
 *============================================================================*/

/**
 * @brief Initialize governor (no local timer needed - driven by cpup guard hook).
 * @return ERRCODE_SUCC on success, error code otherwise.
 */
errcode_t pm_dvfs_governor_init(void);

/**
 * @brief Hook called by kernel OsCpupGuard() every 100ms (swtmr task context).
 *        Reads 1000ms average CPU load, computes cpu_freq, checks boost expiry,
 *        then triggers reeval.
 *        CONFIG_PM_DVFS_ENABLE=0: not called (no impact on original system).
 */
void pm_dvfs_cpup_guard_hook(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
