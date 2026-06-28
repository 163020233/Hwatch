/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides pm dfx port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-08-09， Create file. \n
 */

#include "chip_io.h"
#include "soc_osal.h"
#include "pm_dfx_porting.h"

static uint16_t g_wakeup_src[WAKEUP_SOURCE_MAX] = {0};

#define WAKUP_SRC_STRLEN  12

static const char g_shell_wakup_src_name[][WAKUP_SRC_STRLEN] = {
#if CORE == APPS
    "pwr key dis",
    "ulp gpio",
    "aon pwm",
    "npu",
    "ssi",
    "rtc",
    "uart l0",
    "ipc",
    "uart h0",
    "gpio",
    "dap",
    "dap level",
    "dsp wdt",
    "cwdt",
    "bt wdt",
    "audio",
    "usb valid",
    "vbus",
    "vbus dis",
    "alarm",
    "pwr key",
    "display",
    "te",
#endif
#if CORE == BT
    "osc en",
    "ssi",
    "rtc",
    "ipc",
    "gpio",
    "dap",
    "dap level",
    "coex uart"
#endif
};

void pm_dfx_wakeup_src_statistics(uint32_t interrupt_id)
{
    for (uint16_t bit_count = 0; bit_count <= WAKEUP_SOURCE_COUNT; bit_count++) {
        if ((interrupt_id & BIT(bit_count)) != 0) {
            g_wakeup_src[bit_count]++;
            break;
        }
    }
}

void pm_dfx_ext_wakeup_src_statistics(wakeup_source_t wkup_src)
{
    if (wkup_src < WAKEUP_SOURCE_MAX) {
        g_wakeup_src[wkup_src]++;
    }
}

void pm_dfx_dump_wakeup_src_statistics(void)
{
    for (uint16_t bit_count = 0; bit_count <= WAKEUP_SOURCE_COUNT; bit_count++) {
        if (g_wakeup_src[bit_count] != 0) {
            osal_printk("wkup:%s cnt=%d\n", g_shell_wakup_src_name[bit_count], g_wakeup_src[bit_count]);
        }
    }
}

void pm_dfx_clear_wakeup_src_statistics(void)
{
    for (uint16_t bit_count = 0; bit_count <= WAKEUP_SOURCE_COUNT; bit_count++) {
        g_wakeup_src[bit_count] = 0;
    }
}