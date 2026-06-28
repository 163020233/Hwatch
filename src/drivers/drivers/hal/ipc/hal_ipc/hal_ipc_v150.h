/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 HAL IPC \n
 *
 * History: \n
 * 2022-11-16, Create file. \n
 */
#ifndef HAL_IPC_V150_H
#define HAL_IPC_V150_H

#include "ipc_porting.h"
#include "hal_ipc.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_ipc_v150 IPC V150
 * @ingroup  drivers_hal_ipc
 * @{
 */

/**
 * @if Eng
 * @brief  Get interface between ipc driver and ipc hal.
 * @else
 * @brief  获取Driver层IPC和HAL层IPC的接口实例。
 * @endif
 */
hal_ipc_funcs_t *hal_ipc_v150_funcs_get(void);

/**
 * @if Eng
 * @brief  IPC HAL receive interrupt callback.
 * @param  [in]  src_core The source core of IPC message.
 * @param  [in]  channel IPC message interrupt channel.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  IPC接收消息中断控制回调函数。
 * @param  [in]  src_core IPC消息源核。
 * @param  [in]  channel IPC消息通道
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_ipc_v150_irq_callback(uint8_t src_core, uint8_t channel);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif