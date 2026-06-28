/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: Lcd private interface.
 *
 * Create: 2022-03-05
 */

#include "lcd_private.h"
#include "tcxo.h"
#include "chip_io.h"
#include "platform_core.h"
#include "gpio.h"
#include "pinctrl_porting.h"
#include "product.h"

#ifdef GPU_FPGA_3321
#define RESET_LCD_GPIO        S_HGPIO0
#define LCD_RSTN_REG          0x5600201C /* fpga use reg bit to connect rst pin */
#elif defined(GPU_FPGA_1A21)
#define LCD_RSTN_REG          0x520d101c
#elif defined(GPU_FPGA_3322)
#define LCD_RSTN_REG          0x520c101c
#elif defined(GPU_FPGA_1A22)
#define LCD_RSTN_REG          0x535d3010
#endif

#ifdef GPU_FPGA_1A22
#define MIPI_LCD_RST_BIT 28
#else
#define MIPI_LCD_RST_BIT 0
#endif

#ifdef GPU_1A21
void lcd_gpio_set_dir(pin_t pin, gpio_direction_t dir)
{
    gpio_porting_service_core_handle(pin, OP_SET_DIR, dir);
}

void lcd_gpio_set_val(pin_t pin, gpio_level_t level)
{
    gpio_porting_service_core_handle(pin, OP_SET_LEVEL, level);
}
#else
void lcd_gpio_set_dir(pin_t pin, gpio_direction_t dir)
{
    uapi_gpio_set_dir(pin, dir);
}

void lcd_gpio_set_val(pin_t pin, gpio_level_t level)
{
    uapi_gpio_set_val(pin, level);
}
#endif

static void lcd_drv_power_on(void)
{
#if defined(PRE_FPGA)  && !defined(SUPPORT_GPU_QSPI) || defined(GPU_FPGA_1A22)
    reg32_setbit(LCD_RSTN_REG, 26); /* 26vci en */
    reg32_clrbit(LCD_RSTN_REG, MIPI_LCD_RST_BIT);
    uapi_tcxo_delay_us(20); /* delay 20us */
    reg32_setbit(LCD_RSTN_REG, MIPI_LCD_RST_BIT);
    uapi_tcxo_delay_ms(30); /* delay 30ms */
#else
#if defined(SUPPORT_GPU_QSPI) || defined(CONFIG_PRODUCT_EVB_DITING)
    lcd_gpio_set_dir(RESET_LCD_GPIO, GPIO_DIRECTION_OUTPUT);
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_LOW);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
    lcd_gpio_set_dir(VCI_LCD_GPIO, GPIO_DIRECTION_OUTPUT);
    lcd_gpio_set_val(VCI_LCD_GPIO, GPIO_LEVEL_LOW);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
    lcd_gpio_set_val(VCI_LCD_GPIO, GPIO_LEVEL_HIGH);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_HIGH);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
#else
    lcd_gpio_set_dir(VCI_LCD_GPIO, GPIO_DIRECTION_OUTPUT);
    lcd_gpio_set_val(VCI_LCD_GPIO, GPIO_LEVEL_HIGH);
    lcd_gpio_set_dir(RESET_LCD_GPIO, GPIO_DIRECTION_OUTPUT);
#ifdef GPU_SUPPORT_VIDEO_MODE
    uapi_tcxo_delay_ms(20); /* delay 20 ms */
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_LOW);
    uapi_tcxo_delay_ms(20); /* delay 20 ms */
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_HIGH);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
#else
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_LOW);
    uapi_tcxo_delay_ms(15); /* delay 15 ms */
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_HIGH);
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
#endif
#endif
#endif
}

static void lcd_drv_power_off(void)
{
#if (defined(PRE_FPGA) && !defined(SUPPORT_GPU_QSPI)) || defined(GPU_FPGA_1A22)
    reg32_clrbit(LCD_RSTN_REG, 0);
    uapi_tcxo_delay_ms(5); /* 5000: delay 5ms */
#else
    lcd_gpio_set_val(RESET_LCD_GPIO, GPIO_LEVEL_LOW);
    lcd_gpio_set_dir(RESET_LCD_GPIO, GPIO_DIRECTION_INPUT);
#ifndef SUPPORT_GPU_QSPI
    lcd_gpio_set_val(VCI_LCD_GPIO, GPIO_LEVEL_LOW);
    lcd_gpio_set_dir(VCI_LCD_GPIO, GPIO_DIRECTION_INPUT);
#endif
    uapi_tcxo_delay_ms(10); /* delay 10 ms */
#endif
}

static lcd_drv_private g_lcd_private_api = {
    .power_on = lcd_drv_power_on,
    .power_off = lcd_drv_power_off,
};

lcd_drv_private *lcd_drv_get_private_api(void)
{
    return &g_lcd_private_api;
}
