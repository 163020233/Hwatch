/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * File         ux_bt_audio.h
 * Description:
 */
#ifndef UX_BT_AUDIO_H
#define UX_BT_AUDIO_H
#if defined(__cplusplus) && __cplusplus
extern "C" {
#endif
typedef enum {
    UX_BT_STREAM_FSM_INPUT_START,
    UX_BT_STREAM_FSM_INPUT_START_CB,
    UX_BT_STREAM_FSM_INPUT_STOP,
    UX_BT_STREAM_FSM_INPUT_STOP_CB,
} ux_bt_stream_fsm_input_t;
errcode_t ux_bt_stream_fsm_excute(ux_api_bt_profile_t profile, uint16_t dev_id, ux_bt_stream_fsm_input_t stream_input,
    ux_api_bt_stream_state_t* stream_state);

#if defined(__cplusplus) && __cplusplus
}
#endif /* __cplusplus */
#endif