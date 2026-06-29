/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides ipc port \n
 *
 * History: \n
 * 2022-11-26 Create file. \n
 */

#ifndef IPC_PORTING_H
#define IPC_PORTING_H

#include "platform_types.h"
#include "ipc_ring.h"
#include "ipc_new_shared.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef IPC_USE_OS_DELAY
#define DELAY_NEXT_SEND 10
#endif
/**
 * @brief  Number of IPC communication pipes, the value is equal to the number of cores that communicate with this core.
 */
#define CORE_IPC_TX_PIPE_NUM  (2 * 2)
#define CORE_IPC_RX_PIPE_NUM  (2 * 2)

#define IPC_MESSAGE_MAX_LENGTH 0xC00

/**
 * @brief  IPC TX pipes memory configuration.
 */
extern ipc_memory_manage_t g_ipc_memory_init_tx_config[CORE_IPC_TX_PIPE_NUM];

/**
 * @brief  IPC RX pipes memory configuration.
 */
extern ipc_memory_manage_t g_ipc_memory_init_rx_config[CORE_IPC_RX_PIPE_NUM];

/**
 * @brief  Get the base address of a specified IPC.
 * @return The base address of specified IPC.
 */
uintptr_t ipc_porting_base_addr_get(void);

/**
 * @brief  Register hal funcs objects into hal_ipc module.
 */
void ipc_port_register_hal_funcs(void);

/**
 * @brief  Unregister hal funcs objects into hal_ipc module.
 */
void ipc_port_unregister_hal_funcs(void);

/**
 * @brief  Register the interrupt of ipc.
 */
void ipc_port_register_irq(void);

/**
 * @brief  Unregister the interrupt of ipc.
 */
void ipc_port_unregister_irq(void);

/**
 * @brief  Lock the interrupt of ipc.
 */
uint32_t ipc_port_irq_lock(void);

/**
 * @brief  Lock the interrupt of ipc.
 * @param  [in] status CPSR value before all interrupts are disabled.
 */
void ipc_port_irq_restore(uint32_t status);

/**
 * @brief  Get the IPC interrupt configuration mask based on the peer core.
 * @param  [in]  core The peer core.
 * @return The Mask to be configured based on the peer core.
 */
uint16_t ipc_port_get_int_mask(uint8_t core, uint8_t channel);

/**
 * @brief  Set IPC share memory info.
 * @param  [in]  config ipc config info
 * @param  [in]  cnt info number
 * @param  [dir] ipc direction
 */
void ipc_port_memory_init(ipc_memory_manage_t *config, uint8_t cnt, uint8_t dir);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif