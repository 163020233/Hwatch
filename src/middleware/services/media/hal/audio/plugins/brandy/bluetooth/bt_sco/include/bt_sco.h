/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub sco header
* Author: Media Software Group
* Create: 2025-10-08
*/

#ifndef BLUETOOTH_SCO_H
#define BLUETOOTH_SCO_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t CreateTrackInSCOScene(AudioHandle trackHandle);
int32_t DestroyTrackInSCOScene(AudioHandle trackHandle);
int32_t EnableTrackInSCOScene(AudioHandle trackHandle);
int32_t DisableTrackInSCOScene(AudioHandle trackHandle);
int32_t TrackSetMuteInSCOScene(AudioHandle trackHandle, bool mute);
int32_t TrackGetMuteInSCOScene(AudioHandle trackHandle, bool *mute);
int32_t TrackSetVolumeInSCOScene(AudioHandle trackHandle, float volume);
int32_t TrackGetVolumeInSCOScene(AudioHandle trackHandle, float *volume);
int32_t PauseTrackInSCOScene(AudioHandle trackHandle);
int32_t ResumeTrackInSCOScene(AudioHandle trackHandle);
int32_t SendFrameInSCOScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t GetPositionInSCOScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time);
int32_t InvokeInSCOScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr *invokeAttr);
int32_t RegCallbackInSCOScene(AudioHandle trackHandle, TrackCallback callback,
    const AudioHandle caller);

#ifdef __cplusplus
}
#endif
#endif
