/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: video player service
 * Author: Media Software Group
 * Create: 2025-09-23
 */

#include "video_player_service.h"

namespace OHOS {
    VideoPlayerService::VideoPlayerService(std::string uri, bool isPureVideo)
    {
    }

    VideoPlayerService::VideoPlayerService(int32_t fd, uint64_t offset, bool isPureVideo)
    {
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
        return 0;
    }

    int32_t VideoPlayerService::Stop(void)
    {
        return 0;
    }

    int32_t VideoPlayerService::Pause(void)
    {
        return 0;
    }

    int32_t VideoPlayerService::Resume(void)
    {
        return 0;
    }

    int32_t VideoPlayerService::GetDumpInfo(Media::PlayerDebugInfo *playerInfo)
    {
        return 0;
    }

    int32_t VideoPlayerService::SeekTo(int64_t mSeconds)
    {
        return 0;
    }

    int32_t VideoPlayerService::GetCurrentTime(int64_t *currentTime)
    {
        return 0;
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
        return 0;
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
        return 0;
    }

    bool VideoPlayerService::IsPaused(void)
    {
        return 0;
    }

    bool VideoPlayerService::IsLooping(void)
    {
        return 0;
    }

    void VideoPlayerService::EnableSingleLooping(bool isLoop)
    {
        return 0;
    }

    void VideoPlayerService::SetSurface(Surface *surface)
    {
        return 0;
    }

    void VideoPlayerService::SetUILabelButton(UILabelButton *button)
    {
        return 0;
    }
}