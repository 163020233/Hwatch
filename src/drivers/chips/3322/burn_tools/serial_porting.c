/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Provides burn uart port template \n
 * History: \n
 */
#include <string.h>
#include <stdio.h>
#include "gpio.h"
#include "uart.h"
#include "tcxo.h"
#include "watchdog.h"
#include "pinctrl.h"
#include "uart_porting.h"
#include "soc_osal.h"
#include "osal_adapt.h"
#include "los_ringbuf.h"
#include "serial_port.h"

#define BURN_UART_RECORD_LEN 1024
static osal_ringbuf g_write_Ringbuf = {0};
static osal_ringbuf g_read_Ringbuf = {0};
static int serial_open(u32 baud)
{
    host_burn_uart_reinit(baud);
    CHAR *read_buf = (CHAR *)malloc(BURN_UART_RECORD_LEN);
    CHAR *write_buf = (CHAR *)malloc(BURN_UART_RECORD_LEN);
    if (read_buf == NULL || write_buf == NULL) {
        PRINT("warning: malloc record buffer failed! \r\n");
        return 0;
    }
    osal_ringbuf_init((osal_ringbuf *)&g_read_Ringbuf, read_buf, BURN_UART_RECORD_LEN, OSAL_RBUF_OVERWRITE);
    osal_ringbuf_init((osal_ringbuf *)&g_write_Ringbuf, write_buf, BURN_UART_RECORD_LEN, OSAL_RBUF_OVERWRITE);
    return 0;
}
static int serial_configure(u32 baud)
{
    host_burn_uart_reinit(baud);
    return 0;
}
static int serial_close(void *user)
{
    (void)user;
    host_burn_uart_deinit();
    if (g_read_Ringbuf.fifo != NULL) {
        free(g_read_Ringbuf.fifo);
        g_read_Ringbuf.fifo = NULL;
    }
    if (g_write_Ringbuf.fifo != NULL) {
        free(g_write_Ringbuf.fifo);
        g_write_Ringbuf.fifo = NULL;
    }
    return 0;
}
static int serial_write(void *user, const u8 *data, u32 len)
{
#define MAX_RECORD_LEN 64
    // 返回write的个数
    (void)user;
    int32_t written_len = uapi_uart_write(HOST_BURN_UART_BUS, (const void *)data, len, 0);
    osal_ringbuf_write((osal_ringbuf *)&g_write_Ringbuf, (CHAR*)data, written_len >= MAX_RECORD_LEN ? MAX_RECORD_LEN : written_len);
    return written_len;
}
static int serial_read(void *user, u8 *data, u32 maxlen, u32 timeout_ms)
{
    (void)user;
    int32_t read_len = uapi_uart_read(HOST_BURN_UART_BUS, data, maxlen, timeout_ms);
    osal_ringbuf_write((osal_ringbuf *)&g_read_Ringbuf, (CHAR*)data, read_len);
    return read_len;
}
static uint64_t serial_now_ms(void)
{
    return uapi_tcxo_get_ms();
}
static void serial_sleep_ms(uint32_t ms)
{
    uapi_tcxo_delay_ms(ms);
}

void serial_register_uart_func(burn_host_info_t *info, burn_io_t *io)
{
    if (io == NULL) {
        return;
    }
    io->user = (void *)info;
    io->open = serial_open;
    io->configure = serial_configure;
    io->close = serial_close;
    io->write = serial_write;
    io->read = serial_read;
    io->now_ms = serial_now_ms;
    io->sleep_ms = serial_sleep_ms;
}

void reset_2131_cat1_module(void)
{
    uapi_pin_set_mode(S_AGPIO26, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(S_AGPIO26, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO26, GPIO_LEVEL_LOW);
    uapi_tcxo_delay_us(200); /* 200us */
    uapi_gpio_set_val(S_AGPIO26, GPIO_LEVEL_HIGH);
}

void dump_burn_uart_buffer(void)
{
#define PRINT_LINE_SIZE  16
    PRINT("Latest read cache \r\n");
    CHAR *tmp = g_read_Ringbuf.fifo;
    for (uint32_t i = 0; i < BURN_UART_RECORD_LEN; i++) {
        print_str("%02x ", (unsigned char)tmp[i] & 0xFF);
        if ((i + 1) % PRINT_LINE_SIZE == 0) {
            print_str("\n");
        }
    }
    PRINT("Latest read index = 0x%x \r\n", g_read_Ringbuf.endIdx);
    tmp = g_write_Ringbuf.fifo;
    PRINT("Latest write cache \r\n");
    for (uint32_t i = 0; i < BURN_UART_RECORD_LEN; i++) {
        print_str("%02x ", (unsigned char)tmp[i] & 0xFF);
        if ((i + 1) % PRINT_LINE_SIZE == 0) {
            print_str("\n");
        }
    }
    PRINT("Latest write index = 0x%x \r\n", g_write_Ringbuf.endIdx);
}