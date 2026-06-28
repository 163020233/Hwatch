/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: video player service
 * Author: Media Software Group
 * Create: 2025-09-23
 */

#include "video_player_service.h"
#include "media_video_player.h"
#include "media_log.h"

namespace OHOS {
    VideoPlayerService::VideoPlayerService(std::string uri, bool isPureVideo)
    {
        mediaVideoPlayer_ = new (std::nothrow) MediaVideoPlay(nullptr, nullptr, uri, isPureVideo);
    }

    VideoPlayerService::VideoPlayerService(int32_t fd, uint64_t offset, bool isPureVideo)
    {
        mediaVideoPlayer_ = new MediaVideoPlay(nullptr, fd, offset, isPureVideo);
    }

    VideoPlayerService::~VideoPlayerService()
    {
        if (mediaVideoPlayer_ != nullptr) {
            delete mediaVideoPlayer_;
            mediaVideoPlayer_ = nullptr;
        }
    }

    int32_t VideoPlayerService::Play(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->StartVideoPlay();
    }

    int32_t VideoPlayerService::Stop(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->StopVideoPlay();
    }

    int32_t VideoPlayerService::Pause(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->PauseVideoPlay();
    }

    int32_t VideoPlayerService::Resume(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->ResumeVideoPlay();
    }

    int32_t VideoPlayerService::GetDumpInfo(Media::PlayerDebugInfo *playerInfo)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->GetDumpInfo(playerInfo);
    }

    int32_t VideoPlayerService::SeekTo(int64_t mSeconds)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->SeekVideoPlay(mSeconds);
    }

    int32_t VideoPlayerService::GetCurrentTime(int64_t *currentTime)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->GetCurrentPosition(currentTime);
    }

    int32_t VideoPlayerService::GetVideoWidth(void)
    {
        return 0;
    }

    int32_t VideoPlayerService::GetVideoHeight(void)
    {
        return 0;
    }

    int32_t VideoPlayerService::GetDuration(int64_t &duration)
    {
        if (mediaVideoPlayer_ == nullptr || mediaVideoPlayer_->player_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->player_->GetDuration(duration);
    }

    int32_t SetPlaybackSpeed(PlaybackRateMode mode)
    {
        return 0;
    }

    int32_t GetPlaybackSpeed(PlaybackRateMode &mode)
    {
        return 0;
    }

    bool VideoPlayerService::IsPlaying(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->isEntered_;
    }

    bool VideoPlayerService::IsPaused(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->isPause_;
    }

    bool VideoPlayerService::IsLooping(void)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return MEDIA_ERR;
        }
        return mediaVideoPlayer_->isLoop_;
    }

    void VideoPlayerService::EnableSingleLooping(bool isLoop)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return;
        }
        mediaVideoPlayer_->SetVideoPlayLoop(isLoop);
    }

    void VideoPlayerService::SetSurface(Surface *surface)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return;
        }
        mediaVideoPlayer_->SetVideoSurface(surface);
    }

    void VideoPlayerService::SetUILabelButton(UILabelButton *button)
    {
        if (mediaVideoPlayer_ == nullptr) {
            MEDIA_ERR_LOG("mediaVideoPlayer_ is NUll!");
            return;
        }
        mediaVideoPlayer_->SetUILabelButton(button);
    }
}