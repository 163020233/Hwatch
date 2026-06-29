/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: tee test (SSB).
 * Author: HiSilicon
 * Create: 2025-10-31
 */

#include <stdbool.h>
#include "tcxo.h"
#include "soc_osal.h"
#include "chip_core_irq.h"
#include "chip_io.h"
#include "debug_print.h"
#include "spi.h"

#ifdef TEE_IP_TEST
#define TEE_SPI_TRANS_LEN 4
static uint8_t g_w_data[TEE_SPI_TRANS_LEN] = {0x11, 0x22, 0x33, 0x44};
static uint8_t g_r_data[TEE_SPI_TRANS_LEN] = {0};

static uint32_t g_timer_start;
static uint32_t g_timer_end;

#define CFG_TEE_TIMER                   0x52018088
#define CFG_TEE_TIMER_H                 0x5201808C
#define CFG_TEE_TIMER_L                 0x52018090
#define TEE_TIMER_CURRENT_VALUE_H       0x52018094
#define TEE_TIMER_CURRENT_VALUE_L       0x52018098

#define M_GP_REG0                       0x52000010

#define TEE_SPI_BUS_CLK     64000000
#define TEE_SPI_FREQ_MHZ    2
static int test_spi_init(void)
{
    spi_attr_t attr = {0};
    spi_extra_attr_t extra_attr = {0};

    attr.is_slave = (bool)0;
    attr.slave_num = 1;
    attr.bus_clk = (uint32_t)TEE_SPI_BUS_CLK;
    attr.freq_mhz = (uint32_t)TEE_SPI_FREQ_MHZ;
    attr.clk_polarity = (uint32_t)0;
    attr.clk_phase = (uint32_t)0;
    attr.frame_format = (uint32_t)SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI;
    attr.spi_frame_format = (uint32_t)HAL_SPI_FRAME_FORMAT_STANDARD;
    attr.frame_size = (uint32_t)HAL_SPI_FRAME_SIZE_32;
    attr.tmod = (uint32_t)HAL_SPI_TRANS_MODE_TXRX;
    attr.ndf = 1;
    attr.sste = 0;

    extra_attr.tx_use_dma = 0;
    extra_attr.rx_use_dma = 0;

    errcode_t ret = uapi_spi_init(SPI_BUS_0, &attr, &extra_attr);
    ret +=uapi_spi_init(SPI_BUS_1, &attr, &extra_attr);
    if (ret != ERRCODE_SUCC) {
        PRINT("uapi_spi_init: 0x%x\n", ret);
        return 1;
    }
    return 0;
}

void tee_timer_callback(void)
{
    uint32_t cur_cnt = (readl(TEE_TIMER_CURRENT_VALUE_H) << 16) | readl(TEE_TIMER_CURRENT_VALUE_L);
    writew(CFG_TEE_TIMER, 0x3);
    writew(CFG_TEE_TIMER, 0x0);
    osal_irq_clear(TEE_TIMER_INTR_IRQN);
    g_timer_end = uapi_tcxo_get_ms();
    PRINT("tee irq tcxo timeout ms: %u  tee current timeout count:%u\n", g_timer_end - g_timer_start, cur_cnt);
}

void tee_test_timer(void)
{
    // tee timer
    osal_irq_request(TEE_TIMER_INTR_IRQN, (osal_irq_handler)tee_timer_callback, NULL, NULL, NULL);
    osal_irq_enable(TEE_TIMER_INTR_IRQN);
    writew(CFG_TEE_TIMER, 0x1);
    writew(CFG_TEE_TIMER_H, 0x5);
    g_timer_start = uapi_tcxo_get_ms();
}

void tee_test_spi(void)
{
    test_spi_init();

    // spi0 pinmux
    uapi_pin_set_mode(S_MGPIO12, (pin_mode_t)HAL_PIO_FUNC_SPI0_M1);
    uapi_pin_set_mode(S_MGPIO13, (pin_mode_t)HAL_PIO_FUNC_SPI0_M1);
    uapi_pin_set_mode(S_MGPIO14, (pin_mode_t)HAL_PIO_FUNC_SPI0_M1);
    uapi_pin_set_mode(S_MGPIO15, (pin_mode_t)HAL_PIO_FUNC_SPI0_M1);
    uapi_pin_set_mode(S_MGPIO16, (pin_mode_t)HAL_PIO_FUNC_SPI0_M1);

    // spi1 pinmux
    uapi_pin_set_mode(S_MGPIO23, (pin_mode_t)HAL_PIO_FUNC_SPI1_M1);
    uapi_pin_set_mode(S_MGPIO24, (pin_mode_t)HAL_PIO_FUNC_SPI1_M1);
    uapi_pin_set_mode(S_MGPIO25, (pin_mode_t)HAL_PIO_FUNC_SPI1_M1);
    uapi_pin_set_mode(S_MGPIO26, (pin_mode_t)HAL_PIO_FUNC_SPI1_M1);

    spi_xfer_data_t data = {
        .tx_buff = g_w_data,
        .tx_bytes = TEE_SPI_TRANS_LEN,
        .rx_buff = g_r_data,
        .rx_bytes = TEE_SPI_TRANS_LEN
    };
    PRINT("spi transfor\n");
    writew(M_GP_REG0, 0xff);
    while (1) {
        if (readl(M_GP_REG0) == 0xff) {
            continue;
        }
        PRINT("uapi_spi_master_writeread\n");
        errcode_t ret = uapi_spi_master_writeread(readl(M_GP_REG0), &data, 0xFFFFFFFF);
        PRINT("bus:%d    read: %d %d %d %d\n", readl(M_GP_REG0), g_r_data[0], g_r_data[0x1],
              g_r_data[0x2], g_r_data[0x3]);
        if (ret != ERRCODE_SUCC) {
            PRINT("spi_master_writeread_poll failed, ret = %x\n", ret);
        }
        writew(M_GP_REG0, 0xff);
    }
}
#endif