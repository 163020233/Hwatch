/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm device port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */
#include "pm_dev_porting.h"
#include "common_def.h"
#include "errcode.h"
#include "pm_dev.h"
#include "platform_core.h"
#include "chip_io.h"
#include "tcxo.h"
#include "soc_osal.h"
#if defined(CFG_DRIVERS_MMC) || defined(CFG_DRIVERS_SDIO)
#include "soc/mmc.h"
#include "pmu_sub_control.h"
#endif
#if defined(CONFIG_UART_SUPPORT_CUSTOMIZED_LPM)
#include "uart.h"
#endif
#if defined(CONFIG_DMA_SUPPORT_LPM)
#include "dma.h"
#endif
#if defined(CONFIG_I2C_SUPPORT_LPM)
#include "i2c.h"
#endif
#if defined(CONFIG_SPI_SUPPORT_LPM)
#include "spi.h"
#endif
#if defined(CONFIG_SFC_SUPPORT_LPM)
#include "sfc_v2.h"
#endif
#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
#include "watchdog.h"
#endif
#if defined(CONFIG_PSRAM_SUPPORT_LPM)
#include "arch/cache.h"
#include "psram_v2.h"
#endif
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
#include "cpu_trace_porting.h"
#include "cpu_trace_unified.h"
#include "hal_cpu_trace_regs_def.h"
#include "hal_cpu_trace_regs_op.h"
#endif
#if defined(CONFIG_VIDEO_SUPPORT_LPM)
#include "pm_pmu.h"
#include "soc_lcd_api.h"
#include "drv_gpu_intf.h"
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
#include "fmc_porting.h"
#endif
#if defined(CONFIG_PM_DEV_SUSPEND_DFX)
#include "hal_clocks_ccrg.h"
#endif
#if defined(CONFIG_PWM_SUPPORT_LPM)
#include "pwm_porting.h"
#endif
#if defined(CONFIG_GPIO_SUPPORT_LPM)
#include "gpio.h"
#endif
extern void HalClockEnable(void);

#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
static errcode_t uapi_cpu_trace_customized_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

static errcode_t uapi_cpu_trace_customized_resume(uintptr_t arg)
{
    unused(arg);
    cpu_trace_port_enable();
#if CORE == BT
    hal_cpu_trace_set_clock(BCPU_TRACE_CLOCK);
#elif CORE == APPS
    hal_cpu_trace_set_clock(MCPU_TRACE_CLOCK);
#endif
    hal_cpu_trace_set_sample_sync(1);
    hal_cpu_trace_set_sample_enable(1);
    hal_cpu_trace_set_enable(1);
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_cpu_trace_register = {
    .suspend = uapi_cpu_trace_customized_suspend,
    .suppend_arg = 1,
    .resume = uapi_cpu_trace_customized_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_DMA_SUPPORT_LPM)
static const pm_dev_ops_t g_dev_dma_register = {
    .suspend = uapi_dma_suspend,
    .suppend_arg = 1,
    .resume = uapi_dma_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
#define REG_ADDR_LEN                             4
#define MCPU_SUPPORT_RESORE_S_MGPIO_PIN_NUM      27
static uint8_t g_pinmux_s_mgpio_restore[MCPU_SUPPORT_RESORE_S_MGPIO_PIN_NUM];

static errcode_t uapi_pin_ctrl_suspend(uintptr_t arg)
{
    unused(arg);
    for (uint32_t i = 0; i < MCPU_SUPPORT_RESORE_S_MGPIO_PIN_NUM; i++) {
        g_pinmux_s_mgpio_restore[i] = readw(CFG_S_MGPIO_MODE_CFG_S_MGPIO0_MODE_REG + i * REG_ADDR_LEN);
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_pin_ctrl_resume(uintptr_t arg)
{
    unused(arg);
    // qspi 送显
#ifdef QSPI_DISPLAY
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO0_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO1_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO2_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO3_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO4_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO5_MODE_REG, 1);
#endif
    // norflash
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO6_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO7_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO8_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO9_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO10_MODE_REG, 1);
    writew(CFG_S_EGPIO_MODE_CFG_S_EGPIO11_MODE_REG, 1);

    for (uint32_t i = 0; i < MCPU_SUPPORT_RESORE_S_MGPIO_PIN_NUM; i++) {
        writew(CFG_S_MGPIO_MODE_CFG_S_MGPIO0_MODE_REG + i * REG_ADDR_LEN, g_pinmux_s_mgpio_restore[i]);
    }
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_pin_register = {
    .suspend = uapi_pin_ctrl_suspend,
    .suppend_arg = 1,
    .resume = uapi_pin_ctrl_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_GPIO_SUPPORT_LPM)
static const pm_dev_ops_t g_dev_gpio_register = {
    .suspend = uapi_gpio_suspend,
    .suppend_arg = 1,
    .resume = uapi_gpio_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
static const pm_dev_ops_t g_dev_watchdog_register = {
    .suspend = uapi_watchdog_suspend,
    .suppend_arg = 1,
    .resume = uapi_watchdog_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_UART_SUPPORT_CUSTOMIZED_LPM)
typedef struct {
    volatile uint16_t uartctl;
    volatile uint16_t uartint_en;
    volatile uint16_t uartfifo_ctl;
    volatile uint16_t uartmodem_ctl;
    volatile uint16_t uarthalt_tx;
    volatile uint16_t uartbaud_ctl;
    volatile uint16_t uart_parm;
    volatile uint16_t uartlp_ctl;
    volatile uint16_t uartreceive_ctl;
    volatile uint16_t uartwl_ctl;
} uart_resume_regs_t;
static uart_resume_regs_t g_pm_uart_reg[PM_RESUME_UART_BUS_NUM] = { 0 };

static errcode_t uapi_uart_customized_suspend(uintptr_t arg)
{
#define PM_UART_BUSY_STS                0x10
    unused(arg);
    for (uint32_t i = 0; i < PM_RESUME_UART_BUS_NUM; i++) {
        if (hal_uart_get_funcs(i) == NULL) {
            continue;
        }

        uint32_t base = uart_porting_base_addr_get(i);
        g_pm_uart_reg[i].uartctl = readw(base + 0x8);
        g_pm_uart_reg[i].uartint_en = readw(base + 0x18);
        g_pm_uart_reg[i].uartfifo_ctl = readw(base + 0x24);
        g_pm_uart_reg[i].uartmodem_ctl = readw(base + 0x2c);
        g_pm_uart_reg[i].uarthalt_tx = readw(base + 0x50);
        g_pm_uart_reg[i].uartbaud_ctl = readw(base + 0x58);
        g_pm_uart_reg[i].uart_parm = readw(base + 0x60);
        g_pm_uart_reg[i].uartlp_ctl = readw(base + 0x64);
        g_pm_uart_reg[i].uartreceive_ctl = readw(base + 0x68);
        g_pm_uart_reg[i].uartwl_ctl = readw(base + 0x6c);

        if ((readw(base + 0x44) & PM_UART_BUSY_STS) != 0) {
            reg16_setbit(base + 0x24, 0x6);
            reg16_setbit(base + 0x24, 0x5);
        }
    }

    return ERRCODE_SUCC;
}

static errcode_t uapi_uart_customized_resume(uintptr_t arg)
{
    unused(arg);
    for (uint32_t i = 0; i < PM_RESUME_UART_BUS_NUM; i++) {
        if (hal_uart_get_funcs(i) == NULL) {
            continue;
        }

        uint32_t base = uart_porting_base_addr_get(i);
        writew(base + 0x8, g_pm_uart_reg[i].uartctl);
        writew(base + 0x50, g_pm_uart_reg[i].uarthalt_tx);

        uint32_t div_val = uart_porting_div_value_get(i);
        reg16_setbit(base + 0x8, 0);
        writew(base + 0xc, (div_val >> UART_DIV_INTE_WIDTH) & 0xff);      /* div_h register */
        writew(base + 0x10, div_val & 0xff);            /* div_l register */
        writew(base + 0x14, (div_val >> UART_DIV_FRAC_WIDTH) & 0xffff);  /* div_fra register */
        reg16_clrbit(base + 0x8, 0);

        writew(base + 0x24, g_pm_uart_reg[i].uartfifo_ctl);
        writew(base + 0x2c, g_pm_uart_reg[i].uartmodem_ctl);
        writew(base + 0x18, g_pm_uart_reg[i].uartint_en);
        writew(base + 0x68, g_pm_uart_reg[i].uartreceive_ctl);
    }
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_uart_register = {
    .suspend = uapi_uart_customized_suspend,
    .suppend_arg = 1,
    .resume = uapi_uart_customized_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_I2C_SUPPORT_LPM)
static const pm_dev_ops_t g_dev_i2c_register = {
    .suspend = uapi_i2c_suspend,
    .suppend_arg = 1,
    .resume = uapi_i2c_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_SPI_SUPPORT_LPM)
static errcode_t uapi_spi_customized_suspend(uintptr_t arg)
{
    unused(arg);
    for (uint32_t i = 0; i < SPI_BUS_MAX_NUM; i++) {
        uapi_spi_suspend(i);
    }
    return ERRCODE_SUCC;
}

static errcode_t uapi_spi_customized_resume(uintptr_t arg)
{
    unused(arg);
    for (uint32_t i = 0; i < SPI_BUS_MAX_NUM; i++) {
        uapi_spi_resume(i);
    }
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_spi_register = {
    .suspend = uapi_spi_customized_suspend,
    .suppend_arg = 1,
    .resume = uapi_spi_customized_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_SFC_SUPPORT_LPM)
static errcode_t uapi_sfc_customized_suspend(uintptr_t arg)
{
    return uapi_sfc_suspend(0, arg);
}

static errcode_t uapi_sfc_customized_resume(uintptr_t arg)
{
    return uapi_sfc_resume(0, arg);
}

static const pm_dev_ops_t g_dev_sfc_register = {
    .suspend = uapi_sfc_customized_suspend,
    .suppend_arg = 1,
    .resume = uapi_sfc_customized_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_PSRAM_SUPPORT_LPM)
static errcode_t uapi_psram_customized_suspend(uintptr_t arg)
{
    ArchDCacheFlush();
    return uapi_psram_suspend(arg);
}

static errcode_t uapi_psram_customized_resume(uintptr_t arg)
{
    return uapi_psram_resume(arg);
}

static const pm_dev_ops_t g_dev_psram_register = {
    .suspend = uapi_psram_customized_suspend,
    .suppend_arg = 1,
    .resume = uapi_psram_customized_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_PWM_SUPPORT_LPM)
static errcode_t pm_pwm_clock_ctrl(uint32_t bus, bool on)
{
    unused(bus);
    pwm_port_clock_enable(on);
    return ERRCODE_SUCC;
}
#endif

#if defined(CFG_DRIVERS_NANDFLASH)
static const pm_dev_ops_t g_dev_fmc_register = {
    .suspend = uapi_fmc_suspend,
    .suppend_arg = 1,
    .resume = uapi_fmc_resume,
    .resume_arg = 1,
};
#endif

#if defined(CFG_DRIVERS_MMC)
static errcode_t uapi_emmc_suspend(uintptr_t arg)
{
    unused(arg);
    emmc_save_info();
    uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID, PMU_POWER_OFF);
    return ERRCODE_SUCC;
}

static errcode_t uapi_emmc_resume(uintptr_t arg)
{
    unused(arg);
    uapi_pmu_hs_sub_control(HS_SUB_EMMC_ID, PMU_POWER_ON_AND_RUN);
    emmc_recovery_init();
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_emmc_register = {
    .suspend = uapi_emmc_suspend,
    .suppend_arg = 1,
    .resume = uapi_emmc_resume,
    .resume_arg = 1,
};
#endif

#if defined(CFG_DRIVERS_SDIO)
static errcode_t uapi_sdio_host_suspend(uintptr_t arg)
{
    unused(arg);
    sdio_save_info();
    uapi_pmu_hs_sub_control(HS_SUB_SDIO_HOST_ID, PMU_POWER_OFF);
    return ERRCODE_SUCC;
}

static errcode_t uapi_sdio_host_resume(uintptr_t arg)
{
    unused(arg);
    uapi_pmu_hs_sub_control(HS_SUB_SDIO_HOST_ID, PMU_POWER_ON_AND_RUN);
    sdio_recovery_init();
    return ERRCODE_SUCC;
}

static const pm_dev_ops_t g_dev_sdio_host_register = {
    .suspend = uapi_sdio_host_suspend,
    .suppend_arg = 1,
    .resume = uapi_sdio_host_resume,
    .resume_arg = 1,
};
#endif

#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)

switch_type_t g_display_power_sts = TURN_ON; // 标记当前显示屏上电状态，默认上电
static td_bool g_display_suspend = TD_FALSE; // 重入场景，防重入标记

errcode_t uapi_video_suspend(uintptr_t arg)
{
    uint32_t irq_sts = osal_irq_lock();
    if ((g_display_power_sts == TURN_OFF) || g_display_suspend) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }

    (void)uapi_vau_suspend(NULL);
    (void)uapi_dpu_suspend(NULL);
#ifdef SUPPORT_GPU_GMMU
    (void)uapi_gmmu_suspend(NULL);
#endif
    ext_errno ret = uapi_lcd_suspend();
    if (ret != EXT_ERR_SUCCESS) {
        osal_printk("lcd_suspend fail:0x%x\n", ret);
        if (ret == EXT_ERR_LCD_NOT_INIT) {
            g_display_power_sts = TURN_OFF;
        }
    }

    uapi_pmu_control(PMU_VAU_SUB, PMU_POWER_OFF);
    uapi_pmu_control(PMU_DISPLAY_SUB, PMU_POWER_OFF);

    if (arg == PM_VIDEO_SWITCH_SCREEN_STA) {
        g_display_power_sts = TURN_OFF;
    }
    g_display_suspend = TD_TRUE;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

errcode_t uapi_video_resume(uintptr_t arg)
{
    uint32_t irq_sts = osal_irq_lock();
    if (arg == PM_VIDEO_SWITCH_SCREEN_STA) {
        g_display_power_sts = TURN_ON;
    }

    if ((g_display_power_sts == TURN_OFF) || !g_display_suspend) {
        osal_irq_restore(irq_sts);
        return ERRCODE_SUCC;
    }
    uapi_pmu_control(PMU_DISPLAY_SUB, PMU_POWER_ON_AND_RUN);
    uapi_pmu_control(PMU_VAU_SUB, PMU_POWER_ON_AND_RUN);

    ext_errno ret = uapi_lcd_resume();
    if (ret != EXT_ERR_SUCCESS) { osal_printk("lcd_resume fail:0x%x\n", ret); }
    (void)uapi_dpu_resume(NULL);
    (void)uapi_vau_resume(NULL);
#ifdef SUPPORT_GPU_GMMU
    (void)uapi_gmmu_resume(NULL);
#endif
    g_display_suspend = TD_FALSE;
    osal_irq_restore(irq_sts);
    return ERRCODE_SUCC;
}

void uapi_pm_set_display_power_sts(switch_type_t type)
{
    uint32_t irq_sts = osal_irq_lock();
    g_display_power_sts = type;
    osal_irq_restore(irq_sts);
}

static const pm_dev_ops_t g_dev_video_register = {
    .suspend = uapi_video_suspend,
    .suppend_arg = PM_VIDEO_SWITCH_LOW_PWR_STA,
    .resume = uapi_video_resume,
    .resume_arg = PM_VIDEO_SWITCH_LOW_PWR_STA,
};
#endif

#if CORE == APPS
void pm_register_dev_resume_interface(void)
{
    // suspend from PM_DEV_VIDEO -> PM_DEV_CPU_TRACE, reusme from PM_DEV_CPU_TRACE -> PM_DEV_VIDEO
#if defined(CONFIG_VIDEO_SUPPORT_LPM) && defined(SUPPORT_DISPLAY_TECH)
    uapi_pm_register_dev_ops(PM_DEV_VIDEO, (pm_dev_ops_t *)&g_dev_video_register);
#endif
#if defined(CFG_DRIVERS_SDIO)
    uapi_pm_register_dev_ops(PM_DEV_SDIO_HOST, (pm_dev_ops_t *)&g_dev_sdio_host_register);
#endif
#if defined(CONFIG_GPIO_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_GPIO, (pm_dev_ops_t *)&g_dev_gpio_register);
#endif
#if defined(CFG_DRIVERS_MMC)
    uapi_pm_register_dev_ops(PM_DEV_EMMC, (pm_dev_ops_t *)&g_dev_emmc_register);
#endif
#if defined(CFG_DRIVERS_NANDFLASH)
    uapi_pm_register_dev_ops(PM_DEV_FMC, (pm_dev_ops_t *)&g_dev_fmc_register);
#endif
#if defined(CONFIG_SPI_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_SPI, (pm_dev_ops_t *)&g_dev_spi_register);
#endif
#if defined(CONFIG_I2C_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_I2C, (pm_dev_ops_t *)&g_dev_i2c_register);
#endif
#if defined(CONFIG_DMA_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_DMA, (pm_dev_ops_t *)&g_dev_dma_register);
#endif
#if defined(CONFIG_PSRAM_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_PSRAM, (pm_dev_ops_t *)&g_dev_psram_register);
#endif
#if defined(CONFIG_SFC_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_SFC, (pm_dev_ops_t *)&g_dev_sfc_register);
#endif
#if defined(CONFIG_PINCTRL_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_PINCTRL, (pm_dev_ops_t *)&g_dev_pin_register);
#endif
#if defined(CONFIG_UART_SUPPORT_CUSTOMIZED_LPM)
    uapi_pm_register_dev_ops(PM_DEV_UART, (pm_dev_ops_t *)&g_dev_uart_register);
#endif
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_CPU_TRACE, (pm_dev_ops_t *)&g_dev_cpu_trace_register);
#endif
#if defined(CONFIG_PM_DEV_SUSPEND_DFX)
    uint16_t value = readw(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG);
    if (value != 0) {
        PRINT("reg value: 0x%x"NEWLINE, value);
    }
#endif
}
#endif

#if CORE == BT
void pm_register_dev_resume_interface(void)
{
    // suspend from PM_DEV_DMA -> PM_DEV_CPU_TRACE, reusme from PM_DEV_CPU_TRACE -> PM_DEV_DMA
#if defined(CONFIG_DMA_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_DMA, (pm_dev_ops_t *)&g_dev_dma_register);
#endif
#if defined(CONFIG_WATCHDOG_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_WATCHDOG, (pm_dev_ops_t *)&g_dev_watchdog_register);
#endif
#if defined(CONFIG_UART_SUPPORT_CUSTOMIZED_LPM)
    uapi_pm_register_dev_ops(PM_DEV_UART, (pm_dev_ops_t *)&g_dev_uart_register);
#endif
#if defined(CONFIG_CPU_TRACE_SUPPORT_LPM)
    uapi_pm_register_dev_ops(PM_DEV_CPU_TRACE, (pm_dev_ops_t *)&g_dev_cpu_trace_register);
#endif
}
#endif

#if defined(CONFIG_PM_DEV_SUSPEND_DFX)
void uapi_pm_dev_record_suspend(uint8_t id)
{
    reg16_setbit(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG, id);
}

void uapi_pm_dev_record_resume(uint8_t id)
{
    reg16_clrbit(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG, id);
}

uint8_t uapi_pm_dev_get_suspend_info(uint8_t id)
{
    return reg16_getbit(GLB_CTL_M_RB_GP_REG_NOWDT_RST_REG_17_REG, id);
}
#endif

void pm_register_clock_interface(void)
{
#if defined(CONFIG_PWM_SUPPORT_LPM)
    pm_clock_register(PM_DEV_PWM, pm_pwm_clock_ctrl);
#endif
}
