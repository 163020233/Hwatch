/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "bs_br_media_control.h"
#include "bs_br_media_stream.h"
#include "ux_audio_a2dp_sink.h"
#include "ux_common_log.h"
#include "bs_br_call.h"
#include "ux_event_define.h"
#include "ux_types.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_api_fwk.h"
#include "bs_device_mgr.h"
#include "ux_bt_a2dp.h"

#define BTH_IDX 0
#define UX_IDX  1

#define UX_SAMPLE_RATE_8000  8000
#define UX_SAMPLE_RATE_11025 11025
#define UX_SAMPLE_RATE_12000 12000
#define UX_SAMPLE_RATE_16000 16000
#define UX_SAMPLE_RATE_22050 22050
#define UX_SAMPLE_RATE_24000 24000
#define UX_SAMPLE_RATE_32000 32000
#define UX_SAMPLE_RATE_44100 44100
#define UX_SAMPLE_RATE_48000 48000
#define UX_SAMPLE_RATE_64000 64000
#define UX_SAMPLE_RATE_88200 88200
#define UX_SAMPLE_RATE_96000 96000

#define UX_CHANNEL_COUNT_1 1
#define UX_CHANNEL_COUNT_2 2

ux_bt_a2dp_info_t g_sink_info = {
    .a2dp_role = UX_BT_A2DP_SNK,
    .profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTED,
    .stream_state = UX_API_BT_STREAM_STATE_STOPPED,
    .device_id = BS_CONNECTION_UPPER_LIMIT,
};

const uint32_t g_mapping_bth_sbc_sample_rate_to_ux[][2] = {
    { (uint32_t)BR_MEDIA_SBC_SF_16000, UX_SAMPLE_RATE_16000 },
    { (uint32_t)BR_MEDIA_SBC_SF_32000, UX_SAMPLE_RATE_32000 },
    { (uint32_t)BR_MEDIA_SBC_SF_44100, UX_SAMPLE_RATE_44100 },
    { (uint32_t)BR_MEDIA_SBC_SF_48000, UX_SAMPLE_RATE_48000 },
    { (uint32_t)BR_MEDIA_SBC_SF_ALL,   UX_SAMPLE_RATE_48000 },
};

const uint32_t g_mapping_bth_sbc_channel_mode_to_ux[][2] = {
    { (uint32_t)BR_MEDIA_SBC_CHMODE_MONO,         UX_CHANNEL_COUNT_1 },
    { (uint32_t)BR_MEDIA_SBC_CHMODE_DUAL_CHANNEL, UX_CHANNEL_COUNT_2 },
    { (uint32_t)BR_MEDIA_SBC_CHMODE_STEREO,       UX_CHANNEL_COUNT_2 },
    { (uint32_t)BR_MEDIA_SBC_CHMODE_JOINT_STEREO, UX_CHANNEL_COUNT_2 },
    { (uint32_t)BR_MEDIA_SBC_CHMODE_ALL,          UX_CHANNEL_COUNT_2 },
};

const uint32_t g_mapping_bth_aac_channel_mode_to_ux[][2] = {
    { (uint32_t)BR_MEDIA_AAC_CHANNEL_1,   UX_CHANNEL_COUNT_1 },
    { (uint32_t)BR_MEDIA_AAC_CHANNEL_2,   UX_CHANNEL_COUNT_2 },
    { (uint32_t)BR_MEDIA_AAC_CHANNEL_ALL, UX_CHANNEL_COUNT_2 },
};

const uint32_t g_mapping_bth_aac_sample_rate_to_ux[][2] = {
    { (uint32_t)BR_MEDIA_AAC_SF_8000,  UX_SAMPLE_RATE_8000 },
    { (uint32_t)BR_MEDIA_AAC_SF_11025, UX_SAMPLE_RATE_11025 },
    { (uint32_t)BR_MEDIA_AAC_SF_12000, UX_SAMPLE_RATE_12000 },
    { (uint32_t)BR_MEDIA_AAC_SF_16000, UX_SAMPLE_RATE_16000 },
    { (uint32_t)BR_MEDIA_AAC_SF_22050, UX_SAMPLE_RATE_22050 },
    { (uint32_t)BR_MEDIA_AAC_SF_24000, UX_SAMPLE_RATE_24000 },
    { (uint32_t)BR_MEDIA_AAC_SF_32000, UX_SAMPLE_RATE_32000 },
    { (uint32_t)BR_MEDIA_AAC_SF_44100, UX_SAMPLE_RATE_44100 },
    { (uint32_t)BR_MEDIA_AAC_SF_48000, UX_SAMPLE_RATE_48000 },
    { (uint32_t)BR_MEDIA_AAC_SF_64000, UX_SAMPLE_RATE_64000 },
    { (uint32_t)BR_MEDIA_AAC_SF_88200, UX_SAMPLE_RATE_88200 },
    { (uint32_t)BR_MEDIA_AAC_SF_96000, UX_SAMPLE_RATE_96000 },
};

static void convert_sbc(const bs_br_media_codec_info_t *in_codec, AudioFormatConfig *out_codec)
{
    out_codec->audioFormat = SBC;
    for (uint8_t i = 0; i < arry_num(g_mapping_bth_sbc_channel_mode_to_ux); i++) {
        if (g_mapping_bth_sbc_channel_mode_to_ux[i][BTH_IDX] == in_codec->info.sbc.channel) {
            out_codec->channelCount = g_mapping_bth_sbc_channel_mode_to_ux[i][UX_IDX];
            break;
        }
    }
    
    for (uint8_t i = 0; i < arry_num(g_mapping_bth_sbc_sample_rate_to_ux); i++) {
        if (g_mapping_bth_sbc_sample_rate_to_ux[i][BTH_IDX] == in_codec->info.sbc.sample_rate) {
            out_codec->sampleRate = g_mapping_bth_sbc_sample_rate_to_ux[i][UX_IDX];
            break;
        }
    }
}

static void convert_aac(const bs_br_media_codec_info_t *in_codec, AudioFormatConfig *out_codec)
{
    out_codec->audioFormat = AAC_LC;
    for (uint8_t i = 0; i < arry_num(g_mapping_bth_aac_channel_mode_to_ux); i++) {
        if (g_mapping_bth_aac_channel_mode_to_ux[i][BTH_IDX] == in_codec->info.sbc.channel) {
            out_codec->channelCount = g_mapping_bth_aac_channel_mode_to_ux[i][UX_IDX];
            break;
        }
    }

    for (uint8_t i = 0; i < arry_num(g_mapping_bth_aac_sample_rate_to_ux); i++) {
        if (g_mapping_bth_aac_sample_rate_to_ux[i][BTH_IDX] == in_codec->info.sbc.sample_rate) {
            out_codec->sampleRate = g_mapping_bth_aac_sample_rate_to_ux[i][UX_IDX];
            break;
        }
    }
}

ux_bt_a2dp_info_t* ux_bt_a2dp_get_sink_info(void)
{
    return &g_sink_info;
}

void ux_bt_a2dp_sink_set_vol(uint16_t dev_id, uint8_t avrcp_volume)
{
    if (ux_bt_a2dp_get_sink_info()->device_id != dev_id) {
        return;
    }
    int32_t volume = (int32_t)(avrcp_volume * MEDIA_VOLUME_MAX / AVRCP_VOLUME_TO_BT_MAX);
    if (AudioManagerSetVolume(AUDIO_STREAM_A2DP_MUSIC, volume) != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_a2dp_sink_volume_sync] Fail to set volume.\n");
        return;
    }
}

void ux_bt_a2dp_sink_connect_state_cbk(ux_bt_a2dp_info_t *profile_state_package)
{
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SINK_PROFILE_STATE_CHANGE,
        (uint8_t *)profile_state_package, sizeof(ux_bt_a2dp_info_t));
}

void ux_bt_a2dp_sink_codec_cbk(uint16_t dev_id, uint8_t a2dp_role, bs_br_media_codec_info_t *codec)
{
    ux_bt_a2dp_info_t a2dp_codec_package = {0};
    a2dp_codec_package.device_id = dev_id;
    a2dp_codec_package.a2dp_role = a2dp_role == BR_MEDIA_A2DP_SRC ? UX_BT_A2DP_SRC : UX_BT_A2DP_SNK;
    /* 当前仅支持两种编码格式 */
    if (codec->codec_type == BR_MEDIA_STREAM_CODEC_SBC) {
        convert_sbc(codec, &a2dp_codec_package.codec);
    } else if (codec->codec_type == BR_MEDIA_STREAM_CODEC_AAC) {
        convert_aac(codec, &a2dp_codec_package.codec);
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SINK_CODEC_CHANGE,
        (uint8_t *)(&a2dp_codec_package), sizeof(ux_bt_a2dp_info_t));
}

void ux_bt_a2dp_sink_stream_state_cbk(ux_bt_a2dp_info_t *a2dp_stream_state_package)
{
    if (ux_bt_a2dp_get_sink_info()->profile_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_err("[ux_bt_a2dp_sink_stream_state_cbk] Sevice disconnect.\n");
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SINK_STREAM_STATE_CHANGE,
        (uint8_t *)a2dp_stream_state_package, sizeof(ux_bt_a2dp_info_t));
}