/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "string.h"
#include "crypto_security.h"
#include "ux_bt_call_hf.h"
#include "audio_manager_service.h"
#include "ux_common_log.h"
#include "ux_event_define.h"
#include "ux_fwk_sync_process.h"
#include "ux_fwk_msg_def.h"
#include "ux_audio_hfp_hf.h"
#include "ux_bt_hfp_hf_event.h"
#include "ux_bt_hfp_hf_basic_event.h"
#include "ux_bt_audio.h"
#include "common_def.h"
#include "ux_errcode.h"

typedef struct {
    ux_bt_hal_call_state_t cat1_state_enum;
    const char* cat1_state_name;
} cat1_state_enum_str_pair_t;

static cat1_state_enum_str_pair_t g_cat1_state_enum_str_map[] = {
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_ACTIVE,        .cat1_state_name = "active"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_HELD,          .cat1_state_name = "held"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_DIALING,       .cat1_state_name = "dialing"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_ALERTING,      .cat1_state_name = "alerting"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_INCOMING,      .cat1_state_name = "incoming"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_WAITING,       .cat1_state_name = "waiting"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_RESPONSE_HELD, .cat1_state_name = "response_held"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_FINISHED,      .cat1_state_name = "finished"},
    {.cat1_state_enum = UX_BT_HAL_CALL_STATE_MAX,           .cat1_state_name = "unknow"},
};

static bool addr_is_change(const ux_api_bt_addr_t *addr)
{
    if (memcmp(&ux_bt_call_get_info()->addr, addr, sizeof(ux_api_bt_addr_t)) != 0) {
        return true;
    }
    return false;
}

static bool role_is_change(ux_bt_hal_hfp_role_t role)
{
    if (ux_bt_call_get_info()->role == role) {
        return false;
    }
    return true;
}

void bg_hfp_hf_service_sco_state_change(uint8_t *param, uint16_t param_len)
{
    if ((param == NULL) || (param_len != sizeof(ux_bt_call_sco_state_cb_packet_t))) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_state_change] null param or invalid len, len = %d", param_len);
        return;
    }

    ux_bt_call_sco_state_cb_packet_t *sco_state_info = (ux_bt_call_sco_state_cb_packet_t*)param;
    if (role_is_change(sco_state_info->role)) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_state_change] Role change.");
        return;
    }
    if (addr_is_change(&sco_state_info->addr)) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_state_change] Addr change.");
        return;
    }
    
    ux_log_info("[ux][bg_hfp_hf_service_sco_state_change] Sco state change to %d.\n", sco_state_info->sco_state);
    ux_bt_call_info_t *call_info =  ux_bt_call_get_info();
    if (sco_state_info->sco_state == UX_API_BT_STREAM_STATE_STREAMING) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_SCO, call_info->dev_id,
            UX_BT_STREAM_FSM_INPUT_START_CB, &(call_info->sco_state));
    } else if (sco_state_info->sco_state == UX_API_BT_STREAM_STATE_STOPPED) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_SCO, call_info->dev_id,
            UX_BT_STREAM_FSM_INPUT_STOP_CB, &(call_info->sco_state));
    }
    /* 接受sco建立请求 */
    int32_t bt_sco_volume = 0;
    switch (sco_state_info->sco_state) {
        case UX_API_BT_STREAM_STATE_STARTING:
            ux_bt_common_sco_start(sco_state_info->sco_state);
            break;
        case UX_API_BT_STREAM_STATE_STREAMING:
            // 从媒体获取手表通话音量，并同步到对端
            bt_sco_volume = AudioManagerGetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO) * HFP_VOLUME_MAX / MEDIA_VOLUME_MAX;
            ux_api_bt_control_volume_info_t volume_info = {
                .volume_type = UX_API_BT_VOLUME_TYPE_SPEAKER,
                .control = UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE,
                .volume = bt_sco_volume};
            ux_api_bt_control_volume(UX_API_BT_PROFILE_HFP, &sco_state_info->addr, &volume_info);
            break;
        case UX_API_BT_STREAM_STATE_STOPPED:
            ux_bt_common_sco_stop(sco_state_info->sco_state);
            break;
        default:
            break;
    }
}

void bg_hfp_hf_service_sco_codec_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_codec_change] Nullptr.");
        return;
    }
    if (param_len != sizeof(ux_bt_call_sco_codec_cb_packet_t)) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_codec_change] Invalid len %d.", param_len);
        return;
    }

    ux_bt_call_sco_codec_cb_packet_t *sco_codec_info = (ux_bt_call_sco_codec_cb_packet_t*)param;
    if (role_is_change(sco_codec_info->role)) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_codec_change] Role change.");
        return;
    }
    if (addr_is_change(&sco_codec_info->addr)) {
        ux_log_err("[ux][bg_hfp_hf_service_sco_codec_change] Addr change.");
        return;
    }
    
    ux_audio_sco_set_param(&sco_codec_info->sco_codec);
}

void bg_hfp_hf_service_cat1_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[ux][bg_hfp_hf_service_cat1_state_change] Nullptr.");
        return;
    }
    if (param_len != sizeof(ux_bt_call_cat1_state_packet_t)) {
        ux_log_err("[ux][bg_hfp_hf_service_cat1_state_change] Invalid len %d.", param_len);
        return;
    }
    
    ux_bt_call_cat1_state_packet_t *cat1_state_packet = (ux_bt_call_cat1_state_packet_t*)param;
    if (addr_is_change(&cat1_state_packet->addr)) {
        ux_log_err("[ux][bg_hfp_hf_service_cat1_state_change] Addr change.\n");
        return;
    }
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    /* 根据状态,确定是手机还是手表操作 */
    ux_bt_main_state_handle(call_info->cat1_info.state, cat1_state_packet->cat1_info.state);

    call_info->cat1_info = cat1_state_packet->cat1_info;
    for (uint8_t i = 0; i < arry_num(g_cat1_state_enum_str_map); i++) {
        if (g_cat1_state_enum_str_map[i].cat1_state_enum == call_info->cat1_info.state) {
            ux_log_info("^cat1 state change: %s", g_cat1_state_enum_str_map[i].cat1_state_name);
        }
    }
}

void bg_hfp_hf_service_volume_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_hfp_hf_service_volume_change] Nullptr.");
        return;
    }
    if (param_len != sizeof(ux_bt_call_volume_packet_t)) {
        ux_log_err("[bg_hfp_hf_service_volume_change] Invalid len %d", param_len);
        return;
    }
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_call_volume_packet_t *volume_packet = (ux_bt_call_volume_packet_t*)param;
    ux_bt_call_info_t *hf_info = ux_bt_call_get_info();
    if (volume_packet->volume_info.volume_type == UX_API_BT_VOLUME_TYPE_MIC) {
        hf_info->mic_volume = volume_packet->volume_info.volume;
    } else if (volume_packet->volume_info.volume_type == UX_API_BT_VOLUME_TYPE_SPEAKER) {
        hf_info->speaker_volume = volume_packet->volume_info.volume;
        err = ux_audio_call_set_speaker_volume(hf_info->speaker_volume);
        if (err != UX_ERR_SUCCESS) {
            ux_log_err("[bg_hfp_hf_service_volume_change] Fail to set speaker vol 0x%dx.", err);
            return;
        }
    }
}

/* 接听电话 */
void bg_hfp_hf_service_answer(uint8_t *param, uint16_t param_len)
{
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_call_info_t call_control = {0};
    call_control.request = UX_API_BT_CONTROL_CALL_ANSWER;
    ux_api_bt_control_call(UX_API_BT_PROFILE_HFP, &call_info->addr, &call_control);
}

/* 挂起电话 */
void bg_hfp_hf_service_hangup(uint8_t *param, uint16_t param_len)
{
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_call_info_t call_control = {0};
    call_control.request = UX_API_BT_CONTROL_CALL_HANGUP;
    ux_api_bt_control_call(UX_API_BT_PROFILE_HFP, &call_info->addr, &call_control);
}

/* 拒接电话 */
void bg_hfp_hf_service_reject(uint8_t *param, uint16_t param_len)
{
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_call_info_t call_control = {0};
    call_control.request = UX_API_BT_CONTROL_CALL_REJECT;
    ux_api_bt_control_call(UX_API_BT_PROFILE_HFP, &call_info->addr, &call_control);
}

/* 呼出电话 */
void bg_hfp_hf_service_dailing(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[ux][bg_hfp_hf_service_dailing] Nullptr.");
        return;
    }
    if (param_len > UX_BT_HAL_CALL_PHONENUM_MAX_DIGITS) {
        ux_log_err("[ux][bg_hfp_hf_service_dailing] Invalid len %d.", param_len);
        return;
    }
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_call_info_t call_control = {0};
    call_control.request = UX_API_BT_CONTROL_CALL_DAILING;
    call_control.param = param;
    call_control.param_len = param_len;
    ux_api_bt_control_call(UX_API_BT_PROFILE_HFP, &call_info->addr, &call_control);
}

void bg_hfp_hf_service_volume_up(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_volume_info_t volume_control = {0};

    volume_control.control = UX_API_BT_CONTROL_VOLUME_UP;
    volume_control.volume_type = UX_API_BT_VOLUME_TYPE_SPEAKER;
    
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_HFP, &call_info->addr, &volume_control);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_hfp_hf_service_volume_up] Fail to set vol up 0x%x.\n", err);
        return;
    }
}

void bg_hfp_hf_service_volume_down(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_volume_info_t volume_control = {0};

    volume_control.control = UX_API_BT_CONTROL_VOLUME_DOWN;
    volume_control.volume_type = UX_API_BT_VOLUME_TYPE_SPEAKER;
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_HFP, &call_info->addr, &volume_control);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_hfp_hf_service_volume_down] Fail to set vol down 0x%x.\n", err);
        return;
    }
}

void bg_hfp_hf_service_mute(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    ux_api_bt_control_volume_info_t volume_control = {0};

    volume_control.control = UX_API_BT_CONTROL_VOLUME_MUTE;
    volume_control.volume_type = UX_API_BT_VOLUME_TYPE_SPEAKER;
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_HFP, &call_info->addr, &volume_control);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[bg_hfp_hf_service_mute] Fail to set vol down 0x%x.\n", err);
        return;
    }
}