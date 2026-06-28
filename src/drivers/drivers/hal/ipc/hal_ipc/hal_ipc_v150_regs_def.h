/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V150 ipc register \n
 *
 * History: \n
 * 2022-11-15, Create file. \n
 */
#ifndef HAL_IPC_V150_REGS_DEF_H
#define HAL_IPC_V150_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_ipc_v150_regs_def IPC V150 Regs Definition
 * @ingroup  drivers_hal_ipc
 * @{
 */

/**
 * @brief  Registers associated with IPC interrupts.
 */
typedef struct {
    uint32_t x2self_soft_int_en;    /*!< XXX cores -> this core soft-interrupt-enable register.  <i>Offset: 00h</i>. */
    uint32_t x2self_soft_int;       /*!< XXX cores -> this core soft-interrupt register.         <i>Offset: 04h</i>. */
    uint32_t x2self_soft_int_sts;   /*!< XXX cores -> this core soft-interrupt-status register.  <i>Offset: 08h</i>. */
    uint32_t x2self_soft_int_clr;   /*!< XXX cores -> this core soft-interrupt-clear register.   <i>Offset: 0Ch</i>. */
    uint32_t self2x_soft_int_sts;   /*!< This core -> XXX cores soft-interrupt-status register.  <i>Offset: 10h</i>. */
    uint32_t self2x_soft_int_set;   /*!< This core -> XXX cores soft-interrupt-set register.     <i>Offset: 14h</i>. */
    uint32_t self2x_soft_rst_set;   /*!< This core -> XXX cores soft-reset register.             <i>Offset: 18h</i>. */
} ipc_soft_int_ctrl_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif