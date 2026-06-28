/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides systick porting template \n
 *
 * History: \n
 * 2022-08-01， Create file. \n
 */
#include "systick_porting.h"
#include "systick.h"
#include "hal_systick_v150.h"
#include "chip_io.h"
#include "osal_interrupt.h"
#include "tcxo.h"
#include "clock_calibration.h"

#define ULP_SYSTICK_COUNT_CLEAR_WAIT_TIMEOUT 0xFFFF

static uintptr_t g_systick_base_addr = (uintptr_t)SYSTICK_BASE_ADDR;

uintptr_t systick_porting_base_addr_get(void)
{
    return g_systick_base_addr;
}

void systick_port_cali_xclk(void)
{
}

#if defined(CONFIG_ENABLE_RC_CALIBRATION)
uint64_t convert_s_2_count(uint64_t s)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((s) * rtc_clock);
}

uint64_t convert_count_2_s(uint64_t count)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((count) / rtc_clock);
}

uint64_t convert_ms_2_count(uint64_t ms)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((ms) * rtc_clock / MS_PER_S);
}

uint64_t convert_count_2_ms(uint64_t count)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((count) * MS_PER_S / rtc_clock);
}

uint64_t convert_us_2_count(uint64_t us)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((us) * rtc_clock / (MS_PER_S * US_PER_MS));
}

uint64_t convert_count_2_us(uint64_t count)
{
    uint32_t rtc_clock = CLOCK_32K_CALI_FREQ;
    return ((count) * (MS_PER_S * US_PER_MS) / rtc_clock);
}
#endif

errcode_t uapi_ulp_systick_count_clear(void)
{
    uint32_t timeout = 0;
    uint32_t irq_sts = osal_irq_lock();

    reg16_setbit(LP_CTL_ULP_B_RB_SYS_TICK_CFG_ULP_REG, LP_CTL_ULP_B_RB_SYS_TICK_STZ_ULP_OFFSET);
    while (timeout < ULP_SYSTICK_COUNT_CLEAR_WAIT_TIMEOUT) {
        if (reg16_getbit(LP_CTL_ULP_B_RB_SYS_TICK_CFG_ULP_REG, LP_CTL_ULP_B_RB_SYS_TICK_CLR_ULP_VALID_OFFSET) == 1) {
            osal_irq_restore(irq_sts);
            return ERRCODE_SUCC;
        }
        timeout++;
    }

    osal_irq_restore(irq_sts);
    return ERRCODE_SYSTICK_NOT_CLEARED;
}

uint64_t uapi_ulp_systick_get_count(void)
{
#define ULP_SYSITCK_GET_DELAY_US    60UL
    uint64_t count = 0;
    uint32_t irq_sts = osal_irq_lock();

    reg16_setbit(LP_CTL_ULP_B_RB_SYS_TICK_CFG_ULP_REG, LP_CTL_ULP_B_RB_SYS_TICK_FRZ_ULP_OFFSET);

    uapi_tcxo_delay_us(ULP_SYSITCK_GET_DELAY_US);

    count = (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_3_REG) << 48U;
    count |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_2_REG) << 32U;
    count |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_1_REG) << 16U;
    count |= (uint64_t)readw(LP_CTL_ULP_B_RB_CFG_SYS_TICK_VALUE_ULP_0_REG);

    reg16_clrbit(LP_CTL_ULP_B_RB_SYS_TICK_CFG_ULP_REG, LP_CTL_ULP_B_RB_SYS_TICK_FRZ_ULP_OFFSET);

    osal_irq_restore(irq_sts);

    return count;
}

#ifdef CONFIG_ENABLE_RC_CALIBRATION
volatile uint64_t g_pre_ulp_systick_count = 0;
volatile uint64_t g_pre_ulp_systick_us = 0;
uint64_t ulp_systick_get_time_us(void)
{
    uint32_t irq_sts = osal_irq_lock();
    uint64_t now_count = uapi_ulp_systick_get_count();
    uint64_t cost_us = ((now_count - g_pre_ulp_systick_count) * (US_PER_MS * MS_PER_S)) / CLOCK_32K_CALI_FREQ;
    g_pre_ulp_systick_us += cost_us;
    g_pre_ulp_systick_count = now_count;
    osal_irq_restore(irq_sts);
    return g_pre_ulp_systick_us;
}
#endif

uint64_t uapi_ulp_systick_get_ms(void)
{
#ifdef CONFIG_ENABLE_RC_CALIBRATION
    return ulp_systick_get_time_us() / US_PER_MS;
#endif
    uint64_t count = uapi_ulp_systick_get_count();
    return (count) * MS_PER_S / CLOCK_32K_CALI_FREQ;
}

uint64_t uapi_ulp_systick_get_us(void)
{
#ifdef CONFIG_ENABLE_RC_CALIBRATION
    return ulp_systick_get_time_us();
#endif
    uint64_t count = uapi_ulp_systick_get_count();
    return (count) * MS_PER_S * US_PER_MS / CLOCK_32K_CALI_FREQ;
}

#ifdef CONFIG_ENABLE_RC_CALIBRATION
volatile uint64_t g_pre_systick_count = 0;
volatile uint64_t g_pre_systick_us = 0;
uint64_t systick_get_time_us(void)
{
    uint32_t irq_sts = osal_irq_lock();
    uint64_t now_count = uapi_systick_get_count();
    uint64_t cost_us = ((now_count - g_pre_systick_count) * (US_PER_MS * MS_PER_S)) / CLOCK_32K_CALI_FREQ;
    g_pre_systick_us += cost_us;
    g_pre_systick_count = now_count;
    osal_irq_restore(irq_sts);
    return g_pre_systick_us;
}
#endif

void systick_cail_update(void)
{
#ifdef CONFIG_ENABLE_RC_CALIBRATION
    ulp_systick_get_time_us();
    systick_get_time_us();
#endif
}
