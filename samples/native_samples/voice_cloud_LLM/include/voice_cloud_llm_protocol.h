/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM cloud protocol event definitions. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */
#ifndef VOICE_CLOUD_LLM_PROTOCOL_H
#define VOICE_CLOUD_LLM_PROTOCOL_H

#include <stdbool.h>
#include <stdint.h>
#include "voice_cloud_llm_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VOICE_CLOUD_LLM_EVENT_NONE = 0,
    VOICE_CLOUD_LLM_EVENT_SESSION_CREATED,      /* chat.created */
    VOICE_CLOUD_LLM_EVENT_SESSION_UPDATED,      /* chat.updated */
    VOICE_CLOUD_LLM_EVENT_SPEECH_STARTED,       /* input_audio_buffer.speech_started */
    VOICE_CLOUD_LLM_EVENT_SPEECH_STOPPED,       /* input_audio_buffer.speech_stopped */
    VOICE_CLOUD_LLM_EVENT_INPUT_AUDIO_DONE,     /* input_audio_buffer.completed (server vad commit ACK) */
    VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA,          /* conversation.audio.delta */
    VOICE_CLOUD_LLM_EVENT_TEXT_DELTA,           /* conversation.message.delta */
    VOICE_CLOUD_LLM_EVENT_AUDIO_STREAM_COMPLETED, /* conversation.audio.completed — drain only */
    VOICE_CLOUD_LLM_EVENT_CHAT_COMPLETED,       /* conversation.chat.completed — return to wakeup */
    VOICE_CLOUD_LLM_EVENT_SERVER_ERROR,         /* error / conversation.chat.failed */
} VoiceCloudLlmServerEventType;

typedef struct {
    VoiceCloudLlmServerEventType type;
    const uint8_t *payload;
    uint32_t payloadLen;
    bool payloadAllocated;
} VoiceCloudLlmServerEvent;

bool VoiceCloudLlmBuildSessionStart(const VoiceCloudLlmConfig *config, char *buffer, uint32_t bufferLen);
bool VoiceCloudLlmBuildAudioAppend(const uint8_t *data, uint32_t len, char *buffer, uint32_t bufferLen);
bool VoiceCloudLlmBuildAudioCommit(char *buffer, uint32_t bufferLen);  /* kept for client_vad; unused by server_vad */
bool VoiceCloudLlmBuildCancel(char *buffer, uint32_t bufferLen);

/*
 * Parses one server message into a protocol event.
 *
 * Payload ownership:
 * - If payloadAllocated is true, callers must release it with
 *   VoiceCloudLlmFreeServerEvent().
 * - Otherwise JSON delta payload points to an internal static buffer and is valid
 *   only until the next VoiceCloudLlmParseServerMessage() call. This API is not
 *   reentrant or thread-safe for JSON delta payload access.
 * - For binary events, payload points directly to the caller-owned input data.
 */
VoiceCloudLlmServerEvent VoiceCloudLlmParseServerMessage(const uint8_t *data, uint32_t len, bool binary);
void VoiceCloudLlmFreeServerEvent(VoiceCloudLlmServerEvent *event);

#ifdef __cplusplus
}
#endif

#endif
