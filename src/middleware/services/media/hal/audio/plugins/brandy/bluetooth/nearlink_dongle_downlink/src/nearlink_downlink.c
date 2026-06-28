/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: nearlink downlink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "nearlink_downlink.h"
#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioNearlinkDownlinkPort"

#define SET_ENCODE_PARAM "setencodeparam"
#define SET_BIT_RATE  "setbitrate"
#ifdef MELODY_DEMO_SOUNDBAR
// soundbar 是1T3R，3路AENC
#define AUDIO_MODULE_NUM   3
#else
#define AUDIO_MODULE_NUM   2
#endif
#define SSCANF_NUM3   3
#define SSCANF_NUM1   1
#define DEFAULT_BITRATE_PARAM_SIZE   100

typedef struct {
    uapi_audio_ch_config aencChannel[ANEC_HANDLE_MAX_NUM];
} AencChannelConfig;

static void AudioSDKAencMultipleclose(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
        (void)AudioSDKAencClose(portTrackCtx->aencHandles[i]);
    }
}

static int32_t AudioSDKAencMultipleOpen(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    uapi_aenc_attr aencAttr = {};
    uapi_acodec_id aencCodecId;
    if (!AudioConvertAudioFormatToCodecId(portTrackCtx->inputAttr.audioFormat, &aencCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        return MEDIA_HAL_INVALID_PARAM;
    }
    aencAttr = GetAencAttrInfo(aencCodecId, portTrackCtx->inputAttr);
    int32_t ret;
    for (uint32_t i = 0; i < arrayNum; i++) {
        ret = AudioSDKAencOpen(&portTrackCtx->aencHandles[i], aencAttr, AENC_TYPE_BT, &(portTrackCtx->inputAttr));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAencOpen failed! ret = %d, i = %u", ret, i);
            if (i >= 1) {
                AudioSDKAencMultipleclose(i - 1, portTrackCtx);
            }
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static void AudioSDKoutputAdpMultipleclose(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > OUTPUTADP_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
        (void)AudioSDKAdpClose(portTrackCtx->outputAdpHandles[i]);
    }
}

static int32_t AudioSDKoutputAdpMultipleOpen(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > OUTPUTADP_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    int32_t ret;
    for (uint32_t i = 0; i < arrayNum; i++) {
        ret = AudioSDKAdpOpen(&portTrackCtx->outputAdpHandles[i]);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAdpOpen failed! ret = %d, i = %u", ret, i);
            if (i >= 1) {
                AudioSDKoutputAdpMultipleclose(i - 1, portTrackCtx);
            }
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKAencMultipleStop(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    int32_t ret;
    for (uint32_t i = 0; i < arrayNum; i++) {
        ret = uapi_aenc_stop(portTrackCtx->aencHandles[i]);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_stop failed! ret = %d, i = %u", ret, i);
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKAencMultipleStart(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    int32_t ret;
    for (uint32_t i = 0; i < arrayNum; i++) {
        ret = uapi_aenc_start(portTrackCtx->aencHandles[i]);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_start failed! ret = %d, i = %u", ret, i);
            if (i >= 1) {
                AudioSDKAencMultipleStop(i - 1, portTrackCtx);
            }
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static void AudioSDKAencMultipleDetach(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
#ifdef MELODY_DEMO_SOUNDBAR
        (void)AudioSDKDetach(AUDIO_MOD_AI, portTrackCtx->aiHandle, portTrackCtx->aencHandles[i]);
#else
        (void)AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->aencHandles[i]);
#endif
    }
}

static void AudioSDKoutputAdpMultipleDetach(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > OUTPUTADP_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
        (void)AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandles[i],
            portTrackCtx->outputAdpHandles[i]);
    }
}

static int32_t AudioSDKAencMultipleAttach(uint32_t arrayNum, SubPortTrackContext *portTrackCtx)
{
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
    }
    int32_t ret;
    for (uint32_t i = 0; i < arrayNum; i++) {
#ifdef MELODY_DEMO_SOUNDBAR
        ret = AudioSDKAttach(AUDIO_MOD_AI, portTrackCtx->aiHandle, portTrackCtx->aencHandles[i]);
#else
        ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecHandle, portTrackCtx->aencHandles[i]);
#endif
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttach aenc failed! ret = %d, i = %u", ret, i);
            if (i >= 1) {
                AudioSDKAencMultipleDetach(i - 1, portTrackCtx);
            }
            return ret;
        }
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
        AudioSDKAttach(AUDIO_MOD_AENC, portTrackCtx->aencHandles[i], portTrackCtx->outputAdpHandles[i]);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "AudioSDKAttach outputAdpHandle failed! ret = %d, i = %u", ret, i);
            if (i >= 1) {
                AudioSDKoutputAdpMultipleDetach(i - 1, portTrackCtx);
                AudioSDKAencMultipleDetach(arrayNum, portTrackCtx);
            }
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t NearlinkDownlinkAencMultipleStart(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
#ifdef MELODY_DEMO_SOUNDBAR
    // soundbar 采用AI输入数据
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_start(portTrackCtx->aiHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "ai start failed");
        return MEDIA_HAL_ERR;
    }
#else
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "adec start failed");
        return MEDIA_HAL_ERR;
    }
#endif
    MEDIA_HAL_DOFUNC_TIME(ret = AudioSDKAencMultipleStart(AUDIO_MODULE_NUM, portTrackCtx));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc start failed");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
#endif
}

static int32_t NearlinkDownlinkCreate(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
#ifdef MELODY_DEMO_SOUNDBAR
    AIOpenAttr aiOpenAttr = {portTrackCtx->inputAttr.type, portTrackCtx->inputAttr.micType,
        false, false, portTrackCtx->inputAttr};
    int32_t ret = AudioSDKAIOpen(&portTrackCtx->aiHandle, &aiOpenAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "ai open failed");
        return MEDIA_HAL_ERR;
    }
#else
    int32_t ret = AudioSDKAdpOpen(&portTrackCtx->inputAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "downlink adp open failed");
    portTrackCtx->inputAttr.isDongleUpLink = false;
    ret = AudioSDKAdecOpen(&portTrackCtx->adecHandle, portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto INADP_CLOSE;
    }
#endif
    ret = AudioSDKAencMultipleOpen(AUDIO_MODULE_NUM, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc array open failed");
        goto ADEC_CLOSE;
    }
    ret = AudioSDKoutputAdpMultipleOpen(AUDIO_MODULE_NUM, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "outputAdp array open failed");
        goto AENCARRAY_CLOSE;
    }
#ifdef MELODY_DEMO_DONGLE
    // 只有dongle 从UAC 拿的数据接上ADEC，soundbar AI 直接接AENC
    ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
#endif
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp attach adec failed");
        goto OUTPUTADPARRAY_CLOSE;
    }
    ret = AudioSDKAencMultipleAttach(AUDIO_MODULE_NUM, portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc array attach failed");
        goto AENC_DETACH_ADEC;
    }
    return MEDIA_HAL_OK;

AENC_DETACH_ADEC:
#ifdef MELODY_DEMO_DONGLE
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
#endif
OUTPUTADPARRAY_CLOSE:
    AudioSDKoutputAdpMultipleclose(AUDIO_MODULE_NUM, portTrackCtx);
AENCARRAY_CLOSE:
    AudioSDKAencMultipleclose(AUDIO_MODULE_NUM, portTrackCtx);
ADEC_CLOSE:
#ifdef MELODY_DEMO_SOUNDBAR
    (void)AudioSDKAIClose(portTrackCtx->aiHandle);
#else
    (void)AudioSDKAdecClose(portTrackCtx->adecHandle);
INADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
#endif
    return ret;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    return MEDIA_HAL_OK;
}

int32_t CreateTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    int32_t ret = NearlinkDownlinkCreate(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc array attach failed");
        return ret;
    }
    return MEDIA_HAL_OK;
}

int32_t DestroyTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    AudioSDKoutputAdpMultipleDetach(AUDIO_MODULE_NUM, portTrackCtx);
    AudioSDKAencMultipleDetach(AUDIO_MODULE_NUM, portTrackCtx);
#ifdef MELODY_DEMO_DONGLE
    ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputAdpHandle, portTrackCtx->adecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach aenc failed");
#endif
    AudioSDKoutputAdpMultipleclose(AUDIO_MODULE_NUM, portTrackCtx);
    AudioSDKAencMultipleclose(AUDIO_MODULE_NUM, portTrackCtx);
#ifdef MELODY_DEMO_DONGLE
    ret = AudioSDKAdecClose(portTrackCtx->adecHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");
#endif

#ifdef MELODY_DEMO_SOUNDBAR
    ret = AudioSDKAIClose(portTrackCtx->aiHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aiHandle close failed");
#else
    ret = AudioSDKAdpClose(portTrackCtx->inputAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp close failed");
#endif

#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t EnableTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef MELODY_DEMO_SOUNDBAR
    (void)AudioSDKSetAIScene(portTrackCtx->aiHandle, &portTrackCtx->inputAttr, false);
#else
    (void)AudioSDKSetAIScene(portTrackCtx->adecHandle, &portTrackCtx->inputAttr, false);
#endif
    return MEDIA_HAL_OK;
}

int32_t DisableTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    if (portTrackCtx->aencStarted) {
        int32_t ret = 0;
        MEDIA_HAL_DOFUNC_TIME(ret = AudioSDKAencMultipleStop(AUDIO_MODULE_NUM, portTrackCtx));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "AudioSDKAencMultipleStop failed");
#ifdef MELODY_DEMO_DONGLE
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");
#endif
#ifdef MELODY_DEMO_SOUNDBAR
        MEDIA_HAL_DOFUNC_TIME(ret = uapi_ai_stop(portTrackCtx->aiHandle));
        CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "ai stop failed");
#endif
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t TrackSetMuteInNearlinkDownlinkScene(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetMuteInNearlinkDownlinkScene(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetVolumeInNearlinkDownlinkScene(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetVolumeInNearlinkDownlinkScene(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);
    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t PauseTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t ResumeTrackInNearlinkDownlinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t SendFrameInNearlinkDownlinkScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t GetPositionInNearlinkDownlinkScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t NearlinkSetAencBitRateParam(SubPortTrackContext *portTrackCtx, const char *value, uint32_t valueLen,
    uint32_t paramLen, uint32_t num)
{
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    uint8_t *param = (uint8_t *)MediaHalCalloc(DEFAULT_BITRATE_PARAM_SIZE);
    CHK_NULL_RETURN(param, MEDIA_HAL_INVALID_PARAM, "param malloc failed!");
    int32_t ret = GetParamByValue(param, DEFAULT_BITRATE_PARAM_SIZE, value, valueLen, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MediaHalFree(param);
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed, ret = %d", ret);
        return ret;
    }
    for (uint32_t i = 0; i < num && i < ANEC_HANDLE_MAX_NUM; i++) {
        ret = uapi_aenc_set_param(portTrackCtx->aencHandles[i], UAPI_AENC_PARAM_BGC_RATE_CFG, (td_void *)param, paramLen);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_set_param rate config failed!");
            MediaHalFree(param);
            return ret;
        }
    }
    MediaHalFree(param);
    return MEDIA_HAL_OK;
}

static int32_t NearlinkSetAencChannelMode(SubPortTrackContext *portTrackCtx, const char *value, uint32_t valueLen,
    uint32_t paramLen, uint32_t num)
{
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    AencChannelConfig config = {};
    if (paramLen > sizeof(AencChannelConfig)) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = GetParamByValue((uint8_t *)&config, paramLen, value, valueLen, paramLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetParamByValue failed!");
    }
    for (uint32_t i = 0; i < num && i < ANEC_HANDLE_MAX_NUM; i++) {
        if (config.aencChannel[i].ch_mode == UAPI_AUDIO_CHANNEL_MODE_MAX) {
            continue;
        }
        ret = uapi_aenc_set_param(portTrackCtx->aencHandles[i], UAPI_AENC_PARAM_CHANNEL_MODE,
            &config.aencChannel[i], sizeof(uapi_audio_ch_config));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi_aenc_set_param failed!");
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

int32_t SetAencParam(SubPortTrackContext *portTrackCtx, const char *value, uint32_t valueLen)
{
    uint32_t num = 0;
    uint32_t aencParamTypeLen = 0;
    uapi_aenc_param_type paramType;
    int32_t ret = sscanf_s(value, "setencodeparam = %u#%u#%u:", &num, &paramType, &aencParamTypeLen);
    if (ret != SSCANF_NUM3) {
        MEDIA_HAL_LOGE(MODULE_NAME, "setencodeparam sscanf_s failed, ret = %d", ret);
        return ret;
    }
    if (paramType == UAPI_AENC_PARAM_CHANNEL_MODE) {
        if (aencParamTypeLen > sizeof(AencChannelConfig)) {
            return MEDIA_HAL_INVALID_PARAM;
        }
    }
    if (paramType != UAPI_AENC_PARAM_CHANNEL_MODE && paramType != UAPI_AENC_PARAM_BGC_RATE_CFG) {
        MEDIA_HAL_LOGE(MODULE_NAME, "paramType is invalid, paramType = %d", paramType);
        return MEDIA_HAL_ERR;
    }
    if (paramType == UAPI_AENC_PARAM_CHANNEL_MODE) {
        ret = NearlinkSetAencChannelMode(portTrackCtx, value, valueLen, aencParamTypeLen, num);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "NearlinkSetAencChannelMode failed, ret = %d", ret);
            return ret;
        }
    } else {
        ret = NearlinkSetAencBitRateParam(portTrackCtx, value, valueLen, aencParamTypeLen, num);
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "NearlinkSetAencBitRateParam failed, ret = %d", ret);
            return ret;
        }
    }
    return MEDIA_HAL_OK;
}

static int32_t NearlinkSetAencParam(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t ret = SetAencParam(portTrackCtx, value, valueLen);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set aenc param failed!");
        return MEDIA_HAL_ERR;
    }
    ret = NearlinkDownlinkAencMultipleStart(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "NearlinkDownlinkAencMultipleStart failed!");
        return MEDIA_HAL_ERR;
    }
    portTrackCtx->aencStarted = true;
    return MEDIA_HAL_OK;
}

static int32_t AudioSDKAencSetBiteRate(SubPortTrackContext *portTrackCtx, char *value, uint32_t valueLen, uint32_t arrayNum)
{
    MEDIA_HAL_UNUSED(valueLen);
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    if (arrayNum > ANEC_HANDLE_MAX_NUM) {
        MEDIA_HAL_LOGE(MODULE_NAME, "index overflow!");
        return MEDIA_HAL_ERR;
    }
    uint32_t bitRate = 0;
    int32_t ret = sscanf_s(value, "%u", &bitRate);
    if (ret != SSCANF_NUM1) {
        MEDIA_HAL_LOGE(MODULE_NAME, "set bit rate sscanf_s failed!");
        return MEDIA_HAL_ERR;
    }
    for (uint32_t i = 0; i < arrayNum; i++) {
        ret = uapi_aenc_set_param(portTrackCtx->aencHandles[i], UAPI_AENC_PARAM_BIT_RATE, &bitRate, sizeof(uint32_t));
        if (ret != MEDIA_HAL_OK) {
            MEDIA_HAL_LOGE(MODULE_NAME, "uapi set aenc send cmd bit Rate faild! ret = %d, i = %u", ret, i);
            return ret;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "uapi set aenc bit Rate success, bitrate: %u", bitRate);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_SET_PARAM_STREAM, MEDIA_CHR_SET_BIT_RATE);
    return MEDIA_HAL_OK;
}

static int32_t SetBitRate(AudioHandle trackHandle, char *value, uint32_t valueLen)
{
    MEDIA_HAL_UNUSED(valueLen);
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    CHK_NULL_RETURN(value, MEDIA_HAL_INVALID_PARAM, "input param value is null");
    int32_t ret = AudioSDKAencSetBiteRate(portTrackCtx, value, valueLen, AUDIO_MODULE_NUM);
    if (ret != MEDIA_HAL_OK) {
        return MEDIA_HAL_ERR;
    }
    return MEDIA_HAL_OK;
}

static struct AudioPortInvoke g_nearlinkDownLinkInvokeFuncList[] = {
    {SET_ENCODE_PARAM, NearlinkSetAencParam, false},
    {SET_BIT_RATE, SetBitRate, true},
};

int32_t InvokeInNearlinkDownlinkScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
        {
            *((uint32_t *)invokeAttr->request) = portTrackCtx->inputAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "data channel id: 0x%x", *((uint32_t *)invokeAttr->request));
            break;
        }
        case INVOKE_ID_SET_EXTRA_PARAMS:
        case INVOKE_ID_GET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_nearlinkDownLinkInvokeFuncList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(portTrackCtx, invokeAttr, g_nearlinkDownLinkInvokeFuncList, (uint32_t)size);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "extra param invoke failed!");
            break;
        }
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(invokeID);
    MEDIA_HAL_UNUSED(invokeAttr);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "invoke %d success", invokeID);
    return MEDIA_HAL_OK;
}


int32_t RegCallbackInNearlinkDownlinkScene(AudioHandle trackHandle, TrackCallback callback,
    const AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */