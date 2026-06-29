/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: Provides ux watch bg \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-06-16， Create file. \n
 */
#include "ux_common_log.h"
#include "bg_gap_service.h"
#include "ux_errcode.h"
#include "securec.h"
#include "ux_event_define.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_bt.h"
#include "ux_device_manager.h"
#include "bs_ble_service_api.h"
#include "ux_api_bt.h"
#include "bg_gap_service.h"
#ifdef CONFIG_SUPPORT_HFP_HF
#include "bg_hfp_hf_service.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
#include "bg_a2dp_sink_service.h"
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
#include "bg_a2dp_src_service.h"
#endif
#ifdef CONFIG_SUPPORT_AVRCP
#include "bg_avrcp_service.h"
#endif
#ifdef CONFIG_SUPPORT_PBAP
#include "bg_pbap_service.h"
#endif
#include "bg_dfx_service.h"
#include "watch_bg_response_bg.h"

typedef void (*bg_response_bg_func)(uint8_t *param, uint16_t param_len);
typedef struct {
    ux_event_bg_bt_t sub_event;
    bg_response_bg_func process;
} sub_event_func_pair_t;

static sub_event_func_pair_t sub_event_func_map[] = {
    { .sub_event = UX_BG_ENABLE,          .process = bg_gap_service_enable },
    { .sub_event = UX_BG_DISABLE,         .process = bg_gap_service_disable },
    { .sub_event = UX_BG_BT_DISABLE_CB,   .process = bg_gap_service_disable_cb },
    { .sub_event = UX_BG_START_DISCOVERY, .process = bg_gap_service_start_discovery },
    { .sub_event = UX_BG_STOP_DISCOVERY,  .process = bg_gap_service_stop_discovery },
    { .sub_event = UX_BG_START_PAIR,      .process = bg_gap_service_start_pair },
    { .sub_event = UX_BG_DISCONNECTED,    .process = bg_gap_service_disconnect },
    { .sub_event = UX_BG_REMOVE_DEVICE,   .process = bg_gap_service_remove_device },
    { .sub_event = UX_BG_BT_CORE_CRASH,   .process = bg_dfx_service_bt_core_crash },
#ifdef CONFIG_SUPPORT_HFP_HF
    { .sub_event = UX_BG_SCO_STATE_CHANGE,    .process = bg_hfp_hf_service_sco_state_change },
    { .sub_event = UX_BG_SCO_CODEC_CHANGE,    .process = bg_hfp_hf_service_sco_codec_change },
    { .sub_event = UX_BG_CALL_CAT1_CHANGE,    .process = bg_hfp_hf_service_cat1_state_change },
    { .sub_event = UX_BG_CALL_VOLUME_CHANGE,  .process = bg_hfp_hf_service_volume_change },
    { .sub_event = UX_BG_HF_ANSWER,       .process = bg_hfp_hf_service_answer },
    { .sub_event = UX_BG_HF_HANGUP,       .process = bg_hfp_hf_service_hangup },
    { .sub_event = UX_BG_HF_REJECT,       .process = bg_hfp_hf_service_reject },
    { .sub_event = UX_BG_HF_DIALING,      .process = bg_hfp_hf_service_dailing },
    { .sub_event = UX_BG_HF_VOLUME_UP,      .process = bg_hfp_hf_service_volume_up },
    { .sub_event = UX_BG_HF_VOLUME_DOWN,      .process = bg_hfp_hf_service_volume_down },
    { .sub_event = UX_BG_HF_VOLUME_MUTE,      .process = bg_hfp_hf_service_mute },
#endif
#ifdef CONFIG_SUPPORT_A2DP_SINK
    { .sub_event = UX_BG_A2DP_SINK_PROFILE_STATE_CHANGE, .process = bg_a2dp_sink_service_profile_state_change },
    { .sub_event = UX_BG_A2DP_SINK_STREAM_STATE_CHANGE,  .process = bg_a2dp_sink_service_stream_state_change },
    { .sub_event = UX_BG_A2DP_SINK_CODEC_CHANGE,         .process = bg_a2dp_sink_service_codec_change },
#endif
#ifdef CONFIG_SUPPORT_A2DP_SRC
    { .sub_event = UX_BG_A2DP_SRC_PROFILE_STATE_CHANGE, .process = bg_a2dp_src_service_profile_state_change },
    { .sub_event = UX_BG_A2DP_SRC_CODEC_CHANGE,         .process = bg_a2dp_src_service_codec_change },
    { .sub_event = UX_BG_A2DP_SRC_STREAM_STATE_CHANGE,  .process = bg_a2dp_src_service_stream_state_change },
    { .sub_event = UX_BG_A2DP_SRC_START_STREAM,         .process = bg_a2dp_src_service_start_stream },
    { .sub_event = UX_BG_A2DP_SRC_STOP_STREAM,          .process = bg_a2dp_src_service_stop_stream },
#endif
#ifdef CONFIG_SUPPORT_AVRCP
    { .sub_event = UX_BG_AVRCP_PROFILE_STATE_CHANGE,   .process = bg_avrcp_service_profile_state_change },
    { .sub_event = UX_BG_MEDIA_VOLUME_SYNC_REPORT,     .process = bg_avrcp_ct_service_volume_sync_report },
    { .sub_event = UX_BG_MEDIA_CONTROL_REQUEST,        .process = bg_avrcp_tg_service_media_control_request },
    { .sub_event = UX_BG_MEDIA_PLAY_STATE_CHANGE,      .process = bg_avrcp_ct_service_media_play_state_change },
    { .sub_event = UX_BG_MEDIA_ATTRIBUTE_REPORT,       .process = bg_avrcp_ct_service_media_attribute_report },
    { .sub_event = UX_BG_MEDIA_VOLUME_CONTROL_REQUEST, .process = bg_avrcp_tg_service_vol_control_request },
    { .sub_event = UX_BG_MEDIA_PLAY,         .process = bg_avrcp_ct_service_play_media },
    { .sub_event = UX_BG_MEDIA_PAUSE,        .process = bg_avrcp_ct_service_pause_media },
    { .sub_event = UX_BG_MEDIA_PLAY_NEXT,    .process = bg_avrcp_ct_service_play_next_media },
    { .sub_event = UX_BG_MEDIA_PLAY_PRE,     .process = bg_avrcp_ct_service_play_pre_media },
    { .sub_event = UX_BG_MEDIA_FAST_FORWARD, .process = bg_avrcp_ct_service_fast_forward_media },
    { .sub_event = UX_BG_MEDIA_VOLUMEUP,     .process = bg_avrcp_ct_service_media_volume_up },
    { .sub_event = UX_BG_MEDIA_VOLUMEDOWN,   .process = bg_avrcp_ct_service_media_volume_down },
    { .sub_event = UX_BG_MEDIA_MUTE,         .process = bg_avrcp_ct_service_media_volume_mute},
#endif
#ifdef CONFIG_SUPPORT_PBAP
    { .sub_event = UX_BG_PBAP_PROFILE_STATE_CHANGE, .process = bg_pbap_service_profile_state_change},
    { .sub_event = UX_BG_PBAP_RECEVICE_FINISH,      .process = bg_pbap_service_finish_receive_file},
#endif
};

errcode_t bg_process_bg_msg(uint8_t event, uint8_t *param, uint16_t param_len)
{
    for (uint8_t idx = 0; idx < arry_num(sub_event_func_map); idx++) {
        if (sub_event_func_map[idx].sub_event == (ux_event_bg_bt_t)event) {
            sub_event_func_map[idx].process(param, param_len);
            break;
        }
    }
    return UX_ERR_SUCCESS;
}