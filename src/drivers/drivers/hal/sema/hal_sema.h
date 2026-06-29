/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides HAL sema \n
 */
#ifndef HAL_SEAM_H
#define HAL_SEAM_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_sema sema
 * @ingroup  drivers_hal_sema
 * @{
 */

/**
 * @if Eng
 * @brief  Set the sem_set param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @param  [in] wait_time_us set sem_set wait time len.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置信号量锁存标记参数。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @param  [in] wait_time_us 信号量锁存设置等待时长。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sema_get_sem(uint8_t sema_index, uint32_t wait_time_us);


/**
 * @if Eng
 * @brief  Set the sub_clr param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置信号量释放锁存标记参数。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sema_put_sem(uint8_t sema_index);

/**
 * @if Eng
 * @brief  Set the force_clr param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置信号量强制清除锁存标记参数。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t hal_sema_force_clear(uint8_t sema_index);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
