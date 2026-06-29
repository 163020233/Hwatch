/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
 * Description: RISCV70 idle task config for LiteOS
 */

#include "idle_config.h"
#include "core.h"
#include "chip_io.h"
#include "los_task.h"
#include "cmsis_extension.h"
#if ((USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG == NO) && defined(LIBLOG))
#include "log_oam_msg.h"
#endif
#include "watchdog.h"
#include "soc_osal.h"
#if (ENABLE_LOW_POWER == YES)
#include "pm_sleep.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if (ENABLE_LOW_POWER == YES)
bool g_pm_enable = true;
#else
bool g_pm_enable = false;
#endif

void idle_set_open_pm(bool en)
{
    g_pm_enable = en;
}

static void idle_task_process(void)
{
    uapi_watchdog_kick();

    log_oam_prase_message();

#if (ENABLE_LOW_POWER == YES)
    if (g_pm_enable) {
        uapi_pm_enter_sleep();
    }
#endif
}

void idle_task_config(void)
{
    osal_idle_handler_hook_reg(idle_task_process);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
