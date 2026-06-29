/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide managing device instance service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-25， Create file. \n
 */
#include <stdbool.h>
#ifndef DEMO_WATCH
#include "feature_config.h"
#endif
#include "securec.h"
#include "ux_config.h"
#include "key_id.h"
#include "nv.h"
#include "property.h"
#include "systick.h"
#include "autoconf.h"
#include "ux_config_common.h"
#include "ux_event_define.h"
#include "ux_common_log.h"
#include "ux_power.h"
#include "ux_api_audio_common.h"
#include "ux_api_audio_setting.h"
#include "ux_api_fwk.h"
#include "ux_api_tws.h"
#include "ux_fwk_sync_process.h"
#include "ux_api_usb.h"
#include "ux_jack.h"
#include "ux_api_communication.h"
#include "ux_errcode.h"
#include "errcode.h"
#include "ux_device_manager.h"
#include "ux_logic_manager.h"
#include "ux_powermgr.h"
#include "ux_hal_memory.h"
#include "ux_audio_utils.h"
#include "ux_api_system_config.h"
#ifdef SUPPORT_RX_DEMO_FEATURE
#include "ui_demo_api.h"
#endif
#if defined(UX_BS_BT_SUPPORT)
#include "ux_bt_common.h"
#include "ux_bt_tws.h"
#include "bt_service.h"
#include "ux_multiple_device.h"
#include "ux_bs_manager.h"
#endif
#ifdef CONFIG_SUPPORT_TONE
#include "ux_api_audio_tone.h"
#include "tone_service.h"
#endif
#include "ux_api_sysmgr.h"
#include "ux_system_manager.h"
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
#define DEFAULT_DELAY_WEAR_AIDMODE              0       /* 佩戴延时进助听延时NV项的默认值 */
#define DEFAULT_WEAR_AIDMODE 0
static uint8_t g_device_reconn_times = 0; // 回连触发次数
#endif


#define POWER_ON_MIN_DELAY_TIME 1000  // 单位ms
#define DEFAULT_TOUCH_CONFIG 2           /* 触控检测控制NV项的默认值 */
#define DEFAULT_AUDIO_AID_MIX 4          /* 混音模式控制NV项的默认值 */
#define DEFAULT_TIME_SWITCH_AIDMODE 3000 /* 音频切助听延时时间NV项的默认值 */
#define DEFAULT_SERVICE_TIME_AUDIO 0     /* 音频播放时长记录NV项的默认值 */
#define DEFAULT_SERVICE_TIME_CALLING 0   /* 通话时长记录NV项的默认值 */
#define DEFAULT_SERVICE_TIME_CONFIG 0    /* 配置APP连接时长记录NV项的默认值 */
#define LED_EVENT_OFFSET 16
#define POWER_ON_MIN_DELAY_TIME 1000  // 单位ms
#define AFC_MIN_TIME 5             /* 啸叫校准下发时间下限 */
#define AFC_MAX_TIME 10            /* 啸叫校准下发时间上限 */

#define TERM_NODE_MAX_NUM 4

static ux_system_info_t g_system_info;
static uint64_t g_audio_service_time = 0;
static uint64_t g_config_service_time = 0;
static uint64_t g_calling_service_time = 0;
ux_system_info_t *ux_get_system_info(void)
{
    return &g_system_info;
}

void write_service_time_to_nv(void)
{
    errcode_t err = property_set_bytes(NV_ID_DEV_STATISTIC,
        (uint8_t *)&g_system_info.nv_content.service_time,
        sizeof(ux_service_time_count_t));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][write_service_time_to_nv] property_set_bytes failed! err:[%x]", err);
    }
}

void update_service_time(void)
{
    ux_service_time_count_t time_count;
    uint8_t *p_ret = property_get_bytes(NV_ID_DEV_STATISTIC, (uint8_t *)&time_count, sizeof(ux_service_time_count_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][update_service_time] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.service_time.audio_time = time_count.audio_time;
    g_system_info.nv_content.service_time.calling_time = time_count.calling_time;
    g_system_info.nv_content.service_time.config_time = time_count.config_time;
}

void init_service_time(void)
{
    g_audio_service_time = 0;
    g_config_service_time = 0;
    g_system_info.nv_content.service_time.audio_time = 0;
    g_system_info.nv_content.service_time.config_time = 0;
    g_calling_service_time = 0;
#if defined(MELODY_DEMO_BMIC_RX)
    g_system_info.nv_content.service_time.calling_time = 0;
#endif
}

void set_real_charge_state(uint8_t state)
{
    if (state >= STATUS_CHARGE_BUTT) {
        ux_log_err("[ux][set_real_charge_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.local_info.charge_state = state;
}

uint8_t get_real_charge_state(void)
{
    return g_system_info.local_info.charge_state;
}

void set_device_disconnect_reason(uint8_t state)
{
    g_system_info.pair_state.device_disconnect_reason = state;
}

void set_device_reconn_phase(uint8_t phase)
{
    if (phase > STATUS_DEVICE_RECONN_END) {
        ux_log_err("[ux][set_device_reconn_phase] invalid phase:[%u]", phase);
        return;
    }
    g_system_info.pair_state.device_reconn_phase = phase;
}

uint8_t get_device_reconn_phase(void)
{
    return g_system_info.pair_state.device_reconn_phase;
}

void set_device_pair_state(uint8_t state)
{
    if (state >= STATUS_DEVICE_PAIR_BUTT) {
        ux_log_err("[ux][set_device_pair_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.device_pair_state = state;
}

uint8_t get_device_pair_state(void)
{
    return g_system_info.pair_state.device_pair_state;
}

uint8_t get_bt_device_pair_state(void)
{
    return g_system_info.pair_state.bt_device_pair_state;
}

void set_bt_device_pair_state(uint8_t state)
{
    if (state >= STATUS_DEVICE_PAIR_BUTT) {
        ux_log_err("[ux][set_bt_device_pair_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.bt_device_pair_state = state;
}

uint8_t get_sle_device_pair_state(void)
{
    return g_system_info.pair_state.sle_device_pair_state;
}

void set_sle_device_pair_state(uint8_t state)
{
    if (state >= STATUS_DEVICE_PAIR_BUTT) {
        ux_log_err("[ux][set_sle_device_pair_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.sle_device_pair_state = state;
}

uint8_t get_connected_protocol_type(void)
{
    return g_system_info.pair_state.connected_protocol_type;
}

void set_connected_protocol_type(uint8_t type)
{
    if (type > UX_WORK_PROTOCOL_MAX) {
        ux_log_err("[ux][set_connected_protocol_type] invalid type:[%u]", type);
        return;
    }
    g_system_info.pair_state.connected_protocol_type = type;
}

uint8_t get_peer_connected_protocol_type(void)
{
    return g_system_info.pair_state.peer_connected_protocol_type;
}

void set_peer_connected_protocol_type(uint8_t type)
{
    if (type > UX_WORK_PROTOCOL_MAX) {
        ux_log_err("[ux][set_peer_connected_protocol_type] invalid type:[%u]", type);
        return;
    }
    g_system_info.pair_state.peer_connected_protocol_type = type;
}

uint8_t get_combo_pairing_state(void)
{
    return g_system_info.pair_state.combo_pairing_state;
}

uint8_t get_bt_pairing_state(void)
{
    return g_system_info.pair_state.bt_pairing_state;
}

uint8_t get_sle_pairing_state(void)
{
    return g_system_info.pair_state.sle_pairing_state;
}

uint8_t get_ble_pairing_state(void)
{
    return g_system_info.pair_state.ble_pairing_state;
}

uint8_t get_peer_bt_pairing_state(void)
{
    return g_system_info.pair_state.peer_bt_pairing_state;
}

uint8_t get_peer_sle_pairing_state(void)
{
    return g_system_info.pair_state.peer_sle_pairing_state;
}

uint8_t get_peer_combo_pairing_state(void)
{
    return g_system_info.pair_state.peer_combo_pairing_state;
}

void set_device_reconn_type(uint8_t type)
{
    g_system_info.pair_state.device_reconn_type = type; /* 回连类型: ux_event_reconn_code_t */
}

uint8_t get_device_reconn_type(void)
{
    return g_system_info.pair_state.device_reconn_type;
}

void set_peer_device_reconn_type(uint8_t type)
{
    g_system_info.pair_state.peer_device_reconn_type = type; /* 回连类型: ux_event_reconn_code_t */
}

uint8_t get_peer_device_reconn_type(void)
{
    return g_system_info.pair_state.peer_device_reconn_type;
}

void set_bt_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.bt_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_bt_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.bt_pairing_state = state;
#if defined(UX_BS_BT_SUPPORT)
    ux_bt_send_tws_connect_state((uint8_t)STATUS_SEND_BT_PAIRING);
#endif
}

void set_sle_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.sle_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_sle_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.sle_pairing_state = state;
#if defined(UX_BS_BT_SUPPORT)
    ux_bt_send_tws_connect_state((uint8_t)STATUS_SEND_SLE_PAIRING);
#endif
}

void set_combo_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.combo_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_combo_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.combo_pairing_state = state;
#if defined(UX_BS_BT_SUPPORT)
    ux_bt_send_tws_connect_state((uint8_t)STATUS_SEND_COMBO_PAIRING);
#endif
}

void set_ble_pairing_state(uint8_t state)
{
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_ble_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.ble_pairing_state = state;
}

void set_peer_bt_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.peer_bt_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_peer_bt_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_bt_pairing_state = state;
}

void set_peer_sle_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.peer_sle_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_peer_sle_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_sle_pairing_state = state;
}

void set_peer_combo_pairing_state(uint8_t state)
{
    if (state == g_system_info.pair_state.peer_combo_pairing_state) {
        return;
    }
    if (state >= STATUS_DEVICE_PAIRING_BUTT) {
        ux_log_err("[ux][set_peer_combo_pairing_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_combo_pairing_state = state;
}

void set_lowbattery_state(uint8_t state)
{
    if (state > STATUS_LOW_BATTERY_BUTT) {
        ux_log_err("[ux][set_lowbattery_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.lowbattery_state = state;
}

uint8_t get_lowbattery_state(void)
{
    return g_system_info.state.lowbattery_state;
}

void set_working_state(uint8_t state)
{
    if (state >= STATUS_WORKING_BUTT) {
        ux_log_err("[ux][set_working_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.work_state.ux_manager_pre_state = g_system_info.state.work_state.ux_manager_state;
    g_system_info.state.work_state.ux_manager_state = state;
}

uint8_t get_working_state(void)
{
    return g_system_info.state.work_state.ux_manager_state;
}

uint8_t get_pre_working_state(void)
{
    return g_system_info.state.work_state.ux_manager_pre_state;
}

void set_bt_working_state(uint8_t state)
{
    if (state >= STATUS_WORKING_BUTT) {
        ux_log_err("[ux][set_bt_working_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.work_state.ux_bg_state = state;
}

uint8_t get_bt_working_state(void)
{
    return g_system_info.state.work_state.ux_bg_state;
}

#if defined(CONFIG_SUPPORT_SOURCE_MANAGER_FOCUS_MODE)
errcode_t set_play_mode(uint8_t play_mode)
{
    if ((ux_api_audio_stream_type_t)play_mode >= UX_API_AUDIO_STREAM_TYPE_INVALID) {
        ux_log_err("[ux][set_play_mode] invalid play_mode:[%u]\n", play_mode);
        return ERRCODE_FAIL;
    }

    g_system_info.nv_content.play_mode = play_mode;
    errcode_t ret = property_set_bytes(NV_ID_PLAY_MODE, (uint8_t *)&play_mode,
        sizeof(uint8_t));
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux_api_source_manager_set_play_mode]save play_mode failed, ret=%x\n", ret);
        return ret;
    }
    return ret;
}

uint8_t get_play_mode(void)
{
    return g_system_info.nv_content.play_mode;
}

void update_play_mode(void)
{
    uint8_t play_mode = UX_API_AUDIO_STREAM_TYPE_CALL_NEARLINK;
    uint8_t *p_ret = property_get_bytes(NV_ID_PLAY_MODE, (uint8_t *)&play_mode, sizeof(uint8_t));
    ux_log_info("[ux][update_play_mode] NV = [%u] \n", play_mode);
    if (p_ret == NULL) {
        ux_log_err("[ux][update_play_mode] property_get_bytes failed!\n");
        return;
    }
    if (play_mode >= UX_API_AUDIO_STREAM_TYPE_INVALID) {
        ux_log_err("[ux][update_play_mode] play_mode=%d INVALID\n", play_mode);
        // 无效值，设置默认值
        if (set_play_mode(UX_API_AUDIO_STREAM_TYPE_CALL_NEARLINK) != ERRCODE_SUCC) {
            ux_log_err("[ux][update_play_mode] set_play_mode failed\n");
        }
        return;
    }
    g_system_info.nv_content.play_mode = play_mode;
    ux_log_info("[ux][update_play_mode] play_mode=%u OK\n", g_system_info.nv_content.play_mode);
}
#endif

void set_led_state(uint8_t state)
{
    if (state >= NOTIFICA_WORK_STATE_BUTT) {
        ux_log_err("[ux][set_led_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.notifica_state.led_state = state;
}

static void power_on_notifica_state_init(void)
{
    set_led_state(POWER_ON_NOTIFICA_NOT_FINISH);
#if defined(CONFIG_SUPPORT_TONE)
    set_tone_state(POWER_ON_NOTIFICA_NOT_FINISH);
#endif
}

static void power_off_notifica_state_init(void)
{
    set_led_state(POWER_OFF_NOTIFICA_NOT_FINISH);
#if defined(CONFIG_SUPPORT_TONE)
    set_tone_state(POWER_OFF_NOTIFICA_NOT_FINISH);
#endif
}

#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || \
defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
void set_notifica_state_finish(notifica_work_state_t state)
{
    if (state >= NOTIFICA_WORK_STATE_BUTT) {
        ux_log_err("[ux][device][set_notifica_state_finish] invalid state : [%u]", state);
        return;
    }
    set_led_state((uint8_t)state);
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_tone_state((uint8_t)state);
#endif
}
#endif

bool is_off_working_state(void)
{
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_EARPHONE) || \
defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
#ifdef DEPENDENCY_WORKING_STATE
#endif
    if ((get_working_state() == STATUS_OFF_WORKING) || (get_working_state() == STATUS_OFF_WORKING_DELAY) ||
        (get_working_state() == STATUS_ACC_OFF)) {
        return true;
    }
#if defined(MELODY_DEMO_BMIC_TX)
#endif
#endif
    return false;
}

void working_state_init(void)
{
    uint8_t ftm_mode;
    uint8_t *p_ret = property_get_bytes(NV_ID_FTM_CONFIG, &ftm_mode, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][working_state_init] property_get_bytes failed!");
        set_working_state(STATUS_OFF_WORKING);
        return;
    }

    if (ftm_mode > STATUS_FTM_DISABLE && ftm_mode < STATUS_FTM_BUTT) {
        /* 进入产线模式 */
        set_working_state(STATUS_ON_DUT);
        ux_log_warn("[ux][working_state_init] working_state_init enter dut!");
        return;
    }
    set_working_state(STATUS_OFF_WORKING);
}

void set_ftm_mode(ftm_state_t state)
{
    if (state >= STATUS_FTM_BUTT) {
        ux_log_err("[ux][set_ftm_mode] invalid state : [%u]", state);
        return;
    }

    uint8_t ftm_mode = (uint8_t)state;
    nv_key_attr_t attr = { false, false, true, 0 };
    errcode_t nv_op = uapi_nv_write_with_attr(NV_ID_FTM_CONFIG, &ftm_mode, sizeof(uint8_t), &attr, NULL);
    if (nv_op != ERRCODE_SUCC) {
        ux_log_err("[ux][set_ftm_mode] property_set_bytes failed! err: [%x]", nv_op);
    }
}

uint8_t get_ftm_mode(void)
{
    uint8_t values;
    uint8_t* ftm_mode = property_get_bytes(NV_ID_FTM_CONFIG, &values, sizeof(uint8_t));
    if (ftm_mode == NULL) {
        ux_log_err("[ux][get_ftm_mode] property_get_bytes failed!\n");
        return STATUS_FTM_DISABLE;
    }
    return *ftm_mode;
}

static void set_all_working_state(uint8_t state)
{
    if (state >= STATUS_WORKING_BUTT) {
        ux_log_err("[ux][set_all_working_state] invalid state : [%u]", state);
        return;
    }
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_bt_working_state(state);
    set_haid_working_state(state);
#endif
    set_working_state(state);
}

void set_storage_power_state(uint8_t state)
{
    if (state >= STATUS_STORAGE_POWER_BUTT) {
        ux_log_err("[ux][set_storage_power_state] invalid state : [%u]", state);
        return;
    }
    g_system_info.state.storage_power_state = state;
}

uint8_t get_storage_power_state(void)
{
    return g_system_info.state.storage_power_state;
}

bool ux_sys_mgr_check_power_on_state(void)
{
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    if (ux_powermgr_get_power_state() == UX_API_POWER_MGR_STAY_POWER_ON) {
        return true;
    }

    return false;
#else
    return true;
#endif
}

bool ux_sys_mgr_check_local_device_id(uint16_t device_id)
{
#if defined(MELODY_DEMO_BMIC_TX) || defined(MELODY_DEMO_BMIC_RX)
    uint16_t local_dev_id;
    errcode_t ret;
    ret = ux_device_mgr_get_local_device_id(&local_dev_id);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux_sys_mgr_check_local_device_id] get local device error!");
        return false;
    }

    if (device_id == local_dev_id || device_id == UX_DEVICE_ID_DEFAULT) {
        return true;
    }
    return false;
#else
    unused(device_id);
    return true;
#endif
}

void set_box_battery_percent(uint8_t percent)
{
    if (percent > BATTERY_INFO_MAX_VALUE) {
        ux_log_err("[ux][set_box_battery_percent] invalid percent : [%u]", percent);
        return;
    }
    g_system_info.box_info.battery_percent = percent;
}

uint8_t get_box_battery_percent(void)
{
    return g_system_info.box_info.battery_percent;
}

void update_button_config(void)
{
    ux_button_config_t button_config;
    uint8_t *p_ret = property_get_bytes(NV_ID_DEV_BUTTON_CONFIG, (uint8_t *)&button_config, sizeof(ux_button_config_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][update_button_config] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.button_config.long_button = button_config.long_button;
    g_system_info.nv_content.button_config.short_button = button_config.short_button;
}

void set_ota_state(uint8_t state)
{
    if (state > STATUS_OTA_BUTT) {
        ux_log_err("[ux][set_ota_state] invalid! state: [%u]", state);
        return;
    }
    g_system_info.state.ota_state = state;
}

uint8_t get_ota_state(void)
{
    return g_system_info.state.ota_state;
}

void set_power_off_mode(uint8_t mode)
{
    if (mode >= POWER_OFF_MODE_BUTT) {
        ux_log_err("[ux][set_power_off_mode] failed！ mode：[%u]", mode);
        return;
    }
    ux_log_warn("[ux]set_power_off_mode:%u", mode);
    g_system_info.state.power_mode.ux_power_off_mode = mode;
}

uint8_t get_power_off_mode(void)
{
    return g_system_info.state.power_mode.ux_power_off_mode;
}

void set_power_on_mode(uint8_t mode)
{
    if (mode >= POWER_ON_MODE_BUTT) {
        ux_log_err("[ux][set_power_on_mode] failed! mode:[%u]", mode);
        return;
    }
    g_system_info.state.power_mode.ux_power_on_mode = mode;
}
bool is_bt_init_state(void)
{
    return g_system_info.state.bt_init_state;
}

void set_scene_mode_state(uint16_t state)
{
    g_system_info.nv_content.scene_mode_status = state;
}

void update_scene_mode_status(void)
{
    uint16_t status;
    uint8_t *p_ret = property_get_bytes(NV_ID_SCENE_MODE_STATUS, (uint8_t *)&status, sizeof(uint16_t));
    if (p_ret == NULL) {
        ux_log_err("[ux] get NV_ID_SCENE_MODE_STATUS failed!");
        return;
    }

    set_scene_mode_state(status);
    ux_log_err("[ux]update_scene_mode_status status[%u]!", g_system_info.nv_content.scene_mode_status);

    uint8_t nv_type = APP_SET_SCENE_MODE_STATUS;
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
}

void set_bt_power_off_mode(uint8_t mode)
{
    if (mode >= STATUS_BT_POWER_OFF_BUTT) {
        ux_log_err("[ux]set_bt_power_off_mode invalid mode[%u]!", mode);
        return;
    }

    g_system_info.state.bt_power_off_mode = mode;
}

void default_nv_value_init(void)
{
    g_system_info.nv_content.touch_config = DEFAULT_TOUCH_CONFIG;
    g_system_info.nv_content.service_time.calling_time = DEFAULT_SERVICE_TIME_CALLING;
    g_system_info.nv_content.audio_aid_mix = DEFAULT_AUDIO_AID_MIX;
    g_system_info.nv_content.time_switch_aidmode = DEFAULT_TIME_SWITCH_AIDMODE;
    g_system_info.nv_content.service_time.audio_time = DEFAULT_SERVICE_TIME_AUDIO;
    g_system_info.nv_content.service_time.config_time = DEFAULT_SERVICE_TIME_CONFIG;
    g_system_info.nv_content.button_config.long_button = STATUS_BUTTON_CONFIG_VALID;
    g_system_info.nv_content.button_config.short_button = STATUS_BUTTON_CONFIG_VALID;
    g_system_info.nv_content.tone_enable_status = STATUS_TONE_ENABLE_INVALID;
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    g_system_info.nv_content.wear_control_status = WEAR_CONTROL_SENSOR;
    g_system_info.nv_content.wear_detection_switch = UX_API_TWS_WEAR_DETECTION_ON;
    g_system_info.nv_content.low_latency_switch = UX_API_TWS_LOW_LATENCY_OFF;
    g_system_info.nv_content.dual_connection_switch = UX_API_TWS_DUAL_CONNECTION_OFF;
    g_system_info.nv_content.delay_wear_sensor_aidmode = DEFAULT_DELAY_WEAR_AIDMODE;
    g_system_info.nv_content.delay_wear_in_aidmode = DEFAULT_DELAY_WEAR_AIDMODE;
    g_system_info.state.haid_enable_state = STATUS_HAID_ENABLED;
#endif
}

// 上电和关盒前会执行 ux_system_manager_init 函数
static void ux_system_manager_init(void)
{
    init_service_time();
    set_device_pair_state(STATUS_DEVICE_PAIR_DISCONNECT);
    set_bt_device_pair_state(STATUS_DEVICE_PAIR_DISCONNECT);
    set_sle_device_pair_state(STATUS_DEVICE_PAIR_DISCONNECT);
    set_device_disconnect_reason(DISCONN_CODE_BUTT);
    set_bt_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_sle_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_combo_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_ble_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_device_reconn_type(RECONN_DEVICE_BY_LOCAL);
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_call_state(CALL_IDLE);
    set_haid_state(HAID_IDLE);
    init_device_reconn_times();
    init_device_pairing_failed_nums();
    set_slave_trigger_dev_pair_state(STATUS_SLAVE_DEVICE_PAIR_NO_TRIGGER); // 开机按键强配初始未触发
    set_peer_ear_state(STATUS_PEER_EAR_DISCONNECT);
    set_peer_ear_spp_state(STATUS_PEER_EAR_SPP_DISCONNECT);
    set_peer_device_pair_state(STATUS_DEVICE_PAIR_DISCONNECT);
    set_peer_bt_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_peer_sle_pairing_state(STATUS_DEVICE_PAIRING_INIT);
    set_peer_device_reconn_type(RECONN_DEVICE_BY_LOCAL);
    set_power_on_haid_delay_state(false);
    set_wear_in_haid_delay_state(false);
    set_device_reconnect_state(false);
    update_tone_enable_status();
    update_tws_ear_side();
    update_tws_pair_state();
#ifdef CONFIG_SUPPORT_WEAR_DETECTION
    update_wear_detection_switch();
#endif
#ifdef CONFIG_SUPPORT_DUAL_CONNECTION
    update_dual_connection_switch();
#endif
#endif
#if defined(MELODY_DEMO_BMIC_TX)
    set_recorder_state(STATUS_RECORDER_STOP);
#endif
}

// 开盒时会执行 ux_device_power_on_init 函数
void ux_device_power_on_init(void)
{
    /* 开机读取NV值 */
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_device_acc_on_state(false);
    update_touch_config_status();
    update_control_wearing_status();
    set_power_on_haid_delay_state(false);
    set_wear_in_haid_delay_state(false);
    set_device_reconnect_state(false);
    set_switch_pairing_state(false);
#ifdef SUPPORT_UX_HAID
    update_delay_wear_sensor_aidmode();
    update_delay_wear_in_aidmode();
    update_audio_aid_mix();
    update_delay_open_aidmode();
    update_time_switch_aidmode();
    update_scene_mode_status();
#endif
    power_on_notifica_state_init();
#endif
#ifdef CONFIG_SUPPORT_TONE
    ux_audio_set_tone_forbidden_state(false);
#endif
    update_service_time();
    update_button_config();
    set_all_working_state(STATUS_ON_WORKING_DELAY);
    set_lowbattery_state(STATUS_LOW_BATTERY_BUTT);
    power_on_notifica_state_init();
    ux_power_set_off_reason(UX_API_POWER_OFF_NORMAL);
    set_bt_power_off_mode(STATUS_BT_POWER_OFF_WITH_BOX);

#ifdef CONFIG_SUPPORT_SOURCE_MANAGER_FOCUS_MODE
    update_play_mode();
#endif

#ifdef CONFIG_SUPPORT_JACK
    /* 3.5mm耳机组件 初始化 */
    if (ux_jack_init() != ERRCODE_SUCC) {
        ux_log_err("[ux][ux_jack] init failed!\r\n");
    }
#endif
}

static void power_off_working_state_init(void)
{
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_bt_working_state(STATUS_OFF_WORKING_DELAY);
    set_haid_working_state(STATUS_OFF_WORKING_DELAY);
#endif
    if (get_working_state() != STATUS_ON_OTA) {
        set_working_state(STATUS_OFF_WORKING_DELAY);
        return;
    }
}

void ux_device_power_off_init(void)
{
    /* 关机写NV值 */
    write_service_time_to_nv();
    ux_system_manager_init();
    power_off_notifica_state_init();
    power_off_working_state_init();
#ifndef MELODY_DEMO_EARPHONE
#ifdef CONFIG_SUPPORT_JACK
    /* 3.5mm耳机组件 初始化 */
    if (ux_jack_deinit() != ERRCODE_SUCC) {
        ux_log_err("[ux][ux_jack] deinit failed!\r\n");
    }
#endif
#endif
}


void ux_device_state_init(void)
{
    working_state_init();
    /* 初始化NV默认值 */
    default_nv_value_init();
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
    set_wearing_status(STATUS_SENSOR_WEAR_INVALID);
#else
    ux_power_set_state(UX_POWER_NORMAL);
#endif
}

#if defined(MELODY_DEMO_BMIC_TX)
uint8_t get_recorder_state(void)
{
    return g_system_info.state.recorder_state;
}

void set_recorder_state(uint8_t state)
{
    g_system_info.state.recorder_state = state;
}
#endif

void set_battery_percent(uint8_t state)
{
    if (state > BATTERY_INFO_MAX_VALUE) {
        ux_log_err("[ux][set_battery_percent] invalid state:[%u]", state);
        return;
    }
    g_system_info.local_info.battery_percent = state;
}

uint8_t get_battery_percent(void)
{
    return g_system_info.local_info.battery_percent;
}

#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
call_state_t get_call_state(void)
{
    return (call_state_t)g_system_info.state.call_state;
}

haid_state_t get_haid_state(void)
{
    return (haid_state_t)g_system_info.state.haid_state;
}

void set_haid_state(haid_state_t state)
{
    if (state >= HAID_STATE_BUTT) {
        ux_log_err("[ux][set_haid_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.haid_state = (uint8_t)state;
}

void set_peer_charge_state(uint8_t state)
{
    g_system_info.peer_info.charge_state = state;
}

uint8_t get_peer_charge_state(void)
{
    return g_system_info.peer_info.charge_state;
}

void set_peer_battery_percent(uint8_t percent)
{
    if (percent > BATTERY_INFO_MAX_VALUE) {
        ux_log_err("[ux][set_peer_battery_percent] invalid state:[%u]", percent);
        return;
    }
    g_system_info.peer_info.battery_percent = percent;
}

uint8_t get_peer_battery_percent(void)
{
    return g_system_info.peer_info.battery_percent;
}

/* 获取设备BLE广播扩展type */
uint8_t ux_get_ble_expand_type(void)
{
    return BLE_ADV_EXPAND_TYPE_VAL;
}

uint8_t get_peer_ear_state(void)
{
    return g_system_info.pair_state.peer_pair_state;
}

void set_peer_ear_state(uint8_t state)
{
    if (state >= STATUS_PEER_EAR_BUTT) {
        ux_log_err("[ux][set_peer_ear_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_pair_state = state;
}

uint32_t get_led_key(uint8_t event_type, uint8_t event_id, uint16_t light_flag)
{
    unused(event_type);
    unused(event_id);
    unused(light_flag);
    return 0;
}

uint8_t get_slave_trigger_dev_pair_state(void)
{
    return g_system_info.pair_state.slave_trigger_dev_pair_state;
}

uint8_t get_device_pairing_failed_nums(void)
{
    return g_system_info.pair_state.pairing_failed_nums;
}

uint8_t get_device_reconn_times(void)
{
    return g_device_reconn_times;
}

void init_device_reconn_duration(void)
{
    g_system_info.pair_state.device_reconn_duration = 0;
}

void set_device_reconn_duration(uint32_t duration)
{
    g_system_info.pair_state.device_reconn_duration = duration;
}

uint32_t get_device_reconn_duration(void)
{
    return g_system_info.pair_state.device_reconn_duration;
}

void set_device_reconnect_state(bool state)
{
    g_system_info.state.device_reconn_state = state;
}

bool is_device_reconnect_state(void)
{
    return g_system_info.state.device_reconn_state;
}

void set_switch_pairing_state(bool state)
{
    g_system_info.state.switch_pairing_state = state;
}

bool get_switch_pairing_state(void)
{
    return g_system_info.state.switch_pairing_state;
}

void set_slave_trigger_dev_pair_state(uint8_t state)
{
    if (state >= STATUS_SLAVE_DEVICE_PAIR_BUTT) {
        ux_log_err("[ux][device][set_slave_trigger_dev_pair_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.slave_trigger_dev_pair_state = state;
}

uint8_t get_peer_ear_spp_state(void)
{
    return g_system_info.pair_state.peer_spp_state;
}

void set_peer_ear_spp_state(uint8_t state)
{
    if (state >= STATUS_PEER_EAR_SPP_BUTT) {
        ux_log_err("[ux][device][set_peer_ear_spp_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_spp_state = state;
}

void init_device_pairing_failed_nums(void)
{
    g_system_info.pair_state.pairing_failed_nums = 0;
}

void record_device_pairing_failed_nums(void)
{
    g_system_info.pair_state.pairing_failed_nums++;
}

void set_device_pairing_failed_nums(uint8_t nums)
{
    g_system_info.pair_state.pairing_failed_nums = nums;
}

void init_peer_pair_nums(void)
{
    g_system_info.pair_state.peer_pair_nums = 0;
}

void record_peer_pair_nums(void)
{
    g_system_info.pair_state.peer_pair_nums++;
}

uint8_t get_peer_pair_nums(void)
{
    return g_system_info.pair_state.peer_pair_nums;
}

void record_device_reconn_times(void)
{
    g_device_reconn_times++;
}

void init_device_reconn_times(void)
{
    g_device_reconn_times = 0;
}

bool is_sound_state(void)
{
    call_state_t state = get_call_state();
    if ((state == INCOMING_STATE) || (state == DIALING_STATE) || (state == ANSWER_STATE) ||
        (state == PLAY_MUSIC_STATE)) {
        return true;
    }

    return false;
}

uint8_t get_peer_device_pair_state(void)
{
    return g_system_info.pair_state.peer_device_pair_state;
}

void set_peer_device_pair_state(uint8_t state)
{
    if (state >= STATUS_DEVICE_PAIR_BUTT) {
        ux_log_err("[ux][device][set_peer_device_pair_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.pair_state.peer_device_pair_state = state;
}

void set_peer_device_connect_nums(uint8_t num)
{
    if (num > BS_CONNECT_NUM_MAX) { /* 双链接最大连接个数为2 */
        ux_log_err("[ux][device][set_peer_device_connect_nums] invalid num:[%u]", num);
        return;
    }
    g_system_info.pair_state.peer_device_connect_nums = num;
}

uint8_t get_peer_device_connect_nums(void)
{
    return g_system_info.pair_state.peer_device_connect_nums;
}

uint8_t get_device_connect_nums(void)
{
    return ux_multiple_get_connect_num();
}

void set_bt_standby_state(uint8_t state)
{
    if (state > STATUS_BT_STANDBY_BUTT) {
        ux_log_err("[ux][device][set_bt_standby_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.bt_standby_state = state;
}

uint8_t get_bt_standby_state(void)
{
    return g_system_info.state.bt_standby_state;
}

uint8_t get_tws_role_state(void)
{
    return g_system_info.local_info.tws_role;
}

void set_tws_role_state(uint8_t state)
{
    if ((state != UX_ROLE_SLAVE) && (state != UX_ROLE_MASTER)) {
        ux_log_err("[ux][device][set_tws_role_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.local_info.tws_role = state;
}

uint8_t get_device_disconnect_reason(void)
{
    return g_system_info.pair_state.device_disconnect_reason;
}

uint8_t get_bt_power_off_mode(void)
{
    return g_system_info.state.bt_power_off_mode;
}

void set_power_on_haid_delay_state(bool state)
{
    g_system_info.state.power_on_haid_delay_state = state;
}

uint8_t get_call_sound_state(void)
{
    return g_system_info.state.call_sound_state;
}

uint8_t get_audio_aid_mix(void)
{
    return g_system_info.nv_content.audio_aid_mix;
}

void set_call_state(call_state_t state)
{
    if (state >= CALL_STATE_BUTT) {
        ux_log_err("[ux][device][set_call_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.call_state = (uint8_t)state;
}

bool is_wear_in_haid_delay_state(void)
{
    return g_system_info.state.wear_in_haid_delay_state;
}

void record_calling_time(void)
{
    if (g_calling_service_time == 0) {
        g_calling_service_time += uapi_systick_get_s();
        return;
    }
    g_system_info.nv_content.service_time.calling_time +=
        (uint32_t)(uapi_systick_get_s() - g_calling_service_time);
    g_calling_service_time = 0;
}

void set_tone_state(uint8_t state)
{
    if (state >= NOTIFICA_WORK_STATE_BUTT) {
        ux_log_err("[ux][device][set_led_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.notifica_state.tone_state = state;
}

uint8_t get_tone_state(void)
{
    return g_system_info.state.notifica_state.tone_state;
}

bool is_power_on_haid_delay_state(void)
{
    return g_system_info.state.power_on_haid_delay_state;
}

void update_time_switch_aidmode(void)
{
    uint16_t time_switch_aidmode;
    uint8_t *p_ret = property_get_bytes(NV_ID_HAID_SWITCH_DELAY, (uint8_t*)&time_switch_aidmode, sizeof(uint16_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_time_switch_aidmode] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.time_switch_aidmode = time_switch_aidmode;
}

uint16_t get_time_switch_aidmode(void)
{
    return g_system_info.nv_content.time_switch_aidmode;
}

void set_wear_in_haid_delay_state(bool state)
{
    g_system_info.state.wear_in_haid_delay_state = state;
}

void set_bt_init_state(bool state)
{
    g_system_info.state.bt_init_state = state;
}

void record_audio_time(void)
{
    if (g_audio_service_time == 0) {
        g_audio_service_time += uapi_systick_get_s();
        return;
    }
    g_system_info.nv_content.service_time.audio_time += (uint32_t)(uapi_systick_get_s() - g_audio_service_time);
    g_audio_service_time = 0;
}

void set_mute_state(mute_state_t state)
{
    g_system_info.state.mute_state = (uint8_t)state;
}

void set_wearing_status(uint8_t state)
{
    if (state >= STATUS_SENSOR_WEAR_BUTT) {
        ux_log_err("[ux][device][set_wearing_status] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.wearing_status = state;
}

uint8_t get_wearing_status(void)
{
    return g_system_info.state.wearing_status;
}
 
mute_state_t get_mute_state(void)
{
    return (mute_state_t)g_system_info.state.mute_state;
}

void set_recover_type(uint8_t type)
{
    g_system_info.recover_info.recover_type = type;
}

uint8_t get_recover_type(void)
{
    return g_system_info.recover_info.recover_type;
}

bool set_haid_enable_state(uint8_t state)
{
    if ((state != STATUS_HAID_ENABLED) && (state != STATUS_HAID_DISABLED))  {
        ux_log_err("[ux][device]set_haid_enable_state invalid state[%u]!", state);
        return false;
    }

    g_system_info.state.haid_enable_state = state;
    return true;
}

uint8_t get_haid_enable_state(void)
{
    return g_system_info.state.haid_enable_state;
}

void set_call_sound_state(uint8_t state)
{
    if (state >= STATUS_CALL_SOUND_BUTT) {
        ux_log_err("[ux][device][set_call_sound_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.call_sound_state = state;
}
/* 获取设备BLE 名称限制长度 */
uint8_t ux_get_ble_name_max_len(void)
{
    uint8_t default_max_len = BLE_ADV_DATA_LEN_MAX - BLE_ADV_DATA_OFFEST; // 32 -2 = 29
    if (default_max_len < BLE_DEV_NAME_MAX_LEN) {
        return default_max_len;
    }
    return BLE_DEV_NAME_MAX_LEN;
}

/* 获取设备BLE广播扩展内容限制长度 */
uint8_t ux_get_ble_expand_val_max_len(void)
{
    uint8_t default_max_len = BLE_ADV_DATA_LEN_MAX - BLE_ADV_DATA_OFFEST - BLE_ADV_EXPAND_END_VAL_LEN;
    if (default_max_len < BLE_EXPAND_VAL_MAX_LEN) {
        return default_max_len;
    }
    return BLE_EXPAND_VAL_MAX_LEN;
}

/* 获取设备BLE广播扩展结束字符 */
uint8_t ux_get_ble_expand_end_val(void)
{
    return BLE_ADV_EXPAND_END_VAL;
}

uint8_t get_power_on_mode(void)
{
    return g_system_info.state.power_mode.ux_power_on_mode;
}

void set_ble_conn_state(uint8_t state)
{
    if (state >= BLE_CONN_BUTT) {
        ux_log_err("[ux][device][set_ble_conn_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.ble_conn_state = state;
}

uint8_t get_ble_conn_state(void)
{
    return g_system_info.state.ble_conn_state;
}

void update_delay_wear_in_aidmode(void)
{
    uint16_t delay_wear_in_aidmode;
    uint8_t *p_ret = property_get_bytes(NV_ID_WEAR_IN_DELAY, (uint8_t*)&delay_wear_in_aidmode, sizeof(uint16_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_delay_wear_in_aidmode]property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.delay_wear_in_aidmode = delay_wear_in_aidmode;
    ux_log_debug("[ux][device][update_delay_wear_in_aidmode] delay_wear_sensor_aidmode:[%d]",
        g_system_info.nv_content.delay_wear_in_aidmode);
}

uint16_t get_delay_wear_aidmode(void)
{
    if (get_wear_control_state() == WEAR_CONTROL_SENSOR) {
        return g_system_info.nv_content.delay_wear_sensor_aidmode;
    } else if (get_wear_control_state() == WEAR_CONTROL_IN) {
        return g_system_info.nv_content.delay_wear_in_aidmode;
    }
    return DEFAULT_WEAR_AIDMODE;
}

bool is_set_haid_afc_calibrate_valid(uint8_t calibrate_time)
{
    /* 时间校验预留 */
    if ((calibrate_time < AFC_MIN_TIME) || (calibrate_time > AFC_MAX_TIME)) {
        ux_log_err("[ux][haid][protocol]is_set_haid_afc_calibrate_valid, invlaid time: %u", calibrate_time);
        return false;
    }

    haid_state_t haid_state = get_haid_state();
    if ((haid_state == HAID_MEDICAL_ENTER) || (haid_state == HAID_MEDICAL_DOING) ||
        (haid_state == HAID_MEDICAL_DONE) || (haid_state == HAID_AFC_CALIBRATE)) {
        ux_log_err("[ux][haid][protocol]is_set_haid_afc_calibrate_valid, invlaid haid state: %d", haid_state);
        return false;
    }

    if (is_sound_state()) {
        ux_log_err("[ux][haid][protocol]is_set_haid_afc_calibrate_valid, invlaid call state: %d", get_call_state());
        return false;
    }

    uint8_t wear_state = get_wearing_status();
    if (wear_state != STATUS_SENSOR_WEAR_IN) {
        ux_log_err("[ux][haid][protocol]is_set_haid_afc_calibrate_valid, invlaid wear state state: %d", wear_state);
        return false;
    }

    return true;
}

uint8_t get_led_state(void)
{
    return g_system_info.state.notifica_state.led_state;
}

void set_haid_working_state(uint8_t state)
{
    if (state >= STATUS_WORKING_BUTT) {
        ux_log_err("[ux][device][set_haid_working_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.state.work_state.ux_haid_state = state;
}

uint8_t get_haid_working_state(void)
{
    return g_system_info.state.work_state.ux_haid_state;
}

void update_control_wearing_status(void)
{
    uint8_t wear_control_status;
    uint8_t *p_ret = property_get_bytes(NV_ID_CONTROL_WEAR_STATUS, &wear_control_status, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_control_wearing_status] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.wear_control_status = wear_control_status;
}

uint8_t get_wear_control_state(void)
{
    return g_system_info.nv_content.wear_control_status;
}

void set_box_charge_state(uint8_t percent)
{
    g_system_info.box_info.charge_state = percent;
}

uint8_t get_box_charge_state(void)
{
    return g_system_info.box_info.charge_state;
}

void update_delay_wear_sensor_aidmode(void)
{
    uint16_t delay_wear_sensor_aidmode;
    uint8_t *p_ret = property_get_bytes(NV_ID_WEAR_SENSOR_DELAY, (uint8_t*)&delay_wear_sensor_aidmode,
                                        sizeof(uint16_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_delay_wear_sensor_aidmode]property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.delay_wear_sensor_aidmode = delay_wear_sensor_aidmode;
}

void update_delay_open_aidmode(void)
{
    uint16_t delay_open_aidmode;
    uint8_t *p_ret = property_get_bytes(NV_ID_POWER_ON_DELAY, (uint8_t*)&delay_open_aidmode, sizeof(uint16_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_delay_open_aidmode] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.delay_open_aidmode = delay_open_aidmode;
}

void update_audio_aid_mix(void)
{
    uint8_t audio_aid_mix;
    uint8_t *p_ret = property_get_bytes(NV_ID_HAID_MIX_SWITCH, &audio_aid_mix, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_audio_aid_mix] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.audio_aid_mix = audio_aid_mix;
}

void record_config_time(void)
{
    if (g_config_service_time == 0) {
        g_config_service_time += uapi_systick_get_s();
        return;
    }
    g_system_info.nv_content.service_time.config_time +=
        (uint32_t)(uapi_systick_get_s() - g_config_service_time);
    g_config_service_time = 0;
}

void update_touch_config_status(void)
{
    uint8_t touch_config;
    uint8_t *p_ret = property_get_bytes(NV_ID_TOUCH_CONFIG, &touch_config, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device][update_touch_config_status] property_get_bytes failed!");
        return;
    }

    g_system_info.nv_content.touch_config = touch_config;
}

void update_tone_enable_status(void)
{
    uint8_t enable_status;
    uint8_t *p_ret = property_get_bytes(NV_ID_TONE_ENABLE_STATUS, &enable_status, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device] get NV_ID_TONE_ENABLED_STATUS failed!");
        return;
    }
    g_system_info.nv_content.tone_enable_status = enable_status;
}

uint16_t get_scene_mode_state(void)
{
    return g_system_info.nv_content.scene_mode_status;
}

errcode_t set_wear_control_state(uint8_t state)
{
    if (state >= WEAR_CONTROL_BUTT) {
        ux_log_err("[ux][device][set_wear_control_state] invalid state：[%u]", state);
        return ERRCODE_INVALID_PARAM;
    }

    errcode_t err = property_set_bytes(NV_ID_CONTROL_WEAR_STATUS, &state, sizeof(uint8_t));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][device][set_wear_control_state] property_set_bytes failed! err：[%x]", err);
        return err;
    }
    uint8_t nv_type = APP_CONTROL_WEAR_STATUS;
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
}

uint8_t get_tone_enable_status(void)
{
    return g_system_info.nv_content.tone_enable_status;
}

#ifdef CONFIG_SUPPORT_WEAR_DETECTION
uint8_t get_wear_detection_switch(void)
{
    return g_system_info.nv_content.wear_detection_switch;
}

static errcode_t write_wear_detection_switch_to_nv(uint8_t new_switch)
{
    if (new_switch >= UX_API_TWS_WEAR_DETECTION_BUTT) {
        ux_log_err("[ux][device]invalid wear_detection_switch[%u]", new_switch);
        return ERRCODE_INVALID_PARAM;
    }
    if (new_switch == get_wear_detection_switch()) {
        ux_log_info("[ux][device]same wear_detection_switch[%u]", new_switch);
        return ERRCODE_SUCC;
    }
    errcode_t err = property_set_non_upgrade_bytes(NV_ID_UX_WEAR_DETECTION_SWITCH, (uint8_t *)&new_switch,
                                                   sizeof(new_switch));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][device][set_wear_detection_switch] property_set_bytes failed! err:[%x]", err);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t set_wear_detection_switch(uint8_t new_switch)
{
    errcode_t ret = write_wear_detection_switch_to_nv(new_switch);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint8_t nv_type = APP_WEAR_DETECTION_STATUS;
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
    return ERRCODE_SUCC;
}

errcode_t set_local_wear_detection_switch(uint8_t new_switch)
{
    errcode_t ret = write_wear_detection_switch_to_nv(new_switch);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    g_system_info.nv_content.wear_detection_switch = new_switch;
    return ERRCODE_SUCC;
}

void update_wear_detection_switch(void)
{
    uint8_t new_switch = UX_API_TWS_WEAR_DETECTION_ON;
    uint8_t *p_ret = property_get_bytes(NV_ID_UX_WEAR_DETECTION_SWITCH, &new_switch, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device]get WEAR_DETECTION_SWITCH failed!");
        return;
    }
    if (new_switch == get_wear_detection_switch()) {
        ux_log_info("[ux][device]same wear_detection:%u", new_switch);
        return;
    }
    g_system_info.nv_content.wear_detection_switch = new_switch;
    if (!is_bt_init_state()) {
        return;
    }
    ux_bs_manager_switch_wear_detection();
}
#endif

#ifdef CONFIG_SUPPORT_LOW_LATENCY
void init_low_latency_switch(void)
{
    g_system_info.nv_content.low_latency_switch = 0;
}

uint8_t get_low_latency_switch(void)
{
    return g_system_info.nv_content.low_latency_switch;
}

errcode_t set_low_latency_switch(uint8_t new_switch)
{
    if (new_switch >= UX_API_TWS_LOW_LATENCY_BUTT) {
        ux_log_err("[ux][device][set_low_latency_switch] invalid new_switch[%u]", new_switch);
        return ERRCODE_INVALID_PARAM;
    }
    g_system_info.nv_content.low_latency_switch = new_switch;

    uint8_t nv_type = APP_LOW_LATENCY_STATUS;
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
    return ERRCODE_SUCC;
}

void update_low_latency_switch(void)
{
    ux_log_debug("[ux][device]update_low_latency:%u", g_system_info.nv_content.low_latency_switch);
    ux_bs_manager_switch_low_latency();
}
#endif

void update_bs_device_name(void)
{
    ux_log_debug("[ux][device]update_bs_device_name");
    ux_bs_manager_config_device_name();
}

uint8_t get_dual_connection_switch(void)
{
#ifdef CONFIG_SUPPORT_DUAL_CONNECTION
    return g_system_info.nv_content.dual_connection_switch;
#else
    return (uint8_t)UX_API_TWS_DUAL_CONNECTION_OFF;
#endif
}

#ifdef CONFIG_SUPPORT_DUAL_CONNECTION
static errcode_t write_dual_connection_switch_to_nv(uint8_t new_switch)
{
    if (new_switch >= UX_API_TWS_DUAL_CONNECTION_BUTT) {
        ux_log_err("[ux][device][set_dual_connection_switch] invalid new_switch[%u]", new_switch);
        return ERRCODE_INVALID_PARAM;
    }
    if (new_switch == get_dual_connection_switch()) {
        ux_log_info("[ux][device][set_dual_connection_switch] same new_switch[%u]", new_switch);
        return ERRCODE_SUCC;
    }
    errcode_t err = property_set_non_upgrade_bytes(NV_ID_UX_DUAL_CONNECTION_SWITCH,
        (uint8_t *)&new_switch, sizeof(new_switch));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][device][set_dual_connection_switch] property_set_bytes failed! err：[%x]", err);
        return ERRCODE_FAIL;
    }
    return ERRCODE_SUCC;
}

errcode_t set_dual_connection_switch(uint8_t new_switch)
{
    errcode_t ret = write_dual_connection_switch_to_nv(new_switch);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    uint8_t nv_type = APP_DUAL_CONNECTION_STATUS;
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
    return ERRCODE_SUCC;
}

errcode_t set_local_dual_connection_switch(uint8_t new_switch)
{
    errcode_t ret = write_dual_connection_switch_to_nv(new_switch);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    g_system_info.nv_content.dual_connection_switch = new_switch;
    return ERRCODE_SUCC;
}

void update_dual_connection_switch(void)
{
    uint8_t new_switch = UX_API_TWS_DUAL_CONNECTION_OFF;
    uint8_t *p_ret = property_get_bytes(NV_ID_UX_DUAL_CONNECTION_SWITCH, &new_switch, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device] get NV_ID_UX_DUAL_CONNECTION_SWITCH failed!");
        return;
    }
    if (new_switch == get_dual_connection_switch()) {
        ux_log_info("[ux][device]same dual_connection[%u]", new_switch);
        return;
    }
    g_system_info.nv_content.dual_connection_switch = new_switch;
    if (!is_bt_init_state()) {
        return;
    }
    ux_bs_manager_switch_dual_connection();
}
#endif

void set_device_position_state(uint8_t state)
{
    if (state >= STATUS_BOX_BUTT) {
        ux_log_err("[ux][device][set_device_position_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.local_info.position = state;
}

void set_cover_state(uint8_t state)
{
    if (state >= STATUS_COVER_BUTT) {
        ux_log_err("[ux][device][set_cover_state] invalid state:[%u]", state);
        return;
    }
    g_system_info.box_info.cover_state = state;
}

uint8_t get_cover_state(void)
{
    return g_system_info.box_info.cover_state;
}

uint8_t get_ux_bg_pdl_num(void)
{
    return ux_bt_common_get_pdl_num();
}

void set_connect_dongle_state(bool connect_dongle)
{
    if (connect_dongle != true && connect_dongle != false) {
        ux_log_err("[ux][device][set_connect_dongle] invalid state:[%u]", connect_dongle);
        return;
    }
    g_system_info.connect_dongle = connect_dongle;
}

bool get_connect_dongle_state(void)
{
    return g_system_info.connect_dongle;
}

uint8_t get_tws_pair_state(void)
{
    return g_system_info.local_info.tws_pair_state;
}

errcode_t set_tws_pair_state(uint8_t state)
{
    if (state > STATUS_TWS_PAIR_VALID) {
        ux_log_err("[ux][device][set_tws_pair_state] invalid state[%u]", state);
        return ERRCODE_INVALID_PARAM;
    }
#ifdef CONFIG_SUPPORT_PEER_DISCOVER_FEATURE
    errcode_t err = property_set_non_upgrade_bytes(NV_ID_UX_TWS_PAIR_CONFIG, (uint8_t *)&state, sizeof(uint8_t));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][device][set_tws_pair_state] property_set_bytes failed! err:[%x]", err);
        return ERRCODE_FAIL;
    }
#endif
    update_tws_pair_state();
    return ERRCODE_SUCC;
}

void update_tws_pair_state(void)
{
    uint8_t state = 0;
#ifdef CONFIG_SUPPORT_PEER_DISCOVER_FEATURE
    uint8_t *p_ret = property_get_bytes(NV_ID_UX_TWS_PAIR_CONFIG, &state, sizeof(uint8_t));
    if (p_ret == NULL) {
        ux_log_err("[ux][device] get NV_ID_UX_TWS_PAIR_CONFIG failed!");
        return;
    }
#endif
    g_system_info.local_info.tws_pair_state = state;
}

static void set_tws_role_state_init(void)
{
    if (get_tws_ear_side() == LEFT_EAR) {
        set_tws_role_state(UX_ROLE_SLAVE);
    } else if (get_tws_ear_side() == RIGHT_EAR) {
        set_tws_role_state(UX_ROLE_MASTER);
    }
}

uint8_t get_tws_ear_side(void)
{
    return g_system_info.local_info.tws_ear_side;
}

errcode_t set_tws_ear_side(uint8_t ear_side)
{
    if (ear_side < LEFT_EAR || ear_side > RIGHT_EAR) {
        ux_log_err("[ux][device]invalid ear_side[%u]", ear_side);
        return ERRCODE_INVALID_PARAM;
    }
    if (ear_side == get_tws_ear_side()) {
        ux_log_info("[ux][device]same ear_side[%u]", ear_side);
        return ERRCODE_SUCC;
    }
    errcode_t ret = property_set_int(NV_ID_UX_EAR_SIDE, ear_side);
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][device]ear_side property_set_int failed! ret:[%x]", ret);
        return ERRCODE_FAIL;
    }
    uint8_t nv_type = APP_SET_EAR_SIDE;
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_APP_INFO, &nv_type, sizeof(uint8_t));
    return ERRCODE_SUCC;
}

void update_tws_ear_side(void)
{
    uint8_t ear_side = (uint8_t)property_get_int(NV_ID_UX_EAR_SIDE, CONFIG_DEFAULT_EARSIDE);
    if (ear_side == get_tws_ear_side()) {
        ux_log_info("[ux][device][update_tws_ear_side] same ear_side[%d]", ear_side);
        return;
    }
    g_system_info.local_info.tws_ear_side = ear_side;
    if (get_device_acc_on_state()) {
        set_tws_role_state_init();
    }
}

void device_acc_on_init(void)
{
    ux_system_manager_init();
    set_connected_protocol_type(UX_WORK_PROTOCOL_MAX);
    set_all_working_state(STATUS_OFF_WORKING);
    set_device_position_state(STATUS_BOX_IN);
    set_bt_standby_state(STATUS_BT_STANDBY_BUTT);
    set_lowbattery_state(STATUS_LOW_BATTERY_BUTT);
    set_cover_state(STATUS_COVER_CLOSE);
    set_call_sound_state(STATUS_PHONE_SOUND_ACTIVE);
    set_bt_init_state(false);
    set_ble_conn_state(BLE_DISCONN_STATE);
    set_connect_dongle_state(false);
    set_tws_role_state_init();
    set_device_acc_on_state(true);
#ifdef CONFIG_SUPPORT_LOW_LATENCY
    init_low_latency_switch();
#endif
}
#endif

void set_device_acc_on_state(bool is_acc_on)
{
    g_system_info.state.acc_on_state = is_acc_on;
}

bool get_device_acc_on_state(void)
{
    return g_system_info.state.acc_on_state;
}

#ifdef SUPPORT_UX_HAID
bool write_haid_enable_state_to_nv(uint8_t state)
{
    if (!set_haid_enable_state(state)) {
        ux_log_err("[ux][earphone]write_haid_enable_state_to_nv invalid state[%u]!", state);
        return false;
    }

    errcode_t err = property_set_non_upgrade_bytes(NV_ID_HAID_ENABLE_SWITCH, &state, sizeof(uint8_t));
    if (err != ERRCODE_SUCC) {
        ux_log_err("[ux][earphone][write_haid_enable_state_to_nv] uapi_nv_write failed! err:[%x]", err);
        return false;
    }

    ux_log_debug("[ux][earphone][write_haid_enable_state_to_nv]success! state[%u]", state);
    return true;
}
#endif

void ux_system_info_restore(void)
{
    ux_audio_info_restore();
    ux_system_cfg_restore_transmit_mode();
    ux_power_info_restore();
#ifdef MELODY_DEMO_BMIC_RX
    ux_logic_mgr_info_restore();
#endif
}

ux_api_sysmgr_pair_state_t ux_api_sysmgr_get_peer_pair_state(void)
{
    return (ux_api_sysmgr_pair_state_t)get_peer_ear_state();
}
 
ux_api_sysmgr_role_t ux_api_sysmgr_get_device_role(void)
{
    return (ux_api_sysmgr_role_t)get_tws_role_state();
}

ux_api_sysmgr_working_state_t ux_api_sysmgr_get_working_state(void)
{
    return (ux_api_sysmgr_working_state_t)get_working_state();
}
 
bool ux_api_sysmgr_is_off_working_state(void)
{
    return is_off_working_state();
}
 
errcode_t ux_api_sysmgr_set_working_state(ux_api_sysmgr_working_state_t state)
{
    set_working_state((uint8_t)state);
    return UX_ERR_SUCCESS;
}