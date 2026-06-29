/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description:  bst uart
 */

#ifndef BST_UART_H
#define BST_UART_H

#include "error_types.h"
#include "uart.h"

#define BST_FRAME_DELIMITER        (0x7E)
#define BST_FRAME_DELIMITER_LENGTH (1)
#define BST_FRAME_HEADER_LEN       (8)

typedef void (*bst_uart_callback_t)(const void *cbk_param);

/**
 * @brief  Initialises the uart logger before RTOS functions are available.
 */
void bst_uart_register_rx_callback(uart_rx_callback_t callback);

void bst_uart_init(void);

void bst_msg_write(const uint8_t *buffer, uint16_t length, uart_rx_callback_t finished_with_buffer_func);

void bst_msg_write_int(const uint8_t *buffer, uint16_t length, const void *cbk_param, bst_uart_callback_t cbk);

errcode_t bst_msg_write_dma(const uint8_t *buffer, uint16_t length, const void *cbk_param, bst_uart_callback_t cbk);

errcode_t bst_msg_write_asyn_dma(const uint8_t *buffer, uint16_t length, const void *cbk_param,
                                 bst_uart_callback_t cbk);

#endif
