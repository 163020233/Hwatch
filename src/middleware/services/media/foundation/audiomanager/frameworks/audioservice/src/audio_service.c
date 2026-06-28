/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description:Audio service
* Author: Media Software Group
* Create: 2021-02-28
*/

#include "audio_service.h"
#include <errno.h>
#ifdef __STDC_NO_ATOMICS__
#include <stdatomic.h>
#endif

#include <limits.h>
#include "securec.h"
#include "device_manager.h"
#include "policy_manager.h"
#include "stream_manager.h"

#include "audio_errors.h"
#include "audio_utils.h"
#include "media_thread_adapt.h"
#include "media_chr_log.h"

#define LOG_MODULE_NAME "AudioService"
#define BUFFER_LEN 120

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

const int32_t VOLUME_MIN = 0;
const int32_t VOLUME_MAX = 100;
const int32_t MIXWEIGHT_MIN = 0;
const int32_t MIXWEIGHT_MAX = 100;
const float PERCENT_BASE = 100.0f;
const float VOLUME_PERCENT_MIN = 0.00f;
const float VOLUME_PERCENT_MAX = 1.00f;

typedef struct {
    AudioSession audioSessionId;
    int32_t  availableOutputDevices;
} AudioServiceContext;

static AudioServiceContext g_audioSVCCtx;
static bool g_audioSVCInited = false;
static MediaMutexHandle g_audioSVCMutex = NULL;
static AudioServiceHaidEventCallback g_audioServiceHaidEvent = NULL;

int32_t AudioServiceInit(void)
{
    int32_t ret;
    if (g_audioSVCMutex == NULL) {
        MediaMutexGlobalLock();
        if (g_audioSVCMutex == NULL) {
            g_audioSVCMutex = MediaMutexCreate(NULL);
        }
        MediaMutexGlobalUnLock();
    }

    AUDIO_LOCK(g_audioSVCMutex);
    if (g_audioSVCInited) {
        AUDIO_UNLOCK(g_audioSVCMutex);
        return AUDIO_SUCCESS;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, 0, 0, 0);
    ret = memset_s(&g_audioSVCCtx, sizeof(AudioServiceContext),
                   0x0, sizeof(AudioServiceContext));
    if (ret != EOK) {
        ALOGE("memset_s: g_audioSVCCtx failed");
        goto UNLOCK_RET;
    }
    ret = DeviceMgrInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("DeviceMgrInit failed");
        goto UNLOCK_RET;
    }
    ret = PolicyMgrInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("PolicyMgrInit failed");
        goto POLICY_FAILED;
    }
    ret = StreamMgrInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("StreamMgrInit failed");
        goto STREAM_FAILED;
    }
    g_audioSVCInited = true;
    AUDIO_UNLOCK(g_audioSVCMutex);
    ALOGI("AudioServiceInit SUCCESS");
    return AUDIO_SUCCESS;

STREAM_FAILED:
    PolicyMgrDeInit();
POLICY_FAILED:
    DeviceMgrDeInit();
UNLOCK_RET:
    AUDIO_UNLOCK(g_audioSVCMutex);
    return ret;
}

int32_t AudioServiceDeInit(void)
{
    int32_t ret;
    AUDIO_LOCK(g_audioSVCMutex);
    if (!g_audioSVCInited) {
        AUDIO_UNLOCK(g_audioSVCMutex);
        return AUDIO_SUCCESS;
    }
    ret = DeviceMgrDeInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("DeviceMgrDeInit failed");
    }
    ret = PolicyMgrDeInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("PolicyMgrDeInit failed");
    }
    ret = StreamMgrDeInit();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("StreamMgrDeInit failed");
    }
    g_audioSVCInited = false;
    AUDIO_UNLOCK(g_audioSVCMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, 0, 0);
    ALOGI("SUCCESS");
    return ret;
}

AudioSession AudioServiceAcquireAudioSessionId(void)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_SESSION_ID_NONE, "Audio Service not init");
#ifdef __STDC_NO_ATOMICS__
    uint32_t base = (uint32_t)atomic_fetch_add_explicit(&g_audioSVCCtx.audioSessionId,
                                                        (uint_fast32_t)1, memory_order_acq_rel);
#else
    AUDIO_LOCK(g_audioSVCMutex);
    uint32_t base = ++g_audioSVCCtx.audioSessionId;
    AUDIO_UNLOCK(g_audioSVCMutex);
#endif
    ALOGI("base:%u sessionId:%u", base, g_audioSVCCtx.audioSessionId);
    // not allow overflow for session ids
    if (base == UINT_MAX) {
        AUDIO_LOCK(g_audioSVCMutex);
        g_audioSVCCtx.audioSessionId = AUDIO_SESSION_ID_NONE;
        AUDIO_UNLOCK(g_audioSVCMutex);
        ALOGW("unique AudioSession overflow");
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_POLICY_MANAGER, MEDIA_CHR_MAKE_SESSION_ID, 0);
    return g_audioSVCCtx.audioSessionId;
}

bool AudioServiceSetVolume(AudioStreamType streamType, int32_t volume)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    if (volume < VOLUME_MIN || volume > VOLUME_MAX) {
        return false;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_SET_VOLUME, 0);
    float typeVolume = volume / PERCENT_BASE;
    int32_t ret = StreamMgrSetTypeVolume(streamType, typeVolume);
    if (ret != AUDIO_SUCCESS) {
        return false;
    }
    return true;
}

int32_t AudioServiceSetStreamVolume(AudioStreamType streamType, float typeVolume)
{
    return StreamMgrSetTypeVolume(streamType, typeVolume);
}

int32_t AudioServiceGetVolume(AudioStreamType streamType)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    const int32_t volumeInvalid = -1;
    float typeVolume = 0;
    if (!StreamMgrGetTypeVolume(streamType, &typeVolume)) {
        return volumeInvalid;
    }
    int32_t volume;
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_GET_VOLUME, 0);
    if (typeVolume <= VOLUME_PERCENT_MIN) {
        volume = VOLUME_MIN;
    } else if (typeVolume >= VOLUME_PERCENT_MAX) {
        volume = VOLUME_MAX;
    } else {
        volume = (int32_t)(typeVolume * PERCENT_BASE);
    }
    return volume;
}

int32_t AudioServiceGetMinVolume(AudioStreamType streamType)
{
    AUDIO_UNUSED(streamType);
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    return VOLUME_MIN;
}

int32_t AudioServiceGetMaxVolume(AudioStreamType streamType)
{
    AUDIO_UNUSED(streamType);
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    return VOLUME_MAX;
}

bool AudioServiceMute(AudioStreamType streamType)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_SET_MUTE, 0);
    int32_t ret = StreamMgrSetMute(streamType, true);
    if (ret != AUDIO_SUCCESS) {
        return false;
    }
    return true;
}

bool AudioServiceUnMute(AudioStreamType streamType)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_SET_UNMUTE, 0);
    int32_t ret = StreamMgrSetMute(streamType, false);
    if (ret != AUDIO_SUCCESS) {
        return false;
    }
    return true;
}

bool AudioServiceIsMute(AudioStreamType streamType)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_GET_MUTE, 0);
    return StreamMgrIsMute(streamType);
}

bool AudioServiceSetMasterMute(bool isMute)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_SET_MASTER_MUTE, 0);
    return StreamMgrSetMasterMute(isMute);
}

bool AudioServiceIsMasterMute(void)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_GET_MASTER_MUTE, 0);
    return StreamMgrIsMasterMute();
}

bool AudioServiceSetMicrophoneMute(bool isMute)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_SET_MIC_MUTE, 0);
    return StreamMgrSetMicrophoneMute(isMute);
}

bool AudioServiceIsMicrophoneMute(void)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_VOLUME_MANAGER, MEDIA_CHR_AM_GET_MIC_MUTE, 0);
    return StreamMgrIsMicrophoneMute();
}

bool AudioServiceSetRingerMode(AudioRingMode mode)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, MEDIA_CHR_AM_SET_RING_MODE, 0);
    return StreamMgrSetRingerMode(mode);
}

int32_t AudioServiceGetRingerMode(void)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, MEDIA_CHR_AM_GET_RING_MODE, 0);
    return StreamMgrGetRingerMode();
}

int32_t AudioServiceGetDeviceCount(DeviceFlag flag, uint32_t *deviceCount)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    return DeviceMgrGetAvailableDeviceCount(flag, deviceCount);
}

int32_t AudioServiceGetDevices(DeviceFlag flag, uint32_t deviceCount, AudioDeviceInfo *devices)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    return DeviceMgrGetAvailableDevices(flag, deviceCount, devices);
}

int32_t AudioServiceSetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    return DeviceMgrSetDeviceConnectionState(device, state);
}

bool AudioServiceIsOffloadSupported(const AudioOffloadInfo *offloadInfo)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, false, "Audio Service not init");
    CHK_NULL_RETURN(offloadInfo, false, "offloadInfo is NULL");
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, MEDIA_CHR_AM_OFF_LOAD, 0);
    uint32_t supportDeviceCount = 0;
    AudioDeviceType *supportDevices = NULL;
    ALOGI("streamType:0x%x", offloadInfo->streamType);
    if (!PolicyMgrGetSupportDevicesForStream(offloadInfo->streamType, &supportDeviceCount, &supportDevices) ||
        supportDeviceCount == 0 ||
        supportDevices == NULL) {
        ALOGE("PolicyMgrGetSupportDevicesForStream failed");
        return false;
    }
    ALOGI("supportDeviceCount:%u", supportDeviceCount);
    for (uint32_t index = 0; index < supportDeviceCount; ++index) {
        if (!DeviceMgrIsSupportOffload(supportDevices[index], offloadInfo)) {
            MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT,
                MEDIA_CHR_AM_OFF_LOAD, MEDIA_CHR_AM_NOT_SUPPORT_OFF_LOAD);
            return false;
        }
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT,
        MEDIA_CHR_AM_OFF_LOAD, MEDIA_CHR_AM_SUPPORT_OFF_LOAD);
    return true;
}

int32_t AudioServiceActivateAudioInterrupt(const AudioInterruptProxy *interrupt)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    CHK_NULL_RETURN(interrupt, AUDIO_INVALID_PARAMS, "interrupt is NULL");
    return PolicyMgrActivateAudioInterrupt(interrupt);
}

int32_t AudioServiceDeactivateAudioInterrupt(const AudioInterruptProxy *interrupt)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    CHK_NULL_RETURN(interrupt, AUDIO_INVALID_PARAMS, "interrupt is NULL");
    int32_t ret = PolicyMgrDeactivateAudioInterrupt(interrupt);
    CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "policy manager deactive fail");
    ALOGI("AudioServiceDeactivateAudioInterrupt StreamManagerDeactivateCorrespondingStream");
    ret =  StreamManagerDeactivateCorrespondingStream(interrupt->sessionID, interrupt->streamType);
    if (ret != AUDIO_SUCCESS) {
        ALOGI("StreamManager Deactivate CorrespondingStream fail, maybe sm no record");
    }
    return AUDIO_SUCCESS;
}

int32_t AudioServiceDumpInfo(AudioServiceDebugInfo *audioServiceDebugInfo)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    CHK_NULL_RETURN(audioServiceDebugInfo, AUDIO_INVALID_PARAMS, "audioServiceDebugInfo is NULL");
    int32_t ret = PolicyMgrDumpInterruptInfo(&audioServiceDebugInfo->audioInterruptInfo);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("PolicyMgrDumpInterruptInfo failed:%u", ret);
        return ret;
    }
    ret = StreamMgrDumpInfo(&audioServiceDebugInfo->audioStreamInfo);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("StreamMgrDumpInfo failed:%u", ret);
        return ret;
    }
    ret = DeviceMgrDumpInfo(&audioServiceDebugInfo->audioDeviceInfo);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("DeviceMgrDumpInfo failed:%u", ret);
        return ret;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_DEINIT, MEDIA_CHR_AM_GET_DUMP_INFO, 0);
    return AUDIO_SUCCESS;
}

int32_t AudioServiceInvoke(AudioInvokeId audioInvokeId, char *param, uint32_t *paramlength)
{
    return DeviceMgrInvoke(audioInvokeId, param, paramlength);
}

static int32_t AudioDeviceHaidEventCallbackFunc(int32_t eventType, void *param, void *context)
{
    CHK_NULL_RETURN(g_audioServiceHaidEvent, AUDIO_INVALID_PARAMS, "audio service call back func is NULL");
    return g_audioServiceHaidEvent(eventType, param, context);
}

int32_t AudioServiceRegCallBack(AudioServiceHaidEventCallback callBack, void *context)
{
    CHK_NULL_RETURN(callBack, AUDIO_INVALID_PARAMS, "audio service call back func is NULL");
    AUDIO_LOCK(g_audioSVCMutex);
    g_audioServiceHaidEvent = callBack;
    int32_t ret = DeviceMgrRegCallBack(AudioDeviceHaidEventCallbackFunc, context);
    AUDIO_UNLOCK(g_audioSVCMutex);
    return ret;
}

int32_t AudioServiceSetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    CHK_NULL_RETURN(param, AUDIO_INVALID_PARAMS, "param is NULL");
    int32_t ret = StreamManagerSetParam(direction, param, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("Audio Service set param failed!");
    }
    return ret;
}

int32_t AudioServiceGetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    CHK_NULL_RETURN(param, AUDIO_INVALID_PARAMS, "param is NULL");
    int32_t ret = StreamManagerGetParam(direction, param, len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("Audio Service get param failed!");
    }
    return ret;
}

int32_t AudioServiceSetMixWeight(AudioStreamType streamType, int32_t mixWeight)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    if (mixWeight < MIXWEIGHT_MIN || mixWeight > MIXWEIGHT_MAX) {
        ALOGE("mixWeight[%d] is invaild!", mixWeight);
        return AUDIO_ERROR;
    }
    float streamMixWeight = mixWeight / PERCENT_BASE;
    int32_t ret = StreamMgrSetMixWeight(streamType, streamMixWeight);
    if (ret != AUDIO_SUCCESS) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

uint32_t AudioServiceGetMixWeight(AudioStreamType streamType)
{
    CHK_FAILED_RETURN(g_audioSVCInited, true, AUDIO_NO_INIT, "Audio Service not init");
    uint32_t retMixWeight = 0;
    float mixWeight = 0;
    if (!StreamMgrGetMixWeight(streamType, &mixWeight)) {
        return retMixWeight;
    }
    if (mixWeight <= VOLUME_PERCENT_MIN) {
        retMixWeight = VOLUME_MIN;
    } else if (mixWeight >= VOLUME_PERCENT_MAX) {
        retMixWeight = VOLUME_MAX;
    } else {
        retMixWeight = (uint32_t)(mixWeight * PERCENT_BASE);
    }
    return retMixWeight;
}

int32_t AudioServiceRecordDbVolume(AudioStreamType streamType, int32_t dbVol)
{
    char buf[BUFFER_LEN] = { 0 };
    int32_t ret = sprintf_s(buf, BUFFER_LEN, "setdbvol = %d", dbVol);
    if (ret == -1) {
        ALOGE("record db volume failed, ret = %d", ret);
        return AUDIO_ERROR;
    }
    return StreamMgrRecordDbVolume(streamType, buf);
}

int32_t AudioServiceStreamTypeFromFrameworkToHal(int32_t stream)
{
    return DeviceMgrStreamTypeFromFrameworkToHal(stream);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
