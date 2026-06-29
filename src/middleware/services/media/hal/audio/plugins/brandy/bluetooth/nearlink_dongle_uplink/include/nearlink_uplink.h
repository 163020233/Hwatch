/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: nearlink uplink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#ifndef NEARLINK_UPLINK_H
#define NEARLINK_UPLINK_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t CreateTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t DestroyTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t EnableTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t DisableTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t TrackSetMuteInNearlinkUplinkScene(AudioHandle trackHandle, bool mute);
int32_t TrackGetMuteInNearlinkUplinkScene(AudioHandle trackHandle, bool *mute);
int32_t TrackSetVolumeInNearlinkUplinkScene(AudioHandle trackHandle, float volume);
int32_t TrackGetVolumeInNearlinkUplinkScene(AudioHandle trackHandle, float *volume);
int32_t PauseTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t ResumeTrackInNearlinkUplinkScene(AudioHandle trackHandle);
int32_t SendFrameInNearlinkUplinkScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t GetPositionInNearlinkUplinkScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time);
int32_t InvokeInNearlinkUplinkScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);
int32_t RegCallbackInNearlinkUplinkScene(AudioHandle trackHandle, TrackCallback callback,
    const AudioHandle caller);

#ifdef __cplusplus
}
#endif
#endif
