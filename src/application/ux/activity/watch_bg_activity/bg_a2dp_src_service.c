/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "ux_audio_a2dp_src.h"
#include "ux_bt_a2dp_src.h"
#include "ux_common_log.h"
#include "bs_device_mgr.h"
#include "ux_api_bt_audio.h"
#include "ux_event_define.h"
#include "ux_audio_a2dp.h"
#include "adapter_bt_br.h"
#include "ux_bt_audio.h"
#include "bg_a2dp_src_service.h"
#ifndef unused
#define unused(x) ((void)(x))
#endif
bool g_audio_stream_flag = false;

void bg_a2dp_src_service_profile_state_change(uint8_t *param, uint16_t param_len)
{
    ux_bt_a2dp_info_t *profile_state_package = (ux_bt_a2dp_info_t*)param;
    ux_bt_a2dp_info_t *src_info = ux_bt_a2dp_get_src_info();
    ux_api_bt_profile_state_t new_profile_state = profile_state_package->profile_state;
    /**
     * 规格约束，从设备可以和多个主设备配对，但只能同时与1个手机建立连接
     * 作为src端，连接起点为connected，在connected时，更新基本信息
     */
    if (new_profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        src_info->a2dp_role = profile_state_package->a2dp_role;
        src_info->device_id = profile_state_package->device_id;
        src_info->profile_state = profile_state_package->profile_state;
        src_info->stream_state = UX_API_BT_STREAM_STATE_STOPPED;
        /* 通知播放器连接 */
        ux_audio_notify_a2dp_profile_state(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_CONNECTED);
    } else {
        /**
         * 非完成连接状态，需要校验dev_id和role
         * 若非正在连接的状态触发的回调，过滤
         */
        if (profile_state_package->a2dp_role != src_info->a2dp_role ||
            profile_state_package->device_id != src_info->device_id) {
            ux_log_err("[bg_a2dp_src_service_profile_state_change] Device no match.\n");
            return;
        }
        if (profile_state_package->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTED &&
               ux_bt_a2dp_get_src_info()->device_id == profile_state_package->device_id) {
            /* 通知播放器断连 */
            ux_audio_notify_a2dp_profile_state(OUT_BLUETOOTH_A2DP, AUDIO_DEVICE_DISCONNECTED);
        }
        src_info->profile_state = profile_state_package->profile_state;
    }
    return;
}

void bg_a2dp_src_service_codec_change(uint8_t *param, uint16_t param_len)
{
    AudioA2dpCodecAttr *codec_cfg = (AudioA2dpCodecAttr*)param;
    ux_audio_a2dp_src_set_param(codec_cfg);
}

void bg_a2dp_src_service_stream_state_change(uint8_t *param, uint16_t param_len)
{
    /**
     * src端收到UX_BT_A2DP_STREAM_STATE_STARTING也不处理
     * 只接受avrcp的音频控制命令
     */
    if (param == NULL) {
        ux_log_err("[bg_a2dp_src_service_stream_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_a2dp_info_t)) {
        ux_log_err("[bg_a2dp_src_service_stream_state_change] Invalid len %d.\n", param_len);
        return;
    }
    ux_bt_a2dp_info_t *stream_state_package = (ux_bt_a2dp_info_t*)param;
    ux_bt_a2dp_info_t *src_info = ux_bt_a2dp_get_src_info();
    ux_api_bt_stream_state_t new_state = stream_state_package->stream_state;
    if (new_state == UX_API_BT_STREAM_STATE_STREAMING) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, src_info->device_id,
            UX_BT_STREAM_FSM_INPUT_START_CB, &(src_info->stream_state));
    } else if (new_state == UX_API_BT_STREAM_STATE_STOPPED) {
        ux_bt_stream_fsm_excute(UX_API_BT_PROFILE_A2DP, src_info->device_id,
            UX_BT_STREAM_FSM_INPUT_STOP_CB, &(src_info->stream_state));
    }
}

void bg_a2dp_src_service_start_stream(uint8_t *param, uint16_t param_len)
{
    unused(param_len);
    unused(param);
    ux_bt_a2dp_info_t *a2dp_info = ux_bt_a2dp_get_src_info();
    bs_addr_t *addr = bs_get_addr_by_device_id(a2dp_info->device_id);
    if (addr == NULL) {
        ux_log_err("[bg_a2dp_src_service_start_stream] Fail to get addr");
        return;
    }
    ux_api_bt_audio_start(UX_API_BT_PROFILE_A2DP, (ux_api_bt_addr_t*)addr);
}

void bg_a2dp_src_service_stop_stream(uint8_t *param, uint16_t param_len)
{
    unused(param_len);
    unused(param);
    ux_bt_a2dp_info_t *a2dp_info = ux_bt_a2dp_get_src_info();
    bs_addr_t *addr = bs_get_addr_by_device_id(a2dp_info->device_id);
    if (addr == NULL) {
        ux_log_err("[bg_a2dp_src_service_stop_stream] Fail to get addr");
        return;
    }
    ux_api_bt_audio_stop(UX_API_BT_PROFILE_A2DP, (ux_api_bt_addr_t*)addr);
}