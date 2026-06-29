/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides spi port UT \n
 *
 * History: \n
 * 2022-08-18， Create file. \n
 */
#include "spi_porting.h"
#include "hal_spi_v151_regs_def.h"
#include "hal_spi_v151_regs_op.h"
#include "hal_spi_v151.h"
#include "hal_spi.h"
#ifdef CONFIG_SPI_SUPPORT_DSP
#include "audio_osal.h"
#else
#include "osal_interrupt.h"
#include "pinctrl.h"
#include "chip_io.h"
#include "std_def.h"
#include "platform_core.h"
#include "arch_port.h"
#include "pm_clocks.h"
#endif

/** -----------------------------------------------------
 *          Reg Bit fild Max and Shift Value
 * ---------------------------------------------------- */
 /**
  * @brief  CTRLR0 : Control register 0.
  */
#define HAL_SPI_CTRLR0_REG_MAX       0xFFFFFFFF
#define HAL_SPI_CE_LIN_TOGGLE_ENABLE (BIT(24))

/**
 * @brief  FRF : Frame format.
 */
#define HAL_SPI_FRAME_FORMAT_MAX   0x03
#define HAL_SPI_FRAME_FORMAT_SHIFT 0x15

/**
 * @brief  DFS : Data frame size.
 */
#define HAL_SPI_FRAME_SIZE_MAX   0x1F
#define HAL_SPI_FRAME_SIZE_SHIFT 0x10
#define HAL_SPI_FRAME_SIZE_8     0x07
#define HAL_SPI_FRAME_SIZE_16    0x0F
#define HAL_SPI_FRAME_SIZE_32    0x1F

/**
 * @brief  TMOD : Trans mode.
 */
#define HAL_SPI_TRANS_MODE_MAX    0x03
#define HAL_SPI_TRANS_MODE_SHIFT  0x08
#define HAL_SPI_TRANS_MODE_TXRX   0x00
#define HAL_SPI_TRANS_MODE_TX     0x01
#define HAL_SPI_TRANS_MODE_RX     0x02
#define HAL_SPI_TRANS_MODE_EEPROM 0x03

/**
 * @brief  CLOCK : SCPOL and SCPH.
 */
#define HAL_SPI_CLKS_MODE_MAX   0x03
#define HAL_SPI_CLKS_MODE_SHIFT 0x06

/**
 * @brief  CTRLR1 : Control register 1.
 */
#define HAL_SPI_RECEIVED_DATA_REG_MAX 0xFFFF

/**
 * @brief  SSIENR : SSI enable register.
 */
#define HAL_SPI_ENABLE 0x01

/**
 * @brief  SER : Slave enable register.
 */
#define HAL_SPI_SLAVE_ENABLE_REG_MAX 0xFFFFFFFF

/**
 * @brief  BAUDR : Baud rate select.
 */
#define HAL_SPI_CLK_DIV_REG_MAX 0xFFFF

/**
 * @brief  SR : Status register.
 */
#define HAL_SPI_RX_FIFO_FULL_FLAG      (BIT(4))
#define HAL_SPI_RX_FIFO_NOT_EMPTY_FLAG (BIT(3))
#define HAL_SPI_TX_FIFO_EMPTY_FLAG     (BIT(2))
#define HAL_SPI_TX_FIFO_NOT_FULL_FLAG  (BIT(1))
#define HAL_SPI_BUSY_FLAG              (BIT(0))

/**
 * @brief IMR : Interrupt mask register.
 */
#define HAL_SPI_INTERRUPT_REG_MAX 0x3F


/**
 * @brief  SPI_CTRLR0:  SPI control register.
 */
#define HAL_QSPI_CTRLR0_REG_MAX 0xFFFFFFFF

/**
 * @brief  Wait cycles.
 */
#define HAL_QSPI_WAIT_CYCLE_MAX   0x1F
#define HAL_QSPI_WAIT_CYCLE_SHIFT 0x0B
#define HAL_QSPI_WAIT_CYCLE_6     0x06
#define HAL_QSPI_WAIT_CYCLE_4     0x04
#define HAL_QSPI_WAIT_CYCLE_2     0x02

#define HAL_SPI_RX_SAMPLE_DLY_MAX   0xFF
#define HAL_SPI_RX_SAMPLE_DLY_SHIFT 0

/**
 * @brief  Command length.
 */
#define HAL_QSPI_CMD_LENTH_MAX   0x03
#define HAL_QSPI_CMD_LENTH_SHIFT 0x08
#define HAL_QSPI_CMD_LENTH_8     0x02

/**
 * @brief  Address length.
 */
#define HAL_QSPI_ADDR_LENTH_MAX   0x0F
#define HAL_QSPI_ADDR_LENTH_SHIFT 0x02
#define HAL_QSPI_ADDR_LENTH_24    0x06

/**
 * @brief  Trans type.
 */
#define HAL_QSPI_TRANS_TYPES_MAX          0x03
#define HAL_QSPI_TRANS_TYPES_SHIFT        0x00
#define HAL_QSPI_TRANS_TYPES_CMD_S_ADDR_Q 0x01
#define HAL_SPI_DR_REG_SIZE 36

/** -----------------------------------------------------
 *          SPI Register Address
 * ----------------------------------------------------
 */
#define HAL_QSPI_DMA_CFG        (*(volatile unsigned short *)(0x5C000404))
#define HAL_QSPI_1_DMA_CFG_MASK 0x0FU
#define HAL_QSPI_1_DMA_SEC_CORE 0x05
#define HAL_QSPI_1_DMA_APP_CORE 0x0A
#define HAL_QSPI_1_DMA_DSP_CORE 0x0F
#define HAL_QSPI_2_DMA_CFG_MASK 0xF0U
#define HAL_QSPI_2_DMA_SEC_CORE 0x50
#define HAL_QSPI_2_DMA_APP_CORE 0xA0
#define HAL_QSPI_2_DMA_DSP_CORE 0xF0

#define HAL_QSPI_INT_CFG             (*(volatile unsigned short *)(0x5C000408))
#define HAL_QSPI_1_INT_SEC_CORE_MASK (BIT(0))
#define HAL_QSPI_1_INT_APP_CORE_MASK (BIT(2))
#define HAL_QSPI_2_INT_SEC_CORE_MASK (BIT(4))
#define HAL_QSPI_2_INT_APP_CORE_MASK (BIT(6))

#define HAL_SPI_DATA_FRAME_SIZE_BIT     16
#define HAL_SPI_DATA_FRAME_SIZE_BITFILD 5
#define hal_spi_frame_size_trans_to_frame_bytes(x) (((x) + 1) >> 0x03)

#define HAL_SPI_MINUMUM_CLK_DIV 2
#define HAL_SPI_MAXIMUM_CLK_DIV 65534

#define hal_spi_mhz_to_hz(x) ((x) * 1000000)

#define HAL_SPI_RXDS_EN     BIT(18)
#define HAL_SPI_INST_DDR_EN BIT(17)
#define HAL_SPI_DDR_EN      BIT(16)
#define HAL_SPI_INST_L_POSE 8
#define HAL_SPI_ADDR_L_POSE 2

#define HAL_SPI_RSVD_NONE   0
#define HAL_SPI_RSVD_X8     0
#define HAL_SPI_RSVD_X8_X8  1
#define HAL_SPI_RSVD_X16    0x11
#define HAL_SPI_CLK_DIV_2   2
#define HAL_SPI_CLK_DIV_4   4
#define HAL_SPI_CLK_DIV_20  20

#define HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_SHIFT 0x2

#define CTRLR0          0x0
#define CTRLR1          0x4
#define SSIENR          0x8
#define SER             0x10
#define BAUDR           0x14
#define TXFTLR          0x18
#define RXFTLR          0x1C
#define IMR             0x2C
#define DMACR           0x4C
#define DMATDLR         0x50
#define DMARDLR         0x54
#define RX_SAMPLE_DLY   0xF0
#define SPI_CTRLR0      0xF4

static uint32_t g_spi_max_slave_select[SPI_BUS_MAX_NUM] = {
    SPI_SLAVE2,
    SPI_SLAVE2,
};

spi_v151_regs_t *g_spi_base_addrs[SPI_BUS_MAX_NUM] = {
#if SPI_BUS_MAX_NUM >= 1
    (spi_v151_regs_t *)SPI_BUS_0_BASE_ADDR,
#endif
#if SPI_BUS_MAX_NUM >= 2
    (spi_v151_regs_t *)SPI_BUS_1_BASE_ADDR,
#endif
#if SPI_BUS_MAX_NUM >= 3
    (spi_v151_regs_t *)SPI_BUS_2_BASE_ADDR,
#endif
};

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
#if SPI_BUS_MAX_NUM >= 1
static int irq_spi0_handler(int i, void *p);
#endif

#if SPI_BUS_MAX_NUM >= 2
static int irq_spi1_handler(int i, void *p);
#endif

#if SPI_BUS_MAX_NUM >= 3
static int irq_spi2_handler(int i, void *p);
#endif

typedef struct spi_interrupt {
    core_irq_t irq_num;
    osal_irq_handler irq_func;
} hal_spi_interrupt_t;

static const hal_spi_interrupt_t g_spi_interrupt_lines[SPI_BUS_MAX_NUM] = {
#if SPI_BUS_MAX_NUM >= 1
    { SPI_BUS_0_IRQN, irq_spi0_handler},
#endif
#if SPI_BUS_MAX_NUM >= 2
    { SPI_BUS_1_IRQN, irq_spi1_handler},
#endif
#if SPI_BUS_MAX_NUM >= 3
    { SPI_BUS_2_IRQN, irq_spi2_handler},
#endif
};

#if SPI_BUS_MAX_NUM >= 1
static int irq_spi0_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_spi_v151_irq_handler(SPI_BUS_0);
    osal_irq_clear(g_spi_interrupt_lines[SPI_BUS_0].irq_num);
    return 0;
}
#endif

#if SPI_BUS_MAX_NUM >= 2
static int irq_spi1_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_spi_v151_irq_handler(SPI_BUS_1);
    osal_irq_clear(g_spi_interrupt_lines[SPI_BUS_1].irq_num);
    return 0;
}
#endif

#if SPI_BUS_MAX_NUM >= 3
static int irq_spi2_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_spi_v151_irq_handler(SPI_BUS_2);
    osal_irq_clear(g_spi_interrupt_lines[SPI_BUS_2].irq_num);
    return 0;
}
#endif
#endif

uintptr_t spi_porting_base_addr_get(spi_bus_t bus)
{
    return (uintptr_t)g_spi_base_addrs[bus];
}

uint32_t spi_porting_max_slave_select_get(spi_bus_t bus)
{
    return g_spi_max_slave_select[bus];
}

void spi_porting_set_device_mode(spi_bus_t bus, spi_mode_t mode)
{
#ifdef CONFIG_SPI_SUPPORT_DSP
    return;
#else
    regw_setbit(M_CTL_RB_SPI_CFG_REG, 0, M_CTL_RB_MCU_SPI1_CS_MODE_OFFSET);
    regw_setbit(M_CTL_RB_SPI_CFG_REG, 0, M_CTL_RB_MCU_SPI0_CS_MODE_OFFSET);
    switch (bus) {
#if SPI_BUS_MAX_NUM >= 1
        case SPI_BUS_0:
            regw_setbits(M_CTL_RB_SPI_CFG_REG, 0x0, M_CTL_RB_SPI0_MS_SLAVE_MODE_OFFSET, 0x1, mode);
            break;
#endif
#if SPI_BUS_MAX_NUM >= 2
        case SPI_BUS_1:
            regw_setbits(M_CTL_RB_SPI_CFG_REG, 0x0, M_CTL_RB_SPI1_MS_SLAVE_MODE_OFFSET, 0x1, mode);
            break;
#endif
#if SPI_BUS_MAX_NUM >= 3
        case SPI_BUS_2:
            regw_setbits(M_CTL_RB_SPI_CFG_REG, 0x0, HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_SHIFT, 0x1, mode);
            break;
#endif
        default:
            break;
    }
#endif
}

spi_mode_t spi_porting_get_device_mode(spi_bus_t bus)
{
#ifdef CONFIG_SPI_SUPPORT_DSP
    return SPI_MODE_MASTER;
#else
    spi_mode_t mode = SPI_MODE_NONE;

    switch (bus) {
#if SPI_BUS_MAX_NUM >= 1
        case SPI_BUS_0:
            mode = (regw_getbits(M_CTL_RB_SPI_CFG_REG, 0x0, M_CTL_RB_SPI0_MS_SLAVE_MODE_OFFSET, 0x1) &
                (uint16_t)1) ? SPI_MODE_SLAVE : SPI_MODE_MASTER;
            break;
#endif
#if SPI_BUS_MAX_NUM >= 2
        case SPI_BUS_1:
            mode = (regw_getbits(M_CTL_RB_SPI_CFG_REG, 0x0, M_CTL_RB_SPI1_MS_SLAVE_MODE_OFFSET, 0x1) &
                (uint16_t)1) ? SPI_MODE_SLAVE : SPI_MODE_MASTER;
            break;
#endif
#if SPI_BUS_MAX_NUM >= 3
        case SPI_BUS_2:
            mode = (regw_getbits(M_CTL_RB_SPI_CFG_REG, 0x0, HAL_SPI_BUS_2_DEVICE_MODE_SET_BIT_SHIFT, 0x1) &
                (uint16_t)1) ? SPI_MODE_SLAVE : SPI_MODE_MASTER;
            break;
#endif
        default:
            break;
    }

    return mode;
#endif
}

#ifdef TEST_SUITE
/* brandy SPI testsuit pinctrl config, here we use bus_2 by default. */
void spi_porting_test_spi_init_pin(void)
{
    uapi_pin_set_mode(S_MGPIO12, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO13, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO14, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO15, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO23, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO24, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO25, PIN_MODE_1);
    uapi_pin_set_mode(S_MGPIO26, PIN_MODE_1);
}
#endif

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
uint8_t spi_port_get_dma_trans_dest_handshaking(spi_bus_t bus)
{
    switch (bus) {
#if SPI_BUS_MAX_NUM >= 1
        case SPI_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_0_TX;
#endif
#if SPI_BUS_MAX_NUM >= 2
        case SPI_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_1_TX;
#endif
#if SPI_BUS_MAX_NUM >= 3
        case SPI_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_2_TX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t spi_port_get_dma_trans_src_handshaking(spi_bus_t bus)
{
    switch (bus) {
#if SPI_BUS_MAX_NUM >= 1
        case SPI_BUS_0:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_0_RX;
#endif
#if SPI_BUS_MAX_NUM >= 2
        case SPI_BUS_1:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_1_RX;
#endif
#if SPI_BUS_MAX_NUM >= 3
        case SPI_BUS_2:
            return (uint8_t)DMA_HANDSHAKE_SPI_BUS_2_RX;
#endif
        default:
            return (uint8_t)HAL_DMA_HANDSHAKING_MAX_NUM;
    }
}

uint8_t spi_port_tx_data_level_get(spi_bus_t bus)
{
    switch (bus) {
        case SPI_BUS_0:
        case SPI_BUS_1:
            return (uint8_t)SPI_DMA_TX_DATA_LEVEL_4;
        default:
            return 0;
    }
}

uint8_t spi_port_rx_data_level_get(spi_bus_t bus)
{
    unused(bus);
    return SPI_DMA_RX_DATA_LEVEL_4;
}
#endif

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
void spi_port_register_irq(spi_bus_t bus)
{
    osal_irq_request(g_spi_interrupt_lines[bus].irq_num, g_spi_interrupt_lines[bus].irq_func, NULL, NULL, NULL);
    osal_irq_set_priority(g_spi_interrupt_lines[bus].irq_num, irq_prio(g_spi_interrupt_lines[bus].irq_num));
    osal_irq_enable(g_spi_interrupt_lines[bus].irq_num);
}

void spi_port_unregister_irq(spi_bus_t bus)
{
    osal_irq_disable(g_spi_interrupt_lines[bus].irq_num);
}
#endif

void spi_funcreg_adapt(spi_bus_t bus)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ;
    }
    (void)hal_spi_regs_init();
    return;
}

uint32_t spi_porting_lock(spi_bus_t bus)
{
    unused(bus);
#ifdef CONFIG_SPI_SUPPORT_DSP
    return audio_osal_irq_lock();
#else
    return osal_irq_lock();
#endif
}

void spi_porting_unlock(spi_bus_t bus, uint32_t irq_sts)
{
    unused(bus);
#ifdef CONFIG_SPI_SUPPORT_DSP
    audio_osal_irq_restore(irq_sts);
#else
    osal_irq_restore(irq_sts);
#endif
}

void spi_port_dynamic_clock_enable(spi_bus_t bus, bool on)
{
#ifdef CONFIG_SPI_SUPPORT_DSP
    return;
#else
    switch (bus) {
        case SPI_BUS_0:
            uapi_clocks_switch(CLOCKS_ID_M_SPI0, (switch_type_t)on);
            break;
        case SPI_BUS_1:
            uapi_clocks_switch(CLOCKS_ID_M_SPI1, (switch_type_t)on);
            break;
        default:
            break;
    }
#endif
}
