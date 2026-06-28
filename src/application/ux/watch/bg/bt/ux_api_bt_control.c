/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#ifdef CONFIG_SUPPORT_AVRCP
#include "ux_bt_avrcp.h"
#endif
#ifdef CONFIG_SUPPORT_HFP_HF
#include "ux_bt_hal_hfp_hf.h"
#include "ux_bt_call_hf.h"
#endif
#include "bs_device_mgr.h"
#include "ux_errcode.h"
#include "ux_common_log.h"
#include "ux_device_manager.h"
#include "ux_api_bt_control.h"
errcode_t ux_api_bt_control_volume(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_volume_info_t *volume_control_info)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_control_volume] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    if (volume_control_info->control >= UX_API_BT_CONTROL_VOLUME_MAX ||
        volume_control_info->control < UX_API_BT_CONTROL_VOLUME_UP) {
        ux_log_err("[ux_api_bt_control_volume] Invalid control type.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ux_log_info("[ux_api_bt_control_volume] profile=%d, addr:****%02x%02x%02x%02x, vol_type=%d, vol_ctrl=%d, vol=%d",
        profile, addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], volume_control_info->volume_type, volume_control_info->control,
        volume_control_info->volume);
    ux_bg_device_info_t *find_result = NULL;
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL) {
        ux_log_err("[ux_api_bt_control_volume] ux_bg_find_device_info_by_addr fail.");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    if (find_result->connect_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_err("[ux_api_bt_control_volume] device disconnect.");
        return UX_ERR_BG_ACL_DISCONNECT;
    }

    switch (profile) {
        case UX_API_BT_PROFILE_HFP:
#ifdef CONFIG_SUPPORT_AUTO_CONNECT_HFP_HF
            return ux_bt_call_set_volume_request(addr, volume_control_info);
#endif
            break;
        case UX_API_BT_PROFILE_AVRCP:
#ifdef CONFIG_SUPPORT_AVRCP
            return ux_bt_media_volume_control(addr, volume_control_info);
#endif
            break;
        default:
            break;
    }
    return UX_ERR_NOT_SUPPORT;
}

errcode_t ux_api_bt_control_play(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr,
    ux_api_bt_control_play_t play_control)
{
    if (addr == NULL) {
        ux_log_err("[ux_api_bt_control_play] Nullptr.");
        return UX_ERR_NULL_PTR;
    }
    if (play_control >= UX_API_BT_CONTROL_PLAY_MAX || play_control < UX_API_BT_CONTROL_PLAY_START) {
        ux_log_err("[ux_api_bt_control_play] Invalid control type.\n");
        return UX_ERR_INVALID_PARAMETER;
    }
    ux_log_info("[ux_api_bt_control_play] profile=%d, addr:****%02x%02x%02x%02x, play_control=%d.\n",
        profile, addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], play_control);

    ux_bg_device_info_t *find_result = NULL;
    find_result = ux_bg_find_device_info_by_addr(ux_bg_get_paired_device_list(), addr, UX_API_BT_PROTOCOL_BR);
    if (find_result == NULL) {
        ux_log_err("[ux_api_bt_control_play] ux_bg_find_device_info_by_addr fail.");
        return UX_ERR_BG_DEVICE_DOES_NOT_EXIST;
    }
    if (find_result->connect_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_err("[ux_api_bt_control_play] device disconnect.");
        return UX_ERR_BG_ACL_DISCONNECT;
    }

    switch (profile) {
        case UX_API_BT_PROFILE_HFP:
            return UX_ERR_NOT_SUPPORT;
        case UX_API_BT_PROFILE_AVRCP:
#ifdef CONFIG_SUPPORT_AVRCP
            return ux_bt_avrcp_play_control(addr, play_control);
#else
            return UX_ERR_NOT_SUPPORT;
#endif
        default:
            return UX_ERR_NOT_SUPPORT;
    }
}

errcode_t ux_api_bt_control_call(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr,
    const ux_api_bt_control_call_info_t *call_control)
{
    /* 入参检测 */
    if (addr == NULL || call_control == NULL) {
        ux_log_err("[ux_api_bt_control_call] Nullptr.\n");
        return UX_ERR_NULL_PTR;
    }
    if (profile != UX_API_BT_PROFILE_HFP) {
        ux_log_err("[ux_api_bt_control_call] Not support.\n");
        return UX_ERR_NOT_SUPPORT;
    }
    ux_api_bt_control_call_t call_req = call_control->request;
    if (call_req >= UX_API_BT_CONTROL_CALL_MAX || call_req < UX_API_BT_CONTROL_CALL_DAILING) {
        ux_log_err("[ux_api_bt_control_call] invalid param, call_req=%d.\n", call_req);
        return UX_ERR_INVALID_PARAMETER;
    }
    if (call_req == UX_API_BT_CONTROL_CALL_DAILING && call_control->param == NULL) {
        ux_log_err("[ux_api_bt_control_call] invalid dailing.\n");
        return UX_ERR_NULL_PTR;
    }
    ux_log_info("[ux_api_bt_control_call] profile=%d, addr:****%02x%02x%02x%02x, addr_type=%d, call_control=%d.\n",
        profile, addr->addr[BG_ADDR_INDEX_3], addr->addr[BG_ADDR_INDEX_2], addr->addr[BG_ADDR_INDEX_1],
        addr->addr[BG_ADDR_INDEX_0], addr->type, call_control->request);
#ifdef CONFIG_SUPPORT_HFP_HF
    return ux_bt_hal_call_operate_request(addr, call_control);
#endif
    return UX_ERR_NOT_SUPPORT;
}

const ux_api_bt_control_info_t* ux_api_bt_control_get_info(ux_api_bt_profile_t profile, const ux_api_bt_addr_t *addr)
{
    if (addr == NULL || profile >= UX_API_BT_PROFILE_MAX) {
        ux_log_err("[ux_api_bt_control_get_info] invalid param, profile=%d.\n", profile);
        return NULL;
    }
    typedef ux_api_bt_control_info_t* (*ux_bt_get_control_info_handle)(const ux_api_bt_addr_t *addr);
    static ux_bt_get_control_info_handle s_get_control_info_handle_set[UX_API_BT_PROFILE_MAX] = {
#ifdef CONFIG_SUPPORT_HFP_HF
        [UX_API_BT_PROFILE_HFP] = ux_bt_call_get_control_info,
#endif
#ifdef CONFIG_SUPPORT_AVRCP
        [UX_API_BT_PROFILE_AVRCP] = ux_bt_get_avrcp_control_info,
#endif
    };
    if (s_get_control_info_handle_set[profile] == NULL) {
        ux_log_err("[ux_api_bt_control_get_info] not support.\n");
        return NULL;
    }
    return s_get_control_info_handle_set[profile](addr);
}