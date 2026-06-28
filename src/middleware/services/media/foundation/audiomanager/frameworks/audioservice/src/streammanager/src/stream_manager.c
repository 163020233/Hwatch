/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
* Description: stream manager interfaces
* Author: Media Software Group
* Create: 2021-04-21
*/

#include "stream_manager.h"
#include <stdlib.h>
#include <math.h>
#include "securec.h"
#include "policy_manager.h"
#include "source_handle.h"
#include "track_handle.h"
#include "audio_errors.h"
#include "audio_utils.h"
#include "stream_config.h"
#include "device_manager.h"
#ifdef SUPPORT_NV
#include "nv.h"
#ifdef MELODY_AUDIO_SUPPORT
#include "key_struct.h"
#else
#include "common.h"
#endif
#endif
#include "media_thread_adapt.h"
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
#include "hifi_clock_freq_manager.h"
#endif
#include "media_mem.h"
#include "media_chr_log.h"

#define LOG_MODULE_NAME "StreamMgr"
#define MAX_KEY_STR_SIZE 30
#define FLOAT_VOLUME_TYPE 0xFF
#define FLOAT_TO_INT 1000
#define BUFFER_SIZE 10
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#define CONCURRENT_ACTIVE_STREAM_MAX_NUM 10
#define ONE_STREAM_EXCLUSIVE 1
#define MAX_LISTENER_NUM 6
#define GOLDEN_POINT 0.618
#define DEFAULT_MIXWEIGHT 0.5
#define MAX_VOLUME_POINT 1
#define THOUSAND 1000.0
#define MAX_NV_TO_CHAR_NUM 256
#define MAX_STREAM_IN_NUM 3
#define PERCENT_TO_GAIN_LOG_FACTOR 20
#define MAGNIFY_TIMES 1000.0
#define ROUND_UP 0.5
#define MEDIA_TYPE_VOLUME "media_type_volume"
#define RING_TYPE_VOLUME "ring_type_volume"
#define VOICE_CALL_TYPE_VOLUME "voice_call_type_volume"
#define SSCANF_RET_NUM 8

typedef struct {
    StreamChangeCallback *listenerPtr;
    int curCapbility;
    int curNum;
} ListenersWrapper;

typedef struct {
    int32_t streamId;
    uintptr_t streamHandle;
    AudioStreamType streamType;
    bool active;
    AudioLinkDirection linkDir;
} StreamSimpleInfo;

typedef struct {
    // 同时存在的流应当不会超过10个
    StreamSimpleInfo simpleInfos[CONCURRENT_ACTIVE_STREAM_MAX_NUM];
    StreamSimpleInfo curInputStreamInfo[MAX_STREAM_IN_NUM];
    float typeVolume[STREAM_TYPE_NUM];
    bool typeMute[STREAM_TYPE_NUM];
    float typeMixWeight[STREAM_TYPE_NUM];
    bool masterMute;
    bool micMute;
    AudioRingMode ringMode;
    ListenersWrapper listeners;
    int32_t maxOutStreamId;
    int32_t maxInputStreamId;
     // 记录混音时候修改流类型音量的计数，此时修改的流类型音量不需要记录
    int32_t tempVolumeChangeCount[STREAM_TYPE_NUM];
} StreamManagerContext;

typedef enum {
    CREATE,
    DESTROY
} CurrentStage;

typedef struct {
    AudioStreamType streamType;
    AudioLinkDirection currentDirection;
    AudioLinkDirection dependDirection;
    AudioDeviceType dependRouteDevice;
} DependStream;

static DependStream g_dependStreamType[] = {
    {AUDIO_STREAM_A2DP_MUSIC, AUDIO_DOWN_LINK, AUDIO_UP_LINK, IN_BLUETOOTH_A2DP},
    {AUDIO_STREAM_VOICE_CALL_BT_SCO, AUDIO_DOWN_LINK, AUDIO_UP_LINK, IN_BLUETOOTH_SCO},
    {AUDIO_STREAM_VOICE_CALL_BT_SCO, AUDIO_UP_LINK, AUDIO_DOWN_LINK, OUT_BLUETOOTH_SCO},
};

static StreamManagerContext g_streamMgrContext = {0};
static bool g_inited = false;
static MediaMutexHandle g_mutex = NULL;
static MediaMutexHandle g_globalVarMutex = NULL;

static AudioStreamType g_supportRouteDeviceChange[] = {
    AUDIO_STREAM_MUSIC,
    AUDIO_STREAM_NOTIFICATION_SYSTEM,
};

static inline void SetDownlinkStreamVolume(int32_t index, float value)
{
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("SetDownlinkStreamVolume: invalid index %d", index);
        return;
    }
    MediaMutexLock(g_globalVarMutex);
    g_streamMgrContext.typeVolume[index] = value;
    MediaMutexUnLock(g_globalVarMutex);
}

static inline void SetDownlinkStreamMute(int32_t index, bool mute)
{
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("SetDownlinkStreamMute: invalid index %d", index);
        return;
    }
    MediaMutexLock(g_globalVarMutex);
    g_streamMgrContext.typeMute[index] = mute;
    MediaMutexUnLock(g_globalVarMutex);
}

static inline void SetMixWeight(int32_t index, float weight)
{
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("SetMixWeight: invalid index %d", index);
        return;
    }
    MediaMutexLock(g_globalVarMutex);
    g_streamMgrContext.typeMixWeight[index] = weight;
    MediaMutexUnLock(g_globalVarMutex);
}

static inline float GetDownlinkStreamVolume(int32_t index)
{
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("GetDownlinkStreamVolume: invalid index %d", index);
        return 0.0;
    }
    MediaMutexLock(g_globalVarMutex);
    float value = g_streamMgrContext.typeVolume[index];
    MediaMutexUnLock(g_globalVarMutex);
    return value;
}

static inline bool GetDownlinkStreamMute(int32_t index)
{
    // 防御性编程：确保 index 在合法范围内
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("GetDownlinkStreamMute: invalid index %d", index);
        return false;
    }
    MediaMutexLock(g_globalVarMutex);
    bool mute = g_streamMgrContext.typeMute[index];
    MediaMutexUnLock(g_globalVarMutex);
    
    return mute;
}

static inline float GetMixWeight(int32_t index)
{
    if (index < 0 || index >= STREAM_TYPE_NUM) {
        ALOGE("GetMixWeight: invalid index %d", index);
        return 0.0;
    }
    MediaMutexLock(g_globalVarMutex);
    float weight = g_streamMgrContext.typeMixWeight[index];
    MediaMutexUnLock(g_globalVarMutex);
    return weight;
}

static StreamTypeHashId FindStreamTypeHashId(int32_t streamType);
static AudioStreamStrategy *GetStrategyFromPolicyManager(const AudioStreamInfo *inf,
    uint32_t streamId, StreamStatus status);

inline static bool IsValidType(AudioStreamType streamType)
{
    for (int32_t i = 0; i < STREAM_TYPE_NUM; i++) {
        if (streamType == typeVolumes[i].streamType) {
            return true;
        }
    }
    return false;
}

static bool CheckStreamTypeSupportRouteDeviceChange(AudioStreamType streamType)
{
    for (uint32_t i = 0; i < sizeof(g_supportRouteDeviceChange) /
        sizeof(g_supportRouteDeviceChange[0]); i++) {
        if (g_supportRouteDeviceChange[i] == streamType) {
            return true;
        }
    }
    return false;
}

inline static double ReverseLogCalculation(double value)
{
    double lineVolume = exp(value / PERCENT_TO_GAIN_LOG_FACTOR);
    return lineVolume;
}

#if defined(SUPPORT_NV)
static void RecordValue(uint16_t keyId, const audio_stream_common_t *nvData)
{
    uint32_t index = 0;
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    for (uint32_t i = 0; i < STREAM_TYPE_NUM; i++) {
        int16_t key = 0;
        int16_t newValue = 0;
        if (index <= nvDataSize) {
            key = nvData->value[index];
            newValue = nvData->value[index + 1];
            index += 0x2;
        }
        if (keyId == AUDIO_VOLUME_FOR_STORAGE) {
            SetDownlinkStreamVolume(key, newValue / THOUSAND);
        } else if (keyId == AUDIO_MIXWEIGHT_FOR_STORAGE) {
            SetMixWeight(key, newValue / THOUSAND);
        }
    }
}
#endif

static void RecordDefalutValue(uint16_t keyId)
{
    for (uint32_t i = 0; i < STREAM_TYPE_NUM; i++) {
        if (keyId == AUDIO_VOLUME_FOR_STORAGE) {
            if (i == AUDIO_STREAM_VOICE_CALL_VOLTE_HASH_ID ||
                i == AUDIO_STREAM_LIVE_MIC_HASH_ID) {
                SetDownlinkStreamVolume(i, MAX_VOLUME_POINT);
            } else {
                SetDownlinkStreamVolume(i, GOLDEN_POINT);
            }
        } else if (keyId == AUDIO_MIXWEIGHT_FOR_STORAGE) {
            SetMixWeight(i, DEFAULT_MIXWEIGHT);
        }
    }
}

static void GetValueByKey(uint16_t keyId)
{
#if defined(SUPPORT_NV)
    uint16_t readLen = 0;
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    audio_stream_common_t *nvData = (audio_stream_common_t *)MediaMemCalloc(nvDataSize);
    if (nvData == NULL) {
         ALOGE("nvData malloc failed!");
         return;
    }
    errcode_t nvRet = uapi_nv_read(keyId, nvDataSize, &readLen, (uint8_t *)nvData);
    if (nvRet == ERRCODE_SUCC) {
        RecordValue(keyId, nvData);
    } else {
        ALOGI("nv get value wrong, ret[%d]", nvRet);
        RecordDefalutValue(keyId);
    }
    MediaMemFree(nvData);
#else
    ALOGI("not support key value read, just default value");
    RecordDefalutValue(keyId);
#endif
}

static int32_t WriteValueByKey(uint16_t keyId)
{
#if defined(SUPPORT_NV)
    uint32_t nvDataSize = sizeof(audio_stream_common_t);
    audio_stream_common_t *nvData = (audio_stream_common_t *)MediaMemCalloc(nvDataSize);
    if (nvData == NULL) {
         ALOGE("nvData malloc failed!");
         return AUDIO_ERROR;
    }
    uint32_t index = 0;
    for (uint32_t i = 0; i < STREAM_TYPE_NUM; i++) {
        float value = 0;
        if (keyId == AUDIO_VOLUME_FOR_STORAGE) {
            value = GetDownlinkStreamVolume(i);
        } else if (keyId == AUDIO_MIXWEIGHT_FOR_STORAGE) {
            value = GetMixWeight(i);
        }
        int32_t newValue = (int32_t)(value * THOUSAND + ROUND_UP);
        if (index <= nvDataSize) {
            nvData->value[index] = (int16_t)i;
            nvData->value[index + 1] = (int16_t)newValue;
            index += 0x2;
        }
    }
    errcode_t nvRet = uapi_nv_write(keyId, (uint8_t *)nvData, nvDataSize);
    if (nvRet != ERRCODE_SUCC) {
        ALOGW("uapi_nv_write failed! ret = [%x]", nvRet);
        MediaMemFree(nvData);
        return AUDIO_ERROR;
    }
    MediaMemFree(nvData);
    return AUDIO_SUCCESS;
#else
    ALOGW("not support storage keyId[%x]", keyId);
    return AUDIO_ERROR;
#endif
}

static void InitTypeVolume(void)
{
    GetValueByKey(AUDIO_VOLUME_FOR_STORAGE);
}

static void InitMixWeight(void)
{
    GetValueByKey(AUDIO_MIXWEIGHT_FOR_STORAGE);
}

static int32_t RouteDeviceChange(int32_t deviceId)
{
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        bool valid = g_streamMgrContext.simpleInfos[i].active &&
            g_streamMgrContext.simpleInfos[i].streamId != -1;
        if (!valid) {
            continue;
        }
        // sco/fitness video no need change
        AudioStreamType streamType = g_streamMgrContext.simpleInfos[i].streamType;
        if (CheckStreamTypeSupportRouteDeviceChange(streamType)) {
            uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
            int32_t ret = SetTrackOutputDevice(streamHandle, deviceId, NULL);
            CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "set output device failed");
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t PolicyCallBack(PolicyCallBackType type, uint32_t dataLen, const uint8_t *callbackData)
{
    AUDIO_UNUSED(dataLen);
    MediaMutexLock(g_mutex);
    switch (type) {
        case POLICY_ROUTE_DEVICE_CHANGED: {
            int32_t deviceId = *((int32_t*)callbackData);
            if (RouteDeviceChange(deviceId) != AUDIO_SUCCESS) {
                ALOGE("change route device err");
                MediaMutexUnLock(g_mutex);
                return AUDIO_ERROR;
            }
            break;
        }
        default: {
            ALOGE("invalid type");
            MediaMutexUnLock(g_mutex);
            return AUDIO_ERROR;
        }
    }
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static void InitTypeMute(void)
{
    for (int i = 0; i < STREAM_TYPE_NUM; i++) {
        g_streamMgrContext.typeMute[i] = false;
    }
}
static void InitInputStream(void)
{
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        g_streamMgrContext.curInputStreamInfo[i].streamId = -1;
        g_streamMgrContext.curInputStreamInfo[i].active = false;
    }
}
static void InitOutputStream(void)
{
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        g_streamMgrContext.simpleInfos[i].active = false;
        g_streamMgrContext.simpleInfos[i].streamId = -1;
    }
}
int32_t StreamMgrInit(void)
{
    ALOGI("StreamMgrInit IN");
    MediaInitStaticMutexLock(&g_mutex);
    MediaInitStaticMutexLock(&g_globalVarMutex);
    MediaMutexLock(g_mutex);
    if (g_inited) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_SUCCESS;
    }
    InitMixWeight();
    InitTypeVolume();
    const AudioPolicyCallback callback = { PolicyCallBack };
    int32_t ret = PolicyMgrRegisterPolicyCallback(&callback);
    if (ret != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    g_streamMgrContext.maxOutStreamId = -1;
    g_streamMgrContext.maxInputStreamId = -1;
    InitInputStream();
    InitOutputStream();

    g_streamMgrContext.listeners.listenerPtr = NULL;
    g_streamMgrContext.listeners.curCapbility = 0;
    g_streamMgrContext.listeners.curNum = 0;
    g_streamMgrContext.ringMode = RINGER_MODE_NORMAL;
    g_streamMgrContext.micMute = false;
    g_streamMgrContext.masterMute = false;
    InitTypeMute();
    g_inited = true;
    MediaMutexUnLock(g_mutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_AUDIO_MANAGER_INIT, MEDIA_CHR_STREAM_MANAGER_INIT, 0);
    ALOGI("StreamMgrInit SUCCESS");
    return AUDIO_SUCCESS;
}

int32_t StreamMgrDeInit(void)
{
    // 如果有线程，这里销毁，其他资源也要释放
    MediaMutexLock(g_mutex);
    if (!g_inited) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    int32_t ret = WriteValueByKey(AUDIO_VOLUME_FOR_STORAGE);
    if (ret != AUDIO_SUCCESS) {
        ALOGW("write volume nv failed!");
    }
    ret = WriteValueByKey(AUDIO_MIXWEIGHT_FOR_STORAGE);
    if (ret != AUDIO_SUCCESS) {
        ALOGW("write mix weight nv failed!");
    }
    g_inited = false;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

static int32_t CheckUpdateListener(void)
{
    bool err = g_streamMgrContext.listeners.curCapbility == 0 &&
        g_streamMgrContext.listeners.listenerPtr != NULL;
    if (err) {
        return AUDIO_ERROR;
    }

    if (g_streamMgrContext.listeners.curCapbility == 0) {
        g_streamMgrContext.listeners.listenerPtr =
            (StreamChangeCallback*)MediaMemMalloc(MAX_LISTENER_NUM * sizeof(StreamChangeCallback));
        if (g_streamMgrContext.listeners.listenerPtr == NULL) {
            return AUDIO_ERROR;
        }
        g_streamMgrContext.listeners.curCapbility = MAX_LISTENER_NUM;
    }
    if (g_streamMgrContext.listeners.curNum + 1 == g_streamMgrContext.listeners.curCapbility) {
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamMgrSetCallback(const StreamChangeCallback *callback)
{
    CHK_NULL_RETURN(callback, AUDIO_ERROR, "StreamChangeCallback is NULL");
    MediaMutexLock(g_mutex);
    if (CheckUpdateListener() != AUDIO_SUCCESS) {
        ALOGE("check listener update fail");
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    int curIndex = g_streamMgrContext.listeners.curNum;
    g_streamMgrContext.listeners.listenerPtr[curIndex] = *callback;
    g_streamMgrContext.listeners.curNum++;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

static AudioStreamStrategy *GetDestroyStrategyFromPolicyManager(uintptr_t streamHandle, StreamStatus status)
{
    int32_t streamId;
    AudioSession sessionId;
    GetHandleId(streamHandle, &streamId, &sessionId);
    AudioStreamInfo streamInfForDestroy = { 0 };
    streamInfForDestroy.streamType = GetStreamType(streamHandle);
    streamInfForDestroy.sessionID = sessionId;
    return GetStrategyFromPolicyManager(&streamInfForDestroy, (uint32_t)streamId, status);
}

static int32_t DestroyDownLinkStream(AudioStreamType streamType,
    AudioLinkDirection dependDirection)
{
    if (dependDirection != AUDIO_DOWN_LINK) {
        return AUDIO_SUCCESS;
    }
    for (int32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.simpleInfos[i];
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        if (curStreamInfo.streamType != streamType || streamHandle == 0) {
            continue;
        }
        if (PolicyMgrIsNeedResumeForStream(curStreamInfo.streamType) &&
            curStreamInfo.streamType != AUDIO_STREAM_A2DP_MUSIC) {
            if (PauseTrack(streamHandle) != AUDIO_SUCCESS) {
                ALOGE("stop stream type [%x] track handle failed!", streamType);
                return AUDIO_ERROR;
            }
        } else {
            if (StopTrack(streamHandle) != AUDIO_SUCCESS) {
                ALOGE("stop stream type [%x] track handle failed!", streamType);
                return AUDIO_ERROR;
            }
            GetDestroyStrategyFromPolicyManager(streamHandle, STREAM_OUT_DESTROY);
            if (DestroyTrackHandle(streamHandle, false) != AUDIO_SUCCESS) {
                ALOGE("destroy stream type [%x] track handle failed!", streamType);
                return AUDIO_ERROR;
            }
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t DestroyUpLinkStream(AudioStreamType streamType,
    AudioLinkDirection dependDirection)
{
    if (dependDirection != AUDIO_UP_LINK) {
        return AUDIO_SUCCESS;
    }
    for (int32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        if (curStreamInfo.streamType != streamType || streamHandle == 0) {
            continue;
        }
        if (StopSource(streamHandle) != AUDIO_SUCCESS) {
            ALOGE("stop stream type [%x] source handle failed!", streamType);
            return AUDIO_ERROR;
        }
        GetDestroyStrategyFromPolicyManager(streamHandle, STREAM_IN_DESTROY);
        if (DestroySourceHandle(streamHandle, false) != AUDIO_SUCCESS) {
            ALOGE("destroy stream type [%x] source handle failed!", streamType);
            return AUDIO_ERROR;
        }
        g_streamMgrContext.curInputStreamInfo[i].streamHandle = 0;
    }
    return AUDIO_SUCCESS;
}

static int32_t DestroyDependStream(AudioStreamType streamType, AudioLinkDirection currentDirection)
{
    int32_t ret = AUDIO_SUCCESS;
    size_t size = sizeof(g_dependStreamType) / sizeof(g_dependStreamType[0]);
    for (size_t i = 0; i < size; i++) {
        if (g_dependStreamType[i].streamType == streamType &&
            g_dependStreamType[i].currentDirection == currentDirection) {
            ret = DestroyDownLinkStream(g_dependStreamType[i].streamType, g_dependStreamType[i].dependDirection);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("DestroyDownLinkStream failed!");
                return ret;
            }
            ret = DestroyUpLinkStream(g_dependStreamType[i].streamType, g_dependStreamType[i].dependDirection);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("DestroyUpLinkStream failed!");
                return ret;
            }
        }
    }
    return ret;
}

static int32_t DestroyExclusiveStreamByStrategy(const AudioStreamStrategy *strategy,
    bool isOutput)
{
    int32_t ret = AUDIO_SUCCESS;
    for (uint32_t i = 0; i < strategy->exclusiveStreamsNum; i++) {
        AudioStreamType streamType = strategy->exclusiveStreams[i]->streamDesc.streamType;
        ALOGI("DestroyExclusiveStreamByStrategy stream type [%x] isOutput[%d]!", streamType, isOutput);
        if (isOutput) {
            ret = DestroyDependStream(streamType, AUDIO_DOWN_LINK);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("DestroyDependStream failed!");
                continue;
            }
            ret = DestroyDownLinkStream(streamType, AUDIO_DOWN_LINK);
        } else {
            ret = DestroyDependStream(streamType, AUDIO_UP_LINK);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("DestroyDependStream failed!");
                continue;
            }
            ret = DestroyUpLinkStream(streamType, AUDIO_UP_LINK);
        }
    }
    return ret;
}

static bool AnalyzeStrategyForDevice(const AudioStreamStrategy *strategy, int32_t newStreamId,
    uint32_t *defaultRouteDevice, bool isOutput)
{
    bool isFound = false;
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        if (id == newStreamId) {
            *defaultRouteDevice = strategy->activeStreams[i]->currentRoutedDevices;
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        *defaultRouteDevice = 0;
        ALOGE("stream[%d] not find", newStreamId);
        return isFound;
    }
    if (DestroyExclusiveStreamByStrategy(strategy, isOutput) != AUDIO_SUCCESS) {
        ALOGE("DestroyExclusiveStreamByStrategy failed!");
        return false;
    }
    return isFound;
}

static int32_t CheckTrackState(const AudioStreamStrategy *strategy, int32_t newStreamId,
    AudioStreamType streamType)
{
    // 1. 如果不是独占情况，那只能是混音，在混音策略下，不必改变流的状态
    if (strategy->activeStreamNum > ONE_STREAM_EXCLUSIVE) {
        return AUDIO_SUCCESS;
    }
    // 2.1 如果是独占，独占肯定是新流独占，否则报错
    if (newStreamId != strategy->activeStreams[0]->streamDesc.streamIndex) {
        ALOGE("exclusive stream must be new stream[%d]", newStreamId);
        return AUDIO_ERROR;
    }
    // 2.2 如果是独占，独占的也是新流，那就检查当前老流中还running的，如果不符合那就pause非当前流类型的正在运行的流
    //     此时，新流还未入档，g_streamMgrContext 里面记录的都还是老流
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (g_streamMgrContext.simpleInfos[i].active) {
            uintptr_t handle = g_streamMgrContext.simpleInfos[i].streamHandle;
            if (GetState(handle) == RUNNING && GetStreamType(handle) != streamType) {
                CHK_FAILED_RETURN(PauseTrack(handle), AUDIO_SUCCESS, AUDIO_ERROR, "pause failed");
            }
        }
    }
    return AUDIO_SUCCESS;
}

static AudioStreamStrategy *GetStrategyFromPolicyManager(const AudioStreamInfo *inf,
    uint32_t streamId, StreamStatus status)
{
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    if (typeHashId >= STREAM_TYPE_NUM) {
        ALOGE("streamId[%d] sessionId[%d] get invalid TyepHashId[%d]!", streamId, inf->sessionID, typeHashId);
        return NULL;
    }
    float typeVol = GetDownlinkStreamVolume((int32_t)typeHashId);
    AudioStreamDescriptor newStreamDesc = { streamId, inf->streamType, typeVol, inf->linkDir };
    ALOGI("streamId[%d] sessionId[%d] typeVol[%f]", streamId, inf->sessionID, typeVol);
    return PolicyMgrGetStrategyForStream(inf->sessionID, &newStreamDesc, status);
}

static void RecordOneStream(const AudioStreamInfo *inf, int32_t newStreamId, uintptr_t handle, bool isOutput)
{
    int i;
    int32_t streamNum;
    StreamSimpleInfo *curStreamInfo = NULL;
    streamNum = isOutput ? CONCURRENT_ACTIVE_STREAM_MAX_NUM : MAX_STREAM_IN_NUM;
    curStreamInfo = isOutput ? g_streamMgrContext.simpleInfos : g_streamMgrContext.curInputStreamInfo;
    for (i = 0; i < streamNum; i++) {
        bool isNotUsed = !curStreamInfo[i].active &&
                    curStreamInfo[i].streamId == -1;
        if (isNotUsed) {
            curStreamInfo[i].streamId = newStreamId;
            curStreamInfo[i].streamHandle = handle;
            curStreamInfo[i].streamType = inf->streamType;
            curStreamInfo[i].active = true;
            curStreamInfo[i].linkDir = inf->linkDir;
            break;
        }
    }

    if (i == streamNum) {
        ALOGW("there no free record for id[%d]", newStreamId);
    }

    return;
}

static void DeleteOneStream(int32_t streamId, bool isOutput)
{
    int i;
    int32_t streamNum;
    StreamSimpleInfo *curStreamInfo = NULL;
    streamNum = isOutput ? CONCURRENT_ACTIVE_STREAM_MAX_NUM : MAX_STREAM_IN_NUM;
    curStreamInfo = isOutput ? g_streamMgrContext.simpleInfos : g_streamMgrContext.curInputStreamInfo;
    for (i = 0; i < streamNum; i++) {
        bool isMatch = curStreamInfo[i].streamId == streamId &&
                    curStreamInfo[i].active;
        if (isMatch) {
            curStreamInfo[i].streamId = -1;
            curStreamInfo[i].active = false;
            curStreamInfo[i].streamHandle = 0;
            break;
        }
    }

    if (i == streamNum) {
        ALOGW("there no match record for id[%d]", streamId);
    }

    return;
}

static void NotifyListenersStreamCreated(int newStreamId, AudioSession sessionId)
{
    // 可能有多个观察者，例如 AudioService，AS 想要了解流的变化
    int32_t listernerNum = g_streamMgrContext.listeners.curNum;
    for (int i = 0; i < listernerNum; i++) {
        uintptr_t upperInstance = g_streamMgrContext.listeners.listenerPtr[i].instance;
        if (g_streamMgrContext.listeners.listenerPtr[i].OnEvent != NULL) {
            g_streamMgrContext.listeners.listenerPtr[i].OnEvent(upperInstance,
                EventCreated, (uint32_t)newStreamId, sessionId, (uintptr_t)NULL);
        }
    }
    return;
}

static void NotifyListenersStreamDestroy(int newStreamId, AudioSession sessionId)
{
    // 可能有多个观察者，例如 AudioService，AS 想要了解流的变化
    int32_t listernerNum = g_streamMgrContext.listeners.curNum;
    for (int i = 0; i < listernerNum; i++) {
        uintptr_t upperInstance = g_streamMgrContext.listeners.listenerPtr[i].instance;
        if (g_streamMgrContext.listeners.listenerPtr[i].OnEvent != NULL) {
            g_streamMgrContext.listeners.listenerPtr[i].OnEvent(upperInstance,
                EventDestroied, (uint32_t)newStreamId, sessionId, (uintptr_t)NULL);
        }
    }
    return;
}

static bool GetMixWeightForNewStream(const AudioStreamStrategy *strategy, int32_t newStreamId,
    float *mixWeight)
{
    // 已经保证非NULL
    // 1.如果是独占，看master，mixWeight 是 1
    // 2.如果是混音，看slave，通过newStreamId找到对应的
    // 3.其他，这是不合理的
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        if (id == newStreamId) {
            *mixWeight = strategy->activeStreams[i]->mixWeight;
            return true;
        }
    }
    return false;
}

static bool IsAlreadyRecorded(int32_t streamId, uintptr_t *streamHandle, AudioStreamType *streamType)
{
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!g_streamMgrContext.simpleInfos[i].active) {
            continue;
        }
        if (streamId == g_streamMgrContext.simpleInfos[i].streamId) {
            *streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
            *streamType = g_streamMgrContext.simpleInfos[i].streamType;
            return true;
        }
    }
    return false;
}

static int32_t UpdateHaidVolume(float mixWeight)
{
    StreamTypeHashId typeHashId = FindStreamTypeHashId(AUDIO_STREAM_HEAR_AID);
    if (typeHashId == AUDIO_STREAM_TYPE_HASH_ID_MAX) {
        ALOGE("AUDIO_STREAM_HEAR_AID hash id is inValid");
        return AUDIO_ERROR;
    }
    char buf[MAX_KEY_STR_SIZE] = { 0 };
    int64_t integer = (int64_t)((GetDownlinkStreamVolume((int32_t)typeHashId) * mixWeight) * FLOAT_TO_INT);
    int32_t ret = sprintf_s(buf, MAX_KEY_STR_SIZE, "%d#%d#%d", (int32_t)(integer), 0,
        FLOAT_VOLUME_TYPE);
    if (ret == -1) {
        ALOGE("set haid volume sprintf_s failed, ret = %d", ret);
        return AUDIO_ERROR;
    }
    uint32_t length = MAX_KEY_STR_SIZE;
    ret = DeviceMgrInvoke(AUDIO_INVOKE_HAID_SET_VOLUME, buf, &length);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set haid volume failed:%d", ret);
    }
    return ret;
}

static int32_t UpdateActiveStreamMixWeightInner(AudioStreamType streamType, float mixWeightSet,
    uintptr_t streamHandle, uintptr_t curStreamhandle, CurrentStage stage)
{
    float curMixWeight = 0;
    GetStreamMixWeight(streamHandle, &curMixWeight);
    if (fabs(curMixWeight - mixWeightSet) >= COMPARE_THRESHOLD) {
        SetStreamMixWeight(streamHandle, mixWeightSet);
        ALOGE("mixWeightSet:%d, streamType:%x", (int32_t)(mixWeightSet * THOUSAND), streamType);
        StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
        if (typeHashId == AUDIO_STREAM_TYPE_HASH_ID_MAX) {
            ALOGE("streamtype hash id is inValid");
            return AUDIO_ERROR;
        }
        int32_t ret = 0;
        SetTrackMixVolumeFlag(streamHandle, stage == CREATE ? true : false);
#if defined(MELODY_DEMO_EARPHONE) || defined(MELODY_DEMO_SOUND_BOX) || defined(MELODY_DEMO_HA)
        if (TrackGetStreamSync(curStreamhandle) && TrackGetStreamSync(streamHandle)) {
            ret = TrackVolumeChangeCallback(streamHandle, GetDownlinkStreamVolume((int32_t)typeHashId), mixWeightSet);
            if (ret != AUDIO_SUCCESS) {
                ALOGE("callback set volume failed!");
            }
            g_streamMgrContext.tempVolumeChangeCount[typeHashId]++;
            ALOGI("start streamType:%x,tempVolumeChangeCount:%d",
                streamType,
                g_streamMgrContext.tempVolumeChangeCount[typeHashId]);
            return AUDIO_SUCCESS;
        }
#else
        AUDIO_UNUSED(curStreamhandle);
#endif
        ret = SetTrackVolume(
            streamHandle, GetDownlinkStreamVolume((int32_t)typeHashId), mixWeightSet, NOT_SET_STREAM_VOLUME);
        // 解决多线程竞争，概率出现老流未启动设置混音失败，此时新流也会失败退出，
        // 老流会在启动的时候设置音量，此处设置失败不应该直接向上返回错误，
        // 后续要考虑下老流设置音量的时机，此处是不是只记录音量信息即可
        if ((uint32_t)ret == AUDIO_BAD_STATE) {
            ALOGW("current state is not right!");
            return AUDIO_SUCCESS;
        }
        if (ret != AUDIO_SUCCESS) {
            ALOGW("update stream volume fail when new stream create: %d", ret);
            return AUDIO_ERROR;
        }
    }
    return AUDIO_SUCCESS;
}

static int32_t UpdateActiveStreamMixWeight(const AudioStreamStrategy *strategy, CurrentStage stage, uintptr_t curStreamhandle)
{
    for (uint32_t i = 0; i < strategy->activeStreamNum; i++) {
        int32_t id = strategy->activeStreams[i]->streamDesc.streamIndex;
        ALOGI("mix num[%u] i[%d] id[%d]", strategy->activeStreamNum, i, id);
        uintptr_t streamHandle;
        AudioStreamType streamType = strategy->activeStreams[i]->streamDesc.streamType;
        float mixWeightSet = strategy->activeStreams[i]->mixWeight;
        if (streamType == AUDIO_STREAM_HEAR_AID) {
            UpdateHaidVolume(mixWeightSet);
            continue;
        }
        if (!IsAlreadyRecorded(id, &streamHandle, &streamType)) {
            if (stage == CREATE) {
                continue; // 创建阶段, active stream 可能有新流，新流不在记录中
            } else {
                return AUDIO_ERROR; // 销毁阶段, active stream 必须在记录中
            }
        }
        int32_t ret = UpdateActiveStreamMixWeightInner(streamType, mixWeightSet, streamHandle, curStreamhandle, stage);
        if (ret != AUDIO_SUCCESS) {
            return ret;
        }
    }
    return AUDIO_SUCCESS;
}

// 1.创建流下更新和销毁流下更新，这两个情况是不同的，
// 例如，创建流时，如果是独占策略，肯定是新流是独占
// 销毁流时，如果是独占策略，是老流独占
// 2.strategy 信息无法完整描述记录中流的情况
// 例如，当前有两个流在混音，ring流过来，要独占，那么策略返回的
// 只会有ring流信息，此时发生在ring流创建的时候；
static int32_t UpdateTrackMixWeightWhenCreate(const AudioStreamStrategy *strategy, uintptr_t curStreamhandle)
{
    // 独占策略下，strategy只会返回新流情况，无需更新老流权重，直接返回即可
    if (strategy->currentStrategy == STRATEGY_EXCLUSIVE) {
        return AUDIO_SUCCESS;
    }

    // 到此，必须是混音策略，其他策略是非法的
    if (strategy->currentStrategy != STRATEGY_MIX) {
        return AUDIO_ERROR;
    }
    return UpdateActiveStreamMixWeight(strategy, CREATE, curStreamhandle);
}

static int32_t UpdateTrackMixWeightWhenDestroy(const AudioStreamStrategy *strategy, uintptr_t curStreamhandle)
{
    // 只有混音情况下，才需要对老流的权重进行更新
    if (strategy->currentStrategy == STRATEGY_NONE) {
        return AUDIO_SUCCESS;
    }

    return UpdateActiveStreamMixWeight(strategy, DESTROY, curStreamhandle);
}

static bool IsCurrentStreamMute(AudioStreamType streamType)
{
    // 当前该类型状态已经是设置状态，无需往下走
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    return GetDownlinkStreamMute((int32_t)typeHashId);
}

#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
static bool IsAllStreamDestroyed(AudioStreamType streamType)
{
    if (streamType != AUDIO_STREAM_VOICE_CALL_BT_SCO &&
        streamType != AUDIO_STREAM_VOICE_CALL_VOIP) {
        return true;
    }
    for (int32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.simpleInfos[i];
        if (curStreamInfo.streamType != streamType) {
            continue;
        }
        if (curStreamInfo.active) {
            return false;
        }
    }
    for (int32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (curStreamInfo.streamType != streamType) {
            continue;
        }
        if (curStreamInfo.active) {
            return false;
        }
    }
    return true;
}
#endif

uintptr_t StreamMgrCreateStreamOut(const AudioStreamInfo *inf)
{
    CHK_NULL_RETURN(inf, (uintptr_t)NULL, "input arg inf is NULL");
    CHK_FAILED_RETURN(IsValidType(inf->streamType), true, (uintptr_t)NULL, "invalid stream type");
    MediaMutexLock(g_mutex);
    // 1. streamId 成功后才会更新maxId
    int32_t curMaxId = g_streamMgrContext.maxOutStreamId;
    int32_t newStreamId = ++curMaxId;
    // 2. 获取到策略，包括默认输出设备，混音比重等
    AudioStreamStrategy *strategy = GetStrategyFromPolicyManager(inf, (uint32_t)newStreamId, STREAM_OUT_CREATE);
    if (strategy == NULL || strategy->currentStrategy == STRATEGY_NONE) {
        ALOGE("get strategy fail or invalid strategy when create out stream");
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 3. 从策略中获取默认 device
    uint32_t defaultRouteDevice = 0;
    if (!AnalyzeStrategyForDevice(strategy, newStreamId, &defaultRouteDevice, true)) {
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 4. 创建 handle context
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    VolumeInf volInf = {IsCurrentStreamMute(inf->streamType), GetDownlinkStreamVolume((int32_t)typeHashId)};
    uintptr_t handle = CreateTrackHandle(newStreamId, inf, defaultRouteDevice, volInf);
    if (handle == 0) {
        ALOGE("create track fail");
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    float mixWgt = 0;
    // 5. 创建成功后，根据策略对老流状态进行审视，以及老流混音比重的更新
    if (CheckTrackState(strategy, newStreamId, inf->streamType) != AUDIO_SUCCESS) {
        ALOGE("CheckTrackState fail");
        goto ERR;
    }
    if (UpdateTrackMixWeightWhenCreate(strategy, handle) != AUDIO_SUCCESS) {
        ALOGE("UpdateTrackMixWeight fail");
        goto ERR;
    }
    // 6. 获取到新流的混音比重
    if (!GetMixWeightForNewStream(strategy, newStreamId, &mixWgt)) {
        ALOGE("analysize strategy for mix may be wrong");
        goto ERR;
    }
    // 7. 上面都正常处理了，该流计入管理
    // 要根据新策略信息更新整个流记录，而非只是增加新流，新流的介入对老流肯定是有影响的
    SetStreamMixWeight(handle, mixWgt);
    RecordOneStream(inf, newStreamId, handle, true);
    g_streamMgrContext.maxOutStreamId++;
    // 8. 通知外界流变化-流创建
    NotifyListenersStreamCreated(newStreamId, inf->sessionID);
    MediaMutexUnLock(g_mutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_INIT_STREAM, MEDIA_CHR_STREAM_OUT_INIT);
    return handle;
ERR:
    MediaMutexUnLock(g_mutex);
    MediaMemFree((void*)handle);
    return (uintptr_t)NULL;
}

int32_t StreamMgrDestroyStreamOut(uintptr_t handle)
{
    if (!g_inited) {
        return AUDIO_SUCCESS;
    }
    CHK_NULL_RETURN((void*)handle, AUDIO_ERROR, "handle is NULL when destroy");
    int32_t streamId;
    AudioSession sessionId;
    int32_t ret = GetHandleId(handle, &streamId, &sessionId);
    CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "get handle failed");
    MediaMutexLock(g_mutex);
    // 告知策略模块即将清掉的流
    AudioStreamInfo streamInfForDestroy = { 0 };
    streamInfForDestroy.streamType = GetStreamType(handle);
    streamInfForDestroy.sessionID = sessionId;
    AudioStreamStrategy *strategy =
        GetStrategyFromPolicyManager(&streamInfForDestroy, (uint32_t)streamId, STREAM_OUT_DESTROY);
    if (strategy == NULL) {
        ALOGE("get strategy fail when destroy out stream");
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    AudioStreamType streamType = GetStreamType(handle);
    AudioDeviceType deviceType = (AudioDeviceType)GetStreamOutDeviceId(handle);
#endif
    // hal 销毁
    if (DestroyTrackHandle(handle, true) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    // 更新的话，先把流从管理中删除
    // 然后根据新的策略，把老流的信息该更新了
    // 通知外界流销毁了
    DeleteOneStream(streamId, true);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    if (IsAllStreamDestroyed(streamType)) {
        SetHifiClockFreqByStreamTypeAndDeviceType(streamType, deviceType, false);
    }
#endif
    if (UpdateTrackMixWeightWhenDestroy(strategy, handle) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        ALOGI("UpdateTrackMixWeightWhenDestroy sessionId:%u failed", sessionId);
        return AUDIO_ERROR;
    }
    NotifyListenersStreamDestroy(streamId, sessionId);
    MediaMutexUnLock(g_mutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_STREAM_OUT);
    ALOGI("StreamMgrDestroyStreamOut sessionId:%u SUCCESS", sessionId);
    return AUDIO_SUCCESS;
}

uintptr_t StreamMgrCreateStreamIn(const AudioStreamInfo *inf)
{
    CHK_NULL_RETURN(inf, (uintptr_t)NULL, "input arg inf is NULL");
    MediaMutexLock(g_mutex);
    // 0. streamId 成功后才会更新maxId
    int32_t curMaxId = g_streamMgrContext.maxInputStreamId;
    int32_t newStreamId = ++curMaxId;
    MediaMutexUnLock(g_mutex);
    // 1.先通过PM，获取策略和路由
    // a.策略决定要不要继续
    AudioStreamStrategy *strategy = GetStrategyFromPolicyManager(inf, (uint32_t)newStreamId, STREAM_IN_CREATE);
    if (strategy == NULL ||
        strategy->currentStrategy == STRATEGY_REJECT ||
        strategy->currentStrategy == STRATEGY_NONE) {
        ALOGE("this new input stream is invalid, goodbye newer");
        return (uintptr_t)NULL;
    }
    // b.获取路由设备，可能是MIC，也可能是SCO-IN
    // todo 如果出现失败，要不要告知下PM?
    MediaMutexLock(g_mutex);
    uint32_t defaultRouteDevice = 0;
    if (!AnalyzeStrategyForDevice(strategy, newStreamId, &defaultRouteDevice, false)) {
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    // 2. 创建 handle context
    StreamTypeHashId typeHashId = FindStreamTypeHashId(inf->streamType);
    if (typeHashId >= STREAM_TYPE_NUM) {
        ALOGE("streamType[%d] sessionId[%d] get invalid TyepHashId[%d]!", inf->streamType, inf->sessionID, typeHashId);
        MediaMutexUnLock(g_mutex);
        return (uintptr_t)NULL;
    }
    VolumeInf volInf = { g_streamMgrContext.micMute, 0.0 };
    if (inf->linkDir == AUDIO_DOWN_LINK) {
        volInf.streamMute = false;
        volInf.typeVolume = GetDownlinkStreamVolume((int32_t)typeHashId);
    }
    uintptr_t handle = CreateSourceHandle(newStreamId, inf, defaultRouteDevice, volInf);
    if (handle != 0) {
        // 3. 上面都成功后，记录并增加maxId
        RecordOneStream(inf, newStreamId, handle, false);
        g_streamMgrContext.maxInputStreamId++;
    }
    MediaMutexUnLock(g_mutex);
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_INIT_STREAM, MEDIA_CHR_INIT_STREAM_IN);
    return handle;
}

int32_t StreamMgrDestroyStreamIn(uintptr_t handle)
{
    MediaMutexLock(g_mutex);
    int32_t streamId = GetSourceStreamId(handle);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    AudioStreamType streamType = GetSourceStreamType(handle);
    AudioDeviceType deviceType = (AudioDeviceType)GetSourceDeviceId(handle);
#endif
    if (DestroySourceHandle(handle, true) != AUDIO_SUCCESS) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_ERROR;
    }
    DeleteOneStream(streamId, false);
#ifdef SUPPORT_SET_DSP_CLOCK_FREQ
    if (IsAllStreamDestroyed(streamType)) {
        SetHifiClockFreqByStreamTypeAndDeviceType(streamType, deviceType, false);
    }
#endif
    MEDIA_CHR_LOG(MT_MEDIA_CHR_AUDIO_MANAGER, MEDIA_CHR_STREAM_MANAGER,
        MEDIA_CHR_RELEASE_STREAM, MEDIA_CHR_RELEASE_STREAM_IN);
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static bool IsValidStream(int32_t streamId, bool active)
{
    return streamId != -1 && active;
}

inline static bool IsRingModeControlStream(AudioStreamType streamType)
{
    return streamType == AUDIO_STREAM_RING || streamType == AUDIO_STREAM_ALARM_CLOCK ||
        streamType == AUDIO_STREAM_NOTIFICATION_SYSTEM;
}

static int32_t StreamMgrSetTypeVolumeInner(int32_t streamType, float typeVolume,
    StreamSimpleInfo simpleInfos[], uint32_t infoSize)
{
    bool findType = false;
    for (uint32_t i = 0; i < infoSize; i++) {
        StreamSimpleInfo *curInfo = &(simpleInfos[i]);
        if (curInfo->streamType != (AudioStreamType)streamType) {
            continue;
        }
        if (!IsValidStream(curInfo->streamId, curInfo->active)) {
            continue;
        }
        findType = true;
        uintptr_t streamHandle = curInfo->streamHandle;
        if (streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
            int32_t ret = SetSourceVolume(streamHandle, typeVolume);
            if ((uint32_t)ret == AUDIO_ERROR) {
                return AUDIO_ERROR;
            }
        } else {
            // 获取混音比例，这个是不是也应该放在 trackcontext 更合适，目前先这么实现
            float mixWeight;
            if (!GetStreamMixWeight(streamHandle, &mixWeight)) {
                return AUDIO_ERROR;
            }
            int32_t ret = SetTrackVolume(streamHandle, typeVolume, mixWeight, NOT_SET_STREAM_VOLUME);
            if ((uint32_t)ret == AUDIO_ERROR) {
                return AUDIO_ERROR;
            }
        }
    }
    if (!findType) {
        ALOGI("there is no [%X]-type stream in stream infos", streamType);
    }
    return AUDIO_SUCCESS;
}

int32_t StreamMgrSetTypeVolume(int32_t streamType, float typeVolume)
{
    if (!IsValidType(streamType) || !IsValidVolumeValue(typeVolume)) {
        ALOGE("set type volume invalid param");
        return AUDIO_ERROR;
    }
    MediaMutexLock(g_mutex);
    // 找到对应类型的流，分别获取流音量，计算后设置下去
    // 如果当前没有该类型的流，不用报错
    if (streamType == AUDIO_STREAM_VOICE_CALL_VOLTE) {
        StreamMgrSetTypeVolumeInner(streamType, typeVolume,
            g_streamMgrContext.curInputStreamInfo, MAX_STREAM_IN_NUM);
    } else {
        StreamMgrSetTypeVolumeInner(streamType, typeVolume,
            g_streamMgrContext.simpleInfos, CONCURRENT_ACTIVE_STREAM_MAX_NUM);
    }
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    SetDownlinkStreamVolume((int32_t)typeHashId, typeVolume);
    MediaMutexUnLock(g_mutex);
#ifndef MELODY_DEMO_EARPHONE
    int32_t ret = WriteValueByKey(AUDIO_VOLUME_FOR_STORAGE);
    if (ret != AUDIO_SUCCESS) {
        ALOGW("set value fail[%d] type[%d] value[%f]", ret, streamType, typeVolume);
    }
#endif
    return AUDIO_SUCCESS;
}

int32_t StreamMgrSetMixWeight(int32_t streamType, float mixWeight)
{
    if (!IsValidType(streamType) || !IsValidVolumeValue(mixWeight)) {
        ALOGE("set type volume invalid param");
        return AUDIO_ERROR;
    }
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    SetMixWeight((int32_t)typeHashId, mixWeight);
    int32_t ret = WriteValueByKey(AUDIO_MIXWEIGHT_FOR_STORAGE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set value fail[%d] type[%d] value[%f]", ret, streamType, mixWeight);
    }
    return ret;
}

static int32_t StreamMgrGetVolumeAndFlag(const char *keyValue, int32_t *typeVolume, int32_t *mixVolumeTriggered)
{
    char key[MAX_KEY_STR_SIZE] = { 0 };
    uint32_t audioModule = 0;
    uint32_t deviceId = 0;
    uint32_t audioPort = 0;
    uint32_t audioChannel = 0;
    uint32_t audioVolType = 0;
    int32_t ret = sscanf_s(keyValue, "%s = %d#%u#%u#%u#%u#%u#%d", key, MAX_KEY_STR_SIZE, typeVolume, &audioModule,
        &audioChannel, &audioVolType, &audioPort, &deviceId, mixVolumeTriggered);
    if (ret != SSCANF_RET_NUM) {
        ALOGE("sscanf_s key value failed, ret = %d", ret);
        return ret;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamMgrRecordDbVolume(int32_t streamType, const char *keyValue)
{
    if (!IsValidType(streamType)) {
        ALOGE("set type volume invalid param");
        return AUDIO_ERROR;
    }
    MediaMutexLock(g_mutex);
    int32_t mixVolumeTriggered = 0;
    int32_t typeVolume = 0;
    int32_t ret = StreamMgrGetVolumeAndFlag(keyValue, &typeVolume, &mixVolumeTriggered);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("StreamMgrGetVolumeAndFlag fail[%d] type[%d] value[%d]", ret, streamType, typeVolume);
        MediaMutexUnLock(g_mutex);
        return ret;
    }
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    if (mixVolumeTriggered != 0 && g_streamMgrContext.tempVolumeChangeCount[typeHashId] > 0) {
        g_streamMgrContext.tempVolumeChangeCount[typeHashId]--;
        ALOGI("end streamType:%x,tempVolumeChangeCount:%d", streamType, g_streamMgrContext.tempVolumeChangeCount[typeHashId]);
        MediaMutexUnLock(g_mutex);
        return AUDIO_SUCCESS;
    }
    double tmpVolume = ReverseLogCalculation(typeVolume / MAGNIFY_TIMES);
    SetDownlinkStreamVolume((int32_t)typeHashId, tmpVolume);
    MediaMutexUnLock(g_mutex);
    ret = WriteValueByKey(AUDIO_VOLUME_FOR_STORAGE);
    if (ret != AUDIO_SUCCESS) {
        ALOGW("set value fail[%d] type[%d] value[%d]", ret, streamType, typeVolume);
    }
    return AUDIO_SUCCESS;
}

static StreamTypeHashId FindStreamTypeHashId(int32_t streamType)
{
    AudioStreamType type = (AudioStreamType)streamType;
    for (int i = 0; i < STREAM_TYPE_NUM; i++) {
        if (type == typeVolumes[i].streamType) {
            return typeVolumes[i].typeId;
        }
    }
    return AUDIO_STREAM_INVALID_TYPE_HASH_ID;
}

bool StreamMgrGetTypeVolume(int32_t streamType, float *typeVolume)
{
    // 校验下参数
    CHK_NULL_RETURN(typeVolume, false, "type volume ptr is NULL");
    if (!IsValidType(streamType)) {
        ALOGE("invalid stream type[%d]", streamType);
        return false;
    }
    // hash 方式来处理
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    *typeVolume = GetDownlinkStreamVolume((int32_t)typeHashId);
    return true;
}

int32_t StreamMgrSetMute(AudioStreamType streamType, bool mute)
{
    // 校验下参数
    if (!IsValidType(streamType)) {
        ALOGE("set mute invalid stream type[%d]", streamType);
        return AUDIO_ERROR;
    }

    MediaMutexLock(g_mutex);
    // 当前该类型状态已经是设置状态，无需往下走
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    if (mute == GetDownlinkStreamMute((int32_t)typeHashId)) {
        MediaMutexUnLock(g_mutex);
        return AUDIO_SUCCESS;
    }

    // 找到对应类型的流，分别获取流音量，计算后设置下去
    bool findType = false;
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        if (g_streamMgrContext.simpleInfos[i].streamType != (AudioStreamType)streamType) {
            continue;
        }
        findType = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        if ((uint32_t)SetTrackMute(streamHandle, mute) == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            ALOGE("streamType:%x set mute:%d SetTrackMute failed!", streamType, mute);
            return AUDIO_ERROR;
        }
    }
    if (!findType) {
        ALOGI("there is no [%d]-type stream in stream infos", streamType);
    }
    // 成功就设置下来
    SetDownlinkStreamMute((int32_t)typeHashId, mute);
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

bool StreamMgrIsMute(AudioStreamType streamType)
{
    // 校验下参数
    if (!IsValidType(streamType)) {
        ALOGE("invalid stream type[%d]", streamType);
        return false;
    }

    // hash 方式来处理
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    bool isTypeMute = GetDownlinkStreamMute((int32_t)typeHashId);
    return isTypeMute;
}

#ifdef SUPPORT_DOWN_LINK_GLOBAL_MUTE
int32_t StreamMgrSetGlobalMute(bool isMute)
{
    char buf[BUFFER_SIZE] = {0};
    int32_t ret = sprintf_s(buf, BUFFER_SIZE, "%d", isMute);
    if (ret == -1) {
        ALOGE("set globe mute sprintf_s failed, ret = %d", ret);
        return AUDIO_INVALID_OPERATION;
    }
    uint32_t len = BUFFER_SIZE;
    return DeviceMgrInvoke(AUDIO_INVOKE_SET_GLOBAL_MUTE, buf, &len);
}
#endif

bool StreamMgrSetMasterMute(bool isMute)
{
    MediaMutexLock(g_mutex);
    // 不做阻拦，考虑场景：1.全局mute 2.A类型解mute 3.全局在mute
    // 找到有效的流
    ALOGI("set global mute:%d!", isMute);
#ifdef SUPPORT_DOWN_LINK_GLOBAL_MUTE
    if (StreamMgrSetGlobalMute(isMute) != AUDIO_SUCCESS) {
        ALOGE("set global mute:%d failed!", isMute);
        MediaMutexUnLock(g_mutex);
        return false;
    }
#else
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        // 要先判断有效
        if (!g_streamMgrContext.simpleInfos[i].active || g_streamMgrContext.simpleInfos[i].streamId == -1) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackMute(streamHandle, isMute);
        if ((uint32_t)ret == AUDIO_ERROR) {
            ALOGE("streamType:%x set mute:%d SetTrackMute failed!", g_streamMgrContext.simpleInfos[i].streamType, isMute);
            MediaMutexUnLock(g_mutex);
            return false;
        }
    }
    // 成功才会设置下来，全type都要禁
    for (int i = 0; i < STREAM_TYPE_NUM; ++i) {
        SetDownlinkStreamMute(i, isMute);
    }
#endif
#ifdef SUPPORT_UP_LINK_GLOBAL_MUTE
    for (size_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        // 要先判断有效
        if (!g_streamMgrContext.curInputStreamInfo[i].active ||
            g_streamMgrContext.curInputStreamInfo[i].streamId == -1) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        int32_t ret = SetSourceMute(streamHandle, isMute);
        if ((uint32_t)ret == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            return false;
        }
    }
    g_streamMgrContext.micMute = isMute;
#endif
    g_streamMgrContext.masterMute = isMute;
    MediaMutexUnLock(g_mutex);
    return true;
}

bool StreamMgrIsMasterMute(void)
{
    MediaMutexLock(g_mutex);
    bool isMasterMute = g_streamMgrContext.masterMute;
    MediaMutexUnLock(g_mutex);
    return isMasterMute;
}

int32_t StreamMgrDumpInfo(AudioStreamDebugInfo *audioStreamInfo)
{
    CHK_NULL_RETURN(audioStreamInfo, AUDIO_ERROR, "audioStreamInfo is NULL");
    MediaMutexLock(g_mutex);
    uint32_t index = 0;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        audioStreamInfo->allstreamInfo[index] = DumpTrackInfo(streamHandle);
        index++;
        if (index >= MAX_INST_NUM) {
            break;
        }
    }

    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (index >= MAX_INST_NUM) {
            break;
        }
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        audioStreamInfo->allstreamInfo[index] = DumpSourceInfo(streamHandle);
        index++;
    }
    audioStreamInfo->streamInfoCnt = index;
    MediaMutexUnLock(g_mutex);
    return AUDIO_SUCCESS;
}

inline static bool IsValidRingMode(AudioRingMode ringMode)
{
    return ringMode >= RINGER_MODE_NORMAL && ringMode <= RINGER_MODE_SILENT;
}

static bool SetRingTypeMute(bool setMute)
{
    const AudioStreamType ringControlTypes[] = { AUDIO_STREAM_RING,
                                                 AUDIO_STREAM_ALARM_CLOCK,
                                                 AUDIO_STREAM_NOTIFICATION_SYSTEM };
    size_t size = sizeof(ringControlTypes) / sizeof(ringControlTypes[0]);
    for (size_t i = 0; i < size; i++) {
        StreamTypeHashId typeHashId = FindStreamTypeHashId(ringControlTypes[i]);
        if (typeHashId == AUDIO_STREAM_TYPE_HASH_ID_MAX) {
            return false;
        }
        SetDownlinkStreamMute((int32_t)typeHashId, setMute);
    }
    return true;
}

bool StreamMgrSetRingerMode(int mode)
{
    AudioRingMode ringMode = (AudioRingMode)mode;
    if (!IsValidRingMode(ringMode)) {
        return false;
    }

    bool setMute = false;
    if (mode == RINGER_MODE_SILENT || mode == RINGER_MODE_VIBRATE) {
        setMute = true;
    }
    if (mode == RINGER_MODE_NORMAL) {
        setMute = false;
    }
    MediaMutexLock(g_mutex);
    for (int i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId, g_streamMgrContext.simpleInfos[i].active) ||
            !IsRingModeControlStream(g_streamMgrContext.simpleInfos[i].streamType)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackMute(streamHandle, setMute);
        if ((uint32_t)ret == AUDIO_ERROR) {
            MediaMutexUnLock(g_mutex);
            return false;
        }
    }
    // 成功后设置
    g_streamMgrContext.ringMode = ringMode;
    bool ret = SetRingTypeMute(setMute);
    MediaMutexUnLock(g_mutex);
    return ret;
}

int32_t StreamMgrGetRingerMode(void)
{
    MediaMutexLock(g_mutex);
    AudioRingMode ringMode = g_streamMgrContext.ringMode;
    MediaMutexUnLock(g_mutex);
    return ringMode;
}

bool StreamMgrSetMicrophoneMute(bool isMute)
{
    MediaMutexLock(g_mutex);
    if (isMute == g_streamMgrContext.micMute) {
        MediaMutexUnLock(g_mutex);
        return true;
    }

    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        if (g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_RECORD ||
            ((g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO ||
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_VOLTE ||
#ifdef HI3322_CHIP_V100
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_LIVE_MIC ||
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI) &&
#else
            g_streamMgrContext.curInputStreamInfo[i].streamType == AUDIO_STREAM_LIVE_MIC) &&
#endif
            g_streamMgrContext.curInputStreamInfo[i].linkDir == AUDIO_UP_LINK)) {
            uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
            int32_t ret = SetSourceMute(streamHandle, isMute);
            if (ret != 0) {
                ALOGE("SetSourceMute failed ret:%d", ret);
                MediaMutexUnLock(g_mutex);
                return false;
            }
            break;
        }
    }
    g_streamMgrContext.micMute = isMute;
    MediaMutexUnLock(g_mutex);
    return true;
}

bool StreamMgrIsMicrophoneMute(void)
{
    MediaMutexLock(g_mutex);
    bool isMute = g_streamMgrContext.micMute;
    MediaMutexUnLock(g_mutex);
    return isMute;
}

static int32_t StreamManagerDeactivateCorrespondingInputStream(AudioSession sessionID)
{
    uint32_t i;
    for (i = 0; i < MAX_STREAM_IN_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.curInputStreamInfo[i].streamId,
                           g_streamMgrContext.curInputStreamInfo[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.curInputStreamInfo[i].streamHandle;
        AudioSession sessionIdTemp = AUDIO_SESSION_ID_NONE;
        int32_t ret = GetSourceSessionId(g_streamMgrContext.curInputStreamInfo[i].streamHandle, &sessionIdTemp);
        if (ret != AUDIO_SUCCESS) {
            continue;
        }
        if (sessionIdTemp == sessionID) {
            if (DeactivateCorrespondingSource(streamHandle) != AUDIO_SUCCESS) {
                return AUDIO_ERROR;
            }
            break;
        }
    }
    if (i == MAX_STREAM_IN_NUM) {
        ALOGI("not find match in stream for session[%u]", sessionID);
        return AUDIO_ERROR;
    }

    return AUDIO_SUCCESS;
}

static int32_t StreamManagerDeactivateCorrespondingOutputStream(AudioSession sessionID)
{
    uint32_t i;
    for (i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
                           g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t streamIdTemp = 0;
        AudioSession sessionIdTemp = AUDIO_SESSION_ID_NONE;
        int32_t ret = GetHandleId(streamHandle, &streamIdTemp, &sessionIdTemp);
        CHK_FAILED_RETURN(ret, AUDIO_SUCCESS, AUDIO_ERROR, "get handle failed");
        if (sessionID == sessionIdTemp) {
            if (DeactivateCorrespondingTrack(streamHandle) != AUDIO_SUCCESS) {
                ALOGI("no corresponding track[sessionID(%u)]", sessionID);
                return AUDIO_ERROR;
            }
            break;
        }
    }
    if (i == CONCURRENT_ACTIVE_STREAM_MAX_NUM) {
        ALOGI("not find match out stream for session[%u]", sessionID);
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamManagerDeactivateCorrespondingStream(AudioSession sessionID, AudioStreamType streamType)
{
    MediaMutexLock(g_mutex);
    int32_t ret;
    if (streamType == AUDIO_STREAM_VOICE_RECORD) {
        ret = StreamManagerDeactivateCorrespondingInputStream(sessionID);
    } else if (streamType == AUDIO_STREAM_VOICE_CALL_BT_SCO ||
        streamType == AUDIO_STREAM_VOICE_CALL_VOLTE ||
#ifdef HI3322_CHIP_V100
        streamType == AUDIO_STREAM_VOICE_CALL_VOLTE_SPI ||
#endif
        streamType == AUDIO_STREAM_A2DP_MUSIC) {
        ret = StreamManagerDeactivateCorrespondingInputStream(sessionID);
        ret = StreamManagerDeactivateCorrespondingOutputStream(sessionID);
    } else if (streamType == AUDIO_STREAM_ALARM_SYSTEM || streamType == AUDIO_STREAM_ALARM_CLOCK ||
               streamType == AUDIO_STREAM_RING || streamType == AUDIO_STREAM_NOTIFICATION_PROMPT ||
               streamType == AUDIO_STREAM_MUSIC) {
        ret = StreamManagerDeactivateCorrespondingOutputStream(sessionID);
    } else {
        ALOGW("should not be here");
        ret = AUDIO_ERROR;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

static int32_t GetStreamOutParamValue(char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
            g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uint32_t deviceId = GetStreamOutDeviceId(g_streamMgrContext.simpleInfos[i].streamHandle);
        if (deviceId == OUT_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = GetTrackParam(streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get track param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGI("not find match out stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t SetStreamOutParamValue(const char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
        if (!IsValidStream(g_streamMgrContext.simpleInfos[i].streamId,
            g_streamMgrContext.simpleInfos[i].active)) {
            continue;
        }
        uint32_t deviceId = GetStreamOutDeviceId(g_streamMgrContext.simpleInfos[i].streamHandle);
        if (deviceId == OUT_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        uintptr_t streamHandle = g_streamMgrContext.simpleInfos[i].streamHandle;
        int32_t ret = SetTrackParam(streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set track param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match out stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t GetStreamInParamValue(char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (!IsValidStream(curStreamInfo.streamId, curStreamInfo.active)) {
            continue;
        }
        uint32_t deviceId = GetSourceDeviceId(curStreamInfo.streamHandle);
        if (deviceId == IN_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        int32_t ret = GetSourceParam(curStreamInfo.streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("get source param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match in stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

static int32_t SetStreamInParamValue(const char *param, uint32_t len)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (!IsValidStream(curStreamInfo.streamId, curStreamInfo.active)) {
            continue;
        }
        uint32_t deviceId = GetSourceDeviceId(curStreamInfo.streamHandle);
        if (deviceId == IN_BLUETOOTH_SCO) {
            continue;
        }
        isFound = true;
        int32_t ret = SetSourceParam(curStreamInfo.streamHandle, param, len);
        if (ret != AUDIO_SUCCESS) {
            ALOGE("set source param failed, ret:%d.", ret);
            return AUDIO_ERROR;
        }
    }
    if (!isFound) {
        ALOGE("not find match in stream!");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t StreamManagerSetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    int32_t ret = AUDIO_SUCCESS;
    MediaMutexLock(g_mutex);
    switch (direction) {
        case AUDIO_DOWN_LINK:
            ret = SetStreamOutParamValue(param, len);
            break;
        case AUDIO_UP_LINK:
            ret = SetStreamInParamValue(param, len);
            break;
        default:
            ALOGI("not find match direction!");
            ret = AUDIO_ERROR;
            break;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

int32_t StreamManagerGetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    int32_t ret = AUDIO_SUCCESS;
    MediaMutexLock(g_mutex);
    switch (direction) {
        case AUDIO_DOWN_LINK:
            ret = GetStreamOutParamValue(param, len);
            break;
        case AUDIO_UP_LINK:
            ret = GetStreamInParamValue(param, len);
            break;
        default:
            ALOGE("not find match direction!");
            ret = AUDIO_ERROR;
            break;
    }
    MediaMutexUnLock(g_mutex);
    return ret;
}

bool StreamMgrIsCreateStreamByType(AudioStreamType streamType)
{
    bool isFound = false;
    for (uint32_t i = 0; i < MAX_STREAM_IN_NUM; i++) {
        StreamSimpleInfo curStreamInfo = g_streamMgrContext.curInputStreamInfo[i];
        if (curStreamInfo.streamType == streamType &&
            curStreamInfo.streamId != -1) {
            isFound = true;
            break;
        }
    }
    if (!isFound) {
        for (uint32_t i = 0; i < CONCURRENT_ACTIVE_STREAM_MAX_NUM; i++) {
            StreamSimpleInfo curStreamInfo = g_streamMgrContext.simpleInfos[i];
            if (curStreamInfo.streamType == streamType &&
                curStreamInfo.streamId != -1) {
                isFound = true;
                break;
            }
        }
    }
    return isFound;
}

bool StreamMgrGetMixWeight(int32_t streamType, float *mixWeight)
{
    // 校验下参数
    CHK_NULL_RETURN(mixWeight, false, "type volume ptr is NULL");
    if (!IsValidType(streamType)) {
        ALOGE("invalid stream type[%d]", streamType);
        return false;
    }
    // hash 方式来处理
    StreamTypeHashId typeHashId = FindStreamTypeHashId(streamType);
    *mixWeight = GetMixWeight((int32_t)typeHashId);
    return true;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

