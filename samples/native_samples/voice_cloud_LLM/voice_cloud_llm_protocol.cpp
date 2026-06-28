/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM cloud JSON builders and server event parser. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm_protocol.h"

#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

namespace {

const uint32_t JSON_DELTA_BUFFER_SIZE = 4096;
uint8_t g_jsonDeltaBuffer[JSON_DELTA_BUFFER_SIZE];
const char BASE64_ENCODE_TABLE[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

bool CopyJsonToBuffer(cJSON *root, char *buffer, uint32_t bufferLen)
{
    if (root == nullptr || buffer == nullptr) {
        return false;
    }

    char *json = cJSON_PrintUnformatted(root);
    if (json == nullptr) {
        return false;
    }

    const size_t jsonLen = strlen(json);
    const bool ok = (jsonLen + 1U <= bufferLen);
    if (ok) {
        memcpy(buffer, json, jsonLen + 1U);
    }
    cJSON_free(json);
    return ok;
}

bool AddEventType(cJSON *root, const char *eventType)
{
    return cJSON_AddStringToObject(root, "event_type", eventType) != nullptr;
}

uint32_t Base64EncodedLen(uint32_t len)
{
    return ((len + 2U) / 3U) * 4U;
}

bool EncodeBase64(const uint8_t *data, uint32_t len, char *buffer, uint32_t bufferLen)
{
    if (data == nullptr || buffer == nullptr) {
        return false;
    }

    const uint32_t encodedLen = Base64EncodedLen(len);
    if (encodedLen + 1U > bufferLen) {
        return false;
    }

    uint32_t in = 0;
    uint32_t out = 0;
    while (in < len) {
        const uint32_t octetA = data[in++];
        const uint32_t octetB = (in < len) ? data[in++] : 0;
        const uint32_t octetC = (in < len) ? data[in++] : 0;
        const uint32_t triple = (octetA << 16U) | (octetB << 8U) | octetC;

        buffer[out++] = BASE64_ENCODE_TABLE[(triple >> 18U) & 0x3FU];
        buffer[out++] = BASE64_ENCODE_TABLE[(triple >> 12U) & 0x3FU];
        buffer[out++] = BASE64_ENCODE_TABLE[(triple >> 6U) & 0x3FU];
        buffer[out++] = BASE64_ENCODE_TABLE[triple & 0x3FU];
    }

    const uint32_t padding = (3U - (len % 3U)) % 3U;
    for (uint32_t i = 0; i < padding; ++i) {
        buffer[encodedLen - 1U - i] = '=';
    }
    buffer[encodedLen] = '\0';
    return true;
}

uint8_t Base64DecodeChar(char ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return static_cast<uint8_t>(ch - 'A');
    }
    if (ch >= 'a' && ch <= 'z') {
        return static_cast<uint8_t>(ch - 'a' + 26);
    }
    if (ch >= '0' && ch <= '9') {
        return static_cast<uint8_t>(ch - '0' + 52);
    }
    if (ch == '+') {
        return 62;
    }
    if (ch == '/') {
        return 63;
    }
    return 0xFF;
}

bool DecodeBase64(const char *text, uint8_t *buffer, uint32_t bufferLen, uint32_t *decodedLen)
{
    if (text == nullptr || buffer == nullptr || decodedLen == nullptr) {
        return false;
    }

    const size_t textLen = strlen(text);
    if (textLen == 0 || (textLen % 4U) != 0) {
        return false;
    }

    uint32_t out = 0;
    for (size_t i = 0; i < textLen; i += 4U) {
        const uint8_t a = Base64DecodeChar(text[i]);
        const uint8_t b = Base64DecodeChar(text[i + 1U]);
        const bool hasC = text[i + 2U] != '=';
        const bool hasD = text[i + 3U] != '=';
        const uint8_t c = hasC ? Base64DecodeChar(text[i + 2U]) : 0;
        const uint8_t d = hasD ? Base64DecodeChar(text[i + 3U]) : 0;
        if (a == 0xFF || b == 0xFF || (hasC && c == 0xFF) || (hasD && d == 0xFF)) {
            return false;
        }

        const uint32_t triple = (static_cast<uint32_t>(a) << 18U) |
            (static_cast<uint32_t>(b) << 12U) |
            (static_cast<uint32_t>(c) << 6U) |
            d;
        if (out >= bufferLen) {
            return false;
        }
        buffer[out++] = static_cast<uint8_t>((triple >> 16U) & 0xFFU);
        if (hasC) {
            if (out >= bufferLen) {
                return false;
            }
            buffer[out++] = static_cast<uint8_t>((triple >> 8U) & 0xFFU);
        }
        if (hasD) {
            if (out >= bufferLen) {
                return false;
            }
            buffer[out++] = static_cast<uint8_t>(triple & 0xFFU);
        }
    }

    *decodedLen = out;
    return true;
}

bool AddInputAudio(cJSON *data, const VoiceCloudLlmConfig *config)
{
    cJSON *inputAudio = cJSON_CreateObject();
    if (inputAudio == nullptr) {
        return false;
    }

    if (cJSON_AddStringToObject(inputAudio, "format", "pcm") == nullptr ||
        cJSON_AddStringToObject(inputAudio, "codec", "pcm") == nullptr ||
        cJSON_AddNumberToObject(inputAudio, "sample_rate", config->inputSampleRate) == nullptr ||
        cJSON_AddNumberToObject(inputAudio, "channel", config->channels) == nullptr ||
        cJSON_AddNumberToObject(inputAudio, "bit_depth", config->bitsPerSample) == nullptr ||
        !cJSON_AddItemToObject(data, "input_audio", inputAudio)) {
        cJSON_Delete(inputAudio);
        return false;
    }

    return true;
}

bool AddOutputAudio(cJSON *data, const VoiceCloudLlmConfig *config)
{
    cJSON *outputAudio = cJSON_CreateObject();
    cJSON *pcmConfig = cJSON_CreateObject();
    cJSON *limitConfig = cJSON_CreateObject();
    if (outputAudio == nullptr || pcmConfig == nullptr || limitConfig == nullptr) {
        cJSON_Delete(outputAudio);
        cJSON_Delete(pcmConfig);
        cJSON_Delete(limitConfig);
        return false;
    }

    if (cJSON_AddStringToObject(outputAudio, "codec", "pcm") == nullptr ||
        cJSON_AddNumberToObject(pcmConfig, "sample_rate", config->outputSampleRate) == nullptr ||
        cJSON_AddNumberToObject(pcmConfig, "frame_size_ms", config->downlinkFrameMs) == nullptr ||
        cJSON_AddNumberToObject(limitConfig, "period", 1) == nullptr ||
        cJSON_AddNumberToObject(limitConfig, "max_frame_num", config->downlinkFrameNum) == nullptr ||
        !cJSON_AddItemToObject(pcmConfig, "limit_config", limitConfig) ||
        !cJSON_AddItemToObject(outputAudio, "pcm_config", pcmConfig)) {
        cJSON_Delete(outputAudio);
        cJSON_Delete(pcmConfig);
        cJSON_Delete(limitConfig);
        return false;
    }
    pcmConfig = nullptr;

    if (!cJSON_AddItemToObject(data, "output_audio", outputAudio)) {
        cJSON_Delete(outputAudio);
        return false;
    }

    return true;
}

bool AddSessionData(cJSON *root, const VoiceCloudLlmConfig *config)
{
    cJSON *data = cJSON_CreateObject();
    cJSON *chatConfig = cJSON_CreateObject();
    cJSON *turnDetection = cJSON_CreateObject();
    const char *subscriptions[] = {
        "conversation.audio.delta",
        "conversation.audio.completed",
        "conversation.chat.completed",
        "conversation.chat.failed",
        "input_audio_buffer.completed",
        "input_audio_buffer.speech_started",
        "input_audio_buffer.speech_stopped",
        "chat.created",
        "chat.updated",
        "error",
    };
    cJSON *eventSubscriptions = cJSON_CreateStringArray(subscriptions,
        static_cast<int>(sizeof(subscriptions) / sizeof(subscriptions[0])));

    if (data == nullptr || chatConfig == nullptr || turnDetection == nullptr ||
        eventSubscriptions == nullptr) {
        cJSON_Delete(data);
        cJSON_Delete(chatConfig);
        cJSON_Delete(turnDetection);
        cJSON_Delete(eventSubscriptions);
        return false;
    }

    if (cJSON_AddBoolToObject(chatConfig, "auto_save_history", true) == nullptr ||
        cJSON_AddStringToObject(chatConfig, "user_id", "voice_cloud_llm_device") == nullptr ||
        !cJSON_AddItemToObject(data, "chat_config", chatConfig)) {
        cJSON_Delete(data);
        cJSON_Delete(chatConfig);
        cJSON_Delete(turnDetection);
        cJSON_Delete(eventSubscriptions);
        return false;
    }
    chatConfig = nullptr;

    if (!AddInputAudio(data, config)) {
        cJSON_Delete(data);
        cJSON_Delete(turnDetection);
        cJSON_Delete(eventSubscriptions);
        return false;
    }

    if (cJSON_AddStringToObject(turnDetection, "type", "server_vad") == nullptr ||
        cJSON_AddNumberToObject(turnDetection, "prefix_padding_ms", 600) == nullptr ||
        cJSON_AddNumberToObject(turnDetection, "silence_duration_ms", 500) == nullptr ||
        !cJSON_AddItemToObject(data, "turn_detection", turnDetection)) {
        cJSON_Delete(data);
        cJSON_Delete(turnDetection);
        cJSON_Delete(eventSubscriptions);
        return false;
    }
    turnDetection = nullptr;

    if (!AddOutputAudio(data, config)) {
        cJSON_Delete(data);
        cJSON_Delete(eventSubscriptions);
        return false;
    }

    if (!cJSON_AddItemToObject(data, "event_subscriptions", eventSubscriptions)) {
        cJSON_Delete(data);
        cJSON_Delete(eventSubscriptions);
        return false;
    }
    eventSubscriptions = nullptr;

    if (!cJSON_AddItemToObject(root, "data", data)) {
        cJSON_Delete(data);
        return false;
    }

    return true;
}

VoiceCloudLlmServerEventType EventTypeFromString(const char *eventType)
{
    if (eventType == nullptr) {
        return VOICE_CLOUD_LLM_EVENT_NONE;
    }
    if (strcmp(eventType, "chat.created") == 0) {
        return VOICE_CLOUD_LLM_EVENT_SESSION_CREATED;
    }
    if (strcmp(eventType, "chat.updated") == 0) {
        return VOICE_CLOUD_LLM_EVENT_SESSION_UPDATED;
    }
    if (strcmp(eventType, "input_audio_buffer.speech_started") == 0) {
        return VOICE_CLOUD_LLM_EVENT_SPEECH_STARTED;
    }
    if (strcmp(eventType, "input_audio_buffer.speech_stopped") == 0) {
        return VOICE_CLOUD_LLM_EVENT_SPEECH_STOPPED;
    }
    if (strcmp(eventType, "input_audio_buffer.completed") == 0) {
        return VOICE_CLOUD_LLM_EVENT_INPUT_AUDIO_DONE;
    }
    if (strcmp(eventType, "conversation.audio.delta") == 0) {
        return VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA;
    }
    if (strcmp(eventType, "conversation.message.delta") == 0) {
        return VOICE_CLOUD_LLM_EVENT_TEXT_DELTA;
    }
    if (strcmp(eventType, "conversation.audio.completed") == 0) {
        return VOICE_CLOUD_LLM_EVENT_AUDIO_STREAM_COMPLETED;
    }
    if (strcmp(eventType, "conversation.chat.completed") == 0) {
        return VOICE_CLOUD_LLM_EVENT_CHAT_COMPLETED;
    }
    if (strcmp(eventType, "error") == 0 ||
        strcmp(eventType, "conversation.chat.failed") == 0) {
        return VOICE_CLOUD_LLM_EVENT_SERVER_ERROR;
    }
    return VOICE_CLOUD_LLM_EVENT_NONE;
}

void CopyStringPayload(const char *payload, VoiceCloudLlmServerEvent *event)
{
    if (payload == nullptr || event == nullptr) {
        return;
    }

    // JSON payloads live in a bounded static buffer because cJSON memory is freed before return.
    size_t payloadLen = strlen(payload);
    if (payloadLen >= JSON_DELTA_BUFFER_SIZE) {
        payloadLen = JSON_DELTA_BUFFER_SIZE - 1U;
    }
    memcpy(g_jsonDeltaBuffer, payload, payloadLen);
    g_jsonDeltaBuffer[payloadLen] = '\0';
    event->payload = g_jsonDeltaBuffer;
    event->payloadLen = static_cast<uint32_t>(payloadLen);
}

void CopyDeltaPayload(cJSON *root, VoiceCloudLlmServerEvent *event)
{
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    cJSON *delta = cJSON_GetObjectItemCaseSensitive(data, "delta");
    if (!cJSON_IsString(delta) || delta->valuestring == nullptr) {
        return;
    }

    CopyStringPayload(delta->valuestring, event);
}

uint32_t Base64DecodedMaxLen(const char *text)
{
    if (text == nullptr) {
        return 0;
    }
    const size_t textLen = strlen(text);
    if (textLen == 0 || (textLen % 4U) != 0) {
        return 0;
    }

    uint32_t padding = 0;
    if (text[textLen - 1U] == '=') {
        ++padding;
    }
    if (textLen > 1U && text[textLen - 2U] == '=') {
        ++padding;
    }
    return static_cast<uint32_t>((textLen / 4U) * 3U - padding);
}

void CopyContentPayload(cJSON *root, VoiceCloudLlmServerEvent *event)
{
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");
    cJSON *content = cJSON_GetObjectItemCaseSensitive(data, "content");
    if (!cJSON_IsString(content) || content->valuestring == nullptr) {
        return;
    }

    const uint32_t maxDecodedLen = Base64DecodedMaxLen(content->valuestring);
    if (maxDecodedLen == 0) {
        return;
    }

    if (maxDecodedLen > JSON_DELTA_BUFFER_SIZE) {
        uint8_t *payload = static_cast<uint8_t *>(malloc(maxDecodedLen));
        if (payload == nullptr) {
            return;
        }

        uint32_t decodedLen = 0;
        if (DecodeBase64(content->valuestring, payload, maxDecodedLen, &decodedLen)) {
            event->payload = payload;
            event->payloadLen = decodedLen;
            event->payloadAllocated = true;
            return;
        }
        free(payload);
        return;
    }

    uint32_t decodedLen = 0;
    if (DecodeBase64(content->valuestring, g_jsonDeltaBuffer, JSON_DELTA_BUFFER_SIZE, &decodedLen)) {
        event->payload = g_jsonDeltaBuffer;
        event->payloadLen = decodedLen;
    }
}

} // namespace

extern "C" bool VoiceCloudLlmBuildSessionStart(const VoiceCloudLlmConfig *config, char *buffer, uint32_t bufferLen)
{
    if (config == nullptr || buffer == nullptr) {
        return false;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    const bool ok = cJSON_AddStringToObject(root, "id", "voice_cloud_llm_session") != nullptr &&
        AddEventType(root, "chat.update") &&
        AddSessionData(root, config) &&
        CopyJsonToBuffer(root, buffer, bufferLen);
    cJSON_Delete(root);
    return ok;
}

extern "C" bool VoiceCloudLlmBuildAudioCommit(char *buffer, uint32_t bufferLen)
{
    if (buffer == nullptr) {
        return false;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    const bool ok = AddEventType(root, "input_audio_buffer.complete") &&
        CopyJsonToBuffer(root, buffer, bufferLen);
    cJSON_Delete(root);
    return ok;
}

extern "C" bool VoiceCloudLlmBuildAudioAppend(const uint8_t *data, uint32_t len, char *buffer, uint32_t bufferLen)
{
    if (data == nullptr || len == 0 || buffer == nullptr) {
        return false;
    }

    const uint32_t encodedLen = Base64EncodedLen(len);
    char *encoded = static_cast<char *>(cJSON_malloc(encodedLen + 1U));
    if (encoded == nullptr) {
        return false;
    }
    if (!EncodeBase64(data, len, encoded, encodedLen + 1U)) {
        cJSON_free(encoded);
        return false;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON *payload = cJSON_CreateObject();
    bool ok = root != nullptr && payload != nullptr &&
        AddEventType(root, "input_audio_buffer.append") &&
        cJSON_AddStringToObject(payload, "delta", encoded) != nullptr;
    if (ok) {
        ok = cJSON_AddItemToObject(root, "data", payload);
        if (ok) {
            payload = nullptr;
            ok = CopyJsonToBuffer(root, buffer, bufferLen);
        }
    }
    cJSON_Delete(root);
    cJSON_Delete(payload);
    cJSON_free(encoded);
    return ok;
}

extern "C" bool VoiceCloudLlmBuildCancel(char *buffer, uint32_t bufferLen)
{
    if (buffer == nullptr) {
        return false;
    }

    cJSON *root = cJSON_CreateObject();
    if (root == nullptr) {
        return false;
    }

    const bool ok = AddEventType(root, "response.cancel") &&
        CopyJsonToBuffer(root, buffer, bufferLen);
    cJSON_Delete(root);
    return ok;
}

extern "C" VoiceCloudLlmServerEvent VoiceCloudLlmParseServerMessage(const uint8_t *data, uint32_t len, bool binary)
{
    VoiceCloudLlmServerEvent event = {
        VOICE_CLOUD_LLM_EVENT_NONE,
        nullptr,
        0,
        false,
    };

    if (data == nullptr || len == 0) {
        return event;
    }

    if (binary) {
        event.type = VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA;
        event.payload = data;
        event.payloadLen = len;
        return event;
    }

    cJSON *root = cJSON_ParseWithLength(reinterpret_cast<const char *>(data), len);
    if (root == nullptr) {
        return event;
    }

    cJSON *eventType = cJSON_GetObjectItemCaseSensitive(root, "event_type");
    if (cJSON_IsString(eventType)) {
        event.type = EventTypeFromString(eventType->valuestring);
        if (event.type == VOICE_CLOUD_LLM_EVENT_TEXT_DELTA) {
            CopyDeltaPayload(root, &event);
        } else if (event.type == VOICE_CLOUD_LLM_EVENT_AUDIO_DELTA) {
            CopyContentPayload(root, &event);
        }
    }

    cJSON_Delete(root);
    return event;
}

extern "C" void VoiceCloudLlmFreeServerEvent(VoiceCloudLlmServerEvent *event)
{
    if (event == nullptr) {
        return;
    }
    if (event->payloadAllocated && event->payload != nullptr) {
        free(const_cast<uint8_t *>(event->payload));
    }
    event->payload = nullptr;
    event->payloadLen = 0;
    event->payloadAllocated = false;
}
