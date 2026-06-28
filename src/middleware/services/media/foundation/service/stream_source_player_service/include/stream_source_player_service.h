/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 * Description: stream source player service header file
 * Author: Media Software Group
 * Create: 2026-01-27
 */

#ifndef STREAM_SOURCE_PLAYER_SERVICE_H
#define STREAM_SOURCE_PLAYER_SERVICE_H

#include <string>
#include <vector>
#include <memory>
#include <sys/time.h>
#include "source.h"
#include "player.h"
#include "format.h"
#include "audio_manager.h"
#include "audio_base_type.h"
#include "media_thread_adapt.h"

namespace OHOS {
using namespace::Audio;
using namespace OHOS::Media;
using OHOS::Media::StreamSource;
using OHOS::Media::StreamCallback;
using OHOS::Media::Player;

enum class StreamSourcePlayerStates : int32_t {
    STREAM_SOURCE_PLAYER_IDLE,
    STREAM_SOURCE_PLAYER_PAUSED,
    STREAM_SOURCE_PLAYER_PLAYED,
    STREAM_SOURCE_PLAYER_STOPED,
};

typedef struct TagIdleBuffer {
    size_t idx;
    size_t offset;
    size_t size;
    int32_t flag;
} IdleBuffer;

class StreamSourcePlayerCallback {
public:
    StreamSourcePlayerCallback() = default;
    virtual ~StreamSourcePlayerCallback() {}
    virtual void OnInterrupt(int32_t type, int32_t hint) = 0;
    virtual void OnError(int32_t errorType, int32_t errorCode) = 0;
    virtual void OnInfo(int32_t type, int32_t extra) = 0;
    virtual void OnPlaybackComplete() = 0;
};

class StreamSourcePlayerService : public PlayerCallback, public StreamSource, public InterruptListener,
    public std::enable_shared_from_this<StreamSourcePlayerService> {
public:
    ~StreamSourcePlayerService() override;
    StreamSourcePlayerService();

    void SetPlayerCallback(const std::shared_ptr<StreamSourcePlayerCallback> &cb);
    int32_t SetAudioStreamType(AudioStreamType type);
    int32_t SetStreamSourceInfo(Format &formats);
#if defined(ENABLE_UIKIT)
    int32_t SetVideoSurface(Surface *surface);
#endif

    int32_t Start(void);
    int32_t Stop(void);
    int32_t Pause(void);
    int32_t Resume(void);
    // stream source
    void OnBufferAvailable(size_t index, size_t offset, size_t size, int32_t flag) override;
    void SetStreamCallback(const std::shared_ptr<StreamCallback> &callback) override;
    uint8_t *GetBufferAddress(size_t idx);
    void QueueBuffer(size_t index, size_t offset, size_t size, int64_t timestampUs, uint32_t flags);
    int32_t GetAvailableBuffer(IdleBuffer *buffer);

    int32_t GetPlayStatus(void);
    bool IsPlaying(void);
    int32_t GetDuration(int64_t &durationMs);
protected:
    void OnPlaybackComplete() override;
    void OnError(int32_t errorType, int32_t errorCode) override;
    void OnInfo(int32_t type, int32_t extra) override;
    void OnRewindToComplete() override;
    void OnInterrupt(int32_t type, int32_t hint) override;
private:
    int32_t InitPlayer(void);
    void DeinitPlayer(void);
    int32_t PreparePlayer(void);
    StreamSourcePlayerStates states_ = StreamSourcePlayerStates::STREAM_SOURCE_PLAYER_IDLE;
    AudioStreamType streamType_ = AUDIO_STREAM_MUSIC;
    Format formats_;
    Surface *surface_ = nullptr;
    std::shared_ptr<Player> player_ = nullptr;
    std::shared_ptr<StreamSourcePlayerCallback> callBack_ = nullptr;
    std::weak_ptr<StreamCallback> streamCallBack_;
    std::vector<IdleBuffer> aviableBuffer_;
    MediaMutexHandle mutex_;
};
}

#endif
