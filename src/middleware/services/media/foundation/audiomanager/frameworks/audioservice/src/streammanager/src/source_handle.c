/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description: source handle interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "source_handle.h"
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "audio_capture.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "device_manager.h"
#include "policy_manager.h"
#include "securec.h"
#include "media_mem.h"
#include "media_chr_log.h"

#define LOG_MODULE_NAME "StreamMgr"
#define ERR_INVALID_READ (-1)
#define PART_STR_NUM 15
#define TOTAL_STR_NUM 50

typedef struct {
    int32_t streamId;
    AudioSession sessionId;
    int32_t streamType;
    int32_t format;
    float volume;
    uint32_t sampleRate;
    uint32_t bitWidth;
    uint32_t channelCnt;
    uint32_t deviceId;
    uint32_t connectedId;
    uint32_t defalutAudioInputDevice;
    uint32_t userAudioInputDevice;
    CapturerCallback callback;
    const void *caller;
    struct AudioCapture *capture;
    StreamState state;
    char *keyValueList;
    bool micMute;
    bool disableSEA;
    uint32_t micType;
    bool isAlwaysWake;
    MediaMutexHandle sourceMutex;
    uint32_t frameLength;
} SourceHandleContext;

uintptr_t CreateSourceHandle(int32_t streamId, const AudioStreamInfo *inf,
    uint32_t defaultRouteDevice, VolumeInf volInf)
{
    // 创建的时候，需要全局锁吗，初步看起来不用，涉及到全局共享变量的才需要
    // malloc context，然后赋值
    SourceHandleContext *sourceHandleCtx = (SourceHandleContext*)MediaMemMalloc(sizeof(SourceHandleContext));
    CHK_NULL_RETURN(sourceHandleCtx, (uintptr_t)NULL, "malloc SourceHandleContext fail");
    memset_s(sourceHandleCtx, sizeof(SourceHandleContext), 0x00, sizeof(SourceHandleContext));
    sourceHandleCtx->streamId = streamId;
    sourceHandleCtx->sessionId = inf->sessionID;
    sourceHandleCtx->streamType = inf->streamType;
    sourceHandleCtx->format = inf->format;
    sourceHandleCtx->defalutAudioInputDevice = defaultRouteDevice; // fw device
    sourceHandleCtx->sampleRate = inf->sampleRate;
    sourceHandleCtx->channelCnt = (uint32_t)inf->channelCount;
    sourceHandleCtx->bitWidth = inf->bitWidth;
    sourceHandleCtx->state = IDLE;
    sourceHandleCtx->micMute = volInf.streamMute;
    sourceHandleCtx->keyValueList = NULL;
    sourceHandleCtx->capture = NULL;
    sourceHandleCtx->disableSEA = inf->disableSEA;
    sourceHandleCtx->micType = inf->micType;
    sourceHandleCtx->volume = volInf.typeVolume;
    sourceHandleCtx->isAlwaysWake = inf->isAlwaysWake;
    sourceHandleCtx->frameLength = inf->frameLength;
    sourceHandleCtx->sourceMutex = MediaMutexCreate(NULL);
    if (sourceHandleCtx->sourceMutex == NULL) {
        ALOGE("create source mutex failed");
        MediaMemFree(sourceHandleCtx);
        return (uintptr_t)NULL;
    }
    return (uintptr_t)sourceHandleCtx;
}

int32_t DestroySourceHandle(uintptr_t handle, bool isDestoryHandle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    int32_t ret = AUDIO_SUCCESS;
    MediaMutexLock(context->sourceMutex);
    if (context->state == IDLE || context->state == RELEASE) {
        goto EXIT;
    }
    uint32_t validFwDevice = context->userAudioInputDevice == 0 ?
        context->defalutAudioInputDevice :
        context->userAudioInputDevice;
    ret = DeviceMgrDestroyStreamIn(validFwDevice, context->capture);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("source[%d] destroy failed 0x%x", context->streamId, ret);
    }
    context->state = RELEASE;
EXIT:
    if (context->keyValueList != NULL) {
        MediaMemFree(context->keyValueList);
        context->keyValueList = NULL;
    }
    if (isDestoryHandle) {
        MediaMutexUnLock(context->sourceMutex);
        MediaMutexDestroy(&context->sourceMutex);
        MediaMemFree(context);
    } else {
        MediaMutexUnLock(context->sourceMutex);
    }
    return ret;
}

int32_t SetSourceCallback(uintptr_t handle, CapturerCallback cbk, const void *caller)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->sourceMutex);
    context->callback = cbk;
    context->caller = caller;
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

uint32_t GetSourceDeviceId(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    uint32_t deviceId = ((context->userAudioInputDevice != 0) ? context->userAudioInputDevice :
                                                            context->defalutAudioInputDevice);
    MediaMutexUnLock(context->sourceMutex);
    return deviceId;
}

int32_t GetSourceChannelId(uintptr_t handle, uint32_t *channelId)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != RUNNING) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_BAD_STATE;
    }
    int32_t ret = context->capture->attr.GetCurrentChannelId((AudioHandle)context->capture, channelId);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("Get channel id failed: 0x%x", ret);
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    ALOGI("Channel id: 0x%x", *channelId);
    MediaMutexUnLock(context->sourceMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_GET_CHANNEL_ID, MEDIA_CHR_GET_CHANNEL_ID_STREAM_IN);
    return AUDIO_SUCCESS;
}

int32_t GetSourceSessionId(uintptr_t handle, AudioSession *sessionId)
{
    CHK_NULL_RETURN(sessionId, AUDIO_ERROR, "sessionId is NULL");
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    *sessionId = context->sessionId;
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

AudioStreamType GetSourceStreamType(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_STREAM_INVALID, "source handle is NULL");
    return (AudioStreamType)context->streamType;
}

int32_t GetSourceStreamId(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    return context->streamId;
}

int32_t SourceAttachBackend(uintptr_t handle, uint32_t backEndId)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != RUNNING) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_BAD_STATE;
    }
    char keyValuePair[TOTAL_STR_NUM] = { 0 };
    int32_t convertRet = sprintf_s(keyValuePair, sizeof(keyValuePair), "%s = %u", "attachbackend", backEndId);
    if (convertRet == -1) {
        ALOGE("splice key=value str fail[%d]", convertRet);
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->sourceMutex);
    int32_t ret = SetSourceParam(handle, keyValuePair, TOTAL_STR_NUM);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("SetSourceParam fail[%d]", ret);
        return AUDIO_ERROR;
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_ATTACH_CHANNEL_ID, MEDIA_CHR_ATTACH_CHANNEL_ID_STREAM_IN);
    return AUDIO_SUCCESS;
}

static int32_t SetExtraData(SourceHandleContext *context)
{
    CHK_NULL_RETURN(context->keyValueList, AUDIO_SUCCESS, "keyValueList is NULL");
    int32_t ret = context->capture->attr.SetExtraParams((AudioHandle)context->capture, context->keyValueList);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("source_handle.c SetExTraData:call render->attr.SetExtraParams failed ");
        if (context->keyValueList != NULL) {
            MediaMemFree(context->keyValueList);
            context->keyValueList = NULL;
        }
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t GetSourceTimestamp(uintptr_t handle, struct timespec *timestamp)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != RUNNING) {
        ALOGE("source not run");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }

    uint64_t frameIndex;
    struct AudioTimeStamp time = {};
    int32_t ret = context->capture->GetCapturePosition(context->capture, &frameIndex, &time);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("GetCapturePosition failed 0x%x", ret);
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }

    timestamp->tv_sec = (time_t)time.tvSec;
    timestamp->tv_nsec = (long)time.tvNSec;
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

int32_t SetSourceInputDevice(uintptr_t handle, int32_t deviceId, const AudioStreamInfo *info)
{
    // 1. 查看状态，如果还未running，就直接设置
    // 2. 如果已经running，调用 changeToute
    AUDIO_UNUSED(info);
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "track handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (!PolicyMgrCanBeRoutedToThisDevice(deviceId, false)) {
        goto ERR;
    }

    if (context->state == IDLE) {
        context->userAudioInputDevice = (uint32_t)deviceId;
    } else if (context->state == RUNNING || context->state == PAUSE) {
        int32_t halDevice = 0;
        if (ConvertOutDeviceToHal((uint32_t)deviceId, &halDevice) != ONE_DEVICE) {
            goto ERR;
        }
        struct AudioSceneDescriptor sceneDescriptor;
        sceneDescriptor.desc.pins = (uint32_t)halDevice;
        int32_t ret = context->capture->scene.SelectScene((AudioHandle)context->capture, &sceneDescriptor);
        if (ret != AUDIO_SUCCESS) {
            goto ERR;
        }
    } else {
        ALOGE("bad state[%d]", context->state);
        goto ERR;
    }
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;

ERR:
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_ERROR;
}

static bool CheckCaptureValid(const struct AudioCapture *capturer)
{
    CHK_NULL_RETURN(capturer, false, "capturer is NULL");

    CHK_NULL_RETURN(capturer->control.Start, false, "capturer control Start is NULL");
    CHK_NULL_RETURN(capturer->control.Stop, false, "capturer control Stop is NULL");

    CHK_NULL_RETURN(capturer->attr.GetCurrentChannelId, false, "capturer attr GetCurrentChannelId is NULL");

    CHK_NULL_RETURN(capturer->scene.SelectScene, false, "capturer scene SelectScene is NULL");
    CHK_NULL_RETURN(capturer->volume.GetMute, false, "capturer volume GetMute is NULL");
    CHK_NULL_RETURN(capturer->volume.SetMute, false, "capturer volume SetMute is NULL");
    CHK_NULL_RETURN(capturer->volume.SetVolume, false, "capturer volume SetVolume is NULL");
    CHK_NULL_RETURN(capturer->attr.SetExtraParams, false, "capturer attr SetExtraParams is NULL");
    CHK_NULL_RETURN(capturer->attr.GetExtraParams, false, "capturer attr GetExtraParams is NULL");
    CHK_NULL_RETURN(capturer->RegCallback, false, "capturer RegCallback is NULL");
    return true;
}

static int32_t OnCaptureCallback(enum AudioCaptureCallbackType type, const void *param, uint32_t len, void *cookie)
{
    SourceHandleContext *context = (SourceHandleContext *)cookie;
    AUDIO_UNUSED(len);
    if (context == NULL || context->callback == NULL) {
        ALOGE("context or call back is null!");
        return AUDIO_ERROR;
    }
    AudioCallbackEventType callBackType = AUDIO_EVENT_ERROR_OCCUR;
    switch (type) {
        case AUDIO_CAPTURE_VOICE_BEGIN:
            callBackType = AUDIO_EVENT_VOICE_BEGIN;
            break;
        case AUDIO_CAPTURE_VOICE_END:
            callBackType = AUDIO_EVENT_VOICE_END;
            break;
        case AUDIO_CAPTURE_VOICE_COMMAND:
            callBackType = AUDIO_EVENT_VOICE_COMMAND;
            break;
        case AUDIO_CAPTURE_ERROR_OCCUR:
            callBackType = AUDIO_EVENT_ERROR_OCCUR;
            break;
        default:
            ALOGE("Not support event Type[%d]", type);
            return AUDIO_ERROR;
    }
    return context->callback(context->caller, callBackType, param);
}

static int32_t SetVolumeAndSetMute(SourceHandleContext *context)
{
    if (SetExtraData(context) != AUDIO_SUCCESS) {
        ALOGE("render call SetExTraData failed");
        return AUDIO_ERROR;
    }

    int32_t ret = context->capture->volume.SetMute((AudioHandle)context->capture, context->micMute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("capture set volume failed");
        return AUDIO_ERROR;
    }
    if (context->streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        ret = context->capture->volume.SetVolume((AudioHandle)context->capture, context->volume);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("capture set volume failed");
            return AUDIO_ERROR;
        }
    }
    return AUDIO_SUCCESS;
}

int32_t StartSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    int32_t ret = AUDIO_SUCCESS;
    if (context->state == RUNNING) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == STOP) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    if (context->state == IDLE) {
        uint32_t validFwDevice = context->userAudioInputDevice == 0 ? context->defalutAudioInputDevice :
                                                                      context->userAudioInputDevice;
        AudioStreamConfig attr = { context->streamType, false, context->format,
                                   context->sampleRate, context->channelCnt, context->deviceId,
                                   context->connectedId, context->bitWidth, context->disableSEA,
                                   context->micType, context->bitWidth, context->isAlwaysWake, context->frameLength };
        Handle capture;
        ret = DeviceMgrCreateStreamIn(validFwDevice, attr, &capture);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("source[%d] start failed 0x%x", context->streamId, ret);
            goto EXIT;
        }
        context->capture = (struct AudioCapture *)capture;
        if (!CheckCaptureValid(context->capture)) {
            ALOGE("capturer is invalid");
            goto EXIT;
        }
        ret = context->capture->RegCallback((AudioHandle)context->capture, OnCaptureCallback, (void *)context);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("render call RegCallback failed");
            goto EXIT;
        }
    }
    if (SetVolumeAndSetMute(context) != AUDIO_SUCCESS) {
        ALOGE("SetVolumeAndSetMute failed");
        goto EXIT;
    }
    ret = context->capture->control.Start((AudioHandle)context->capture);
    if (ret != AUDIO_SUCCESS) {
        goto EXIT;
    }
    context->state = RUNNING;
    MediaMutexUnLock(context->sourceMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_START_STREAM, MEDIA_CHR_START_STREAM_IN);
    return AUDIO_SUCCESS;
EXIT:
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_ERROR;
}

int32_t ObtainSourceBuffer(uintptr_t handle, const CapturerBuffer *buffer)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != RUNNING) {
        ALOGE("source not run");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->sourceMutex);
    uint64_t readlen = ERR_INVALID_READ;
    int32_t ret = context->capture->CaptureFrame(context->capture, buffer->data, buffer->size, &readlen);
    if (ret != AUDIO_SUCCESS) {
        if ((uint32_t)ret != AUDIO_RETRY_READ) {
            ALOGE("CaptureFrame failed 0x%x", ret);
            return ERR_INVALID_READ;
        }
        return AUDIO_RETRY_READ;
    }
    return (int32_t)readlen;
}

int32_t StopSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state == STOP) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE || context->state == RELEASE) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_SUCCESS;
    }
    context->state = STOP;
    int32_t ret = context->capture->control.Stop((AudioHandle)context->capture);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("source[%d] stop failed 0x%x", context->streamId, ret);
        MediaMutexUnLock(context->sourceMutex);
        return ret;
    }
    MediaMutexUnLock(context->sourceMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_STOP_STREAM, MEDIA_CHR_STOP_STREAM_IN);
    return AUDIO_SUCCESS;
}

int32_t DeactivateCorrespondingSource(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "TrackHandleContext *context is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state == STOP ||
        context->state == RELEASE) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_SUCCESS;
    }
    if (context->state == IDLE) {
        context->state = STOP;
    }
    if (context->state == RUNNING) {
        context->state = STOP;
        int32_t ret = context->capture->control.Stop((AudioHandle)context->capture);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("source[%d] stop failed 0x%x in activate progress", context->streamId, ret);
            MediaMutexUnLock(context->sourceMutex);
            return AUDIO_ERROR;
        }
    }
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

int32_t SetSourceMute(uintptr_t handle, bool mute)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != IDLE && context->state != RUNNING) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    if (context->state == IDLE) {
        context->micMute = mute;
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_SUCCESS;
    }
    int32_t ret = context->capture->volume.SetMute((AudioHandle)context->capture, mute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("context->capture->volume.SetMute failed");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    context->micMute = mute;
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

int32_t SetSourceVolume(uintptr_t handle, float volume)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state == IDLE ||
        context->state == STOP ||
        context->state == RELEASE) {
        ALOGE("set volume failed, current state:%d", context->state);
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_BAD_STATE;
    }
    int32_t ret = context->capture->volume.SetVolume((AudioHandle)context->capture, volume);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set volume failed");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

StreamDebugInfo DumpSourceInfo(uintptr_t handle)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    if (context == NULL) {
        StreamDebugInfo temp = {};
        return temp;
    }
    MediaMutexLock(context->sourceMutex);
    ALOGD("=================stream in info streamId:%d sessionId:%u begin======================",
        context->streamId, context->sessionId);

    uint32_t curDevice = (context->userAudioInputDevice != 0) ? context->userAudioInputDevice :
                                                              context->defalutAudioInputDevice;
    ALOGD("streamState[%s] stream type[%s] stream in device[0X%X]",
        StringState(context->state), StringType(context->streamType), curDevice);

    ALOGD("=================stream in info streamId:%d sessionId:%u end======================",
        context->streamId, context->sessionId);

    AudioStreamInfo streamInfo = {
        context->streamType, context->sessionId, context->format,
        context->sampleRate, 0x10, 0, context->channelCnt, context->deviceId,
        context->connectedId, AUDIO_CHANNEL_MODE_NONE, AUDIO_STREAM_FLAG_NONE,
        AUDIO_LINK_NONE, 0, false, CAPTURE_MIC_AMIC_0, context->isAlwaysWake, false, context->frameLength
    };

    StreamDebugInfo inf = { context->state, context->streamId, streamInfo, 0, 0, curDevice };
    MediaMutexUnLock(context->sourceMutex);
    return inf;
}

int32_t SetSourceParam(uintptr_t handle, const char *keyValueList, size_t len)
{
    AUDIO_UNUSED(len);
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "keyValueList is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->state != IDLE && context->state != RUNNING) {
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    if (context->keyValueList != NULL) {
        MediaMemFree(context->keyValueList);
        context->keyValueList = NULL;
    }
    context->keyValueList = MediaMemMalloc(len + 1);
    if (context->keyValueList == NULL) {
        ALOGE("malloc failed");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = memcpy_s(context->keyValueList, len + 1, keyValueList, len);
    if (ret < 0) {
        ALOGE("memcpy_s failed");
        MediaMemFree(context->keyValueList);
        context->keyValueList = NULL;
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    if (context->capture != NULL) {
        ret = SetExtraData(context);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("Set extra data failed! ret = %d", ret);
            MediaMutexUnLock(context->sourceMutex);
            return ret;
        }
    }
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_SET_PARAM_STREAM, MEDIA_CHR_SET_PARAM_STREAM_IN);
    MediaMutexUnLock(context->sourceMutex);
    return AUDIO_SUCCESS;
}

int32_t GetSourceParam(uintptr_t handle, char *keyValueList, size_t len)
{
    SourceHandleContext *context = (SourceHandleContext *)handle;
    CHK_NULL_RETURN(context, AUDIO_ERROR, "source handle is NULL");
    CHK_NULL_RETURN(keyValueList, AUDIO_ERROR, "keyValueList is NULL");
    MediaMutexLock(context->sourceMutex);
    if (context->capture == NULL) {
        ALOGE("not create capture");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    int32_t ret = context->capture->attr.GetExtraParams((AudioHandle)context->capture, keyValueList, (int32_t)len);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("call get extra params failed ");
        MediaMutexUnLock(context->sourceMutex);
        return AUDIO_ERROR;
    }
    MediaMutexUnLock(context->sourceMutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_GET_PARAM_STREAM, MEDIA_CHR_GET_PARAM_STREAM_IN);
    return AUDIO_SUCCESS;
}
