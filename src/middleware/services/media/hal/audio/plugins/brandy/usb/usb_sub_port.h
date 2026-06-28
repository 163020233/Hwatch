/*
* Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd.. 2025-2025. All rights reserved.
* Description: usb sub port header
* Author: Media Software Group
* Create: 2025-03-05
*/

#ifndef USB_SUB_PORT_H
#define USB_SUB_PORT_H

#include "audio_port_plugin.h"
#include "media_hal_thread_adapt.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    MediaHalThreadIdHandle taskHandle;
    MediaHalMutexHandle mutex;
    MediaHalThreadCondHandle condHandle;
    bool taskActive;
    uint32_t frameSize;
    uint32_t adpHandle;
    uint32_t trackHandle;
    struct PortPluginAttr inputAttr;
} UsbReceiveDataContext;

typedef struct {
    MediaHalThreadIdHandle taskHandle;
    MediaHalMutexHandle mutex;
    MediaHalThreadCondHandle condHandle;
    bool taskActive;
    // 标记第一次发送usb数据
    bool isUsbFirstSendFrame;
    uint32_t frameSize;
    uint32_t adpHandle;
} UsbSendDataContext;

/* sub port track api */
int32_t AudioUsbSubPortTrackGetLatency(AudioHandle trackHandle, uint32_t *latencyMs);

int32_t AudioUsbSubPortEnableTrack(AudioHandle trackHandle);
int32_t AudioUsbSubPortDisableTrack(AudioHandle trackHandle);

int32_t AudioUsbSubPortPauseTrack(AudioHandle trackHandle);
int32_t AudioUsbSubPortResumeTrack(AudioHandle trackHandle);

int32_t AudioUsbSubPortAcquireFrame(AudioHandle trackHandle, uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioUsbSubPortGetPosition(AudioHandle trackHandle, uint64_t *frames, struct AudioTimeStamp *time);

int32_t AudioUsbSubPortSendFrame(AudioHandle trackHandle, const uint8_t *buffer, uint64_t requestBytes,
    uint64_t *replyBytes);

int32_t AudioUsbSubPortTrackSetMute(AudioHandle trackHandle, bool mute);
int32_t AudioUsbSubPortTrackGetMute(AudioHandle trackHandle, bool *mute);

int32_t AudioUsbSubPortTrackSetVolume(AudioHandle trackHandle, float volume);
int32_t AudioUsbSubPortTrackGetVolume(AudioHandle trackHandle, float *volume);

int32_t AudioUsbSubPortInvoke(AudioHandle trackHandle, enum InvokeID invokeID, struct InvokeAttr *invokeAttr);

int32_t AudioUsbSubPortTrackRegCallback(AudioHandle trackHandle, TrackCallback callback, AudioHandle caller);

/* sub port api */
int32_t AudioUsbSubPortCreate(AudioHandle *portHandle, UsbReceiveDataContext *reCtx, UsbSendDataContext *sendCtx);
int32_t AudioUsbSubPortDestroy(AudioHandle portHandle);

int32_t AudioUsbSubPortGetCapability(const struct AudioPort *port, struct AudioPortCapability *capability);

int32_t AudioUsbSubPortCreateTrack(AudioHandle portHandle, const struct PortPluginAttr *inputAttr,
    enum AudioPortPin portPin, AudioHandle *trackHandle);
int32_t AudioUsbSubPortDestroyTrack(AudioHandle portHandle, AudioHandle trackHandle);

#ifdef __cplusplus
}
#endif

#endif
