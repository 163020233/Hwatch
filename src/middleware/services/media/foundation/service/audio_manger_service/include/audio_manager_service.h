/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: audio manager c wrapper header
 * Author: Media Software Group
 * Create: 2023-08-11
 */

#ifndef AUDIO_MANAGER_SERVICE_H
#define AUDIO_MANAGER_SERVICE_H

#include "audio_base_type.h"

#ifdef __cplusplus
extern "C" {
#endif

bool AudioManagerInit(void);

AudioSession AudioManagerMakeSessionId(void);

int32_t AudioManagerSetDeviceConnectionState(AudioDeviceType deviceType, AudioDeviceConnectState connectState);

int32_t AudioManagerActivateInterrupt(AudioStreamType streamType, AudioSession sessionId);

int32_t AudioManagerDeactivateInterrupt(void);

int32_t AudioManagerSetVolume(AudioStreamType streamType, int32_t volume);

int32_t AudioManagerGetVolume(AudioStreamType streamType);

int32_t AudioManagerSetSeaEnable(bool enable);

int32_t AudioStreamInInit(CapturerInputConfig captureConfig);

int32_t AudioStreamInStart(void);

int32_t AudioStreamInObtainBuffer(uint8_t *data, uint32_t *size);

int32_t AudioStreamInSetBitRate(uint32_t bitrate);

int32_t AudioStreamInStop(void);

int32_t AudioStreamInDeinit(void);

int32_t AudioStreamOutInit(AudioRendererConfig renderConfig);

int32_t AudioStreamOutSetMute(bool mute);

int32_t AudioStreamOutPlay(void);

int32_t AudioStreamOutStreamWrite(const uint8_t *data, uint32_t size);

int32_t AudioStreamOutStop(void);

int32_t AudioStreamOutDeinit(void);

int32_t AudioManagerSetSeaParam(int32_t cmd, const uint8_t *seaParam, uint16_t len);

int32_t AudioManagerGetSeaParam(int32_t cmd, uint8_t *seaParam, uint16_t len);

int32_t AudioManagerSetAefParam(int32_t effectType, const uint8_t *aefParam, uint16_t len);

int32_t AudioManagerGetAefParam(int32_t effectType, uint8_t *aefParam, uint16_t len);

int32_t AudioManagerSetA2dpAttr(AudioA2dpCodecAttr *data, uint32_t dataLen);

void AudioManagerDumpInfo(void);

int32_t AudioManagerSetSeaLibId(int32_t seaId);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_MANAGER_C_WRAPPER_H */
