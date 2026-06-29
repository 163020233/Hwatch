/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLAYER_DEBUG_INFO_H
#define PLAYER_DEBUG_INFO_H

#include <cstdint>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

struct PlayerVideoDebugInfo {
    int32_t s32UsedVideoStreamIndex;
    int32_t enVideoType;
    uint32_t u32Width;
    uint32_t u32Height;
    uint32_t frameBufCnt;
};

struct PlayerAudioDebugInfo {
    int32_t s32UsedAudioStreamIndex;
    int32_t enAudioType;
    uint32_t u32AudioChannelCnt;
    uint16_t sampleFmt;
    uint32_t u32SampleRate;
    uint16_t bitWidth;
};

struct PlayerControlDebugInfo {
    uint32_t sessionId;
    int32_t outDevice;
    int64_t currentPosition;
    int64_t rewindPosition;
    float speed;
    int32_t playerControlState;
    bool isSingleLoop;
    bool isVideoStarted;
    bool isAudioStarted;
    bool hasRenderAudioEos;
    bool hasRenderVideoEos;
    bool offloadAudio;
};

struct PlayerFileDebugInfo {
    uint32_t streamType;
    std::string filePath;
    int32_t fd;
    uint64_t offset;
    int64_t s64FileSize;
    int64_t s64StartTime;
    int64_t s64Duration;
    int32_t s32UsedVideoStreamIndex;
    int32_t s32UsedAudioStreamIndex;
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif