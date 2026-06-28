/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2023. All rights reserved.
 *
 * Description: Provides V151 HAL watchdog \n
 *
 * History: \n
 * 2022-11-26, Create file. \n
 */
#include <stdint.h>
#include "securec.h"
#include "common_def.h"
#include "tcxo.h"
#include "hal_watchdog.h"
#include "hal_watchdog_v151_regs_op.h"
#include "hal_watchdog_v151.h"

#define LOAD_RESEV          8
#define HAL_WDT_RESTART_KEY 0x5A5A5A5A
#define HAL_WDT_LOCK_KEY    0x5A5A5A5A
#define MS_PER_SECONDS   1000
#define TOP_MIN_CLKS     0xFF
#define TOP_MAX_CLKS     0xFFFFFFFF

// max wait 2000us
#define WAIT_CCVR_LOCK_DELAY_US 1
#define WAIT_CCVR_LOCK_MAX_CNT  2000

#define DISABLE 0
#define ENABLE  1

STATIC uint32_t g_timeout = 0;
STATIC hal_watchdog_callback_t g_hal_watchdog_callback = NULL;

STATIC void hal_watchdog_v151_clear_interrupt(void)
{
    (void)hal_watchdog_wdt_eoi_set_wdt_eoi(ENABLE);
}

#pragma weak hal_watchdog_kick = hal_watchdog_v151_kick
void hal_watchdog_v151_kick(void)
{
    hal_watchdog_wdt_restart_set(HAL_WDT_RESTART_KEY);
}

#pragma weak hal_watchdog_disable = hal_watchdog_v151_disable
void hal_watchdog_v151_disable(void)
{
    hal_watchdog_v151_clear_interrupt();
    hal_watchdog_v151_wdt_cr_set_wdt_en(DISABLE);
}

#pragma weak hal_watchdog_init = hal_watchdog_v151_init
errcode_t hal_watchdog_v151_init(void)
{
    errcode_t ret = hal_watchdog_regs_init();
    return ret;
}

#pragma weak hal_watchdog_deinit = hal_watchdog_v151_deinit
void hal_watchdog_v151_deinit(void)
{
    hal_watchdog_regs_deinit();
}


#pragma weak hal_watchdog_set_attr = hal_watchdog_v151_set_attr
errcode_t hal_watchdog_v151_set_attr(uint32_t timeout)
{
    uint32_t timeout_s = (timeout * watchdog_port_get_clock());
    if (((timeout == 0) || (timeout_s / timeout == watchdog_port_get_clock())) == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    if ((timeout_s < TOP_MIN_CLKS) || (timeout_s > TOP_MAX_CLKS)) {
        return ERRCODE_INVALID_PARAM;
    }

    g_timeout = timeout;

    /* Obtaining the timeout that can be written to the register */
    timeout_s = timeout_s >> LOAD_RESEV;

    /* Unlock the register. */
    hal_watchdog_wdt_lock_set(HAL_WDT_LOCK_KEY);

    /* Be sure the WATCHDOG is disabled before configuring it. */
    hal_watchdog_v151_disable();

    /* wdt_cr initial value setting. */
    hal_watchdog_wdt_cr_set_rst_en(ENABLE);

    hal_watchdog_wdt_cr_set_wdt_imsk(DISABLE);

    hal_watchdog_wdt_cr_set_rst_pl(CONFIG_WATCHDOG_USING_V151_RST_PL);

    /* Set wdt timeout range(From 0xFF to 0xFFFFFFFF). */
    hal_watchdog_wdt_load_set_wdt_load(timeout_s);

    /* Restart register */
    hal_watchdog_wdt_restart_set(HAL_WDT_RESTART_KEY);

    return ERRCODE_SUCC;
}

#pragma weak hal_watchdog_get_attr = hal_watchdog_v151_get_attr
uint32_t hal_watchdog_v151_get_attr(void)
{
    return g_timeout;
}

#pragma weak hal_watchdog_enable = hal_watchdog_v151_enable
void hal_watchdog_v151_enable(hal_wdt_mode_t mode)
{
    /* Be sure that the WATCHDOG is disabled before configuring it. */
    hal_watchdog_v151_disable();

    /* Set wdt mode */
    if (mode == HAL_WDT_MODE_RESET) {
        hal_watchdog_wdt_cr_set_wdt_mode(WDT_MODE_RESET_BY_FIRST_INTR);
    } else if (mode == HAL_WDT_MODE_INTERRUPT) {
        hal_watchdog_wdt_cr_set_wdt_mode(WDT_MODE_RESET_BY_SECOND_INTR);
    }

    hal_watchdog_v151_clear_interrupt();

    /* Enable watchdog. */
    hal_watchdog_v151_wdt_cr_set_wdt_en(ENABLE);

    hal_watchdog_v151_kick();
}

// 连续两次调用hal_watchdog_v151_get_left_time之间，必须等待，
// 以使得hal_watchdog_wdt_cnt_get读取后，hal_watchdog_wdt_ccvr_en_set_wdt_ccvr_req设置可以被捕获，
// 建议等待时间大于2倍32K时钟周期（约61us）。
#pragma weak hal_watchdog_get_left_time = hal_watchdog_v151_get_left_time
uint32_t hal_watchdog_v151_get_left_time(void)
{
    uint32_t timeout_cnt = 0;

    /* Initiate a request to view time. */
    hal_watchdog_wdt_ccvr_en_set_wdt_ccvr_req(ENABLE);

    while (hal_watchdog_wdt_ccvr_en_get_wdt_ccvr_lock() == 0) {
        if (timeout_cnt >= WAIT_CCVR_LOCK_MAX_CNT) {
            return 0;
        }
        uapi_tcxo_delay_us(WAIT_CCVR_LOCK_DELAY_US);
        timeout_cnt++;
    }

    /* Returns the time remaining in seconds(s). */
    return (uint32_t)(((uint64_t)hal_watchdog_wdt_cnt_get() * MS_PER_SECONDS) / watchdog_port_get_clock());
}

#pragma weak hal_register_watchdog_callback = hal_register_watchdog_v151_callback
void hal_register_watchdog_v151_callback(hal_watchdog_callback_t callback)
{
    g_hal_watchdog_callback = callback;
}

void hal_watchdog_v151_irq_handler(uintptr_t param)
{
    if (g_hal_watchdog_callback != NULL) {
        g_hal_watchdog_callback(param);
    }

    hal_watchdog_v151_clear_interrupt();
}
