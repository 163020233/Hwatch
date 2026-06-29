/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provide ux power solution \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-11-11， Create file. \n
 */

#include "property.h"
#include "ux_fwk_msg_def.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "timer_manager.h"
#include "power_mgmt.h"
#include "feature_config.h"
#include "key_id.h"
#ifdef CONFIG_SUPPORT_USB_DEVICE
#include "usb_drv.h"
#include "usb_device.h"
#endif
#include "ux_audio_recorder.h"
#include "ux_device_manager.h"
#include "ux_bg_config.h"
#include "timer_manager.h"
#include "watchdog_porting.h"
#include "ux_fwk_process.h"
#include "ux_api_box.h"
#include "ux_broadcast.h"
#include "ux_api_broadcast.h"
#if (!defined(MELODY_DEMO_DONGLE) && !defined(MELODY_DEMO_SOUNDBAR)) && !defined(DEMO_WATCH)
#include "ux_api_charge.h"
#endif
#include "ux_power_customer.h"
#include "ux_api_power.h"
#include "ux_power.h"

ux_power_state_t g_ux_power_state = UX_POWER_NORMAL;
ux_power_b_core_t g_ux_b_core_state = UX_POWER_BT_NORMAL;
reset_state_t g_power_reset_state = STATUS_RESET_INVALID;
bool g_is_actual_power_off = true;

static uint32_t g_delay_power_off_time = 0xFFFFFFFF;
#ifndef CONFIG_AUTO_POWER_OFF_TIMEOUT
    #define CONFIG_AUTO_POWER_OFF_TIMEOUT 1800000
#endif
#define AUTO_POWER_OFF_DETECT_TIMEOUT 10000

#ifndef DEMO_WATCH
static uint32_t get_delay_power_off_time(void)
{
    return g_delay_power_off_time;
}
#endif

static errcode_t instant_acc_off_handle(void)
{
    ux_power_state_t curr_state;
    errcode_t err = ux_power_get_state(&curr_state);
    if (err == ERRCODE_SUCC) {
        if (curr_state != UX_POWER_NORMAL) {
            ux_log_err("[ux][power][instant_acc_off_handle]mcu current state:%u", curr_state);
        }
    } else {
        ux_log_err("[ux][power][instant_acc_off_handle] ux_power_get_state failed! err:[%x]", err);
    }

    ux_api_power_acc_off();
    return ERRCODE_SUCC;
}

static void timer_callback_acc_off_handle(void)
{
    ux_power_set_state(UX_POWER_NORMAL);
    g_delay_power_off_time = 0xFFFFFFFF;
    ux_api_power_acc_off();
}

static errcode_t delay_acc_off_handle(ux_api_power_off_reason_t reason, uint32_t time)
{
    errcode_t err;
    if (time >= g_delay_power_off_time) {
        ux_log_err("[ux][power][delay_acc_off_handle]In delayed shutdown state");
        return ERRCODE_FAIL;
    }

    err = ux_power_set_off_reason(reason);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][delay_acc_off_handle] record power on reason failed,err:%x", err);
        return err;
    }
    g_delay_power_off_time = time;
    ux_timer_stop(ACC_OFF_DELAY_TIMER);
    err = ux_timer_start(ACC_OFF_DELAY_TIMER, time);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][delay_acc_off_handle] ux_timer_start failed,err:%x", err);
        return err;
    }
    ux_log_debug("[ux][power][delay_acc_off_handle] ux power off in %ums", time);

    ux_power_set_state(UX_POWER_OFF_DELAY);
    return ERRCODE_SUCC;
}

#if !defined(DEMO_WATCH)
static bool check_auto_power_off_condition(void)
{
    uint16_t device_id_arr[UX_MAX_REMOTE_DEVICE];
    uint32_t device_num;

    ux_device_mgr_get_remote_connected_device_id(device_id_arr, &device_num);
    if (device_num != 0) {
        return false;
    }
#ifdef CONFIG_SUPPORT_USB_DEVICE
    uint8_t usb_connect_stat = USBD_STATE_NOT_READY;
    uint8_t usb_connect_time = USB_CONNECTED_TIME_MAX;
    uapi_usbd_get_usb_stat(&usb_connect_stat, &usb_connect_time);
    if (usb_connect_stat == USBD_STATE_CONNECT) {
        return false;
    }
#endif
#ifdef CONFIG_SUPPORT_AUDIO_RECORDER
    if (ux_audio_get_local_recorder_status() == UX_AUDIO_RECORDER_STATUS_START) {
        return false;
    }
#endif
    return true;
}

static void auto_power_off_detect(void)
{
    errcode_t ret;
    ux_power_state_t curr_state;

    ux_timer_start(AUTO_POWER_OFF_TIMER, AUTO_POWER_OFF_DETECT_TIMEOUT);
    if (check_auto_power_off_condition() == false) {
        ret = ux_power_get_state(&curr_state);
        if (ret != ERRCODE_SUCC) {
            ux_log_err("[ux][power][auto_power_off_detect]ux_power_get_state failed, ret:0x%x", ret);
            return;
        }
        if (curr_state == UX_POWER_OFF_DELAY) {
            ux_api_power_cancel_delay_off();
            ux_log_info("[ux][power][auto_power_off_detect]current task prevents automatic shutdown");
        }
    } else {
        if (get_delay_power_off_time() > CONFIG_AUTO_POWER_OFF_TIMEOUT) {
            ret = ux_api_power_off(UX_API_POWER_OFF_USER, CONFIG_AUTO_POWER_OFF_TIMEOUT);
            if (ret != ERRCODE_SUCC) {
                ux_log_err("[ux][power][auto_power_off_detect]ux_api_power_off failed,err : %x", ret);
            }
        }
    }
}
#endif

errcode_t ux_api_power_cancel_delay_reset(void)
{
    ux_power_state_t curr_state;
    errcode_t err;

    err = ux_power_get_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_off] ux_power_get_state failed,err : %x", err);
        return err;
    }

    if (curr_state != UX_POWER_RESET_DELAY) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_reset]not in acc off delay, state:%u", curr_state);
        return ERRCODE_FAIL;
    }
    err = ux_timer_stop(RESET_DELAY_TIMER);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_reset] ux_timer_stop failed,err:%x", err);
        return err;
    }
    ux_log_debug("[ux][power][ux_api_power_cancel_delay_reset] ux power cancel delay reset success");

    ux_power_set_state(UX_POWER_NORMAL);
    err = ux_power_set_off_reason(UX_API_POWER_OFF_NORMAL);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_reset] ux_power_set_off_reason failed,err:%x", err);
    }
    return err;
}

static errcode_t acc_off_pre_process(void)
{
    ux_power_state_t curr_state;
    errcode_t err = ux_power_get_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][acc_off_pre_process]ux_power_get_state failed,err : %x", err);
        return err;
    }

    if (curr_state == UX_POWER_RESET_DELAY) {
        err = ux_api_power_cancel_delay_reset();
        if (err != ERRCODE_SUCC) {
            ux_log_err("[ux][power][acc_off_pre_process] ux_api_power_cancel_delay_reset failed,err:%x", err);
            return err;
        }
    }

    return ERRCODE_SUCC;
}

errcode_t ux_api_power_off(ux_api_power_off_reason_t reason, uint32_t time)
{
    if (acc_off_pre_process() != ERRCODE_SUCC) {
        ux_log_err("[ux][power][acc_off_pre_process]acc_off_pre_process failed!");
        return ERRCODE_FAIL;
    }

    if (time == 0) {
        errcode_t err = ux_power_set_off_reason(reason);
        if (err != ERRCODE_SUCC) {
            ux_log_err("[ux][power][ux_api_power_off] record power on reason failed!err:%x", err);
            return err;
        }
        return instant_acc_off_handle();
    }
    return delay_acc_off_handle(reason, time);
}

#if (!defined(MELODY_DEMO_DONGLE) && !defined(MELODY_DEMO_SOUNDBAR)) && !defined(DEMO_WATCH)
static bool check_power_cancel_delay_off(void)
{
#ifdef CONFIG_SUPPORT_CONFIG_BATTERY
    errcode_t ret;
    uint32_t battery_soc = 0;
    uint8_t pwr_off_soc;
    ux_api_box_cover_state_t cover_state;
    ux_api_charge_state_t cur_state;

    ret = ux_api_charger_get_charge_state((ux_api_charge_state_t *)&cur_state); /* 充电状态获取 */
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][check_power_cancel_delay_off]get charge state err%d.", ret);
        return true;
    }
    ret = ux_api_charger_get_battery_percentage(&battery_soc);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][check_power_cancel_delay_off]get battery percentage failed.\n");
        return true;
    }
    pwr_off_soc = ux_power_get_instance()->get_power_off_soc();
    /* 低电关机且不充电场景下不能取消下电流程 */
    if (battery_soc <= pwr_off_soc && cur_state != UX_API_CHARGE_STATE_CHARGING) {
        return false;
    }
    ret = ux_api_box_get_cover_status(&cover_state);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][check_power_cancel_delay_off]get cover status err:0x%X.\n", ret);
        return true;
    }
    /* 满电关盒场景下不能取消下电流程 */
    if (cur_state == UX_API_CHARGE_STATE_CHARGE_DONE && cover_state == UX_API_BOX_COVER_STATUS_CLOSE) {
        return false;
    }
#endif
    return true;
}
#endif

errcode_t ux_api_power_cancel_delay_off(void)
{
    ux_power_state_t curr_state;
    errcode_t err;
#if (!defined(MELODY_DEMO_DONGLE) && !defined(MELODY_DEMO_SOUNDBAR)) && !defined(DEMO_WATCH)
    if (!check_power_cancel_delay_off()) {
        ux_log_err("[ux][power]shut-down cannot be canceled in current mode.");
        return ERRCODE_NOT_SUPPORT;
    }
#endif
    err = ux_power_get_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_off]ux_power_get_state failed,err:%x", err);
        return err;
    }

    if (curr_state != UX_POWER_OFF_DELAY) {
        ux_log_info("[ux][power][ux_api_power_cancel_delay_off]not in acc off delay, current state:%u", curr_state);
        return ERRCODE_SUCC;
    }
    err = ux_timer_stop(ACC_OFF_DELAY_TIMER);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_off] ux_timer_stop failed,err : %x", err);
        return err;
    }
    g_delay_power_off_time = 0xFFFFFFFF;
    ux_log_debug("[ux][power][ux_api_power_cancel_delay_off] ux power cancel delay off success");

    ux_power_set_state(UX_POWER_NORMAL);
    return err;
}

static errcode_t instant_reset_handle(void)
{
    ux_power_state_t curr_state;
    errcode_t err = ux_power_get_state(&curr_state);
    if (err == ERRCODE_SUCC) {
        if (curr_state != UX_POWER_NORMAL) {
            ux_log_err("[ux][power][instant_reset_handle]mcu current state:%u", curr_state);
        }
    } else {
        ux_log_err("[ux][power][instant_reset_handle] ux_power_get_state failed,err : %x", err);
    }

    ux_event_msg_t msg = { .task_src = UX_POWER };
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
    return ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_RESET, (uint8_t *)(&msg), sizeof(msg));
#endif
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_RESET_INFO, (uint8_t *)(&msg), sizeof(msg));
}

static void timer_callback_reset_handle(void)
{
    ux_event_msg_t msg = { .task_src = UX_POWER };
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
    ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_RESET, (uint8_t *)(&msg), sizeof(msg));
#endif
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_RESET_INFO, (uint8_t *)(&msg), sizeof(msg));
}

static errcode_t delay_reset_handle(ux_api_power_off_reason_t reason, uint32_t time)
{
    errcode_t err;
    ux_power_state_t curr_state;

    err = ux_power_get_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][delay_reset_handle] ux_power_get_state failed,err : %x", err);
        return err;
    }

    if (curr_state != UX_POWER_NORMAL) {
        ux_log_err("[ux][power][delay_reset_handle]curr_state invalid, state: %u", curr_state);
        return ERRCODE_FAIL;
    }

    err = ux_power_set_off_reason(reason);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][delay_reset_handle] record power on reason failed,err : %x", err);
        return err;
    }
    err = ux_timer_start(RESET_DELAY_TIMER, time);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][delay_reset_handle] ux_timer_start failed,err : %x", err);
        return err;
    }
    ux_log_debug("[ux][power][delay_reset_handle] ux power reset in %ums", time);

    ux_power_set_state(UX_POWER_RESET_DELAY);
    return ERRCODE_SUCC;
}

static errcode_t reset_pre_process(void)
{
    ux_power_state_t curr_state;
    errcode_t err = ux_power_get_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][reset_pre_process] ux_power_get_state failed,err:%x", err);
        return err;
    }

    if (curr_state == UX_POWER_OFF_DELAY) {
        err = ux_api_power_cancel_delay_off();
        if (err != ERRCODE_SUCC) {
            ux_log_err("[ux][power][reset_pre_process] ux_api_power_cancel_delay_off failed,err:%x", err);
            return err;
        }
    }
    return ERRCODE_SUCC;
}

errcode_t ux_api_power_reset(ux_api_power_off_reason_t reason, uint32_t time)
{
    if (reset_pre_process() != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_reset] reset_pre_process failed!");
        return ERRCODE_FAIL;
    }

    if (time == 0) {
        errcode_t err = ux_power_set_off_reason(reason);
        if (err != ERRCODE_SUCC) {
            ux_log_err("[ux][power][ux_api_power_reset] record power reset reason failed!");
            return err;
        }
        return instant_reset_handle();
    }
    return delay_reset_handle(reason, time);
}

errcode_t ux_power_get_state(ux_power_state_t *state)
{
    if (state == NULL) {
        ux_log_err("[ux][power][ux_power_get_state] state is null");
        return ERRCODE_FAIL;
    }

    *state = g_ux_power_state;
    return ERRCODE_SUCC;
}

void ux_power_set_state(ux_power_state_t state)
{
    if (state >= UX_POWER_STATE_MAX) {
        ux_log_err("[ux][power][ux_power_set_state] invalid state:[%u]", state);
        return;
    }
    g_ux_power_state = state;
}

void update_reset_info_to_nv(reset_state_t state)
{
    if (state > STATUS_RESET_VALID) {
        ux_log_err("[ux][power][write_reset_info_to_nv] failed!state:[%u]", state);
        return;
    }

    errcode_t err = property_set_non_upgrade_bytes(NV_ID_UX_RESET_CONFIG, (uint8_t *)&state, sizeof(state));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][write_reset_info_to_nv]uapi_nv_write failed! err :[%x]", err);
    }
}

errcode_t ux_power_set_off_reason(ux_api_power_off_reason_t reason)
{
    if (reason >= UX_API_POWER_OFF_MAX) {
        ux_log_err("[ux][power][ux_power_set_off_reason]invalid reason: %u", reason);
        return ERRCODE_FAIL;
    }

    errcode_t err =
        property_set_bytes(NV_ID_UX_POWER_OFF_REASON, (uint8_t *)&reason, sizeof(reason));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_set_off_reason] property_set_bytes failed! err:[%x]", err);
    }
    return err;
}

ux_api_power_off_reason_t ux_api_power_get_off_reason(void)
{
    ux_api_power_off_reason_t reason;

    uint8_t *p_ret = property_get_bytes(NV_ID_UX_POWER_OFF_REASON, (uint8_t *)&reason, sizeof(reason));
    if (p_ret == NULL) {
        ux_log_err("[ux_api_power_get_off_reason] property_get_bytes failed!");
        return UX_API_POWER_OFF_MAX;
    }
    return reason;
}

void ux_power_set_actual_power_off(bool actual_power_off)
{
    g_is_actual_power_off = actual_power_off;
}

bool ux_power_get_actual_power_off(void)
{
    return g_is_actual_power_off;
}

#ifndef DEMO_WATCH
static void ux_power_watchdog_state_callback(cores_t core_type)
{
    if (core_type == HIFI) {
        ux_log_err("[UX][power] Dsp watchdog interrupt occurred.\n");
    } else if (core_type == BT) {
        ux_log_err("[UX][power] B core watchdog interrupt occurred.\n");
    }
}
#endif

errcode_t ux_power_deinit(void)
{
    ux_timer_stop(AUTO_POWER_OFF_TIMER);
    ux_timer_deregister(AUTO_POWER_OFF_TIMER);
    return UX_ERR_SUCCESS;
}

errcode_t ux_api_power_on(void)
{
    ux_event_msg_t event_msg;
    event_msg.task_src = UX_MASTER;
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
    return ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_ON, (uint8_t *)(&event_msg), sizeof(event_msg));
#endif
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POWER_ON, (uint8_t *)(&event_msg), sizeof(event_msg));
}

errcode_t ux_api_power_acc_on(void)
{
#if defined(MELODY_DEMO_BMIC_RX) || defined(MELODY_DEMO_BMIC_TX)
    return ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_ACC_ON, NULL, 0);
#endif
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_ACC_ON, NULL, 0);
}

errcode_t ux_api_power_standby(void)
{
    return ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_STANDBY, NULL, 0);
}

errcode_t ux_api_power_acc_off(void)
{
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA) || defined(DEMO_WATCH)
    ux_adapter_power_off_process();
    return UX_ERR_SUCCESS;
#endif
    if (g_is_actual_power_off) {
        return ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_ACC_OFF, NULL, 0);
    } else {
        return ux_api_power_acc_on();
    }
}

void ux_power_set_bt_core_state(ux_power_b_core_t state)
{
    if (state > UX_POWER_BT_MAX) {
        ux_log_err("[ux][power][ux_power_set_bt_core_state] invalid state:[%u]", state);
        return;
    }

    /* 上报BT核下电状态,用于下电关闭音频通路 */
    switch (state) {
        case UX_POWER_BT_OFF:
            ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_BT_POWER_OFF, NULL, 0);
            break;
        case UX_POWER_BT_NORMAL:
            ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_BT_POWER_ON, NULL, 0);
            break;
        case UX_POWER_BT_ON_DOING:
            ux_api_fwk_push_event(UX_EVENT_TYPE_POWER, UX_POWER_BT_POWER_ON, NULL, 0);
            break;
        default:
            break;
    }

    g_ux_b_core_state = state;
}

errcode_t ux_power_get_bt_core_state(ux_power_b_core_t *state)
{
    if (state == NULL) {
        ux_log_err("[ux][power][ux_power_get_bt_core_state] state is null");
        return ERRCODE_FAIL;
    }

    *state = g_ux_b_core_state;
    return ERRCODE_SUCC;
}

errcode_t ux_power_cancel_bt_delay_off(void)
{
    ux_power_b_core_t curr_state;
    errcode_t err;

    err = ux_power_get_bt_core_state(&curr_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_off] ux_power_get_bt_core_state failed,err : %x", err);
        return err;
    }

    if (curr_state != UX_POWER_BT_OFF_DELAY) {
        ux_log_err("[ux][power][ux_power_cancel_bt_delay_off]not in bt acc off delay, current state:%u", curr_state);
        return ERRCODE_FAIL;
    }
    err = ux_timer_stop(BT_ACC_OFF_DELAY_TIMER);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_api_power_cancel_delay_off] ux_timer_stop failed,err : %x", err);
        return err;
    }

    ux_power_set_bt_core_state(UX_POWER_BT_NORMAL);
    return ERRCODE_SUCC;
}

errcode_t ux_power_bt_on(void)
{
    ux_power_b_core_t state;
    errcode_t err = ux_power_get_bt_core_state(&state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_bt_on] ux_power_get_bt_core_state fail err:%x", err);
        return err;
    }

    switch (state) {
        /* 若设备处于b核延时下电中，取消延时下电操作，此时序后续可根据产品需求进行调整 */
        case UX_POWER_BT_OFF_DELAY:
            if (ux_power_cancel_bt_delay_off() != ERRCODE_SUCC) {
                ux_log_err("[ux][power][ux_power_bt_on] ux_power_cancel_bt_delay_off failed");
                return ERRCODE_FAIL;
            }
            return ERRCODE_SUCC;
        /* B核正常上电无需重复上电 */
        case UX_POWER_BT_NORMAL:
            return ERRCODE_FAIL;
        /* B核上电中无需重复上电 */
        case UX_POWER_BT_ON_DOING:
            return ERRCODE_FAIL;
        /* 其他状态正常上电 */
        default:
            break;
    }

    err = ux_adapter_power_on(UX_POWER_CORE_BT);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_bt_on]uapi_power_on_core fail err:%x", err);
        return err;
    }

    ux_power_set_bt_core_state(UX_POWER_BT_NORMAL);
    return ERRCODE_SUCC;
}

static errcode_t bt_off_impl(void)
{
    errcode_t err;

    err = ux_adapter_power_off(UX_POWER_CORE_BT);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][bt_off_impl]uapi_power_off_core fail,err:%x", err);
        return err;
    }

    ux_power_set_bt_core_state(UX_POWER_BT_OFF);
    return ERRCODE_SUCC;
}

static void timer_callback_bt_off_handle(void)
{
    errcode_t err = bt_off_impl();
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][timer_callback_bt_off_handle]bt_off_impl fail,err:%x", err);
    }
}

errcode_t ux_power_bt_off(uint32_t time)
{
    ux_power_b_core_t state;
    errcode_t err = ux_power_get_bt_core_state(&state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_bt_off] ux_power_get_bt_core_state fail err:%x", err);
        return err;
    }

    switch (state) {
        /* bt延时下电切换为取消延时下电，在后面继续执行下电操作 */
        case UX_POWER_BT_OFF_DELAY:
            if (ux_power_cancel_bt_delay_off() != ERRCODE_SUCC) {
                ux_log_err("[ux][power][ux_power_bt_off]ux_power_cancel_bt_delay_off fail");
            }
            break;
        /* bt已下电返回错误，无需重复下电 */
        case UX_POWER_BT_OFF:
            return ERRCODE_FAIL;
        /* 其他状态bt正常下电 */
        default:
            break;
    }

    if (time == 0) {
        return bt_off_impl();
    } else {
        err = ux_timer_start(BT_ACC_OFF_DELAY_TIMER, time);
        if (err != ERRCODE_SUCC) {
            ux_log_err("[ux][power][ux_power_bt_off] ux_timer_start failed,err:%x", err);
            return err;
        }

        ux_power_set_bt_core_state(UX_POWER_BT_OFF_DELAY);
        return ERRCODE_SUCC;
    }
}

#if !defined(DEMO_WATCH)
errcode_t ux_power_get_auto_power_off_setting(uint8_t* auto_power_off_enable, uint8_t len)
{
    uint8_t *p_ret = property_get_bytes(NV_ID_AUTO_POWER_OFF, (uint8_t *)auto_power_off_enable, len);
    if (p_ret == NULL) {
        ux_log_err("[ux][power][ux_power_get_auto_power_off_setting]property_get_bytes error");
        return UX_ERR_READ_NV_FAILED;
    }

    return UX_ERR_SUCCESS;
}

errcode_t ux_api_power_set_auto_power_off(bool enable)
{
    errcode_t ret;
    uint8_t auto_power_off_enable;

    ret = ux_power_get_auto_power_off_setting(&auto_power_off_enable, sizeof(uint8_t));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][power][ux_api_power_set_auto_power_off]get auto power setting err");
        return ret;
    }

    if (enable == auto_power_off_enable) {
        return UX_ERR_SUCCESS;
    }

    if (enable == false) {
        ret = ux_timer_stop(AUTO_POWER_OFF_TIMER);
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux][power][ux_api_power_set_auto_power_off] ux_timer_stop error");
            return ret;
        }
    } else {
        ret = ux_timer_start(AUTO_POWER_OFF_TIMER, AUTO_POWER_OFF_DETECT_TIMEOUT);
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux][power][ux_api_power_set_auto_power_off]ux_timer_start error");
            return ret;
        }
    }

    auto_power_off_enable = enable;
    ret = property_set_bytes(NV_ID_AUTO_POWER_OFF, (uint8_t *)&auto_power_off_enable,
        sizeof(auto_power_off_enable));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][power][ux_api_power_set_auto_power_off]property_set_bytes error [%d]", ret);
        return ERRCODE_FAIL;
    }
    return UX_ERR_SUCCESS;
}
#endif

errcode_t ux_power_init(void)
{
    errcode_t ret;
#ifndef DEMO_WATCH
    uint8_t auto_power_off_enable;
    ux_adapter_register_watchdog_irq_cb((watchdog_dump_callback_t)ux_power_watchdog_state_callback);
    ret = ux_api_power_acc_on();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][power][ux_power_init]ux_api_power_acc_on err:0x%x.\n", ret);
        return ret;
    }
    ux_timer_register(AUTO_POWER_OFF_TIMER, auto_power_off_detect);
    ret = ux_power_get_auto_power_off_setting(&auto_power_off_enable, sizeof(uint8_t));
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux][power][ux_power_init]ux_power_get_auto_power_off_setting err:0x%x", ret);
        return ret;
    }
    if (auto_power_off_enable == true) {
        ux_log_debug("[ux][power][ux_power_init]initiate scheduled shutdown");
        ux_timer_start(AUTO_POWER_OFF_TIMER, AUTO_POWER_OFF_DETECT_TIMEOUT);
    }
#endif
    ret = ux_timer_register(ACC_OFF_DELAY_TIMER, timer_callback_acc_off_handle);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_init]ux_timer_register failed,err:%x", ret);
        return ret;
    }
    ret = ux_timer_register(RESET_DELAY_TIMER, timer_callback_reset_handle);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_init]ux_timer_register failed,err:%x", ret);
        return ret;
    }
    ret = ux_timer_register(BT_ACC_OFF_DELAY_TIMER, timer_callback_bt_off_handle);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][power][ux_power_init]ux_timer_register failed,err:%x", ret);
        return ret;
    }
    ux_broadcast_broadcast_event(UX_API_BROADCAST_EVENT_ACC_ON);
    return UX_ERR_SUCCESS;
}

reset_state_t ux_power_get_reset_valid_state(void)
{
    return g_power_reset_state;
}

errcode_t ux_power_set_reset_valid_state(reset_state_t reset_state)
{
    if (reset_state != STATUS_RESET_INVALID && reset_state != STATUS_RESET_VALID) {
        ux_log_err("[ux][power]invalid reset_state:%u", reset_state);
        return UX_ERR_INVALID_PARAMETER;
    }

    g_power_reset_state = reset_state;
    return UX_ERR_SUCCESS;
}

void ux_power_info_restore(void)
{
    reset_state_t reset_state;
    uint8_t *p_ret = property_get_bytes(NV_ID_UX_RESET_CONFIG, (uint8_t *)&reset_state, sizeof(reset_state_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][power][ux_power_info_restore] property_get_bytes failed!");
        return;
    }
    errcode_t ret = ux_power_set_reset_valid_state(reset_state);
    if (ret != UX_ERR_SUCCESS) {
        ux_power_set_reset_valid_state(STATUS_RESET_INVALID);
    }
}

errcode_t ux_api_power_set_power_off_mode(ux_api_power_off_mode_t mode)
{
    if (mode >= UX_API_POWER_OFF_MODE_MAX) {
        ux_log_err("[ux][power]invalid mode");
        return ERRCODE_INVALID_PARAM;
    }

    if (mode == UX_API_POWER_OFF_MODE_HARD) {
        ux_power_set_actual_power_off(true);
    } else {
        ux_power_set_actual_power_off(false);
    }
    return ERRCODE_SUCC;
}

errcode_t ux_api_power_set_off_reason(ux_api_power_off_reason_t reason)
{
    return ux_power_set_off_reason(reason);
}