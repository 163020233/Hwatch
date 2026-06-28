/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "bs_br_call.h"
#include "ux_types.h"
#include "ux_common_log.h"
#include "bs_br_media_control.h"
#include "ux_errcode.h"
#include "ux_api_fwk.h"
#include "ux_device_manager.h"
#include "bs_br_service_api.h"
#include "ux_event_define.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_bt.h"
#include "ux_bt_a2dp_sink.h"
#include "ux_bt_a2dp_src.h"
#include "ux_audio_a2dp_sink.h"
#include "adapter_bt_br.h"
#include "ux_bt_avrcp.h"

uint8_t g_mapping_ux_media_volume_control_request_to_bth[UX_API_BT_CONTROL_VOLUME_MAX] = {
    [UX_API_BT_CONTROL_VOLUME_UP] =           BR_MEDIA_VOLUME_OPT_UP,
    [UX_API_BT_CONTROL_VOLUME_DOWN] =         BR_MEDIA_VOLUME_OPT_DOWN,
    [UX_API_BT_CONTROL_VOLUME_MUTE] =         BR_MEDIA_VOLUME_OPT_MUTE,
    [UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE] = BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE,
};
uint8_t g_mapping_ux_media_play_state_to_bth[UX_BT_MEDIA_PLAY_STATE_MAX] = {
    [UX_BT_MEDIA_PLAY_STATE_STOPPED]   = BR_MEDIA_PLAY_STATUS_STOPPED,
    [UX_BT_MEDIA_PLAY_STATE_PLAYING]   = BR_MEDIA_PLAY_STATUS_PLAYING,
    [UX_BT_MEDIA_PLAY_STATE_PAUSED]    = BR_MEDIA_PLAY_STATUS_PAUSED,
    [UX_BT_MEDIA_PLAY_STATE_SEEK_NEXT] = BR_MEDIA_PLAY_STATUS_FWD_SEEK,
    [UX_BT_MEDIA_PLAY_STATE_SEEK_PRE]  = BR_MEDIA_PLAY_STATUS_REV_SEEK,
};

uint8_t g_mapping_ux_media_control_request_to_bth[UX_API_BT_CONTROL_PLAY_MAX] = {
    [UX_API_BT_CONTROL_PLAY_START]        = BR_MEDIA_OPT_PLAY,
    [UX_API_BT_CONTROL_PLAY_STOP]         = BR_MEDIA_OPT_STOP,
    [UX_API_BT_CONTROL_PLAY_PAUSE]        = BR_MEDIA_OPT_PAUSE,
    [UX_API_BT_CONTROL_PLAY_NEXT]         = BR_MEDIA_OPT_FORWARD,
    [UX_API_BT_CONTROL_PLAY_PRE]          = BR_MEDIA_OPT_BACKWARD,
    [UX_API_BT_CONTROL_PLAY_REWIND]       = BR_MEDIA_OPT_REWIND,
    [UX_API_BT_CONTROL_PLAY_FAST_FORWARD] = BR_MEDIA_OPT_FAST_FORWARD,
};

uint8_t g_mapping_ux_profile_state_to_bth[UX_API_BT_PROFILE_STATE_MAX] = {
    [UX_API_BT_PROFILE_STATE_CONNECTING] =    BR_PROFILE_STATE_CONNECTING,
    [UX_API_BT_PROFILE_STATE_CONNECTED] =     BR_PROFILE_STATE_CONNECTED,
    [UX_API_BT_PROFILE_STATE_DISCONNECTING] = BR_PROFILE_STATE_DISCONNECTING,
    [UX_API_BT_PROFILE_STATE_DISCONNECTED] =  BR_PROFILE_STATE_DISCONNECTED,
};

ux_bt_avrcp_info_t g_avrcp_info[BS_CONNECTION_UPPER_LIMIT] = {0};

ux_bt_avrcp_info_t* ux_bt_avrcp_get_info(void)
{
    return g_avrcp_info;
}

errcode_t ux_bt_media_report_volume(uint16_t dev_id, uint8_t volume)
{
    if (volume > AVRCP_VOLUME_MAX || dev_id >= BS_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_bt_media_report_volume] Invalid vol %d, dev_id %d.\n", volume, dev_id);
        return UX_ERR_INVALID_PARAMETER;
    }
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    if (avrcp_info[dev_id].profile_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_err("[ux_bt_media_report_volume] Service disconnect %d.\n", avrcp_info[dev_id].profile_state);
        return UX_ERR_BG_PROFILE_DISCONNECT;
    }

    errcode_t err = UX_ERR_SUCCESS;
    err = bs_br_media_volume_notify(dev_id, volume);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_media_report_volume] Fail to notify ct cur volume 0x%x.\n", err);
        return UX_ERR_BG_FAIL_TO_REPLY;
    }
    return UX_ERR_SUCCESS;
}

static uint8_t calculate_vol_by_ctrl_type(ux_api_bt_control_volume_t vol_ctrl, uint8_t cur_vol, uint8_t absolute_vol)
{
    uint8_t res = cur_vol;
    uint8_t equal_portion = (AVRCP_VOLUME_TO_BT_MAX + COMPLEMENT_ONE) / (AVRCP_VOLUME_MAX + COMPLEMENT_ONE);
    switch (vol_ctrl) {
        case UX_API_BT_CONTROL_VOLUME_UP:
            res = (cur_vol + equal_portion > AVRCP_VOLUME_TO_BT_MAX) ?
                           AVRCP_VOLUME_TO_BT_MAX : (cur_vol + equal_portion);
            break;
        case UX_API_BT_CONTROL_VOLUME_DOWN:
            res = (cur_vol < equal_portion) ? AVRCP_VOLUME_MIN : (cur_vol - equal_portion);
            break;
        case UX_API_BT_CONTROL_VOLUME_MUTE:
            res = AVRCP_VOLUME_MIN;
            break;
        case UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE:
            res = absolute_vol;
            break;
        default:
            break;
    }
    return res;
}
/**
 * 按协议，应使用bs_br_media_volume_operate接口实现，但大多数设备不响应该接口组建的报文
 * 多数设备仅响应bs_br_media_volume_notify组建的报文，所以使用bs_br_media_volume_notify接口重新实现音量控制
 */
errcode_t ux_bt_media_volume_control(const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info)
{
    if (addr == NULL || volume_control_info == NULL || volume_control_info->control >= UX_API_BT_CONTROL_VOLUME_MAX) {
        ux_log_err("[ux_bt_media_volume_control] Invalid param, vol_ctrl=%d.\n", volume_control_info->control);
        return UX_ERR_INVALID_PARAMETER;
    }
    uint16_t device_id = ux_bt_find_device_id_by_addr(UX_API_BT_PROTOCOL_BR, addr);
    if (volume_control_info->volume > AVRCP_VOLUME_TO_BT_MAX || device_id >= UX_BT_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_bt_media_volume_control] Invalid param, dev_id=%d, vol=%d.\n",
            device_id, volume_control_info->volume);
        return UX_ERR_INVALID_PARAMETER;
    }
    errcode_t err = UX_ERR_SUCCESS;
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    uint8_t avrcp_volume = calculate_vol_by_ctrl_type(volume_control_info->control, avrcp_info[device_id].volume,
        volume_control_info->volume);
    bool is_mute = false;
    if (volume_control_info->control == UX_API_BT_CONTROL_VOLUME_MUTE) {
        is_mute = true;
    }
    err = bs_br_media_volume_notify(device_id, avrcp_volume);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_media_volume_control] Fail to send volume control request 0x%x.\n", err);
        return UX_ERR_BG_FAIL_TO_REQUEST;
    }
    if (is_mute) {
        avrcp_info[device_id].is_mute = is_mute;
    } else {
        avrcp_info[device_id].volume = avrcp_volume;
        avrcp_info[device_id].is_mute = is_mute;
    }
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    ux_bt_a2dp_sink_set_vol(device_id, avrcp_volume);
#endif
    return err;
}

errcode_t ux_bt_avrcp_play_control(const ux_api_bt_addr_t *addr, ux_api_bt_control_play_t play_control)
{
    errcode_t err = UX_ERR_SUCCESS;
    bs_br_media_operate_type_t opt_type = g_mapping_ux_media_control_request_to_bth[play_control];
    uint16_t device_id = bs_find_device_id_by_addr(BS_PROTOCOL_BREDR, addr->type, addr->addr);
    err = bs_br_media_operate_request(device_id, (uint8_t)opt_type, NULL, 0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_avrcp_play_control] Fail to send media control request 0x%x.\n", err);
        return UX_ERR_BG_FAIL_TO_REQUEST;
    }
    return err;
}

/**
 * tg、ct端共同函数，bth上报avrcp链路状态
 */
void ux_bt_avrcp_connect_state_cbk(uint16_t dev_id, uint8_t avrcp_role, uint8_t conn_state)
{
    ux_log_info("[ux][bt][ux_bt_avrcp_connect_state_cbk] avrcp state cb dev_id[%d] avrcp_role[%d], conn_state[%d].",
        dev_id, avrcp_role, conn_state);
    /**
     * 规格约束，bt可以配对多个设备，但是只能连接一副耳机，一台手机
     * 每次新连接的起点为connectting或connected回调
     * 过滤掉异步操作引起的非当前连接回调上报
     */

    if (avrcp_role > BR_MEDIA_AVRCP_TG || avrcp_role < BR_MEDIA_AVRCP_CT) {
        ux_log_err("[ux_bt_avrcp_connect_state_cbk] Role err.\n");
        return;
    }

    if (conn_state > BR_PROFILE_STATE_DISCONNECTED || conn_state < BR_PROFILE_STATE_CONNECTING) {
        ux_log_err("[ux_bt_avrcp_connect_state_cbk] Role err.\n");
        return;
    }
    ux_bt_avrcp_info_t avrcp_profile_package = {
        .device_id = dev_id,
        .profile_state = UX_API_BT_PROFILE_STATE_MAX,
        .role = avrcp_role == BR_MEDIA_AVRCP_CT ? UX_BT_AVRCP_ROLE_CT : UX_BT_AVRCP_ROLE_TG,
    };

    for (uint8_t ux_state = 0; ux_state < arry_num(g_mapping_ux_profile_state_to_bth); ux_state++) {
        if (g_mapping_ux_profile_state_to_bth[ux_state] == conn_state) {
            avrcp_profile_package.profile_state = ux_state;
            break;
        }
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_AVRCP_PROFILE_STATE_CHANGE,
        (uint8_t *)(&avrcp_profile_package), sizeof(avrcp_profile_package));
}

/**
 * ct端特有函数，
 * 本端调用bs_br_media_volume_operate, 向对端请求音量同步
 * 对端调用bs_br_media_volume_notify, 回复音量信息
 */
void ux_bt_avrcp_volume_report_cbk(uint16_t dev_id, uint8_t volume)
{
    ux_log_info("[ux][bt][ux_bt_avrcp_volume_report_cbk] dev_id[%d], sync volume[%d].", dev_id, volume);
    if (volume > AVRCP_VOLUME_TO_BT_MAX || volume < AVRCP_VOLUME_MIN) {
        ux_log_err("[ux_bt_avrcp_volume_report_cbk] Ivalid volume.\n");
        return;
    }
    bs_addr_t *addr = bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[ux_bt_avrcp_volum_control_req_cbk] Nullptr.\n");
        return;
    }
    ux_bt_avrcp_info_t vol_msg = {0};
    vol_msg.device_id = dev_id;
    vol_msg.volume = volume;
    if (ux_bt_a2dp_get_src_info()->device_id == dev_id) {
        ux_adapter_bt_avrcp_tg_volume_control_process((bd_addr_t*)addr, volume);
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_VOLUME_SYNC_REPORT, (uint8_t*)&vol_msg, sizeof(vol_msg));
}

/**
 * tg端特有函数
 * 对端调用bs_br_media_operate_request, 本端收到对端的媒体控制请求
 */
void ux_bt_avrcp_media_control_req_cbk(uint16_t dev_id, uint8_t opt_type, uint8_t *param, uint16_t param_len)
{
    ux_log_info("[ux][bt][ux_bt_avrcp_media_control_req_cbk] dev_id[%d] receive control[%d].", dev_id, opt_type);

    if (opt_type >= BR_MEDIA_OPT_REGISTER_ATTRIBUTE_NOTIFICATION || opt_type < BR_MEDIA_OPT_PLAY) {
        ux_log_err("[ux_bt_avrcp_media_control_req_cbk] Ivalid opt type.\n");
        return;
    }
    ux_bt_avrcp_play_ctrl_msg_t play_ctrl_msg = {0};
    play_ctrl_msg.dev_id = dev_id;

    for (uint8_t ux_req = 0; ux_req < arry_num(g_mapping_ux_media_control_request_to_bth); ux_req++) {
        if (g_mapping_ux_media_control_request_to_bth[ux_req] == opt_type) {
            play_ctrl_msg.play_ctrl = ux_req;
            break;
        }
    }
    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)dev_id);
    if (addr == NULL) {
        ux_log_err("[ux_bt_avrcp_media_control_req_cbk] Nullptr.\n");
        return;
    }
    if (ux_bt_a2dp_get_src_info()->device_id == dev_id) {
        ux_adapter_bt_avrcp_tg_play_control_process(addr, play_ctrl_msg.play_ctrl);
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_CONTROL_REQUEST,
        (uint8_t*)(&play_ctrl_msg), sizeof(play_ctrl_msg));
}

/**
 * ct端特有函数
 * 对端调用bs_br_media_update_info函数，本端收到媒体信息
 * 支持两种媒体信息:
 * 1、媒体播放状态
 * 2、当前播放的媒体信息
 */
void ux_bt_avrcp_media_info_cbk(uint16_t dev_id, uint8_t type, uint8_t *param, uint16_t param_len)
{
    ux_log_info("[ux][bt][ux_bt_avrcp_media_info_cbk]dev_id[%d], type[%d].", dev_id, type);
    if (type < BR_MEDIA_INFO_PLAY_STATUS || type > BR_MEDIA_INFO_ELEMENT_ATTR) {
        ux_log_err("[ux_bt_avrcp_media_info_cbk] Invalid media type.\n");
        return;
    }
    if (param == NULL) {
        ux_log_err("[ux_bt_avrcp_media_info_cbk] Nullptr.\n");
        return;
    }
    if (param_len == 0) {
        ux_log_err("[ux_bt_avrcp_media_info_cbk] Invalid len.\n");
        return;
    }

    if ((br_media_info_type_t)type == BR_MEDIA_INFO_PLAY_STATUS) {
        ux_bt_avrcp_play_state_msg_t play_msg = {0};
        play_msg.dev_id = dev_id;
        for (uint8_t ux_state = 0; ux_state < arry_num(g_mapping_ux_media_play_state_to_bth); ux_state++) {
            if (g_mapping_ux_media_play_state_to_bth[ux_state] == *param) {
                play_msg.play_state = ux_state;
                break;
            }
        }
        // 更新媒体播放状态
        ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_PLAY_STATE_CHANGE, (uint8_t*)(&play_msg), sizeof(play_msg));
    } else if ((br_media_info_type_t)type == BR_MEDIA_INFO_ELEMENT_ATTR) {
        // bth传进来的是一个数组，和ux_bt_media_element_attr_value_t对齐
        if (((param_len % sizeof(ux_bt_media_element_attr_value_t)) != 0)) {
            ux_log_err("[ux_bt_avrcp_media_info_cbk] Invalid len.\n");
            return;
        }
        // 更新媒体属性，作者、发行日期、歌词等
        ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_ATTRIBUTE_REPORT, param, param_len);
    }
}

/**
 * tg端特有函数
 * 对端调用bs_br_media_volume_operate函数，本端收到调节音量通知
 */
void ux_bt_avrcp_volum_control_req_cbk(uint16_t dev_id, uint8_t type, uint8_t volume)
{
    ux_log_info("[ux][bt][ux_bt_avrcp_volum_control_req_cbk] dev_id[%d] type[%d] volume[%d].", dev_id, type, volume);
    if (type < BR_MEDIA_VOLUME_OPT_UP || type > BR_MEDIA_VOLUME_OPT_SET_ABSOLUTE) {
        ux_log_err("[ux_bt_avrcp_volum_control_req_cbk] Invalid volume type.\n");
    }

    if (volume > AVRCP_VOLUME_TO_BT_MAX || volume < AVRCP_VOLUME_MIN) {
        ux_log_err("[ux_bt_avrcp_volum_control_req_cbk] Ivalid volume.\n");
        return;
    }
    ux_bt_avrcp_vol_ctrl_msg_t vol_ctrl_msg = {0};
    vol_ctrl_msg.dev_id = dev_id;
    for (uint8_t ux_req = 0; ux_req < arry_num(g_mapping_ux_media_volume_control_request_to_bth); ux_req++) {
        if (g_mapping_ux_media_volume_control_request_to_bth[ux_req] == type) {
            vol_ctrl_msg.vol_ctrl_info.control = ux_req;
            break;
        }
    }

    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    if (vol_ctrl_msg.vol_ctrl_info.control != UX_API_BT_CONTROL_VOLUME_SET_ABSOLUTE) {
        vol_ctrl_msg.vol_ctrl_info.volume = avrcp_info[dev_id].volume;
    } else {
        vol_ctrl_msg.vol_ctrl_info.volume = volume;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_MEDIA_VOLUME_CONTROL_REQUEST,
        (uint8_t *)(&vol_ctrl_msg), sizeof(vol_ctrl_msg));
}

bs_br_media_callbacks_t g_ux_avrcp_ct_cb = {
    .connect_state_cbk =        ux_bt_avrcp_connect_state_cbk,
    .media_volume_report_cbk =  ux_bt_avrcp_volume_report_cbk,
    .media_opt_req_cbk =        ux_bt_avrcp_media_control_req_cbk,
    .media_update_cbk =         ux_bt_avrcp_media_info_cbk,
    .media_volume_operate_cbk = ux_bt_avrcp_volum_control_req_cbk,
};

errcode_t ux_bt_avrcp_init(void)
{
    errcode_t err = bs_br_media_register_callbacks(0, &g_ux_avrcp_ct_cb);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_avrcp_init] Fail to register avrcp callbacks 0x%x.", err);
        return err;
    }
    return err;
}

errcode_t ux_bt_avrcp_deinit(void)
{
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    for (uint8_t i = 0; i < BS_CONNECTION_UPPER_LIMIT; i++) {
        if (avrcp_info[i].profile_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
            continue;
        }
        bs_br_media_avrcp_role_t role =  BR_MEDIA_AVRCP_CT;
        if ((avrcp_info[i].role&BR_MEDIA_AVRCP_TG) == BR_MEDIA_AVRCP_TG) {
            role = BR_MEDIA_AVRCP_TG;
        }
        ux_bt_avrcp_connect_state_cbk(i, (uint8_t)role, BR_PROFILE_STATE_DISCONNECTED);
    }
    errcode_t err = bs_br_media_unregister_callbacks(0);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_avrcp_deinit] Fail to register avrcp callbacks 0x%x.", err);
        return err;
    }
    return err;
}

errcode_t ux_bt_avrcp_tg_notify_play_state(uint16_t dev_id, ux_bt_media_play_state_t play_state)
{
    if (play_state >= UX_BT_MEDIA_PLAY_STATE_MAX || play_state < UX_BT_MEDIA_PLAY_STATE_STOPPED) {
        ux_log_err("[ux_bt_avrcp_tg_notify_play_state] Invalid param %d.", play_state);
        return UX_ERR_INVALID_PARAMETER;
    }
    br_media_play_status_t bth_play_state = g_mapping_ux_media_play_state_to_bth[play_state];
    errcode_t err = UX_ERR_SUCCESS;
    err = bs_br_media_update_info(dev_id, BR_MEDIA_INFO_PLAY_STATUS,
        (uint8_t *)(&bth_play_state), sizeof(br_media_play_status_t));
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_avrcp_tg_notify_play_state] Fail to update play state 0x%x.", err);
        return err;
    }
    return err;
}

ux_api_bt_control_info_t* ux_bt_get_avrcp_control_info(const ux_api_bt_addr_t *addr)
{
    static ux_api_bt_control_info_t s_control_info[BS_CONNECTION_UPPER_LIMIT] = {0};
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    errcode_t err = UX_ERR_SUCCESS;
    for (uint8_t i = 0; i < BS_CONNECTION_UPPER_LIMIT; i++) {
        ux_api_bt_addr_t avrcp_addr = {0};
        err = ux_bt_get_addr_by_device_id(avrcp_info[i].device_id, &avrcp_addr);
        if (err != UX_ERR_SUCCESS) {
            continue;
        }
        if (memcmp(addr, &avrcp_addr, sizeof(avrcp_addr)) != 0) {
            continue;
        }
        s_control_info[i].profile_state = avrcp_info[i].profile_state;
        s_control_info[i].is_speaker_mute = avrcp_info[i].is_mute;
        s_control_info[i].speaker_volume = avrcp_info[i].volume;
        return &s_control_info[i];
    }
    return NULL;
}
