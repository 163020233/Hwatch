/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: bluetooth sub a2dp uplink header
* Author: Media Software Group
* Create: 2025-10-08
*/

#ifndef BLUETOOTH_A2DP_UPLINK_H
#define BLUETOOTH_A2DP_UPLINK_H

#include "audio_port_plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* sub port track api */
int32_t CreateTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t DestroyTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t EnableTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t DisableTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t TrackSetMuteInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, bool mute);
int32_t TrackGetMuteInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, bool* mute);
int32_t TrackSetVolumeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, float volume);
int32_t TrackGetVolumeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, float* volume);
int32_t PauseTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t ResumeTrackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle);
int32_t SendFrameInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, const uint8_t *buffer,
    uint64_t requestBytes, uint64_t *replyBytes);
int32_t GetPositionInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, uint64_t *frames,
    struct AudioTimeStamp *time);
int32_t InvokeInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle, enum InvokeID invokeID,
    struct InvokeAttr* invokeAttr);
int32_t RegCallbackInBluetoothA2DPSinkNormalScene(AudioHandle trackHandle,
    TrackCallback callback, const AudioHandle caller);

#ifdef __cplusplus
}
#endif
#endif
