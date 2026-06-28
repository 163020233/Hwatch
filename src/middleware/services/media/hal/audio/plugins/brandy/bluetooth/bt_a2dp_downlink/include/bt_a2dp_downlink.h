/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub a2dp downlink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#ifndef BLUETOOTH_A2DP_DOWNLINK_H
#define BLUETOOTH_A2DP_DOWNLINK_H

#include "audio_port_plugin.h"
#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t CreateTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t DestroyTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t EnableTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t DisableTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t TrackSetMuteInA2DPSourceNormalScene(AudioHandle trackHandle, bool mute);
int32_t TrackGetMuteInA2DPSourceNormalScene(AudioHandle trackHandle, bool* mute);
int32_t TrackSetVolumeInA2DPSourceNormalScene(AudioHandle trackHandle, float volume);
int32_t TrackGetVolumeInA2DPSourceNormalScene(AudioHandle trackHandle, float* volume);
int32_t PauseTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t ResumeTrackInA2DPSourceNormalScene(AudioHandle trackHandle);
int32_t SendFrameInA2DPSourceNormalScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t GetPositionInA2DPSourceNormalScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time);
int32_t InvokeInA2DPSourceNormalScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr);
int32_t RegCallbackInA2DPSourceNormalScene(AudioHandle trackHandle,
    TrackCallback callback, const AudioHandle caller);
int32_t RegCallbackInA2DPSourceNormalScene(AudioHandle trackHandle,
    TrackCallback callback, const AudioHandle caller);
int32_t EstablishTranscodeBackendPipeline(SubPortContext *portCtx, SubPortTrackContext *portTrackCtx,
    int32_t trackId);
int32_t AbrogationTranscodeBackendPipeline(const SubPortContext *portCtx,
    SubPortTrackContext *portTrackCtx);
int32_t BtEventCBCreate(SubPortContext *portCtx);
int32_t BtEventCBDestroy(void);

#ifdef __cplusplus
}
#endif
#endif
