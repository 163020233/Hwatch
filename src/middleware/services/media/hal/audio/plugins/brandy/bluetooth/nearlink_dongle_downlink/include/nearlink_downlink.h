/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: nearlink downlink header
* Author: Media Software GroDown
* Create: 2025-10-08
*/

#ifndef NEARLINK_DOWNLINK_H
#define NEARLINK_DOWNLINK_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t CreateTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t DestroyTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t EnableTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t DisableTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t TrackSetMuteInNearlinkDownlinkScene(AudioHandle trackHandle, bool mute);
int32_t TrackGetMuteInNearlinkDownlinkScene(AudioHandle trackHandle, bool *mute);
int32_t TrackSetVolumeInNearlinkDownlinkScene(AudioHandle trackHandle, float volume);
int32_t TrackGetVolumeInNearlinkDownlinkScene(AudioHandle trackHandle, float *volume);
int32_t PauseTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t ResumeTrackInNearlinkDownlinkScene(AudioHandle trackHandle);
int32_t SendFrameInNearlinkDownlinkScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t GetPositionInNearlinkDownlinkScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time);
int32_t InvokeInNearlinkDownlinkScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);
int32_t RegCallbackInNearlinkDownlinkScene(AudioHandle trackHandle, TrackCallback callback,
    const AudioHandle caller);

#ifdef __cplusplus
}
#endif
#endif
