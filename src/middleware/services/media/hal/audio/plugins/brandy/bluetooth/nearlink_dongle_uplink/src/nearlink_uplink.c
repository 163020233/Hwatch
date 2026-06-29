/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: nearlink uplink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#include "nearlink_uplink.h"
#include "common.h"
#include "bluetooth_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MODULE_NAME "AudioNearlinkUplinkPort"

static int32_t NearlinkUplinkAttach(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAttach(AUDIO_MOD_ADP, portTrackCtx->inputUplinkAdpHandle, portTrackCtx->adecUplinkHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp atach adec failed");
        goto OUTADP_CLOSE;
    }
    ret = AudioSDKAttach(AUDIO_MOD_ADEC, portTrackCtx->adecUplinkHandle, portTrackCtx->aencHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc attach track failed");
        goto ADP_DETACH_ADEC;
    }
    ret = AudioSDKAttach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputUplinkAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "outputUplinkAdpHandle attach track failed");
        goto ADEC_DETACH_AENC;
    }
    return MEDIA_HAL_OK;

ADEC_DETACH_AENC:
    (void)AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecUplinkHandle, portTrackCtx->aencHandle);
ADP_DETACH_ADEC:
    (void)AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputUplinkAdpHandle, portTrackCtx->adecUplinkHandle);
OUTADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->outputUplinkAdpHandle);
    return ret;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    return MEDIA_HAL_OK;
#endif
}

static int32_t NearlinkUplinkCreate(SubPortTrackContext *portTrackCtx)
{
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKAdpOpen(&portTrackCtx->inputUplinkAdpHandle);
    CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "uplink adp open failed");
    portTrackCtx->inputAttr.isDongleUpLink = true;
    ret = AudioSDKAdecOpen(&portTrackCtx->adecUplinkHandle, portTrackCtx->inputAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adec open failed");
        goto INADP_CLOSE;
    }
    uapi_aenc_attr aencAttr = {};
    uapi_acodec_id aencCodecId;
    if (!AudioConvertAudioFormatToCodecId(portTrackCtx->inputAttr.audioFormat, &aencCodecId)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "AudioConvertAudioFormatToCodecId failed");
        ret = MEDIA_HAL_INVALID_PARAM;
        goto INADP_CLOSE;
    }
    aencAttr = GetAencAttrInfo(aencCodecId, portTrackCtx->inputAttr);
    ret = AudioSDKAencOpen(&portTrackCtx->aencHandle, aencAttr, AENC_TYPE_DONGLE_UPLINK, &(portTrackCtx->inputAttr));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "aenc open failed");
        goto ADEC_CLOSE;
    }
    ret = AudioSDKAdpOpen(&portTrackCtx->outputUplinkAdpHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "adp atach adec failed");
        goto AENC_CLOSE;
    }
    ret = NearlinkUplinkAttach(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "NearlinkUplinkAttach failed");
        return ret;
    }
    return MEDIA_HAL_OK;

AENC_CLOSE:
    (void)AudioSDKAencClose(portTrackCtx->aencHandle);
ADEC_CLOSE:
    (void)AudioSDKAdecClose(portTrackCtx->adecUplinkHandle);
INADP_CLOSE:
    (void)AudioSDKAdpClose(portTrackCtx->inputUplinkAdpHandle);
    return ret;
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
    return MEDIA_HAL_OK;
#endif
}

int32_t CreateTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    int32_t ret = NearlinkUplinkCreate(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "NearlinkUplinkCreate failed");
        return ret;
    }
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t DestroyTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = AudioSDKDetach(AUDIO_MOD_ADP, portTrackCtx->inputUplinkAdpHandle, portTrackCtx->adecUplinkHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec detach track failed");

    ret = AudioSDKDetach(AUDIO_MOD_ADEC, portTrackCtx->adecUplinkHandle, portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc detach track failed");

    ret = AudioSDKDetach(AUDIO_MOD_AENC, portTrackCtx->aencHandle, portTrackCtx->outputUplinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adp detach aenc failed");
    
    ret = AudioSDKAencClose(portTrackCtx->aencHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc close failed");

    ret = AudioSDKAdecClose(portTrackCtx->adecUplinkHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec close failed");

    ret = AudioSDKAdpClose(portTrackCtx->inputUplinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "input adp close failed");

    ret = AudioSDKAdpClose(portTrackCtx->outputUplinkAdpHandle);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "output adp close failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t EnableTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret = 0;
    (void)AudioSDKSetAIScene(portTrackCtx->inputUplinkAdpHandle, &portTrackCtx->inputAttr, true);
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_start(portTrackCtx->adecUplinkHandle));
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGI(MODULE_NAME, "adec start failed");
        return MEDIA_HAL_ERR;
    }
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_start(portTrackCtx->aencHandle));
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

int32_t DisableTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
#ifdef AUDIO_SDK_SUPPORT
    int32_t ret;
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_adec_stop(portTrackCtx->adecUplinkHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "adec stop failed");
    MEDIA_HAL_DOFUNC_TIME(ret = uapi_aenc_stop(portTrackCtx->aencHandle));
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "aenc stop failed");
#else
    MEDIA_HAL_UNUSED(portTrackCtx);
#endif
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t TrackSetMuteInNearlinkUplinkScene(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetMuteInNearlinkUplinkScene(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackSetVolumeInNearlinkUplinkScene(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t TrackGetVolumeInNearlinkUplinkScene(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t PauseTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t ResumeTrackInNearlinkUplinkScene(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t SendFrameInNearlinkUplinkScene(AudioHandle trackHandle, const uint8_t *buffer,
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

int32_t GetPositionInNearlinkUplinkScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(frames);
    MEDIA_HAL_UNUSED(time);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static struct AudioPortInvoke g_nearlinkUpLinkInvokeFuncList[] = {};

int32_t InvokeInNearlinkUplinkScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    int32_t ret;

    switch (invokeID) {
        case INVOKE_ID_GET_CHANNEL_ID:
        {
            *((uint32_t *)invokeAttr->request) = portTrackCtx->outputUplinkAdpHandle;
            invokeAttr->replyBytes = (uint64_t)sizeof(uint32_t);
            MEDIA_HAL_LOGI(MODULE_NAME, "data channel id: 0x%x", *((uint32_t *)invokeAttr->request));
            break;
        }
        case INVOKE_ID_SET_EXTRA_PARAMS:
        case INVOKE_ID_GET_EXTRA_PARAMS:
        {
            size_t size = sizeof(g_nearlinkUpLinkInvokeFuncList) / sizeof(struct AudioPortInvoke);
            ret = ExtraParamsInvoke(portTrackCtx, invokeAttr, g_nearlinkUpLinkInvokeFuncList, (uint32_t)size);
            CHK_FAILED_RETURN(ret, MEDIA_HAL_OK, ret, "extra param invoke failed!");
            break;
        }
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid invoke id: %d", invokeID);
            return MEDIA_HAL_INVALID_PARAM;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t RegCallbackInNearlinkUplinkScene(AudioHandle trackHandle, TrackCallback callback,
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