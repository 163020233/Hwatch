/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux event init \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-08， Create file. \n
 */

#ifndef DEMO_WATCH
#include "feature_config.h"
#include "ux_bs_filter.h"
#endif
#include "fsm_box.h"
#include "fsm_master.h"
#include "fsm_pairing.h"
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_SOUND_BOX)
#include "ux_led_process.h"
#endif
#ifdef CONFIG_SUPPORT_BUTTON
#include "ux_button_process.h"
#endif
#ifdef CONFIG_SUPPORT_BOX
#include "ux_box.h"
#endif
#include "ux_system_manager.h"
#include "ux_common_log.h"
#ifndef DEMO_WATCH
#include "ux_communication.h"
#include "ux_device_manager.h"
#endif
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
#include "master_process.h"
#include "pairing_process.h"
#include "ux_audio_manager_adapt.h"
#endif
#include "ux_bs_property.h"
#include "ux_fwk_sync_process.h"
#include "ux_fwk_init.h"

static void fsm_init_process(void)
{
    fsm_master_init();
    if (get_working_state() == STATUS_ON_DUT) {
        set_fsm_master_state(MASTER_DUT);
    }
}

void ux_framework_init(void)
{
#ifndef DEMO_WATCH
    ux_device_state_init();
    /* device_mgr初始化 */
    ux_device_mgr_init();
#endif

    /* frame 初始化 */
    ux_fwk_set_response_notify_task(UX_TASK_BUTT);

    /* 业务状态机 初始化 */
    fsm_init_process();

    /* 配对状态机 初始化 */
    fsm_pairing_init();

    /* 盒子状态机 初始化 */
    fsm_box_init();

#ifdef CONFIG_SUPPORT_BUTTON
    /* 按键组件 初始化 */
    if (ux_button_init() != ERRCODE_SUCC) {
        ux_log_err("[ux][button] init failed!\r\n");
    }
#endif

#ifdef CONFIG_SUPPORT_BOX
    /* 盒子组件 初始化 */
    ux_box_init();
#endif
#ifndef DEMO_WATCH
    /* 数传通道组件 初始化 */
    ux_commu_init();
#endif
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    /* 业务组件 初始化 */
    master_init_process();
    /* 配对组件 初始化 */
    pairing_init_process();
#endif

#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    ux_audio_service_init();
#endif

#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_DONGLE) || \
    defined(MELODY_DEMO_SOUNDBAR) || defined(MELODY_DEMO_SOUND_BOX)
    ux_bs_filter_init();
#endif
}
