/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2024-2024. All rights reserved.
* Description: audio manager impl
* Author: Media Software Group
* Create: 2024-07-10
*/

#include "audio_manager_impl.h"
#include <climits>
#include <list>
#include <sys/time.h>
#ifdef SUPPORT_SYS_PRCTL
#include <sys/prctl.h>
#endif
#include <unistd.h>

#include "audio_utils.h"
#include "audio_errors.h"
#include "securec.h"

#define LOG_MODULE_NAME "AudioManagerImpl"

namespace Audio {
AudioManager::AudioManagerImpl::AudioManagerImpl()
{
}

AudioManager::AudioManagerImpl::~AudioManagerImpl()
{
}

AudioSession AudioManager::AudioManagerImpl::MakeSessionId()
{
    return 0;
}

bool AudioManager::AudioManagerImpl::SetVolume(AudioStreamType streamType, int32_t volume)
{
    return false;
}

int32_t AudioManager::AudioManagerImpl::GetVolume(AudioStreamType streamType)
{
    return 0;
}

int32_t AudioManager::AudioManagerImpl::GetMinVolume(AudioStreamType streamType)
{
    return 0;
}

int32_t AudioManager::AudioManagerImpl::GetMaxVolume(AudioStreamType streamType)
{
    return 0;
}

bool AudioManager::AudioManagerImpl::Mute(AudioStreamType streamType)
{
    return false;
}

bool AudioManager::AudioManagerImpl::UnMute(AudioStreamType streamType)
{
    return false;
}

bool AudioManager::AudioManagerImpl::IsMute(AudioStreamType streamType)
{
    return false;
}

bool AudioManager::AudioManagerImpl::SetMasterMute(bool isMute)
{
    return false;
}

bool AudioManager::AudioManagerImpl::IsMasterMute()
{
    return false;
}

bool AudioManager::AudioManagerImpl::SetMicrophoneMute(bool isMute)
{
    return false;
}

bool AudioManager::AudioManagerImpl::IsMicrophoneMute()
{
    return false;
}

bool AudioManager::AudioManagerImpl::SetRingerMode(AudioRingMode mode)
{
    return false;
}

int32_t AudioManager::AudioManagerImpl::GetRingerMode()
{
    return 0;
}

std::vector<AudioDeviceInfo> AudioManager::AudioManagerImpl::GetDevices(DeviceFlag flag)
{
    uint32_t deviceCount = 0;
    std::vector<AudioDeviceInfo> devices;
    AudioDeviceInfo *deviceInfo = new AudioDeviceInfo[deviceCount];
    if (deviceInfo == nullptr) {
        return devices;
    }
    for (uint32_t i = 0; i < deviceCount; i++) {
        devices.push_back(deviceInfo[i]);
    }
    delete []deviceInfo;
    return devices;
}

bool AudioManager::AudioManagerImpl::IsSupportOffload(AudioOffloadInfo &info)
{
    return false;
}

int32_t AudioManager::AudioManagerImpl::SetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    return 0;
}

void AudioManager::AudioManagerImpl::InterruptListenerImpl(uintptr_t owner, uintptr_t userData,
                                                           int32_t type, int32_t hint)
{
}

int32_t AudioManager::AudioManagerImpl::ConvertIAudioInterrupt(AudioInterrupt &interrupt,
                                                               AudioInterruptProxy &interruptInternal)
{
    if (interrupt.interruptListener.get() == nullptr) {
        return AUDIO_INVALID_PARAMS;
    }
    interruptInternal.interruptListenerCb = InterruptListenerImpl;
    interruptInternal.userData = reinterpret_cast<uintptr_t>(interrupt.interruptListener.get());
    interruptInternal.streamType = interrupt.streamType;
    interruptInternal.sessionID = interrupt.sessionID;

    return AUDIO_SUCCESS;
}

int32_t AudioManager::AudioManagerImpl::ActivateAudioInterrupt(AudioInterrupt &interrupt)
{
    return 0;
}

int32_t AudioManager::AudioManagerImpl::DeactivateAudioInterrupt(AudioInterrupt &interrupt)
{
    return 0;
}

int32_t AudioManager::AudioManagerImpl::DumpInfo(AudioManagerDebugInfo &audioManagerInfo)
{
    return AUDIO_SUCCESS;
}

int32_t AudioManager::AudioManagerImpl::SetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    return 0;
}

int32_t AudioManager::AudioManagerImpl::GetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    return 0;
}
}  // namespace Audio
