
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025. All rights reserved.
 *
 * Description: SLE Service Framework (SSF) Functions about Init API File.
 */

#ifndef SSF_INIT_H
#define SSF_INIT_H

#include <stdint.h>
#include "sle_common.h"
#include "errcode.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @if Eng
 * @brief Enum of SLE stack state.
 * @else
 * @brief SLE协议栈状态枚举.
 * @endif
 */
typedef enum {
    SLE_STATE_ENABLED,     /*!< @if Eng SLE stack is enabled.
                                @else   SLE协议栈已使能。 @endif */
    SLE_STATE_DISABLED,    /*!< @if Eng SLE stack is disabled.
                                @else   SLE协议栈已去使能。 @endif */
    SLE_STATE_PDL_CLEARED, /*!< @if Eng SLE stack pdl is cleared.
                                @else   SLE协议栈pdl已清除。 @endif */
} bs_sle_state_t;

/**
 * @if Eng
 * @brief  Callback invoked when stack enabled / disabled.
 * @par    When registered, this callback notifies the upper layer
 *         when sle stack is enabled or disabled.
 * @param  [in]  state sle stack state.
 * @par Dependency:
 * @see bs_sle_callbacks_t
 * @else
 * @brief  协议栈使能/去使能的回调函数。
 * @par    注册该回调函数之后，stack使能或者去使能后该回调函数会上报上层。
 * @param  [in]  state sle协议栈状态。
 * @par 依赖:
 * @see bs_sle_callbacks_t
 * @endif
 */
typedef void (*bs_sle_stack_state_cbk)(uint8_t state);

/**
 * @if Eng
 * @brief Struct of sle enable callback function.
 * @else
 * @brief sle状态回调函数接口定义。
 * @endif
 */
typedef struct {
    bs_sle_stack_state_cbk stack_state_cbk;
} bs_sle_stack_state_callbacks_t;

/**
 * @if Eng
 * @brief Registers the callback function for enable sle.
 * @par Description: This function is used to register a callback function that reports state of sle.
 * status.
 * @param[in] id The identifier used to distinguish different applications.
 * @param[in] func The callback function to be registered.
 * @retval errcode_t The execution result error code.
 * @par Depends:
 * @li ssf_init.h
 * @else
 * @brief 注册sle协议栈状态上报回调函数。
 * @par Description: 该函数用于注册一个回调函数，用于上报sle协议栈状态。
 * @param[in] app_id 用于区分不同应用的标识。
 * @param[in] cbks 要注册的回调函数。
 * @retval errcode_t 执行结果错误码。
 * @par 依赖：
 * @li ssf_init.h
 * @endif
 */
errcode_t bs_sle_register_stack_state_callbacks(uint32_t app_id, bs_sle_stack_state_callbacks_t *cbks);

/**
 * @if Eng
 * @brief  Enable SLE stack.
 * @par Description: Enable SLE stack.
 * @param NULL
 * @retval error code.
 * @par Depends:
 * @else
 * @brief  使能SLE协议栈。
 * @par Description: 使能SLE协议栈。
 * @param NULL
 * @retval 执行结果错误码。
 * @par 依赖：
 * @endif
 */
errcode_t bs_sle_enable(void);

/**
 * @if Eng
 * @brief  Disable SLE stack.
 * @par Description: Disable SLE stack.
 * @param NULL
 * @retval error code.
 * @par Depends:
 * @else
 * @brief  关闭SLE协议栈。
 * @par Description: 关闭SLE协议栈。
 * @param NULL
 * @retval 执行结果错误码。
 * @par 依赖：
 * @endif
 */
errcode_t bs_sle_disable(void);

#ifdef __cplusplus
}
#endif
#endif /* SSF_INIT_H */