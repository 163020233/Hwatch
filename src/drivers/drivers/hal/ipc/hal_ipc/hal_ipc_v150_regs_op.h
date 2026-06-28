/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 ipc register operation api \n
 *
 * History: \n
 * 2022-11-15, Create file. \n
 */
#ifndef HAL_IPC_V150_REGS_OP_H
#define HAL_IPC_V150_REGS_OP_H

#include <stdint.h>
#include "hal_ipc_v150_regs_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_ipc_v150_regs_op IPC V150 Regs Operation
 * @ingroup  drivers_hal_ipc
 * @{
 */

extern ipc_soft_int_ctrl_t *g_ipc_soft_int_regs;

/**
 * @brief  Write the value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_en.
 * @param  [in]  val The value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_en.
 */
static inline void hal_ipc_x2self_soft_int_en_write(uint16_t val)
{
    uint32_t real_val = g_ipc_soft_int_regs->x2self_soft_int_en;
    real_val |= val;
    g_ipc_soft_int_regs->x2self_soft_int_en = real_val;
}

/**
 * @brief  Read the value of @ref ipc_soft_int_ctrl_t.x2self_soft_int.
 * @return The value of @ref ipc_soft_int_ctrl_t.x2self_soft_int.
 */
static inline uint16_t hal_ipc_x2self_soft_int_read(void)
{
    return (uint16_t)(g_ipc_soft_int_regs->x2self_soft_int);
}

/**
 * @brief  Read the value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_sts.
 * @return The value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_sts.
 */
static inline uint16_t hal_ipc_x2self_soft_int_sts_read(void)
{
    return (uint16_t)(g_ipc_soft_int_regs->x2self_soft_int_sts);
}

/**
 * @brief  Write the value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_clr.
 * @param  [in]  val The value of @ref ipc_soft_int_ctrl_t.x2self_soft_int_clr.
 */
static inline void hal_ipc_x2self_soft_int_clr_write(uint16_t val)
{
    uint32_t real_val = g_ipc_soft_int_regs->x2self_soft_int_clr;
    real_val |= val;
    g_ipc_soft_int_regs->x2self_soft_int_clr = real_val;
}

/**
 * @brief  Write the value of @ref ipc_soft_int_ctrl_t.self2x_soft_int_sts.
 * @return The value of @ref ipc_soft_int_ctrl_t.self2x_soft_int_sts.
 */
static inline uint16_t hal_ipc_self2x_soft_int_sts_read(void)
{
    return (uint16_t)(g_ipc_soft_int_regs->self2x_soft_int_sts);
}

/**
 * @brief  Write the value of @ref ipc_soft_int_ctrl_t.self2x_soft_int_set.
 * @param  [in]  val The value of @ref ipc_soft_int_ctrl_t.self2x_soft_int_set.
 */
static inline void hal_ipc_self2x_soft_int_set_write(uint16_t val)
{
    uint32_t real_val = g_ipc_soft_int_regs->self2x_soft_int_set;
    real_val |= val;
    g_ipc_soft_int_regs->self2x_soft_int_set = real_val;
}

/**
 * @brief  Write the value of @ref g_ipc_soft_rst_reg.
 * @param  [in]  val The value of @ref g_ipc_soft_rst_reg.
 */
#if defined(CONFIG_IPC_SUPPORT_RST) && (CONFIG_IPC_SUPPORT_RST == 1)
static inline void hal_ipc_self2x_soft_rst_set_write(uint16_t val)
{
    uint32_t real_val = g_ipc_soft_int_regs->self2x_soft_rst_set;
    real_val |= val;
    g_ipc_soft_int_regs->self2x_soft_rst_set = real_val;
}
#endif

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif