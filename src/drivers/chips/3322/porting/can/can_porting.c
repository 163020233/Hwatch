/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides CAN-FD port template \n
 *
 * History: \n
 * 2023-02-24, Create file. \n
 */
#include "soc_reg.h"
#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "chip_core_irq.h"
#include "arch_port.h"
#if defined(CONFIG_PM_CLOCK_SUPPORT_SFC_HIGH_FREQ)
#include "pm_clock_porting.h"
#endif
#include "pinctrl.h"
#include "tcxo.h"
#include "hal_can_fd_v330.h"
#include "pmu_sub_control.h"
#include "pm_clocks.h"
#include "clocks_switch.h"
#include "can_porting.h"

#define CAN_FD_REG_BASE_ADDR        0x52066000
#define CAN_FD_SRAM_BASE_ADDR       0x52067000
/* All SRAM address will be 4-byte aligned automaticly during configuration. */
#define CAN_FD_STD_FILTER_BASE_ADDR 0x52067000   // MAX CONFIG_STANDARD_CAN_FILTER_NUM is 24
#define CAN_FD_EX_FILTER_BASE_ADDR  0x52067060   // MAX CONFIG_EXTENDED_CAN_FILTER_NUM is 24
#define CAN_FD_RX_FIFO_0_BASE_ADDR  0x52067120   // MAX CONFIG_CAN_RX_FIFO0_SIZE is 20
#define CAN_FD_RX_FIFO_1_BASE_ADDR  0x520676C0   // MAX CONFIG_CAN_RX_FIFO1_SIZE is 20
#define CAN_FD_TX_FIFO_BASE_ADDR    0x52067C60   // MAX CONFIG_CAN_TX_BUFFER_SIZE is 11
#define CAN_FD_TX_EVENT_BASE_ADDR   0x52067F78   // MAX CONFIG_CAN_TX_BUFFER_SIZE is 11
#define CAN_CCLK_20M                20000
#define CAN_CCLK_40M                40000

#define CAN_TIMEOUT_DURATION_MS 50

#ifndef BUILD_NOOSAL
static osal_mutex g_can_mutex = { NULL };
static bool g_can_mutex_inited = false;
#endif

static const uint32_t g_can_hclk_freq = CAN_CCLK_20M;

uintptr_t can_porting_get_reg_base_addr(void)
{
    return (uintptr_t)CAN_FD_REG_BASE_ADDR;
}

uintptr_t can_porting_get_sram_base_addr(void)
{
    return (uintptr_t)CAN_FD_SRAM_BASE_ADDR;
}

uintptr_t can_porting_get_std_filter_base_addr(void)
{
    return (uintptr_t)CAN_FD_STD_FILTER_BASE_ADDR;
}

uintptr_t can_porting_get_ex_filter_base_addr(void)
{
    return (uintptr_t)CAN_FD_EX_FILTER_BASE_ADDR;
}

uintptr_t can_porting_get_rx_fifo_0_base_addr(void)
{
    return (uintptr_t)CAN_FD_RX_FIFO_0_BASE_ADDR;
}

uintptr_t can_porting_get_rx_fifo_1_base_addr(void)
{
    return (uintptr_t)CAN_FD_RX_FIFO_1_BASE_ADDR;
}

uintptr_t can_porting_get_tx_fifo_base_addr(void)
{
    return (uintptr_t)CAN_FD_TX_FIFO_BASE_ADDR;
}

uintptr_t can_porting_get_tx_event_base_addr(void)
{
    return (uintptr_t)CAN_FD_TX_EVENT_BASE_ADDR;
}

static int can_line0_handler(void)
{
    hal_can_fd_v330_irq_handler(0);
    osal_irq_clear(M_TTCAN_INT0);
    return 0;
}

static int can_line1_handler(void)
{
    hal_can_fd_v330_irq_handler(1);
    osal_irq_clear(M_TTCAN_INT1);
    return 0;
}

void can_porting_regitser_irq(void)
{
    osal_irq_request(M_TTCAN_INT0, (osal_irq_handler)can_line0_handler, NULL, NULL, NULL);
    osal_irq_request(M_TTCAN_INT1, (osal_irq_handler)can_line1_handler, NULL, NULL, NULL);
    osal_irq_set_priority(M_TTCAN_INT0, irq_prio(M_TTCAN_INT0));
    osal_irq_set_priority(M_TTCAN_INT1, irq_prio(M_TTCAN_INT1));
    osal_irq_enable(M_TTCAN_INT0);
    osal_irq_enable(M_TTCAN_INT1);
}

void can_porting_unregitser_irq(void)
{
    osal_irq_disable(M_TTCAN_INT1);
    osal_irq_disable(M_TTCAN_INT0);
    osal_irq_free(M_TTCAN_INT1, NULL);
    osal_irq_free(M_TTCAN_INT0, NULL);
    osal_irq_clear(M_TTCAN_INT1);
    osal_irq_clear(M_TTCAN_INT0);
}

static void can_io_reuse(void)
{
    writew(CFG_S_AGPIO_MODE_CFG_S_AGPIO33_MODE_REG, 0x1); // can tx
    writew(CFG_S_AGPIO_MODE_CFG_S_AGPIO34_MODE_REG, 0x1); // can rx
}

void can_porting_pwr_on(void)
{
    uapi_clocks_config_mode(PM_ID_CAN, PM_MODE_PERFORMACE);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_CAN, CLOCKS_SRC_PLL, 40); // 800 / 40 = 20MHz
    uapi_pmu_hs_sub_control(HS_SUB_CAN_ID, PMU_POWER_ON_AND_RUN);
    /* disable CAN */
    reg16_clrbit(HS_CTL_RB_HS_SUB_CAN_CFG_REG, HS_CTL_RB_CFG_HS_SOFT_RST_CAN_N_OFFSET);
    /* enable CAN */
    reg16_setbit(HS_CTL_RB_HS_SUB_CAN_CFG_REG, HS_CTL_RB_CFG_HS_SOFT_RST_CAN_N_OFFSET);
    can_io_reuse();
}

void can_porting_pwr_off(void)
{
    uapi_pmu_hs_sub_control(HS_SUB_CAN_ID, PMU_POWER_OFF);
    clocks_ccrg_set_frequency(HAL_CLOCKS_MODULE_CAN, CLOCKS_SRC_MAX, 1); // 800 / 40 = 20MHz
    uapi_clocks_config_mode(PM_ID_CAN, PM_MODE_LP);
}

uint32_t can_porting_get_seg_ticks(uint32_t baud_rate)
{
    return g_can_hclk_freq / baud_rate;
}

uint64_t can_porting_get_cur_time(void)
{
    return uapi_tcxo_get_ms();
}

uint32_t can_porting_get_timeout_duration(void)
{
    return CAN_TIMEOUT_DURATION_MS;
}

void can_port_lock_init(void)
{
#ifndef BUILD_NOOSAL
    if (!g_can_mutex_inited) {
        osal_mutex_init(&g_can_mutex);
        g_can_mutex_inited = true;
    }
#endif
}

uint32_t can_port_get_lock(void)
{
#ifndef BUILD_NOOSAL
    if (g_can_mutex_inited && !osal_in_interrupt()) {
        return osal_mutex_lock_timeout(&g_can_mutex, OSAL_MUTEX_WAIT_FOREVER);
    }
#endif
    return ERRCODE_SUCC;
}

void can_port_restore_lock(uint32_t lock_sts)
{
    unused(lock_sts);
#ifndef BUILD_NOOSAL
    if (g_can_mutex_inited && !osal_in_interrupt()) {
        osal_mutex_unlock(&g_can_mutex);
    }
#endif
}