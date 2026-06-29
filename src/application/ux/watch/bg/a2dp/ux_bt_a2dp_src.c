/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved. \n
 * Description: \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2025-10-10， Create file. \n
 */
#include "common_def.h"
#include "bs_br_media_control.h"
#include "bs_br_media_stream.h"
#include "ux_audio_a2dp_src.h"
#include "ux_common_log.h"
#include "bs_br_call.h"
#include "ux_event_define.h"
#ifdef DEMO_WATCH
#include "ux_event_def.h"
#endif
#include "ux_api_fwk.h"
#include "adapter_bt_br.h"
#include "ux_api_bt.h"
#include "ux_bt_a2dp.h"

ux_bt_a2dp_info_t g_src_info = {
    .a2dp_role = UX_BT_A2DP_SRC,
    .profile_state = UX_API_BT_PROFILE_STATE_DISCONNECTED,
    .stream_state = UX_API_BT_STREAM_STATE_STOPPED,
    .device_id = BS_CONNECTION_UPPER_LIMIT,
};

ux_bt_a2dp_info_t* ux_bt_a2dp_get_src_info(void)
{
    return &g_src_info;
}

void ux_bt_a2dp_src_connect_state_cbk(ux_bt_a2dp_info_t *profile_state_package)
{
    // 通知媒体
    ux_api_bt_addr_t *addr = (ux_api_bt_addr_t *)bs_get_addr_by_device_id((uint8_t)profile_state_package->device_id);
    if (addr == NULL) {
        ux_log_err("[ux_bt_a2dp_src_connect_state_cbk] nullptr.\n");
        return;
    }
    if (profile_state_package->profile_state == UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_adapter_bt_a2dp_src_state_process(addr, UX_API_BT_PROFILE_STATE_CONNECTED);
    } else if (profile_state_package->profile_state == UX_API_BT_PROFILE_STATE_DISCONNECTED &&
               ux_bt_a2dp_get_src_info()->device_id == profile_state_package->device_id) {
        ux_adapter_bt_a2dp_src_state_process(addr, UX_API_BT_PROFILE_STATE_DISCONNECTED);
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SRC_PROFILE_STATE_CHANGE,
        (uint8_t *)profile_state_package, sizeof(ux_bt_a2dp_info_t));
}

static void convert_src_sbc_codec(const bs_br_media_sbc_codec_t *in_codec, AudioA2dpCodecAttr *out_codec)
{
    out_codec->sampleRate = (uint16_t)in_codec->sample_rate;
    out_codec->channelMode = in_codec->channel;
    out_codec->blockLength = in_codec->block_length;
    out_codec->subband = in_codec->subband;
    out_codec->allocMethod = in_codec->alloc_method;
    out_codec->minBitpool = in_codec->min_bitpool;
    out_codec->maxBitpool = in_codec->max_bitpool;
}

static void convert_src_aac_codec(const bs_br_media_aac_codec_t *in_codec, AudioA2dpCodecAttr *out_codec)
{
    out_codec->sampleRate = (uint16_t)in_codec->sample_rate;
    out_codec->bitRate = in_codec->bit_rate;
    out_codec->channel = in_codec->channel;
    out_codec->objectType = in_codec->object_type;
    out_codec->rfa = in_codec->rfa;
    out_codec->vbr = in_codec->vbr;
}

void ux_bt_a2dp_src_codec_cbk(uint16_t dev_id, uint8_t a2dp_role, bs_br_media_codec_info_t *codec)
{
    unused(dev_id);
    unused(a2dp_role);
    AudioA2dpCodecAttr codec_cfg = {0};
    switch (codec->codec_type) {
        case BR_MEDIA_STREAM_CODEC_SBC:
            convert_src_sbc_codec(&codec->info.sbc, &codec_cfg);
            break;
        case BR_MEDIA_STREAM_CODEC_AAC:
            convert_src_aac_codec(&codec->info.aac, &codec_cfg);
            break;
        default:
            return;
    }
    if (codec->codec_type == BR_MEDIA_STREAM_CODEC_SBC)
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SRC_CODEC_CHANGE,
        (uint8_t *)(&codec_cfg), sizeof(AudioA2dpCodecAttr));
}

void ux_bt_a2dp_src_stream_state_cbk(ux_bt_a2dp_info_t *a2dp_stream_state_package)
{
    if (ux_bt_a2dp_get_src_info()->profile_state != UX_API_BT_PROFILE_STATE_CONNECTED) {
        ux_log_err("[ux_bt_a2dp_src_stream_state_cbk] Sevice disconnect.\n");
        return;
    }
    ux_api_fwk_push_event(UX_EVENT_TYPE_BG, UX_BG_A2DP_SRC_STREAM_STATE_CHANGE,
        (uint8_t *)a2dp_stream_state_package, sizeof(ux_bt_a2dp_info_t));
}