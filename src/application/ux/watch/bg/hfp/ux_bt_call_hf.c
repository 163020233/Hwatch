/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "bts_def.h"
#include "securec.h"
#include "ux_common_log.h"
#include "adapter_bt_br.h"
#include "ux_bt_hfp_hf_basic_event.h"
#include "ux_bt_hfp_hf_event.h"
#include "ux_bt_call_hf.h"

ux_bt_call_info_t g_ux_bt_call_info = {0};

ux_bt_call_info_t* ux_bt_call_get_info(void)
{
    return &g_ux_bt_call_info;
}

void ux_bt_hfp_hf_conn_state_cbk(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t hfp_role,
    ux_bt_hal_hfp_connect_state_t conn_state)
{
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    errno_t err = memcpy_s(&call_info->addr, sizeof(call_info->addr), addr, sizeof(ux_api_bt_addr_t));
    call_info->role = hfp_role;
    call_info->hfp_state = conn_state;
    call_info->sco_state = UX_API_BT_STREAM_STATE_STOPPED;
    if (err != EOK) {
        ux_log_err("[ux_bt_hfp_hf_conn_state_cbk] Fail to memcpy addr 0x%x.", err);
        return;
    }

    /* HFP状态变化事件进一步通知上层app */
    ux_adapter_bt_hfp_state_process((bd_addr_t *)addr, (uint8_t)conn_state);
}

errcode_t ux_bt_hf_init(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    err = ux_bt_hfp_register(UX_BT_HAL_HFP_ROLE_HF);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_hf_init]Fail to register hfp service 0x%x.", err);
        return err;
    }
    static ux_bt_hal_sco_callbacks_t sco_cb = {
        .ux_bt_hal_sco_state_cbk = ux_bt_hfp_hf_sco_state_cbk,
        .ux_bt_hal_sco_codec_cbk = ux_bt_hfp_hf_sco_codec_cbk,
    };

    /* sco的回调函数，两边是不一样的 */
    err = ux_bt_hal_sco_register_callbacks(0, &sco_cb);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_hf_init]Fail to register sco cb 0x%x.", err);
        return err;
    }

    static ux_bt_hal_hf_callbacks_t hfp_hf_cbs = {
        .hfp_connect_state_cb = ux_bt_hfp_hf_conn_state_cbk,
        .cat1_state_from_ag_cb = ux_bt_hfp_hf_cat1_state_cbk,
        .call_ring_cb = NULL,
        .set_volume_request_cb = ux_bt_hfp_hf_set_volume_request_cbk,
    };
    return ux_bt_hal_hfp_hf_register_callbacks(0, &hfp_hf_cbs);
}

errcode_t ux_bt_hf_deinit(void)
{
    errcode_t err = UX_ERR_SUCCESS;
    err = ux_bt_hfp_unregister(UX_BT_HAL_HFP_ROLE_HF);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_hf_deinit]Fail to unregister hfp service 0x%x.", err);
        return err;
    }
    err = ux_bt_hal_sco_unregister_callbacks(0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_hf_deinit]Fail to unregister sco cb 0x%x.", err);
        return err;
    }
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    if (call_info->hfp_state == UX_BT_HAL_HFP_STATE_CONNECTED) {
        ux_bt_hfp_hf_conn_state_cbk(&call_info->addr, call_info->role, UX_BT_HAL_HFP_STATE_DISCONNECTED);
    }
    return ux_bt_hal_hfp_hf_unregister_callbacks(0);
}

ux_api_bt_audio_info_t* ux_bt_call_get_audio_info(const ux_api_bt_addr_t *addr)
{
    static ux_api_bt_audio_info_t call_audio_info = {0};
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    /* sco的profile state和 stream state合并 */
    switch (call_info->sco_state) {
        case UX_API_BT_STREAM_STATE_STARTING :
            call_audio_info.profile_state = UX_API_BT_PROFILE_STATE_CONNECTING;
            break;
        case UX_API_BT_STREAM_STATE_STREAMING :
            call_audio_info.profile_state = UX_API_BT_PROFILE_STATE_CONNECTED;
            break;
        case UX_API_BT_STREAM_STATE_STOPPING :
            call_audio_info.profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTING;
            break;
        case UX_API_BT_STREAM_STATE_STOPPED :
            call_audio_info.profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
            break;
        default:
            call_audio_info.profile_state = UX_API_BT_PROFILE_STATE_MAX;
            break;
    }
    call_audio_info.stream_state = call_info->sco_state;
    return &call_audio_info;
}

ux_api_bt_control_info_t* ux_bt_call_get_control_info(const ux_api_bt_addr_t *addr)
{
    static ux_api_bt_control_info_t call_control_info = {0};
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();
    if (call_info->hfp_state == UX_BT_HAL_HFP_STATE_CONNECTED) {
        call_control_info.profile_state = UX_API_BT_PROFILE_STATE_CONNECTED;
    } else {
        call_control_info.profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTED;
    }
    call_control_info.is_mic_mute = call_info->mic_is_mute;
    call_control_info.mic_volume = call_info->mic_volume;
    call_control_info.is_speaker_mute = call_info->speaker_is_mute;
    call_control_info.speaker_volume = call_info->speaker_volume;
    return &call_control_info;
}