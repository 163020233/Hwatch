/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog port template \n
 *
 * History: \n
 * 2022-06-06， Create file. \n
 */
#include "watchdog_porting.h"
#include "hal_watchdog_v151.h"
#include "watchdog.h"
#include "tcxo.h"
#include "platform_core.h"
#if CORE == APPS
#include "cpu_utils.h"
#endif
#include "debug_print.h"
#include "non_os.h"
#include "chip_io.h"
#include "osal_interrupt.h"
#include "chip_core_definition.h"

#define PARAM       0
#define WAIT_APPS_DUMP_DSP_DELAY_MS  15000
#define WDT_PORTING_ENABLE_DELAY     60uLL
#define DEFAULT_WDT_TIME             8
#define WDT_EOI                      0xC
#define BWDT_RST_BIT_OFFSET          2

uintptr_t g_watchdog_base_addr = (uintptr_t)CHIP_WDT_BASE_ADDRESS;
hal_chip_watchdog_dump_callback_t g_hal_dsp_chip_watchdog_callback = NULL;
static uint32_t g_watchdog_config = DEFAULT_WDT_TIME ;
static uint32_t g_watchdog_clock = CONFIG_WDT_CLOCK;
#if CORE == APPS
static uint32_t g_wdt_timeout_pc_val = 0;
static uint32_t g_wdt_timeout_lr_val = 0;
#endif

void watchdog_port_register_hal_funcs(void)
{
    hal_watchdog_register_funcs(hal_watchdog_v151_funcs_get());
}

void watchdog_port_unregister_hal_funcs(void)
{
    hal_watchdog_unregister_funcs();
}

void watchdog_port_register_irq(void)
{
    return;
}

void irq_wdt_handler(void)
{
}

/* ------------------------------------------------------------------------------*/
/* The following is the product adaptation section. */
/* ------------------------------------------------------------------------------*/

#if CORE != APPS
void watchdog_disable_reset(void)
{
    writew(WDT_DISABLE_RESET_REG, 0x1); // disable watchdog reset subsystem
}
#endif

#if CORE == APPS
#define DRV_WATCHDOG_BASE_ADDR 0x5800b000
__IRQ void irq_wdt_dsp_handler(void)
{
    PRINT("mcu get dsp watchdog interrupt\r\n");
     /* 解锁 */
    writel(DRV_WATCHDOG_BASE_ADDR + 0x0, 0x5A5A5A5A);
    /* 计数停止 */
    writel(DRV_WATCHDOG_BASE_ADDR + 0x010, 0x0);
    /* wdt_eoi */
    writel(DRV_WATCHDOG_BASE_ADDR + 0x00C, 0x1);
    /* 等待wdt空闲 */
    while (readl(DRV_WATCHDOG_BASE_ADDR + 0x018) != 0) {
    }
    /* 上锁 */
    writel(DRV_WATCHDOG_BASE_ADDR + 0x0, 0x0);

    PRINT("clean dsp wdt done\r\n");
#ifdef IPC_NEW
    req_reboot_msg msg = {0};
    msg.core = CORES_PROTOCOL_CORE;
    msg.req_reboot_reason = REBOOT_CAUSE_PROTOCOL_WATCHDOG;

    cpu_utils_exception_ind((uint8_t *)&msg, sizeof(msg));
#endif
}

void watchdog_dsp_inform_mcpu_enable(void)
{
    osal_irq_request(D2MCU_DSP_WDG0_IRQN, (osal_irq_handler)irq_wdt_dsp_handler, NULL, NULL, NULL);
    (void)osal_irq_set_priority(D2MCU_DSP_WDG0_IRQN, irq_prio(D2MCU_DSP_WDG0_IRQN));
    osal_irq_enable(D2MCU_DSP_WDG0_IRQN);
}


__IRQ void irq_wdt_bt_handler(void)
{
    writew(BT_WDT_BASE_ADDR + WDT_EOI, 0x1);
    PRINT("mcu get bt watchdog interrupt\r\n");
}

void watchdog_bt_inform_mcpu_enable(void)
{
    osal_irq_request(B2MCU_WDT_IRQN, (osal_irq_handler)irq_wdt_bt_handler, NULL, NULL, NULL);
    (void)osal_irq_set_priority(B2MCU_WDT_IRQN, irq_prio(B2MCU_WDT_IRQN));
    osal_irq_enable(B2MCU_WDT_IRQN);
}

void watchdog_timeout_save_info_init(void)
{
    if (reg16_getbits(GLB_CTL_M_RB_MCPU_PC_LR_LOAD_REG, GLB_CTL_M_RB_GCPU_LOAD_OK_OFFSET,
                      GLB_CTL_M_RB_GCPU_LOAD_OK_LEN) == 0x1) { // 0x1 means watchdog timeout load ok.
        g_wdt_timeout_pc_val = (readw(GLB_CTL_M_RB_MCPU_PC_L_REG) & 0xFFFF) |
                               ((readw(GLB_CTL_M_RB_MCPU_PC_H_REG) << 16) & 0xFFFF0000); // high offset 16bits
        g_wdt_timeout_lr_val = (readw(GLB_CTL_M_RB_MCPU_LR_L_REG) & 0xFFFF) |
                               ((readw(GLB_CTL_M_RB_MCPU_LR_H_REG) << 16) & 0xFFFF0000); // high offset 16bits
        reg16_setbit(GLB_CTL_M_RB_MCPU_PC_LR_LOAD_REG, GLB_CTL_M_RB_GCPU_LOAD_CLR_OFFSET);
        uapi_tcxo_delay_ms(1);
        reg16_clrbit(GLB_CTL_M_RB_MCPU_PC_LR_LOAD_REG, GLB_CTL_M_RB_GCPU_LOAD_CLR_OFFSET);
        PRINT("mcu get watchdog timeout pc=0x%X, lr=0x%X\r\n", g_wdt_timeout_pc_val, g_wdt_timeout_lr_val);
    }
    reg16_setbit(GLB_CTL_M_RB_MCPU_PC_LR_ENABLE_REG, 0x2); // 0x2 means enable watchdog timeout lock pclr.
}

void watchdog_get_before_timeout_infos(uint32_t *pc, uint32_t *lr)
{
    *pc = g_wdt_timeout_pc_val;
    *lr = g_wdt_timeout_pc_val;
}
#endif

void watchdog_func_adapt(uint32_t time)
{
    reg_setbit(WDT_NMI_EN_REG, 0, WDT_NMI_EN_BIT);
#if CORE == APPS
    watchdog_dsp_inform_mcpu_enable();
#else
    watchdog_disable_reset();
#endif
    watchdog_set_configtime(time);
}

void watchdog_turnon_clk(void)
{
    reg_setbit(WDT_CLK_EN, 0, WDT_CLK_EN_BIT);
    uapi_tcxo_delay_us(WDT_PORTING_ENABLE_DELAY);
}

void watchdog_turnoff_clk(void)
{
    reg_clrbit(WDT_CLK_EN, 0, WDT_CLK_EN_BIT);
}

void watchdog_set_configtime(uint32_t time)
{
    g_watchdog_config = time;
}

uint32_t watchdog_get_configtime(void)
{
    return g_watchdog_config;
}

void hal_dsp_chip_wdt_register_dump_callback(hal_chip_watchdog_dump_callback_t callback)
{
    if (callback != NULL) {
        g_hal_dsp_chip_watchdog_callback = callback;
    }
}

void watchdog_port_set_clock(uint32_t wdt_clock)
{
    uint32_t irq_sts = osal_irq_lock();
    g_watchdog_clock = wdt_clock;
    osal_irq_restore(irq_sts);
}

uint32_t watchdog_port_get_clock(void)
{
    return g_watchdog_clock;
}
