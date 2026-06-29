/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Application core chip init function for standard
 * Author:
 * Create:
 */
#include "chip_init.h"
#include "chip_io.h"
#include "tcxo.h"
#include "systick.h"
#include "timer.h"
#include "rtc.h"
#include "gpio.h"
#include "ipc.h"
#include "uart_porting.h"
#include "log_memory_region.h"
#include "dma.h"
#include "pm_clocks.h"
#include "pm_pmu.h"
#include "pmu_ulp.h"
#include "pinctrl.h"
#include "arch_port.h"
#include "debug_print.h"
#include "efuse.h"
#include "pm_pmu.h"
#include "clocks_switch.h"
#include "pmu_sub_control.h"
#include "ipc_ecdh_clac.h"
#include "secure_verify_boot.h"
#include "los_config.h"
#include "sec_trng_master.h"
#include "watchdog_porting.h"
#include "calendar_porting.h"
#include "pm_cpu_porting.h"
#include "osal_addr.h"
#include "hal_reboot.h"
#include "cpu_trace_porting.h"
#include "security_init.h"
#include "secure_verify_boot_porting.h"
#include "pm_porting.h"
#if defined(SAVE_EXC_INFO)
#include "exception.h"
#endif

#define KEY_PRESS_TIME_MS   3000

static void psram_init_handle(void)
{
    errcode_t retcode = ERRCODE_FAIL;
    for (uint8_t pool_id = MEM_POOL_BSP; pool_id < MEM_POOL_MAX; pool_id++) {
        if (uapi_sys_get_memory_pool_need_init(pool_id)) {
            retcode = osal_pool_mem_init((void *)(uintptr_t)uapi_sys_get_memory_pool_addr(pool_id),
                uapi_sys_get_memory_pool_length(pool_id));
            if (retcode != 0) {
                PRINT("memory init pool %x error\r\n", pool_id);
            }
        }
    }
}

#ifdef CONFIG_PRODUCT_EVB_DITING
static void cat1_power_on(void)
{
    /* cat1_vbat_en */
    uapi_pin_set_mode(S_AGPIO1, (pin_mode_t)HAL_PIO_FUNC_GPIO);
    uapi_gpio_set_dir(S_AGPIO1, GPIO_DIRECTION_OUTPUT);
    uapi_gpio_set_val(S_AGPIO1, GPIO_LEVEL_HIGH);
    PRINT("cat1_power_on succ." NEWLINE);
}
#endif

static void chip_power_on_config(void)
{
    if ((uapi_pmu_ulp_get_reboot_reason() != PMU_ULP_PWRKEY_POWER_UP) ||
        (hal_reboot_get_ulp_aon_no_poweroff_flag() == true)) {
        return;
    }
    uint64_t timeout = 0;
    uint64_t start_ms = uapi_tcxo_get_ms();
    while (uapi_pmu_ulp_get_pwrkey_press_state() == true) {
        timeout = uapi_tcxo_get_ms() - start_ms;
        if (timeout >= KEY_PRESS_TIME_MS) {
            *calendar_get_first_poweron_flag() = 0;
            break;
        }
    }

    if (timeout < KEY_PRESS_TIME_MS) {
        *calendar_get_first_poweron_flag() = 1;
        pm_port_enter_shipmode(0);
    }
}

static void close_bus_clock(void)
{
    reg16_clrbit(GLB_CTL_M_RB_PWM_CTRL_REG, GLB_CTL_M_RB_CFG_AON_PWM_CLKEN_OFFSET); // aon pwm
    reg16_clrbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_PWM_CLKEN_OFFSET);                 // m pwm
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_I2C0_CLKEN_OFFSET);                // i2c0
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_I2C1_CLKEN_OFFSET);                // i2c1
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_I2C2_CLKEN_OFFSET);                // i2c2
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_I2C3_CLKEN_OFFSET);                // i2c3
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_SPI0_MS_CLKEN_OFFSET);             // spi0
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_SPI1_M_CLKEN_OFFSET);              // spi1
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_UART_H0_CLKEN_OFFSET);             // uart0
    reg16_clrbit(M_CTL_RB_M_CLKEN1_REG, M_CTL_RB_UART_H1_CLKEN_OFFSET);             // uart1
    return;
}

static void open_chipwdt_rec(void)
{
    if (reg16_getbit(GLB_CTL_M_RB_SYS_ABNORMAL_STATUS0_REG, GLB_CTL_M_RB_GLB_WDT_RST_OFF_OFFSET)) {
        PRINT("cwdt pc,l:%04x,h:%04x.\r\n", readw(GLB_CTL_M_RB_MCPU_PC_L_REG), readw(GLB_CTL_M_RB_MCPU_PC_H_REG));
        PRINT("lr,l:%04x,h:%04x.\r\n", readw(GLB_CTL_M_RB_MCPU_LR_L_REG), readw(GLB_CTL_M_RB_MCPU_LR_H_REG));
        PRINT("sp,l:%04x,h:%04x.\r\n", readw(GLB_CTL_M_RB_MCPU_SP_L_REG), readw(GLB_CTL_M_RB_MCPU_SP_H_REG));
    }
 
    reg16_setbit(M_CTL_RB_M_CLKEN0_REG, M_CTL_RB_CFG_M_TRIGGER_CLKEN_OFFSET);
    // wdt trigger
    writew(GLB_CTL_M_RB_MCPU_PC_LR_ENABLE_REG, 0x4);
    return;
}

void chip_init(void)
{
    uapi_pin_init();
    uapi_gpio_init();
    // slp oe fllow work oe
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_SLP_VAL_1_REG, LP_CTL_GLB_RB_CFG_PAD_OE_WRK_SLP_SEL_SLP_VAL_OFFSET);
    close_bus_clock();
    uapi_efuse_init();

#ifndef CFG_DRIVERS_USB_AS_SERIAL
    /* uart init */
    log_uart_port_init();
    PRINT("App uart init succ." NEWLINE);
#endif
    watchdog_func_adapt(CHIP_WDT_TIMEOUT_32S);
#if defined(SAVE_EXC_INFO)
    hard_watchdog_reset_copy_comram();
#endif
#ifdef CONFIG_PRODUCT_EVB_DITING
    cat1_power_on();
#endif
    uapi_tcxo_init();
    uapi_systick_init();
    uapi_rtc_init();
    uapi_rtc_adapter(RTC_0, RTC_0_IRQN, irq_prio(RTC_0_IRQN));

    uapi_timer_init();
    uapi_timer_adapter(TIMER_INDEX_1, TIMER_1_IRQN, irq_prio(TIMER_1_IRQN));

    uapi_dma_init();
    uapi_dma_open();

    uapi_ipc_init();
#ifdef AT_IPC_V1_TEST
    test_ipc_init();
    test_ipc_async_init();
#endif
    uapi_drv_cipher_env_init();
    ipc_acce_ecdh_init();
    secure_verify_init();

    log_memory_region_init();

    uapi_pmu_init();
    chip_power_on_config();
#if CHIP_ASIC
    uapi_clocks_init();
#endif
    open_chipwdt_rec();

#ifdef CFG_DRIVERS_QSPI
    uapi_clocks_switch(CLOCKS_ID_M_QSPI_DISPLAY, TURN_ON);
#endif
    uapi_sec_trng_random_buffer_master_init();

    uapi_pm_lpc_init();
#ifdef CFG_DRIVERS_SDIO
    clocks_set_sdio_freq(CLOCKS_SDIO_FREQ_32M);
    uapi_pmu_hs_sub_control(HS_SUB_SDIO_HOST_ID, PMU_POWER_ON_AND_RUN);
#endif
#ifdef CFG_DRIVERS_MMC
    uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID, PMU_POWER_ON_AND_RUN);
#elif defined(CFG_DRIVERS_NANDFLASH)
    uapi_pmu_hs_sub_control(HS_SUB_FMC_ID, PMU_POWER_ON_AND_RUN);
#endif
    psram_init_handle();
}
