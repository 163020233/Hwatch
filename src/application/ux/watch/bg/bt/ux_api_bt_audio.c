/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
#include "ux_bt_a2dp.h"
#endif
#ifdef CONFIG_SUPPORT_AVRCP_CONTROL_A2DP
#include "ux_bt_avrcp.h"
#endif
#ifdef CONFIG_SUPPORT_HFP_HF
#include "ux_bt_call_hf.h"
#endif
#include "ux_common_log.h"
#include "bs_device_mgr.h"
#include "ux_device_manager.h"
#include "ux_bt_audio.h"
#include "bs_br_media_stream.h"
#include "bs_br_call_sco.h"
#include "ux_api_bt_audio.h"

typedef errcode_t (*ux_bt_stream_fsm_action)(ux_api_bt_profile_t profile, uint16_t dev_id);

typedef struct {
    ux_api_bt_stream_state_t cur_state;
    ux_bt_stream_fsm_input_t input;
    ux_api_bt_stream_state_t next_state;
    ux_bt_stream_fsm_action action;
} ux_bt_stream_fsm_branch_t;

static errcode_t fsm_start_stream(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (profile) {
        case UX_API_BT_PROFILE_A2DP:
            err = bs_br_start_media_stream(dev_id);
            break;
        case UX_API_BT_PROFILE_SCO:
            err = bs_br_create_sco_connection(dev_id);
            break;
        default:
            break;
    }
    ux_log_info("[ux][fsm_start_stream], profile:%d, dev_id:%d, err:0x%x", profile, dev_id, err);
    return err;
}

static errcode_t fsm_stop_stream(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (profile) {
        case UX_API_BT_PROFILE_A2DP:
            err = bs_br_stop_media_stream(dev_id);
            break;
        case UX_API_BT_PROFILE_SCO:
            err = bs_br_release_sco_connection(dev_id);
            break;
        default:
            break;
    }
    ux_log_info("[ux][fsm_stop_stream], profile:%d, dev_id:%d, err:0x%x", profile, dev_id, err);
    return err;
}

static errcode_t start_stream(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    // 调用bth接口起流
    ux_log_info("[ux][stream_fsm] start_stream.\n");
    return fsm_start_stream(profile, dev_id);
}

static errcode_t stop_stream(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    // 调用bth接口停流
    ux_log_info("[ux][stream_fsm] stop_stream.\n");
    return fsm_stop_stream(profile, dev_id);
}
static errcode_t final_start_stm(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    // 调用bth接口起流
    ux_log_info("[ux][stream_fsm] final_start_stream.\n");
    return fsm_start_stream(profile, dev_id);
}

static errcode_t final_stop_stm(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    // 调用bth接口起流
    ux_log_info("[ux][stream_fsm] final_stop_stream.\n");
    return fsm_stop_stream(profile, dev_id);
}

static errcode_t cat_excp(ux_api_bt_profile_t profile, uint16_t dev_id)
{
    // 打印异常
    ux_log_info("[ux][stream_fsm] except profile=%d, dev_id=%d.\n", profile, dev_id);
    return UX_ERR_SUCCESS;
}

ux_bt_stream_fsm_branch_t g_stream_operate_map[] = {
     /* 现状态 */                     /* 输入 */                    /* 次状态 */                      /* 行为 */
    // 流停止状态，请求起流，状态切换为正在起流
    {UX_API_BT_STREAM_STATE_STOPPED, UX_BT_STREAM_FSM_INPUT_START, UX_API_BT_STREAM_STATE_STARTING, start_stream},
    // 流停止状态，重复停流
    {UX_API_BT_STREAM_STATE_STOPPED, UX_BT_STREAM_FSM_INPUT_STOP, UX_API_BT_STREAM_STATE_STOPPED, NULL},
    // 流停止状态，收到起流回调，底层异常
    {UX_API_BT_STREAM_STATE_STOPPED, UX_BT_STREAM_FSM_INPUT_START_CB, UX_API_BT_STREAM_STATE_STREAMING, cat_excp},
    // 流停止状态，再次收到停流回调，底层异常
    {UX_API_BT_STREAM_STATE_STOPPED, UX_BT_STREAM_FSM_INPUT_STOP_CB, UX_API_BT_STREAM_STATE_STOPPED, cat_excp},
   
    // 正在起流，重复起流
    {UX_API_BT_STREAM_STATE_STARTING, UX_BT_STREAM_FSM_INPUT_START, UX_API_BT_STREAM_STATE_STARTING, NULL},
    // 正在起流，请求停流，状态切换为正在停流
    {UX_API_BT_STREAM_STATE_STARTING, UX_BT_STREAM_FSM_INPUT_STOP, UX_API_BT_STREAM_STATE_STOPPING, NULL},
    // 正在起流，起流成功，切换为起流成功
    {UX_API_BT_STREAM_STATE_STARTING, UX_BT_STREAM_FSM_INPUT_START_CB, UX_API_BT_STREAM_STATE_STREAMING, NULL},
    // 正在起流，停流回调，调用接口起流
    {UX_API_BT_STREAM_STATE_STARTING, UX_BT_STREAM_FSM_INPUT_STOP_CB, UX_API_BT_STREAM_STATE_STARTING, final_start_stm},

    // 正在停流，请求起流，切换为正在起流
    {UX_API_BT_STREAM_STATE_STOPPING, UX_BT_STREAM_FSM_INPUT_START, UX_API_BT_STREAM_STATE_STARTING, NULL},
    // 正在停流，重复停流
    {UX_API_BT_STREAM_STATE_STOPPING, UX_BT_STREAM_FSM_INPUT_STOP, UX_API_BT_STREAM_STATE_STOPPING, NULL},
    // 正在停流，起流回调，调用接口停流
    {UX_API_BT_STREAM_STATE_STOPPING, UX_BT_STREAM_FSM_INPUT_START_CB, UX_API_BT_STREAM_STATE_STOPPING, final_stop_stm},
    // 正在停流，停流回调，切换为流停止状态
    {UX_API_BT_STREAM_STATE_STOPPING, UX_BT_STREAM_FSM_INPUT_STOP_CB, UX_API_BT_STREAM_STATE_STOPPED, NULL},

    // 起流成功，请求起流，切换为正在起流
    {UX_API_BT_STREAM_STATE_STREAMING, UX_BT_STREAM_FSM_INPUT_START, UX_API_BT_STREAM_STATE_STREAMING, NULL},
    // 起流成功，请求停流
    {UX_API_BT_STREAM_STATE_STREAMING, UX_BT_STREAM_FSM_INPUT_STOP, UX_API_BT_STREAM_STATE_STOPPING, stop_stream},
    // 起流成功，再次收到起流回调，底层异常
    {UX_API_BT_STREAM_STATE_STREAMING, UX_BT_STREAM_FSM_INPUT_START_CB, UX_API_BT_STREAM_STATE_STOPPING, cat_excp},
    // 起流成功，收到停流回调，底层异常
    {UX_API_BT_STREAM_STATE_STREAMING, UX_BT_STREAM_FSM_INPUT_STOP_CB, UX_API_BT_STREAM_STATE_STOPPED, cat_excp},
};

errcode_t ux_bt_stream_fsm_excute(ux_api_bt_profile_t profile, uint16_t dev_id, ux_bt_stream_fsm_input_t stream_input,
    ux_api_bt_stream_state_t* stream_state)
{
    ux_log_info("[ux][stream_fsm] profile:%d, dev_id:%d, stream_state:%d, stream_input:%d",
        profile, dev_id, *stream_state, stream_input);
    errcode_t err = UX_ERR_SUCCESS;
    for (uint8_t i = 0; i < sizeof(g_stream_operate_map) / sizeof(g_stream_operate_map[0]); i++) {
        if (g_stream_operate_map[i].cur_state == *stream_state &&
            g_stream_operate_map[i].input == stream_input) {
            if (g_stream_operate_map[i].action == NULL) {
                // 无操作分支视作成功
                err = UX_ERR_SUCCESS;
            } else {
                err = g_stream_operate_map[i].action(profile, dev_id);
            }
            if (err != UX_ERR_SUCCESS) {
                ux_log_err("[ux][stream_fsm] process err:0x%x", err);
                return err;
            }
            *stream_state = g_stream_operate_map[i].next_state;
            break;
        }
    }
    return err;
}

errcode_t ux_api_bt_audio_start(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_audio_start] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_audio_start] profile=%d, ****%02x%02x%02x%02x", profile, addr->addr[BG_ADDR_INDEX_3],
        addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    ux_bg_device_info_t *find_result = NULL;
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL || find_result->id >= BS_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_api_bt_audio_start] invalid find_result, id=%d", find_result->id);
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
#ifdef CONFIG_SUPPORT_AVRCP_CONTROL_A2DP
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    avrcp_info[find_result->id].media_play_state = UX_BT_MEDIA_PLAY_STATE_PLAYING;
#endif
    switch (profile) {
        case UX_API_BT_PROFILE_SCO:
#if defined(CONFIG_SUPPORT_HFP_HF) || defined(CONFIG_SUPPORT_HFP_AG)
            return ux_bt_hal_sco_create_connection(addr);
#else
            return UX_ERR_NOT_SUPPORT;
#endif
        case UX_API_BT_PROFILE_A2DP:
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
            return ux_bt_a2dp_start_stream(addr);
#else
            return UX_ERR_NOT_SUPPORT;
#endif
        default:
            return UX_ERR_NOT_SUPPORT;
    }
}

errcode_t ux_api_bt_audio_stop(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_audio_stop] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_audio_stop] profile=%d, ****%02x%02x%02x%02x", profile, addr->addr[BG_ADDR_INDEX_3],
        addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1], addr->addr[BG_ADDR_INDEX_0]);
    ux_bg_device_info_t *find_result = NULL;
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL || find_result->id >= BS_CONNECTION_UPPER_LIMIT) {
        ux_log_err("[ux_api_bt_audio_stop] invalid find_result, id=%d", find_result->id);
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }

#ifdef CONFIG_SUPPORT_AVRCP_CONTROL_A2DP
    ux_bt_avrcp_info_t *avrcp_info = ux_bt_avrcp_get_info();
    avrcp_info[find_result->id].media_play_state = UX_BT_MEDIA_PLAY_STATE_STOPPED;
#endif
    errcode_t err = UX_ERR_NOT_SUPPORT;
    switch (profile) {
        case UX_API_BT_PROFILE_SCO:
#if defined(CONFIG_SUPPORT_HFP_HF) || defined(CONFIG_SUPPORT_HFP_AG)
            return ux_bt_hal_sco_release_connection(addr);
#endif
            break;
        case UX_API_BT_PROFILE_A2DP:
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
            return ux_bt_a2dp_stop_stream(addr);
#endif
            break;
        default:
            return UX_ERR_NOT_SUPPORT;
    }
}

const ux_api_bt_audio_info_t* ux_api_bt_audio_get_info(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL || profile >= UX_API_BT_PROFILE_MAX) {
        ux_log_err("[ux_api_bt_audio_get_info] invalid param, profile=%d.\n", profile);
        return NULL;
    }
    typedef ux_api_bt_audio_info_t* (*ux_bt_get_audio_info_handle)(const ux_api_bt_addr_t *addr);
    static ux_bt_get_audio_info_handle s_get_audio_info_handle_set[UX_API_BT_PROFILE_MAX] = {
#ifdef CONFIG_SUPPORT_HFP_HF
        [UX_API_BT_PROFILE_SCO] = ux_bt_call_get_audio_info,
#endif
#if defined(CONFIG_SUPPORT_A2DP_SINK) || defined(CONFIG_SUPPORT_A2DP_SRC)
        [UX_API_BT_PROFILE_A2DP] = ux_bt_a2dp_get_audio_info,
#endif
    };
    if (s_get_audio_info_handle_set[profile] != NULL) {
        return s_get_audio_info_handle_set[profile](addr);
    }
    return NULL;
}