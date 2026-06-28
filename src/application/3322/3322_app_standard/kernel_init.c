/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: Application core kernel init function for standard
 * Author:
 * Create:
 */

#include "kernel_init.h"
#include "stdint.h"
#include "chip_io.h"
#include "securec.h"
#include "panic.h"
#include "pmp_config.h"
#include "los_task_pri.h"
#include "los_printf_pri.h"
#include "arch/exception.h"
#include "arch/canary.h"
#include "exception.h"
#include "cmsis_os2.h"
#include "soc_osal.h"
#include "vectors.h"
#include "debug_print.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "pm_pmu.h"
#include "pinctrl_porting.h"
#include "psram_v2.h"
#include "psram_porting.h"
#if defined(HI3322_WSTP) && (HI3322_WSTP == 1)
#include "app_at_uart.h"
#endif

#ifdef CONFIG_AT_UART_BAUDRATE
#define COM_UART_BAUDRATE CONFIG_AT_UART_BAUDRATE
#else
#define COM_UART_BAUDRATE 115200
#endif

static void chip_watchdog_init(void)
{
    watchdog_turnoff_clk();
    watchdog_turnon_clk();
    uapi_watchdog_init(CHIP_WDT_TIMEOUT_32S);
    uapi_watchdog_enable(WDT_MODE_INTERRUPT);
}

#ifdef CONFIG_PSRAM_SUPPORT
static void copy_from_flash_to_psram(void)
{
    int32_t ret;
    uint32_t size = (uint32_t)(&__psram_data_end) - (uint32_t)(&__psram_data_start);
    ret = memcpy_s((void *)((uint32_t)(&__psram_data_start)), size, (void *)(uint32_t)(&__psram_data_load), size);
    if (ret != ERRCODE_SUCC) {
        PRINT("psram data memcpy fail.\r\n");
    }

    size = (uint32_t)(&__psram_bss_end) - (uint32_t)(&__psram_bss_start);
    ret = memset_s((void *)((uint32_t)(&__psram_bss_start)), size, 0, size);
    if (ret != ERRCODE_SUCC) {
        PRINT("psram bss memset fail.\r\n");
    }
}

#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
void get_fs_pool_info(uint32_t *pool, uint32_t *size)
{
    *pool = uapi_sys_get_memory_pool_addr(MEM_POOL_FS);
    *size = uapi_sys_get_memory_pool_length(MEM_POOL_FS);
    return;
}
#endif
#endif

static void copy_flash_to_psram(void)
{
#ifdef CONFIG_PSRAM_SUPPORT
    copy_from_flash_to_psram();
#ifdef CONFIG_SUPPORT_FS_INDEPENDENT_MEMORY_POOL
    OS_FSMemPoolHookReg(get_fs_pool_info);
#endif
#endif
}

void kernel_init(void)
{
    chip_watchdog_init();
    copy_flash_to_psram();
    reg16_setbit(COM_CTL_RB_COM_UART_REG, COM_CTL_RB_COM_UART_CLKEN_OFFSET); // open com uart clock en
    reg16_setbit(COM_CTL_RB_COM_UART_REG, COM_CTL_RB_COM_UART_SOFT_RST_N_OFFSET); // rst
#ifndef CFG_DRIVERS_USB_AS_SERIAL
    sw_debug_uart_init(COM_UART_BAUDRATE);
#endif
#if defined(HI3322_WSTP) && (HI3322_WSTP == 1)
    at_uart_set_bus_num(AT_UART_BUS); // 此处设置dut 公共组件at print输出bus号
#endif

    panic_init();

    /* enable icache and dcahce */
    pmp_init();
    OsCacheInit();

    /* os kernel adapt config */
    ArchSetExcHook((EXC_PROC_FUNC)do_fault_handler);
    ArchSetNMIHook((NMI_PROC_FUNC)do_hard_fault_handler);
    OsSetMainTask();
    OsCurrTaskSet((LosTaskCB *)OsGetMainTask());

    (void)osKernelInitialize();
}
