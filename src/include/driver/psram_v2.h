/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: psram v2 driver header file \n
 *
 * History: \n
 * 2024-05-15, Create file. \n
 */
#ifndef PSRAM_V2_H
#define PSRAM_V2_H

#include "errcode.h"
#include "psram_porting.h"
#include "hal_psram_regs_op.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup _
 * @ingroup
 * @{
 */

/**
 * @if Eng
 * @brief  Init PSRAM.
 * @param  [in]  id Psram number.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  初始化psram
 * @param  [in]  id Psram编号
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_psram_init(uint8_t id);

/**
 * @if Eng
 * @brief  Suspend psram.
 * @param  [in]  arg Psram params.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  挂起psram
 * @param  [in]  arg Psram参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_psram_suspend(uintptr_t arg);

/**
 * @if Eng
 * @brief  Resume psram.
 * @param  [in]  arg Psram params.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  恢复psram
 * @param  [in]  arg Psram参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_psram_resume(uintptr_t arg);

/**
 * @if Eng
 * @brief  Wait PSRAM hs status ready.
 * @param  [in]  arg Psram params.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  等待psram hs 状态就绪
 * @param  [in]  arg Psram参数
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_psram_wait_hs_status(uintptr_t arg);

/**
 * @if Eng
 * @brief  Init PSRAM controller.
 * @param  [in]  id Psram number.
 * @else
 * @brief  初始化psram控制器
 * @param  [in]  id Psram编号
 * @endif
 */
void uapi_psram_controller_init(uint8_t id);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif