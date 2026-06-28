/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2026-2026. All rights reserved.
 *
 * Description: voice_cloud_LLM websocket client. \n
 *
 * History: \n
 * 2026-06-27, Create file. \n
 */

#include "voice_cloud_llm_config.h"
#include "net_websocket.h"
#include "media_thread_adapt.h"
#include "cmsis_os2.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Single-line tick-stamped printf wrapper. Matches the [t=NNNNNNNNNN] prefix
 * used by the rest of the voice_cloud_LLM logs so all components share one
 * timeline.
 */
#define VC_WS_LOG(fmt, ...) \
    printf("[t=%010u] " fmt, osKernelGetTickCount(), ##__VA_ARGS__)

typedef void (*VoiceCloudLlmWsEventCallback)(const uint8_t *data, uint32_t len, bool binary, void *ctx);
typedef struct {
    int clientId;
    bool connected;
    bool closing;
    bool openRejected;
    VoiceCloudLlmWsEventCallback callback;
    VoiceCloudLlmWsEventCallback statusCallback;
    void *callbackCtx;
    uint8_t *fragmentBuffer;
    uint32_t fragmentLen;
    uint32_t fragmentCap;
    bool fragmentBinary;
    bool fragmentActive;
} VoiceCloudLlmWsClient;

int32_t VoiceCloudLlmWsConnect(const VoiceCloudLlmConfig *config, VoiceCloudLlmWsEventCallback callback,
    VoiceCloudLlmWsEventCallback statusCallback, void *ctx);
int32_t VoiceCloudLlmWsSendText(const char *text);
int32_t VoiceCloudLlmWsSendBinary(const uint8_t *data, uint32_t len);
int32_t VoiceCloudLlmWsClose(void);

static VoiceCloudLlmWsClient g_wsClient = {-1, false, false, false, nullptr, nullptr, nullptr, nullptr, 0, 0,
    false, false};
static MediaMutexHandle g_wsMutex = nullptr;
static const uint32_t VOICE_CLOUD_LLM_WS_OPEN_POLL_MS = 10;
static const uint32_t VOICE_CLOUD_LLM_WS_FRAGMENT_MAX_LEN = 64 * 1024;
static const uint32_t VOICE_CLOUD_LLM_WS_CONNECT_URL_BUF_LEN =
    VOICE_CLOUD_LLM_WS_URL_MAX_LEN + VOICE_CLOUD_LLM_BOT_ID_MAX_LEN +
    VOICE_CLOUD_LLM_AUTH_MAX_LEN + 64;

static void VoiceCloudLlmWsLock(void)
{
    if (g_wsMutex != nullptr) {
        MediaMutexLock(g_wsMutex);
    }
}

static void VoiceCloudLlmWsUnlock(void)
{
    if (g_wsMutex != nullptr) {
        MediaMutexUnLock(g_wsMutex);
    }
}

static void VoiceCloudLlmWsEnsureMutex(void)
{
    if (g_wsMutex != nullptr) {
        return;
    }
    MediaMutexAttr mutexAttr = {false};
    g_wsMutex = MediaMutexCreate(&mutexAttr);
}

static bool VoiceCloudLlmUrlHasParam(const char *url, const char *param)
{
    if (url == nullptr || param == nullptr || param[0] == '\0') {
        return false;
    }

    const char *query = strchr(url, '?');
    if (query == nullptr) {
        return false;
    }
    ++query;

    const size_t paramLen = strlen(param);
    while (*query != '\0') {
        if (strncmp(query, param, paramLen) == 0 && query[paramLen] == '=') {
            return true;
        }
        query = strchr(query, '&');
        if (query == nullptr) {
            return false;
        }
        ++query;
    }
    return false;
}

static bool VoiceCloudLlmAppendQueryParam(char *url, uint32_t urlLen, const char *key, const char *value)
{
    if (url == nullptr || key == nullptr || value == nullptr || value[0] == '\0') {
        return true;
    }
    if (VoiceCloudLlmUrlHasParam(url, key)) {
        return true;
    }
    const size_t cur = strlen(url);
    if (cur >= urlLen) {
        return false;
    }
    const char separator = (strchr(url, '?') == nullptr) ? '?' : '&';
    const int ret = snprintf(url + cur, urlLen - cur, "%c%s=%s", separator, key, value);
    return ret > 0 && static_cast<uint32_t>(ret) < urlLen - cur;
}

static bool VoiceCloudLlmBuildConnectUrl(const VoiceCloudLlmConfig *config, char *url, uint32_t urlLen)
{
    if (config == nullptr || url == nullptr || urlLen == 0 || config->wsUrl[0] == '\0') {
        return false;
    }

    const int written = snprintf(url, urlLen, "%s", config->wsUrl);
    if (written <= 0 || static_cast<uint32_t>(written) >= urlLen) {
        return false;
    }
    if (!VoiceCloudLlmAppendQueryParam(url, urlLen, "bot_id", config->botId)) {
        return false;
    }
    return true;
}

/*
 * Coze WS authenticates via the HTTP "Authorization: Bearer <token>" header,
 * not via a URL query parameter. The netstack WebSocket client takes custom
 * headers through the option parameter (per net_websocket.h: "string for
 * websocket custom header, must end with '\r\n'").
 *
 * Returns NULL when no token is configured. Caller owns the returned buffer.
 */
static char *VoiceCloudLlmBuildConnectOption(const VoiceCloudLlmConfig *config)
{
    if (config == nullptr || config->authToken[0] == '\0') {
        return nullptr;
    }
    const bool alreadyBearer = strncmp(config->authToken, "Bearer ", 7) == 0;
    const uint32_t optionLen = VOICE_CLOUD_LLM_AUTH_MAX_LEN + 32;
    char *option = static_cast<char *>(malloc(optionLen));
    if (option == nullptr) {
        return nullptr;
    }
    const int written = snprintf(option, optionLen, "Authorization: %s%s\r\n",
        alreadyBearer ? "" : "Bearer ", config->authToken);
    if (written <= 0 || static_cast<uint32_t>(written) >= optionLen) {
        free(option);
        return nullptr;
    }
    return option;
}

static void VoiceCloudLlmWsReset(void)
{
    if (g_wsClient.fragmentBuffer != nullptr) {
        free(g_wsClient.fragmentBuffer);
    }
    g_wsClient = {-1, false, false, false, nullptr, nullptr, nullptr, nullptr, 0, 0, false, false};
}

static void VoiceCloudLlmWsAbandonOpeningClient(int clientId)
{
    VoiceCloudLlmWsLock();
    if (clientId == g_wsClient.clientId) {
        /*
         * The netstack close path joins its worker thread. During a failed
         * open that worker can still be blocked in DNS/socket connect, so a
         * synchronous close here would pin voice_cloud_LLM's worker and leave
         * early capture running. Drop only this wrapper's reference; late
         * OnOpen/OnError/OnClose callbacks are ignored as stale, and the
         * netstack worker releases itself once its connect failure unwinds.
         */
        VoiceCloudLlmWsReset();
    }
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws abandoned unopened clientId=%d\r\n", clientId);
}

static void VoiceCloudLlmWsNotifyStatus(void)
{
    if (g_wsClient.statusCallback != nullptr && !g_wsClient.closing) {
        g_wsClient.statusCallback(nullptr, 0, false, g_wsClient.callbackCtx);
    }
}

static bool VoiceCloudLlmWsWaitOpen(int clientId, uint32_t timeoutMs)
{
    uint32_t elapsedMs = 0;
    while (true) {
        VoiceCloudLlmWsLock();
        const bool clientChanged = clientId != g_wsClient.clientId;
        const bool connected = g_wsClient.connected;
        const bool rejected = g_wsClient.openRejected;
        VoiceCloudLlmWsUnlock();

        if (clientChanged || rejected) {
            return false;
        }
        if (connected) {
            return true;
        }
        if (elapsedMs >= timeoutMs) {
            return false;
        }

        uint32_t sleepMs = VOICE_CLOUD_LLM_WS_OPEN_POLL_MS;
        if (sleepMs > timeoutMs - elapsedMs) {
            sleepMs = timeoutMs - elapsedMs;
        }
        usleep(sleepMs * 1000U);
        elapsedMs += sleepMs;
    }
}

static void OnOpen(int client)
{
    VoiceCloudLlmWsLock();
    if (client != g_wsClient.clientId) {
        VoiceCloudLlmWsUnlock();
        VC_WS_LOG("[voice_cloud_llm] ws OnOpen ignored, stale clientId=%d (cur=%d)\r\n", client, g_wsClient.clientId);
        return;
    }
    g_wsClient.connected = true;
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws OnOpen, clientId=%d\r\n", client);
}

static bool VoiceCloudLlmWsAppendFragment(const char *data, size_t length)
{
    if (data == nullptr && length > 0) {
        return false;
    }
    if (length > VOICE_CLOUD_LLM_WS_FRAGMENT_MAX_LEN ||
        g_wsClient.fragmentLen > VOICE_CLOUD_LLM_WS_FRAGMENT_MAX_LEN - length) {
        return false;
    }

    const uint32_t requiredLen = g_wsClient.fragmentLen + static_cast<uint32_t>(length);
    if (requiredLen > g_wsClient.fragmentCap) {
        uint32_t newCap = (g_wsClient.fragmentCap == 0) ? 1024U : g_wsClient.fragmentCap;
        while (newCap < requiredLen) {
            newCap *= 2U;
        }
        uint8_t *newBuffer = static_cast<uint8_t *>(realloc(g_wsClient.fragmentBuffer, newCap));
        if (newBuffer == nullptr) {
            return false;
        }
        g_wsClient.fragmentBuffer = newBuffer;
        g_wsClient.fragmentCap = newCap;
    }

    if (length > 0) {
        (void)memcpy(g_wsClient.fragmentBuffer + g_wsClient.fragmentLen, data, length);
    }
    g_wsClient.fragmentLen = requiredLen;
    return true;
}

static void VoiceCloudLlmWsClearFragment(void)
{
    g_wsClient.fragmentLen = 0;
    g_wsClient.fragmentBinary = false;
    g_wsClient.fragmentActive = false;
}

static void OnMessage(int client, const char *data, size_t length, bool isFinished, bool isBinary)
{
    VoiceCloudLlmWsLock();
    if (client != g_wsClient.clientId) {
        VoiceCloudLlmWsUnlock();
        return;
    }
    if (g_wsClient.callback == nullptr || (data == nullptr && length > 0)) {
        VoiceCloudLlmWsUnlock();
        return;
    }

    if (isFinished && !g_wsClient.fragmentActive) {
        g_wsClient.callback(reinterpret_cast<const uint8_t *>(data), static_cast<uint32_t>(length), isBinary,
            g_wsClient.callbackCtx);
        VoiceCloudLlmWsUnlock();
        return;
    }

    const bool firstFragment = !g_wsClient.fragmentActive;
    if (firstFragment) {
        g_wsClient.fragmentActive = true;
        g_wsClient.fragmentBinary = isBinary;
    }
    if (!VoiceCloudLlmWsAppendFragment(data, length)) {
        VoiceCloudLlmWsClearFragment();
        VoiceCloudLlmWsNotifyStatus();
        VoiceCloudLlmWsUnlock();
        return;
    }

    if (!isFinished) {
        VoiceCloudLlmWsUnlock();
        return;
    }

    g_wsClient.callback(g_wsClient.fragmentBuffer, g_wsClient.fragmentLen, g_wsClient.fragmentBinary,
        g_wsClient.callbackCtx);
    VoiceCloudLlmWsClearFragment();
    VoiceCloudLlmWsUnlock();
}

static void OnError(int client, struct WebSocketErrorResult errorResult)
{
    VoiceCloudLlmWsLock();
    if (client != g_wsClient.clientId) {
        VoiceCloudLlmWsUnlock();
        VC_WS_LOG("[voice_cloud_llm] ws OnError ignored, stale clientId=%d (cur=%d)\r\n", client, g_wsClient.clientId);
        return;
    }
    const bool wasConnected = g_wsClient.connected;
    if (!g_wsClient.connected) {
        g_wsClient.openRejected = true;
    }
    g_wsClient.connected = false;
    VoiceCloudLlmWsClearFragment();
    VoiceCloudLlmWsNotifyStatus();
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws OnError, clientId=%d errCode=%d msg='%s' wasConnected=%d\r\n",
        client, errorResult.errorCode,
        errorResult.errorMessage != nullptr ? errorResult.errorMessage : "(null)",
        wasConnected ? 1 : 0);
}

static void OnClose(int client, struct WebSocketErrorResult errorResult)
{
    VoiceCloudLlmWsLock();
    if (client != g_wsClient.clientId) {
        VoiceCloudLlmWsUnlock();
        VC_WS_LOG("[voice_cloud_llm] ws OnClose ignored, stale clientId=%d (cur=%d)\r\n", client, g_wsClient.clientId);
        return;
    }
    const bool wasConnected = g_wsClient.connected;
    if (!g_wsClient.connected) {
        g_wsClient.openRejected = true;
    }
    g_wsClient.connected = false;
    VoiceCloudLlmWsClearFragment();
    VoiceCloudLlmWsNotifyStatus();
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws OnClose, clientId=%d errCode=%d msg='%s' wasConnected=%d\r\n",
        client, errorResult.errorCode,
        errorResult.errorMessage != nullptr ? errorResult.errorMessage : "(null)",
        wasConnected ? 1 : 0);
}

int32_t VoiceCloudLlmWsConnect(const VoiceCloudLlmConfig *config, VoiceCloudLlmWsEventCallback callback,
    VoiceCloudLlmWsEventCallback statusCallback, void *ctx)
{
    VoiceCloudLlmWsEnsureMutex();
    if (config == nullptr || config->wsUrl[0] == '\0') {
        return WEBSOCKET_MISSED_PARAM;
    }

    /*
     * Heap-allocate the URL/option buffers — they sum to ~640 bytes and this
     * function is invoked from the worker thread, which also runs the state
     * machine on the same stack. Keeping these on the heap was the fix for an
     * observed stack overflow after wakeup on the original 4 KB worker stack.
     */
    char *connectUrl = static_cast<char *>(malloc(VOICE_CLOUD_LLM_WS_CONNECT_URL_BUF_LEN));
    if (connectUrl == nullptr) {
        return WEBSOCKET_MISSED_PARAM;
    }
    if (!VoiceCloudLlmBuildConnectUrl(config, connectUrl, VOICE_CLOUD_LLM_WS_CONNECT_URL_BUF_LEN)) {
        free(connectUrl);
        return WEBSOCKET_MISSED_PARAM;
    }
    char *connectOption = VoiceCloudLlmBuildConnectOption(config);

    VoiceCloudLlmWsLock();
    const bool hasOldClient = g_wsClient.clientId >= 0;
    VoiceCloudLlmWsUnlock();
    if (hasOldClient) {
        (void)VoiceCloudLlmWsClose();
    }

    const int clientId = WebSocketClientConstructor(OnOpen, OnMessage, OnError, OnClose);
    if (clientId < 0) {
        VC_WS_LOG("[voice_cloud_llm] ws Constructor failed, ret=%d\r\n", clientId);
        free(connectUrl);
        free(connectOption);
        return clientId;
    }
    VC_WS_LOG("[voice_cloud_llm] ws connect attempt, clientId=%d url='%s' authHeader=%d\r\n",
        clientId, connectUrl, connectOption != nullptr ? 1 : 0);

    VoiceCloudLlmWsLock();
    g_wsClient.clientId = clientId;
    g_wsClient.connected = false;
    g_wsClient.closing = false;
    g_wsClient.openRejected = false;
    /*
     * Don't hook the message callback yet — if we did, server frames arriving
     * during the WaitOpen window would be dispatched to the worker before the
     * state machine is in WS_CONNECTING/CHAT_CREATED. Both callbacks are attached
     * once we know the connection succeeded.
     */
    g_wsClient.callback = nullptr;
    g_wsClient.statusCallback = nullptr;
    g_wsClient.callbackCtx = ctx;
    VoiceCloudLlmWsUnlock();

    const int ret = WebSocketClientConnect(clientId, connectUrl, connectOption);

    if (ret != WEBSOCKET_OK) {
        VC_WS_LOG("[voice_cloud_llm] ws Connect failed, clientId=%d ret=%d\r\n", clientId, ret);
        free(connectUrl);
        free(connectOption);
        VoiceCloudLlmWsLock();
        g_wsClient.closing = true;
        VoiceCloudLlmWsUnlock();
        (void)WebSocketClientClose(clientId);
        VoiceCloudLlmWsLock();
        VoiceCloudLlmWsReset();
        VoiceCloudLlmWsUnlock();
        return ret;
    }
    VC_WS_LOG("[voice_cloud_llm] ws Connect issued, clientId=%d waiting OnOpen (timeout=%ums)\r\n",
        clientId, VOICE_CLOUD_LLM_CONNECT_TIMEOUT_MS);

    if (!VoiceCloudLlmWsWaitOpen(clientId, VOICE_CLOUD_LLM_CONNECT_TIMEOUT_MS)) {
        VC_WS_LOG("[voice_cloud_llm] ws WaitOpen failed, clientId=%d (rejected or timed out)\r\n", clientId);
        free(connectUrl);
        free(connectOption);
        VoiceCloudLlmWsAbandonOpeningClient(clientId);
        return WEBSOCKET_NO_CONNECTION;
    }

    free(connectUrl);
    free(connectOption);
    VoiceCloudLlmWsLock();
    g_wsClient.callback = callback;
    g_wsClient.statusCallback = statusCallback;
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws Connect ok, clientId=%d callbacks attached\r\n", clientId);
    return WEBSOCKET_OK;
}

int32_t VoiceCloudLlmWsSendText(const char *text)
{
    if (text == nullptr) {
        return WEBSOCKET_SEND_DATA_NULL;
    }
    VoiceCloudLlmWsLock();
    const int clientId = g_wsClient.clientId;
    const bool connected = g_wsClient.connected;
    VoiceCloudLlmWsUnlock();
    if (clientId < 0 || !connected || !WebSocketClientIsConnected(clientId)) {
        return WEBSOCKET_NO_CONNECTION;
    }

    return WebSocketClientSendText(clientId, text, strlen(text));
}

int32_t VoiceCloudLlmWsSendBinary(const uint8_t *data, uint32_t len)
{
    if (data == nullptr || len == 0) {
        return WEBSOCKET_SEND_DATA_NULL;
    }
    VoiceCloudLlmWsLock();
    const int clientId = g_wsClient.clientId;
    const bool connected = g_wsClient.connected;
    VoiceCloudLlmWsUnlock();
    if (clientId < 0 || !connected || !WebSocketClientIsConnected(clientId)) {
        return WEBSOCKET_NO_CONNECTION;
    }

    return WebSocketClientSendBin(clientId, reinterpret_cast<const char *>(data), len);
}

int32_t VoiceCloudLlmWsClose(void)
{
    VoiceCloudLlmWsEnsureMutex();

    VoiceCloudLlmWsLock();
    const int clientId = g_wsClient.clientId;
    if (clientId < 0) {
        VoiceCloudLlmWsReset();
        VoiceCloudLlmWsUnlock();
        return WEBSOCKET_OK;
    }

    g_wsClient.closing = true;
    VoiceCloudLlmWsUnlock();

    VC_WS_LOG("[voice_cloud_llm] ws Close requested, clientId=%d\r\n", clientId);
    const int ret = WebSocketClientClose(clientId);

    VoiceCloudLlmWsLock();
    VoiceCloudLlmWsReset();
    VoiceCloudLlmWsUnlock();
    VC_WS_LOG("[voice_cloud_llm] ws Close done, clientId=%d ret=%d\r\n", clientId, ret);
    return ret;
}
