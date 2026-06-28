/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides device operation header \n
 *
 * History: \n
 * 2023-01-09， Create file. \n
 */

#ifndef PM_DEV_H
#define PM_DEV_H

#include "common_def.h"
#include "errcode.h"
#include "pm_dev_porting.h"
#include "stdbool.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup middleware_utils_pm_dev PM dev
 * @ingroup  middleware_utils_pm
 * @{
 */

/**
 * @if Eng
 * @brief  APIs for device operations.
 * @else
 * @brief  设备操作API接口。
 * @endif
 */
typedef struct {
    errcode_t (*suspend)(uintptr_t arg);
    uintptr_t suppend_arg;
    errcode_t (*resume)(uintptr_t arg);
    uintptr_t resume_arg;
} pm_dev_ops_t;

/**
 * @if Eng
 * @brief  Registers the device management function for the PM.
 * @param  [in]  dev_id The device ID.
 * @param  [in]  dev_ops Specifies the operation functions of the device ID.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为PM注册设备操作函数。
 * @param  [in]  dev_id 设备ID。
 * @param  [in]  dev_ops 指定设备ID的操作函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pm_register_dev_ops(pm_dev_id_t dev_id, pm_dev_ops_t *dev_ops);

/**
 * @if Eng
 * @brief  Unregisters the device management function for the PM.
 * @param  [in]  dev_id The device ID.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为PM去注册设备操作函数。
 * @param  [in]  dev_id 设备ID。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pm_unregister_dev_ops(pm_dev_id_t dev_id);

/**
 * @if Eng
 * @brief  The device suspend API before sleep.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  睡眠前设备挂起API。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pm_dev_suspend(void);

/**
 * @if Eng
 * @brief  The device resume API after wake-up.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  唤醒后设备恢复API。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_pm_dev_resume(void);

/**
 * @if Eng
 * @brief  Clock control callback function type.
 * @param  [in]  inst_id The instance ID.
 * @param  [in]  on      Clock control flag (true: enable, false: disable).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  时钟控制回调函数类型。
 * @param  [in]  inst_id 实例ID。
 * @param  [in]  on      时钟控制标志（true: 使能, false: 禁用）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*pm_clock_ctrl_t)(uint32_t inst_id, bool on);

/**
 * @if Eng
 * @brief  Registers the clock control callback function for the PM.
 * @param  [in]  dev_id The device ID.
 * @param  [in]  cb     The clock control callback function.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  为PM注册时钟控制回调函数。
 * @param  [in]  dev_id 设备ID。
 * @param  [in]  cb     时钟控制回调函数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t pm_clock_register(pm_dev_id_t dev_id, pm_clock_ctrl_t cb);

/**
 * @if Eng
 * @brief  Sets the clock state for the specified device instance.
 * @param  [in]  dev_id  The device ID.
 * @param  [in]  inst_id The instance ID.
 * @param  [in]  on      Clock control flag (true: enable, false: disable).
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置指定设备实例的时钟状态。
 * @param  [in]  dev_id  设备ID。
 * @param  [in]  inst_id 实例ID。
 * @param  [in]  on      时钟控制标志（true: 使能, false: 禁用）。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t pm_clock_set(pm_dev_id_t dev_id, uint32_t inst_id, bool on);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
