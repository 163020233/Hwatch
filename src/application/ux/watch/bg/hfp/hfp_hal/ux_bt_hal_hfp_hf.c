/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 *
 * Description:
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-09-11， Create file. \n
 */
#include "ux_types.h"
#include "bts_def.h"
#include "bs_br_call.h"
#include "bs_br_service_api.h"
#include "bs_device_mgr.h"
#include "ux_event_define.h"
#include "securec.h"
#include "ux_bt_call_hf.h"
#include "ux_common_log.h"
#include "adapter_bt_br.h"
#include "ux_bt_hal_hfp_hf.h"
#define BTH_IDX    0
#define UX_HAL_IDX 1

#ifndef unused
#define unused(x) ((void)(x))
#endif

uint8_t g_call_control_map[] = {
    [UX_API_BT_CONTROL_CALL_DAILING] = BR_CALL_REQUEST_OUT,
    [UX_API_BT_CONTROL_CALL_ANSWER]  = BR_CALL_REQUEST_ACCEPT,
    [UX_API_BT_CONTROL_CALL_HANGUP]  = BR_CALL_REQUEST_FINISH,
    [UX_API_BT_CONTROL_CALL_REJECT]  = BR_CALL_REQUEST_REJECT,
    [UX_API_BT_CONTROL_CALL_HOLD]    = BR_CALL_REQUEST_HOLD,
    [UX_API_BT_CONTROL_CALL_RECORD]  = BR_CALL_REQUEST_CALL_INFO,
};

uint8_t g_bth_hfp_connect_state_mapping_to_ux[][2] = {
    {(uint8_t)BR_PROFILE_STATE_CONNECTING,    (uint8_t)UX_BT_HAL_HFP_STATE_CONNECTING},
    {(uint8_t)BR_PROFILE_STATE_CONNECTED,     (uint8_t)UX_BT_HAL_HFP_STATE_CONNECTED},
    {(uint8_t)BR_PROFILE_STATE_DISCONNECTING, (uint8_t)UX_BT_HAL_HFP_STATE_DISCONNECTING},
    {(uint8_t)BR_PROFILE_STATE_DISCONNECTED,  (uint8_t)UX_BT_HAL_HFP_STATE_DISCONNECTED},
};

uint8_t g_bth_ag_call_state_mapping_to_ux[][2] = {
    {(uint8_t)BR_CALL_STATE_ACTIVE,        (uint8_t)UX_BT_HAL_CALL_STATE_ACTIVE},
    {(uint8_t)BR_CALL_STATE_HELD,          (uint8_t)UX_BT_HAL_CALL_STATE_HELD},
    {(uint8_t)BR_CALL_STATE_DIALING,       (uint8_t)UX_BT_HAL_CALL_STATE_DIALING},
    {(uint8_t)BR_CALL_STATE_ALERTING,      (uint8_t)UX_BT_HAL_CALL_STATE_ALERTING},
    {(uint8_t)BR_CALL_STATE_INCOMING,      (uint8_t)UX_BT_HAL_CALL_STATE_INCOMING},
    {(uint8_t)BR_CALL_STATE_WAITING,       (uint8_t)UX_BT_HAL_CALL_STATE_WAITING},
    {(uint8_t)BR_CALL_STATE_RESPONSE_HELD, (uint8_t)UX_BT_HAL_CALL_STATE_RESPONSE_HELD},
    {(uint8_t)BR_CALL_STATE_FINISHED,      (uint8_t)UX_BT_HAL_CALL_STATE_FINISHED},
};
ux_bt_hal_hf_callbacks_t *g_hf_notice = NULL;

errcode_t ux_bt_hfp_register(ux_bt_hal_hfp_role_t hfp_role)
{
    if (hfp_role >= UX_BT_HAL_HFP_ROLE_MAX || hfp_role < UX_BT_HAL_HFP_ROLE_HF) {
        return UX_ERR_INVALID_PARAMETER;
    }
    bs_br_call_hfp_role_t role = hfp_role == UX_BT_HAL_HFP_ROLE_HF ? BR_CALL_HFP_HF : BR_CALL_HFP_AG;
    return bs_br_call_hfp_register((uint8_t)role);
}

errcode_t  ux_bt_hfp_unregister(ux_bt_hal_hfp_role_t hfp_role)
{
    if (hfp_role >= UX_BT_HAL_HFP_ROLE_MAX || hfp_role < UX_BT_HAL_HFP_ROLE_HF) {
        return UX_ERR_INVALID_PARAMETER;
    }
    bs_br_call_hfp_role_t role = hfp_role == UX_BT_HAL_HFP_ROLE_HF ? BR_CALL_HFP_HF : BR_CALL_HFP_AG;
    return bs_br_call_hfp_unregister((uint8_t)role);
}

/* 通话操作，可以考虑封装成对外接口供native层调用 */
errcode_t ux_bt_hal_call_operate_request(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_call_info_t *call_control)
{
    if (addr == NULL || call_control == NULL || call_control->request >= arry_num(g_call_control_map)) {
        ux_log_err("[ux_bt_hal_call_operate_request] invalid param.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    bs_br_call_opt_type_t bth_req = g_call_control_map[call_control->request];
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, addr->type, addr->addr);
    return bs_br_call_operate_request(dev_id, (uint8_t)bth_req, call_control->param, call_control->param_len);
}

/* hfp建立链接，这个其实没必要对外暴露接口了，ux自己消化就好了，做好状态管理在已连接上就建好连接 */
errcode_t ux_bt_call_hfp_connect(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t role)
{
    if (addr == NULL) {
        return UX_ERR_NULL_PTR;
    }
    if (role >= UX_BT_HAL_HFP_ROLE_MAX || role < UX_BT_HAL_HFP_ROLE_HF) {
        return UX_ERR_INVALID_PARAMETER;
    }
    bs_br_call_hfp_role_t hfp_role = role == UX_BT_HAL_HFP_ROLE_HF ? BR_CALL_HFP_HF : BR_CALL_HFP_AG;
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, (uint8_t)addr->type, addr->addr);

    return bs_br_call_hfp_connect(dev_id, (uint8_t)hfp_role);
}

/* hfp建立链接，这个其实没必要对外暴露接口了，ux自己消化就好了，做好状态管理在已连接上就建好连？还是再电话来得时候建立？ 后者 */
errcode_t ux_bt_call_hfp_disconnect(const ux_api_bt_addr_t *addr, ux_bt_hal_hfp_role_t role)
{
    if (addr == NULL) {
        return UX_ERR_NULL_PTR;
    }
    if (role >= UX_BT_HAL_HFP_ROLE_MAX || role < UX_BT_HAL_HFP_ROLE_HF) {
        return UX_ERR_INVALID_PARAMETER;
    }
    bs_br_call_hfp_role_t hfp_role = role == UX_BT_HAL_HFP_ROLE_HF ? BR_CALL_HFP_HF : BR_CALL_HFP_AG;
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, (uint8_t)addr->type, addr->addr);
    return bs_br_call_hfp_disconnect(dev_id, (uint8_t)hfp_role);
}

static errcode_t set_hfp_volume(uint16_t dev_id, bs_br_call_volume_type_t volume_type,
    ux_api_bt_control_volume_t control, uint8_t *cur_volume, uint8_t new_volume)
{
    bool is_mute = false;
    errcode_t err = UX_ERR_SUCCESS;
    switch (control) {
        case UX_API_BT_CONTROL_VOLUME_UP:
            if (*cur_volume + SENSOR_TRIGGER_VOLUME_STEP <= HFP_VOLUME_MAX) {
                new_volume = *cur_volume + SENSOR_TRIGGER_VOLUME_STEP;
            } else {
                new_volume = HFP_VOLUME_MAX;
            }
            break;
        case UX_API_BT_CONTROL_VOLUME_DOWN:
            if (*cur_volume - SENSOR_TRIGGER_VOLUME_STEP >= HFP_VOLUME_MIN) {
                new_volume = *cur_volume - SENSOR_TRIGGER_VOLUME_STEP;
            } else {
                new_volume = HFP_VOLUME_MIN;
            }
            break;
        case UX_API_BT_CONTROL_VOLUME_MUTE:
            new_volume = HFP_VOLUME_MIN;
            is_mute = true;
            break;
        case UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE:
            if (new_volume > HFP_VOLUME_MAX) {
                new_volume = HFP_VOLUME_MAX;
            }
            break;
        default:
            return UX_ERR_INVALID_PARAMETER;
    }
    if (!is_mute) {
        *cur_volume = new_volume;
    }
    /* 同步设置本地音量 */
    if (volume_type == BR_CALL_VOLUME_SPEAKER) {
        err = ux_audio_call_set_speaker_volume(new_volume);
        if (err != UX_ERR_SUCCESS) {
            ux_log_err("[ux_bt_call_set_volume_request] Fail to set speaker volume 0x%x.\n", err);
            return err;
        }
    }
    return bs_br_call_set_volume_request(dev_id, (uint8_t)volume_type, new_volume);
}

/* 封装成对外接口，做好+1  -1  */
errcode_t ux_bt_call_set_volume_request(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info)
{
    if (addr == NULL || volume_control_info == NULL) {
        return UX_ERR_NULL_PTR;
    }
    ux_api_bt_volume_type_t volume_type = volume_control_info->volume_type;
    uint8_t new_volume = volume_control_info->volume;
    ux_api_bt_control_volume_t control = volume_control_info->control;
    uint16_t dev_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, addr->type, addr->addr);
    ux_bt_call_info_t *call_info = ux_bt_call_get_info();

    /**
     * 根据音量类型(麦克风或喇叭)传参
     */
    switch (volume_type) {
        case UX_API_BT_VOLUME_TYPE_MIC:
            return set_hfp_volume(dev_id, BR_CALL_VOLUME_MIC, control, &call_info->mic_volume, new_volume);
        case UX_API_BT_VOLUME_TYPE_SPEAKER:
            return set_hfp_volume(dev_id, BR_CALL_VOLUME_SPEAKER, control, &call_info->speaker_volume, new_volume);
        default:
            return UX_ERR_INVALID_PARAMETER;
            break;
    }
}

static void normalize_hfp_connect_state_cb(uint16_t dev_id, uint8_t hfp_role, uint8_t conn_state)
{
    ux_log_info("[ux][bt][normalize_hfp_connect_state_cb] hfp_role[%d], conn_state[%d]", hfp_role, conn_state);
    if (hfp_role < BR_CALL_HFP_HF || hfp_role > BR_CALL_HFP_AG) {
        return;
    }

    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[normalize_hfp_connect_state_cb] Fail to get addr");
        return;
    }
    ux_bt_hal_hfp_role_t hal_role = hfp_role == BR_CALL_HFP_HF ? UX_BT_HAL_HFP_ROLE_HF : UX_BT_HAL_HFP_ROLE_AG;
    
    ux_bt_hal_hfp_connect_state_t hal_hfp_connect_state = UX_BT_HAL_HFP_STATE_MAX;
    for (uint8_t idx = 0; idx < arry_num(g_bth_hfp_connect_state_mapping_to_ux); idx++) {
        if (g_bth_hfp_connect_state_mapping_to_ux[idx][BTH_IDX] == conn_state) {
            hal_hfp_connect_state = g_bth_hfp_connect_state_mapping_to_ux[idx][UX_HAL_IDX];
        }
    }
    if (hal_hfp_connect_state == UX_BT_HAL_HFP_STATE_MAX) {
        return;
    }
    if (g_hf_notice == NULL || g_hf_notice->hfp_connect_state_cb == NULL) {
        return;
    }
    g_hf_notice->hfp_connect_state_cb(addr, hal_role, hal_hfp_connect_state);
}

static void normalize_set_call_volume_request_cb(uint16_t dev_id, uint8_t volume_type, uint8_t volume)
{
    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[normalize_set_call_volume_request_cb] Fail to get addr");
        return;
    }
    ux_log_info("[ux][bt][normalize_set_call_volume_request_cb] volume_type[%d], volume[%d]",
        volume_type, volume);

    ux_bt_hal_volume_info_t volume_info = {0};
    volume_info.volume = volume;

    if ((bs_br_call_volume_type_t)volume_type == BR_CALL_VOLUME_MIC) {
        volume_info.volume_type = UX_API_BT_VOLUME_TYPE_MIC;
    } else if ((bs_br_call_volume_type_t)volume_type == BR_CALL_VOLUME_SPEAKER) {
        volume_info.volume_type = UX_API_BT_VOLUME_TYPE_SPEAKER;
    }
    if (g_hf_notice == NULL || g_hf_notice->set_volume_request_cb == NULL) {
        return;
    }
    g_hf_notice->set_volume_request_cb(addr, &volume_info);
}

static void normalize_hfp_ag_call_state_cb(uint16_t dev_id, bs_br_call_info_t *call_info)
{
    if (call_info == NULL) {
        return;
    }
    ux_log_info("[ux][bt][normalize_hfp_ag_call_state_cb] state[%d], outgoing[%d]",
        call_info->state, call_info->outgoing);

    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[normalize_hfp_ag_call_state_cb] Fail to get addr");
        return;
    }
    ux_bt_hal_call_cat1_info_t ag_call_state = {
        .id = 0,
        .multi_party = 0,
    };
    int err = EOK;
    ag_call_state.number_len = call_info->number_len;
    err = memcpy_s(ag_call_state.number, arry_num(ag_call_state.number), call_info->number, call_info->number_len);
    if (err != EOK) {
        return;
    }
    for (uint8_t i = 0; i < arry_num(g_bth_ag_call_state_mapping_to_ux); i++) {
        if (g_bth_ag_call_state_mapping_to_ux[i][BTH_IDX] == call_info->state) {
            ag_call_state.state = g_bth_ag_call_state_mapping_to_ux[i][UX_HAL_IDX];
        }
    }
    ag_call_state.outgoing = call_info->outgoing;
    if (g_hf_notice == NULL || g_hf_notice->cat1_state_from_ag_cb == NULL) {
        return;
    }
    g_hf_notice->cat1_state_from_ag_cb(addr, &ag_call_state);
    /* call state change事件进一步通知上层app */
    ux_adapter_bt_call_state_process((bd_addr_t *)addr, (uint8_t)(call_info->state),
        (char *)(call_info->number), call_info->number_len, call_info->outgoing);
}

static void normalize_call_ring_cb(uint16_t dev_id, bool is_band_in, uint8_t ring_status)
{
    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[normalize_call_ring_cb] Fail to get addr");
        return;
    }
    // 电话响铃, 根据is_band_in标记判断是播放带内铃声还是带外本地提示音
    if (!is_band_in) {
        // 带外的情况，播放/停止本地提示音
        if (ring_status == BR_CALL_INCOMING_RING_GOING) {
            ux_log_info("[normalize_call_ring_cb] out_band ring start!");
        } else {
            ux_log_info("[normalize_call_ring_cb] out_band ring end!");
        }
    } else {
        // 带内铃声情况下，等待手机侧建立sco通路即可，手表端会响应sco请求并建立通路播放带内铃声
        if (ring_status == BR_CALL_INCOMING_RING_GOING) {
            ux_log_info("[normalize_call_ring_cb] in_band ring start!");
        } else {
            ux_log_info("[normalize_call_ring_cb] in_band ring end!");
        }
    }

    if (g_hf_notice == NULL || g_hf_notice->call_ring_cb == NULL) {
        return;
    }
    g_hf_notice->call_ring_cb(addr, is_band_in, ring_status);
}

/* 不需要对外暴露，内部消化的回调注册 */
errcode_t ux_bt_hal_hfp_hf_register_callbacks(uint8_t app_id, ux_bt_hal_hf_callbacks_t *func)
{
    if (func == NULL) {
        return UX_ERR_NULL_PTR;
    }
    g_hf_notice = func;
    static bs_br_call_callbacks_t hf_cb = {
        .connect_state_cbk = normalize_hfp_connect_state_cb,
        .set_volume_request_cbk = normalize_set_call_volume_request_cb,
        .call_status_notify_cbk = normalize_hfp_ag_call_state_cb,
        .band_ring_tone_changed_cbk = normalize_call_ring_cb,
    };
    return bs_br_call_register_callbacks(app_id, &hf_cb);
}
errcode_t ux_bt_hal_hfp_hf_unregister_callbacks(uint8_t app_id)
{
    g_hf_notice = NULL;
    return bs_br_call_unregister_callbacks(app_id);
}