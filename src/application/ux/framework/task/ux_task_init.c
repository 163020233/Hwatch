/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: ux task init src code.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-03-15, Create file. \n
 */
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#include "stdbool.h"
#include "osal_wait.h"
#include "ux_fwk_msg.h"
#include "ux_cmd_manager.h"
#include "ux_common_log.h"
#include "ux_cmd_audio.h"
#include "ux_cmd_bs.h"
#include "ux_cmd_usb.h"
#include "ux_cmd_system.h"
#include "ux_cmd_recovery.h"
#include "ux_system_manager.h"
#if defined(CONFIG_SUPPORT_USB_DEVICE)
#include "ux_usb.h"
#endif
#if defined(CONFIG_SUPPORT_CAPSENSOR)
#include "ux_sensor_process.h"
#endif
#include "ux_fwk_msg.h"
#include "ux_fwk_init.h"
#include "ux_audio_manager.h"
#include "ux_activity_manager.h"
#include "ux_fwk_init.h"
#include "ux_audio_manager.h"
#include "ux_activity_manager.h"
#include "ux_powermgr.h"
#include "ux_fwk_sync_event.h"
#include "ux_hal_memory.h"
#include "ux_power.h"
#include "ux_module_init.h"
#include "ux_task_init.h"

void ux_fwk_feature_load(void)
{
#ifndef DEMO_WATCH
    UX_FEATURE_LOAD();
    ux_log_info("[ux] feature loaded\r\n");
#endif
}

void ux_api_fwk_system_init(void)
{
    ux_hal_memory_init();
    ux_msg_queue_init();
    ux_fwk_feature_load();
    return;
}

void ux_api_fwk_system_deinit(void)
{
    ux_hal_memory_deinit();
    return;
}

int ux_api_fwk_task(void *data)
{
    unused(data);
    ux_cmd_msg_t cmd_msg;

    ux_framework_init();
#ifndef DEMO_WATCH
    /* ux各模块初始化在此处添加 */
    ux_system_info_restore();
    ux_audio_manager_init();
#endif

#ifndef DEMO_WATCH
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    ux_cmd_audio_register();
#endif
    ux_cmd_usb_register();
    ux_cmd_recovery_register();
    ux_cmd_system_register();
#endif
#if defined(CONFIG_SUPPORT_USB_DEVICE)
    ux_usb_init();
#endif
#if defined(CONFIG_SUPPORT_CAPSENSOR)
    ux_sensor_init();
#endif
    ux_activity_init();
    ux_fwk_sync_event_init();
    ux_power_init();
#ifndef DEMO_WATCH
    ux_powermgr_init();
#endif
    while (true) {
        if (sys_get_msg(SYS_MODULE_APP_UX, &cmd_msg, OSAL_WAIT_FOREVER) != ERRCODE_SUCC) {
            continue;
        }

        if (ux_msg_handle(&cmd_msg) != ERRCODE_SUCC) {
            ux_log_err("[ux][ux_task]event %d handle failed\n", cmd_msg.mail.comm.event);
        }
#ifdef UT_TEST
        break;
#endif
    }
    return 0;
}
