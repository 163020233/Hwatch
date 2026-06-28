/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: app at uart header file \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-12, Create file. \n
 */
#ifndef APP_AT_UART_H
#define APP_AT_UART_H

#include <stdint.h>
#include <string.h>
#include <uart_porting.h>
#include <errcode.h>
#include <uart.h>

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

typedef void (*at_uart_data_rx)(const uint8_t* data, uint32_t len);
typedef void (*at_uart_data_tx)(uart_bus_t bus, const char* buf, uint16_t length);

void at_uart_print(const char* str, uint32_t len);
void at_uart_printf(const char* format, ...);
void app_at_uart_init(uart_bus_t bus);
void app_at_uart_deinit(void);
void app_at_uart_data_rx_reg(at_uart_data_rx handle);
void at_uart_register_tx(at_uart_data_tx data_tx);
void at_uart_set_bus_num(uart_bus_t bus);

#define at_print(data)                do { (at_uart_print((const char *)(data), strlen(data))); } while (0)

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
