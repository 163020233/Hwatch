/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides i2c port template \n
 *
 * History: \n
 * 2022-08-15， Create file. \n
 */

#include "i2c_porting.h"
#include "hal_i2c.h"
#include "hal_i2c_v151.h"
#include "hal_i2c_v151_comm.h"
#include "pinctrl.h"
#include "osal_interrupt.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#if defined(CONFIG_I2C_SUPPORT_DMA) && (CONFIG_I2C_SUPPORT_DMA == 1)
#include "dma_porting.h"
#endif
#include "pm_clocks.h"

#define BUS_CLOCK_TIME_32M 32000000UL
#define CON                     0x0
#define TAR                     0x4
#define SAR                     0x8
#define HS_MADDR                0xc
#define SS_SCL_HCNT             0x14
#define SS_SCL_LCNT             0x18
#define FS_SCL_HCNT             0x1C
#define FS_SCL_LCNT             0x20
#define HS_SCL_HCNT             0x24
#define HS_SCL_LCNT             0x28
#define INTR_MASK               0x30
#define RX_TL                   0x38
#define TX_TL                   0x3C
#define ENABLE                  0x6C
#define SDA_HOLD                0x7C
#define SLAVE_DATA_NACK_ONLY    0x84
#define DMA_CR                  0x88
#define DMA_TDLR                0x8C
#define DMA_RDLR                0x90
#define DMA_SETUP               0x94
#define ACK_GENERAL_CALL        0x98

typedef void (*i2c_porting_irq_handler)(void);

typedef struct i2c_irq_handler {
    uint32_t irq_id;
    i2c_porting_irq_handler irq_handler;
} i2c_irq_handler_t;

uintptr_t g_i2c_base_addrs[I2C_BUS_MAX_NUM] = {
#if I2C_BUS_MAX_NUM >= 1
    (uintptr_t)I2C_BUS_0_BASE_ADDR,
#endif
#if I2C_BUS_MAX_NUM >= 2
    (uintptr_t)I2C_BUS_1_BASE_ADDR,
#endif
#if I2C_BUS_MAX_NUM >= 3
    (uintptr_t)I2C_BUS_2_BASE_ADDR,
#endif
#if I2C_BUS_MAX_NUM >= 4
    (uintptr_t)I2C_BUS_3_BASE_ADDR,
#endif
};

uintptr_t i2c_porting_base_addr_get(i2c_bus_t bus)
{
    return g_i2c_base_addrs[bus];
}

#if I2C_BUS_MAX_NUM >= 1
    static void irq_i2c0_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_0);
}
#endif
#if I2C_BUS_MAX_NUM >= 2
static void irq_i2c1_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_1);
}
#endif

#if I2C_BUS_MAX_NUM >= 3
static void irq_i2c2_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_2);
}
#endif

#if I2C_BUS_MAX_NUM >= 4
static void irq_i2c3_handler(void)
{
    hal_i2c_v151_irq_handler(I2C_BUS_3);
}
#endif

static i2c_irq_handler_t g_i2c_irq_id[I2C_BUS_MAX_NUM] = {
#if I2C_BUS_MAX_NUM >= 1
    {I2C_BUS_0_IRQN, irq_i2c0_handler},
#endif
#if I2C_BUS_MAX_NUM >= 2
    {I2C_BUS_1_IRQN, irq_i2c1_handler},
#endif
#if I2C_BUS_MAX_NUM >= 3
    {I2C_BUS_2_IRQN, irq_i2c2_handler},
#endif
#if I2C_BUS_MAX_NUM >= 4
    {I2C_BUS_3_IRQN, irq_i2c3_handler},
#endif
};

uint32_t i2c_port_get_clock_value(i2c_bus_t bus)
{
    if (bus >= I2C_BUS_MAX_NUM) {
        return 0;
    }
    return BUS_CLOCK_TIME_32M;
}

void i2c_port_register_irq(i2c_bus_t bus)
{
    i2c_irq_handler_t irq = g_i2c_irq_id[bus];
    osal_irq_request(irq.irq_id, (osal_irq_handler)irq.irq_handler, NULL, NULL, NULL);
    osal_irq_enable(irq.irq_id);
}

void i2c_port_unregister_irq(i2c_bus_t bus)
{
    i2c_irq_handler_t irq = g_i2c_irq_id[bus];
    osal_irq_free(irq.irq_id, NULL);
}

uint32_t i2c_porting_lock(i2c_bus_t bus)
{
    unused(bus);
    return osal_irq_lock();
}

void i2c_porting_unlock(i2c_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
    osal_irq_restore(irq_sts);
}

#if defined(CONFIG_I2C_SUPPORT_DMA)
uint8_t i2c_port_get_dma_trans_dest_handshaking(i2c_bus_t bus)
{
    switch (bus) {
#if I2C_BUS_MAX_NUM >= 1
        case I2C_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_0_TX;
#endif
#if I2C_BUS_MAX_NUM >= 2
        case I2C_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_1_TX;
#endif
#if I2C_BUS_MAX_NUM >= 3
        case I2C_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_2_TX;
#endif
#if I2C_BUS_MAX_NUM >= 4
        case I2C_BUS_3:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_3_TX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t i2c_port_get_dma_trans_src_handshaking(i2c_bus_t bus)
{
    switch (bus) {
#if I2C_BUS_MAX_NUM >= 1
        case I2C_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_0_RX;
#endif
#if I2C_BUS_MAX_NUM >= 2
        case I2C_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_1_RX;
#endif
#if I2C_BUS_MAX_NUM >= 3
        case I2C_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_2_RX;
#endif
#if I2C_BUS_MAX_NUM >= 4
        case I2C_BUS_3:
            return (uint8_t)DMA_HANDSHAKE_I2C_BUS_3_RX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}
#endif  /* CONFIG_I2C_SUPPORT_DMA */

void i2c_port_set_sda_tx_hold_value(i2c_bus_t bus, uint32_t val)
{
    hal_i2c_v151_set_ic_disable(bus);
    hal_i2c_v151_set_sda_tx_hold_value(bus, val);
    hal_i2c_v151_set_ic_enable(bus);
}

#ifdef TEST_SUITE
void i2c_port_test_i2c_init_pin(void)
{
    uapi_pin_set_mode(S_MGPIO4, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO5, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO6, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO7, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO8, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO9, PIN_MODE_1);
}
#endif

#if defined(CONFIG_I2C_SUPPORT_LPC)
void i2c_port_clock_enable(i2c_bus_t bus, bool on)
{
    switch (bus) {
        case I2C_BUS_0:
            uapi_clocks_switch(CLOCKS_ID_M_I2C0, (switch_type_t)on);
            break;
        case I2C_BUS_1:
            uapi_clocks_switch(CLOCKS_ID_M_I2C1, (switch_type_t)on);
            break;
        case I2C_BUS_2:
            uapi_clocks_switch(CLOCKS_ID_M_I2C2, (switch_type_t)on);
            break;
        case I2C_BUS_3:
            uapi_clocks_switch(CLOCKS_ID_M_I2C3, (switch_type_t)on);
            break;
        default:
            break;
    }
}
#endif
