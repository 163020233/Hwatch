/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved. \n
 *
 * Description: Provide ux cmd manager at service \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-02， Create file. \n
 */
#ifndef DEMO_WATCH
#include <string.h>
#include "securec.h"
#include "app_at_uart.h"
#include "audio_base_type.h"
#include "common_def.h"
#include "dut.h"
#include "nv_storage.h"
#include "property.h"
#include "watchdog.h"
#include "ux_at_manager.h"
#include "ux_audio_common.h"
#include "ux_api_audio_setting.h"
#include "ux_types.h"
#include "ux_common_log.h"
#include "ux_cmd_utils.h"
#include "ux_audio_utils.h"
#include "ux_api_audio_recorder.h"
#include "ux_api_audio_music.h"
#include "ux_api_audio_call.h"
#include "ux_status_define.h"
#include "ux_api_fwk.h"
#include "feature_config.h"
#include "ux_config.h"
#include "ux_api_bs_audio.h"
#include "ux_fwk_msg_def.h"

#ifdef MELODY_DEMO_BMIC_TX
#include "ux_system_manager.h"
#include "ux_at_api.h"
#include "ux_cmd_mic_tx.h"
#endif
#include "ux_api_audio_setting.h"
#include "ux_at_interface.h"
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
#include "ux_cmd_bt.h"
#include "ux_cmd_tws.h"
#endif
#ifdef CONFIG_SUPPORT_MUSIC_A2DP_FEATURE
#include "ux_state_common.h"
#include "ux_bt_common.h"
#endif
#ifdef CONFIG_SUPPORT_TONE
#include "tone_service.h"
#include "ux_api_audio_tone.h"
#include "bs_api_stru.h"
#endif
#include "ux_device_manager.h"
#include "ux_cmd_bs.h"
#include "ux_cmd_manager.h"
#endif

#include "ux_bg_config.h"
#if defined(MELODY_DEMO_HA)
#include "ux_cmd_haid.h"
#endif
#ifdef DEMO_WATCH
#include "ux_cmd_bt.h"
#endif
#include "ux_system_manager.h"
#if defined(MELODY_DEMO_SOUND_BOX)
#include "ux_input_passthrough.h"
#endif

#ifndef DEMO_WATCH
#define TIME_STAMP_LEN_MAX 12
#define DEV_NAME_STR_LEN (DEVICE_NAME_LEN * 2)
#define SNPRINTF_S_ERR_NUM (-1)

#define MIC_TYPE_0 0
#define MIC_TYPE_1 1
#define MIC_SPK_LOOP_MAX_VOL 20
#define MAX_LANGUAGE_NUM 10
#define NO_PAIR "no pair"
#define NO_ADDR "no addr"
#define NUM_INVALID "num is invalid"

bool g_recorder_status = false;

typedef errcode_t (*ux_haid_set_callback_t)(uint8_t *data, uint16_t data_len);

bool ux_cmd_check_device_id_valid(uint16_t device_id)
{
    if (device_id == UX_DEVICE_ID_DEFAULT) {
#if defined(MELODY_DEMO_BMIC_TX)
        return true;
#else
        return false;
#endif
    } else if (device_id >= UX_DEVICE_ID_TX_START && device_id < UX_DEVICE_ID_TX_END) {
        return true;
    } else if (device_id >= UX_DEVICE_ID_RX_START && device_id < UX_DEVICE_ID_RX_END) {
        return true;
    } else {
        return false;
    }
}
#ifdef CONFIG_SUPPORT_TONE
errcode_t ux_cmd_play_tone_by_id(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    char *p = NULL;
    char *tone = strtok_r((char*)data, "\r", &p);
    if (tone == NULL) {
        return ERRCODE_FAIL;
    }
    // 使用十进制解析，避免自动识别进制
    char *endptr = NULL;
    int32_t id = (int32_t)strtol(tone, &endptr, DECIMAL_BASE);
    // 检查解析是否失败
    if (endptr == tone || *endptr != '\0') {
        return ERRCODE_FAIL;
    }
    // 检查是否在有效范围内（非负，且不超过最大 ID）
    if (id < 0 || id >= ux_audio_get_tone_info_max()) {
        return ERRCODE_FAIL;
    }
    ux_api_audio_tone_play((uint8_t)id);
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_tone_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t tone_vol = (int32_t)strtol((char *)data, NULL, 0);
    if (tone_vol == ux_api_audio_tone_get_vol()) {
        return ERRCODE_SUCC;
    }
    return ux_api_audio_tone_set_vol(tone_vol);
}

errcode_t ux_cmd_set_tone_work_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t enable_flag = (int32_t)strtol((char *)data, NULL, 0);
    uint8_t work_state = (enable_flag == 0) ? STATUS_TONE_DISABLE : STATUS_TONE_ENABLE;
    return ux_audio_set_tone_play_work_state(work_state);
}

errcode_t ux_cmd_set_outband_ring_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t ring_state = (int32_t)strtol((char *)data, NULL, 0);
    uint16_t tone_id = UX_API_AUDIO_TONE_OUTBAND_RING_OFF;
    if (ring_state == (int32_t)RING_PLAY) {
        tone_id = UX_API_AUDIO_TONE_OUTBAND_RING_ON;
    }
    ux_audio_play_tone(tone_id);
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_tone_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t tone_vol = ux_audio_get_tone_vol();
    int32_t ret = snprintf_s((char *)data, data_len, data_len - 1, "%d", tone_vol);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}
#endif
errcode_t ux_cmd_set_mic_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t mic_vol = (int32_t)strtol((char *)data, NULL, 0);
    return ux_audio_set_mic_vol(mic_vol);
}

errcode_t ux_cmd_get_mic_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t mic_vol = ux_audio_get_mic_vol();
    int32_t ret = snprintf_s((char *)data, data_len, data_len - 1, "%d", mic_vol);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_max_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t max_vol = (int32_t)strtol((char *)data, NULL, 0);
    return ux_audio_set_max_vol(max_vol);
}

errcode_t ux_cmd_get_max_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t max_vol = ux_audio_get_max_vol();
    int32_t ret = snprintf_s((char *)data, data_len, data_len - 1, "%d", max_vol);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_set_db_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t vol = (int32_t)strtol((char *)data, NULL, 0);
    return ux_audio_set_db_vol(vol);
}

errcode_t ux_cmd_get_db_vol(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t db_vol = ux_audio_get_db_vol();
    int32_t ret = snprintf_s((char *)data, data_len, data_len - 1, "%d", db_vol);
    if (ret <= 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

/* AT 获取a2dp状态 */
errcode_t ux_cmd_get_a2dp_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t ret = 0;
#ifdef CONFIG_SUPPORT_MUSIC_A2DP_FEATURE
    ux_api_bs_music_state_t a2dp_state = ux_api_bs_get_music_state();
    if (a2dp_state == UX_API_BS_MUSIC_STATE_PLAYING) {
        ret = sprintf_s((char*)data, data_len, "%s", PLAY_MUSIC_TYPE);
    } else if (a2dp_state == UX_API_BS_MUSIC_STATE_PLAY) {
        ret = sprintf_s((char*)data, data_len, "%s", PLAY_MUSIC_TYPE);
    } else if (a2dp_state == UX_API_BS_MUSIC_STATE_PAUSED) {
        ret = sprintf_s((char*)data, data_len, "%s", PAUSE_MUSIC_TYPE);
    } else if (a2dp_state == UX_API_BS_MUSIC_STATE_STOPPED) {
        ret = sprintf_s((char*)data, data_len, "%s", STOP_MUSIC_TYPE);
    } else {
        ret = sprintf_s((char*)data, data_len, "%s", IDLE_TYPE);
    }
#endif

    return (ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
}

/* AT 获取hfp状态 */
errcode_t ux_cmd_get_hfp_state(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    int32_t ret = 0;
#ifdef CONFIG_SUPPORT_CALL_HFP_FEATURE
    ux_api_bs_call_state_t hfp_state = ux_api_bs_get_call_state();
    if (hfp_state == UX_API_BS_CALL_STATE_CONNECTED) {
        ret = sprintf_s((char*)data, data_len, "%s", CONNECTED);
    } else if (hfp_state == UX_API_BS_CALL_STATE_DISCONNECTED) {
        ret = sprintf_s((char*)data, data_len, "%s", DISCONNECTED);
    } else {
        ret = sprintf_s((char*)data, data_len, "%s", UX_UNKNOWN);
    }
#endif
    return (ret <= 0) ? ERRCODE_FAIL : ERRCODE_SUCC;
}

errcode_t ux_cmd_loopback_on(uint8_t *data, uint16_t data_len)
{
    if (data == NULL || data_len == 0) {
        return ERRCODE_FAIL;
    }

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_loopback_off(uint8_t *data, uint16_t data_len)
{
    UNUSED(data);
    UNUSED(data_len);

    return ERRCODE_SUCC;
}

errcode_t ux_cmd_get_avrcp_volume(uint8_t *data, uint16_t data_len)
{
    unused(data);
    unused(data_len);

    errcode_t ret = ERRCODE_SUCC;
#ifdef CONFIG_SUPPORT_MUSIC_A2DP_FEATURE
    if (data == NULL || data_len == 0) {
        return UX_ERR_INVALID_PARAMETER;
    }
    *data = (uint8_t)ux_get_audio_config()->avrcp_vol;
#endif
    return ret;
}

errcode_t ux_cmd_get_hfp_volume(uint8_t *data, uint16_t data_len)
{
    unused(data);
    unused(data_len);

    errcode_t ret = ERRCODE_SUCC;
#ifdef CONFIG_SUPPORT_CALL_HFP_FEATURE
    if (data == NULL || data_len == 0) {
        return UX_ERR_INVALID_PARAMETER;
    }
    *data = (uint8_t)ux_get_audio_config()->hfp_vol;
#endif
    return ret;
}

errcode_t ux_cmd_manager_wear_in(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    uint8_t status = STATUS_SENSOR_WEAR_IN;
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_WEAR_INFO, &status, sizeof(status));
}

errcode_t ux_cmd_manager_wear_out(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    uint8_t status = STATUS_SENSOR_WEAR_OUT;
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_WEAR_INFO, &status, sizeof(status));
}

errcode_t ux_cmd_manager_inbox(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    ux_event_msg_t event_msg = { 0 };
    event_msg.task_src = UX_BOX;
    event_msg.para[0] = POWER_OFF_WITH_INBOX;
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POSITION_INFO,
        (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_outbox(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);

    ux_event_msg_t event_msg = { 0 };
    event_msg.task_src = UX_BOX;
    event_msg.para[0] = POWER_OFF_WITH_INBOX;
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POSITION_INFO,
        (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_power_toggle(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POWER_BUTTON_KEY, data, data_len);
}

errcode_t ux_cmd_manager_suspend_bs(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    ux_event_msg_t event_msg = { .task_src = UX_MASTER, .para = { POWER_ON_WITH_COVER } };
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POWER_ON, (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_resume_bs(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    ux_event_msg_t event_msg = { .task_src = UX_MASTER, .para = { POWER_OFF_WITH_COVER } };
    return ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, UX_MASTER_POWER_OFF, (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_pair_bs(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    ux_log_info("[ux][cmd][ux_cmd_manager_pair_bs] IN\n");
    ux_event_msg_t event_msg = { .task_src = UX_BUTTON, .para = { STATUS_BUTTON_CONFIG_VALID } };
    return ux_api_fwk_push_event(UX_EVENT_TYPE_PAIRING, UX_PAIRING_CONN_DEVICE_REQ, (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_reconnect_bs(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    ux_log_info("[ux][cmd][ux_cmd_manager_reconnect_bs] IN\n");
    ux_event_msg_t event_msg = { .task_src = UX_BUTTON, .para = { STATUS_BUTTON_CONFIG_VALID } };
    return ux_api_fwk_push_event(UX_EVENT_TYPE_PAIRING, UX_PAIRING_RECONN_DEVICE, (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_manager_disconnect_bs(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
    ux_log_info("[ux][cmd][ux_cmd_manager_disconnect_bs] IN\n");
    ux_event_msg_t event_msg = { .task_src = UX_BUTTON, .para = { STATUS_BUTTON_CONFIG_VALID } };
    return ux_api_fwk_push_event(UX_EVENT_TYPE_PAIRING, UX_PAIRING_DISCONN_DEVICE, (uint8_t *)&event_msg, sizeof(event_msg));
}

errcode_t ux_cmd_uac_music_play_or_pause(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_SOUND_BOX)
    ux_log_info("[ux][cmd][ux_cmd_uac_music_play_or_pause] get_connect_dongle_state()=%d", get_connect_dongle_state());
    if (get_connect_dongle_state() == true) {
        ux_input_manager_button_event_passthrough(UX_INPUT_BUTTON1_CLICK_1);
    }
#endif
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_uac_music_prev(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_SOUND_BOX)
    ux_log_info("[ux][cmd][ux_cmd_uac_music_prev] get_connect_dongle_state()=%d", get_connect_dongle_state());
    if (get_connect_dongle_state() == true) {
        ux_input_manager_button_event_passthrough(UX_INPUT_BUTTON1_CLICK_2);
    }
#endif
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_uac_music_next(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_SOUND_BOX)
    ux_log_info("[ux][cmd][ux_cmd_uac_music_next] get_connect_dongle_state()=%d", get_connect_dongle_state());
    if (get_connect_dongle_state() == true) {
        ux_input_manager_button_event_passthrough(UX_INPUT_BUTTON2_CLICK_2);
    }
#endif
    return ERRCODE_SUCC;
}

errcode_t ux_cmd_uac_music_volume_up(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_SOUND_BOX)
    ux_log_info("[ux][cmd][ux_cmd_uac_music_volume_up] get_connect_dongle_state()=%d\r\n", get_connect_dongle_state());
    if (get_connect_dongle_state() == true) {
        ux_input_manager_button_event_passthrough(UX_INPUT_BUTTON1_CLICK_3);
    }
#endif
    return ERRCODE_SUCC;
}


errcode_t ux_cmd_uac_music_volume_down(uint16_t device_id, uint8_t *data, uint16_t data_len)
{
    unused(device_id);
    unused(data);
    unused(data_len);
#if defined(MELODY_DEMO_SOUND_BOX)
    ux_log_info("[ux][cmd][ux_cmd_uac_music_volume_down] get_connect_dongle_state()=%d\r\n", get_connect_dongle_state());
    if (get_connect_dongle_state() == true) {
        ux_input_manager_button_event_passthrough(UX_INPUT_BUTTON2_CLICK_3);
    }
#endif
    return ERRCODE_SUCC;
}

#endif

