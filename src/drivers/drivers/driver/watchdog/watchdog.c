
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides watchdog driver source \n
 *
 * History: \n
 * 2022-07-27, Create file. \n
 */

#include <stdio.h>
#include <stdbool.h>
#include "securec.h"
#include "soc_osal.h"
#include "errcode.h"
#include "watchdog_porting.h"
#include "hal_watchdog.h"
#include "watchdog.h"

STATIC bool g_watchdog_inited = false;
STATIC bool g_watchdog_enabled = false;
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
STATIC wdt_mode_t g_wdt_mode = WDT_MODE_INTERRUPT;
STATIC uint32_t g_wdt_timeout = 0;
#endif
__attribute__((weak)) void watchdog_turnon_clk(void) {}

__attribute__((weak)) void watchdog_turnoff_clk(void) {}

errcode_t uapi_watchdog_init(uint32_t timeout)
{
    unused(timeout);

    errcode_t ret = ERRCODE_FAIL;
    watchdog_turnon_clk();
    watchdog_port_register_irq();

    unsigned int irq_sts = osal_irq_lock();
    ret = hal_watchdog_init();
    if (ret != ERRCODE_SUCC) {
        goto end;
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_timeout = timeout;
#endif
#if !defined(CONFIG_WATCHDOG_ALREADY_START)
    ret = hal_watchdog_set_attr(timeout);
#endif  /* CONFIG_WATCHDOG_ALREADY_START */
    g_watchdog_inited = true;

end:
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_watchdog_deinit(void)
{
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_SUCC;
    }

    if (unlikely(g_watchdog_enabled != 0)) {
        uapi_watchdog_disable();
    }

    watchdog_port_unregister_hal_funcs();
    watchdog_turnoff_clk();
    g_watchdog_inited = false;

    return ERRCODE_SUCC;
}

#if !defined(CONFIG_WATCHDOG_ALREADY_START)
errcode_t uapi_watchdog_enable(wdt_mode_t mode)
{
    if (unlikely((g_watchdog_inited == 0) || (mode >= WDT_MODE_MAX))) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_mode = mode;
#endif
    hal_watchdog_enable((hal_wdt_mode_t)mode);
    g_watchdog_enabled = true;
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}
#else
errcode_t uapi_watchdog_enable(wdt_mode_t mode)
{
    if (unlikely((g_watchdog_inited == 0) || (mode >= WDT_MODE_MAX))) {
        return ERRCODE_FAIL;
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_mode = mode;
#endif
    g_watchdog_enabled = true;

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_WATCHDOG_ALREADY_START */

errcode_t uapi_watchdog_disable(void)
{
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
    hal_watchdog_disable();
    g_watchdog_enabled = false;
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_set_time(uint32_t timeout)
{
    errcode_t ret = ERRCODE_FAIL;

    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_FAIL;
    }

    if (unlikely(g_watchdog_enabled != 0)) {
        uapi_watchdog_disable();
    }
#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
    g_wdt_timeout = timeout;
#endif
    unsigned int irq_sts = osal_irq_lock();
    ret = hal_watchdog_set_attr(timeout);
    osal_irq_restore(irq_sts);

    return ret;
}

errcode_t uapi_watchdog_get_left_time(uint32_t *timeout)
{
    if (unlikely(g_watchdog_enabled == 0) || unlikely(timeout == NULL)) {
        return ERRCODE_FAIL;
    }

    *timeout = hal_watchdog_get_left_time();
    if (unlikely(*timeout == 0)) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_kick(void)
{
    if (unlikely(g_watchdog_enabled == 0)) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
#if defined(CONFIG_WATCHDOG_SUPPORT_ULP_WDT) && (CONFIG_WATCHDOG_SUPPORT_ULP_WDT == 1)
    ulp_wdt_kick();
#endif
    hal_watchdog_kick();
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

errcode_t uapi_register_watchdog_callback(watchdog_callback_t callback)
{
    if (unlikely((g_watchdog_inited == 0) || (callback == NULL))) {
        return ERRCODE_FAIL;
    }

    unsigned int irq_sts = osal_irq_lock();
    hal_register_watchdog_callback(callback);
    osal_irq_restore(irq_sts);

    return ERRCODE_SUCC;
}

#ifdef CONFIG_WATCHDOG_SUPPORT_LPM
errcode_t uapi_watchdog_suspend(uintptr_t arg)
{
    unused(arg);
    return ERRCODE_SUCC;
}

errcode_t uapi_watchdog_resume(uintptr_t arg)
{
    errcode_t ret = ERRCODE_FAIL;
    unused(arg);
    if (unlikely(g_watchdog_inited == 0)) {
        return ERRCODE_SUCC;
    }
    ret = hal_watchdog_set_attr(g_wdt_timeout);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    hal_watchdog_enable((hal_wdt_mode_t)g_wdt_mode);
    return ERRCODE_SUCC;
}
#endif
