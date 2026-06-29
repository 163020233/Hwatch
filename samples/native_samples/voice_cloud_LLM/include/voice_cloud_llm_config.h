/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM cloud config: WS URL, auth token, bot id, audio params. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */
#ifndef VOICE_CLOUD_LLM_CONFIG_H
#define VOICE_CLOUD_LLM_CONFIG_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

#define VOICE_CLOUD_LLM_INPUT_SAMPLE_RATE  16000
/*
 * Output rate must match what the adec firmware on this MCU accepts for raw
 * PCM. 24000 Hz consistently fails uapi_adec_set_attr with 0x80005321 even
 * when DSP0 is freshly powered (verified: failure happens within 130 ms of a
 * successful 16 kHz capture stop, no deep-sleep in between). The local TTS
 * engine reference uses 8000 Hz; 16 kHz is the safe rate that also matches
 * input, so we request 16 kHz output PCM from Coze.
 */
#define VOICE_CLOUD_LLM_OUTPUT_SAMPLE_RATE 16000
#define VOICE_CLOUD_LLM_DEFAULT_CHANNELS   1
#define VOICE_CLOUD_LLM_DEFAULT_BITS       16
#define VOICE_CLOUD_LLM_UPLINK_FRAME_MS    20
#define VOICE_CLOUD_LLM_DOWNLINK_FRAME_MS  160
#define VOICE_CLOUD_LLM_DOWNLINK_FRAME_NUM 7
/*
 * Number of 20 ms capture frames we accumulate before posting a single
 * input_audio_buffer.append. At 16 kHz / mono / 16-bit:
 *   8 frames  = 160 ms = 2560  B PCM →  ~3.5 KB after base64+JSON envelope
 *   10 frames = 200 ms = 3200  B PCM →  ~4.4 KB
 *   20 frames = 400 ms = 6400  B PCM →  ~8.7 KB → broke TLS! ⚠️
 *
 * Empirically at batchFrames=20 we saw "WsSocketSend ... sended 754 but want
 * 17138" spam for >10 s: the TLS layer on this MCU can only flush ~754 bytes
 * per socket write, and a 17 kB payload starves the WS so badly that server
 * VAD didn't see speech_started until 8 s after the user actually spoke. The
 * smaller the per-message payload the better; 8 frames keeps per-message
 * payload under ~3.5 KB while still cutting uplink to ~6 Hz (vs. 50 Hz raw).
 */
#define VOICE_CLOUD_LLM_UPLINK_BATCH_FRAMES  8
#define VOICE_CLOUD_LLM_CONNECT_TIMEOUT_MS   10000
#define VOICE_CLOUD_LLM_RESPONSE_TIMEOUT_MS  30000
#define VOICE_CLOUD_LLM_IDLE_TIMEOUT_MS      15000
/*
 * Silence energy gate for the capture thread. When the device is in
 * IDLE_LISTENING between conversation turns, PCM batches whose RMS falls
 * below this threshold are suppressed to a reduced keep-alive cadence
 * instead of being uploaded at the full ~6 Hz rate.  This conserves MCU
 * power, TLS uplink bandwidth, and server-side VAD cycles.
 *
 * RMS threshold is expressed in 16-bit PCM sample units (0–32767).
 * 400 ≈ –38 dBFS, which is well below normal speech but above electrical
 * noise floor (~–60 dBFS on this platform).  Tune per device/mic-gain if
 * ambient noise is consistently higher.
 *
 * Set to 0 to disable the energy gate entirely (always send every batch).
 */
#define VOICE_CLOUD_LLM_SILENCE_RMS_THRESHOLD 400
/*
 * Minimum interval between silence keep-alive batches during IDLE_LISTENING.
 * Server VAD needs occasional audio data to stay alive and detect the next
 * utterance; 800 ms strikes a balance between uplink reduction (~5:1) and
 * VAD responsiveness (worst-case added latency < 1 s).
 */
#define VOICE_CLOUD_LLM_SILENCE_KEEPALIVE_MS  800
/*
 * Number of batches to send unconditionally after capture thread starts before
 * the silence energy gate takes effect.
 */
#define VOICE_CLOUD_LLM_ENERGY_GATE_WARMUP_BATCHES 12
/*
 * Capture/playback PCM frame queue depth. 64 × 20 ms frame slots = 1.28 s of
 * audio buffered before the producer (capture thread / WS recv) starts
 * dropping. Bumped from 32 because sustained ≥10 s utterances showed
 * "audio queue full, dropped oldest frame" storm at 32.
 */
#define VOICE_CLOUD_LLM_QUEUE_DEPTH        64
#define VOICE_CLOUD_LLM_WS_URL_MAX_LEN     256
#define VOICE_CLOUD_LLM_AUTH_MAX_LEN       256
#define VOICE_CLOUD_LLM_BOT_ID_MAX_LEN     96

#ifndef VOICE_CLOUD_LLM_CAPTURE_DISABLE_SEA
#define VOICE_CLOUD_LLM_CAPTURE_DISABLE_SEA 1
#endif

typedef struct {
    char wsUrl[VOICE_CLOUD_LLM_WS_URL_MAX_LEN];
    char authToken[VOICE_CLOUD_LLM_AUTH_MAX_LEN];
    char botId[VOICE_CLOUD_LLM_BOT_ID_MAX_LEN];
    uint32_t inputSampleRate;
    uint32_t outputSampleRate;
    uint32_t channels;
    uint32_t bitsPerSample;
    uint32_t uplinkFrameMs;
    uint32_t downlinkFrameMs;
    uint32_t downlinkFrameNum;
} VoiceCloudLlmConfig;

const VoiceCloudLlmConfig *VoiceCloudLlmGetConfig(void);
errcode_t VoiceCloudLlmSetAuth(const char *botId, const char *authToken);
errcode_t VoiceCloudLlmPrecheckConfig(const VoiceCloudLlmConfig *config);

#ifdef __cplusplus
}
#endif

#endif
