/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: stream source player service
 * Author: Media Software Group
 * Create: 2026-01-27
 */

#include "stream_source_player_service.h"

#include <climits>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include "source.h"
#include "player.h"
#include "format.h"
#include "securec.h"

#include "audio_manager.h"
#include "audio_base_type.h"

#include "media_log.h"

#ifdef ENABLE_UIKIT
#include "surface.h"
#endif
#include "media_mem.h"

namespace OHOS {
using OHOS::Media::Player;
using OHOS::Media::Source;
using OHOS::Media::Format;

using namespace OHOS::Media;
using namespace::Audio;
using Audio::AudioManager;

#define CHK_NULL_RETURN(ptr, ret, printfString) \
do { \
    if ((ptr) == NULL) { \
        MEDIA_ERR_LOG("%s :ret:%d", ((printfString) == NULL) ? "" : (printfString), ret); \
        return (ret); \
    } \
} while (0)

StreamSourcePlayerService::StreamSourcePlayerService()
    : mutex_(MediaMutexCreate(nullptr))
{
    aviableBuffer_.clear();
}

StreamSourcePlayerService::~StreamSourcePlayerService(void)
{
    aviableBuffer_.clear();
    MediaMutexDestroy(&mutex_);
}

int32_t StreamSourcePlayerService::InitPlayer(void)
{
    player_ = std::make_shared<Player>();
    if (player_ == nullptr || player_.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return -1;
    }
    player_->SetPlayerCallback(shared_from_this());
    return MEDIA_OK;
}

int32_t StreamSourcePlayerService::PreparePlayer(void)
{
    Source source(shared_from_this(), formats_);
    int32_t ret = player_->SetSource(source);
    if (ret != 0) {
        MEDIA_ERR_LOG("set source failed!");
        return ret;
    }
    ret = player_->SetAudioStreamType(AUDIO_STREAM_FITNESS_VIDEO);
    if (ret != 0) {
        MEDIA_ERR_LOG("SetAudioStreamType failed");
        return ret;
    }
    ret = player_->Prepare();
    if (ret != 0) {
        MEDIA_ERR_LOG("Prepare failed");
        return ret;
    }
#if defined(ENABLE_UIKIT)
    ret = player_->SetVideoSurface(surface_);
#endif
    if (ret != 0) {
        MEDIA_ERR_LOG("SetVideoSurface failed");
        return ret;
    }
    MEDIA_INFO_LOG("sucess");
    return MEDIA_OK;
}

void StreamSourcePlayerService::DeinitPlayer(void)
{
    (void)player_.reset();
}

int32_t StreamSourcePlayerService::SetStreamSourceInfo(Format &formats)
{
    formats_.CopyFrom(formats);
    return MEDIA_OK;
}

#if defined(ENABLE_UIKIT)
int32_t StreamSourcePlayerService::SetVideoSurface(Surface *surface)
{
    if (surface == nullptr) {
        MEDIA_ERR_LOG("surface is nullptr");
        return MEDIA_INVALID_PARAM;
    }
    surface_ = surface;
    return MEDIA_OK;
}
#endif

int32_t StreamSourcePlayerService::SetAudioStreamType(AudioStreamType type)
{
    if (type != AUDIO_STREAM_MUSIC && type != AUDIO_STREAM_FITNESS_VIDEO) {
        MEDIA_ERR_LOG("type :%x invalid", type);
        return MEDIA_INVALID_PARAM;
    }
    streamType_ = type;
    return MEDIA_OK;
}

int32_t StreamSourcePlayerService::Start()
{
    if (states_ != StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_IDLE) {
        MEDIA_ERR_LOG("Play not IDLE");
        return MEDIA_ERR;
    }
    int32_t ret = InitPlayer();
    if (ret != 0) {
        return MEDIA_ERR;
    }
    ret = PreparePlayer();
    if (ret != 0) {
        return MEDIA_ERR;
    }
    ret = player_->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play failed");
        return MEDIA_ERR;
    }
    states_ = StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_PLAYED;
    return MEDIA_OK;
}

int32_t StreamSourcePlayerService::Pause()
{
    if (states_ != StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_PLAYED) {
        MEDIA_ERR_LOG("Play not PLAYED");
        return MEDIA_ERR;
    }
    int32_t ret = player_->Pause();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play Pause failed");
        return MEDIA_ERR;
    }
    states_ = StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_PAUSED;
    return MEDIA_OK;
}

int32_t StreamSourcePlayerService::Resume()
{
    if (states_ != StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_PAUSED) {
        MEDIA_ERR_LOG("Play not paused");
        return MEDIA_ERR;
    }
    int32_t ret = player_->Play();
    if (ret != 0) {
        MEDIA_ERR_LOG("Play resume failed");
        return MEDIA_ERR;
    }
    states_ = StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_PLAYED;
    return MEDIA_OK;
}

int32_t StreamSourcePlayerService::Stop()
{
    // IDLE Ò²ÔÊÐístop£¬·ñÔò Start ÖÐ¼äÊ§°Ü£¬×ÊÔ´ÎÞ·¨ÊÍ·ÅµÄÇé¿ö
    if (states_ == StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_STOPED) {
        MEDIA_INFO_LOG("Has already stoped or not init");
        return MEDIA_OK;
    }
    if (player_ == nullptr || player_.get() == nullptr) {
        MEDIA_ERR_LOG("player is nullptr");
        return MEDIA_OK;
    }
    int32_t ret = player_->Stop();
    if (ret != 0) {
        MEDIA_ERR_LOG("Stop failed");
    }
    ret = player_->Reset();
    if (ret != 0) {
        MEDIA_ERR_LOG("Reset failed");
    }
    ret = player_->Release();
    if (ret != 0) {
        MEDIA_ERR_LOG("Release failed");
    }
    states_ = StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_STOPED;
    return MEDIA_OK;
}

void StreamSourcePlayerService::SetPlayerCallback(const std::shared_ptr<StreamSourcePlayerCallback> &cb)
{
    callBack_ = cb;
}

void StreamSourcePlayerService::SetStreamCallback(const std::shared_ptr<StreamCallback> &callback)
{
    streamCallBack_ = callback;
}

uint8_t *StreamSourcePlayerService::GetBufferAddress(size_t idx)
{
    std::shared_ptr<StreamCallback> callback = streamCallBack_.lock();
    if (callback == nullptr) {
        return nullptr;
    }
    return callback->GetBuffer(idx);
}

void StreamSourcePlayerService::QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags)
{
    std::shared_ptr<StreamCallback> callback = streamCallBack_.lock();
    if (callback == nullptr) {
        return;
    }
    callback->QueueBuffer(index, offset, size, timestampUs, flags);
}

void StreamSourcePlayerService::OnBufferAvailable(size_t index, size_t offset, size_t size, int32_t flag)
{
    IdleBuffer buffer;
    MediaMutexLock(mutex_);
    buffer.idx = index;
    buffer.offset = offset;
    buffer.size = size;
    buffer.flag = flag;
    aviableBuffer_.push_back(buffer);
    MediaMutexUnLock(mutex_);
}

int32_t StreamSourcePlayerService::GetAvailableBuffer(IdleBuffer *buffer)
{
    CHK_NULL_RETURN(buffer, MEDIA_ERR, "buffer null");
    MediaMutexLock(mutex_);
    if (aviableBuffer_.empty()) {
        MediaMutexUnLock(mutex_);
        return -1;
    }
    *buffer = aviableBuffer_.back();
    aviableBuffer_.pop_back();
    MediaMutexUnLock(mutex_);
    return 0;
}

int32_t StreamSourcePlayerService::GetPlayStatus(void)
{
    MediaMutexLock(mutex_);
    int32_t state = (int32_t)states_;
    MediaMutexUnLock(mutex_);
    return state;
}

bool StreamSourcePlayerService::IsPlaying(void)
{
    if (player_ == nullptr) {
        MEDIA_DEBUG_LOG("player_ is NULL!");
        return false;
    }
    return player_->IsPlaying();
}

int32_t StreamSourcePlayerService::GetDuration(int64_t &durationMs)
{
    if (player_ == nullptr) {
        MEDIA_ERR_LOG("player_ is NULL!");
        return MEDIA_ERR;
    }
    return player_->GetDuration(durationMs);
}

void StreamSourcePlayerService::OnInterrupt(int32_t type, int32_t hint)
{
    MEDIA_INFO_LOG("OnInterrupt, type:%d, hint:%d", type, hint);
    if (callBack_ != nullptr) {
        callBack_->OnInterrupt(type, hint);
        return;
    }
}

void StreamSourcePlayerService::OnPlaybackComplete()
{
    if (callBack_ != nullptr) {
        callBack_->OnPlaybackComplete();
        MEDIA_INFO_LOG("on playback complete!");
    }
}

void StreamSourcePlayerService::OnError(int32_t errorType, int32_t errorCode)
{
    MEDIA_INFO_LOG("on playback Error, type:%d errorCode:%d", errorType, errorCode);
    if (callBack_ != nullptr) {
        callBack_->OnError(errorType, errorCode);
        MEDIA_INFO_LOG("on playback Error!");
    }
}

void StreamSourcePlayerService::OnInfo(int32_t type, int32_t extra)
{
    MEDIA_INFO_LOG("on playback info, type:%dm extra:%d", type, extra);
    if (callBack_ != nullptr) {
        callBack_->OnInfo(type, extra);
        MEDIA_INFO_LOG("on playback OnInfo!");
    }
}

void StreamSourcePlayerService::OnRewindToComplete()
{
    MEDIA_INFO_LOG("on playback rewind to complete");
}

}; // OHOS
