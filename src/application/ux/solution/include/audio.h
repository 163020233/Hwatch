/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * File          audio.h
 * Description:  Audio Common Definition.
 */
#ifndef AUDIO_H
#define AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "audio_types.h"
#include "ux_api_audio_effect.h"
#include "ux_api_audio_common.h"

/* audio soundtrigger event */
typedef enum {
    AUDIO_SOUNDTRIGGER_EVT_VOICEASSISTANT_CALL,  // apply for voice assistent service

    AUDIO_SOUNDTRIGGER_EVT_ACCEPT_CALL,      // answer the call
    AUDIO_SOUNDTRIGGER_EVT_REJECT_CALL,      // reject the call
    AUDIO_SOUNDTRIGGER_EVT_HANGUP_CALL,      // hangup the call
    AUDIO_SOUNDTRIGGER_EVT_PLAY_MUSIC,       // play music
    AUDIO_SOUNDTRIGGER_EVT_STOP_MUSIC,       // stop music
    AUDIO_SOUNDTRIGGER_EVT_PREVIOUS_SONG,    // previous song
    AUDIO_SOUNDTRIGGER_EVT_NEXT_SONG,        // next song
    AUDIO_SOUNDTRIGGER_EVT_VOLUMEUP,         // non-voice volume up
    AUDIO_SOUNDTRIGGER_EVT_VOLUMEDOWN,       // non-voice volume down
    AUDIO_SOUNDTRIGGER_EVT_ANC_START,        // anc start
    AUDIO_SOUNDTRIGGER_EVT_ANC_STOP,         // anc stop
    AUDIO_SOUNDTRIGGER_EVT_HT_START,         // ht start
    AUDIO_SOUNDTRIGGER_EVT_HT_STOP,          // ht stop
    AUDIO_SOUNDTRIGGER_EVT_GET_AVRCPVOLUME,  // get avrcp volume
    AUDIO_SOUNDTRIGGER_EVT_GET_HFPVOLUME,    // get hfp volume
    AUDIO_SOUNDTRIGGER_EVT_MUTE,             // mute
    AUDIO_SOUNDTRIGGER_EVT_UNMUTE,           // unmute
    AUDIO_SOUNDTRIGGER_EVT_ATTR_DISABLE,     // disable music attr
    AUDIO_SOUNDTRIGGER_EVT_ATTR_ENABLE,      // enable music attr
    AUDIO_SOUNDTRIGGER_EVT_END,
} audio_soundtrigger_event;

/* audio soundtrigger param */
typedef enum {
    AUDIO_SOUNDTRIGGER_PARAM_NONE,
    AUDIO_SOUNDTRIGGER_PARAM_XIAOEXIAOE,
    AUDIO_SOUNDTRIGGER_PARAM_NIHAOYOYO,
    AUDIO_SOUNDTRIGGER_PARAM_OKAY_EMY,
    AUDIO_SOUNDTRIGGER_PARAM_DEAR_HONOR,
    AUDIO_SOUNDTRIGGER_PARAM_HEY_GOOGLE,
    AUDIO_SOUNDTRIGGER_PARAM_OK_GOOGLE,
    AUDIO_SOUNDTRIGGER_PARAM_END,
} audio_sound_trigger_param;

/* A2DP codec type */
typedef enum {
    AUDIO_A2DP_CODEC_TYPE_SBC,   // SBC Codec
    AUDIO_A2DP_CODEC_TYPE_AAC,   // AAC Codec
    AUDIO_A2DP_CODEC_TYPE_LHDC,  // LHDC Codec
    AUDIO_A2DP_CODEC_TYPE_LC3,   // LC3 Codec
    AUDIO_A2DP_CODEC_TYPE_L2HC,  // L2HC Codec
    AUDIO_A2DP_CODEC_TYPE_HAC,   // L2HC Codec
    AUDIO_A2DP_CODEC_TYPE_LDAC,  // LDAC Codec
    AUDIO_A2DP_CODEC_TYPE_END,
} audio_a2dp_codec_type;

/** Hires Record codec type */
typedef enum {
    AUDIO_HIRES_RECORD_CODEC_TYPE_LC3,  // LC3 Codec
    AUDIO_HIRES_RECORD_CODEC_TYPE_END,
} audio_hires_record_codec_type;

/** SCO codec type */
typedef enum {
    AUDIO_SCO_CODEC_TYPE_MSBC,  // mSBC codec
    AUDIO_SCO_CODEC_TYPE_CVSD,  // CVSD codec
    AUDIO_SCO_CODEC_TYPE_LC3,   // LC3 codec
    AUDIO_SCO_CODEC_TYPE_END,
} audio_sco_codec_type;

/* Volume adjust direction */
typedef enum {
    AUDIO_VOLUME_TURN_DOWN,
    AUDIO_VOLUME_TURN_UP,
    AUDIO_VOLUME_ADJUST_END,
} audio_volume_adjust_type;

/* bitdepth format */
typedef enum {
    AUDIO_BITDEPTH_8BIT,   // 8bit depth
    AUDIO_BITDEPTH_16BIT,  // 16bit depth
    AUDIO_BITDEPTH_24BIT,  // 24bit depth
    AUDIO_BITDEPTH_32BIT,  // 32bit depth
    AUDIO_BITDEPTH_END,
} audio_bit_depth;

/* product form */
typedef enum {
    AUDIO_FORM_TWS_LEFT,   // product form is only left channel
    AUDIO_FORM_TWS_RIGHT,  // product form is only right channel
    AUDIO_FORM_NON_TWS,    // product form is not tws
    AUDIO_FORM_END,
} audio_form;

/* audio TWS mode */
typedef enum {
    AUDIO_TWS_MODE_NON_TWS,         // 单耳模式
    AUDIO_TWS_MODE_FORWARD,         // 转发模式
    AUDIO_TWS_MODE_LISTEN,          // 协同模式
    AUDIO_TWS_MODE_DUAL_TRANSMIT,   // 双发模式
    AUDIO_TWS_MODE_MTWS,
    AUDIO_TWS_MODE_END,
} audio_tws_mode;

/* audio mic role */
typedef enum {
    AUDIO_MIC_ROLE_MASTER,  // audio mic role is master (use mic for uplink)
    AUDIO_MIC_ROLE_SLAVE,   // audio mic role is slave (DON'T use mic for uplink)
    AUDIO_MIC_ROLE_END,
} audio_mic_role;

/*
 * notification file playback finished callback.
 * @param[in]   type - callback input param
 * @return     void
 */
typedef void (*audio_notification_end_callback)(int32_t type);

typedef bool (*audio_notification_event_callback)(uint8_t audio_tone_type_id);

typedef enum {
    AUDIO_HIRES_RECORD_MODE_HD,
    AUDIO_HIRES_RECORD_MODE_MESH,
    AUDIO_HIRES_RECORD_MODE_END,
} audio_hires_record_mode;

/* Hires record codec config */
typedef struct {
    audio_hires_record_codec_type codec;  // Hires record codec type
    uint32_t sample_rate;                 // Hires record sample rate
    uint32_t channel_num;                 // Hires record channel num
    audio_bit_depth bitdepth;             // Hires record bitdepth
    audio_hires_record_mode mode;
} audio_hires_record_codec;

typedef struct {
    uint32_t notif_valid_flag;  // 0:notification don't sync playbacking; 1:notification sync playbacking
    uint32_t toggle_count;      // toggle_count - if notif_valid_flag equal 0, notification play immediately
    uint32_t diff_count;        // diffrenct toggle count
    uint32_t toggle_period;     // toggle count period
} audio_notification_sync;

/* audio volume set */
typedef struct {
    uint32_t toggle_count;     // toggle_count - if notif_valid_flag equal 1, the volume will take effect in
    uint8_t volume;            // audio volume (0 -- 15)
    uint8_t notif_valid_flag;  // 0, the volume will take effect immediately;
                               // 1, the volume will take effect in the toggle_count interrupt;
    uint16_t reserved;
} audio_volume_set_struct;

/* audio notification file struct */
typedef struct {
    char *name;                    // notification name
    bool b_fade;                   // true: fade in/out
    uint16_t loop_flag;            // 0: notification playing once; 1: notification loop playbacking
    uint8_t audio_tone_type_id;    // tone type id
    int8_t priority;               // play priority
    uint8_t play_way;              // play way: 1:sync; 2:local
    int32_t mix_volume;                        // music or call mix volume value
    int32_t max_interrupt_count;   // 提示音打断次数
    bool immediate_play_back_complete_callback;  // 是否播放完成后立刻回调LITE_PLAY_EVENT_COMPLETE事件
    audio_notification_sync sync;  // notification sync param
    audio_notification_end_callback notification_err_cb;  // audio_notification_end_callback
    audio_notification_end_callback notification_cb;      // audio_notification_end_callback
    audio_notification_event_callback event_cb;
    bool anc_state;
    ux_api_audio_anc_mode_t anc_mode;
} audio_notification_file;

#define UX_STR_NUM_MAX 255
typedef struct {
    uint8_t datalen;
    uint8_t data[UX_STR_NUM_MAX];
} audio_param_str_data;

/* Protocol report codec config */
typedef struct {
    codec_format_t codec_format;
    ux_api_audio_stream_type_t stream_type;
    ux_api_audio_stream_direction_t direction;
    audio_stream_l2hc_protocol_t protocol;
    audio_stream_vendor_id_t vendor_id;
    audio_stream_l2hc_version_t version;
    audio_stream_aggregation_t aggregation;
    uint32_t sample_rate;
    uint8_t channel_num;
    uint16_t sample_bit_width;
    uint16_t frame_length;
} ux_audio_stream_codec;

#endif
