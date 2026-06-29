/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * File          ux_bt_basic_event.c
 * Description:  handle the basic event. all the events are defined in g_ux_bt_basic_event_handler[].
 */
#include "ux_bt_hal_sco.h"
#include "ux_event_define.h"
#include "ux_bt_call_hf.h"
#include "ux_audio_hfp_hf.h"
#include "ux_common_log.h"
#include "ux_api_fwk.h"
#include "ux_bt_hfp_hf_basic_event.h"
#include "ux_api_bt_audio.h"
#include "common_def.h"

struct {
    ux_bt_hal_call_state_t old_state;
    ux_bt_hal_call_state_t new_state;
    ux_event_master_t user_behavior;
} ux_bt_call_user_behavior_map[] = {
    {UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_BT_HAL_CALL_STATE_HELD,       UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_MASTER_SOUND_MUSIC_PLAY_PHONE},
    {UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_BT_HAL_CALL_STATE_INCOMING,   UX_MASTER_SOUND_CALL_INCOMING},
    {UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_BT_HAL_CALL_STATE_DIALING,    UX_MASTER_SOUND_CALL_DIALING},
    {UX_BT_HAL_AUDIO_STATE_IDLE,      UX_BT_HAL_CALL_STATE_INCOMING,   UX_MASTER_SOUND_CALL_INCOMING},
    {UX_BT_HAL_AUDIO_STATE_IDLE,      UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_MASTER_SOUND_MUSIC_PLAY_PHONE},
    {UX_BT_HAL_AUDIO_STATE_IDLE,      UX_BT_HAL_CALL_STATE_HELD,       UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_AUDIO_STATE_IDLE,      UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_AUDIO_STATE_IDLE,      UX_BT_HAL_CALL_STATE_DIALING,    UX_MASTER_SOUND_CALL_DIALING},
    {UX_BT_HAL_CALL_STATE_INCOMING,   UX_BT_HAL_CALL_STATE_HELD,       UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_CALL_STATE_INCOMING,   UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_CALL_STATE_INCOMING,   UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_CALL_REJECT_PHONE},
    {UX_BT_HAL_CALL_STATE_INCOMING,   UX_BT_HAL_AUDIO_STATE_IDLE,      UX_MASTER_SOUND_CALL_REJECT_PHONE},
    {UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_MUSIC_PAUSE_PHONE},
    {UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_BT_HAL_AUDIO_STATE_IDLE,      UX_MASTER_SOUND_MUSIC_PAUSE_PHONE},
    {UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_BT_HAL_CALL_STATE_INCOMING,   UX_MASTER_SOUND_CALL_INCOMING},
    {UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_AUDIO_STATE_ACTIVE,    UX_BT_HAL_CALL_STATE_DIALING,    UX_MASTER_SOUND_CALL_DIALING},
    {UX_BT_HAL_CALL_STATE_ACTIVE,     UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_CALL_HANGUP_PHONE},
    {UX_BT_HAL_CALL_STATE_ACTIVE,     UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_CALL_HANGUP_PHONE},
    {UX_BT_HAL_CALL_STATE_ACTIVE,     UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_CALL_STATE_DIALING,    UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_CALL_HANGUP_PHONE},
    {UX_BT_HAL_CALL_STATE_DIALING,    UX_BT_HAL_AUDIO_STATE_IDLE,      UX_MASTER_SOUND_CALL_HANGUP_PHONE},
    {UX_BT_HAL_CALL_STATE_DIALING,    UX_BT_HAL_CALL_STATE_INCOMING,   UX_MASTER_SOUND_CALL_INCOMING},
    {UX_BT_HAL_CALL_STATE_DIALING,    UX_BT_HAL_CALL_STATE_ACTIVE,     UX_MASTER_SOUND_CALL_ANSWER_PHONE},
    {UX_BT_HAL_CALL_STATE_HELD,       UX_BT_HAL_AUDIO_STATE_NOT_READY, UX_MASTER_SOUND_CALL_HANGUP_PHONE},
    {UX_BT_HAL_CALL_STATE_HELD,       UX_BT_HAL_AUDIO_STATE_IDLE,      UX_MASTER_SOUND_CALL_HANGUP_PHONE},
};

void ux_bt_main_state_handle(uint8_t old_state, uint8_t new_state)
{
    ux_event_master_t user_behavior = UX_MASTER_MAX;
    for (uint8_t i = 0; i < arry_num(ux_bt_call_user_behavior_map); i++) {
        ux_bt_hal_call_state_t temp_new_state = ux_bt_call_user_behavior_map[i].new_state;
        ux_bt_hal_call_state_t temp_old_state = ux_bt_call_user_behavior_map[i].old_state;
        if (new_state == temp_new_state && old_state == temp_old_state) {
            user_behavior =  ux_bt_call_user_behavior_map[i].user_behavior;
        }
    }
    if (user_behavior == UX_MASTER_MAX) {
        return;
    }
    /* 需要发送给Native app */
    ux_api_fwk_push_event(UX_EVENT_TYPE_MASTER, (uint8_t)user_behavior, NULL, 0);
}

void ux_bt_common_sco_start(ux_api_bt_stream_state_t sco_state)
{
    unused(sco_state);
    /* 拉起音频通路 */
    errcode_t ret = ux_audio_sco_start();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_common_sco_start] Fail to start audio sco 0x%x.", ret);
        return;
    }

    /* 接受sco建立请求 */
    bool accept = true;
    if (ux_bt_hal_sco_reply_request(&ux_bt_call_get_info()->addr, accept) != UX_ERR_SUCCESS) {
        ret = ux_audio_sco_stop();
        if (ret != UX_ERR_SUCCESS) {
            ux_log_err("[ux_bt_common_sco_start] Fail to stop sco audio 0x%x.\n", ret);
        }
        ux_log_err("[ux_bt_common_sco_start] Fail to replay request.\n");
    }
    return;
}

void ux_bt_common_sco_stop(ux_api_bt_stream_state_t sco_state)
{
    unused(sco_state);
    /* 3322销毁音频通路 */
    errcode_t ret = ux_audio_sco_stop();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[ux_bt_common_sco_stop] Fail to stop sco 0x%x.", ret);
    }
}