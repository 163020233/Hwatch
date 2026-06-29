/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * description: driver for pl010/pl011 uarts
 * author: audio
 */

#include <serial_dw.h>

#include "mem_addr.h"
#include "reg_addr.h"
#include "audio_osal.h"
#include "audio_type.h"
#include "audio_debug.h"

#define UART_BAUDRATE 115200

#define DRV_CFG_REG_DIV_CFG1   (DRV_CFG_SUB_BASE_ADDR + 0x0010)
#define UART_DATA              (DRV_UART_BASE_ADDR + 0x04)
#define UART_CTL               (DRV_UART_BASE_ADDR + 0x08)
#define UART_DIV_H             (DRV_UART_BASE_ADDR + 0x0c)
#define UART_DIV_L             (DRV_UART_BASE_ADDR + 0x10)
#define UART_DIV_FRA           (DRV_UART_BASE_ADDR + 0x14)
#define UART_INTR_EN           (DRV_UART_BASE_ADDR + 0x18)
#define UART_FIFO_CTL          (DRV_UART_BASE_ADDR + 0x24)
#define UART_LINE_STATUS       (DRV_UART_BASE_ADDR + 0x34)
#define UART_FIFO_STATUS       (DRV_UART_BASE_ADDR + 0x44)
#define UART_BAUD_CTL          (DRV_UART_BASE_ADDR + 0x58)
#define UART_RECEIVE_CTL       (DRV_UART_BASE_ADDR + 0x68)

#define BIT(b) (1UL << (b))
#define UART_BIT_AVAILABLE     BIT(5)
#define UART_TX_FIFO_FULL      BIT(0)

static td_bool s_uart_init = 0;

td_void drv_uart_div_enable(td_bool status)
{
    if (status) {
        /* enable uart div */
        audio_reg_bit_wr(DRV_CFG_REG_DIV_CFG1, AUDIO_BIT27, AUDIO_BIT27, 0x1);
    } else {
        /* disable uart div */
        audio_reg_bit_wr(DRV_CFG_REG_DIV_CFG1, AUDIO_BIT27, AUDIO_BIT27, 0x0);
    }

    return;
}

/* baud_rate = UART_SCLK / baud_div / 16 */
td_void drv_uart_div_switch(td_u32 freq)
{
    audio_unused(freq);
    audio_reg_write(UART_DIV_L, 17);    /* DSP_CLK / 16 / UART_DIV = 32000000 / 16 / 17 = 117647 ~= 115200 + 2% */
    audio_reg_write(UART_DIV_H, 0);
    audio_reg_write(UART_DIV_FRA, 0);

    /* set uart clk div(cfg nmanager) */
    audio_reg_write(DRV_CFG_REG_DIV_CFG1, 0x07ffffff);
    audio_reg_write(DRV_CFG_REG_DIV_CFG1, 0x0fffffff);

    return;
}

/*****************************************************************************
 函 数 名  : drv_uart_init
 功能描述  : 串口驱动软件初始化, 建议在上电时初始化
 输入参数  : td_void
 输出参数  : 无
 返 回 值  : td_void
*****************************************************************************/
td_void drv_uart_init(td_void)
{
    audio_reg_write(UART_FIFO_CTL, 0x7f);

    /* 1 stop, no parity, 8bit, no break, en divisor */
    audio_reg_write(UART_CTL, 0x01);
    /* set baudrate 115200 */
    drv_uart_div_switch(UART_BAUDRATE);
    /* 1 stop, no parity, 8bit, no break, disable divisor */
    audio_reg_write(UART_CTL, 0x0c);

    /* rx sample in 7th clock, every bit contains 16 clock */
    audio_reg_write(UART_BAUD_CTL, 0x7f);

    audio_reg_write(UART_INTR_EN, 0x0);

    audio_reg_write(UART_RECEIVE_CTL, 0x1);
    s_uart_init = 1;
}

static td_void drv_uart_pl011putc(const td_char ucdata)
{
    volatile td_u32 reg_fifo_status;

    audio_reg_read(UART_FIFO_STATUS, reg_fifo_status);
    while (reg_fifo_status & UART_TX_FIFO_FULL) {
        audio_reg_read(UART_FIFO_STATUS, reg_fifo_status);
    }

    audio_reg_write(UART_DATA, (ucdata & 0xff));
}

static td_void drv_uart_putc(const td_char ucdata)
{
    if (ucdata == '\n') {
        drv_uart_pl011putc('\r');
    }
    drv_uart_pl011putc(ucdata);
}

td_void drv_uart_puts(const td_char *s)
{
    while (*s) {
        drv_uart_putc(*s++);
    }
}

td_u32 drv_uart_pl011getc(td_char *data)
{
    volatile td_u16 *reg_uart_line_status = (td_u16 *)UART_LINE_STATUS;

    if ((*reg_uart_line_status & UART_BIT_AVAILABLE) == 0) {
        return AUDIO_FAILURE;
    }
    audio_reg_read(UART_DATA, *data);

    return AUDIO_SUCCESS;
}

td_u32 drv_uart_getc(td_char *ucdata)
{
    td_s32 ret;

    if (ucdata != NULL) {
        ret = drv_uart_pl011getc(ucdata);
        if (ret != AUDIO_SUCCESS) {
            return ret;
        }
    }

    return AUDIO_SUCCESS;
}

td_s32 serial_init(td_void)
{
    /* sdk环境使用UART H0，A核已经完成初始化 */
#ifndef SAP_SDK_SUPPORT
    drv_uart_init();
#endif

    return AUDIO_SUCCESS;
}

td_void serial_putc(const td_char ch)
{
    drv_uart_putc(ch);
}

td_void serial_putc_raw(const td_char c)
{
    audio_unused(c);
}

td_void serial_puts(const td_char *s)
{
    drv_uart_puts(s);
}

td_s32 serial_getc(td_void)
{
    td_char ch;
    td_u32 ret;

    ret = drv_uart_getc(&ch);
    if (ret != AUDIO_SUCCESS) {
        ch = '\0';
    }

    return ch;
}

td_s32 serial_tstc(td_void)
{
    return 0;
}

td_void serial_setbrg(td_void)
{
    return;
}
