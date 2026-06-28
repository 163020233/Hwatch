/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides tcxo driver source \n
 *
 * History: \n
 * 2022-08-16， Create file. \n
 */

#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "hal_tcxo.h"
#include "tcxo_porting.h"
#include "tcxo.h"

#define USECS_PER_MSEC 1000

bool g_tcxo_inited = false;

#if defined(CONFIG_TCXO_SUPPORT_LPM)
STATIC uint64_t g_tcxo_sleep_compensation_count = 0;
STATIC uint64_t g_tcxo_sleep_suspend_current_count = 0;
#endif

errcode_t uapi_tcxo_init(void)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(g_tcxo_inited)) {
        return ERRCODE_SUCC;
    }
    uint32_t irq_sts = osal_irq_lock();
    ret = hal_tcxo_init();
    if (ret == ERRCODE_SUCC) {
        g_tcxo_inited = true;
    }
    osal_irq_restore(irq_sts);
    return ret;
}

errcode_t uapi_tcxo_deinit(void)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(!g_tcxo_inited)) {
        return ERRCODE_SUCC;
    }
    uint32_t irq_sts = osal_irq_lock();
    ret = hal_tcxo_deinit();

    g_tcxo_inited = false;
    osal_irq_restore(irq_sts);
    return ret;
}

uint64_t uapi_tcxo_get_count(void)
{
    uint64_t count = 0;
    if (unlikely(!g_tcxo_inited)) {
        return count;
    }
    uint32_t irq_sts = osal_irq_lock();
    count = hal_tcxo_get();
    osal_irq_restore(irq_sts);

#if defined(CONFIG_TCXO_SUPPORT_LPM)
    return count + g_tcxo_sleep_compensation_count;
#else
    return count;
#endif
}

uint64_t uapi_tcxo_get_ms(void)
{
    return uapi_tcxo_get_count() / (tcxo_porting_ticks_per_usec_get() * USECS_PER_MSEC);
}

uint64_t uapi_tcxo_get_us(void)
{
#if defined(CONFIG_TCXO_UNUSED)
    return 0;
#else
    return uapi_tcxo_get_count() / tcxo_porting_ticks_per_usec_get();
#endif
}

STATIC errcode_t uapi_tcxo_delay_count(uint64_t ticks_delay)
{
    uint64_t target_ticks;
    uint64_t origin_ticks;

    origin_ticks = uapi_tcxo_get_count();
    target_ticks = origin_ticks + ticks_delay;

    while (uapi_tcxo_get_count() < target_ticks) {
    }

    return ERRCODE_SUCC;
}

errcode_t tcxo_delay(uint32_t delay, uint32_t count)
{
    if (unlikely(!g_tcxo_inited)) {
        return ERRCODE_FAIL;
    }
    uint64_t ticks_to_delay;
    ticks_to_delay = (uint64_t)delay * count * tcxo_porting_ticks_per_usec_get();
    return uapi_tcxo_delay_count(ticks_to_delay);
}

errcode_t uapi_tcxo_delay_ms(uint32_t m_delay)
{
    return tcxo_delay(m_delay, USECS_PER_MSEC);
}

errcode_t uapi_tcxo_delay_us(uint32_t u_delay)
{
    return tcxo_delay(u_delay, 1);
}

#if defined(CONFIG_TCXO_UNUSED)
void tcxo_delay_us(uint64_t delay)
{
    volatile uint64_t val = delay;
    volatile uint64_t count;

    while (--val > 0) {
        count = DELAY_COUNT_US;
        while (count-- > 0) {
            ;
        }
    }
}

void tcxo_delay_ms(uint64_t delay)
{
    volatile uint64_t val = delay;
    volatile uint64_t count;

    while (--val > 0) {
        count = DELAY_COUNT_MS;
        while (count-- > 0) {
            ;
        }
    }
}
#endif // #if defined(CONFIG_TCXO_UNUSED)

#if defined(CONFIG_TCXO_SUPPORT_LPM)
errcode_t uapi_tcxo_suspend(uintptr_t arg)
{
    g_tcxo_sleep_suspend_current_count = uapi_tcxo_get_count();
    if ((uint64_t *)arg != NULL) {
        *(uint64_t *)arg = g_tcxo_sleep_suspend_current_count;
    }
    return ERRCODE_SUCC;
}

errcode_t uapi_tcxo_resume(uintptr_t arg)
{
    uint64_t sleep_compensation_us = *(uint64_t *)arg;
    uint64_t sleep_compensation_count = sleep_compensation_us * tcxo_porting_ticks_per_usec_get();
    g_tcxo_sleep_compensation_count = g_tcxo_sleep_suspend_current_count + sleep_compensation_count;
    uapi_tcxo_deinit();
    return uapi_tcxo_init();
}
#endif  /* CONFIG_TCXO_SUPPORT_LPM */