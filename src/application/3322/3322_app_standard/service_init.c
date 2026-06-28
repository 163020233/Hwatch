/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Application core board init function for standard
 * Author:
 * Create:
 */

#include <stdio.h>
#include "partition.h"
#include "pm.h"
#include "cpu_utils.h"
#include "log_common.h"
#include "log_uart.h"
#include "exception.h"
#include "log_oml_exception.h"
#ifdef HSO_SUPPORT
#include "dfx_system_init.h"
#endif
#include "chip_io.h"
#include "log_file.h"
#include "tcxo.h"
#ifdef AT_COMMAND
#include "at_adapter.h"
#endif
#include "pmu_sub_control.h"
#ifdef SUPPORT_CRASH_DATA_RAM
#include "crash_data.h"
#endif
#include "calendar.h"
#include "partition.h"
#ifdef CONFIG_PRODUCT_EVB_DITING
#include "modem_service_msg.h"
#include "tiot_service_interface.h"
#include "pinctrl.h"
#include "gpio.h"
#include "pmu_ulp.h"
#endif
#include "preserve.h"
#include "massdata_buffer.h"
#ifdef CONFIG_OTA_UPDATE_SUPPORT
#include "upg.h"
#include "soc_osal.h"
#include "dfx_feature_config.h"

#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
#include "log_limit.h"

#define CORE_ID_OFFSET 13

#define MAX_LOGCNT_APP              5000
#define MAX_LOGCNT_PLT              2000
#define MAX_LOGCNT_MEDIA            1000
#define MAX_LOGCNT_AUDIO            1000
#define MAX_LOGCNT_BTH              2000
#define MAX_LOGCNT_BGH              1000
#define MAX_LOGCNT_GUI              1000
#define MAX_LOGCNT_OHOS             1000
#define MAX_LOGCNT_DSP              1000

static log_limit_t g_log_limits[] = {
    {LOG_PFMODULE | (CORE << CORE_ID_OFFSET),       true, 1, MAX_LOGCNT_PLT, MAX_LOGCNT_PLT, 0, 0, 0, 0},
    {LOG_BTHMODULE | (CORE << CORE_ID_OFFSET),      true, 2, MAX_LOGCNT_BTH, MAX_LOGCNT_BTH, 0, 0, 0, 0},
    {LOG_BGHMODULE | (CORE << CORE_ID_OFFSET),      true, 2, MAX_LOGCNT_BGH, MAX_LOGCNT_BGH, 0, 0, 0, 0},
    {LOG_APPMODULE | (CORE << CORE_ID_OFFSET),      true, 4, MAX_LOGCNT_APP, MAX_LOGCNT_APP, 0, 0, 0, 0},
    {LOG_MEDIAMODULE | (CORE << CORE_ID_OFFSET),    true, 6, MAX_LOGCNT_MEDIA, MAX_LOGCNT_MEDIA, 0, 0, 0, 0},
    {LOG_DSPMODULE | (CORE << CORE_ID_OFFSET),      true, 7, MAX_LOGCNT_DSP, MAX_LOGCNT_DSP, 0, 0, 0, 0},
};
#endif

uint32_t ux_at_cmd_init(void);
errcode_t uapi_power_display_init(void);

static void upg_putc(const char c)
{
    PRINT("%c", c);
}

static void *upg_malloc(const uint32_t size)
{
    return osal_kmalloc(size, 0);
}

static void upg_free(void *ptr)
{
    return osal_kfree(ptr);
}
#endif

#ifdef CONFIG_PRODUCT_EVB_DITING
static void reset_cat1_module(void)
{
    int8_t reboot = 1;
    if (get_system_boot_status() == REBOOT_POWER_ON_TRIGGER_STATUS) {
        /* 上电场景，跳过cat1复位 */
        reboot = 0;
    }

    if (uapi_pmu_ulp_get_reboot_reason() != PMU_ULP_VBUS_POWER_UP) {
        /* 按键开机、按键重启等场景，进行cat1复位 */
        reboot = 1;
    }

    if (reboot) {
        uapi_pin_set_mode(S_AGPIO26, (pin_mode_t)HAL_PIO_FUNC_GPIO);
        uapi_gpio_set_dir(S_AGPIO26, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(S_AGPIO26, GPIO_LEVEL_LOW);
        uapi_tcxo_delay_us(200); /* 200us */
        uapi_gpio_set_val(S_AGPIO26, GPIO_LEVEL_HIGH);
        PRINT("cat1_reset succ." NEWLINE);
    }
}
#endif

void service_init(void)
{
    uapi_partition_init();

    /* dfx init step 1: log init */
    log_init();
    log_init_after_rtos();
    log_uart_init_after_rtos();

#if (defined(CONFIG_DFX_SUPPORT_LOG_LIMIT) && (CONFIG_DFX_SUPPORT_LOG_LIMIT == DFX_YES))
    uapi_log_limit_init(g_log_limits, sizeof(g_log_limits) / sizeof(log_limit_t));
    uapi_log_limit_set_enable(CORE, false);
#endif

    uapi_massdata_init();
#ifdef CONFIG_PRODUCT_EVB_DITING
    uapi_modem_service_init();
#endif
#ifdef AT_COMMAND
    at_adapter_init();
#ifdef DEMO_WATCH
#ifndef SUPPORT_AUTO_OTA
    ux_at_cmd_init();  /* UX AT CMD TABLE init */
#endif
#endif
#endif
    show_reboot_info();
    cpu_utils_init();
    hal_register_exception_dump_callback(log_exception_dump);
#ifdef SUPPORT_CRASH_DATA_RAM
    crash_data_register_reboot_handle(hal_exception_mcu_callback);
#endif
    /* dfx init step 2: dfx init */
#ifdef HSO_SUPPORT
    dfx_system_init();
#endif
#ifdef CONFIG_OTA_UPDATE_SUPPORT
    upg_func_t upg_func = {0};
    upg_func.malloc = upg_malloc;
    upg_func.free = upg_free;
    upg_func.serial_putc = upg_putc;
    (void)uapi_upg_init(&upg_func);
#endif
#ifdef SUPPORT_POWER_MANAGER
    uapi_power_display_init();
#endif
#ifdef CONFIG_PRODUCT_EVB_DITING
    reset_cat1_module();
    int32_t ret = tiot_service_init();
    if (ret != 0) {
        PRINT("tiot service init fail\n");
    }
#endif
}
