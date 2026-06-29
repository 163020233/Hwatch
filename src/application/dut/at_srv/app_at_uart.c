/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: app at uart source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-10-12, Create file. \n
 */

#include <common_def.h>
#include <securec.h>
#include <platform_core.h>
#include <at_config.h>
#include <stdarg.h>
#include <app_at_uart.h>

#ifndef AT_UART_BAUDRATE
#define AT_UART_BAUDRATE           (115200UL)
#endif

#define SYS_PRINT_TAIL                  "\r\n"
#define UART_PRINTF_BUF_SIZE            (256)
#define AT_UART_RX_BUFFER_SIZE          (AT_CMD_MAX_LENGTH)

#define AT_REMAIN_BUF_LEN UART_PRINTF_BUF_SIZE
uart_bus_t g_app_at_uart = UART_BUS_NONE;
at_uart_data_rx g_at_data_rx = NULL;
uint16_t g_buf_index = 0;
uint8_t g_at_buff[AT_REMAIN_BUF_LEN];
uint8_t g_at_rx_buff[AT_UART_RX_BUFFER_SIZE];
static at_uart_data_tx g_at_uart_data_tx = NULL;

void at_uart_register_tx(at_uart_data_tx data_tx)
{
    g_at_uart_data_tx = data_tx;
}

void app_at_uart_data_rx_reg(at_uart_data_rx handle)
{
    g_at_data_rx = handle;
}

static void at_uart_transmit(uart_bus_t bus, const char* buf, uint16_t length)
{
    if (g_at_uart_data_tx != NULL) {
        g_at_uart_data_tx(bus, buf, length);
    } else {
        uapi_uart_write(bus, (uint8_t*)buf, length, 0);
    }
}

void at_uart_rx_callback(const void *buffer, uint16_t length, bool error)
{
    uint32_t buf_length = AT_REMAIN_BUF_LEN;
    uint16_t buf_index;
    uint16_t buf_offset = 0;

    if (error) {
        if (memcpy_s(g_at_buff, buf_length, buffer, length) != EOK) {
            return;
        }
        g_buf_index = 0;
    } else {
        buf_index = g_buf_index;
        if (memcpy_s(g_at_buff + buf_offset + buf_index, buf_length - buf_offset - buf_index, buffer,
            length) != EOK) {
            return;
        }
        if (g_at_data_rx != NULL) {
            g_at_data_rx(g_at_buff, length + buf_offset + g_buf_index);
        }
        g_buf_index = 0;
        if (memset_s(g_at_buff, sizeof(uint8_t) * buf_length, 0, sizeof(uint8_t) * buf_length) != EOK) {
            return;
        }
    }
}

void at_uart_print(const char* str, uint32_t len)
{
    if (str == NULL || len == 0) {
        return;
    }

    if (g_app_at_uart >= UART_BUS_NONE) {
        return;
    }

    at_uart_transmit(g_app_at_uart, str, len);
}

static char *at_uart_get_buf(va_list args, const char* format, uint32_t *length)
{
    char *buf = NULL;
    buf = (char *)malloc(UART_PRINTF_BUF_SIZE);
    if (buf == NULL) {
        return NULL;
    }

    int32_t len = vsprintf_s(buf, UART_PRINTF_BUF_SIZE, format, args);
    if (len <= 0 || len >= UART_PRINTF_BUF_SIZE) {
        free(buf);
        buf = NULL;
        return NULL;
    }
    buf[len] = 0;

    *length = (uint32_t)len;
    return buf;
}

void at_uart_printf(const char* format, ...)
{
    uint32_t len = 0;
    va_list args;

    va_start(args, format);
    char *buf = at_uart_get_buf(args, format, &len);
    va_end(args);
    if (buf == NULL) {
        return;
    }

    at_uart_print((char*)buf, (uint32_t)len);
    free(buf);
    buf = NULL;
    return;
}

void app_at_uart_init(uart_bus_t bus)
{
    if (bus >= UART_BUS_NONE) {
        return;
    }

    uart_attr_t attr = {
        .baud_rate = AT_UART_BAUDRATE,
        .data_bits = UART_DATA_BIT_8,
        .stop_bits = UART_STOP_BIT_1,
        .parity = UART_PARITY_NONE
    };

    uart_buffer_config_t at_buff_conf = {
        .rx_buffer = g_at_rx_buff,
        .rx_buffer_size = sizeof(g_at_rx_buff),
    };

    errcode_t errcode = uapi_uart_init(bus, NULL, &attr, NULL, &at_buff_conf);
    if (errcode != ERRCODE_SUCC) {
        printf("uart init failed!\n");
        return;
    }

    uapi_uart_register_rx_callback(bus, UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
        sizeof(g_at_rx_buff), at_uart_rx_callback);

    g_app_at_uart = bus;
}

void app_at_uart_deinit(void)
{
    if (g_app_at_uart >= UART_BUS_NONE) {
        return;
    }

    uapi_uart_deinit(g_app_at_uart);
    g_app_at_uart = UART_BUS_NONE;
}

void at_uart_set_bus_num(uart_bus_t bus)
{
    g_app_at_uart = bus;
}

