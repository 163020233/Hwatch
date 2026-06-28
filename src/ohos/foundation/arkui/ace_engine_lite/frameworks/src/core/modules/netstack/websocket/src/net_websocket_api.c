/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <signal.h>
#include "net_websocket_debug.h"
#include "net_websocket_string.h"
#include "net_websocket_thread.h"
#include "net_websocket_opts.h"
#include "net_websocket_priv.h"
#include "net_websocket.h"
#include "net_websocket_frame.h"

static const char *PATH_START = "/";

int WebSocketClientConstructor(WebSocketOnOpenCallback onOpen, WebSocketOnMessageCallback onMessage,
    WebSocketOnErrorCallback onError, WebSocketOnCloseCallback onClose)
{
    int client = AllocWsClient();
    if (client < 0) {
        return WEBSOCKET_CLIENT_NULL;
    }

    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NULL;
    }
    clientPtr->onOpen = onOpen;
    clientPtr->onMessage = onMessage;
    clientPtr->onError = onError;
    clientPtr->onClose = onClose;

    return client;
}

static void InnerParseUri(char *p, const char **prot, const char **ads, int *port, const char **path)
{
    const char *end;
    char unixSkt = 0;

    /* cut up the location into address, port and path */
    *prot = p;
    while (*p && (*p != ':' || p[1] != '/' || p[2] != '/')) { /* 1, 2: index  */
        p++;
    }
    if (!*p) {
        end = p;
        p = (char *)*prot;
        *prot = end;
    } else {
        *p = '\0';
        p += 3; /* 3: offset */
    }
    if (*p == '+') {
        unixSkt = 1;
    } /* unix skt */

    *ads = p;
    if (!strcmp(*prot, "http") || !strcmp(*prot, "ws")) {
        *port = 80; /* 80: http port */
    } else if (!strcmp(*prot, "https") || !strcmp(*prot, "wss")) {
        *port = 443; /* 443: https port */
    }
    if (*p == '[') {
        ++(*ads);
        while (*p && *p != ']') {
            p++;
        }
        if (*p) {
            *p++ = '\0';
        }
    } else {
        while (*p && *p != ':' && (unixSkt || *p != '/')) {
            p++;
        }
    }

    if (*p == ':') {
        *p++ = '\0';
        *port = strtol(p, NULL, 10); /* 10 : base */
        while (*p && *p != '/') {
            p++;
        }
    }
    *path = "/";
    if (*p) {
        *path = p;
    }
}

static void WsFreeUrlBuffer(struct WebSocketClient *clientPtr, char *tempUrl)
{
    if (clientPtr->host != NULL) {
        free(clientPtr->host);
        clientPtr->host = NULL;
    }

    if (clientPtr->url != NULL) {
        free(clientPtr->url);
        clientPtr->url = NULL;
    }

    if (clientPtr->path != NULL) {
        free(clientPtr->path);
        clientPtr->path = NULL;
    }

    if (tempUrl != NULL) {
        free(tempUrl);
    }
}

static bool WsParseUrl(char *url, struct WebSocketClient *clientPtr)
{
    char *tempPrefix = NULL;
    char *tempAddress = NULL;
    char *tempPath = NULL;
    char *hostEnd  = NULL;
    int urlLen = strlen(url);
    int port;
    int tempUrlLen = urlLen * 2;
    int hostLen = 0;

    char *tempUrl = malloc(tempUrlLen);
    if (tempUrl == NULL) {
        return NULL;
    }
    (void)memset_s(tempUrl, tempUrlLen, 0, tempUrlLen);

    if (strcpy_s(tempUrl, tempUrlLen, url) < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy_s failed\n"));
        goto ERROR;
    }
    clientPtr->url = malloc(urlLen + 1);
    if (clientPtr->url == NULL) {
        goto ERROR;
    }
    (void)memset_s(clientPtr->url, urlLen + 1, 0, urlLen + 1);
    clientPtr->host = malloc(urlLen + 1);
    if (clientPtr->host == NULL) {
        goto ERROR;
    }
    (void)memset_s(clientPtr->host, urlLen + 1, 0, urlLen + 1);
    clientPtr->path = malloc(urlLen + 1);
    if (clientPtr->path == NULL) {
        goto ERROR;
    }
    (void)memset_s(clientPtr->path, urlLen + 1, 0, urlLen + 1);

    (void)InnerParseUri(tempUrl, (const char **)&tempPrefix, (const char **)&tempAddress, &(clientPtr->port),
        (const char **)&tempPath);
    if (strcpy_s(clientPtr->scheme, WS_MAX_SCHEME_LEN, tempPrefix) < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy_s failed"));
        goto ERROR;
    }
    if (strcpy_s(clientPtr->url, urlLen + 1, url) < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy_s failed"));
        goto ERROR;
    }

    hostEnd = tempAddress;
    while(*hostEnd && *hostEnd != '/') {
        hostEnd++;
    }
    hostLen = hostEnd - tempAddress;
    if (strcpy_s(clientPtr->host, urlLen + 1, tempAddress) < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy_s failed"));
        goto ERROR;
    }
    clientPtr->host[hostLen] = '\0';
    if (strcpy_s(clientPtr->path, urlLen + 1, tempPath) < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy_s failed"));
        goto ERROR;
    }
    free(tempUrl);
    return true;
ERROR:
    WsFreeUrlBuffer(clientPtr, tempUrl);
    return false;
}

static void WebSocketCopyOption(const char *option, struct WebSocketClient *clientPtr)
{
    if (option == NULL) {
        return;
    }
    size_t optionLen = strlen(option);
    if (optionLen < 2 || option[optionLen - 2] != '\r' || /* 2: \r\n length */
        option[optionLen - 1] != '\n') { /* 2: \r\n length */
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("set connect option fail, option must end with '\\r\\n'\n"));
        return;
    }

    if (clientPtr->requestOption != NULL) {
        free(clientPtr->requestOption);
        clientPtr->requestOption = NULL;
    }

    clientPtr->requestOption = (char *)malloc(optionLen + 1);
    if (clientPtr->requestOption == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("malloc failed\n"));
        return;
    }
    (void)memset_s(clientPtr->requestOption, optionLen + 1, 0, optionLen + 1);

    if (strncpy_s(clientPtr->requestOption, optionLen + 1, option, optionLen) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strcpy failed\n"));
        free(clientPtr->requestOption);
        clientPtr->requestOption = NULL;
        return;
    }
}

int WebSocketClientConnect(int client, const char *url, const char *option)
{
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL || url == NULL || strlen(url) == 0) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }

    if (WebSocketClientIsConnected(client)) {
        return WEBSOCKET_OK;
    }

    if (clientPtr->workThread != NULL) {
        return WEBSOCKE_ERROR_CONNECTION_NOT_CLOSED;
    }

    if (!WsParseUrl((char *)url, clientPtr)) {
        return WEBSOCKET_CONNECTION_PARSE_URL_ERROR;
    }
    WebSocketCopyOption(option, clientPtr);

    return WsCreateStartWorkThread(clientPtr);
}

int WebSocketClientSendText(int client, const char *text, size_t length)
{
    int ret;
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    if (!clientPtr->isConnected || clientPtr->socket == WS_INVALID_SOCKET) {
        return WEBSOCKET_NO_CONNECTION;
    }
    if (text == NULL || strlen(text) == 0 || length != strlen(text)) {
        return WEBSOCKET_SEND_DATA_NULL;
    }
    if (strlen(text) > WS_MAX_DATA_LENGTH || length > WS_MAX_DATA_LENGTH) {
        return WEBSOCKET_DATA_LENGTH_EXCEEDED;
    }

    WsMutexLock(clientPtr->sendMutex);
    ret = WsSocketSendTextPriv(clientPtr, text);
    WsMutexUnLock(clientPtr->sendMutex);
    return ret;
}

static int ClientSendBinInner(int client, const char *data, size_t length, WsBinary binType)
{
    int ret;
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    if (!clientPtr->isConnected || clientPtr->socket == WS_INVALID_SOCKET) {
        return WEBSOCKET_NO_CONNECTION;
    }
    if (data == NULL || length == 0) {
        return WEBSOCKET_SEND_DATA_NULL;
    }
    if (length > WS_MAX_DATA_LENGTH) {
        return WEBSOCKET_DATA_LENGTH_EXCEEDED;
    }

    WsMutexLock(clientPtr->sendMutex);
    ret = WsClientSendBinPriv(clientPtr, data, length, binType);
    WsMutexUnLock(clientPtr->sendMutex);
    return ret;
}

int WebSocketClientSendBinStart(int client, const char *data, size_t length)
{
    return ClientSendBinInner(client, data, length, WS_BINARY_START);
}

int WebSocketClientSendBinContinue(int client, const char *data, size_t length)
{
    return ClientSendBinInner(client, data, length, WS_BINARY_CONTINUE);
}

int WebSocketClientSendBinEnd(int client, const char *data, size_t length)
{
    return ClientSendBinInner(client, data, length, WS_BINARY_END);
}

int WebSocketClientSendBin(int client, const char *data, size_t length)
{
    return ClientSendBinInner(client, data, length, WS_BINARY);
}

int WebSocketClientClose(int client)
{
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    (void)WsClientIdleDataSend(clientPtr);
    WebSocketClientClosePriv(clientPtr);
    return WEBSOCKET_OK;
}

bool WebSocketClientIsConnected(int client)
{
    bool ret;
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return false;
    }
    WsMutexLock(clientPtr->sendMutex);
    ret = clientPtr->isConnected;
    WsMutexUnLock(clientPtr->sendMutex);
    return ret;
}

int WebsocketSuspend(int client)
{
    int ret;
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    return WsThreadSuspend(clientPtr->workThread);
}

int WebsocketResume(int client)
{
    int ret;
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    return WsThreadResume(clientPtr->workThread);
}

#ifdef WEBSOCKET_SSL_ENABLE

int WebSocketClientSetServerCert(int client, const char *serverCert, int serverCertLen)
{
    struct WebSocketClient *clientPtr = WsGetClient(client);
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NOT_CREATED;
    }
    clientPtr->serverCert = serverCert;
    clientPtr->serverCertLen = serverCertLen;
    return WEBSOCKET_OK;
}
#endif
