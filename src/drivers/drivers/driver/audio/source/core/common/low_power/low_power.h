/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: low power message between app and dsp.
 * Author: audio
 */

#ifndef __LOW_POWER_H__
#define __LOW_POWER_H__

#include "ai_struct.h"
#include "audio_aef.h"

typedef enum {
    MSG_CMD_DSP_STATUS,
    MSG_CMD_FREQ_REQ,
    MSG_CMD_MIC_CTL,
    MSG_CMD_A2DP_HAID_CTL,
} msg_cmd_type;

typedef enum {
    DSP_POWER_OFF = 0,
    DSP_POWER_ON = 1
} dsp_status;

typedef enum {
    A2DP_HAID_OFF = 0,
    A2DP_HAID_ON  = 1
} a2dp_haid_ctrl;

typedef enum {
    FREQ_ADJUST_START = 0,
    FREQ_ADJUST_END = 1,
    FREQ_PON_INIT = 2
} dsp_freq_adjust_cmd;

typedef enum {
    SCENE_HAID,
    SCENE_MUSIC,
    SCENE_CALL,
    SCENE_MAX,
} mic_clt_scene;

typedef enum {
    MIC_0,
    MIC_1,
} mic_id;

typedef struct ipc_payload_adjust_slave_freq {
    td_u32 action;
    td_u32 slave_freq;
} ipc_payload_adjust_slave_freq;

typedef struct {
    td_u32 cmd_id;
    td_u32 status;
} msg_audio2app_dsp_status_rsp;

typedef struct {
    td_u32 cmd_id;
    td_u32 request_freq_scene;
} msg_audio2app_set_freq_request;

typedef struct {
    td_u32 cmd_id;
    td_u8 scene;
    td_u8 mic_id;
    td_u8 enable;
    td_u8 reserved; /* 保留字段,按4字节对齐 */
} msg_mic_control;

typedef struct {
    td_u32 cmd_id;
    td_u32 haid_ctrl;
} msg_a2dp_haid_control;

td_s32 lpc_set_dsp_status(dsp_status state);

td_s32 lpc_set_freq_request(td_u32 request_freq_scene);

td_void lpc_mic_control(ai_amic_sel mic_sel, mic_clt_scene scene, td_bool enable);

td_s32 lpc_a2dp_haid_control(a2dp_haid_ctrl haid_state);

td_void dsp_lpc_reset(audio_scene scene);

td_void dsp_lpc_set_lhdc_enable(td_bool enable);

td_void dsp_lpc_set_scene(audio_scene scene);

td_void dsp_lpc_set_aef_enable(aef_type effect_type, td_bool enable);

td_bool dsp_lpc_get_aef_enable(aef_type effect_type);

#endif /* __LOW_POWER_H__ */
