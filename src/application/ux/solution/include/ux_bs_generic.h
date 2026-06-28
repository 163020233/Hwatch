/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * File          ux_bs_generic.h
 * Description:  provide ux bs generic header file.
 */
#ifndef UX_BS_GENERIC_H
#define UX_BS_GENERIC_H

#include "errcode.h"
#include "stddef.h"
#include "stdbool.h"
#include "ux_common_define.h"
#include "ux_bg_config.h"
#include "ux_api_device.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef enum {
    UX_BS_CALL_PROC_NONE = 0,
    UX_BS_CALL_PROC_ANSWER,
    UX_BS_CALL_PROC_HANGUP,
    UX_BS_CALL_PROC_REJECT,
    UX_BS_CALL_PROC_MAX
} ux_bs_call_proc_type;

typedef enum {
    UX_BS_MUSIC_PROC_NONE = 0,
    UX_BS_MUSIC_PROC_PLAY,
    UX_BS_MUSIC_PROC_PAUSE,
    UX_BS_MUSIC_PROC_PREVIOUS,
    UX_BS_MUSIC_PROC_NEXT,
    UX_BS_MUSIC_PROC_REWIND,
    UX_BS_MUSIC_PROC_FORWARD,
    UX_BS_MUSIC_PROC_DISABLE_ATTR,
    UX_BS_MUSIC_PROC_ENABLE_ATTR,
    UX_BS_MUSIC_PROC_MAX
} ux_bs_music_proc_type;

typedef enum {
    UX_BS_SOUND_PROC_NONE = 0,
    UX_BS_SOUND_PROC_MUSIC_VOLUME,
    UX_BS_SOUND_PROC_CALL_VOLUME,
    UX_BS_SOUND_PROC_MUTE,
    UX_BS_SOUND_PROC_UNMUTE,
    UX_BS_SOUND_PROC_MAX
} ux_bs_sound_proc_type;

typedef enum {
    UX_BS_VOICE_ASSISTANT_PROC_NONE = 0,
    UX_BS_VOICE_ASSISTANT_PROC_OPEN,
    UX_BS_VOICE_ASSISTANT_PROC_CLOSE,
    UX_BS_VOICE_ASSISTANT_PROC_MAX
} ux_bs_voice_assistant_proc_type;

typedef struct {
    uint8_t type;
    unsigned char addr[UX_BS_ADDR_SIZE];
} ux_bs_addr_t;

typedef uint32_t ux_bs_obj_t;

errcode_t ux_bs_generic_init(void);
errcode_t ux_bs_generic_deinit(void);
errcode_t ux_bs_generic_set_config(void);
errcode_t ux_bs_generic_enable(void);
errcode_t ux_bs_generic_disable(void);
errcode_t ux_bs_generic_start_pair(void);
errcode_t ux_bs_generic_stop_pair(void);
errcode_t ux_bs_generic_start_connect(void);
errcode_t ux_bs_generic_stop_connect(void);
errcode_t ux_bs_generic_disconnect(void);
errcode_t ux_bs_generic_set_reconn_phase(void);
errcode_t ux_bs_generic_set_battery_level(uint8_t battery);
errcode_t ux_bs_generic_try_handover(void);
errcode_t ux_bs_generic_stream_replay(bool accept, uint8_t type);
errcode_t ux_bs_generic_config_device_name(void);
errcode_t ux_bs_generic_update_connect_state(ux_work_protocol_t protocol, uint8_t connect_num);
uint8_t ux_bs_generic_get_pairing_state(void);
uint8_t ux_bs_generic_get_peer_pairing_state(void);

errcode_t ux_bs_generic_toggle_music_play(uint8_t *cmd, uint16_t cmd_len);
errcode_t ux_bs_generic_call_process(ux_bs_call_proc_type type, uint8_t *cmd, uint16_t cmd_len);
errcode_t ux_bs_generic_music_process(ux_bs_music_proc_type type, uint8_t *cmd, uint16_t cmd_len);
errcode_t ux_bs_generic_voice_assistant_process(ux_bs_voice_assistant_proc_type type, uint8_t *cmd, uint16_t cmd_len);
errcode_t ux_bs_generic_sound_process(ux_bs_sound_proc_type type, uint8_t *cmd, uint16_t cmd_len, uint8_t volume);
errcode_t ux_bs_generic_set_audio_channel(ux_api_device_audio_channel_t audio_channel);
errcode_t ux_bs_generic_clear_pair_info(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // end UX_BS_GENERIC_H
