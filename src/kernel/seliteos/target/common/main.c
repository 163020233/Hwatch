/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: seliteos main function for SELiteOS.
 */

#include "los_typedef.h"
#include "los_memory.h"
#include "los_mem.h"
#include "arch/tes_csr_op.h"
#ifdef SW_UART_DEBUG
#include "uart_porting.h"
#include "los_printf.h"
#endif
#ifdef LOSCFG_SUPPORT_TA
#include "task.h"
#endif
#include "pmp_mgr.h"
#ifdef SELITEOS_KERNEL_TEST
#include "seliteos_test_init.h"
#endif
#include "chips_porting.h"

LITE_OS_SEC_RODATA_USRSPACE UINT32 g_sysMode = SYS_MODE_DEFAULT_KERNEL;

WEAK void tee_soc_init(void)
{
    set_tmescr();
}

WEAK void tee_pmp_init(void)
{
    return;
}

WEAK void tee_rapc_init(void)
{
    return;
}

WEAK void tee_tzasc_init(void)
{
    return;
}

WEAK void tee_mempool_init(void)
{
    (void)los_kernel_mem_init();
#ifdef LOSCFG_SUPPORT_TA
    (void)los_drvbox_mem_init();
#endif
}

WEAK void tee_driver_and_service_init(void)
{
#ifdef SW_UART_DEBUG
    /* init uart driver */
    sw_debug_uart_init(SW_UART_BAUDRATE);

    LOS_Printf("Welcome to SEliteOS!\r\n");
#endif
}

#ifdef LOSCFG_SUPPORT_TA
static void tee_ta_init(void)
{
    task_init();
    register_ta();
}
#endif

WEAK void tee_ready_to_ree(void)
{
    set_pmp_switch_tee_to_ree(NORMAL_TYPE);
}

int main(void)
{
    /* Init soc regs, including tes csrs. */
    tee_soc_init();

    /* Init pmp regions for tee ram and flash, enable CPU access isolation. */
    tee_pmp_init();

    /* Init rapc and tzpc, enable peripherals access isolation. */
    tee_rapc_init();

    /* init tee mempool */
    tee_mempool_init();

    /* init tee driver and service init */
    tee_driver_and_service_init();
#ifdef LOSCFG_SUPPORT_TA
    /* init task for U-M mode switching, and register TA */
    tee_ta_init();
#endif

#ifdef LOSCFG_SUPPORT_TZASC
    /* Init ddr tzasc. */
    tee_tzasc_init();
#endif

#ifdef SELITEOS_KERNEL_TEST
    /* init tee testcase */
    seliteos_testcase_init();
#endif
    /* be ready to jump to ree. */
    tee_ready_to_ree();

    return 0;
}
