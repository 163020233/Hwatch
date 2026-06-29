/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "ux_audio_a2dp_sink.h"
#include "ux_bt_a2dp_sink.h"
#include "ux_common_log.h"
#include "ux_api_bt_audio.h"
#include "ux_bt_audio.h"
#include "bg_a2dp_sink_service.h"
void bg_a2dp_sink_service_profile_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_a2dp_sink_service_profile_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_a2dp_info_t)) {
        ux_log_err("[bg_a2dp_sink_service_profile_state_change] Invalid len.\n");
        return;
    }
    ux_bt_a2dp_info_t *profile_state_package = (ux_bt_a2dp_info_t*)param;
    ux_bt_a2dp_info_t *sink_info = ux_bt_a2dp_get_sink_info();
    ux_api_bt_profile_state_t new_profile_state = profile_state_package->profile_state;
    /**
     * 规格约束，从设备可以和多个主设备配对，但只能同时与1个手机建立连接
     * 作为sink端，连接起点为connected，在connected时，更新基本信息
     */
    if (new_profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        sink_info->a2dp_role = profile_state_package->a2dp_role;
        sink_info->device_id = profile_state_package->device_id;
        sink_info->profile_state = profile_state_package->profile_state;
        sink_info->stream_state = UX_API_BT_STREAM_STATE_STOPPED;
        return;
    } else {
        /**
         * 非完成连接状态，需要校验dev_id和role
         * 若非正在连接的状态触发的回调，过滤
         */
        if (profile_state_package->a2dp_role != sink_info->a2dp_role ||
            profile_state_package->device_id != sink_info->device_id) {
            ux_log_err("[bg_a2dp_sink_service_profile_state_change] Device no match.\n");
            return;
        }
        sink_info->profile_state = profile_state_package->profile_state;
    }
    return;
}

void bg_a2dp_sink_service_codec_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_a2dp_sink_service_codec_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_a2dp_info_t)) {
        ux_log_err("[bg_a2dp_sink_service_codec_change] Invalid len.\n");
        return;
    }
    ux_bt_a2dp_info_t *codec_package = (ux_bt_a2dp_info_t*)param;
    ux_bt_a2dp_info_t *sink_info = ux_bt_a2dp_get_sink_info();
    sink_info->codec = codec_package->codec;
    ux_audio_a2dp_sink_set_param(&sink_info->codec);
}

void bg_a2dp_sink_service_stream_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_a2dp_sink_service_stream_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_a2dp_info_t)) {
        ux_log_err("[bg_a2dp_sink_service_stream_state_change] Invalid len.\n");
        return;
    }
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_a2dp_info_t *stream_state_package = (ux_bt_a2dp_info_t*)param;
    ux_bt_a2dp_info_t *sink_info = ux_bt_a2dp_get_sink_info();
    // 更新流状态
    ux_api_bt_stream_state_t old_state = sink_info->stream_state;
    ux_api_bt_stream_state_t new_state = stream_state_package->stream_state;
    if (new_state == UX_API_BT_STREAM_STATE_STREAMING) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, sink_info->device_id,
            UX_BT_STREAM_FSM_INPUT_START_CB, &(sink_info->stream_state));
    } else if (new_state == UX_API_BT_STREAM_STATE_STOPPED) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, sink_info->device_id,
            UX_BT_STREAM_FSM_INPUT_STOP_CB, &(sink_info->stream_state));
    }
    if (new_state == UX_API_BT_STREAM_STATE_STARTING) {
        err = ux_audio_a2dp_sink_start();
        if (err != UX_ERR_SUCCESS) {
            return;
        }
        bool accept = true;
        err = ux_bt_hal_a2dp_req_reply(accept);
        // 若成功创建音频通路, 但接受请求失败，则销毁音频通路
        if (err != UX_ERR_SUCCESS) {
            ux_audio_a2dp_sink_stop();
            return;
        }
        return;
    } else if (new_state == UX_API_BT_STREAM_STATE_STOPPED) {
        ux_audio_a2dp_sink_stop();
    }
    ux_log_err("[bg_a2dp_sink_service_stream_state_change] State trans err, old state = %d, new state = %d.\n",
        old_state, new_state);
    return;
}