/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:  Application Core Platform Definitions
 *
 * Date:
 */

#ifndef PLATFORM_CORE_H
#define PLATFORM_CORE_H

#include "product.h"
#include "product_pin.h"
#include "soc_reg.h"
#include "chip_core_definition.h"

/**
 * @defgroup DRIVER_PLATFORM_CORE CHIP Platform CORE Driver
 * @ingroup DRIVER_PLATFORM
 * @{
 */
#define FUSE_CTL_RB_ADDR        0x57027000
#define FLASH_START_ADDR        0x26000000
#define ULP_AON_CTL_RB_ADDR     0x5702c000 // no used
#define UART0_BASE              0x59010000 /* 共存/LOG UART */
#define UART1_BASE              0x55004000 /* COM UART */
#define DMA_BASE_ADDR           0x5A000000 /* B_DMA */

// UART_BUS_X和DMA握手号映射关系
#define DMA_HANDSHAKE_UART_BUS_0_TX      HAL_DMA_HANDSHAKING_UART_L0_TX
#define DMA_HANDSHAKE_UART_BUS_0_RX      HAL_DMA_HANDSHAKING_UART_L0_RX
#define DMA_HANDSHAKE_UART_BUS_1_TX      HAL_DMA_HANDSHAKING_UART_COM_TX
#define DMA_HANDSHAKE_UART_BUS_1_RX      HAL_DMA_HANDSHAKING_UART_COM_RX

#ifdef PRE_EDA
#define EDA_PRINT_ADDR          0x59001F64
#endif

#define SYSTICK_BASE_ADDR       0x59000A60
#define TCXO_COUNT_BASE_ADDR    0x59000320

#define RTC_0_BASE_ADDR         (GLB_RTC_BASE_ADDR + 0x200)

#define WDT_CLK_EN              B_CTL_RB_BTOP0_L_REG
#define WDT_CLK_EN_BIT          BWDT_RST_BIT_OFFSET
#define WDT_NMI_EN_REG          B_CTL_RB_BT_NMI_MASK_REG
#define WDT_NMI_EN_BIT          B_CTL_RB_BWDT_INT_NMI_MASK_OFFSET
#define WDT_DISABLE_RESET_REG   B_CTL_RB_BWDT_DIS_REG
#define WDT_BASE_ADDR           0x59007000
#define CHIP_WDT_BASE_ADDRESS   WDT_BASE_ADDR
#define NMI_CTL_REG_BASE_ADDR   B_CTL_RB_BT_NMI_MASK_REG // bt core nmi int addr

#define HAL_SOFT_RST_CTL_BASE                   (GLB_CTL_M_RB_BASE)
#define HAL_GLB_CTL_M_ATOP1_L_REG_OFFSET        0x168
#define HAL_CHIP_WDT_ATOP1_RST_BIT              31

// GPIO regs
#define GPIO0_BASE_ADDR         0x57013000

#define HAL_BCPU_TRACE_ENABLE                       0x59008500
#define HAL_CPU_TRACE_CTL_RB_BASE_ADDR              0x5900C000
#define HAL_CPU_TRACE_DIAG_EN_OFFSET                0x104
#define HAL_CPU_TRACE_DIAG_CLOCK_CFG_OFFSET         0x108
#define HAL_CPU_TRACE_DIAG_CLOCK_SELECT_OFFSET      0x10C
#define HAL_CPU_TRACE_DIAG_SAMPLE_MODE_CFG_OFFSET   0x200

#define TRACE_MEM_REGION_START                      BCPU_TRACE_MEM_REGION_START
#define TRACE_MEM_REGION_LENGTH                     CPU_TRACE_MEM_REGION_LENGTH
#define TRACE_CPU_INDEX                             0 // CPU_TRACE_TRACED_BCPU

// GPIO select core
#define HAL_GPIO_D_CORE_SET_REG                     0x570001B0
#define HAL_GPIO_NON_D_CORE_SET_REG                 0x57000180
#define HAL_GPIO_CORE_SET_CHANNEL_OFFSET            0x08
#define HAL_GPIO_CORE_SET_REG_OFFSET                2
#define HAL_GPIO_CORE_SET_GPIOS                     16

// Timer reg base addr
#define TIMER_BASE_ADDR                             0x59006000
#define TIMER_0_BASE_ADDR                           (TIMER_BASE_ADDR + 0x100)
#define TIMER_1_BASE_ADDR                           (TIMER_BASE_ADDR + 0x200)
#define TIMER_2_BASE_ADDR                           (TIMER_BASE_ADDR + 0x300)
#define TIMER_3_BASE_ADDR                           (TIMER_BASE_ADDR + 0x400)
#define TIMER_SYSCTL_BASE_ADDR                      (TIMER_BASE_ADDR + 0x500)

#define TICK_TIMER_BASE_ADDR                        TIMER_0_BASE_ADDR

/**
 * @brief  Maximum UART buses
 * Defined here rather than in the UART_BUS enum, due to needing to use it for conditional compilation
 */
#define UART_BUS_MAX_NUMBER                         1  // !< Max number of UARTS available
#define PM_RESUME_UART_BUS_NUM                      1  // only resume coex-uart

#define GPIO_MAX_NUMBER                             5  // !< Max number of GPIO available
#define B_DMA_CHANNEL_MAX_NUM                       4  // !< Max number of B_DMA available
#define DMA_CHANNEL_MAX_NUM                         (B_DMA_CHANNEL_MAX_NUM)

#define CHIP_FIXED_TX_PIN                           S_AGPIO11
#define CHIP_FIXED_RX_PIN                           S_AGPIO12
#define SW_DEBUG_UART_BUS                           UART_BUS_0
#define CHIP_FIXED_UART_BUS                         UART_BUS_0

typedef enum {
    UART_BUS_0 = 0,                                 // !< 共存/LOG UART
    UART_BUS_NONE = UART_BUS_MAX_NUMBER             // !< Value used as invalid/unused UART number
} uart_bus_t;

#define TEST_SUITE_UART_BUS     UART_BUS_0

#define PIN_MAX_NUMBER                              PIN_NONE // value USED to iterate in arrays

#define TIMER_MAX_AVAILABLE_NUMBER                  3  // !< Max number of timer available
#define RTC_MAX_AVAILABLE_NUMBER                    1  // !< Max number of rtc available

#define ENABLE_CPU_TRACE                            3
#define BCPU_TRACE_ENABLE_CFG                       YES

#define IS_MAIN_CORE                                NO
#define FIXED_IN_ROM                                NO

#define CRITICAL_INT_RESTORE                        YES

/**
 * @}
 */
#endif
