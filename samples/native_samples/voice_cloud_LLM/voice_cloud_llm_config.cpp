/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM config impl: runtime auth set and precheck. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm_config.h"
#include "voice_cloud_llm_errcode.h"

#include "cmsis_os2.h"

#include <cstdio>
#include <cstring>

#ifndef VOICE_CLOUD_LLM_DEFAULT_WS_URL
#define VOICE_CLOUD_LLM_DEFAULT_WS_URL "wss://ws.coze.cn/v1/chat"
#endif

#ifndef VOICE_CLOUD_LLM_DEFAULT_AUTH_TOKEN
#define VOICE_CLOUD_LLM_DEFAULT_AUTH_TOKEN ""
#endif

#ifndef VOICE_CLOUD_LLM_DEFAULT_BOT_ID
#define VOICE_CLOUD_LLM_DEFAULT_BOT_ID ""
#endif

#define VC_LOG(fmt, ...) \
    std::printf("[t=%010u] " fmt, osKernelGetTickCount(), ##__VA_ARGS__)

static VoiceCloudLlmConfig g_voiceCloudLlmConfig = {
    VOICE_CLOUD_LLM_DEFAULT_WS_URL,
    VOICE_CLOUD_LLM_DEFAULT_AUTH_TOKEN,
    VOICE_CLOUD_LLM_DEFAULT_BOT_ID,
    VOICE_CLOUD_LLM_INPUT_SAMPLE_RATE,
    VOICE_CLOUD_LLM_OUTPUT_SAMPLE_RATE,
    VOICE_CLOUD_LLM_DEFAULT_CHANNELS,
    VOICE_CLOUD_LLM_DEFAULT_BITS,
    VOICE_CLOUD_LLM_UPLINK_FRAME_MS,
    VOICE_CLOUD_LLM_DOWNLINK_FRAME_MS,
    VOICE_CLOUD_LLM_DOWNLINK_FRAME_NUM,
};

extern "C" const VoiceCloudLlmConfig *VoiceCloudLlmGetConfig(void)
{
    return &g_voiceCloudLlmConfig;
}

static bool VoiceCloudLlmCopyConfigString(char *dst, uint32_t dstLen, const char *src)
{
    if (dst == nullptr || dstLen == 0 || src == nullptr) {
        return false;
    }
    const size_t srcLen = std::strlen(src);
    if (srcLen == 0 || srcLen >= dstLen) {
        return false;
    }
    (void)std::memcpy(dst, src, srcLen + 1U);
    return true;
}

extern "C" errcode_t VoiceCloudLlmSetAuth(const char *botId, const char *authToken)
{
    char newBotId[VOICE_CLOUD_LLM_BOT_ID_MAX_LEN] = {};
    char newAuthToken[VOICE_CLOUD_LLM_AUTH_MAX_LEN] = {};

    if (!VoiceCloudLlmCopyConfigString(newBotId, sizeof(newBotId), botId) ||
        !VoiceCloudLlmCopyConfigString(newAuthToken, sizeof(newAuthToken), authToken)) {
        VC_LOG("[voice_cloud_llm] config auth invalid: botIdLen=%u tokenLen=%u\r\n",
            botId == nullptr ? 0U : static_cast<uint32_t>(std::strlen(botId)),
            authToken == nullptr ? 0U : static_cast<uint32_t>(std::strlen(authToken)));
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }

    (void)std::memcpy(g_voiceCloudLlmConfig.botId, newBotId, sizeof(newBotId));
    (void)std::memcpy(g_voiceCloudLlmConfig.authToken, newAuthToken, sizeof(newAuthToken));
    VC_LOG("[voice_cloud_llm] config auth updated: botId='%s' tokenLen=%u\r\n",
        g_voiceCloudLlmConfig.botId, static_cast<uint32_t>(std::strlen(g_voiceCloudLlmConfig.authToken)));
    return ERRCODE_SUCC;
}

extern "C" errcode_t VoiceCloudLlmPrecheckConfig(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr) {
        VC_LOG("[voice_cloud_llm] precheck: config is NULL\r\n");
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    if (config->wsUrl[0] == '\0') {
        VC_LOG("[voice_cloud_llm] precheck: wsUrl is empty\r\n");
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    const bool isWss = std::strncmp(config->wsUrl, "wss://", 6) == 0;
    const bool isWs = std::strncmp(config->wsUrl, "ws://", 5) == 0;
    if (!isWss && !isWs) {
        VC_LOG("[voice_cloud_llm] precheck: wsUrl must start with ws:// or wss:// (got '%s')\r\n",
            config->wsUrl);
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    if (config->botId[0] == '\0') {
        VC_LOG("[voice_cloud_llm] precheck: botId is empty, set it by AT+MEDIA=VOICECLOUDLLMCONFIG\r\n");
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    if (config->authToken[0] == '\0') {
        VC_LOG("[voice_cloud_llm] precheck: authToken is empty, set it by AT+MEDIA=VOICECLOUDLLMCONFIG\r\n");
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    if (config->inputSampleRate == 0 || config->outputSampleRate == 0 ||
        config->channels == 0 || config->bitsPerSample == 0 || config->uplinkFrameMs == 0) {
        VC_LOG("[voice_cloud_llm] precheck: audio params invalid "
                    "(in=%u out=%u ch=%u bits=%u frameMs=%u)\r\n",
            config->inputSampleRate, config->outputSampleRate, config->channels,
            config->bitsPerSample, config->uplinkFrameMs);
        return ERRCODE_VOICE_CLOUD_LLM_CONFIG_INVALID;
    }
    VC_LOG("[voice_cloud_llm] precheck ok: wsUrl='%s' botId='%s' tokenLen=%u in=%u out=%u\r\n",
        config->wsUrl, config->botId, static_cast<uint32_t>(std::strlen(config->authToken)),
        config->inputSampleRate, config->outputSampleRate);
    return ERRCODE_SUCC;
}
