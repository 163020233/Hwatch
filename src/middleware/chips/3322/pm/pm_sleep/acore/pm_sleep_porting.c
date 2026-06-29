/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provides pm sleep port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "pm_sleep_porting.h"
#include "common_def.h"
#include "chip_io.h"
#include "chip_core_irq.h"
#include "platform_core.h"
#include "osal_interrupt.h"
#include "los_task_pri.h"
#include "arch_barrier.h"
#include "arch_encoding.h"
#include "debug_print.h"
#include "hal_systick.h"
#include "systick.h"
#include "rtc.h"
#include "hal_tcxo.h"
#include "pm_veto.h"
#include "pm_porting.h"
#include "psram_v2.h"
#include "pinctrl.h"
#if defined(CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS) && (CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS == 1)
#include "pm_dfx_porting.h"
#endif
#include "pmu_mem.h"
#include "watchdog_porting.h"
#if defined(CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP) && (CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP == 1)
#include "soc_lcd_api.h"
#endif
#include "uart.h"

#define PM_OS_TICKS_PER_S       LOSCFG_BASE_CORE_TICK_PER_SECOND
#define PM_OS_TICKS_PER_MS      (PM_OS_TICKS_PER_S / LOSCFG_BASE_CORE_TICK_PER_SECOND)
#define PM_WAKEUP_EN_MASK       (LP_CTL_MCU_RB_ULP_PWRON_DIS_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_ULP_GPIO_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_PWM_CFG_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_M_RTC_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_M_L0_UART_RXD_IN_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_M_IPC_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_M_GPIO_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_CWDT_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_DSP_WDT_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_BT_WDT_INTR_WKUP_EN_MASK)

#define PM_WAKEUP_EN_MASK_1     (LP_CTL_MCU_RB_ULP_VBUS_SENSE_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_ULP_VBUS_SENSE_DIS_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_ULP_SYS_TICK_ALARM_INTR_WKUP_EN_MASK | \
                                 LP_CTL_MCU_RB_ULP_PWRON_INTR_WKUP_EN_MASK)

#define PM_ULP_WKUP_EN_MASK     (LP_CTL_ULP_A_RB_PWRON_WKUP_SHIP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_ULP_GPIO_INT_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_SYS_TICK_ALARM_INT_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_PWRON_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_VBUS_SENSE_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_VBUS_SENSE_DIS_WKUP_EN_MASK | \
                                 LP_CTL_ULP_A_RB_PWRON_DIS_WKUP_EN_MASK)

#define PM_UART_DEBUG_VETO_TIMEOUT_MS        2000UL
#define PM_FORCE_SLEEP_NO_WAKE_MAGIC         0x5A5A
#define PM_WAKEUP_STA_OFFSET_16              16

static uint64_t g_entry_sleep_time = 0;
static uint64_t g_exit_sleep_time = 0;
static rtc_handle_t g_pm_rtc = NULL;
static uint16_t g_pm_nmi_int_en_val = 0;
static uint16_t g_pm_debug_level = 0;
static uint8_t g_display_intr_en = 0;
static uint64_t g_pm_resume_tcxo_count;
pm_sleep_record_t g_pm_sleep_record;

uint64_t get_timer_base(void);
uint64_t get_last_timer_systick(void);
void HalClockEnableAndReload(uint32_t value);
void clear_last_timer_systick(void);
#if !defined(BUILD_RECOVERY_IMAGE)
extern int lowpower_entry(uint32_t param1, uint32_t param2, uint32_t param3, uint32_t param4); // tee 侧进wfi
#endif

pm_sleep_record_t* get_pm_sleep_record(void)
{
    return &g_pm_sleep_record;
}

void pm_port_set_start_time(void)
{
    if (g_pm_debug_level) {
        g_pm_resume_tcxo_count = hal_tcxo_get();
    }
}

static void pm_clear_slp_int_and_event(void)
{
    // /* makesure event and int clr */
    uint16_t mask = (LP_CTL_MCU_RB_MCPU_SLEEPING_STS_MASK | LP_CTL_MCU_RB_MCPU_SLEEPING_INT_STS_MASK);
    do {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_SLEEPING_REG,
            LP_CTL_MCU_RB_MCPU_SLEEPING_CLR_MASK | LP_CTL_MCU_RB_MCPU_SLEEPING_INT_CLR_MASK);
    } while ((readw(LP_CTL_MCU_RB_LP_CTL_MCU_STS_2_REG) & mask) != 0);
}

void pm_port_set_debug_level(uint16_t level)
{
    g_pm_debug_level = level;
    PRINT("g_pm_debug_level:%u."NEWLINE, g_pm_debug_level);
    return;
}

void pm_port_start_tickless(void)
{
    g_entry_sleep_time = hal_systick_get_count();
    HalClockDisable();
}

void pm_port_stop_tickless(uint32_t sleep_ms)
{
#define PM_SLEEP_VETO_SLEEP_TIMEOUT_COUNT       40  // veto timeout > 1ms

    unused(sleep_ms);
#if defined(CONFIG_SUPPORT_XO_32K) // 绝对时间补偿方案, 依赖xo 32k
    g_exit_sleep_time = hal_systick_get_count();
    uint32_t rtc_clock = rtc_porting_clock_value_get();
    uint64_t abs_cnt_diff = (g_exit_sleep_time - get_timer_base()) * PM_OS_TICKS_PER_S;
    // 1. tickcount 补偿
    uint64_t os_tick = osal_get_jiffies();
    uint64_t rtc_ms = abs_cnt_diff / rtc_clock;
    if (os_tick < rtc_ms) {
        oal_ticks_restore((uint32_t)(rtc_ms - os_tick));
    }

    // 2. timer tick align
    uint32_t timer_reload = (rtc_clock - (abs_cnt_diff % rtc_clock)) * OS_CYCLE_PER_TICK / rtc_clock;
    HalClockEnableAndReload(timer_reload);
#else   // 相对补偿方案，不依赖xo 32k
    unused(sleep_ms);
    g_exit_sleep_time = hal_systick_get_count();
    uint32_t rtc_clock = rtc_porting_clock_value_get();
    // 1. tickcount 补偿
    uint32_t enter_sleep_tick = get_last_timer_systick() == 0 ? g_entry_sleep_time : get_last_timer_systick();
    uint32_t actual_sleep_count = ((uint32_t)(g_exit_sleep_time - enter_sleep_tick) * PM_OS_TICKS_PER_S);
    oal_ticks_restore(actual_sleep_count / rtc_clock);
    clear_last_timer_systick();

    // 2. timer tick align
    uint32_t timer_reload = (rtc_clock - (actual_sleep_count % rtc_clock)) * OS_CYCLE_PER_TICK / rtc_clock;
    HalClockEnableAndReload(timer_reload);
    uapi_pm_add_sleep_veto_with_timeout_cnt(PM_ID_BSP, PM_SLEEP_VETO_SLEEP_TIMEOUT_COUNT);
#endif

    if (g_pm_debug_level) {
        g_pm_resume_tcxo_count = hal_tcxo_get() - g_pm_resume_tcxo_count;
    }
}

uint32_t oal_get_sleep_ticks_debug(VOID)
{
    UINT32 taskId, swtmrId;
    UINTPTR taskHandler, timerHandler;

    uint32_t intSave = osal_irq_lock();
    uint32_t taskTimeout = OsSortLinkGetNextTaskExpireTime(&taskId, &taskHandler);
    uint32_t swtmrTimeout = OsSortLinkGetNextSwtmrExpireTime(&swtmrId, &timerHandler);

#ifndef CFG_DRIVERS_USB_AS_SERIAL
    if (g_pm_debug_level >= 0x2) {
        if (swtmrTimeout < taskTimeout) {
            PRINT("timer id: 0x%x, %dms, 0x%x"NEWLINE, swtmrId, swtmrTimeout, timerHandler);
        } else {
            PRINT("task id: 0x%x, %dms, 0x%x"NEWLINE, taskId, taskTimeout, taskHandler);
        }
        while (!uapi_uart_tx_fifo_is_empty(SW_DEBUG_UART_BUS)) {
        }
    }
#endif

    if (swtmrTimeout < taskTimeout) {
        taskTimeout = swtmrTimeout;
    }
    osal_irq_restore(intSave);
    return taskTimeout;
}

uint32_t pm_port_get_sleep_ms(void)
{
    return oal_get_sleep_ticks_debug() / PM_OS_TICKS_PER_MS;
}

void pm_port_allow_deepsleep(bool allow)
{
    if (allow) {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_ALLOW_TO_SLEEP_REG, LP_CTL_MCU_RB_M_ALLOW_TO_SLEEP_MASK);
    } else {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_ALLOW_TO_SLEEP_REG, 0);
    }
}

void pm_port_enter_wfi(void)
{
    if (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_ALLOW_TO_SLEEP_REG, LP_CTL_MCU_RB_M_ALLOW_TO_SLEEP_OFFSET) != 0) {
        dsb();
#if defined(BUILD_RECOVERY_IMAGE)
        wfi();
#else
        lowpower_entry(0, 0, 0, 0); // deepsleep goto tee lowpower_entry, will return when wakeup from deepsleep
#endif
        isb();
    } else {
        dsb();
        wfi();
        isb();
        nop();
        nop();
        nop();
        nop();
    }
}

static void pm_sleep_rtc_callback(uintptr_t data)
{
    unused(data);
}

void pm_port_start_wakeup_timer(uint32_t sleep_ms)
{
#define LIGHT_SLEPP_BEFORE_VALUE_MS      2
#define DEEP_SLEPP_BEFORE_VALUE_MS       3

    uint32_t rtc_ms = sleep_ms;
    if (sleep_ms <= CONFIG_PM_DEEP_SLEEP_THRESHOLD_MS) {
        rtc_ms = sleep_ms - LIGHT_SLEPP_BEFORE_VALUE_MS;
    } else {
        rtc_ms = sleep_ms - DEEP_SLEPP_BEFORE_VALUE_MS;
    }

    if (g_pm_rtc == NULL) {
        uapi_rtc_create(RTC_0, &g_pm_rtc);
        uapi_rtc_start(g_pm_rtc, rtc_ms, pm_sleep_rtc_callback, 0);
    } else {
        uapi_rtc_stop(g_pm_rtc);
        uapi_rtc_start(g_pm_rtc, rtc_ms, pm_sleep_rtc_callback, 1);
    }
}

void pm_port_lightsleep_config(void)
{
    pm_port_allow_deepsleep(false);
}

void pm_port_light_wakeup_config(void)
{
    return;
}

void pm_port_force_sleep(void)
{
    writew(LP_CTL_GLB_RB_CFG_LP_CTL_GLB_GP_REG4_REG, PM_FORCE_SLEEP_NO_WAKE_MAGIC);
}

void pm_port_set_display_intr_en(uint8_t en)
{
    g_display_intr_en = en;
}

void pm_port_deepsleep_config(void)
{
    g_pm_nmi_int_en_val = readw(M_CTL_RB_M_NMI_INT_EN_REG);

    /* enable sleep event and wakeup */
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_SLEEPING_REG, LP_CTL_MCU_RB_MCPU_SLEEPING_EN_MASK); // enable sleeping event

    if (readw(LP_CTL_GLB_RB_CFG_LP_CTL_GLB_GP_REG4_REG) != PM_FORCE_SLEEP_NO_WAKE_MAGIC) {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_REG, PM_WAKEUP_EN_MASK);            // wakeup event
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_1_REG, PM_WAKEUP_EN_MASK_1);        // ulp wakeup event
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_REG, PM_WAKEUP_EN_MASK);        // wakeup int
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_1_REG, PM_WAKEUP_EN_MASK_1);    // ulp wakeup int
    } else {
        watchdog_turnoff_clk();
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_REG, 0);        // wakeup event
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_1_REG, 0);      // ulp wakeup event
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_REG, 0);    // wakeup int
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_1_REG, 0);  // ulp wakeup int
    }

#if defined(CONFIG_PM_SUPPORT_DSI_TE_WKUP) && (CONFIG_PM_SUPPORT_DSI_TE_WKUP == 1)
    /* dsi_te and mcu_int event and int */
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_EN_OFFSET);
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_INT_EN_OFFSET);
#endif
#if defined(CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP) && (CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP == 1)
    if (g_display_intr_en) {
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_INTR_EN_REG, LP_CTL_MCU_RB_DISPLAY_INTR_EN_OFFSET);
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_INT_EN_OFFSET);
    }
    uapi_lcd_set_mcuint2te_time(WAKE_SCENE_MCU_INT_TO_TE_TIME);
#endif
}

void pm_port_deep_wakeup_config(void)
{
    pm_clear_slp_int_and_event();

#ifdef CONFIG_PSRAM_SUPPORT_HALF_SLEEP
    if (uapi_psram_wait_hs_status(0) != ERRCODE_SUCC) {
        PRINT("Psram wait hs status timeout"NEWLINE);
    }
#endif

    if (g_pm_debug_level >= 0x2) {
        uint32_t vendor_id = uapi_psram_read_vendor_id();
        if ((vendor_id != 0xD) && (vendor_id != 0x5) && (vendor_id != 0x6)) {
            PRINT("psram id err 0x%x" NEWLINE, vendor_id);
        }
    }

    writew(M_CTL_RB_M_NMI_INT_EN_REG, g_pm_nmi_int_en_val);
}

// 唤醒时间配置寄存器地址
typedef struct reg_cfg {
    uint32_t reg_addr;
    uint16_t value;
} reg_cfg_t;

// rc 200us
static const reg_cfg_t g_wakeup_wait_time_reg_config[] = {
    // ulp
    { LP_CTL_ULP_A_RB_SHIP_MDOE_FLAG_BOOT_TIME_REG, 0x0 },
    { LP_CTL_ULP_A_RB_LP_REF_BG_BOOT_TIME_REG, 0x2 },
    { LP_CTL_ULP_A_RB_LP_REF_IBG_BOOT_TIME_REG, 0x9 },
    { LP_CTL_ULP_A_RB_LP_REF_BUF_BOOT_TIME_REG, 0x10 },
    { LP_CTL_ULP_A_RB_OVP_EN_BOOT_TIME_REG, 0x17 },
    { LP_CTL_ULP_A_RB_ULVO_EN_BOOT_TIME_REG, 0x17 },
    { LP_CTL_ULP_A_RB_IOLDO4_EN_ECO_BOOT_TIME_REG, 0x1E },
    { LP_CTL_ULP_A_RB_BUCK0_EN_BOOT_TIME_REG, 0x21 },
    { LP_CTL_ULP_A_RB_BUCK1_EN_BOOT_TIME_REG, 0x2B },
    { LP_CTL_ULP_A_RB_RST_DBB_BOOT_TIME_REG, 0x3D },
    { LP_CTL_ULP_A_RB_AON_TO_ULP_ISO_BOOT_TIME_REG, 0x38 },
    { LP_CTL_ULP_A_RB_SHIP_MDOE_FLAG_SAV_TIME_REG, 0x1D },
    { LP_CTL_ULP_A_RB_LP_REF_BG_SAV_TIME_REG, 0x21 },
    { LP_CTL_ULP_A_RB_LP_REF_IBG_SAV_TIME_REG, 0x20 },
    { LP_CTL_ULP_A_RB_LP_REF_BUF_SAV_TIME_REG, 0x1F },
    { LP_CTL_ULP_A_RB_OVP_EN_SAV_TIME_REG, 0x1E },
    { LP_CTL_ULP_A_RB_ULVO_EN_SAV_TIME_REG, 0x1E },
    { LP_CTL_ULP_A_RB_IOLDO4_EN_ECO_SAV_TIME_REG, 0x19 },
    { LP_CTL_ULP_A_RB_BUCK0_EN_SAV_TIME_REG, 0x18 },
    { LP_CTL_ULP_A_RB_BUCK1_EN_SAV_TIME_REG, 0x6 },
    { LP_CTL_ULP_A_RB_RST_DBB_SAV_TIME_REG, 0x1 },
    { LP_CTL_ULP_A_RB_AON_TO_ULP_ISO_SAV_TIME_REG, 0x5 },

    // pmu2 200rc
    { LP_CTL_GLB_RB_LP_CTL_GLB_DELAY_TIME_2_REG, 0x7 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_FIRST_BOOT_CNT_END_REG, 0x1f },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_11_REG, 0x1 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_12_REG, 0x1 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_13_REG, 0x17 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_14_REG, 0x18 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_20_REG, 0x10 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_30_REG, 0x2 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_31_REG, 0x9 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_32_REG, 0x16 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_33_REG, 0x17 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_34_REG, 0x18 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_35_REG, 0x1 },
    { LP_CTL_GLB_RB_LP_CTL_GLB_WKUP_TIME_36_REG, 0x1a },

    // mcu rc200
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_REG, 0x18 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_1_REG, 0x0a },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_2_REG, 0x0b },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_3_REG, 0x24 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_4_REG, 0x0d },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_5_REG, 0x0c },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_6_REG, 0x0a },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_7_REG, 0x0b },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_8_REG, 0x25 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_9_REG, 0x20 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_10_REG, 0x10 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_11_REG, 0x201 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_13_REG, 0x10 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_14_REG, 0x10 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_15_REG, 0x10 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_16_REG, 0x10 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_17_REG, 0x11 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_18_REG, 0x30 },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_20_REG, 0xe },
    { LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_TIME_23_REG, 0x19 },

    // audio
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_REG, 0x8F },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_1_REG, 0x0a },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_2_REG, 0x0b },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_3_REG, 0x95 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_4_REG, 0x0d },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_5_REG, 0x0c },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_6_REG, 0x0a },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_7_REG, 0x0b },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_8_REG, 0x96 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_9_REG, 0x93 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_10_REG, 0x98 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_12_REG, 0x91 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_13_REG, 0x92 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_14_REG, 0x911 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_15_REG, 0x0e },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_16_REG, 0x91 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_17_REG, 0x93 },
    { LP_CTL_AUDIO_RB_LP_CTL_AUDIO_WKUP_TIME_18_REG, 0x90 },
};

static const reg_cfg_t g_sleep_wait_time_reg_config[] = {
};

void pm_wakeup_wait_time_config(uint16_t level)
{
    unused(level);
    for (uint32_t i = 0; i < sizeof(g_wakeup_wait_time_reg_config) / sizeof(reg_cfg_t); i++) {
        writew(g_wakeup_wait_time_reg_config[i].reg_addr, g_wakeup_wait_time_reg_config[i].value);
    }

    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DELAY_TIME_4_REG, LP_CTL_GLB_RB_D_PMU_XLDO0_EN_DELAY_TIME_OFFSET,
        LP_CTL_GLB_RB_D_PMU_XLDO0_EN_DELAY_TIME_LEN, 0x0a);
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_DELAY_TIME_4_REG, LP_CTL_GLB_RB_HW_TCXO_EXIST_DELAY_TIME_OFFSET,
        LP_CTL_GLB_RB_HW_TCXO_EXIST_DELAY_TIME_LEN, 0x7);
    // seq sw to 1MHz
    reg16_setbit(LP_CTL_GLB_RB_LP_CTL_GLB_DEF_76_REG, LP_CTL_GLB_RB_MCU_PULSE2LEVEL_BYPASS_OFFSET);

    reg16_clrbit(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_CLKEN_OFFSET);
    reg16_clrbit(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_LOAD_DIV_EN_OFFSET);
    reg16_setbits(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_CLK_DIV_NUM_OFFSET,
        GLB_CTL_M_RB_CFG_GLBM_STM_CLK_DIV_NUM_LEN, 0x20); // 32分频 32M->1M
    reg16_setbit(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_LOAD_DIV_EN_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_CLK_SEL_OFFSET);
    reg16_setbit(GLB_CTL_M_RB_LP_CLK_STM_DIV_REG, GLB_CTL_M_RB_CFG_GLBM_STM_CLKEN_OFFSET);

    // modify comsub and mcu sub power on delay cnt
    reg16_setbits(LP_CTL_GLB_RB_LP_CTL_GLB_SEQ_DELAY_CNT_1_REG, LP_CTL_GLB_RB_COM_SUB_PWR_EN_ON_SEQ_DELAY_CNT_OFFSET,
                  LP_CTL_GLB_RB_COM_SUB_PWR_EN_ON_SEQ_DELAY_CNT_LEN, 0x3);
    reg16_setbits(LP_CTL_MCU_RB_LP_CTL_MCU_SEQ_DELAY_CNT_4_REG, LP_CTL_MCU_RB_MCU_SUB_PWR_EN_ON_SEQ_DELAY_CNT_OFFSET,
                  LP_CTL_MCU_RB_MCU_SUB_PWR_EN_ON_SEQ_DELAY_CNT_LEN, 0x3);
}

void pm_wakeup_mode_config(void)
{
}

void pm_sleep_wait_time_config(void)
{
    for (uint32_t i = 0; i < sizeof(g_sleep_wait_time_reg_config) / sizeof(reg_cfg_t); i++) {
        writew(g_sleep_wait_time_reg_config[i].reg_addr, g_sleep_wait_time_reg_config[i].value);
    }
}

void pm_auto_cg_config(bool on)
{
    if (on) {
        writel(M_CTL_RB_MCU_SUB_AUTO_CG_BYPASS0_REG, 0x1001A); // close H2H autocg, sync 1A21
        writew(M_CTL_RB_DMA_HRDATA_EN_BYPASS_REG, 0);
        writew(M_CTL_RB_AHB_HREADY_MUX_BAK_REG, 0);
        writew(XIP_CACHE_CTL_RB_AUTO_CG_BYPASS_REG, 0);
        writew(XIP_CACHE_CTL_RB_MEM_X2X_A2X_CRTL_REG, 0);
        writew(COM_CTL_RB_AUDIO_BRG_LP_CFG_REG, 0);
        writew(COM_CTL_RB_EH2H_AUTO_CG_CFG_REG, 0x34E0);        // close E2H autocg, sync 1A21
        writew(COM_CTL_RB_COM_BUS_LP_CFG_REG, 0);
    } else {
        writel(M_CTL_RB_MCU_SUB_AUTO_CG_BYPASS0_REG, 0x0FFFFFFF);
        writew(M_CTL_RB_DMA_HRDATA_EN_BYPASS_REG, 0x03FC);
        writew(M_CTL_RB_AHB_HREADY_MUX_BAK_REG, 0x3);
        writew(M_CTL_RB_M_AXI_BRG_LP_CFG0_REG, 0x481F);
        writew(XIP_CACHE_CTL_RB_AUTO_CG_BYPASS_REG, 0x1FFF);
        writew(XIP_CACHE_CTL_RB_MEM_X2X_A2X_CRTL_REG, 0x301);
        writew(COM_CTL_RB_EH2H_AUTO_CG_CFG_REG, 0xB4C0);
        writew(COM_CTL_RB_AUDIO_BRG_LP_CFG_REG, 0x333);
        writew(COM_CTL_RB_COM_BUS_LP_CFG_REG, 0x1);
    }
}

static void pm_clear_wakeup_int(uint16_t wkup_int_sts)
{
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_CLR_REG, PM_WAKEUP_EN_MASK);
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_CLR_REG, PM_WAKEUP_EN_MASK);

    if (wkup_int_sts & LP_CTL_MCU_RB_M_L0_UART_RXD_IN_WKUP_INT_STS_MASK) {
        uapi_pm_add_sleep_veto_with_timeout(PM_ID_DEBUG, PM_UART_DEBUG_VETO_TIMEOUT_MS);
    }

    do {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_CLR_REG, PM_WAKEUP_EN_MASK);
    } while ((readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_STS_REG) & PM_WAKEUP_EN_MASK) != 0);

    do {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_CLR_REG, PM_WAKEUP_EN_MASK);        // clear wakeup event
    } while ((readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_STS_REG) & PM_WAKEUP_EN_MASK) != 0);
}

static void pm_clear_wakeup_int_1(uint16_t wkup_int_sts)
{
    unused(wkup_int_sts);
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_CLR_1_REG, PM_WAKEUP_EN_MASK_1);
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_CLR_1_REG, PM_WAKEUP_EN_MASK_1);

    do {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_CLR_1_REG, PM_WAKEUP_EN_MASK_1);
    } while ((readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_STS_1_REG) & PM_WAKEUP_EN_MASK_1) != 0);

    do {
        writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_CLR_1_REG, PM_WAKEUP_EN_MASK_1);        // clear wakeup event
    } while ((readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_STS_1_REG) & PM_WAKEUP_EN_MASK_1) != 0);
}

static void pm_clear_wakeup_int_2(void)
{
#if defined(CONFIG_PM_SUPPORT_DSI_TE_WKUP) && (CONFIG_PM_SUPPORT_DSI_TE_WKUP == 1)
    /* dsi_te and mcu_int event and int */
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_CLR_OFFSET);
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_INT_CLR_OFFSET);
#endif
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_CLR_OFFSET);
    reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_INT_CLR_OFFSET);

    do {
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_CLR_OFFSET);
    } while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_1_REG, LP_CTL_MCU_RB_DSI_TE_STS_OFFSET) != 0);

    do {
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_INT_CLR_OFFSET);
    } while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_1_REG, LP_CTL_MCU_RB_DSI_TE_INT_STS_OFFSET) != 0);

    do {
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_CLR_OFFSET);
    } while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_1_REG, LP_CTL_MCU_RB_DISPLAY_INTR_STS_OFFSET) != 0);

    do {
        reg32_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_INT_CLR_OFFSET);
    } while (reg16_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_1_REG, LP_CTL_MCU_RB_DISPLAY_INTR_INT_STS_OFFSET) != 0);
}

__IRQ void pm_wakeup_handler(void)
{
    uint16_t wkup_int_sts = readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_STS_REG);
    uint16_t wkup_int_sts_1 = readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_STS_1_REG);
    uint16_t wkup_int_sts_2 = readw(LP_CTL_MCU_RB_LP_CTL_MCU_STS_1_REG) &
        (LP_CTL_MCU_RB_DISPLAY_INTR_INT_STS_MASK | LP_CTL_MCU_RB_DSI_TE_INT_STS_MASK);

    if (g_pm_debug_level) {
        PRINT("wkup isr: 0x%x, 0x%x, 0x%x, wkup time: %d us" NEWLINE, wkup_int_sts, wkup_int_sts_1, wkup_int_sts_2,
              (uint32_t)(g_pm_resume_tcxo_count / 8));
    }
    g_pm_sleep_record.ipc_cnts += (wkup_int_sts >> LP_CTL_MCU_RB_M_IPC_INTR_WKUP_INT_STS_OFFSET) & 0x1;
    g_pm_sleep_record.rtc_cnts += (wkup_int_sts >> LP_CTL_MCU_RB_M_RTC_INTR_WKUP_INT_STS_OFFSET) & 0x1;
    g_pm_sleep_record.m_gpio_cnts += (wkup_int_sts >> LP_CTL_MCU_RB_M_DAP_WKUP_INT_STS_OFFSET) & 0x1;
    g_pm_sleep_record.button_cnts += (wkup_int_sts_1 >> LP_CTL_MCU_RB_ULP_PWRON_INTR_WKUP_INT_STS_OFFSET) & 0x1;

    // clear wakeup int and event
    if (wkup_int_sts != 0) {
        pm_clear_wakeup_int(wkup_int_sts);
#if defined(CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS) && (CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS == 1)
        pm_dfx_wakeup_src_statistics(wkup_int_sts);
#endif
    }

    if (wkup_int_sts_1 != 0) {
        pm_clear_wakeup_int_1(wkup_int_sts_1);
#if defined(CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS) && (CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS == 1)
        pm_dfx_wakeup_src_statistics((wkup_int_sts_1 << PM_WAKEUP_STA_OFFSET_16));
#endif
    }

    if (wkup_int_sts_2 != 0) {
        pm_clear_wakeup_int_2();
#if defined(CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS) && (CONFIG_PM_SLEEP_WAKUP_SRC_STATISTICS == 1)
        if (wkup_int_sts_2 & LP_CTL_MCU_RB_DISPLAY_INTR_INT_STS_MASK) {
            pm_dfx_ext_wakeup_src_statistics(DISPLAY_INTR_WKUP);
        }
        if (wkup_int_sts_2 & LP_CTL_MCU_RB_DSI_TE_INT_STS_MASK) {
            pm_dfx_ext_wakeup_src_statistics(TE_INTR_WKUP);
        }
#endif
    }

    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_REG, 0);        // wakeup event
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_EN_1_REG, 0);      // ulp wakeup event
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_REG, 0);    // wakeup int
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_INT_EN_1_REG, 0);  // ulp wakeup int

#if defined(CONFIG_PM_SUPPORT_DSI_TE_WKUP) && (CONFIG_PM_SUPPORT_DSI_TE_WKUP == 1)
    reg32_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG,
        LP_CTL_MCU_RB_DSI_TE_EN_OFFSET); // dsi_te and mcu_int event and int
    reg32_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DSI_TE_INT_EN_OFFSET);
#endif
#if defined(CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP) && (CONFIG_PM_SUPPORT_DISPLAY_INT_WKUP == 1)
    reg32_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_INTR_EN_REG, LP_CTL_MCU_RB_DISPLAY_INTR_EN_OFFSET);
    reg32_clrbit(LP_CTL_MCU_RB_LP_CTL_MCU_DEF_15_REG, LP_CTL_MCU_RB_DISPLAY_INTR_INT_EN_OFFSET);
    uapi_lcd_set_mcuint2te_time(MCU_INT_TO_TE_TIME);
#endif

    osal_irq_clear(M_WAKEUP_IRQN);
}

void pm_wakeup_int_config(void)
{
    // enable all ipc int and gpio int and rtc
    // bit0 d2b int1; bit1 d2b int0; bit2 d2m int1; bit3 d2m int0
    // bit4 b2m int1; bit5 b2m int0; bit6 b2d int1; bit7 b2d int0
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_INTR_EN_REG, 0xF3C);
    writew(LP_CTL_MCU_RB_LP_CTL_MCU_INTR_EN_1_REG, 0x1);

    // wkup int enable
    osal_irq_request(M_WAKEUP_IRQN, (osal_irq_handler)pm_wakeup_handler, NULL, NULL, NULL);
    osal_irq_set_priority(M_WAKEUP_IRQN, irq_prio(M_WAKEUP_IRQN));
    osal_irq_enable(M_WAKEUP_IRQN);
}

static void pm_auto_ie_config(void)
{
    reg16_setbit(LP_CTL_MCU_RB_LP_CTL_MCU_MAN_FRC_3_REG, LP_CTL_MCU_RB_GPIO_ISO_EN_MCU_MAN_FRC_OFFSET);

    // all pin sleep ie disable
    writew(GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_0_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_1_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_AGPIO0_CTRL_IE_2_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_HGPIO0_CTRL_IE_0_REG, 0);

    writew(GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_0_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_MGPIO0_CTRL_IE_1_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_0_REG, 0);
    writew(GLB_CTL_M_RB_CFG_S_EGPIO0_CTRL_IE_1_REG, 0);

    // uart l0 support wakeup
    uapi_pin_set_slp_ie(S_MGPIO18, PIN_IE_ENABLE);
    // DSI TE
    uapi_pin_set_slp_ie(S_AGPIO19, PIN_IE_ENABLE);
}

void pm_auto_pg_config(void)
{
    // disable efuse auto load
    reg16_clrbit(ULP_FUSE_CTL_RB_ULP_EFUSE_AUTO_RD_EN_REG, ULP_FUSE_CTL_RB_EFUSE_AUTO_RD_EN_OFFSET);

    reg16_clrbit(ULP_AON_FUSE_CTL_RB_EFUSE_AUTO_RD_EN_REG, ULP_AON_FUSE_CTL_RB_AON_SUB_EFUSE0_AUTO_RD_EN_OFFSET);
    reg16_clrbit(ULP_AON_FUSE_CTL_RB_EFUSE_AUTO_RD_EN_REG, ULP_AON_FUSE_CTL_RB_AON_SUB_EFUSE1_AUTO_RD_EN_OFFSET);
    reg16_clrbit(ULP_AON_FUSE_CTL_RB_EFUSE_AUTO_RD_EN_REG, ULP_AON_FUSE_CTL_RB_AON_SUB_EFUSE0_SEC_AUTO_RD_EN_OFFSET);
    reg16_clrbit(ULP_AON_FUSE_CTL_RB_EFUSE_AUTO_RD_EN_REG, ULP_AON_FUSE_CTL_RB_AON_SUB_EFUSE1_SEC_AUTO_RD_EN_OFFSET);

#ifdef SUPPORT_DISPLAY_TECH
    uint8_t state = PMU_MEM_SLEEP_RETENTION;
#else
    uint8_t state = PMU_MEM_SLEEP_SHUT_DOWN;
#endif
    uapi_pmu_mem_control(PMU_MEM_DISPLAY_L2M_RAM, state);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_0_RAM, state);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_1_RAM, state);
    uapi_pmu_mem_control(PMU_MEM_NPU_L2M_2_RAM, state);
    uapi_pmu_mem_control(PMU_MEM_GMMU_LUT_L2M_RAM, state);
    uapi_pmu_mem_control(PMU_MEM_COM_RAM, PMU_MEM_SLEEP_SHUT_DOWN);

    pm_auto_ie_config();

    // ulp aon auto cg
    reg16_clrbit(GLB_CTL_M_RB_ULP_AON_AUTO_CG_REG, GLB_CTL_M_RB_ULP_AON_AUTO_CG_BYPASS_OFFSET);

    // aon auto cg
    reg16_clrbit(PMU2_CMU_CTL_RB_AON_BUS_IP_CLKEN_REG, PMU2_CMU_CTL_RB_AON_AUTO_CG_BYPASS_EN_OFFSET);
    reg16_clrbit(PMU2_CMU_CTL_RB_AON_BUS_IP_CLKEN_REG, PMU2_CMU_CTL_RB_AON_AHB_HREADY_MUX_OFFSET);
}

uint16_t pm_port_get_sleep_event_status(void)
{
    return reg32_getbit(LP_CTL_MCU_RB_LP_CTL_MCU_STS_2_REG, LP_CTL_MCU_RB_MCPU_SLEEPING_STS_OFFSET);
}

uint16_t pm_port_get_wakeup_event_status(void)
{
    return readw(LP_CTL_MCU_RB_LP_CTL_MCU_WKUP_STS_REG);
}