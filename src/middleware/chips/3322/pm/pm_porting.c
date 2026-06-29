/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved. \n
 *
 * Description: Provides pm port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-01-13， Create file. \n
 */

#include "pm_sleep.h"
#include "pm_veto.h"
#include "pm_sleep_porting.h"
#include "pm_dev.h"
#include "pm.h"
#include "cpu_trace_unified.h"
#include "watchdog.h"
#include "pinctrl.h"
#include "uart.h"
#include "los_task_pri.h"
#include "debug_print.h"
#include "preserve.h"
#include "tcxo.h"
#include "product.h"

void uapi_pm_lpc_init(void)
{
    pm_sleep_funcs_t funcs = {
        .start_tickless            = pm_port_start_tickless,
        .stop_tickless             = pm_port_stop_tickless,
        .get_sleep_ms              = pm_port_get_sleep_ms,
        .start_wakeup_timer        = pm_port_start_wakeup_timer,
        .allow_deepsleep           = pm_port_allow_deepsleep,
        .lightsleep_config         = pm_port_lightsleep_config,
        .deepsleep_config          = pm_port_deepsleep_config,
        .light_wakeup_config       = pm_port_light_wakeup_config,
        .deep_wakeup_config        = pm_port_deep_wakeup_config,
        .enter_wfi                 = pm_port_enter_wfi,
#if defined(CONFIG_PM_POWER_GATING_ENABLE) && (CONFIG_PM_POWER_GATING_ENABLE == 1)
        .cpu_suspend               = pm_port_cpu_suspend,
        .cpu_resume                = pm_port_cpu_resume,
#endif
#if defined(CONFIG_PM_POWER_SHIPMODE) && (CONFIG_PM_POWER_SHIPMODE == 1)
        .enter_shipmode            = pm_port_enter_shipmode,
#endif
    };
    uint16_t wakeup_level = 0;
    // 获取ssb中根据XO起振时间计算档位
#if CORE == BT
    wakeup_level = readw(LP_CTL_BT_RB_CFG_LP_CTL_BT_GP_REG4_REG);
#endif
    uapi_pm_register_sleep_funcs(&funcs);

    uapi_pm_veto_init();

    pm_wakeup_wait_time_config(wakeup_level);
    pm_wakeup_mode_config();
    pm_sleep_wait_time_config();
    pm_wakeup_int_config();

    pm_auto_cg_config(true);
    pm_auto_pg_config();

    pm_register_dev_resume_interface();

    pm_register_clock_interface();

#if (ENABLE_LOW_POWER == NO) || defined(BUILD_RECOVERY_IMAGE)
    uapi_pm_add_sleep_veto(PM_ID_BSP);
#endif
}
