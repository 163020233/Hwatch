/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "ux_bt_avrcp.h"
#include "ux_types.h"
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
#include "ux_audio_a2dp_sink.h"
#include "ux_bt_a2dp_sink.h"
#endif
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SRC)
#include "ux_audio_a2dp_src.h"
#include "ux_bt_a2dp_src.h"
#endif
#include "ux_event_define.h"
#include "bs_device_mgr.h"
#include "ux_common_log.h"
#include "adapter_bt_br.h"
#include "bg_avrcp_service.h"

#ifndef unused
#define unused(x)  ((void)(x))
#endif
void bg_avrcp_service_profile_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_avrcp_service_profile_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_avrcp_info_t)) {
        ux_log_err("[bg_avrcp_service_profile_state_change] Invalid len %d.\n", param_len);
        return;
    }
    /**
     * 更新avrcp链路信息
     * role用|是因为可以同时支持两种角色
     */
    ux_bt_avrcp_info_t *avrcp_profile_package = (ux_bt_avrcp_info_t*)param;
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    uint16_t dev_id = avrcp_profile_package->device_id;
    avrcp_info[dev_id].device_id = avrcp_profile_package->device_id;
    avrcp_info[dev_id].profile_state = avrcp_profile_package->profile_state;
    avrcp_info[dev_id].role |= avrcp_profile_package->role;
}

void bg_avrcp_ct_service_volume_sync_report(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bt_avrcp_info_t)) {
        ux_log_err("[bg_avrcp_ct_service_volume_sync_report] invalid param, param_len=%d.\n", param_len);
        return;
    }
    // 同步tg端发送过来的音量
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    ux_bt_avrcp_info_t *vol_msg = (ux_bt_avrcp_info_t*)param;
    uint8_t taget_vol = *param;
    // 将avrcp协议规定的音量范围0-127转换为ux的音量档位0-15
    convert(taget_vol, AVRCP_VOLUME_MAX, AVRCP_VOLUME_TO_BT_MAX);
    /**
     * avrcp可能控制其他媒体，不只是蓝牙音乐
     * 需要增加其他媒体控制，在下面增加
     */
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    ux_bt_a2dp_sink_set_vol(vol_msg->device_id, vol_msg->volume);
#endif
    avrcp_info[vol_msg->device_id].volume = vol_msg->volume;
}

void bg_avrcp_ct_service_media_play_state_change(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_avrcp_ct_service_media_play_state_change] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_avrcp_play_state_msg_t)) {
        ux_log_err("[bg_avrcp_ct_service_media_play_state_change] Invalid len %d.\n", param_len);
        return;
    }
    ux_bt_avrcp_play_state_msg_t *play_state_msg = (ux_bt_avrcp_play_state_msg_t*)param;
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    avrcp_info[play_state_msg->dev_id].media_play_state = play_state_msg->play_state;
}

void bg_avrcp_ct_service_media_attribute_report(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_avrcp_ct_service_media_attribute_report] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_avrcp_info_t)) {
        ux_log_err("[bg_avrcp_ct_service_media_attribute_report] Invalid len %d.\n", param_len);
        return;
    }
    /**
     * 作为ct端，收到对端发来的媒体属性
     * ux以服务或回调的形式，呈递给上层
     */
    ux_bt_media_element_attr_value_t *media_attribute_arry = (ux_bt_media_element_attr_value_t*)param;
    uint8_t element_num = param_len / sizeof(ux_bt_media_element_attr_value_t);
    ux_log_info("==========cur_media_info==========\n");
    for (uint8_t i = 0; i < element_num; i++) {
        ux_log_info("^attribute: %d, content: %s\n", media_attribute_arry[i].attr_id, media_attribute_arry[i].value);
    }
}

void bg_avrcp_ct_service_play_media(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_play_media] Fail to get addr");
        return;
    }
    ux_api_bt_control_play(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, UX_API_BT_CONTROL_PLAY_START);
}

void bg_avrcp_ct_service_pause_media(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_pause_media] Fail to get addr");
        return;
    }
    ux_api_bt_control_play(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, UX_API_BT_CONTROL_PLAY_STOP);
}

void bg_avrcp_ct_service_play_next_media(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_play_next_media] Fail to get addr");
        return;
    }
    ux_api_bt_control_play(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, UX_API_BT_CONTROL_PLAY_NEXT);
}

void bg_avrcp_ct_service_play_pre_media(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_play_pre_media] Fail to get addr");
        return;
    }
    ux_api_bt_control_play(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, UX_API_BT_CONTROL_PLAY_PRE);
}

void bg_avrcp_ct_service_fast_forward_media(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_fast_forward_media] Fail to get addr");
        return;
    }
    ux_api_bt_control_play(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, UX_API_BT_CONTROL_PLAY_FAST_FORWARD);
}

void bg_avrcp_ct_service_media_volume_up(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = ERRCODE_SUCC;
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_up] Fail to get addr");
        return;
    }
    ux_api_bt_control_volume_info_t vol_ctrl_info = {
        .control = UX_API_BT_CONTROL_VOLUME_UP,
        .volume = ux_bt_avrcp_get_info()->volume,
    };
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, &vol_ctrl_info);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_up] Fail to ctrl vol 0x%x.\n", err);
        return;
    }
}

void bg_avrcp_ct_service_media_volume_down(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = ERRCODE_SUCC;
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_down] Fail to get addr");
        return;
    }
    ux_api_bt_control_volume_info_t vol_ctrl_info = {
        .control = UX_API_BT_CONTROL_VOLUME_DOWN,
        .volume = ux_bt_avrcp_get_info()->volume,
    };
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, &vol_ctrl_info);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_down] Fail to ctrl vol 0x%x.\n", err);
    }
}

void bg_avrcp_ct_service_media_volume_mute(uint8_t *param, uint16_t param_len)
{
    unused(param);
    unused(param_len);
    errcode_t err = ERRCODE_SUCC;
    uint16_t device_id = BS_CONNECTION_UPPER_LIMIT;
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    device_id = ux_bt_a2dp_get_sink_info()->device_id;
#endif
    bs_addr_t *addr = bs_get_addr_by_device_id(device_id);
    if (addr == NULL) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_mute] Fail to get addr");
        return;
    }
    ux_api_bt_control_volume_info_t vol_ctrl_info = {
        .control = UX_API_BT_CONTROL_VOLUME_MUTE,
        .volume = ux_bt_avrcp_get_info()->volume,
    };
    err = ux_api_bt_control_volume(UX_API_BT_PROFILE_AVRCP, (ux_api_bt_addr_t*)addr, &vol_ctrl_info);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[bg_avrcp_ct_service_media_volume_mute] Fail to ctrl vol 0x%x.\n", err);
    }
}

void bg_avrcp_tg_service_media_control_request(uint8_t *param, uint16_t param_len)
{
    if (param == NULL) {
        ux_log_err("[bg_avrcp_tg_service_media_control_request] Nullptr.\n");
        return;
    }
    if (param_len != sizeof(ux_bt_avrcp_play_ctrl_msg_t)) {
        ux_log_err("[bg_avrcp_tg_service_media_control_request] Invalid len %d.\n", param_len);
        return;
    }
    ux_bt_avrcp_play_ctrl_msg_t *play_ctrl_msg = (ux_bt_avrcp_play_ctrl_msg_t*)param;
    ux_bt_media_play_state_t notify_state = UX_BT_MEDIA_PLAY_STATE_MAX;
    switch (play_ctrl_msg->play_ctrl) {
        case UX_API_BT_CONTROL_PLAY_START:
        case UX_API_BT_CONTROL_PLAY_NEXT:
        case UX_API_BT_CONTROL_PLAY_PRE:
        case UX_API_BT_CONTROL_PLAY_REWIND:
        case UX_API_BT_CONTROL_PLAY_FAST_FORWARD:
            notify_state = UX_BT_MEDIA_PLAY_STATE_PLAYING;
            break;
        case UX_API_BT_CONTROL_PLAY_STOP:
        case UX_API_BT_CONTROL_PLAY_PAUSE:
            notify_state = UX_BT_MEDIA_PLAY_STATE_PAUSED;
            break;
        default:
            ux_log_err("[bg_avrcp_tg_service_media_control_request] Invalid play_ctrl %d.\n", play_ctrl_msg->play_ctrl);
            break;
    }
    /* 作为tg端，收到对端发来的媒体控制指令, 回复响应 */
    errcode_t err = ERRCODE_SUCC;
    err = ux_bt_avrcp_tg_notify_play_state(play_ctrl_msg->dev_id, notify_state);
    if (err != ERRCODE_SUCC) {
        ux_log_err("[bg_avrcp_tg_service_media_control_request] Fail to update play state 0x%x.", err);
        return;
    }
}

static uint8_t calculate_target_vol(ux_api_bt_control_volume_t vol_ctrl, uint8_t cur_vol, uint8_t absolute_vol)
{
    uint8_t equal_portion = (AVRCP_VOLUME_TO_BT_MAX + COMPLEMENT_ONE) / (AVRCP_VOLUME_MAX + COMPLEMENT_ONE);
    uint8_t res = cur_vol;
    switch (vol_ctrl) {
        case UX_API_BT_CONTROL_VOLUME_UP:
            if (cur_vol + equal_portion > AVRCP_VOLUME_TO_BT_MAX) {
                res = AVRCP_VOLUME_TO_BT_MAX;
            } else {
                res = cur_vol + equal_portion;
            }
            break;
        case UX_API_BT_CONTROL_VOLUME_DOWN:
            if (cur_vol < equal_portion) {
                res = 0;
            } else if (cur_vol >= AVRCP_VOLUME_TO_BT_MAX) {
                res = AVRCP_VOLUME_TO_BT_MAX - equal_portion;
            } else {
                res = cur_vol - equal_portion;
            }
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

void bg_avrcp_tg_service_vol_control_request(uint8_t *param, uint16_t param_len)
{
    if (param == NULL || param_len != sizeof(ux_bt_avrcp_vol_ctrl_msg_t)) {
        ux_log_err("[bg_avrcp_tg_service_vol_control_request] invalid param , param_len=%d.\n", param_len);
        return;
    }
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    ux_bt_avrcp_vol_ctrl_msg_t *vol_ctrl_msg = (ux_bt_avrcp_vol_ctrl_msg_t*)param;
    /**
     * 将avrcp协议规定的音量范围0-127转换为ux的音量档位0-15
     * 根据音量+、-、绝对值、静音，向dsp传入相应的音量值
     */
    uint8_t cur_vol = avrcp_info[vol_ctrl_msg->dev_id].volume;
    uint8_t vol_ctrl = vol_ctrl_msg->vol_ctrl_info.control;
    uint8_t avrcp_volume = calculate_target_vol(vol_ctrl, cur_vol, vol_ctrl_msg->vol_ctrl_info.volume);
    /**
     * 作为tg端，收到对端发来的音量控制指令
     * 若为src端，ux不作操作，以服务或回调的形式，呈递给上层
     * 若为sink端，ux调节sink音量
     * avrcp不仅可以控制a2dp，也可以控制本地音乐
     * avrcp可能控制其他音频，解耦avrcp和a2dp，如果需要控制其他音频，在下面增加
     */
#if defined(CONFIG_SUPPORT_AVRCP_CONTROL_A2DP) && defined(CONFIG_SUPPORT_A2DP_SINK)
    ux_bt_a2dp_sink_set_vol(vol_ctrl_msg->dev_id, avrcp_volume);
#endif
    if (vol_ctrl_msg->vol_ctrl_info.control == UX_API_BT_CONTROL_VOLUME_MUTE) {
        avrcp_info[vol_ctrl_msg->dev_id].is_mute = true;
    } else {
        avrcp_info[vol_ctrl_msg->dev_id].is_mute = false;
        avrcp_info[vol_ctrl_msg->dev_id].volume = avrcp_volume;
    }
    bs_addr_t *addr = bs_get_addr_by_device_id(vol_ctrl_msg->dev_id);
    if (addr == NULL) {
        ux_log_err("[ux_bt_avrcp_volum_control_req_cbk] Nullptr.\n");
        return;
    }
    /* 转换为绝对音量，发送至上层 */
    if (ux_bt_a2dp_get_src_info()->device_id == vol_ctrl_msg->dev_id) {
        ux_adapter_bt_avrcp_tg_volume_control_process((bd_addr_t*)addr, avrcp_volume);
    }
    // 通知对端当前音量
    ux_bt_media_report_volume(vol_ctrl_msg->dev_id, avrcp_info[vol_ctrl_msg->dev_id].volume);
}
