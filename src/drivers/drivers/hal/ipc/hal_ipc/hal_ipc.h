/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides HAL ipc \n
 *
 * History: \n
 * 2022-11-16, Create file. \n
 */

#ifndef HAL_IPC_H
#define HAL_IPC_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_ipc_api IPC
 * @ingroup  drivers_hal_ipc
 * @{
 */

/**
 * @if Eng
 * @brief  Init device for hal IPC.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层IPC初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_ipc_init_t)(void);

/**
 * @if Eng
 * @brief  Deinit device for hal IPC.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层IPC去初始化接口。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef void (*hal_ipc_deinit_t)(void);

/**
 * @if Eng
 * @brief  Enable all IPC interrupts form the source cores to the local core for hal IPC.
 * @else
 * @brief  HAL层使能所有源核到本核的IPC中断。
 * @endif
 */
typedef void (*hal_ipc_enable_all_int_t)(void);

/**
 * @if Eng
 * @brief  Disable all IPC interrupts form the source cores to the local core for hal IPC.
 * @else
 * @brief  HAL层去使能所有源核到本核的IPC中断。
 * @endif
 */
typedef void (*hal_ipc_disable_all_int_t)(void);

/**
 * @if Eng
 * @brief  Trigger the IPC interrupt from the local core to the destination core for hal IPC.
 * @param  [in]  core The destination core.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  HAL层触发本核到目的核的IPC中断。
 * @param  [in]  core 目标核。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
typedef errcode_t (*hal_ipc_set_core_int_t)(uint8_t core, uint8_t channel);

/**
 * @if Eng
 * @brief  Clear the IPC interrupt status from the source core to the local core for hal IPC.
 * @param  [in]  core The source core.
 * @else
 * @brief  HAL层清除源核到本核的IPC中断状态。
 * @param  [in]  core 源核。
 * @endif
 */
typedef void (*hal_ipc_clear_core_int_t)(uint8_t core, uint8_t channel);

/**
 * @if Eng
 * @brief  Get the IPC interrupt status from the source core to the local core for hal IPC.
 * @param  [in]  core The source core.
 * @else
 * @brief  HAL层获取源核到本核的IPC中断状态。
 * @param  [in]  core 源核。
 * @endif
 */
typedef bool (*hal_ipc_get_core_int_sts_t)(uint8_t core, uint8_t channel);

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
typedef errcode_t (*hal_ipc_irq_callback)(uint8_t src_core, uint8_t channel);

/**
 * @if Eng
 * @brief  Register the IPC interrupt callback.
 * @param  [in]  handler IPC Interrupt callback.
 * @else
 * @brief  注册IPC中断控制函数。
 * @param  [in]  core IPC中断控制函数。
 * @endif
 */
typedef void (*hal_ipc_regist_callback)(hal_ipc_irq_callback callback);

/**
 * @if Eng
 * @brief  Clear the IPC interrupt callback.
 * @else
 * @brief  HAL层清除IPC中断控制函数。
 * @endif
 */
typedef void (*hal_ipc_unregist_callback)(void);

/**
 * @if Eng
 * @brief  Interface between IPC driver and IPC hal.
 * @else
 * @brief  Driver层IPC和HAL层IPC的接口。
 * @endif
 */
typedef struct {
    hal_ipc_init_t    init;                         /*!< @if Eng Init for hal IPC interface.
                                                         @else   HAL层IPC的初始化接口。 @endif */
    hal_ipc_deinit_t  deinit;                       /*!< @if Eng Deinit for hal IPC interface.
                                                         @else   HAL层IPC去初始化接口。 @endif */
    hal_ipc_enable_all_int_t  enable_all_int;       /*!< @if Eng Enable all IPC interrupts form the source cores to
                                                                 the local core.
                                                         @else   使能所有源核到本核的IPC中断。 @endif */
    hal_ipc_disable_all_int_t disable_all_int;      /*!< @if Eng Disable all IPC interrupts form the source cores to
                                                                 the local core.
                                                         @else   去使能所有源核到本核的IPC中断。 @endif */
    hal_ipc_set_core_int_t    set_core_int;         /*!< @if Eng Trigger the IPC interrupt from the local core to
                                                                 a destination core.
                                                         @else   触发本核到目的核的IPC中断。 @endif */
    hal_ipc_clear_core_int_t  clear_core_int;       /*!< @if Eng Clear the IPC interrupt status from a source core
                                                                 to the local core.
                                                         @else   清除源核到本核的IPC中断状态。 @endif */
    hal_ipc_get_core_int_sts_t get_core_int_sts;    /*!< @if Eng Get the IPC interrupt status from a source core
                                                                 to the local core.
                                                         @else   获取源核到本核的IPC中断状态。 @endif */
    hal_ipc_regist_callback    register_callback;   /*!< @if Eng IPC HAL register receive interrupt callback.
                                                         @else   IPC 注册接收中断控制函数。 @endif */
    hal_ipc_unregist_callback  unregister_callback; /*!< @if Eng IPC HAL unregister receive interrupt callback.
                                                         @else   IPC 注册接收中断控制函数。 @endif */
} hal_ipc_funcs_t;

/**
 * @brief  Init the IPC which will set the base address of registers.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        failure.
 */
errcode_t hal_ipc_v150_regs_init(void);

/**
 * @brief  Deinit the IPC which will clear the base address of registers has been set by @ref hal_ipc_v150_regs_init.
 */
void hal_ipc_v150_regs_deinit(void);

/**
 * @if Eng
 * @brief  Register @ref hal_ipc_funcs_t into the g_hal_ipcs_funcs.
 * @param  [in]  funcs Interface between IPC driver and IPC hal.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  注册 @ref hal_ipc_funcs_t 到 g_hal_ipc_funcs。
 * @param  [in]  funcs Driver层IPC和HAL层IPC的接口实例。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @endif
 */
errcode_t hal_ipc_register_funcs(hal_ipc_funcs_t *funcs);

/**
 * @if Eng
 * @brief  Unregister @ref hal_ipc_funcs_t from the g_hal_ipcs_funcs.
 * @else
 * @brief  从g_hal_ipc_funcs注销。参考 @ref hal_ipc_funcs_t
 * @endif
 */
void hal_ipc_unregister_funcs(void);

/**
 * @if Eng
 * @brief  Get interface between IPC driver and IPC hal. For details, see @ref hal_ipc_funcs_t
 * @return Interface between IPC driver and IPC hal. For details, see @ref hal_ipc_funcs_t
 * @else
 * @brief  获取Driver层IPC和HAL层IPC的接口实例。参考 @ref hal_ipc_funcs_t
 * @return Driver层IPC和HAL层IPC的接口实例。参考 @ref hal_ipc_funcs_t
 * @endif
 */
hal_ipc_funcs_t *hal_ipc_get_funcs(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif