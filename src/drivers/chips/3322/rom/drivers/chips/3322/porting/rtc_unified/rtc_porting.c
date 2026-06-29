/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provide rtc port \n
 *
 * History: \n
 * 2023-03-02, Create file. \n
 */

#include "soc_osal.h"
#include "common_def.h"
#include "hal_rtc_v150.h"
#include "chip_core_irq.h"
#include "rtc_porting.h"

uintptr_t g_rtc_base_addr[RTC_MAX_NUM] = {
    RTC_0_BASE_ADDR,
};

rtc_delay_params g_rtc_delay_ctl;

void rtc_porting_delay_set(rtc_delay_ctl_t para, bool val)
{
    if (para >= RTC_DELAY_BUTT) {
        return;
    }
    switch (para) {
        case RTC_STOP_DELAY_EN:
            g_rtc_delay_ctl.rtc_stop_delay_en = val;
            break;
        case RTC_START_DELAY_EN:
            g_rtc_delay_ctl.rtc_start_delay_en = val;
            break;
        default:
            return;
    }
    return;
}

void rtc_porting_delay_init(void)
{
    g_rtc_delay_ctl.rtc_stop_delay_en = 0x1;
    g_rtc_delay_ctl.rtc_start_delay_en = 0x0;
}

uintptr_t rtc_porting_comm_addr_get(void)
{
    return GLB_RTC_BASE_ADDR;
}

uintptr_t rtc_porting_base_addr_get(rtc_index_t index)
{
    return g_rtc_base_addr[index];
}

int rtc0_irq_handler(int i, void *p)
{
    unused(i);
    unused(p);
    hal_rtc_v150_irq_handler(RTC_0);
    osal_irq_clear(RTC_0_IRQN);
    return 0;
}

osal_irq_handler rtc_irq_handler[RTC_MAX_NUM] = {
    rtc0_irq_handler,
};

void rtc_port_register_irq(rtc_index_t index, uint32_t id, uint16_t priority)
{
    osal_irq_disable(id);
    osal_irq_request(id, rtc_irq_handler[index], NULL, NULL, NULL);
    osal_irq_set_priority(id, priority);
    osal_irq_enable(id);
}

void rtc_port_unregister_irq(rtc_index_t index, uint32_t id)
{
    unused(index);
    osal_irq_disable(id);
    osal_irq_free(id, NULL);
}

uint32_t g_rtc_clock_value = CONFIG_RTC_CLOCK_VALUE;

void rtc_porting_clock_value_update(uint32_t clock)
{
    g_rtc_clock_value = clock;
}

uint32_t rtc_porting_clock_value_get(void)
{
    return g_rtc_clock_value;
}

uint64_t rtc_porting_ms_2_cycle(uint32_t ms)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return ((uint64_t)ms * clock_value / MS_PER_S);
}

uint64_t rtc_hw_porting_ms_2_cycle(uint64_t ms)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return (ms * (uint64_t)clock_value / MS_PER_S);
}

uint32_t rtc_porting_cycle_2_us(uint64_t cycle)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    return (uint32_t)((cycle * (MS_PER_S * US_PER_MS)) / (clock_value));
}

uint64_t rtc_porting_compensat_by_tcxo(uint64_t diff)
{
    uint32_t clock_value = rtc_porting_clock_value_get();
    uint64_t rtc_compensat_cycle_tcxo = HAL_TCXO_TICKS_PER_SECOND / clock_value;

    if (diff < rtc_compensat_cycle_tcxo) {
        return 0;
    } else if (diff < rtc_compensat_cycle_tcxo * RTC_2_CYCLE) {
        return RTC_1_CYCLE;
    } else if (diff < rtc_compensat_cycle_tcxo * RTC_3_CYCLE) {
        return RTC_2_CYCLE;
    } else {
        return diff / rtc_compensat_cycle_tcxo;
    }
}
