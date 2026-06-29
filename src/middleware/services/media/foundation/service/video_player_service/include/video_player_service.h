/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: video player service
 * Author: Media Software Group
 * Create: 2025-09-23
 */

#ifndef VIDEO_PLAYER_SERVICE_H
#define VIDEO_PLAYER_SERVICE_H
#include <string>
#include "components/ui_label.h"
#include "components/ui_label_button.h"
#include "components/ui_lite_surface_view.h"
#include "player.h"
#include "media_video_player.h"

enum PlaybackRateMode : int32_t {
    SPEED_FORWARD_0_25_X,
    SPEED_FORWARD_0_50_X,
    SPEED_FORWARD_0_75_X,
    SPEED_FORWARD_1_00_X,
    SPEED_FORWARD_1_25_X,
    SPEED_FORWARD_1_50_X,
    SPEED_FORWARD_1_75_X,
    SPEED_FORWARD_2_00_X,
    SPEED_FORWARD_4_00_X,
    SPEED_FORWARD_8_00_X,
    SPEED_FORWARD_16_00_X,
    SPEED_FORWARD_32_00_X,
    SPEED_FORWARD_64_00_X,
    SPEED_REWIND_64_X,
    SPEED_REWIND_32_X,
    SPEED_REWIND_16_X,
    SPEED_REWIND_8_X,
    SPEED_REWIND_4_X,
    SPEED_REWIND_2_X,
    SPEED_REWIND_1_X,
};

namespace OHOS {
class VideoPlayerService {
public:
    VideoPlayerService(std::string uri, bool isPureVideo);
    VideoPlayerService(int32_t fd, uint64_t offset, bool isPureVideo);
    ~VideoPlayerService();
    VideoPlayerService(const VideoPlayerService &) = delete;
    VideoPlayerService &operator=(const VideoPlayerService &) = delete;
    int32_t Play(void);
    int32_t Stop(void);
    int32_t Pause(void);
    int32_t Resume(void);
    int32_t SeekTo(int64_t mSeconds);
    int32_t GetCurrentTime(int64_t *currentTime);
    int32_t GetVideoWidth(void);
    int32_t GetVideoHeight(void);
    int32_t GetDuration(int64_t &duration);
    int32_t SetPlaybackSpeed(PlaybackRateMode mode);
    int32_t GetPlaybackSpeed(PlaybackRateMode &mode);
    bool IsPlaying(void);
    bool IsPaused(void);
    bool IsLooping(void);
    void EnableSingleLooping(bool isLoop);
    void SetSurface(Surface *surface);
    void SetUILabelButton(UILabelButton *button);
    int32_t GetDumpInfo(Media::PlayerDebugInfo *playerInfo);
private:
    MediaVideoPlay *mediaVideoPlayer_ = NULL;
};
}
#endif