/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2024-2024. All rights reserved.
* Description: audio manager
* Author: Media Software Group
* Create: 2024-07-10
*/

#include "audio_manager.h"
#include "audio_manager_impl.h"
#include "securec.h"
#include "audio_errors.h"
#include "audio_utils.h"

#define LOG_MODULE_NAME "AudioManager"

namespace Audio {
AudioManager::AudioManager() noexcept
    : impl_(nullptr)
{
}

AudioManager::~AudioManager()
{
    delete impl_;
    impl_ = nullptr;
}

bool AudioManager::Initialize()
{
    return false;
}

AudioSession AudioManager::MakeSessionId()
{
    return impl_->MakeSessionId();
}

bool AudioManager::SetVolume(AudioStreamType streamType, int32_t volume)
{
    return impl_->SetVolume(streamType, volume);
}

int32_t AudioManager::GetVolume(AudioStreamType streamType)
{
    return impl_->GetVolume(streamType);
}

int32_t AudioManager::GetMinVolume(AudioStreamType streamType)
{
    return impl_->GetMinVolume(streamType);
}

int32_t AudioManager::GetMaxVolume(AudioStreamType streamType)
{
    return impl_->GetMaxVolume(streamType);
}

bool AudioManager::Mute(AudioStreamType streamType)
{
    return impl_->Mute(streamType);
}

bool AudioManager::UnMute(AudioStreamType streamType)
{
    return impl_->UnMute(streamType);
}

bool AudioManager::IsMute(AudioStreamType streamType)
{
    return impl_->IsMute(streamType);
}

bool AudioManager::SetMasterMute(bool isMute)
{
    return impl_->SetMasterMute(isMute);
}

bool AudioManager::IsMasterMute()
{
    return impl_->IsMasterMute();
}

bool AudioManager::SetMicrophoneMute(bool isMute)
{
    return impl_->SetMicrophoneMute(isMute);
}

bool AudioManager::IsMicrophoneMute()
{
    return impl_->IsMicrophoneMute();
}

bool AudioManager::SetRingerMode(AudioRingMode mode)
{
    return impl_->SetRingerMode(mode);
}

int32_t AudioManager::GetRingerMode()
{
    return impl_->GetRingerMode();
}


std::vector<AudioDeviceInfo> AudioManager::GetDevices(DeviceFlag flag)
{
    return impl_->GetDevices(flag);
}

bool AudioManager::IsSupportOffload(AudioOffloadInfo &info)
{
    return impl_->IsSupportOffload(info);
}

int32_t AudioManager::SetDeviceConnectionState(AudioDeviceType device, AudioDeviceConnectState state)
{
    return impl_->SetDeviceConnectionState(device, state);
}

int32_t AudioManager::ActivateAudioInterrupt(AudioInterrupt &interrupt)
{
    return impl_->ActivateAudioInterrupt(interrupt);
}

int32_t AudioManager::DeactivateAudioInterrupt(AudioInterrupt &interrupt)
{
    return impl_->DeactivateAudioInterrupt(interrupt);
}

int32_t AudioManager::DumpInfo(AudioManagerDebugInfo &audioManagerInfo)
{
    return impl_->DumpInfo(audioManagerInfo);
}

int32_t AudioManager::SetParam(AudioLinkDirection direction, const char *param, uint32_t len)
{
    return impl_->SetParam(direction, param, len);
}

int32_t AudioManager::GetParam(AudioLinkDirection direction, char *param, uint32_t len)
{
    return impl_->GetParam(direction, param, len);
}
}  // namespace Audio
