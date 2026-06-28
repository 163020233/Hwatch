/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 * Description: pm core adapter code
 */

#include "pm_cpu_porting.h"
#include "platform.h"
#include "chip_io.h"
#include "systick.h"
#include "arch_encoding.h"
#include "tcxo_porting.h"
#include "pm_sleep_porting.h"
#include "arch_barrier.h"
#include "arch/cache.h"
#include "pmu_cmu.h"
#include "clocks_config.h"
#include "chip_core_irq.h"
#include "clocks_ccrg.h"
#include "exception.h"
#include "calendar_porting.h"
#include "tcxo.h"

#define CORE_SUSPEND_REGS_CNT      8
#define CLIC_SUSPEND_REGS_CNT      (BUTT_IRQN - LOCAL_INTERRUPT0)

#define PMU_ULP_DELAY_US       60UL

typedef union {
    struct {
        uint8_t int_ip;
        uint8_t int_ie;
        uint8_t int_attr;
        uint8_t int_ctl;
    } regs;
    uint32_t clic;
} clic_group_regs_t;

static uint32_t g_clic_cfg_reg = 0;
static clic_group_regs_t g_clic_group_regs[CLIC_SUSPEND_REGS_CNT] = {0};

uint32_t g_cpu_suspend_regs[CORE_SUSPEND_REGS_CNT] = {0};

static uint16_t g_pm_m_clken0_val = 0;
static uint16_t g_pm_m_clken1_val = 0;
static uint16_t g_pm_uart_trx_val = 0;
static uint16_t g_pm_mcu_bus_div_val = 0;

static uint16_t g_pm_com_uart_val = 0;

static void uapi_cpu_clic_suspend(void)
{
    uint32_t clic_base_addr = CLIC_BASE_REG;
    reg8_read(clic_base_addr + SYS_CLIC_CFG, g_clic_cfg_reg);
    for (uint8_t index = 0; index < CLIC_SUSPEND_REGS_CNT; index++) {
        reg32_read(clic_base_addr + sys_clic_intip(index + LOCAL_INTERRUPT0), g_clic_group_regs[index].clic);

        /* Close all irq */
        if ((index + LOCAL_INTERRUPT0) == MCU_TE_IRQN) {
            // do not clear mcu int irq pending (edge-triggered)
            reg8_write(clic_base_addr + sys_clic_intie(index + LOCAL_INTERRUPT0), 0x0);
        } else {
            reg16_write(clic_base_addr + sys_clic_intip(index + LOCAL_INTERRUPT0), 0x0);
        }
    }
    reg8_write(clic_base_addr + sys_clic_intie(M_WAKEUP_IRQN), 0x1);
}

static void uapi_cpu_clic_resume(void)
{
    uint32_t clic_base_addr = CLIC_BASE_REG;
    reg8_write(clic_base_addr + SYS_CLIC_CFG, g_clic_cfg_reg);
    for (uint8_t index = 0; index < CLIC_SUSPEND_REGS_CNT; index++) {
        if (((index + LOCAL_INTERRUPT0) != MCU_TE_IRQN) &&
            (reg8_getbit(clic_base_addr + sys_clic_intip(index + LOCAL_INTERRUPT0), 0x0) == 0x0)) {
            reg32_write(clic_base_addr + sys_clic_intip(index + LOCAL_INTERRUPT0), g_clic_group_regs[index].clic);
        } else {
            // do'not clear int_pend
            reg8_write(clic_base_addr + sys_clic_intie(index + LOCAL_INTERRUPT0), g_clic_group_regs[index].regs.int_ie);
            reg8_write(clic_base_addr + sys_clic_intattr(index + LOCAL_INTERRUPT0),
                g_clic_group_regs[index].regs.int_attr);
            reg8_write(clic_base_addr + sys_clic_intctl(index + LOCAL_INTERRUPT0),
                g_clic_group_regs[index].regs.int_ctl);
        }
    }
}

static void pm_port_perp_clk_suspend(void)
{
    // m_ctl
    g_pm_m_clken0_val = readw(M_CTL_RB_M_CLKEN0_REG);
    g_pm_m_clken1_val = readw(M_CTL_RB_M_CLKEN1_REG);
    g_pm_mcu_bus_div_val = readw(M_CTL_RB_MCU_BUS_REG) & M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_MASK;
    g_pm_uart_trx_val = readw(M_CTL_RB_UART_TRX_CFG_REG);

    // com_ctl
    g_pm_com_uart_val = readw(COM_CTL_RB_COM_UART_REG);

    // sw to 32M
    clocks_suspend();
}

void pm_port_cpu_suspend(void)
{
    uapi_cpu_clic_suspend();

    pm_port_perp_clk_suspend();

    lowpower_cpu_suspend();
}

static void pm_port_perp_clk_resume(void)
{
    clocks_resume();
#ifdef CFG_DRIVERS_QSPI
    uapi_clocks_switch(CLOCKS_ID_M_QSPI_DISPLAY, TURN_ON);
#endif
    if (g_pm_mcu_bus_div_val > 1) {
        reg16_clrbit(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_LOAD_DIV_CLKEN_OFFSET);
        reg16_setbits(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_OFFSET,
            M_CTL_RB_CFG_M_MCU_BUS_DIV_NUM_LEN, g_pm_mcu_bus_div_val);
        reg16_setbit(M_CTL_RB_MCU_BUS_REG, M_CTL_RB_CFG_M_MCU_BUS_LOAD_DIV_CLKEN_OFFSET);
    }

    writew(M_CTL_RB_M_CLKEN0_REG, g_pm_m_clken0_val);
    writew(M_CTL_RB_M_CLKEN1_REG, g_pm_m_clken1_val);
    writew(M_CTL_RB_UART_TRX_CFG_REG, g_pm_uart_trx_val);
    writew(COM_CTL_RB_COM_UART_REG, g_pm_com_uart_val);

    writew(M_CTL_RB_MCU_UART_LO_CRG_CFG0_REG, 0x1);
    writew(M_CTL_RB_MCU_UART_LO_CRG_CFG0_REG, 0x19); // cfg div_load_en is 1

    // clr slp event and int fisrt to reduce waiting time, which takes 2* 32k cycle to take effect
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_SLEEPING_REG,
           LP_CTL_MCU_RB_MCPU_SLEEPING_CLR_MASK | LP_CTL_MCU_RB_MCPU_SLEEPING_INT_CLR_MASK);
}

void pm_port_cpu_resume(void)
{
    pm_port_set_start_time();

    lowpower_cpu_resume();

    pm_port_perp_clk_resume();

    uapi_cpu_clic_resume();

    pm_auto_cg_config(true);

    (void)ArchICacheEnable(CACHE_64KB);
    (void)ArchICachePrefetchEnable(CACHE_PREF_1_LINES);

    (void)ArchDCacheEnable(CACHE_64KB);
    (void)ArchDCachePrefetchEnable(CACHE_PREF_1_LINES, CACHE_PREF_1_LINES);
}

void pm_port_enter_shipmode(uint32_t args)
{
    unused(args);
    if (calendar_sync_before_shipmode() != 0) {
        return;
    }
    reset_deal_with_fs();
    // clear pwr off flag
    reg16_write(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_12_REG, 0);
 
    // clear pwr on state
    if (reg16_getbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG, LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_INV_STS_STICK_OFFSET)) {
        reg16_setbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_STS_INV_STICK_CLR_OFFSET);
        uapi_tcxo_delay_us(PMU_ULP_DELAY_US);
        reg16_clrbit(LP_CTL_ULP_B_RB_PWR_ON_VBUS_STATE_REG,
            LP_CTL_ULP_B_RB_PMU_ULP_RST_REQ_GRM_STS_INV_STICK_CLR_OFFSET);
    }
 
    // select xo32k
    if (reg16_getbit(LP_CTL_ULP_A_RB_RC_XO_CLK_MUX_REG, LP_CTL_ULP_A_RB_RC_XO_SEL_OFFSET) == 1) {
        reg16_setbit(LP_CTL_ULP_A_RB_LPREF_CTRL_MAN1_REG, LP_CTL_ULP_A_RB_LPREF_BG_EN_SAV_VAL_OFFSET);
        reg16_setbit(LP_CTL_ULP_A_RB_LPREF_CTRL_MAN1_REG, LP_CTL_ULP_A_RB_LPREF_BUF_EN_SAV_VAL_OFFSET);
        reg16_setbit(LP_CTL_ULP_A_RB_LPREF_CTRL_MAN1_REG, LP_CTL_ULP_A_RB_LPREF_IBG_EN_SAV_VAL_OFFSET);
 
        writew(LP_CTL_ULP_A_RB_LP_REF_BG_BOOT_TIME_REG, 0);
        writew(LP_CTL_ULP_A_RB_LP_REF_BUF_BOOT_TIME_REG, 0);
        writew(LP_CTL_ULP_A_RB_LP_REF_IBG_BOOT_TIME_REG, 0);
 
        uapi_tcxo_delay_ms(1);
    }
 
    writew(LP_CTL_ULP_B_RB_POWER_UP_DOWN_SHIP_MODE_SEL_REG, POWER_HOLD_ENTER_SHIP_MODE);
    writew(GLB_CTL_M_RB_HI6535_PWR_HOLD_REG, 0);
    while (1) {
    }
}