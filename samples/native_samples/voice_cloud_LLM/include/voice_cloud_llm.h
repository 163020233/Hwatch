/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM main state machine entry API. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */
#ifndef VOICE_CLOUD_LLM_H
#define VOICE_CLOUD_LLM_H

#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

errcode_t VoiceCloudLlmStart(void);
errcode_t VoiceCloudLlmStop(void);
errcode_t VoiceCloudLlmOnWakeup(void);

/* Internal API used by audio_capture; declared here to avoid hidden cross-TU symbols. */
int32_t VoiceCloudLlmPostAudioFrame(const uint8_t *data, uint32_t len);
bool VoiceCloudLlmIsWakeupTestMode(void);
/*
 * True when the state machine is in IDLE_LISTENING (multi-turn between-turn
 * state, or first round awaiting speech_started after chat.updated).
 */
bool VoiceCloudLlmIsIdleListening(void);
/*
 * True when the state machine is in UPLINK_STREAMING. The capture thread uses
 * this to disable the silence energy gate during active speech upload — all
 * audio is sent unconditionally while the user is speaking.
 */
bool VoiceCloudLlmIsUplinkStreaming(void);

/*
 * True while a phone call is active (SCO audio streaming).  Checked at
 * every audio-stream start point to prevent ASR/capture/player init from
 * corrupting system audio state during a call.
 */
bool VoiceCloudLlmIsCallInterrupted(void);

/*
 * Called from OnInterrupt to trigger ReturnToWakeup on the worker thread.
 * Posts an ERROR message so that cleanup runs on the worker, not the
 * audio platform's interrupt callback thread.
 */
void VoiceCloudLlmPostCallInterrupt(void);

#ifdef __cplusplus
}
#endif

#endif
