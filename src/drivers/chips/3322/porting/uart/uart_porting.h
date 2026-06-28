/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#ifndef UART_PORT_H
#define UART_PORT_H

#include <stdint.h>
#include "errcode.h"
#include "platform_core.h"
#include "std_def.h"
#include "debug_print.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "hal_dma.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_uart Uart
 * @ingroup  drivers_port
 * @{
 */

#define UART_CLOCK_FRQ 32000000

#define AT_RX_BUFFER_SIZE         32
#define AT_RX_CONDITION_SIZE      ((AT_RX_BUFFER_SIZE * 3) / 4)

#define LOG_UART_RX_BUFFER_SIZE 16

#define SW_UART_BAUDRATE 115200UL
#define UART_L_BAUDRATE  115200UL
#define UART_BUS_MAX_NUM UART_BUS_MAX_NUMBER
#define UART_DIV_INTE_WIDTH 8
#define UART_DIV_FRAC_WIDTH 16

#define UART_SHADOW  YES
#define UART_DLF_SIZE  6
#define UART_FIFO_ENABLED  YES
#define UART_FIFO_SIZE 64
#if defined(CONFIG_UART_SUPPORT_DMA)
#define TEST_UART_DMA_BURST_LEN_MANUAL HAL_DMA_BURST_TRANSACTION_LENGTH_1
#endif

/* the hook function of print_str */
typedef errcode_t (*print_str_hook)(uint8_t *data, uint32_t data_len);

/**
 * @brief  Base address list for all of the IPs.
 */
extern const uintptr_t g_uart_base_addrs[UART_BUS_MAX_NUM];

/**
 * @brief  Register hal funcs objects into hal_uart module.
 */
void uart_port_register_hal_funcs(uart_bus_t bus);

uintptr_t uart_porting_base_addr_get(uart_bus_t bus);

uint32_t uart_porting_div_value_get(uart_bus_t bus);

void uart_porting_div_value_set(uart_bus_t bus, uint32_t value);

/**
 * @brief  Get the bus clock of specified uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The bus clock of specified uart.
 */
uint32_t uart_port_get_clock_value(uart_bus_t bus);

/**
 * @brief  set the bus clock of specified uart.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 */
void uart_port_set_clock_value(uart_bus_t bus, uint32_t clock);

/**
 * @brief  Config the pinmux of the uarts above.
 */
void uart_port_config_pinmux(uart_bus_t bus);

/**
 * @brief  Release the pinmux of the uarts above.
 */
void uart_port_release_pinmux(uart_bus_t bus);

void uart_single_txrx_exchange(bool on);

/**
 * @brief  Register the interrupt of uarts.
 */
void uart_port_register_irq(uart_bus_t bus);

/**
 * @brief  unRegister the interrupt of uarts.
 */
void uart_port_unregister_irq(uart_bus_t bus);

/**
 * @brief  Handler of UART0 IRQ.
 */
void irq_uart0_handler(void);

/**
 * @brief  Handler of UART1 IRQ.
 */
void irq_uart1_handler(void);

/**
 * @brief  Handler of UART2 IRQ.
 */
void irq_uart2_handler(void);

/**
 * @brief  Handler of UART3 IRQ.
 */
void irq_uart3_handler(void);

#if defined(CONFIG_UART_SUPPORT_DMA)
/**
 * @brief  Get the DMA destination handshaking select of uart transfer.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The DMA destination handshaking select of uart transfer.
 */
uint8_t uart_port_get_dma_trans_dest_handshaking(uart_bus_t bus);

/**
 * @brief  Get the DMA source handshaking select of uart transfer.
 * @param  [in]  bus The uart bus. see @ref uart_bus_t
 * @return The DMA source handshaking select of uart transfer.
 */
uint8_t uart_port_get_dma_trans_src_handshaking(uart_bus_t bus);
#if defined(CONFIG_UART_SUPPORT_INT_TRIGGER_DMA)
/**
 * @brief  uart irq enable.
 *
 * @param bus bus.
 */
void uart_porting_int_enable(uart_bus_t bus);

/**
 * @brief  uart irq disable.
 *
 * @param bus bus.
 */
void uart_porting_int_disable(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_INT_TRIGGER_DMA */
#endif  /* CONFIG_UART_SUPPORT_DMA */
void hal_uart_clear_pending(uart_bus_t uart);

/**
 * @brief Force trigger uart interrupt.
 */
void uart_port_set_pending_irq(uart_bus_t uart);

/**
 * @brief  uart lock.
 *
 * @param uart bus.
 */
uint32_t uart_porting_lock(uart_bus_t bus);

/**
 * @brief  uart unlock.
 *
 * @param uart bus.
 */
void uart_porting_unlock(uart_bus_t bus, uint32_t irq_sts);

#ifdef LOG_SUPPORT
/**
 * @brief  hso uart init
 *
 * @param void
 */
void log_uart_port_init(void);

/**
 * @if Eng
 * @brief  log uart reinit.
 * @param  [in]  baud Baud rate of the log UART.
 * @return void
 * @else
 * @brief  log uart重新初始化。
 * @param  [in]  index log uart的波特率。
 * @return void
 * @endif
 */
void log_uart_port_reinit(uint32_t baud);
#endif

void uart_print_str_reg_hook(print_str_hook func);
void uart_port_clock_enable(uart_bus_t bus, bool en);

void host_burn_uart_deinit(void);
void host_burn_uart_reinit(uint32_t baud);
void uart_porting_ip_reset(uart_bus_t bus);
#if defined(CONFIG_UART_SUPPORT_RECV_RAW_DATA)
uint32_t uart_port_get_dma_rx_dest_addr(dma_channel_t channel);
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
