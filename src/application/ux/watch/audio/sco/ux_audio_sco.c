/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 * Description: Provides ux sco.\n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-08-21, Create file. \n
 */

#include "securec.h"
#include "ux_common_log.h"
#include "audio_manager_service.h"
#include "ux_errcode.h"
#include "ux_types.h"
#include "ux_audio_hfp_hf.h"
static bool g_audio_sco_init = false;
typedef struct sco_state {
    bool is_started;
} sco_state;

static struct sco_state g_sco_state_handle = {
    .is_started = false,
};

typedef struct {
    AudioSession session_id;
    AudioCodecFormat codec_format;        /* 音频编码格式 */
    uint32_t sample_rate;               /* 采样率 */
    AudioBitWidth sample_bit_width;          /* 采样位宽 */
} ux_audio_manager_sco_context;

static ux_audio_manager_sco_context g_audio_sco_ctx = {
    .session_id = 0,
    .codec_format = mSBC,
    .sample_rate = SAMPLE_RATE_8K,
    .sample_bit_width = BIT_WIDTH_16,
};

static ux_audio_manager_sco_context *audio_manager_sco_get_ctx(void)
{
    return &g_audio_sco_ctx;
};

static errcode_t audio_manager_sco_session_init(void)
{
    if (!AudioManagerInit()) {
        ux_log_err("[audio_manager_sco_session_init] Fail to AudioManagerInit.");
        return UX_ERR_AUDIO_INIT_FAILED;
    }
    g_audio_sco_ctx.session_id = AudioManagerMakeSessionId();
    if (g_audio_sco_ctx.session_id == AUDIO_SESSION_ID_NONE) {
        ux_log_err("audio session id invalid");
        return UX_ERR_AUDIO_APPLY_SESSION_FAIL;
    }
    if (AudioManagerActivateInterrupt(AUDIO_STREAM_VOICE_CALL_BT_SCO, g_audio_sco_ctx.session_id) != UX_ERR_SUCCESS) {
        ux_log_err("activate audio interrupt failed");
        return UX_ERR_AUDIO_ACTIVE_INTERRUPT_FAIL;
    }
    return UX_ERR_SUCCESS;
}

static errcode_t audio_manager_sco_inlink_init(void)
{
    CapturerInputConfig captureConfig = {};
    captureConfig.sessionID = g_audio_sco_ctx.session_id;
    captureConfig.audioFormat = g_audio_sco_ctx.codec_format;
    captureConfig.bitWidth = g_audio_sco_ctx.sample_bit_width;
    captureConfig.sampleRate = g_audio_sco_ctx.sample_rate;
    captureConfig.channelCount = 1; // 通道数 1
    captureConfig.linkDir = AUDIO_UP_LINK;
    captureConfig.streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO;
    int32_t ret = AudioStreamInInit(captureConfig);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[audio_manager_sco_inlink_init]Init inlink stream fail");
        return UX_ERR_AUDIO_CREATE_STREAM_FAILED;
    }
    return ERRCODE_SUCC;
}

static errcode_t audio_manager_sco_outlink_init(void)
{
    // create uplink AudioStreamOut
    AudioRendererConfig renderConfig = {};
    renderConfig.sessionID = g_audio_sco_ctx.session_id;
    renderConfig.sampleRate = g_audio_sco_ctx.sample_rate;
    renderConfig.audioFormat = g_audio_sco_ctx.codec_format;
    renderConfig.bitWidth = g_audio_sco_ctx.sample_bit_width;
    renderConfig.channelCount = 1; // 通道数1
    renderConfig.linkDir = AUDIO_DOWN_LINK;
    renderConfig.streamType = AUDIO_STREAM_VOICE_CALL_BT_SCO;
    int32_t ret = AudioStreamOutInit(renderConfig);
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("[audio_manager_sco_outlink_init] AudioStreamOutInit Fail 0x%x.", ret);
        return UX_ERR_AUDIO_CREATE_STREAM_FAILED;
    }
    return ERRCODE_SUCC;
}

static void audio_manager_device_state_reset(void)
{
    ux_log_err("[ux][audio][audio_manager_device_state_reset]audio_manager_device_state_reset.");
    return;
}

/**
 * Set SCO parameters to audio manager.
 * @param[in]   cfg - SCO parameters
 * @return      Audio error status code (0 - Success)
 */
static errcode_t audio_manager_sco_init(void)
{
    errcode_t ret = audio_manager_sco_session_init();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio][audio_manager_sco_init]audio_manager_sco_session_init fail.");
        return ret;
    }
    ret = audio_manager_sco_inlink_init();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio][audio_manager_sco_init]audio_manager_sco_inlink_init fail.");
        goto exit;
    }

    ret = audio_manager_sco_outlink_init();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio][audio_manager_sco_init]audio_manager_sco_outlink_init fail.");
        goto exit;
    }

    return ERRCODE_SUCC;
exit:
    audio_manager_device_state_reset();
    return ret;
}

static errcode_t audio_manager_sco_start(void)
{
    ux_log_info("[ux][audio]audio_manager_sco_start.");

    int32_t err;
    err = AudioStreamInStart(); // 下行开始
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[audio_manager_sco_start]audio stream in start failed");
        return UX_ERR_AUDIO_START_STREAM_FAIL;
    }
    err = AudioStreamOutPlay(); // 上行开始
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[audio_manager_sco_start]audio stream out play failed");
        return UX_ERR_AUDIO_START_STREAM_FAIL;
    }
    return UX_ERR_SUCCESS;
}

static errcode_t audio_manager_sco_stream_stop(void)
{
    int32_t ret = AudioStreamInStop(); // 停音频流
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("audio stream in stop failed");
    }

    ret = AudioStreamOutStop(); // 停音频流
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("audio stream out stop failed");
    }

    ret = AudioStreamInDeinit(); // 去初始化音频流
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("audio stream in release failed");
    }

    ret = AudioStreamOutDeinit(); // 去初始化音频流
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("audio stream out release failed");
    }

    ret = AudioManagerDeactivateInterrupt();
    if (ret != UX_ERR_SUCCESS) {
        ux_log_err("deactivate audio interrupt failed");
        return UX_ERR_AUDIO_MEDIA_STREAM_DEINIT_FAILED;
    }
    return UX_ERR_SUCCESS;
}
/**
 * Stop SCO stream.
 * @param       void
 * @return      Audio error status code (0 - Success)
 */
errcode_t audio_manager_sco_stop(void)
{
    ux_log_info("[ux][audio]audio_manager_sco_stop.");

    errcode_t ret;
    ret = audio_manager_sco_stream_stop();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio]audio_manager_sco_stream_stop fail.");
        return ret;
    }
    return ERRCODE_SUCC;
}

void ux_audio_sco_init(void)
{
    g_audio_sco_init = true;
    return;
}

void ux_audio_sco_deinit(void)
{
    g_audio_sco_init = false;
    return;
}

errcode_t ux_audio_sco_set_param(ux_audio_sco_codec *sco_cfg)
{
    ux_log_info("[ux][audio]ux_audio_sco_set_param");
    ux_audio_manager_sco_context *sco_ctx = audio_manager_sco_get_ctx();

    sco_ctx->sample_rate = sco_cfg->sample_rate;
    sco_ctx->codec_format = sco_cfg->codec;
    sco_ctx->sample_bit_width = sco_cfg->bit_width;

    return ERRCODE_SUCC;
}

errcode_t ux_audio_sco_start(void)
{
    ux_log_info("[ux][audio][ux_audio_sco_start]");
    errcode_t ret;
    ret = audio_manager_sco_init();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio]audio_manager_sco_init fail");
        return ret;
    }

    ret = audio_manager_sco_start();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio]audio_manager_sco_start fail");
        return ret;
    }
    g_sco_state_handle.is_started = true;
    return ERRCODE_SUCC;
}

errcode_t ux_audio_sco_stop(void)
{
    ux_log_info("[ux][audio][ux_audio_sco_stop]");
    if (!g_sco_state_handle.is_started) {
        ux_log_err("[ux][audio][audio_manager_sco_stop]sco is not start.");
        return ERRCODE_FAIL;
    }
    errcode_t ret = audio_manager_sco_stop();
    if (ret != ERRCODE_SUCC) {
        ux_log_err("[ux][audio][audio_manager_sco_stop]failed, ret: %d", ret);
        return ERRCODE_FAIL;
    }
    g_sco_state_handle.is_started = false;
    return ERRCODE_SUCC;
}

errcode_t ux_audio_call_set_speaker_volume(uint8_t volume)
{
    /* 转换为media 的1-100 */
    int32_t media_vol = ((uint16_t)volume * MEDIA_VOLUME_MAX) / HFP_VOLUME_MAX;
    if (media_vol > MEDIA_VOLUME_MAX) {
        media_vol = MEDIA_VOLUME_MAX;
    }
    int32_t err = AudioManagerSetVolume(AUDIO_STREAM_VOICE_CALL_BT_SCO, media_vol);
    if (err != UX_ERR_SUCCESS) {
        ux_log_err("[ux_audio_call_set_speaker_volume] Fail to set call volume 0x%x.", err);
        return UX_ERR_AUDIO_SET_VOLUME_FAIL;
    }
    return UX_ERR_SUCCESS;
}