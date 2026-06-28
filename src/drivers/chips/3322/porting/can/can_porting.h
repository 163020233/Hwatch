/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide CAN-FD port template. \n
 *
 * History: \n
 * 2023-02-24, Create file. \n
 */
#ifndef CAN_PORTING_H
#define CAN_PORTING_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */


/**
 * @defgroup drivers_port_can CAN
 * @ingroup  drivers_port
 * @{
 */

typedef enum {
    CAN_BUS_0,
    CAN_BUS_MAX
} can_bus_t;

/**
 * @if Eng
 * @brief  Get base address for CAN-FD registers.
 * @else
 * @brief  获取CAN-FD寄存器基地址。
 * @endif
 */
uintptr_t can_porting_get_reg_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD registers.
 * @else
 * @brief  获取CAN-FD SRAM基地址。
 * @endif
 */
uintptr_t can_porting_get_sram_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD standard filter.
 * @else
 * @brief  获取CAN-FD标准滤波配置区域基地址。
 * @endif
 */
uintptr_t can_porting_get_std_filter_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD extended filter.
 * @else
 * @brief  获取CAN-FD扩展滤波配置区域基地址。
 * @endif
 */
uintptr_t can_porting_get_ex_filter_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD Rx FIFO 0.
 * @else
 * @brief  获取CAN-FD Rx FIFO 0基地址。
 * @endif
 */
uintptr_t can_porting_get_rx_fifo_0_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD Rx FIFO 1.
 * @else
 * @brief  获取CAN-FD Rx FIFO 1基地址。
 * @endif
 */
uintptr_t can_porting_get_rx_fifo_1_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD Tx FIFO.
 * @else
 * @brief  获取CAN-FD Tx FIFO基地址。
 * @endif
 */
uintptr_t can_porting_get_tx_fifo_base_addr(void);

/**
 * @if Eng
 * @brief  Get base address for CAN-FD standard filter.
 * @else
 * @brief  获取CAN-FD标准滤波配置区域基地址
 * @endif
 */
uintptr_t can_porting_get_tx_event_base_addr(void);

/**
 * @if Eng
 * @brief  Register hal interrupt handler.
 * @else
 * @brief  注册hal层中断控制器。
 * @endif
 */
void can_porting_regitser_irq(void);

/**
 * @if Eng
 * @brief  unregister hal interrupt handler.
 * @else
 * @brief  解注册hal层中断控制器。
 * @endif
 */
void can_porting_unregitser_irq(void);

/**
 * @if Eng
 * @brief  CAN power-on and clock configuration.
 * @else
 * @brief  CAN 上电和时钟配置。
 * @endif
 */
void can_porting_pwr_on(void);

/**
 * @if Eng
 * @brief  CAN power-off and clock configuration.
 * @else
 * @brief  CAN 下电和时钟配置。
 * @endif
 */
void can_porting_pwr_off(void);

/**
 * @if Eng
 * @brief  CAN get segment ticks
 * @else
 * @brief  CAN 段时间计算。
 * @endif
 */
uint32_t can_porting_get_seg_ticks(uint32_t baud_rate);

/**
 * @if Eng
 * @brief  CAN get current time.
 * @else
 * @brief  CAN 获取当前时间。
 * @endif
 */
uint64_t can_porting_get_cur_time(void);

/**
 * @if Eng
 * @brief  CAN get timeout duration.
 * @else
 * @brief  CAN 获取超时时间。
 * @endif
 */
uint32_t can_porting_get_timeout_duration(void);

/**
 * @if Eng
 * @brief  CAN init lock.
 * @else
 * @brief  CAN 初始化锁。
 * @endif
 */
void can_port_lock_init(void);

/**
 * @if Eng
 * @brief  CAN get lock.
 * @else
 * @brief  CAN 获取锁。
 * @endif
 */
uint32_t can_port_get_lock(void);

/**
 * @if Eng
 * @brief  CAN restore lock.
 * @else
 * @brief  CAN 释放锁。
 * @endif
 */
void can_port_restore_lock(uint32_t lock_sts);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif