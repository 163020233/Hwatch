/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#include "stdint.h"
#include "stdio.h"
#include "ux_api_fwk.h"
#include "watch_bg_response_bg.h"
#include "ux_event_define.h"
#include "watch_bg_response_power.h"
#include "watch_bg_response_master.h"
#include "ux_module_init.h"


void ux_watch_bt_activity_init(void)
{
    /* 整机事件订阅 */
    ux_api_fwk_module_register(UX_EVENT_TYPE_MASTER, bg_process_master_msg);
    ux_api_fwk_module_register(UX_EVENT_TYPE_BG, bg_process_bg_msg);
    ux_api_fwk_module_register(UX_EVENT_TYPE_POWER, bg_process_power_msg);
}

UX_MODULE_REGISTER(ux_watch_bt_activity_init);