/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2021-2021. All rights reserved.
* Description: bluetooth sub port
* Author: Media Software Group
* Create: 2021-09-28
*/

#include "bluetooth_sub_port.h"
#include "plugin_common.h"
#include "media_hal_common.h"
#include "latm_simple_muxer.h"
#include "media_hal_thread_adapt.h"
#include "sample_audio_api.h"
#include "bt_sco.h"
#include "bt_a2dp_downlink.h"
#include "bt_a2dp_uplink.h"
#include "nearlink_uplink.h"
#include "nearlink_downlink.h"
#include "common.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MODULE_NAME "AudioBluetoothSubPort"

static enum AudioFormat g_supportFormats[] = {
#ifdef SUPPORT_LOW_DELAY
    AUDIO_FORMAT_TYPE_SBC,
    AUDIO_FORMAT_TYPE_AAC_LC
#endif
};

typedef int32_t (*CreateTrackFunc)(AudioHandle portHandle);
typedef int32_t (*DestroyTrackFunc)(AudioHandle portHandle);

typedef int32_t (*EnableTrackFunc)(AudioHandle portHandle);
typedef int32_t (*DisableTrackFunc)(AudioHandle portHandle);

typedef int32_t (*TrackSetMuteFunc)(AudioHandle portHandle, bool mute);
typedef int32_t (*TrackGetMuteFunc)(AudioHandle portHandle, bool *mute);

typedef int32_t (*TrackSetVolumeFunc)(AudioHandle portHandle, float volume);
typedef int32_t (*TrackGetVolumeFunc)(AudioHandle portHandle, float *volume);

typedef int32_t (*PauseTrackFunc)(AudioHandle portHandle);
typedef int32_t (*ResumeTrackFunc)(AudioHandle portHandle);

typedef int32_t (*SendFrameFunc)(AudioHandle portHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

typedef int32_t (*GetPositionFunc)(AudioHandle portHandle, uint64_t *frames, struct AudioTimeStamp *time);

typedef int32_t (*InvokeFunc)(AudioHandle portHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);

typedef int32_t (*RegCallbackFunc)(AudioHandle portHandle, TrackCallback callback, const AudioHandle caller);

typedef struct {
    WorkScene workScene;
    CreateTrackFunc createTrack;
    DestroyTrackFunc destroyTrack;
    EnableTrackFunc enableTrack;
    DisableTrackFunc disableTrack;
    TrackSetMuteFunc trackSetMute;
    TrackGetMuteFunc trackGetMute;
    TrackSetVolumeFunc trackSetVolume;
    TrackGetVolumeFunc trackGetVolume;
    PauseTrackFunc pauseTrack;
    ResumeTrackFunc resumeTrack;
    SendFrameFunc sendFrame;
    GetPositionFunc getPosition;
    InvokeFunc invoke;
    RegCallbackFunc regCallback;
} FuncsMap;

static FuncsMap g_funcsList[] = {
    {
        BLUETOOTH_SCO,
        CreateTrackInSCOScene, DestroyTrackInSCOScene,
        EnableTrackInSCOScene, DisableTrackInSCOScene,
        TrackSetMuteInSCOScene, TrackGetMuteInSCOScene,
        TrackSetVolumeInSCOScene, TrackGetVolumeInSCOScene,
        PauseTrackInSCOScene, ResumeTrackInSCOScene,
        SendFrameInSCOScene, GetPositionInSCOScene,
        InvokeInSCOScene, RegCallbackInSCOScene
    },
    {
        BLUETOOTH_IN_A2DP_NORMAL,
        CreateTrackInBluetoothA2DPSinkNormalScene, DestroyTrackInBluetoothA2DPSinkNormalScene,
        EnableTrackInBluetoothA2DPSinkNormalScene, DisableTrackInBluetoothA2DPSinkNormalScene,
        TrackSetMuteInBluetoothA2DPSinkNormalScene, TrackGetMuteInBluetoothA2DPSinkNormalScene,
        TrackSetVolumeInBluetoothA2DPSinkNormalScene, TrackGetVolumeInBluetoothA2DPSinkNormalScene,
        PauseTrackInBluetoothA2DPSinkNormalScene, ResumeTrackInBluetoothA2DPSinkNormalScene,
        SendFrameInBluetoothA2DPSinkNormalScene, GetPositionInBluetoothA2DPSinkNormalScene,
        InvokeInBluetoothA2DPSinkNormalScene, RegCallbackInBluetoothA2DPSinkNormalScene
    },
    {
        BLUETOOTH_OUT_A2DP_NORMAL,
        CreateTrackInA2DPSourceNormalScene, DestroyTrackInA2DPSourceNormalScene,
        EnableTrackInA2DPSourceNormalScene, DisableTrackInA2DPSourceNormalScene,
        TrackSetMuteInA2DPSourceNormalScene, TrackGetMuteInA2DPSourceNormalScene,
        TrackSetVolumeInA2DPSourceNormalScene, TrackGetVolumeInA2DPSourceNormalScene,
        PauseTrackInA2DPSourceNormalScene, ResumeTrackInA2DPSourceNormalScene,
        SendFrameInA2DPSourceNormalScene, GetPositionInA2DPSourceNormalScene,
        InvokeInA2DPSourceNormalScene, RegCallbackInA2DPSourceNormalScene
    },
    {
        NEARLINK_IN_NORMAL,
        CreateTrackInNearlinkUplinkScene, DestroyTrackInNearlinkUplinkScene,
        EnableTrackInNearlinkUplinkScene, DisableTrackInNearlinkUplinkScene,
        TrackSetMuteInNearlinkUplinkScene, TrackGetMuteInNearlinkUplinkScene,
        TrackSetVolumeInNearlinkUplinkScene, TrackGetVolumeInNearlinkUplinkScene,
        PauseTrackInNearlinkUplinkScene, ResumeTrackInNearlinkUplinkScene,
        SendFrameInNearlinkUplinkScene, GetPositionInNearlinkUplinkScene,
        InvokeInNearlinkUplinkScene, RegCallbackInNearlinkUplinkScene
    },
    {
        NEARLINK_OUT_NORMAL,
        CreateTrackInNearlinkDownlinkScene, DestroyTrackInNearlinkDownlinkScene,
        EnableTrackInNearlinkDownlinkScene, DisableTrackInNearlinkDownlinkScene,
        TrackSetMuteInNearlinkDownlinkScene, TrackGetMuteInNearlinkDownlinkScene,
        TrackSetVolumeInNearlinkDownlinkScene, TrackGetVolumeInNearlinkDownlinkScene,
        PauseTrackInNearlinkDownlinkScene, ResumeTrackInNearlinkDownlinkScene,
        SendFrameInNearlinkDownlinkScene, GetPositionInNearlinkDownlinkScene,
        InvokeInNearlinkDownlinkScene, RegCallbackInNearlinkDownlinkScene
    },
};

static bool GetWorkScene(enum AudioPortPin portPin, enum AudioFormat audioFormat, WorkScene *workScene)
{
    bool isLowDelay = false;
    for (uint32_t i = 0; i < ARRAY_SIZE(g_supportFormats); i++) {
        if (audioFormat == g_supportFormats[i]) {
            isLowDelay = true;
        }
    }
    MEDIA_HAL_LOGI(MODULE_NAME, "audioFormat[0x%x] isLowDelay[%d]", audioFormat, isLowDelay);
    switch (portPin) {
        case PIN_IN_BLUETOOTH_SCO:
        case PIN_OUT_BLUETOOTH_SCO:
            *workScene = BLUETOOTH_SCO;
            break;
        case PIN_IN_BLUETOOTH_A2DP:
            *workScene = BLUETOOTH_IN_A2DP_NORMAL;
            break;
        case PIN_OUT_BLUETOOTH_A2DP:
            *workScene = BLUETOOTH_OUT_A2DP_NORMAL;
            break;
        case PIN_IN_NEARLINK_DONGLE:
            *workScene = NEARLINK_IN_NORMAL;
            break;
        case PIN_OUT_NEARLINK_DONGLE:
            *workScene = NEARLINK_OUT_NORMAL;
            break;
        default:
            MEDIA_HAL_LOGW(MODULE_NAME, "invalid pin 0x%x", portPin);
            return false;
    }

    return true;
}

int32_t AudioBluetoothSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].regCallback(trackHandle, callback, caller);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d register callback failed", portTrackCtx->workScene);
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(latencyMs);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortEnableTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    int32_t ret = g_funcsList[portTrackCtx->workScene].enableTrack(trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d enable track failed", portTrackCtx->workScene);
        return MEDIA_HAL_ERR;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortDisableTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].disableTrack(trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d disable track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortPauseTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].pauseTrack(trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d pause track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortResumeTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].resumeTrack(trackHandle);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d resume track failed", portTrackCtx->workScene);
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortFlushTrack(AudioHandle trackHandle)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    /* for bluetooth headset recording scenario */
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(buffer);
    MEDIA_HAL_UNUSED(requestBytes);
    MEDIA_HAL_UNUSED(replyBytes);

    MEDIA_HAL_LOGD(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    CHK_FAILED_RETURN_NO_LOG(portTrackCtx->workScene, BLUETOOTH_OUT_A2DP_NORMAL, MEDIA_HAL_OK);

    int32_t ret = g_funcsList[portTrackCtx->workScene].getPosition(trackHandle, frames, time);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d get position failed", portTrackCtx->workScene);
        return ret;
    }

    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    return g_funcsList[portTrackCtx->workScene].sendFrame(trackHandle, buffer, requestBytes, replyBytes);
}

int32_t AudioBluetoothSubPortTrackSetMute(AudioHandle trackHandle, bool mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;
    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetMute(AudioHandle trackHandle, bool *mute)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(mute);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackSetVolume(AudioHandle trackHandle, float volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    int32_t ret = g_funcsList[portTrackCtx->workScene].trackSetVolume(trackHandle, volume);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d set volume failed", portTrackCtx->workScene);
        return ret;
    }

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortTrackGetVolume(AudioHandle trackHandle, float *volume)
{
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MEDIA_HAL_UNUSED(portTrackCtx);
    MEDIA_HAL_UNUSED(volume);

    MEDIA_HAL_LOGI(MODULE_NAME, "not support");
    return MEDIA_HAL_OK;
}

static int32_t BluetoothPortSetAencAttr(SubPortContext *portCtx, const struct InvokeAttr *invokeAttr)
{
    if (portCtx == NULL || invokeAttr == NULL || invokeAttr->request == NULL) {
        return MEDIA_HAL_INVALID_PARAM;
    }
    MediaHalMutexLock(portCtx->aencMutex);
    AudioA2dpAencAttr *aencAttr = (AudioA2dpAencAttr *)invokeAttr->request;
    if (memcpy_s(&portCtx->aencAttr, sizeof(AudioA2dpAencAttr), aencAttr, sizeof(AudioA2dpAencAttr)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memcpy fail!");
        MediaHalMutexUnLock(portCtx->aencMutex);
        return MEDIA_HAL_ERR;
    }
    MediaHalMutexUnLock(portCtx->aencMutex);
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr)
{
    CHK_NULL_RETURN(trackHandle, MEDIA_HAL_ERR, "trackHandle is NULL");
    /* for set shm id in voice call scenario */
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    if (invokeID == INVOKE_ID_SET_A2DP_CODEC_ATTR) {
        return BluetoothPortSetAencAttr((SubPortContext *)trackHandle, invokeAttr);
    }

    int32_t ret = g_funcsList[portTrackCtx->workScene].invoke(trackHandle, invokeID, invokeAttr);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d invoke failed", portTrackCtx->workScene);
        return ret;
    }

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortCreate(AudioHandle *portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)MediaHalMalloc(sizeof(SubPortContext));
    if (portCtx == NULL) {
        MEDIA_HAL_LOGI(MODULE_NAME, "malloc failed");
        return MEDIA_HAL_NO_MEM;
    }

    int32_t ret = MEDIA_HAL_OK;
    MediaHalMutexAttr attr = { false };
    if (memset_s(portCtx, sizeof(SubPortContext), 0, sizeof(SubPortContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE;
    }

    portCtx->a2dpTrackMutex = MediaHalMutexCreate(&attr);
    if (portCtx->a2dpTrackMutex == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "create mutex failed");
        goto FREE;
    }

    ret = SubPortTrackManagerInit();
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "SubPortTrackManagerInit failed");
        goto DESTROY_MUTEX;
    }
    ret = BtEventCBCreate(portCtx);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "BtEventCBCreate failed");

    *portHandle = portCtx;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;

DESTROY_MUTEX:
    MediaHalMutexDestroy(&portCtx->a2dpTrackMutex);
FREE:
    MediaHalFree(portCtx);
    return MEDIA_HAL_ERR;
}

int32_t AudioBluetoothSubPortDestroy(AudioHandle portHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;

    int32_t ret = BtEventCBDestroy();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "BtEventCBDestroy failed");

    ret = SubPortTrackManagerDeInit();
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortTrackManagerDeInit failed");

    MediaHalMutexDestroy(&portCtx->a2dpTrackMutex);

    MediaHalFree(portCtx);

    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability)
{
    capability->formats = g_supportFormats;
#ifdef AUDIO_MANAGER_DUMP_INFO
    for (uint32_t i = 0; i < sizeof(g_supportFormats) / sizeof(g_supportFormats[0]); i++) {
        MEDIA_HAL_LOGE(MODULE_NAME, "bluetooth supportFormats:%x", g_supportFormats[i]);
    }
#endif
    MEDIA_HAL_UNUSED(port);
    MEDIA_HAL_UNUSED(capability);
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

static int32_t AudioBluetoothSubPortCaptureRegCallback(AudioHandle trackHandle,
    AudioCaptureCallback callback, AudioHandle caller)
{
    MEDIA_HAL_UNUSED(trackHandle);
    MEDIA_HAL_UNUSED(callback);
    MEDIA_HAL_UNUSED(caller);
    return MEDIA_HAL_OK;
}

static bool CheckInputBitWidth(uint32_t bitWidth)
{
    if (bitWidth == AUDIO_BIT_WIDTH_8_BIT ||
        bitWidth == AUDIO_BIT_WIDTH_16_BIT ||
        bitWidth == AUDIO_BIT_WIDTH_24_BIT ||
        bitWidth == AUDIO_BIT_WIDTH_32_BIT ||
        bitWidth == AUDIO_BIT_WIDTH_32_FLOAT_BIT) {
        return true;
    }
    return false;
}

static int32_t SubPortTrackInit(SubPortContext *portCtx, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, SubPortTrackContext *portTrackCtx)
{
    portTrackCtx->inputAttr = *inputAttr;

    /* need fix: the player supports only 16-bit width pcm, need stream manager pass bit depth parameter */
    if (!CheckInputBitWidth(inputAttr->bitWidth)) {
        portTrackCtx->inputAttr.bitWidth = AUDIO_BIT_WIDTH_16_BIT;
    }

    if (!GetSubPortType(portPin, &portTrackCtx->subPortType)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetSubPortType failed");
        return MEDIA_HAL_ERR;
    }

    if (!GetWorkScene(portPin, inputAttr->audioFormat, &portTrackCtx->workScene)) {
        MEDIA_HAL_LOGE(MODULE_NAME, "GetWorkScene failed");
        return MEDIA_HAL_ERR;
    }

    portTrackCtx->common.GetFrameCount = NULL;
    portTrackCtx->common.GetLatency = AudioBluetoothSubPortTrackGetLatency;
    portTrackCtx->common.EnableTrack = AudioBluetoothSubPortEnableTrack;
    portTrackCtx->common.PauseTrack = AudioBluetoothSubPortPauseTrack;
    portTrackCtx->common.ResumeTrack = AudioBluetoothSubPortResumeTrack;
    portTrackCtx->common.FlushTrack = AudioBluetoothSubPortFlushTrack;
    portTrackCtx->common.DisableTrack = AudioBluetoothSubPortDisableTrack;
    portTrackCtx->common.AcquireFrame = AudioBluetoothSubPortAcquireFrame;
    portTrackCtx->common.SendFrame = AudioBluetoothSubPortSendFrame;
    portTrackCtx->common.GetPosition = AudioBluetoothSubPortGetPosition;
    portTrackCtx->common.SetMute = AudioBluetoothSubPortTrackSetMute;
    portTrackCtx->common.GetMute = AudioBluetoothSubPortTrackGetMute;
    portTrackCtx->common.SetVolume = AudioBluetoothSubPortTrackSetVolume;
    portTrackCtx->common.GetVolume = AudioBluetoothSubPortTrackGetVolume;
    portTrackCtx->common.Invoke = AudioBluetoothSubPortInvoke;
    portTrackCtx->common.RegCallback = AudioBluetoothSubPortTrackRegCallback;
    portTrackCtx->common.RegCaptureCallback = AudioBluetoothSubPortCaptureRegCallback;
    portTrackCtx->isRaw = IsRawFormat(inputAttr->audioFormat);
    portTrackCtx->subPortCtx = portCtx;
    portTrackCtx->isHandlePublicResource = true;
    MEDIA_HAL_LOGD(MODULE_NAME, "success");
    return MEDIA_HAL_OK;
}

int32_t AudioBluetoothSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;
    int32_t trackCount = -1;
    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    int32_t trackId = SubPortGetFreeTrack(&trackCount);
    if (trackId == INVALID_ID) {
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t ret = MEDIA_HAL_ERR;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)MediaHalMalloc(sizeof(SubPortTrackContext));
    if (portTrackCtx == NULL) {
        MEDIA_HAL_LOGE(MODULE_NAME, "malloc failed");
        ret = MEDIA_HAL_NO_MEM;
        goto FREE_TRACK;
    }

    if (memset_s(portTrackCtx, sizeof(SubPortTrackContext), 0, sizeof(SubPortTrackContext)) != EOK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "memset_s failed");
        goto FREE_TRACK;
    }

    if (SubPortTrackInit(portCtx, inputAttr, portPin, portTrackCtx) != MEDIA_HAL_OK) {
        goto FREE_TRACK;
    }
    /* sound + aenc + adp for BLUETOOTH_A2DP_NORMAL workScene */
    ret = g_funcsList[portTrackCtx->workScene].createTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        goto FREE_TRACK;
    }

    portTrackCtx->trackId = trackId;
    portCtx->portTrack[trackId] = portTrackCtx;
    *trackHandle = &portTrackCtx->common;
    /* A2dp sink 和 sco 不占用蓝牙资源和aenc，所以这里默认就是暂停的状态 */
    if (portTrackCtx->workScene == BLUETOOTH_IN_A2DP_NORMAL ||
        portTrackCtx->workScene == BLUETOOTH_SCO ||
        portTrackCtx->workScene == NEARLINK_IN_NORMAL ||
        portTrackCtx->workScene == NEARLINK_OUT_NORMAL) {
        SubPortTrackSetPause(trackId, true);
    }

    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    return MEDIA_HAL_OK;

FREE_TRACK:
    (void)SubPortFreeTrack(trackId, &trackCount);
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    return ret;
}

static bool BluetoothPortIsValidTrack(const SubPortContext *portCtx, const SubPortTrackContext *portTrackCtx)
{
    for (int32_t i = 0; i < BT_PORT_MAX_TRACK_NUM; i++) {
        if (portCtx->portTrack[i] == portTrackCtx) {
            return true;
        }
    }
    return false;
}

int32_t AudioBluetoothSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle)
{
    SubPortContext *portCtx = (SubPortContext *)portHandle;
    SubPortTrackContext *portTrackCtx = (SubPortTrackContext *)trackHandle;

    MediaHalMutexLock(portCtx->a2dpTrackMutex);
    bool isValid = BluetoothPortIsValidTrack(portCtx, portTrackCtx);
    if (!isValid) {
        MEDIA_HAL_LOGE(MODULE_NAME, "track is invalid");
        MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
        return MEDIA_HAL_INVALID_PARAM;
    }
    int32_t trackCount = -1;
    int32_t ret = SubPortFreeTrack(portTrackCtx->trackId, &trackCount);
    CHK_FAILED_NO_RETURN(ret, MEDIA_HAL_OK, "SubPortFreeTrack failed");

    portTrackCtx->trackCount = trackCount;
    ret = g_funcsList[portTrackCtx->workScene].destroyTrack(portTrackCtx);
    if (ret != MEDIA_HAL_OK) {
        MEDIA_HAL_LOGE(MODULE_NAME, "work scene %d destroyTrack failed", portTrackCtx->workScene);
    }
    MediaHalMutexUnLock(portCtx->a2dpTrackMutex);
    MEDIA_HAL_LOGD(MODULE_NAME, "success trackId (%d)", portTrackCtx->trackId);

    if (portTrackCtx->inputAttr.extraData != NULL) {
        MediaHalFree(portTrackCtx->inputAttr.extraData);
        portTrackCtx->inputAttr.extraData = NULL;
        portTrackCtx->inputAttr.extraDataSize = 0;
    }

    portCtx->portTrack[portTrackCtx->trackId] = NULL;
    MediaHalFree(portTrackCtx);
    return MEDIA_HAL_OK;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
