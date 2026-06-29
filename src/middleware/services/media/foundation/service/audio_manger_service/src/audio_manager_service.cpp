/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2026. All rights reserved.
 * Description: audio manager c service
 * Author: Media Software Group
 * Create: 2023-06-07
 */

#include "audio_manager_service.h"

#include <iostream>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <string>
#include "securec.h"
#include "audio_utils.h"
#include "audio_errors.h"
#include "audio_manager.h"
#include "audio_stream_in.h"
#include "audio_stream_out.h"
#include "media_mem.h"
#ifdef ENABLE_A2DP_SINK_MUSIC
#include "a2dp_sink_service.h"
#endif

using Audio::AudioManager;
using Audio::AudioStreamIn;
using Audio::AudioStreamOut;
using Audio::InterruptListener;
using Audio::AudioInterrupt;

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MODULE_NAME "AudioManagerCService"
#define BUFFER_SIZE            120
#define VQE_PREFIX_LEN         24
#define PEQ_PREFIX_LEN         50
#define NV_AUDIO_PEQ_PARAM_SIZE 41
#define NV_AUDIO_VQE_PARAM_SIZE 60
#define PEQ_PARAM_SIZE         (NV_AUDIO_PEQ_PARAM_SIZE * sizeof(uint32_t))
#define VQE_PARAM_SIZE         (NV_AUDIO_VQE_PARAM_SIZE * sizeof(uint32_t))
#define SEA_BUFFER_SIZE        (VQE_PARAM_SIZE + VQE_PREFIX_LEN)
#define AEF_BUFFER_LEN         800

static AudioManager& g_amInstance = AudioManager::GetInstance();
static std::shared_ptr<AudioStreamIn> g_streamIn = nullptr;
static std::shared_ptr<AudioStreamOut> g_streamOut = nullptr;
static AudioInterrupt g_interrupt;

class AudioManagerInterruptListener : public InterruptListener {
public:
    AudioManagerInterruptListener(){};
    ~AudioManagerInterruptListener()override {};

    void OnInterrupt(int32_t type, int32_t hint) override
    {
        ALOGI("type:%d hint:%d", type, hint);
    }
};

bool AudioManagerInit(void)
{
    bool success = g_amInstance.Initialize();
    if (!success) {
        ALOGE("audio manager init failed");
        return false;
    }
#ifdef ENABLE_A2DP_SINK_MUSIC
    AudioManagerA2dpSinkRegistCallback();
#endif
    ALOGD("success");
    return true;
}

AudioSession AudioManagerMakeSessionId(void)
{
    return g_amInstance.MakeSessionId();
}

int32_t AudioManagerSetDeviceConnectionState(AudioDeviceType deviceType, AudioDeviceConnectState connectState)
{
    int32_t ret = g_amInstance.SetDeviceConnectionState(deviceType, connectState);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set device 0x%x connect %d failed 0x%x", deviceType, connectState, ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerActivateInterrupt(AudioStreamType streamType, AudioSession sessionId)
{
    std::shared_ptr<AudioManagerInterruptListener> listener = std::make_shared<AudioManagerInterruptListener>();
    CHK_NULL_RETURN(listener.get(), AUDIO_ERROR, "listener is NUll!");
    g_interrupt = { streamType, sessionId, listener };
    if (g_amInstance.ActivateAudioInterrupt(g_interrupt) == INTERRUPT_FAILED) {
        ALOGE("activate audio interrupt faild");
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerDeactivateInterrupt(void)
{
    if (g_amInstance.DeactivateAudioInterrupt(g_interrupt) != AUDIO_SUCCESS) {
        ALOGE("deactivate audio interrupt faild");
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInInit(CapturerInputConfig captureConfig)
{
    if (g_streamIn.get() != nullptr) {
        ALOGE("audio stream in handle has inited!");
        return AUDIO_ERROR;
    }
    g_streamIn = std::make_shared<AudioStreamIn>();
    CHK_NULL_RETURN(g_streamIn.get(), AUDIO_ERROR, "audio stream in handle is NUll!");
    int32_t ret = g_streamIn->Init(captureConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in init failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInStart(void)
{
    CHK_NULL_RETURN(g_streamIn.get(), AUDIO_ERROR, "audio stream in handle is NUll!");
    int32_t ret = g_streamIn->Start();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in start failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInObtainBuffer(uint8_t *data, uint32_t *size)
{
    CHK_NULL_RETURN(g_streamIn.get(), AUDIO_ERROR, "audio stream in handle is NUll!");
    CapturerBuffer buffer;
    buffer.data = data;
    buffer.size = *size;
    int32_t ret = g_streamIn->ObtainBuffer(buffer, false);
    if (ret < 0) {
        return AUDIO_ERROR;
    }
    *size = (uint32_t)ret;
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInSetBitRate(uint32_t bitrate)
{
    CHK_NULL_RETURN(g_streamIn.get(), AUDIO_ERROR, "audio stream in handle is NUll!");
    std::string str("setbitrate = " + std::to_string(bitrate));
    int32_t ret = g_streamIn->SetParam(str.c_str(), str.size());
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set bitrate failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInStop(void)
{
    CHK_NULL_RETURN(g_streamIn.get(), AUDIO_ERROR, "audio stream in handle is NUll!");
    int32_t ret = g_streamIn->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream in stop failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamInDeinit(void)
{
    if (g_streamIn != nullptr) {
        g_streamIn->Release();
        g_streamIn.reset();
        g_streamIn = nullptr;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutInit(AudioRendererConfig renderConfig)
{
    if (g_streamOut.get() != nullptr) {
        ALOGE("audio stream out handle has inited!");
        return AUDIO_ERROR;
    }
    g_streamOut = std::make_shared<AudioStreamOut>();
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");

    int32_t ret = g_streamOut->Init(renderConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out init failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutSetMute(bool mute)
{
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");
    int32_t ret = g_streamOut->SetMute(mute);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out set mute failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetSeaEnable(bool enable)
{
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");
    std::string str("seaenable = " + std::to_string(enable));
    int32_t ret = g_streamOut->SetParam(str.c_str(), str.size());
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set sea enable failed");
        return AUDIO_ERROR;
    }

    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetAefParam(int32_t effectType, const uint8_t *aefParam, uint16_t len)
{
    CHK_NULL_RETURN(aefParam, AUDIO_ERROR, "aefParam null!");
    char *aefBuf = (char *)MediaMemMalloc(AEF_BUFFER_LEN);
    CHK_NULL_RETURN(aefBuf, AUDIO_ERROR, "aefBuf malloc failed!");
    if (memset_s(aefBuf, AEF_BUFFER_LEN, 0, AEF_BUFFER_LEN) != EOK) {
        ALOGE("memset_s fail");
        MediaMemFree(aefBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_LEN, "%s = %d#%u:", "setaefparam", effectType, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefBuf + prefixLen, AEF_BUFFER_LEN - prefixLen, aefParam, len) != EOK) {
        ALOGE("memcpy_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.SetParam(AUDIO_DOWN_LINK, aefBuf, AEF_BUFFER_LEN);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set param failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    MediaMemFree(aefBuf);
    ALOGI("set param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetAefParam(int32_t effectType, uint8_t *aefParam, uint16_t len)
{
    CHK_NULL_RETURN(aefParam, AUDIO_ERROR, "aefParam null!");
    char *aefBuf = (char *)MediaMemMalloc(AEF_BUFFER_LEN);
    CHK_NULL_RETURN(aefBuf, AUDIO_ERROR, "aefBuf malloc failed!");
    if (memset_s(aefBuf, AEF_BUFFER_LEN, 0, AEF_BUFFER_LEN) != EOK) {
        ALOGE("memset_s fail");
        MediaMemFree(aefBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(aefBuf, AEF_BUFFER_LEN, "%s = %d#%u:", "getaefparam", effectType, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.GetParam(AUDIO_DOWN_LINK, aefBuf, AEF_BUFFER_LEN);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("get param failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(aefParam, AEF_BUFFER_LEN, aefBuf, len) != EOK) {
        ALOGE("memcpy_s failed");
        MediaMemFree(aefBuf);
        return AUDIO_ERROR;
    }
    MediaMemFree(aefBuf);
    ALOGI("get param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetVolume(AudioStreamType streamType, int32_t volume)
{
    if (!g_amInstance.SetVolume(streamType, volume)) {
        ALOGE("set cat1 downlink volume failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetVolume(AudioStreamType streamType)
{
    return g_amInstance.GetVolume(streamType);
}

int32_t AudioStreamOutPlay(void)
{
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");
    int32_t ret = g_streamOut->Play();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out play failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutStreamWrite(const uint8_t *data, uint32_t size)
{
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");
    uint64_t writtenSize = 0;
    AudioRendererConfig renderConfig = {};
    int32_t ret = g_streamOut->StreamWrite(data, size, writtenSize, renderConfig);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out play failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutStop(void)
{
    CHK_NULL_RETURN(g_streamOut.get(), AUDIO_ERROR, "audio stream out handle is NUll!");
    int32_t ret = g_streamOut->Stop();
    if (ret != AUDIO_SUCCESS) {
        ALOGE("audio stream out stop failed 0x%x", ret);
        return AUDIO_ERROR;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioStreamOutDeinit(void)
{
    if (g_streamOut != nullptr) {
        g_streamOut->Release();
        g_streamOut.reset();
        g_streamOut = nullptr;
    }

    ALOGD("success");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetA2dpAttr(AudioA2dpCodecAttr *data, uint32_t dataLen)
{
    if (data == NULL || dataLen != sizeof(AudioA2dpCodecAttr)) {
        return AUDIO_ERROR;
    }
    return g_amInstance.Invoke(AUDIO_INVOKE_SET_A2DP_CODEC_ATTR, (void *)data, &dataLen);
}

int32_t AudioManagerSetSeaParam(int32_t cmd, const uint8_t *seaParam, uint16_t len)
{
    CHK_NULL_RETURN(seaParam, AUDIO_ERROR, "sea_param null!");
    char *seaBuf = (char *)MediaMemMalloc(SEA_BUFFER_SIZE);
    CHK_NULL_RETURN(seaBuf, AUDIO_ERROR, "seaBuf malloc null!");
    if (memset_s(seaBuf, SEA_BUFFER_SIZE, 0, SEA_BUFFER_SIZE) != EOK) {
        ALOGE("memset_s fail");
        MediaMemFree(seaBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(seaBuf, SEA_BUFFER_SIZE, "%s = %d#%u:", "setseaparam", cmd, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(seaBuf + prefixLen, SEA_BUFFER_SIZE - prefixLen, seaParam, len) != EOK) {
        ALOGE("memcpy_s failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.SetParam(AUDIO_UP_LINK, seaBuf, SEA_BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set sea param failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    MediaMemFree(seaBuf);
    ALOGI("set sea param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerGetSeaParam(int32_t cmd, uint8_t *seaParam, uint16_t len)
{
    CHK_NULL_RETURN(seaParam, AUDIO_ERROR, "sea_param null!");
    if (len > SEA_BUFFER_SIZE) {
        ALOGE("invalid param!");
        return AUDIO_ERROR;
    }
    char *seaBuf = (char *)MediaMemMalloc(SEA_BUFFER_SIZE);
    CHK_NULL_RETURN(seaBuf, AUDIO_ERROR, "seaBuf malloc null!");
    if (memset_s(seaBuf, SEA_BUFFER_SIZE, 0, SEA_BUFFER_SIZE) != EOK) {
        ALOGE("memset_s fail");
        MediaMemFree(seaBuf);
        return AUDIO_NO_MEM;
    }
    int32_t prefixLen = sprintf_s(seaBuf, SEA_BUFFER_SIZE, "%s = %d#%u", "getseaparam", cmd, len);
    if (prefixLen < 0) {
        ALOGE("sprintf_s failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    int32_t ret = g_amInstance.GetParam(AUDIO_UP_LINK, seaBuf, SEA_BUFFER_SIZE);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("get sea param failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    if (memcpy_s(seaParam, SEA_BUFFER_SIZE, seaBuf, len) != EOK) {
        ALOGE("memcpy_s failed");
        MediaMemFree(seaBuf);
        return AUDIO_ERROR;
    }
    MediaMemFree(seaBuf);
    ALOGI("get sea param success!");
    return AUDIO_SUCCESS;
}

int32_t AudioManagerSetSeaLibId(int32_t seaId)
{
    std::string str("SetSeaLibId = " + std::to_string(seaId));
    uint32_t paramLen = str.size();
    int32_t ret = g_amInstance.Invoke(AUDIO_INVOKE_SEA_SET_LIB_ID, (void *)str.c_str(), &paramLen);
    if (ret != AUDIO_SUCCESS) {
        ALOGE("set sea id failed");
        return AUDIO_ERROR;
    }
    return AUDIO_SUCCESS;
}

void AudioManagerDumpInfo(void)
{
#ifdef AUDIO_MANAGER_DUMP_INFO
    Audio::AudioManagerDebugInfo managerInfo = {};

    (void)g_amInstance.DumpInfo(managerInfo);
    ALOGE("-----------input Interrupt info-----------");
    for (auto it:managerInfo.inputInterruptInfo) {
        ALOGE("streamType:%d", it.streamType);
        ALOGE("sessionID:%d", it.sessionID);
        ALOGE("currentStrategy:%d", it.currentStrategy);
        ALOGE("streamCreateTime:%lldus", it.streamCreateTime);
        ALOGE("streamDestroyTime:%lldus", it.streamDestroyTime);
    }
    ALOGE("-----------output Interrupt info-----------");
    for (auto it:managerInfo.outputInterruptInfo) {
        ALOGE("streamType:%d", it.streamType);
        ALOGE("sessionID:%d", it.sessionID);
        ALOGE("currentStrategy:%d", it.currentStrategy);
        ALOGE("streamCreateTime:%lldus", it.streamCreateTime);
        ALOGE("streamDestroyTime:%lldus", it.streamDestroyTime);
    }
    ALOGE("-----------all stream info-----------");
    for (auto it:managerInfo.allstreamInfo) {
        ALOGE("streamType:%d", it.streamInfo.streamType);
        ALOGE("streamFormat:%d", it.streamInfo.format);
        ALOGE("state:%u", it.state);
        ALOGE("streamIndex:%d", it.streamIndex);
        ALOGE("streamTypeVolume:%.3f", it.streamVolume);
        ALOGE("mixWeight:%f", it.mixWeight);
        ALOGE("curDevice:%x", it.curDevice);
    }
    ALOGE("-----------all device info-----------");
    for (auto it:managerInfo.allDeviceInfo) {
        ALOGE("deviceType:%x", it.deviceInfo.device);
        ALOGE("deviceName:%s", it.deviceInfo.name);
        ALOGE("flag:%d", it.deviceInfo.flag);
        ALOGE("deviceState:%d", it.deviceState);
    }
#else
    ALOGE("not support");
#endif
}

#ifdef __cplusplus
}
#endif
