/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 * Description: Provides recovery functions.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-12-10, Create file. \n
 */
#include "securec.h"
#include "ux_common_log.h"
#include "fsm_master.h"
#include "ux_api_power.h"
#include "nv.h"
#ifdef SUPPORT_FEATURE_CONFIG
#include "feature_config.h"
#endif
#include "ux_config.h"
#include "ux_system_manager.h"
#include "ux_device_manager.h"
#include "ux_api_audio_recorder.h"
#ifndef DEMO_WATCH
#include "ux_api_led.h"
#include "ux_bs_sle.h"
#endif
#include "ux_api_box.h"
#include "ux_api_recovery.h"
#include "mmc_porting.h"
#if !defined(MELODY_DEMO_DONGLE) && !defined(MELODY_DEMO_SOUNDBAR)
#include "ux_api_charge.h"
#endif
#define MAX_RECOVERY_RETRY_TIMES 4
#define ENOERR 0
 
bool g_region_mode[KEY_ID_REGION_MAX_NUM] = {0};
bool g_need_format = false;

static void switch_recovery_state_on(void)
{
    set_fsm_master_state(MASTER_RECOVER);
    set_working_state(STATUS_ON_RECOVER);
}
 
static void switch_recovery_state_off(void)
{
    set_working_state(STATUS_ON_WORKING);
}
 
errcode_t ux_recovery_condition_check(void)
{
    /* 仅允许WORKING和DUT状态下进行恢厂 */
    working_state_t working_state = get_working_state();
    if (!(working_state == STATUS_ON_WORKING) && !(working_state == STATUS_ON_DUT)) {
        ux_log_err("[ux][recovery]not recover in working state: %d", working_state);
        return ERRCODE_FAIL;
    }
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_HA)
    ux_api_box_device_position_t position;
    ux_api_box_get_device_position(&position);
    if (position != UX_API_BOX_DEVICE_POSITION_IN) {
        ux_log_err("[ux][recovery]failed! earphone is out of the box !");
        return ERRCODE_FAIL;
    }
#endif
#if (!defined(MELODY_DEMO_DONGLE) && !defined(MELODY_DEMO_SOUNDBAR)) && !defined(DEMO_WATCH)
    /* 电池低于百分比阈值 */
    uint32_t battery = 0;
    ux_api_charger_get_battery_percentage(&battery);
    if (battery < RECOVERY_BATTERY_THRESHOLD) {     /* 电量低于阈值情况下，不允许升级 */
        ux_log_err("[ux][recovery] fail! battery is less than %d", RECOVERY_BATTERY_THRESHOLD);
        return ERRCODE_FAIL;
    }
#endif
#ifdef CONFIG_SUPPORT_AUDIO_RECORDER
    /* 停止录音 */
    uint16_t local_dev_id;
    errcode_t ret = ux_device_mgr_get_local_device_id(&local_dev_id);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][recovery]get local device error!");
        return ret;
    }
    ret = ux_api_audio_recorder_stop(local_dev_id);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][recovery] recorder stop fail!");
        return ERRCODE_FAIL;
    }
#endif
    return ERRCODE_SUCC;
}

errcode_t ux_recovery_process(void)
{
    switch_recovery_state_on(); /* 满足条件，将状态设置成恢厂状态 */
    nv_restore_mode_t restore_mode;
    (void)memcpy_s(restore_mode.region_mode, sizeof(restore_mode.region_mode),
        g_region_mode, sizeof(bool) * KEY_ID_REGION_MAX_NUM);
    errcode_t ret = uapi_nv_set_restore_mode_partitial(&restore_mode);
    if (ret != ERRCODE_SUCC) { /* 如果恢厂失败，进行一定数量的重试 */
        int retry_times = 1;
        for (; retry_times <= MAX_RECOVERY_RETRY_TIMES; retry_times++) {
            if (uapi_nv_set_restore_mode_partitial(&restore_mode) == ERRCODE_SUCC) {
                break;
            } else {
                /* 打印重试失败次数 */
                ux_log_err("[ux][recovery]recovery partitial fail, retry [%d] times!", retry_times);
            }
        }
        if (retry_times == MAX_RECOVERY_RETRY_TIMES) {
            ux_log_err("[ux][recovery]recovery partitial retry failed max times!");
            switch_recovery_state_off(); /* 连续4次恢厂失败，退出恢厂模式，避免别的功能不可用 */
            return ERRCODE_FAIL;
        }
    }
#if defined(MELODY_DEMO_BMIC_TX) && defined(CONFIG_SUPPORT_FILESYSTEM)
    if (g_need_format == true) {
        int emmc_ret = emmc_drv_format();
        if (emmc_ret != ENOERR) {
            ux_log_err(
                "[ux][recovery] Wrong with errorcode: %d during formatting emmc!\r\n", emmc_ret);
            return ERRCODE_FAIL;
        }
    }
#endif
    switch_recovery_state_off(); /* 恢厂完成，状态调整回去 */
    ux_log_info("[ux][recovery]partial recovery success\r\n");
    ret = ux_api_power_reset(UX_API_POWER_OFF_RECOVER, 0); /* 本地恢厂已完成，调用接口reset(使配置完全生效) */
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][recovery]reset failed!");
        return ret;
    }
    return ERRCODE_SUCC;
}

/* 作为solution层面的对外暴露接口 */
errcode_t ux_api_recovery_partial_process(bool region_mode[KEY_ID_REGION_MAX_NUM], bool need_format)
{
    unused(need_format);
    if (region_mode[0] == true && get_working_state() != STATUS_ON_DUT) { /* 仅允许DUT状态下对第0个区域恢厂 */
        ux_log_err("[ux][recovery]failed! recover region 0");
        return ERRCODE_FAIL;
    }
    errcode_t ret = ux_recovery_condition_check(); /* 检验是否满足恢复出厂的条件 */
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][recovery] fail to permit recovery!");
        return ERRCODE_FAIL;
    }
    /* 恢厂信息暂存到全局变量中 */
    (void)memcpy_s(g_region_mode, sizeof(g_region_mode), region_mode, sizeof(bool) * KEY_ID_REGION_MAX_NUM);
    g_need_format = need_format;
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    ret = ux_bs_sle_stop_livemic_service(UX_BS_SLE_OPERATOR_RECOVERY); /* 直播麦恢厂前关闭星闪直播麦业务 */
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][recovery] stop sle livemic failed\r\n");
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
#endif
    /* 其他产品形态恢厂前不需要关闭蓝牙、星闪业务，直接恢厂 */
    return ux_recovery_process();
}
