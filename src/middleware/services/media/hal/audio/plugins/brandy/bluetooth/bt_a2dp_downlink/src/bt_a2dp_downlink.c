/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub a2dp downlink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "bt_a2dp_downlink.h"
#include <stdio.h>
#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"
#include "bt_audio_hal_interface.h"
#ifdef CONFIG_SUPPORT_A2DP_SRC
#include "ux_api_bt_audio.h"
#include "ux_errcode.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioBluetoothA2dpDownlinkPort"

#define CAST_SND_PORT_NUM 1
#define BT_DETACH_FAIL_RETRY_TIMES 3

#ifdef AUDIO_SDK_SUPPORT
// debug: SND_0 for speaker playback, SND_1 for mixing
#define CAST_SND_ID UAPI_SND_1
#endif

#define BT_STATE_CHANGE_RETRY_CNT (20)
#define BT_STATE_CHANGE_RETRY_SLEEP_TIME_US (100000)
static int32_t g_subPortSndRefCnt = 0;
static MediaHalMutexHandle g_subPortLock = NULL;

#ifdef BT_HAL_SUPPORT
static bool HaveActiveTrack(const SubPortContext *portCtx)
{
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] != NULL) {
            return true;
        }
    }
    return false;
}

static bool ConvertSampleFrequencyToSampleRate(td_u32 sampleFrequency, uapi_audio_sample_rate *sampleRate)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "sampleFrequency: 0x%x", sampleFrequency);

    switch (sampleFrequency) {
        case BT_AUDIO_A2DP_SBC_SF_16000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_16K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_32000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_32K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_44100:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_44K;
            break;
        case BT_AUDIO_A2DP_SBC_SF_48000:
            *sampleRate = UAPI_AUDIO_SAMPLE_RATE_48K;
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid sample frequency: 0x%x", sampleFrequency);
            return false;
    }

    return true;
}

static bool ConvertChnlModeToChannelCount(td_u8 chnlMode, uapi_audio_channel *channelCount)
{
    MEDIA_HAL_LOGI(MODULE_NAME, "chnlMode: 0x%x", chnlMode);

    switch (chnlMode) {
        case BT_AUDIO_A2DP_SBC_CHMODE_MONO:
            *channelCount = UAPI_AUDIO_CHANNEL_1;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_DUAL_CHANNEL:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_STEREO:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        case BT_AUDIO_A2DP_SBC_CHMODE_JOINT_STEREO:
            *channelCount = UAPI_AUDIO_CHANNEL_2;
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid channel mode: 0x%x", chnlMode);
            return false;
    }
    return true;
}

static int32_t Reconfigure(SubPortContext *portCtx)
{
    bt_a2dp_codec_param param = portCtx->btA2dpConfigChgdata.codec;
    bt_a2dp_sbc_codec_caps *caps = (bt_a2dp_sbc_codec_caps *)param.codec_caps;
    MEDIA_HAL_LOGI(MODULE_NAME, "sbc sampleFreq: %02x chnlMode: %02x blockLen: %02x subBand: %02x \
        allocMtd: %02x minBitpool: %02x, maxBitpool: %02x",
        caps->sample_frequency, caps->chnl_mode, caps->block_length, caps->subband, caps->alloc_method,
        caps->min_bitpool, caps->max_bitpool);
    bool success = ConvertSampleFrequencyToSampleRate(caps->sample_frequency, &portCtx->sampleRate);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid sample frequency");
    success = ConvertChnlModeToChannelCount(caps->chnl_mode, &portCtx->channelCount);
    CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid channel mode");

    if (param.codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", param.codec_type);
        return MEDIA_HAL_ERR;
    }

    // 蓝牙音乐暂停时，蓝牙设备断开、重连，此时BTH会上报属性改变，我们只需记录下来，不作用到AENC模块，
    // 恢复播放的时候，再将新属性作用上去
    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    if (portCtx->a2dpTrackCount == 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "no a2dp track");
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_OK;
    }
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);

    uapi_aenc_attr aencAttr = {};
    aencAttr.codec_id = UAPI_ACODEC_ID_SBC;
    aencAttr.max_trans_unit_size = param.payload;
    aencAttr.param.interleaved = true;
    aencAttr.param.channels = portCtx->channelCount;
    aencAttr.param.bit_depth = AUDIO_BIT_WIDTH_16_BIT;
    aencAttr.param.sample_rate = portCtx->sampleRate;
    aencAttr.param.samples_per_frame = ((uint32_t)portCtx->sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    aencAttr.param.private_data = (td_void *)(portCtx->btA2dpConfigChgdata.codec.codec_caps);
    aencAttr.param.private_data_size = param.cap_len;

    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_set_attr(portCtx->aencHandle, &aencAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_set_attr failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static td_void GetBtA2dpConfigData(const td_void *data, SubPortContext *portCtx)
{
    bt_audio_a2dp_config_chg_data configData = *(bt_audio_a2dp_config_chg_data *)data;
    portCtx->btA2dpConfigChgdata.stream_hdl = configData.stream_hdl;
    portCtx->btA2dpConfigChgdata.codec.codec_type = configData.codec.codec_type;
    portCtx->btA2dpConfigChgdata.codec.cap_len = configData.codec.cap_len;
    portCtx->btA2dpConfigChgdata.codec.payload = configData.codec.payload;

    if (memcpy_s(portCtx->btA2dpConfigChgdata.codec.codec_caps, BT_A2DP_CODECCAPS_MAX_LEN, configData.codec.codec_caps,
        configData.codec.cap_len) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy fail!");
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "codec type: %d, payload: %d, cap_len: %d", configData.codec.codec_type,
        configData.codec.payload, configData.codec.cap_len);
    for (int32_t i = 0; i < configData.codec.cap_len; i++) {
        MEDIA_HAL_LOGI(MODULE_NAME, "codec_caps[%u]:%02x ", i, configData.codec.codec_caps[i]);
    }
    portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CONFIG_CHANGE;

    if (HaveActiveTrack(portCtx)) {
        (void)Reconfigure(portCtx);
    }
}

static void UpdateBluetoothA2dpStreamHandle(SubPortContext *portCtx)
{
    CHK_NULL_RETURN_NONE(portCtx->btA2dpStreamHandle, "a2dp stream handle is null");

    if (!HaveActiveTrack(portCtx)) {
        MEDIA_HAL_LOGD(MODULE_NAME, "bluetooth a2dp stream handle no update required because no active track");
        return;
    }

    for (uint32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] == NULL || portCtx->portTrack[i]->workScene != BLUETOOTH_OUT_A2DP_NORMAL) {
            continue;
        }
        portCtx->portTrack[i]->btStreamHandle = portCtx->btA2dpStreamHandle;
    }
}

static td_void BtEventCB(bt_audio_event_type type, const td_void *data, td_s32 size, td_void *portContext)
{
    MEDIA_HAL_LOGE(MODULE_NAME, "event type = %d, size = %d", type, size);
    SubPortContext *portCtx = (SubPortContext *)portContext;
    switch (type) {
#ifndef MELODY_AUDIO_SUPPORT
        case BT_AUDIO_A2DP_SNK_STREAM_CREATE:
            portCtx->btA2dpSinkStreamHandle = *((td_pvoid *)data);
            break;
        case BT_AUDIO_A2DP_SNK_STREAM_CLOSED:
            portCtx->btA2dpSinkStreamHandle = NULL;
            break;
#endif
        case BT_AUDIO_A2DP_STREAM_CREATE:
            portCtx->btA2dpStreamHandle = *((td_pvoid *)data);
            // 实时更新蓝牙A2DP流句柄，比如蓝牙音乐暂停时刻，蓝牙设备断开再连接，
            // 流句柄会变化，需要同步更新到内部维护的track中
            UpdateBluetoothA2dpStreamHandle(portCtx);
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CREATE;
            break;
        case BT_AUDIO_A2DP_STREAM_OPENED: {
            // 首次播放通过OPENED事件上报准确mtu值，需要传给DSP编码打包，此时CONFIG_CHANGE事件上报的是无效值，
            // 播放中途CONFIG_CHANGE会上报准确mtu值
            bt_audio_a2dp_stream_open_data openData = *(bt_audio_a2dp_stream_open_data *)data;
            portCtx->btA2dpConfigChgdata.codec.payload = openData.stream_mtu;
        }
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_OPENED;
            break;
        case BT_AUDIO_A2DP_STREAM_STRAT:
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_STRAT;
            break;
        case BT_AUDIO_A2DP_STREAM_SUSPENDED:
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_SUSPENDED;
            break;
        case BT_AUDIO_A2DP_STREAM_CLOSED:
            portCtx->btA2dpStreamHandle = NULL;
            portCtx->btDevState = BT_AUDIO_A2DP_STREAM_CLOSED;
            portCtx->btAencAttrReceived = false;
            break;
        case BT_AUDIO_A2DP_STREAM_CONFIG_CHANGE: {
            GetBtA2dpConfigData(data, portCtx);
            portCtx->btAencAttrReceived = true;
            break;
        }
        case BT_AUDIO_SCO_CONN_ESTABLISHED:
            portCtx->btScoStreamHandle = *((td_pvoid *)data);
            portCtx->btDevState = BT_AUDIO_SCO_CONN_ESTABLISHED;
            break;
        case BT_AUDIO_SCO_CONN_RELEASED:
            portCtx->btScoStreamHandle = NULL;
            portCtx->btDevState = BT_AUDIO_SCO_CONN_RELEASED;
            break;
        default:
            MEDIA_HAL_LOGI(MODULE_NAME, "invalid callback event type: %d", type);
            break;
    }
}
#endif

int32_t BtEventCBCreate(SubPortContext *portCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    int32_t ret = (int32_t)bt_register_audio_listener(BtEventCB, portCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt_register_audio_listener failed");
#else
    MEDIA_HAL_UNUSED(portCtx);
#endif
    return MEDIA_HAL_OK;
}

int32_t BtEventCBDestroy(void)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    int32_t ret = (int32_t)bt_deregister_audio_listener(BtEventCB);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt_deregister_audio_listener failed");
#endif
    return MEDIA_HAL_OK;
}

static bool GetBTStreamHandle(const SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#ifdef BT_HAL_SUPPORT
    /* check bluetooth stream handle is vailed */
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        if (portCtx->btScoStreamHandle != NULL) {
            portTrackCtx->btStreamHandle = portCtx->btScoStreamHandle;
        } else {
            MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth sco no valid stream handle!");
            return false;
        }
    } else if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        if (portCtx->btA2dpSinkStreamHandle == NULL) {
#ifdef MELODY_AUDIO_SUPPORT
            if (portCtx->btA2dpStreamHandle == NULL) {
                MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth a2dp sink no valid stream handle!");
                return false;
            }
            portTrackCtx->btStreamHandle = portCtx->btA2dpStreamHandle;
            return true;
#endif
        }
        portTrackCtx->btStreamHandle = portCtx->btA2dpSinkStreamHandle;
    } else if (portTrackCtx->workScene == BLUETOOTH_OUT_A2DP_NORMAL) {
        if (portCtx->btA2dpStreamHandle == NULL) {
            MEDIA_HAL_LOGE(MODULE_NAME, "work scene bluetooth a2dp source no valid stream handle!");
            return false;
        }
        portTrackCtx->btStreamHandle = portCtx->btA2dpStreamHandle;
    }
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    return true;
}

static int32_t SndCreateTrack(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    uapi_snd_track_attr defTrackAttr = {};
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_track_default_attr(&defTrackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_get_track_default_attr failed");

    MEDIA_HAL_DOFUNC_TIME(
        ret = uapi_snd_create_track(&(portTrackCtx->trackHandle), portTrackCtx->castSndHandle, &defTrackAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_create_track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t AudioSDKAttachAdpAndAdec(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        return ret;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    }
    return ret;
}
#endif

static int32_t SndAttach(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "input adp open failed");
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach track failed");
            goto ADP_CLOSE;
        }
    } else {
        ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "input adp open failed");
        ret = IsNeedSetExtraFlag(portTrackCtx->inputAttr.audioFormat, &portTrackCtx->inputAttr.isNeedSetExtraParam);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "set extra flag failed");
            goto ADP_CLOSE;
        }
        if (portTrackCtx->inputAttr.isNeedSetExtraParam) {
            MEDIA_HAL_LOGI(MODULE_NAME, "need SetExtraParam adec and attach delay");
            return MEDIA_HAL_OK;
        }
        ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
            goto ADP_CLOSE;
        }
        ret = AudioSDKAttachAdpAndAdec(portTrackCtx);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
            goto ADEC_CLOSE;
        }
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
    return ret;
#endif
}

static int32_t SndDestroyTrack(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = uapi_snd_destroy_track(portTrackCtx->trackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_destroy_track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t CastSndClose(td_handle sndHandle)
{
    MediaHalInitStaticMutexLock(&g_subPortLock);
    MediaHalMutexLock(g_subPortLock);
    g_subPortSndRefCnt--;
    if (g_subPortSndRefCnt == 0) {
        int32_t ret;
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_close(sndHandle));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_close failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
    }
    MediaHalMutexUnLock(g_subPortLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t StopByAudioModule(const SubPortTrackContext *portTrackCtx, AudioSDKModuleType module)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    switch (module) {
        case AUDIO_MOD_AENC: {
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->aencHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_stop failed");
            break;
        }
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_stop failed");
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio module: %d", module);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(module);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "stop success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static void DecreaseTrackCount(SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        return;
    }
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        portTrackCtx->subPortCtx->scoTrackCount--;
    } else {
#ifdef MINI_MEMORY_SUPPORT
        if (!portTrackCtx->isReleased) {
            portTrackCtx->subPortCtx->a2dpTrackCount--;
        }
#else
        portTrackCtx->subPortCtx->a2dpTrackCount--;
#endif
    }
}

static int32_t StartBTStream(SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    uint32_t ret = 0;
    uint32_t cnt = 0;
    if (!portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_LOGT(MODULE_NAME, "Has already start BT stream");
        return MEDIA_HAL_OK;
    }
    if (portTrackCtx->subPortCtx->btAencAttrReceived) {
        while ((portTrackCtx->subPortCtx->btDevState == BT_AUDIO_A2DP_STREAM_STRAT) && cnt++ < BT_STATE_CHANGE_RETRY_CNT) {
            usleep(BT_STATE_CHANGE_RETRY_SLEEP_TIME_US);
        }
        if (cnt >= BT_STATE_CHANGE_RETRY_CNT) {
            MEDIA_HAL_LOGE(MODULE_NAME, "wait a2dpStream suspend timed out!");
            return MEDIA_HAL_ERR;
        }
        ret = bt_start_audio_stream(portTrackCtx->btStreamHandle);
        MEDIA_HAL_LOGT(MODULE_NAME, "bt_start_audio_stream with btDevState:%x, ret:%x", portTrackCtx->subPortCtx->btDevState, ret);
    } else {
#ifdef CONFIG_SUPPORT_A2DP_SRC
        uint16_t deviceNum = 0;
        const ux_api_bt_device_info_t *deviceInfo = NULL;
        deviceInfo = ux_api_bt_get_device_info(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR, &deviceNum);
        CHK_NULL_RETURN(deviceInfo, MEDIA_HAL_ERR, "ux_api_bt_audio_stop failed!");
        ret = ux_api_bt_audio_start(UX_API_BT_PROFILE_A2DP, &deviceInfo->addr);
        MEDIA_HAL_LOGT(MODULE_NAME, "ux_api_bt_audio_start ret:%x", ret);
#endif
    }
    return ret == 0 ? MEDIA_HAL_OK : MEDIA_HAL_ERR;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    return 0;
#endif
}

static void StopBTStream(const SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (!portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_LOGT(MODULE_NAME, "There is other A2DP downlink stream, dont stop BT stream now");
        return;
    }
    uint32_t cnt = 0;
    if (portTrackCtx->subPortCtx->btAencAttrReceived) {
        while ((portTrackCtx->subPortCtx->btDevState != BT_AUDIO_A2DP_STREAM_STRAT) && cnt++ < BT_STATE_CHANGE_RETRY_CNT) {
            usleep(BT_STATE_CHANGE_RETRY_SLEEP_TIME_US);
        }
        if (cnt >= BT_STATE_CHANGE_RETRY_CNT) {
            MEDIA_HAL_LOGE(MODULE_NAME, "wait a2dpStream start timed out!");
            return;
        }
        td_u32 btRet = bt_stop_audio_stream(portTrackCtx->btStreamHandle);
        MEDIA_HAL_LOGT(MODULE_NAME, "bt_stop_audio_stream with btDevState:%x ret:%x", portTrackCtx->subPortCtx->btDevState, btRet);
    } else {
#ifdef CONFIG_SUPPORT_A2DP_SRC
        uint16_t deviceNum = 0;
        const ux_api_bt_device_info_t *deviceInfo = NULL;
        deviceInfo = ux_api_bt_get_device_info(UX_API_BT_AS_PERIPHERAL, UX_API_BT_PROTOCOL_BR, &deviceNum);
        CHK_NULL_RETURN_NONE(deviceInfo, "ux_api_bt_audio_stop failed!");
        errcode_t ret = ux_api_bt_audio_stop(UX_API_BT_PROFILE_A2DP, &deviceInfo->addr);
        MEDIA_HAL_LOGT(MODULE_NAME, "ux_api_bt_audio_stop ret=0x%x", ret);
#endif
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
}

static int32_t SingletonResourceStopInternal(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = StopByAudioModule(portTrackCtx, AUDIO_MOD_AENC);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");
#endif
    StopBTStream(portTrackCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
static int32_t BtDetachHandle(const SubPortTrackContext *portTrackCtx, bt_audio_port_params portParam)
{
    int32_t count = 0;
    td_u32 btRet = MEDIA_HAL_OK;
    while (count < BT_DETACH_FAIL_RETRY_TIMES) {
        btRet = bt_detach_audio_port(portTrackCtx->btStreamHandle, &portParam);
        if (btRet == MEDIA_HAL_OK) {
            MEDIA_HAL_LOGI(MODULE_NAME, "bt detach audio port success!");
            return MEDIA_HAL_OK;
        }
        count++;
        MEDIA_HAL_LOGE(MODULE_NAME, "bt detach audio port retry ret = %x, count: %d", count, btRet);
    }
    return MEDIA_HAL_ERR;
}
#endif

static int32_t BluetoothDetach(const SubPortContext *portCtx, const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portCtx->outputAdpHandle;
    ret = BtDetachHandle(portTrackCtx, portParam);
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return ret;
}

int32_t AbrogationTranscodeBackendPipeline(const SubPortContext *portCtx,
    SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    MEDIA_HAL_UNUSED(portCtx);
    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);
    DecreaseTrackCount(portTrackCtx);
    if (portTrackCtx->subPortCtx->a2dpTrackCount > 0) {
        MEDIA_HAL_LOGI(MODULE_NAME, "in the multi-instance scenario, need keep one aenc");
        (void)CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
        return MEDIA_HAL_OK;
    }

#ifdef MINI_MEMORY_SUPPORT
        // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
        // 1.蓝牙音乐暂停状态退出播放
        // 2.蓝牙音乐暂停状态断开蓝牙耳机
        // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
        if (portTrackCtx->isReleased) {
            MEDIA_HAL_LOGI(MODULE_NAME, "have released");
            return MEDIA_HAL_OK;
        }
#endif

    int32_t ret = SingletonResourceStopInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "singleton resource stop internal failed");

    ret = BluetoothDetach(portCtx, portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bt detach failed");
    ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
    ret = AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound detach aenc failed");
    ret = AudioSDKAdpClose(portTrackCtx->outputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    ret = AudioSDKAencClose(portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    ret = CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "CastSndClose failed");
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static void IncrementTrackCount(SubPortTrackContext *portTrackCtx)
{
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL) {
        return;
    }
    if (portTrackCtx->workScene == BLUETOOTH_SCO) {
        portTrackCtx->subPortCtx->scoTrackCount++;
    } else {
        portTrackCtx->subPortCtx->a2dpTrackCount++;
    }
}

static int32_t CastSndOpen(td_handle *sndHandle, uapi_audio_sample_rate sampleRate, uapi_audio_channel channelCount)
{
    MediaHalInitStaticMutexLock(&g_subPortLock);
    MediaHalMutexLock(g_subPortLock);
    if (g_subPortSndRefCnt == 0) {
        int32_t ret;
        uapi_snd_attr sndAttr = {};
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_get_default_attr(CAST_SND_ID, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_get_default_attr failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
        sndAttr.port_num = CAST_SND_PORT_NUM;
        sndAttr.port_attr[0].out_port = UAPI_SND_OUT_PORT_CAST0;
        /* current platform only support 16 bitwidth */
        sndAttr.bit_depth = UAPI_AUDIO_BIT_DEPTH_16;
        sndAttr.sample_rate = sampleRate;
        sndAttr.channels = channelCount;

        MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_open(sndHandle, CAST_SND_ID, &sndAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_snd_open failed");
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
#ifndef MELODY_AUDIO_SUPPORT
        ret = uapi_snd_set_track_mode(*sndHandle, sndAttr.port_attr[0].out_port, SND_TRACK_MODE_BT_MUSIC);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "set track mode failed, ret = %d", ret);
            MediaHalMutexUnLock(g_subPortLock);
            return ret;
        }
#endif
    }
    g_subPortSndRefCnt++;
    MediaHalMutexUnLock(g_subPortLock);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t InitAencAttr(SubPortContext *portCtx, uapi_aenc_attr *aencAttr)
{
#ifdef BT_HAL_SUPPORT
    const bt_a2dp_codec_param *codecParams = &portCtx->btA2dpConfigChgdata.codec;
    if (codecParams->codec_type != BT_AUDIO_CODEC_SBC) {
        MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", codecParams->codec_type);
        return MEDIA_HAL_ERR;
    }
    aencAttr->codec_id = UAPI_ACODEC_ID_SBC;
    aencAttr->param.interleaved = true;
    aencAttr->param.channels = portCtx->channelCount;
    aencAttr->param.bit_depth = AUDIO_BIT_WIDTH_16_BIT;
    aencAttr->param.sample_rate = portCtx->sampleRate;
    aencAttr->param.samples_per_frame = ((uint32_t)portCtx->sampleRate) / SAMPLERATE_TO_PTNUMPERFRM;
    if (portCtx->btAencAttrReceived) {
        aencAttr->max_trans_unit_size = codecParams->payload;
        bt_a2dp_sbc_codec_caps *btCaps = (bt_a2dp_sbc_codec_caps *)(portCtx->btA2dpConfigChgdata.codec.codec_caps);
        portCtx->audioCap[0x0] = btCaps->chnl_mode | btCaps->sample_frequency;
        portCtx->audioCap[0x1] = btCaps->alloc_method | btCaps->subband | btCaps->block_length;
        portCtx->audioCap[0x2] = btCaps->min_bitpool;
        portCtx->audioCap[0x3] = btCaps->max_bitpool;
    } else {
        aencAttr->max_trans_unit_size = portCtx->aencAttr.payload;
        portCtx->audioCap[0x0] = portCtx->aencAttr.channelMode | portCtx->aencAttr.sampleRate;
        portCtx->audioCap[0x1] = portCtx->aencAttr.allocMethod |
            portCtx->aencAttr.subband | portCtx->aencAttr.blockLength;
        portCtx->audioCap[0x2] = portCtx->aencAttr.minBitpool;
        portCtx->audioCap[0x3] = portCtx->aencAttr.maxBitpool;
    }
    aencAttr->param.private_data = (td_void *)portCtx->audioCap;
    aencAttr->param.private_data_size = sizeof(a2dp_sbc_codec_caps);

    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat: 0x%x, sampleRate: %d, channelCount: %u, bitWidth: %u, payload: %u",
        AUDIO_FORMAT_TYPE_SBC, portCtx->sampleRate, portCtx->channelCount,
        AUDIO_BIT_WIDTH_16_BIT, portCtx->aencAttr.payload);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(aencAttr);
#endif
    return MEDIA_HAL_OK;
}

static int32_t BluetoothAttachInternal(const SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portTrackCtx->isHandlePublicResource) {
        bt_audio_port_params portParam = {};
        portParam.port_type = A2DP;
        portParam.share_mem_id = portTrackCtx->outputAdpHandle;
        td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
        CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt attach audio port failed");
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t StartByAudioModule(const SubPortTrackContext *portTrackCtx, AudioSDKModuleType module)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    uapi_audio_scene_config asc = {
        .scene_type = UAPI_AUDIO_SCENE_BT_MUSIC_SRC,
        .scene_id = 0,
    };
    if (portTrackCtx->inputAttr.type == AUDIO_IN_NOTIFICATION_SYSTEM) {
        asc.scene_type = UAPI_AUDIO_SCENE_BT_TONE;
    }

    switch (module) {
        case AUDIO_MOD_AENC:
            ret = uapi_audio_set_audio_scene(portTrackCtx->aencHandle, &asc);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_audio_set_audio_scene failed");
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->aencHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_aenc_start failed");
            break;
        case AUDIO_MOD_ADEC:
            MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecHandle));
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adec_start failed");
            break;
        default:
            MEDIA_HAL_LOGE(MODULE_NAME, "invalid audio module: %d", module);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(module);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "start success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

static int32_t SingletonResourceStartInternal(const SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    portTrackCtx->aencHandle = portCtx->aencHandle;
    int32_t ret = StartByAudioModule(portTrackCtx, AUDIO_MOD_AENC);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc start failed");
#endif
    return StartBTStream(portTrackCtx);
}

static int32_t BluetoothAttach(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx)
{
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bt_audio_port_params portParam = {};
    portParam.port_type = A2DP;
    portParam.share_mem_id = portCtx->outputAdpHandle;
    td_u32 btRet = bt_attach_audio_port(portTrackCtx->btStreamHandle, &portParam);
    CHK_FAILED_RETURN(btRet, MEDIA_HAL_OK, MEDIA_HAL_ERR, "bt attach audio port failed");
#else
    MEDIA_HAL_UNUSED(portCtx);
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t EstablishPipelineInternal(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx,
    int32_t trackCount)
{
#ifdef AUDIO_SDK_SUPPORT
    if (trackCount > 1) {
        MEDIA_HAL_LOGI(MODULE_NAME, "in the multi-instance scenario, only one aenc is required");
        return MEDIA_HAL_OK;
    }

    uapi_aenc_attr aencAttr = {};
    int32_t ret = InitAencAttr(portCtx, &aencAttr);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "InitAencAttr failed");

    ret = AudioSDKAencOpen(&portCtx->aencHandle, aencAttr, AENC_TYPE_BT, &(portTrackCtx->inputAttr));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc open failed");

    ret = AudioSDKAdpOpen(&portCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp open failed");
        goto AENC_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_SOUND, portCtx->castSndHandle, portCtx->aencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound attach aenc failed");
        goto ADP_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_AENC, portCtx->aencHandle, portCtx->outputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
        goto SND_DETACH_AENC;
    }
#endif
    ret = BluetoothAttach(portCtx, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        goto SND_DETACH_AENC;
    }

    ret = SingletonResourceStartInternal(portCtx, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "singleton resource start internal failed");
        goto BLUETOOTH_DETACH;
    }
    return MEDIA_HAL_OK;
BLUETOOTH_DETACH:
    (void)BluetoothDetach(portCtx, portTrackCtx);
#ifdef AUDIO_SDK_SUPPORT
SND_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_AENC, portCtx->aencHandle, portCtx->outputAdpHandle);
    (void)AudioSDKDetach(AUDIO_MOD_SOUND, portCtx->castSndHandle, portCtx->aencHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portCtx->outputAdpHandle);
AENC_CLOSE:
    (void)AudioSDKAencClose(portCtx->aencHandle);
#endif
    return ret;
}

int32_t EstablishTranscodeBackendPipeline(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx,
    int32_t trackId)
{
    MEDIA_HAL_UNUSED(trackId);
    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);
    IncrementTrackCount(portTrackCtx);
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portCtx->btAencAttrReceived) {
        bt_a2dp_codec_param param = portCtx->btA2dpConfigChgdata.codec;
        if (param.codec_type != BT_AUDIO_CODEC_SBC) {
            MEDIA_HAL_LOGE(MODULE_NAME, "unsupport codec type: 0x%x", param.codec_type);
            return MEDIA_HAL_ERR;
        }
        bt_a2dp_sbc_codec_caps *caps = (bt_a2dp_sbc_codec_caps *)param.codec_caps;
        MEDIA_HAL_LOGI(MODULE_NAME, "sbc sampleFreq: %02x chnlMode: %02x blockLen: %02x subBand: %02x \
            allocMtd: %02x minBitpool: %02x maxBitpool: %02x",
            caps->sample_frequency, caps->chnl_mode, caps->block_length, caps->subband, caps->alloc_method,
            caps->min_bitpool, caps->max_bitpool);
        bool success = ConvertSampleFrequencyToSampleRate(caps->sample_frequency, &portCtx->sampleRate);
        CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid sample frequency");
        success = ConvertChnlModeToChannelCount(caps->chnl_mode, &portCtx->channelCount);
        CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid channel mode");
    } else {
        bool success = ConvertSampleFrequencyToSampleRate(portCtx->aencAttr.sampleRate, &portCtx->sampleRate);
        CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid sample frequency");
        success = ConvertChnlModeToChannelCount(portCtx->aencAttr.channelMode, &portCtx->channelCount);
        CHK_COND_RETURN(!success, MEDIA_HAL_INVALID_PARAM, "invalid channel mode");
    }
#endif
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = CastSndOpen(&portCtx->castSndHandle, portCtx->sampleRate, portCtx->channelCount);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "CastSndOpen failed");

    ret = EstablishPipelineInternal(portCtx, portTrackCtx, portCtx->a2dpTrackCount);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "EstablishPipelineInternal failed");
        goto SND_CLOSE;
    }

    portTrackCtx->trackCount = portCtx->a2dpTrackCount;
    portTrackCtx->castSndHandle = portCtx->castSndHandle;
    portTrackCtx->aencHandle = portCtx->aencHandle;
    portTrackCtx->outputAdpHandle = portCtx->outputAdpHandle;
#endif
    return MEDIA_HAL_OK;

#ifdef AUDIO_SDK_SUPPORT
SND_CLOSE:
    (void)CastSndClose(portCtx->castSndHandle);
    return ret;
#endif
}

int32_t CreateTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    if (portTrackCtx->subPortCtx->btAencAttrReceived) {
        if (!GetBTStreamHandle(portTrackCtx->subPortCtx, portTrackCtx)) {
            MEDIA_HAL_LOGE(MODULE_NAME, "GetBTStreamHandle failed");
            return MEDIA_HAL_ERR;
        }
    }
    int32_t ret = 0;
    ret = EstablishTranscodeBackendPipeline(portTrackCtx->subPortCtx, portTrackCtx, portTrackCtx->trackId);
    if (ret != MEDIA_HAL_OK) {
        DecreaseTrackCount(portTrackCtx);
        goto FREE_TRACK_CTX;
    }
#ifdef AUDIO_SDK_SUPPORT
    ret = SndCreateTrack(portTrackCtx);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "SndCreateTrack failed");

    ret = SndAttach(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SndAttach failed");
        goto DESTROY_TRACK;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    CreateAudioLinkChr(portTrackCtx->inputAttr.type, true, true);
    return MEDIA_HAL_OK;
#ifdef AUDIO_SDK_SUPPORT
DESTROY_TRACK :
    (void)SndDestroyTrack(portTrackCtx);
    (void)AbrogationTranscodeBackendPipeline(portTrackCtx->subPortCtx, portTrackCtx);
#endif
FREE_TRACK_CTX:
    MediaHalFree(portTrackCtx);
    return ret;
}

static int32_t SndDetach(const SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach track failed");
        ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");
        ret = AudioSDKAdecClose(portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");
        ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
    }

    ret = SndDestroyTrack(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound destroy track failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t DestroyTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
    // 1.蓝牙音乐暂停状态退出播放
    // 2.蓝牙音乐暂停状态断开蓝牙耳机
    // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif

#ifdef AUDIO_SDK_SUPPORT
   int32_t ret = SndDetach(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SndDetach failed");
        return MEDIA_HAL_ERR;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    DestoryAudioLinkChr(portTrackCtx->inputAttr.type, true, true);
    ret = AbrogationTranscodeBackendPipeline(portTrackCtx->subPortCtx, portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AbrogationTranscodeBackendPipeline failed");
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

#ifdef AUDIO_SDK_SUPPORT
static int32_t SetExtraDataForAdecAndAttach(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        return ret;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        goto ADEC_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
        goto ADP_DETACH_ADEC;
    }
    return MEDIA_HAL_OK;

ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
    return ret;
}

static int32_t AdpEventCB(td_handle adpHandle, uapi_adp_event_type event, td_void *param, td_void *context)
{
    MEDIA_HAL_UNUSED(adpHandle);
    MEDIA_HAL_UNUSED(param);
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)context;
    if (portTrackCtx->callback == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "callback is null");
        return MEDIA_HAL_ERR;
    }
    int32_t ret;
    switch (event) {
        case UAPI_ADP_EVENT_EOS:
            ret = portTrackCtx->callback(AUDIO_RENDER_EOS, NULL, 0, portTrackCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event eos callback failed");
            break;
#ifndef MELODY_AUDIO_SUPPORT
        case UAPI_ADP_EVENT_EMPTY:
            ret = portTrackCtx->callback(AUDIO_BUFFER_EMPTY, NULL, 0, portTrackCtx->caller);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adp event buffer empty callback failed");
            break;
#endif
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid event: %d", event);
            break;
    }

    return MEDIA_HAL_OK;
}
#endif

int32_t EnableTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->inputAttr.isNeedSetExtraParam) {
        if (SetExtraDataForAdecAndAttach(portTrackCtx) != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "SetExtraDataAdecAndAttach failed");
            return MEDIA_HAL_ERR;
        }
    }

    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackCtx));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_adp_register_event_proc failed");
    uapi_audio_scene_config asc = {
        .scene_type = UAPI_AUDIO_SCENE_BT_MUSIC_SRC,
        .scene_id = 0,
    };
    if (portTrackCtx->inputAttr.type == AUDIO_IN_NOTIFICATION_SYSTEM) {
        asc.scene_type = UAPI_AUDIO_SCENE_BT_TONE;
    }

    ret = uapi_audio_set_audio_scene(portTrackCtx->inputAdpHandle, &asc);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_audio_set_audio_scene failed");

    ret = uapi_snd_track_start(portTrackCtx->trackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_start failed");

    if (!portTrackCtx->isRaw) {
        ret = StartByAudioModule(portTrackCtx, AUDIO_MOD_ADEC);             // 需要开出来
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "StartAudioModule ADEC failed");
    }
#endif
    StartAudioLinkChr(portTrackCtx->inputAttr.type, true, true);
    ret = SubPortTrackSetPause(portTrackCtx->trackId, false);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " pause track failed");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

int32_t DisableTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停时刻会销毁通路，同时正常退出和切换输出设备也会销毁通路，避免重复销毁，需要检查下，典型场景如下：
    // 1.蓝牙音乐暂停状态退出播放
    // 2.蓝牙音乐暂停状态断开蓝牙耳机
    // 3.蓝牙音乐播放中途接听电话，通话中途断开耳机，通话挂断后恢复播放
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif
    int32_t ret = uapi_snd_track_stop(portTrackCtx->trackHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_track_stop failed");

    if (!portTrackCtx->isRaw) {
        ret = StopByAudioModule(portTrackCtx, AUDIO_MOD_ADEC);                        // 需要开出来
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "StopByAudioModule ADEC failed");
    }
#endif
    StopAudioLinkChr(portTrackCtx->inputAttr.type, true, true);
    ret = SubPortTrackSetPause(portTrackCtx->trackId, true);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " pause track failed");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);
    return MEDIA_HAL_OK;
}

int32_t TrackSetMuteInA2DPSourceNormalScene(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetMuteInA2DPSourceNormalScene(AudioHandle trackHandle, bool* mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetVolumeInA2DPSourceNormalScene(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
#ifdef MINI_MEMORY_SUPPORT
    // 目前支持 SDK 小型化方案，暂停的时候会销毁通路，设置音量操作会失败，此时需要记录音量信息，在恢复播放的时候作用上去
    if (portTrackCtx->isReleased) {
        portTrackCtx->volume = volume;
        return MEDIA_HAL_OK;
    }
#endif
    int32_t ret = AudioSDKSetTrackVolume(portTrackCtx->trackHandle, volume, portTrackCtx->inputAttr.type);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uapi_snd_set_track_volume failed");
    portTrackCtx->volume = volume;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
#endif

    MEDIA_HAL_LOGI(MODULE_NAME, "set volume %f success", volume);
    return MEDIA_HAL_OK;
}

int32_t TrackGetVolumeInA2DPSourceNormalScene(AudioHandle trackHandle, float* volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    CHK_NULL_RETURN(volume, MEDIA_HAL_ERR, "volume is NULL");
    *volume = portTrackCtx->volume;
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKStopInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = uapi_snd_track_stop(portTrackCtx->trackHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound track stop failed");

    if (!portTrackCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->aencHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothDetachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = MEDIA_HAL_OK;
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    if (portTrackCtx->isHandlePublicResource) {
        bt_audio_port_params portParam = {};
        portParam.port_type = A2DP;
        portParam.share_mem_id = portTrackCtx->outputAdpHandle;
        ret = BtDetachHandle(portTrackCtx, portParam);
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return ret;
}

static int32_t AudioSDKDetachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    if (portTrackCtx->isRaw) {
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach track failed");
    } else {
        ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");
        ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach adec failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach adp failed");
        ret = AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound detach aenc failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKCloseInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_snd_destroy_track(portTrackCtx->trackHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound destroy track failed");
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKAdpClose(portTrackCtx->outputAdpHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

        ret = AudioSDKAencClose(portTrackCtx->aencHandle);
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");
    }
    ret = CastSndClose(portTrackCtx->subPortCtx->castSndHandle);              // 需要开出去
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "sound close failed");

    ret = AudioSDKAdecClose(portTrackCtx->adecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");

    ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t PauseTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    int32_t ret = SubPortTrackSetPause(portTrackCtx->trackId, true);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " pause track failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return MEDIA_HAL_ERR;
    }
    portTrackCtx->isHandlePublicResource = IsNeedHandlePublicResource();
    // pick up bluetooth voice call when bluetooth music playback
    // 1. sdk stop/detach/close and bt detach for save shared memory
    // 2. sdk aenc close because of only one encoder is supported
    ret = AudioSDKStopInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio stop failed");
#ifdef BT_HAL_SUPPORT
    ret = BluetoothDetachInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "bluetooth detach failed");
#endif
    ret = AudioSDKDetachInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio detach failed");
    ret = AudioSDKCloseInternal(portTrackCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "audio close failed");
    portTrackCtx->isReleased = true;
    portTrackCtx->subPortCtx->a2dpTrackCount--;
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
#endif
    PauseAudioLinkChr(portTrackCtx->inputAttr.type, true);
    StopBTStream(portTrackCtx);
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKOpenInternal(SubPortTrackContext *portTrackCtx)
{
    int32_t ret = CastSndOpen(&portTrackCtx->subPortCtx->castSndHandle, portTrackCtx->subPortCtx->sampleRate,
        portTrackCtx->subPortCtx->channelCount);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound open failed");

    portTrackCtx->castSndHandle = portTrackCtx->subPortCtx->castSndHandle;
    ret = SndCreateTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "sound create track failed");
        goto SOUND_CLOSE;
    }

    ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "input adp open failed");
        goto DESTROY_TRACK;
    }

    ret = AudioSDKAdecOpen(&(portTrackCtx->adecHandle), portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto ADP_CLOSE;
    }
    if (portTrackCtx->isHandlePublicResource) {
        uapi_aenc_attr aencAttr = {};
        ret = InitAencAttr(portTrackCtx->subPortCtx, &aencAttr);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "init aenc attr failed");
        ret = AudioSDKAencOpen(&portTrackCtx->aencHandle, aencAttr, AENC_TYPE_BT, &(portTrackCtx->inputAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "audio attach failed");
            goto ADEC_CLOSE;
        }
        portTrackCtx->subPortCtx->aencHandle = portTrackCtx->aencHandle;

        ret = AudioSDKAdpOpen(&portTrackCtx->outputAdpHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "output adp open failed");
            goto AENC_CLOSE;
        }
        portTrackCtx->subPortCtx->outputAdpHandle = portTrackCtx->outputAdpHandle;
    }
    return MEDIA_HAL_OK;

AENC_CLOSE:
    (void)AudioSDKAencClose(portTrackCtx->aencHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
ADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
DESTROY_TRACK:
    (void)uapi_snd_destroy_track(portTrackCtx->trackHandle);
SOUND_CLOSE:
    (void)CastSndClose(portTrackCtx->subPortCtx->castSndHandle);
    return ret;
}

static int32_t AudioSDKAttachInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret = 0;
    if (portTrackCtx->isHandlePublicResource) {
        ret = AudioSDKAttach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound attach aenc failed");

        ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach adp failed");
            goto SND_DETACH_AENC;
        }
    }

    if (portTrackCtx->isRaw) {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach track failed");
            goto AENC_DETACH_ADP;
        }
    } else {
        ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
            goto AENC_DETACH_ADP;
        }
        ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->trackHandle);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "adec attach track failed");
            goto ADP_DETACH_ADEC;
        }
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
AENC_DETACH_ADP:
    (void)AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputAdpHandle);
SND_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_SOUND, portTrackCtx->castSndHandle, portTrackCtx->aencHandle);
    return ret;
}

static int32_t AudioSDKStartInternal(const SubPortTrackContext *portTrackCtx)
{
    int32_t ret;
    uapi_adp_event_proc adpCallback = AdpEventCB;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adp_register_event_proc(portTrackCtx->inputAdpHandle, adpCallback,
        (td_void *)portTrackCtx));
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "register adp event failed");

    ret = uapi_snd_track_start(portTrackCtx->trackHandle, TD_NULL);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "sound track start failed");

    if (!portTrackCtx->isRaw) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "adec start failed");
    }
    if (portTrackCtx->isHandlePublicResource) {
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->aencHandle));
        CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "aenc start failed");
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t ResumeTrackInA2DPSourceNormalScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    // 蓝牙音乐+蓝牙通话交互场景，手表蓝牙去使能，此时手机和蓝牙设备同时断开，在音乐恢复的时候可能设备已经断开
#if defined(BT_HAL_SUPPORT) && defined(SUPPORT_BREDR)
    bool a2dpStreamClosed = (portTrackCtx->subPortCtx->btDevState == BT_AUDIO_A2DP_STREAM_CLOSED);
    CHK_COND_RETURN(a2dpStreamClosed, MEDIA_HAL_OK, "a2dp stream has been closed");
#endif

#ifdef MINI_MEMORY_SUPPORT
    MediaHalMutexLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    uapi_audio_scene_config asc = {
        .scene_type = UAPI_AUDIO_SCENE_BT_MUSIC_SRC,
        .scene_id = 0,
    };
    if (portTrackCtx->inputAttr.type == AUDIO_IN_NOTIFICATION_SYSTEM) {
        asc.scene_type = UAPI_AUDIO_SCENE_BT_TONE;
    }
    portTrackCtx->isHandlePublicResource = IsNeedHandlePublicResource();
    int32_t ret = SubPortTrackSetPause(portTrackCtx->trackId, false);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, " resume track failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return MEDIA_HAL_ERR;
    }
    // 该判断放在设置状态后面的原因：两路混音场景，第一路恢复播放，需要重新搭建通路，
    // 第二路恢复播放，不需要搭建通路，如果判断放在前面，就会直接返回，不会设置状态,
    // 因此，当前代码逻辑简单理解，先设置状态，再判断是否搭建通路

    ret = AudioSDKOpenInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio open failed");
        MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
        return ret;
    }

    ret = AudioSDKAttachInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio attach failed");
        goto AUDIO_CLOSE;
    }
    ret = BluetoothAttachInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth attach failed");
        goto AUDIO_DETACH;
    }

    ret = uapi_audio_set_audio_scene(portTrackCtx->inputAdpHandle, &asc);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio set scene failed");
        goto BT_DETACH;
    }

    ret = AudioSDKStartInternal(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "audio start failed");
        goto BT_DETACH;
    }
    portTrackCtx->isReleased = false;
    portTrackCtx->subPortCtx->a2dpTrackCount++;
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    ret = StartBTStream(portTrackCtx);
    if (ret != 0) {
        MEDIA_HAL_LOGE(MODULE_NAME, "StartBTStream failed");
        goto AUDIO_STOP;
    }
#endif
    ResumeAudioLinkChr(portTrackCtx->inputAttr.type, true);
#ifdef BT_HAL_SUPPORT
    MEDIA_HAL_LOGI(MODULE_NAME, "success trackId (%d), bt device state (%d)",
        portTrackCtx->trackId, portTrackCtx->subPortCtx->btDevState);
#endif

    return MEDIA_HAL_OK;

#ifdef MINI_MEMORY_SUPPORT
AUDIO_STOP:
    (void)AudioSDKStopInternal(portTrackCtx);
BT_DETACH:
    (void)BluetoothDetachInternal(portTrackCtx);
AUDIO_DETACH:
    (void)AudioSDKDetachInternal(portTrackCtx);
AUDIO_CLOSE:
    (void)AudioSDKCloseInternal(portTrackCtx);
    MediaHalMutexUnLock(portTrackCtx->subPortCtx->a2dpTrackMutex);
    return ret;
#endif
}

int32_t SendFrameInA2DPSourceNormalScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    if (portTrackCtx->isRaw) {
        uapi_audio_frame audioFrame = {
            .interleaved = true,
            .bit_depth = portTrackCtx->inputAttr.bitWidth,
            .sample_rate = portTrackCtx->inputAttr.sampleRate,
            .channels = portTrackCtx->inputAttr.channelCount,
            .pcm_samples = ((uint32_t)portTrackCtx->inputAttr.sampleRate) / SAMPLERATE_TO_PTNUMPERFRM,
            .pcm_buffer = NULL,
            .bits_buffer = (int32_t *)buffer,
            .bits_bytes = (uint32_t)requestBytes,
            .frame_index = 0,
            .eos = false,
        };
        ret = uapi_adp_send_frame(portTrackCtx->inputAdpHandle, &audioFrame);
        if (ret != MEDIA_HAL_OK) {
            *replyBytes = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
        *replyBytes = requestBytes;
    } else {
        uapi_stream_buf streamBuf = {};
        streamBuf.data = (uint8_t *)buffer;
        streamBuf.size = (uint32_t)requestBytes;
        streamBuf.pts = 0;
        streamBuf.eos = (buffer == NULL && requestBytes == 0) ? TD_TRUE : TD_FALSE;
        ret = uapi_adp_send_stream(portTrackCtx->inputAdpHandle, &streamBuf);
        if (ret != MEDIA_HAL_OK) {
            *replyBytes = 0;
            return MEDIA_HAL_ERR_AGAIN;
        }
        *replyBytes = requestBytes;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif

    return MEDIA_HAL_OK;
}

int32_t GetPositionInA2DPSourceNormalScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(frames);

#ifdef MINI_MEMORY_SUPPORT
    if (portTrackCtx->isReleased) {
        MEDIA_HAL_UNUSED(portTrackCtx);
        MEDIA_HAL_UNUSED(time);
        MEDIA_HAL_LOGI(MODULE_NAME, "have released");
        return MEDIA_HAL_OK;
    }
#endif
    return UpdateCurrentPosition(portTrackCtx->trackHandle, time);
}

int32_t InvokeInA2DPSourceNormalScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr)
{
    SubPortTrackContext *portTrackOutCtx = (SubPortTrackContext *)trackHandle;
    char keyFlag[MAX_KEY_STR_SIZE] = { 0 };
    char *value = (char *)MediaMallocz(MAX_KEY_VALUE_SIZE);
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "value malloc failed!");
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_SET_EXTRA_PARAMS:
            ret = ParseKeyValueForExtra(invokeAttr, keyFlag, MAX_KEY_STR_SIZE, value, MAX_KEY_VALUE_SIZE);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "ParseKeyValueForExtra failed");
            if (strcmp(keyFlag, "extradata") == 0) {
                ret = SetExtraData(&portTrackOutCtx->inputAttr, value, (uint32_t)(strlen(value) + 1));
                MEDIA_HAL_LOGE(MODULE_NAME, "SetExtraData failed: %d", ret);
                goto EXIT;
            }
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            MediaHalFree(value);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "invoke %d success", invokeID);
EXIT:
    MediaHalFree(value);
    return ret;
}

int32_t RegCallbackInA2DPSourceNormalScene(AudioHandle trackHandle,
    TrackCallback callback, const AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    portTrackCtx->callback = callback;
    portTrackCtx->caller = caller;
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
