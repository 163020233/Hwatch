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

#include "net_websocket_string.h"
#include "net_websocket_thread.h"

#include "net_websocket_frame.h"
#include "net_websocket_type.h"
#include "net_websocket_debug.h"
#include "net_websocket_opts.h"
#include "net_websocket_priv.h"

#define WS_CONNECT_RETRY_DELAY 200
#define WS_CONNECT_RETRY_TIMES 2
#define WSS_HANDSHAKE_TIMEOUT 20000

#define WSAEWOULDBLOCK EAGAIN
#define WSAEINPROGRESS EINPROGRESS

static struct WebSocketClient *g_wsClient[WS_MAX_CLIENT_NUM] = {0};

static const char *SOCKET_MIN_HTTP_VER = "1.1";
static const char *SEC_WEBSOCKET_ACCEPT = "sec-websocket-accept";

static void WsSocketClose(struct WebSocketClient *clientPtr);

static void WsDeleteAllFramesInList(WsList *listWithFrames);

static void WsSocketSetOption(WsSocket s, int option, int value);

static int WsSslConn(struct WebSocketClient *clientPtr);

static void WsAppendRecvdFrames(struct WebSocketClient *clientPtr, WsFrame *frame);

static int WsSslClose(struct WebSocketClient *clientPtr);

static void WebsocketClientRelease(struct WebSocketClient *clientPtr);

static void WsProcessReceivedFrame(struct WebSocketClient *clientPtr, WsFrame *frame);

/**
 * Map a externally used websocket client to the internal client representation.
 *
 * @param fd externally used websocket client index
 * @return struct WebSocketClient for the client or NULL if not found
 */
struct WebSocketClient *WsGetClient(int fd)
{
    if (fd < 0 || fd >= WS_MAX_CLIENT_NUM) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("get_client(%d): invalid\n", fd));
        return NULL;
    }
    return g_wsClient[fd];
}

/**
 * Allocate a new wsclient.
 * @return fd for the client; -1 on error
 */
int AllocWsClient(void)
{
    int fd = 0;

    for (; fd < WS_MAX_CLIENT_NUM; fd++) {
        if (g_wsClient[fd] == NULL) {
            break;
        }
    }
    if (fd >= WS_MAX_CLIENT_NUM) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("current client num is larger than max(%d)\n", WS_MAX_CLIENT_NUM));
        return WS_INVALID_SOCKET;
    }
    struct WebSocketClient *client = (struct WebSocketClient *)calloc(1, sizeof(struct WebSocketClient));
    if (client == NULL) {
        return WS_INVALID_SOCKET;
    }

    client->recvBuffer = (char *)malloc(WEBSOCKET_RECV_BUFF_SIZE);
    if (client->recvBuffer == NULL) {
        if (client->workMutex != NULL) {
            WsMutexDelete(client->workMutex);
        }
        if (client->sendMutex != NULL) {
            WsMutexDelete(client->sendMutex);
        }
         free(client);
        return WS_INVALID_SOCKET;
    }
    client->recvBufferSize = WEBSOCKET_RECV_BUFF_SIZE;

    client->port = -1;
    client->socket = WS_INVALID_SOCKET;
    client->command = COMMAND_NONE;

    // 初始化 wakeup event
    (void)osal_event_init(&client->wakeupEvent);

    client->workMutex = WsMutexCreateRecursive();
    client->sendMutex = WsMutexCreateRecursive();
    WsInitFrameRandom(client);
    client->clientfd = fd;

    g_wsClient[fd] = client;
    return fd;
}

static void WsInformErrorMessage(struct WebSocketClient *clientPtr, int errorCode, const char *reason)
{
    if (clientPtr->onError != NULL) {
        clientPtr->errorResult.errorCode = errorCode;
        clientPtr->errorResult.errorMessage = reason;
        clientPtr->onError(clientPtr->clientfd, clientPtr->errorResult);
    }
}

void WsSocketInformRecvdFrames(struct WebSocketClient *clientPtr)
{
    WsFrame *frame = NULL;
    WsNode *cur = clientPtr->recvdFrames;
    while (cur) {
        frame = (WsFrame *)cur->value.object;
        if (frame == NULL) {
            cur = cur->next;
            continue;
        }
        switch (frame->opcode) {
            case WS_OPCODE_CONTINUATION:
            case WS_OPCODE_TEXT_FRAME:
            case WS_OPCODE_BINARY_FRAME:
                if (clientPtr->onMessage != NULL) {
                    clientPtr->onMessage(clientPtr->clientfd,
                        (const char *)frame->data,
                        (unsigned int)frame->length,
                        frame->isFinished,
                        (frame->opcode == WS_OPCODE_BINARY_FRAME));
                }
                break;
            case WS_OPCODE_PONG:
                break;
            default:
                break;
        }
        WsDeleteFrame(frame);
        cur->value.object = NULL;
        cur = cur->next;
    }
    WsListDeleteClean(&clientPtr->recvdFrames);
}

void WsReadHandshakeResponceValue(const char *str, char **value)
{
    if (str == NULL || value == NULL) {
        return;
    }
    
    const char *s = NULL;
    size_t len = 0;

    while (*str == ':' || *str == ' ') {
        str++;
    }
    s = str;
    while (*s != '\r' && *s != '\n') {
        s++;
        len++;
    }
    if (len > 0) {
        *value = WsStringCopyLen(str, len);
    }
}

static void WsProcessReceivedContent(struct WebSocketClient *clientPtr)
{
    WsFrame *frame = NULL;
    size_t nframeSize;

    while (clientPtr->receivedLen > 0) {
        if (clientPtr->received == NULL) {
            break;
        }
        nframeSize = WsCheckRecvFrameSize(clientPtr->received, clientPtr->receivedLen);
        if (nframeSize == 0) {
            break;
        }

        frame = WsFrameCreateWithRecvData(clientPtr, clientPtr->received, nframeSize);
        if (frame != NULL) {
            WsProcessReceivedFrame(clientPtr, frame);
        } else {
            break;
        }

        if (nframeSize == clientPtr->receivedLen) {
            clientPtr->receivedLen = 0;
        } else if (clientPtr->receivedLen > nframeSize) {
            size_t nLeftLen = clientPtr->receivedLen - nframeSize;
            if (memmove_s((char *)clientPtr->received,
                clientPtr->receivedLen,
                (char *)clientPtr->received + nframeSize,
                nLeftLen) != 0) {
                WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, memmove fail\n", __FUNCTION__));
            };
            clientPtr->receivedLen = nLeftLen;
        } else {
            break;
        }
    }
}

static void WsRemoveHttpHeader(struct WebSocketClient *clientPtr)
{
    char *data = NULL;
    size_t headerLen = 0;
    char *crlfPtr = strstr((char *)clientPtr->received, "\r\n\r\n");
    if (crlfPtr == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("not find header end\n"));
        return;
    }
    data = crlfPtr + 4; /* 4 : length of \r\n\r\n */
    headerLen = (uintptr_t)(void *)data - (uintptr_t)(void *)clientPtr->received;

    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("header len=%d, received len=%d\n", headerLen, clientPtr->receivedLen));
    if ((size_t)headerLen >= clientPtr->receivedLen) {
        return;
    }

    if (memmove_s(
        (char *)clientPtr->received, clientPtr->receivedLen, (char *)data, clientPtr->receivedLen - headerLen) !=
        0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, memmove fail\n", __FUNCTION__));
    };
    clientPtr->receivedLen -= headerLen;
    WsProcessReceivedContent(clientPtr);
}

bool WsProcessHandshakeResponce(struct WebSocketClient *clientPtr)
{
    const char *str = (const char *)clientPtr->received;
    const char *sub = NULL;
    const char *acceptStr = NULL;

    int httpCode = -1;

    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("header:%s\n", str));

    sub = strstr(str, "HTTP/");
    if (!sub) {
        return false;
    }
    sub += 9; /* 9: string offset */
    if (sscanf_s(sub, "%d", &httpCode) != 1) {
        httpCode = -1;
    }

    sub = strcasestr(str, SEC_WEBSOCKET_ACCEPT);
    if (sub) {
        acceptStr = SEC_WEBSOCKET_ACCEPT;
    }
    if (sub) {
        sub += strlen(acceptStr);
        WsReadHandshakeResponceValue(sub, &clientPtr->secWsAccept);
    }
    if (httpCode != 101 || clientPtr->secWsAccept == NULL) { /* 101 : http err code */
        WsInformErrorMessage(clientPtr,
            WEBSOCKET_ERROR_PARSE_HANDSHAKE,
            "HTTP code non 101 or Accept key not found"); /* 101 : http err code */
        return false;
    }
    return true;
}

static int WsCheckWriteError(struct WebSocketClient *clientPtr, int errorNum)
{
    int socketCode = 0, code = 0;
    socklen_t socketCodeSize = sizeof(socketCode);

    if (clientPtr->socket != WS_INVALID_SOCKET) {
        if (getsockopt(clientPtr->socket, SOL_SOCKET, SO_ERROR, &socketCode, &socketCodeSize) != 0) {
            socketCode = 0;
        }
    } else {
        return WEBSOCKET_CONNECTION_ERROR;
    }

    code = (socketCode > 0) ? socketCode : errorNum;
    if (code <= 0) {
        return WEBSOCKET_OK;
    }

    switch (code) {
        case EACCES:        // Permission denied.
        case EBADF:         // Socket descriptor invalid.
        case ECONNRESET:    // Connection reset.
        case EDESTADDRREQ:  // The socket is not connection-mode, and the destination address is not set.
        case EFAULT:        // The pointer to the user space address is invalid.
        case EINTR:         // The data transmission is interrupted by signals.
        case EINVAL:        // Invalid argument.
        case EISCONN:       // The current socket is connected.
        case EMSGSIZE:    // The data size exceeds the socket sending limit and cannot be sent atomically.
        case ENOBUFS:     // The network port output queue is full.
        case ENOMEM:      // Out of memory.
        case ENOTCONN:    // The socket is not connected.
        case ENOTSOCK:    // The parameter sockfd is an invalid socket.
        case EOPNOTSUPP:  // Some flags are not supported by the socket type.
        case EPIPE:       // The local connection has been closed.
        case ECONNREFUSED:  // The remote host has rejected the connection..
            WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_READ_WRITE_SOCKET, strerror(code));
            return code;
        default:
            break;
    }
    return WEBSOCKET_OK;
}

static bool WsSocketSend(struct WebSocketClient *clientPtr, const uint8_t *data, const size_t length)
{
    int sended = -1, errorNum = -1;
    int ret;
    uint32_t writtenLen = 0;

    while (writtenLen < length) {
#ifdef WEBSOCKET_SSL_ENABLE
        if (strcmp(clientPtr->scheme, "wss") == 0) {
            if (clientPtr->ssl) {
                sended = mbedtls_ssl_write(
                    &(clientPtr->ssl->sslCtx), (const unsigned char *)(data + writtenLen), (int)(length - writtenLen));
            } else {
                WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s:%d ssl invalid ctx\n", __func__, __LINE__));
                sended = -1;
                break;
            }
        } else
#endif
            sended = (int)send(clientPtr->socket, (data + writtenLen), (int)(length - writtenLen), 0);

        if (sended > 0) {
            writtenLen += (size_t)sended;
        } else if (errno == EAGAIN) {
            usleep(30000); // 30 ms
        } else {
            errorNum = errno;
            break;
        }
    }

    if (sended > 0) {
        if (sended < (int)length) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s %d sended %d but want %u", __func__, __LINE__, sended, length));
        }
        return true;
    }

    ret = WsCheckWriteError(clientPtr, errorNum);
    if (ret != WEBSOCKET_OK) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("WsSocketSend fail, return %d, error:%s\n", errorNum, strerror(errorNum)));
        WsSocketClose(clientPtr);
    }

    return false;
}

void WsSocketResizeReceived(struct WebSocketClient *clientPtr, const size_t needSize)
{
    void *res = NULL;
    size_t min = 0;
    size_t size = WS_MAX_THRESH_VAL;

    if (needSize == clientPtr->receivedSize) {
        return;
    }

    if (needSize > WS_MAX_THRESH_VAL) {
        size = needSize;
    }

    res = malloc(size);
    if (res == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, malloc fail\n", __FUNCTION__));
        return;
    }
    min = (clientPtr->receivedSize < size) ? clientPtr->receivedSize : size;
    if (min > 0 && clientPtr->received != NULL) {
        if (memcpy_s(res, size, clientPtr->received, min) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, memcpy fail\n", __FUNCTION__));
        }
    }
    free(clientPtr->received);
    clientPtr->received = res;
    clientPtr->receivedSize = size;
}

static int WsInnerReadSocket(struct WebSocketClient *clientPtr, char *buffer, int bufferSize)
{
    int length = 0;
    char *received = NULL;
#ifdef WEBSOCKET_SSL_ENABLE
    if (strcmp(clientPtr->scheme, "wss") == 0) {
        if (clientPtr->ssl != NULL) {
            length = mbedtls_ssl_read(&(clientPtr->ssl->sslCtx), (unsigned char *)buffer, bufferSize);
        } else {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s %d ssl ctx is null\n", __func__, __LINE__));
            return -1;
        }
    } else
#endif
    {
        length = (int)recv(clientPtr->socket, buffer, bufferSize, MSG_DONTWAIT);
    }

    if (length > 0) {
        if (clientPtr->receivedSize < (size_t)length + clientPtr->receivedLen) {
            WsSocketResizeReceived(clientPtr, clientPtr->receivedSize + length);
        }
        received = (char *)clientPtr->received;
        if (clientPtr->receivedLen > 0) {
            received += clientPtr->receivedLen;
        }
        if (memcpy_s(received, (clientPtr->receivedSize - clientPtr->receivedLen), buffer, length) != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, memcpy fail\n", __func__));
            return -1;
        }
        clientPtr->receivedLen += (size_t)length;
    }
    return length;
}

static bool WsSocketRecv(struct WebSocketClient *clientPtr)
{
    int errorNum, len;
    char *buffer = NULL;
    int bufferSize = 0;
    int sslStatus = 0;
    bool isReading = true;

    buffer = clientPtr->recvBuffer;
    bufferSize = clientPtr->recvBufferSize;
    if (buffer == NULL || bufferSize <= 0) {
        return false;
    }
    (void)memset_s(buffer, bufferSize, 0, bufferSize);
    while (isReading) {
        len = WsInnerReadSocket(clientPtr, buffer, bufferSize);
        errorNum = errno;
        if (len > 0) {
            continue;
        } else if (len == 0) {
            isReading = false;
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("recv return 0 disconnected\n"));
        } else {
            isReading = false;
#ifdef WEBSOCKET_SSL_ENABLE
            if (strcmp(clientPtr->scheme, "wss") != 0) {
                break;
            }
            if (MBEDTLS_ERR_SSL_WANT_READ == len || MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET == len) {
                isReading = true;
                continue;
            }
            if (MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY == len) {
                WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("peer close -0x%04x", -len));
            } else if ((MBEDTLS_ERR_SSL_TIMEOUT == len) || (MBEDTLS_ERR_SSL_CONN_EOF == len) ||
                       (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == len) || (MBEDTLS_ERR_SSL_NON_FATAL == len)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
                sslStatus = 1;
            } else {
                /* Connection error */
                WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("recv -0x%04x\n", -len));
            }
#endif
        }
    }

    if (errorNum != WSAEWOULDBLOCK && errorNum != WSAEINPROGRESS && sslStatus != 1) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("in close websocket %d,error:%s\n", errorNum, strerror(errorNum)));
        WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_READ_WRITE_SOCKET, "Failed read/write socket");
        WsSocketClose(clientPtr);
        return false;
    }
    return true;
}

WsFrame *WsLastUnfinRecvdFrameByOpcode(struct WebSocketClient *clientPtr, const WsOpcode opcode)
{
    WsFrame *last = NULL;
    WsFrame *frame = NULL;
    WsNode *cur = clientPtr->recvdFrames;
    while (cur) {
        frame = (WsFrame *)cur->value.object;
        if (frame != NULL) {
            if (!frame->isFinished) {
                last = frame;
            }
        }
        cur = cur->next;
    }
    return last;
}

static int WsClientAppendSendFrames(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    int ret = WEBSOCKET_OK;
    WsNodeValue frameListVar;
    frameListVar.object = frame;
    if (clientPtr->sendFrames) {
        ret = WsListAppend(clientPtr->sendFrames, frameListVar);
    } else {
        clientPtr->sendFrames = WsListCreate();
        if (clientPtr->sendFrames == NULL) {
            return WEBSOCKET_NO_MEMORY;
        }
        clientPtr->sendFrames->value = frameListVar;
    }

    // 唤醒工作线程处理新发送帧
    osal_event_write(&clientPtr->wakeupEvent, RWS_EVENT_SEND_READY);

    return ret;
}

static void WsProcessBinOrTextFrame(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    WsFrame *lastUnfin = WsLastUnfinRecvdFrameByOpcode(clientPtr, frame->opcode);
    if (lastUnfin) {
        WsFrameCombineDatas(lastUnfin, frame);
        lastUnfin->isFinished = frame->isFinished;
        WsDeleteFrame(frame);
    } else if (frame->data && frame->length) {
        WsAppendRecvdFrames(clientPtr, frame);
    } else if (frame->length == 0 && frame->isFinished) {
        WsAppendRecvdFrames(clientPtr, frame);
    } else {
        WsDeleteFrame(frame);
    }
}

static void WsProcessPingFrame(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    WsFrame *pongFrame = WsFrameCreate(clientPtr);
    if (pongFrame == NULL) {
        return;
    }
    pongFrame->opcode = WS_OPCODE_PONG;
    pongFrame->isMasked = true;
    WsMutexLock(clientPtr->sendMutex);
    WsFrameFillWithSendData(pongFrame, frame->data, frame->length, true);
    WsDeleteFrame(frame);
    int ret = WsClientAppendSendFrames(clientPtr, pongFrame);
    if (ret != WEBSOCKET_OK) {
        WsDeleteFrame(pongFrame);
    }
    WsMutexUnLock(clientPtr->sendMutex);
}

static void WsProcessConnCloseFrame(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_CONNECT_CLOSED, "Connection was closed by endpoint");
    WsDeleteFrame(frame);
}

static void WsProcessReceivedFrame(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("%s: frame->opcode:%d\n", __FUNCTION__, frame->opcode));
    switch (frame->opcode) {
        case WS_OPCODE_PING:
            WsProcessPingFrame(clientPtr, frame);
            break;
        case WS_OPCODE_PONG:
            WsDeleteFrame(frame);
            break;
        case WS_OPCODE_TEXT_FRAME:
        case WS_OPCODE_BINARY_FRAME:
        case WS_OPCODE_CONTINUATION:
            WsProcessBinOrTextFrame(clientPtr, frame);
            break;
        case WS_OPCODE_CONNECTION_CLOSE:
            WsProcessConnCloseFrame(clientPtr, frame);
            break;
        default:
            // unprocessed => delete
            WsDeleteFrame(frame);
            break;
    }
}

void WsSocketIdleRecv(struct WebSocketClient *clientPtr)
{
    if (!WsSocketRecv(clientPtr)) {
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
        return;
    }
    WsProcessReceivedContent(clientPtr);
	// 检查 SSL 缓冲区是否有残留数据
#ifdef WEBSOCKET_SSL_ENABLE
	// 如果 SSL 缓冲区仍有数据，继续读取处理
	while (clientPtr->isConnected && strcmp(clientPtr->scheme, "wss") == 0 && clientPtr->ssl) {
        if (mbedtls_ssl_check_pending(&clientPtr->ssl->sslCtx) <= 0) {
            break;  // SSL 缓冲区已清空
        }
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("%s, mbedtls_ssl_check_pending ret > 0\n", __FUNCTION__));
        if (!WsSocketRecv(clientPtr)) {
            clientPtr->command = COMMAND_INFORM_DISCONNECTED;
            return;
        }
        WsProcessReceivedContent(clientPtr);
    }
#endif
}

int WsClientIdleDataSend(struct WebSocketClient *clientPtr)
{
    WsNode *cur = NULL;
    bool sending = true;
    WsFrame *frame = NULL;
    if (clientPtr->command == COMMAND_END) {
        return WEBSOCKET_OK;
    }
    WsMutexLock(clientPtr->sendMutex);
    cur = clientPtr->sendFrames;
    if (cur == NULL) {
        WsMutexUnLock(clientPtr->sendMutex);
        return WEBSOCKET_SEND_DATA_NULL;
    }
    while (cur && clientPtr->isConnected && sending) {
        frame = (WsFrame *)cur->value.object;
        cur->value.object = NULL;
        if (frame != NULL) {
            sending = WsSocketSend(clientPtr, frame->data, frame->length);
        }
        WsDeleteFrame(frame);
        cur = cur->next;
    }
    WsDeleteAllFramesInList(clientPtr->sendFrames);
    WsListDeleteClean(&clientPtr->sendFrames);
    if (!sending) {
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    }
    WsMutexUnLock(clientPtr->sendMutex);
    return WEBSOCKET_OK;
}

static void WsWaitHandshakeResponse(struct WebSocketClient *clientPtr)
{
    if (!WsSocketRecv(clientPtr)) {
        // sock already closed
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
        return;
    }

    if (clientPtr->receivedLen == 0) {
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("%s, receive length == 0\n", __FUNCTION__));
        return;
    }
    if (WsProcessHandshakeResponce(clientPtr)) {
        WsRemoveHttpHeader(clientPtr);
        clientPtr->isConnected = true;
        clientPtr->command = COMMAND_INFORM_CONNECTED;
    } else {
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    }
    clientPtr->receivedLen = 0;
}

static void WsSocketSendDisconnect(struct WebSocketClient *clientPtr)
{
    char buff[16] = {0}; /* 16: close buff length */
    size_t len = 0;
    uint16_t code = 1000; /* 1000: close ok code */
    WsFrame *frame = NULL;
    bool ret;

    if (clientPtr->socket == WS_INVALID_SOCKET) {
        return;
    }
    frame = WsFrameCreate(clientPtr);
    if (frame == NULL) {
        return;
    }
    frame->isMasked = true;
    frame->opcode = WS_OPCODE_CONNECTION_CLOSE;

    code = htons(code);
    if (memcpy_s(buff, sizeof(buff), &code, sizeof(code)) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, memcpy fail\n", __FUNCTION__));
        return;
    }
    len += sizeof(code);
    WsMutexLock(clientPtr->sendMutex);
    WsFrameFillWithSendData(frame, (const uint8_t *)buff, len, true);
    ret = WsSocketSend(clientPtr, frame->data, frame->length);
    if (!ret) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("%s, socket send fail\n", __FUNCTION__));
    }
    WsMutexUnLock(clientPtr->sendMutex);
    WsDeleteFrame(frame);
    clientPtr->command = COMMAND_END;
    WsThreadMsleep(WS_CONNECT_RETRY_DELAY);  // little bit wait after send message
}

static void WsCopyCustomOption(struct WebSocketClient *clientPtr, char *ptr, int ptrLen, size_t *writed)
{
    if (clientPtr->requestOption == NULL) {
        return;
    }
    int length = strlen(clientPtr->requestOption);
    if (length == 0) {
        return;
    }
    if (strncpy_s(ptr + *writed, ptrLen - *writed, clientPtr->requestOption, length) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("strncpy_s fail"));
        return;
    }
    *writed += length;
}

#define WS_HEADER_LEN 512
static char *WsSocketCreateHeader(struct WebSocketClient *clientPtr, int *length)
{
    int protocalLen = WS_HEADER_LEN + strlen(clientPtr->url) + strlen(clientPtr->host);
    char *ptr = NULL;
    if (clientPtr->requestOption != NULL && strlen(clientPtr->requestOption) > 0) {
        protocalLen += strlen(clientPtr->requestOption);
    }
    if (protocalLen > WS_MAX_DATA_LENGTH) {
        return NULL;
    }
    *length = protocalLen;
    ptr = (char *)malloc(protocalLen);
    if (ptr == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("malloc fail\n"));
        return NULL;
    }
    (void)memset_s(ptr, protocalLen, 0, protocalLen);
    return ptr;
}

static void WsSocketSendHandshakeInner(struct WebSocketClient *clientPtr, char *ptr, int buffLen)
{
    if (WsSocketSend(clientPtr, (const uint8_t *)ptr, buffLen)) {
        clientPtr->command = COMMAND_WAIT_HANDSHAKE_RESPONSE;
    } else {
        WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_SEND_HANDSHAKE, "Send handshake");
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    }
}

static void WsSocketSendHandshake(struct WebSocketClient *clientPtr)
{
    int ret;
    int protocolLen;
    char *ptr = WsSocketCreateHeader(clientPtr, &protocolLen);
    if (ptr == NULL) {
        return;
    }

    char *protocol = (clientPtr->secWsProtocol ? clientPtr->secWsProtocol : "chat, superchat");
    int writed = sprintf_s(ptr, protocolLen, "GET %s HTTP/%s\r\n", clientPtr->path, SOCKET_MIN_HTTP_VER);
    if (writed < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("sprintf fail"));
        goto ERROR;
    }
    if (clientPtr->port == 80) { /* 80: http port */
        ret = sprintf_s(ptr + writed, protocolLen - writed, "Host: %s\r\n", clientPtr->host);
    } else {
        ret =
            sprintf_s(ptr + writed, protocolLen - writed, "Host: %s:%i\r\n", clientPtr->host, clientPtr->port);
    }
    if (ret < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("sprintf fail"));
        goto ERROR;
    }
    writed += ret;

    ret = sprintf_s(ptr + writed, protocolLen - writed,
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Origin: %s://%s\r\n", clientPtr->scheme, clientPtr->host);
    if (ret < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("sprintf fail"));
        goto ERROR;
    }
    writed += ret;
    WsCopyCustomOption(clientPtr, ptr, protocolLen, (size_t *)(&writed));
    ret = sprintf_s(ptr + writed, protocolLen - writed, "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Protocol: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n"
        "\r\n",
        "dGhlIHNhbXBsZSBub25jZQ==", protocol);
    if (ret < 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("sprintf fail"));
        goto ERROR;
    }
    writed += ret;
    WsSocketSendHandshakeInner(clientPtr, ptr, writed);
ERROR:
    free(ptr);
}

static struct addrinfo *WsConnectGetaddrInfo(struct WebSocketClient *clientPtr)
{
    struct addrinfo hints;
    char portstr[16]; /* 16: addr info length */
    struct addrinfo *result = NULL;
    int ret = 0, retryNumber = 0;

    if (clientPtr == NULL) {
        return NULL;
    }

    ret = sprintf_s(portstr, sizeof(portstr), "%i", clientPtr->port);
    if (ret < 0) {
        return NULL;
    }
    while (retryNumber < WS_CONNECT_RETRY_TIMES) {
        result = NULL;
        (void)memset_s(&hints, sizeof(hints), 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        ret = getaddrinfo(clientPtr->host, portstr, &hints, &result);
        if (ret == 0 && result != NULL) {
            return result;
        }

        WEBSOCKET_DEBUGF(
            WS_ERR_LEVEL, ("websoc getaddrinfo failed! ret=0x%x, errno=%d, reason=%s\n", ret, errno, strerror(errno)));

        if (result) {
            freeaddrinfo(result);
        }

        WsThreadMsleep(WS_CONNECT_RETRY_DELAY);
        retryNumber++;
    }

    WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_CONNECT_TO_HOST, "getaddr info fail");
    clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    return NULL;
}

static void WsConnectToHost(struct WebSocketClient *clientPtr)
{
    struct addrinfo *result = NULL;
    struct addrinfo *p = NULL;
    WsSocket sock = WS_INVALID_SOCKET;
    int retryNumber = 0;

    if (clientPtr == NULL) {
        return;
    }
    result = WsConnectGetaddrInfo(clientPtr);
    if (result == NULL) {
        return;
    }

    while ((++retryNumber < WS_CONNECT_RETRY_TIMES) && (sock == WS_INVALID_SOCKET)) {
        for (p = result; p != NULL; p = p->ai_next) {
            sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sock == WS_INVALID_SOCKET) {
                WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("socket init failed!,errno=%d,reason:%s", errno, strerror(errno)));
                break;
            }
            WsSocketSetOption(sock, SO_ERROR, 1);
            WsSocketSetOption(sock, SO_KEEPALIVE, 1);  // Periodically test if connection is alive
            if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
                clientPtr->receivedLen = 0;
                clientPtr->socket = sock;
                fcntl(clientPtr->socket, F_SETFL, O_NONBLOCK);
                break;
            }
            WS_SOCK_CLOSE(sock);
            sock = WS_INVALID_SOCKET;
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("websoc connect failed!,errno=%d,reason:%s", errno, strerror(errno)));
        }

        if (sock == WS_INVALID_SOCKET) {
            WsThreadMsleep(WS_CONNECT_RETRY_DELAY);
        }
    }
    freeaddrinfo(result);

    if (clientPtr->socket == WS_INVALID_SOCKET) {
        WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_CONNECT_TO_HOST, "Failed connect to host");
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    } else {
        clientPtr->command = COMMAND_SEND_HANDSHAKE;
    }
}

// 接收监控线程：阻塞等待 socket 可读，唤醒工作线程
static void WsRecvMonitorTheadFunc(void *userObject)
{
    struct WebSocketClient *clientPtr = (struct WebSocketClient *)userObject;
    fd_set readFds;
    struct timeval tv;
    int ret;

    while (clientPtr->command < COMMAND_END) {
        if (clientPtr->socket == WS_INVALID_SOCKET) {
            WsThreadMsleep(WS_MONITOR_SLEEP_MS);
            continue;
        }

        FD_ZERO(&readFds);
        FD_SET(clientPtr->socket, &readFds);

        // 使用超时，避免线程无法退出
        tv.tv_sec = WS_SELECT_TIMEOUT_S;
        tv.tv_usec = 0;

        ret = select(clientPtr->socket + 1, &readFds, NULL, NULL, &tv);
        if (ret > 0 && clientPtr->socket != WS_INVALID_SOCKET) {
            // socket 有数据可读，唤醒工作线程
            osal_event_write(&clientPtr->wakeupEvent, RWS_EVENT_RECV_READY);
        }
#ifdef WEBSOCKET_SSL_ENABLE
        else if (ret == 0 && clientPtr->isConnected && clientPtr->ssl) {
            // select 超时但 SSL 内部缓冲区可能有已解密的残留数据
            if (mbedtls_ssl_check_pending(&clientPtr->ssl->sslCtx) > 0) {
                osal_event_write(&clientPtr->wakeupEvent, RWS_EVENT_RECV_READY);
            }
        }
#endif
        // ret == 0: 超时，继续循环
        // ret < 0: select 出错（如 socket 被关闭），继续循环检查
        WsThreadMsleep(WS_MONITOR_SLEEP_MS);
    }

    return;
}

// 使用OS Event 实现低功耗等待
// 监控线程负责 socket 可读事件，发送/断连由其他路径触发
static void WsIdleWait(struct WebSocketClient *clientPtr, uint32_t timeoutMs)
{
    int ret;
    // 直接使用 OS Event 挂起线程等待唤醒
    // 监控线程负责 socket 可读事件，发送/断连由其他路径触发
    ret = osal_event_read(&clientPtr->wakeupEvent, RWS_EVENT_ALL, timeoutMs, OSAL_WAITMODE_OR | OSAL_WAITMODE_CLR);
    if (ret == OSAL_FAILURE) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Websocket osal_event_read failed.\n"));
    }
    return;
}

static void WsSocketDelete(struct WebSocketClient *clientPtr)
{
    WsSocketClose(clientPtr);

    // 销毁OS wakeup event
    osal_event_destroy(&clientPtr->wakeupEvent);

    if (clientPtr->received != NULL) {
        free(clientPtr->received);
        clientPtr->received = NULL;
    }

    clientPtr->receivedSize = 0;
    clientPtr->receivedLen = 0;

    WsMutexLock(clientPtr->workMutex);
    WsDeleteAllFramesInList(clientPtr->sendFrames);
    WsListDeleteClean(&clientPtr->sendFrames);
    WsDeleteAllFramesInList(clientPtr->recvdFrames);
    WsListDeleteClean(&clientPtr->recvdFrames);
    WsMutexUnLock(clientPtr->workMutex);

    WsStringDeleteClean(&clientPtr->secWsProtocol);
    WsStringDeleteClean(&clientPtr->secWsAccept);

    WsMutexDelete(clientPtr->workMutex);
    clientPtr->workMutex = NULL;

    WsMutexDelete(clientPtr->sendMutex);
    clientPtr->sendMutex = NULL;

    if (clientPtr->recvBuffer != NULL) {
        free(clientPtr->recvBuffer);
    }
    clientPtr->recvBuffer = NULL;
    clientPtr->recvBufferSize = 0;
    if (clientPtr->host != NULL) {
        free(clientPtr->host);
        clientPtr->host = NULL;
    }

    if (clientPtr->url != NULL) {
        free(clientPtr->url);
        clientPtr->url = NULL;
    }

    if (clientPtr->requestOption != NULL) {
        free(clientPtr->requestOption);
        clientPtr->requestOption = NULL;
    }
}

static void WsSocketWorkLoop(struct WebSocketClient *clientPtr)
{
    switch (clientPtr->command) {
        case COMMAND_INFORM_CONNECTED: {
            clientPtr->command = COMMAND_IDLE;
            if (clientPtr->onOpen) {
                clientPtr->onOpen(clientPtr->clientfd);
            }
            break;
        }
        case COMMAND_INFORM_DISCONNECTED: {
            clientPtr->command = COMMAND_END;
            WsSocketSendDisconnect(clientPtr);
            if (clientPtr->onClose) {
                clientPtr->onClose(clientPtr->clientfd, clientPtr->errorResult);
            }
            break;
        }
        case COMMAND_IDLE: {
            if (clientPtr->recvdFrames) {
                WsSocketInformRecvdFrames(clientPtr);
            }
            break;
        }
        default: {
            break;
        }
    }
}

static void WsSocketWorkFunc(void *userObject)
{
    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("enter WsSocketWorkFunc\n"));
    struct WebSocketClient *clientPtr = (struct WebSocketClient *)userObject;
    if (clientPtr == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("client is null, thread start fail\n"));
        return;
    }

    clientPtr->isRunning = true;
    while (clientPtr->command < COMMAND_END) {

        if (!clientPtr->isRunning && clientPtr->isConnected) {
            clientPtr->command = COMMAND_INFORM_DISCONNECTED;
        } else if (!clientPtr->isRunning) {
            clientPtr->command = COMMAND_END;
        }

        WsMutexLock(clientPtr->workMutex);
        switch (clientPtr->command) {
            case COMMAND_CONNECT_TO_HOST: {
#ifdef WEBSOCKET_SSL_ENABLE
                if (strcmp(clientPtr->scheme, "wss") == 0) {
                    WsSslConn(clientPtr);
                    break;
                } else
#endif /* WEBSOCKET_SSL_ENABLE */
                {
                    WsConnectToHost(clientPtr);
                    break;
                }
            }
            case COMMAND_SEND_HANDSHAKE:
                WsSocketSendHandshake(clientPtr);
                break;
            case COMMAND_WAIT_HANDSHAKE_RESPONSE:
                WsWaitHandshakeResponse(clientPtr);
                break;
            case COMMAND_IDLE:
                if (clientPtr->isConnected) {
                    WsClientIdleDataSend(clientPtr);
                }

                if (clientPtr->isConnected) {
                    WsSocketIdleRecv(clientPtr);
                }
                break;
            default:
                break;
        }

        WsMutexUnLock(clientPtr->workMutex);
        WsSocketWorkLoop(clientPtr);
        if (clientPtr->command == COMMAND_IDLE &&
            clientPtr->recvMonitor != NULL) {
            WsIdleWait(clientPtr, WS_IDLE_WAIT_MS);  // 阻塞等待，进入低功耗
        } else {
            WsThreadMsleep(WS_THREAD_SLEEP_MS);
        }
    }
 
    WsSocketClose(clientPtr);
    WebsocketClientRelease(clientPtr);
    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("end WsSocketWorkFunc\n"));
}

int WsCreateStartWorkThread(struct WebSocketClient *clientPtr)
{
    WsThread workThread;
    if (clientPtr == NULL) {
        return WEBSOCKET_CLIENT_NULL;
    }

    clientPtr->command = COMMAND_NONE;
    clientPtr->workThread = WsThreadCreate(&WsSocketWorkFunc, clientPtr, "WebsocketThread");
    if (clientPtr->workThread != NULL) {
        clientPtr->recvMonitor = WsThreadCreate(&WsRecvMonitorTheadFunc, clientPtr, "WsRevMonitor");
        if (clientPtr->recvMonitor == NULL) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("WsThreadCreate WsRevMonitor fail\n"));
        }
        clientPtr->command = COMMAND_CONNECT_TO_HOST;
        return WEBSOCKET_OK;
    }

    return WEBSOCKET_CONNECTION_ERROR;
}

static void WsSocketClose(struct WebSocketClient *clientPtr)
{
    clientPtr->receivedLen = 0;
    if (clientPtr->socket != WS_INVALID_SOCKET) {
#ifdef WEBSOCKET_SSL_ENABLE
        if (strcmp(clientPtr->scheme, "wss") == 0) {
            WsSslClose(clientPtr);
        } else
#endif /* WEBSOCKET_SSL_ENABLE */
        {
            WS_SOCK_CLOSE(clientPtr->socket);
        }
        clientPtr->socket = WS_INVALID_SOCKET;
    }
    clientPtr->isConnected = false;
}

static void WsAppendRecvdFrames(struct WebSocketClient *clientPtr, WsFrame *frame)
{
    WsNodeValue frameListVar;
    frameListVar.object = frame;

    if (clientPtr->recvdFrames != NULL) {
        if (WsListAppend(clientPtr->recvdFrames, frameListVar) != 0) {
            WsDeleteFrame(frame);
        }
    } else {
        clientPtr->recvdFrames = WsListCreate();
        if (clientPtr->recvdFrames == NULL) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("recvdFrames malloc fail\n"));
            WsDeleteFrame(frame);
            return;
        }
        if (WsListAppend(clientPtr->recvdFrames, frameListVar) != 0) {
            WsDeleteFrame(frame);
            return;
        }
    }
}

int WsSocketSendTextPriv(struct WebSocketClient *clientPtr, const char *text)
{
    WsFrame *frame = NULL;
    frame = WsFrameCreate(clientPtr);
    if (frame == NULL) {
        return WEBSOCKET_NO_MEMORY;
    }
    frame->isMasked = true;
    frame->opcode = WS_OPCODE_TEXT_FRAME;

    WsFrameFillWithSendData(frame, (const uint8_t *)text, strlen(text), true);
    int ret = WsClientAppendSendFrames(clientPtr, frame);
    if (ret != WEBSOCKET_OK) {
        WsDeleteFrame(frame);
        return -1;
    }
    return ret;
}

int WsClientSendBinPriv(struct WebSocketClient *clientPtr, const char *data, size_t length, WsBinary binType)
{
    WsFrame *frame = NULL;
    frame = WsFrameCreate(clientPtr);
    if (frame == NULL) {
        return WEBSOCKET_NO_MEMORY;
    }
    frame->isMasked = true;
    switch (binType) {
        case WS_BINARY_START:
        case WS_BINARY:
            frame->opcode = WS_OPCODE_BINARY_FRAME;
            break;
        case WS_BINARY_CONTINUE:
            frame->opcode = WS_OPCODE_CONTINUATION;
            break;
        case WS_BINARY_END:
            frame->opcode = WS_OPCODE_CONTINUATION;
            break;
        default:
            break;
    }
    WsFrameFillWithSendBinData(frame, (const uint8_t *)data, length, binType);
    if (frame->data == NULL) {
        WsDeleteFrame(frame);
        return WEBSOCKET_NO_MEMORY;
    }
    return WsClientAppendSendFrames(clientPtr, frame);
}

static void WsDeleteAllFramesInList(WsList *listWithFrames)
{
    WsFrame *frame = NULL;
    WsNode *cur = listWithFrames;
    while (cur) {
        frame = (WsFrame *)cur->value.object;
        if (frame) {
            WsDeleteFrame(frame);
        }
        cur->value.object = NULL;
        cur = cur->next;
    }
}

static void WsSocketSetOption(WsSocket s, int option, int value)
{
    setsockopt(s, SOL_SOCKET, option, (char *)&value, sizeof(int));
}

#ifdef WEBSOCKET_SSL_ENABLE
static void WsTlsNetInit(mbedtls_net_context *ctx)
{
    ctx->fd = -1;
}

static void WsTlsNetFree(mbedtls_net_context *ctx)
{
    if (ctx->fd == -1)
        return;

    shutdown(ctx->fd, SHUT_RDWR);
    closesocket(ctx->fd);

    ctx->fd = -1;
}

static int WsTlsNetSend(void *ctx, const unsigned char *buf, size_t len)
{
    int ret;

    int fd = ((mbedtls_net_context *)ctx)->fd;

    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    ret = (int)send(fd, buf, len, 0);
    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);

        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_WRITE);

        return (MBEDTLS_ERR_NET_SEND_FAILED);
    }

    return (ret);
}

static int WsTlsNetRecv(void *ctx, unsigned char *buf, size_t len)
{
    int ret;
    mbedtls_net_context *net_ctx = (mbedtls_net_context *)ctx;
    if (net_ctx == NULL) {
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    int fd = net_ctx->fd;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    ret = (int)recv(fd, buf, len, MSG_DONTWAIT);
    if (ret < 0) {
        if (errno == EPIPE || errno == ECONNRESET)
            return (MBEDTLS_ERR_NET_CONN_RESET);

        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    return (ret);
}

static int WsTlsNetRecvTimeout(void *ctx, unsigned char *buf, unsigned int len, unsigned int timeout)
{
    int ret;
    struct timeval tv;
    fd_set read_fds;
    int fd;
    mbedtls_net_context *net_ctx = (mbedtls_net_context *)ctx;

    if (net_ctx == NULL) {
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);
    }

    fd = net_ctx->fd;
    if (fd < 0)
        return (MBEDTLS_ERR_NET_INVALID_CONTEXT);

    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    tv.tv_sec = timeout / 1000;           /* 1000: ms */
    tv.tv_usec = (timeout % 1000) * 1000; /* 1000: ms */

    /* no wait if timeout == 0 */
    ret = select(fd + 1, &read_fds, NULL, NULL, &tv);
    
    if (ret == 0)
        return (MBEDTLS_ERR_SSL_TIMEOUT);

    if (ret < 0) {
        if (errno == EINTR)
            return (MBEDTLS_ERR_SSL_WANT_READ);

        return (MBEDTLS_ERR_NET_RECV_FAILED);
    }

    /* This call will not block */
    return (WsTlsNetRecv(ctx, buf, (size_t)len));
}

static void WsDebugPrint(void *ctx, int level, const char *file, int line, const char *str)
{
    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("%s\n", str));
}

static int WsSslInit(WsSsl *ssl)
{
    int ret = WEBSOCKET_OK;
    const char *pers = "wss";
#if defined(MBEDTLS_DEBUG_C) && defined(DEBUG_LEVEL)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    WsTlsNetInit(&ssl->netCtx);

    mbedtls_ssl_init(&ssl->sslCtx);
    mbedtls_ssl_config_init(&ssl->sslConf);
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
    mbedtls_pk_init(&ssl->pkey);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ret = psa_crypto_init();
    if (ret != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Failed to initialize PSA Crypto implementation: %d\n", (int)ret));
        goto exit;
    }
#endif
    mbedtls_ctr_drbg_init(&ssl->ctrDrbg);
    mbedtls_entropy_init(&ssl->entropy);
    if ((ret = mbedtls_ctr_drbg_seed(
        &ssl->ctrDrbg, mbedtls_entropy_func, &ssl->entropy, (const unsigned char *)pers,
        strlen(pers))) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ctr_drbg_seed() failed, value:-0x%x.", -ret));
        goto exit;
    }
exit:
    if (ret != WEBSOCKET_OK) {
        mbedtls_ssl_free(&ssl->sslCtx);
        mbedtls_ssl_config_free(&ssl->sslConf);
        mbedtls_x509_crt_free(&ssl->cacert);
        mbedtls_x509_crt_free(&ssl->clicert);
        mbedtls_pk_free(&ssl->pkey);
        mbedtls_ctr_drbg_free(&ssl->ctrDrbg);
        mbedtls_entropy_free(&ssl->entropy);
    }
    return ret;
}

static int WsLoadCertificate(struct WebSocketClient *clientPtr, WsSsl *ssl)
{
    int ret = WEBSOCKET_OK;
    /*
     * Load the Client certificate
     */
    if (clientPtr->clientCert && clientPtr->clientPk) {
        ret = mbedtls_x509_crt_parse(
            &ssl->clicert, (const unsigned char *)clientPtr->clientCert, clientPtr->clientCertLen);
        if (ret < 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret));
            goto exit;
        }

        ret = mbedtls_pk_parse_key(&ssl->pkey,
            (const unsigned char *)clientPtr->clientPk,
            clientPtr->clientPkLen,
            NULL,
            0,
            mbedtls_ctr_drbg_random,
            &ssl->ctrDrbg);
        if (ret != 0) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("failed! mbedtls_pk_parse_key returned -0x%x.", -ret));
            goto exit;
        }
    }

    /*
     * Load the trusted CA
     */
    if (clientPtr->serverCert &&
        ((ret = mbedtls_x509_crt_parse(
            &ssl->cacert, (const unsigned char *)clientPtr->serverCert,
            clientPtr->serverCertLen)) < 0)) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_x509_crt_parse() failed, value:-0x%x.", -ret));
    }
exit:
    if (ret != WEBSOCKET_OK) {
        if (clientPtr->clientCert && clientPtr->clientPk) {
            mbedtls_x509_crt_free(&ssl->clicert);
        }
        if (clientPtr->serverCert) {
            mbedtls_x509_crt_free(&ssl->cacert);
        }
    }
    return ret;
}

/*
 * Setup stuff
 */
static int WsSslSetup(struct WebSocketClient *clientPtr, WsSsl *ssl, int authmode)
{
    int ret;
    if ((ret = mbedtls_ssl_config_defaults(
        &ssl->sslConf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ssl_config_defaults() failed, value:-0x%x.", -ret));
        return ret;
    }
    mbedtls_ssl_conf_read_timeout(&ssl->sslConf, WSS_HANDSHAKE_TIMEOUT);

    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_MD5);
    mbedtls_ssl_conf_cert_profile(&ssl->sslConf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->sslConf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->sslConf, &ssl->cacert, NULL);

    if (clientPtr->clientCert && (ret = mbedtls_ssl_conf_own_cert(&ssl->sslConf, &ssl->clicert, &ssl->pkey)) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, (" failed! mbedtls_ssl_conf_own_cert returned %d.", ret));
        return ret;
    }

    mbedtls_ssl_conf_rng(&ssl->sslConf, mbedtls_ctr_drbg_random, &ssl->ctrDrbg);
    mbedtls_ssl_conf_dbg(&ssl->sslConf, WsDebugPrint, NULL);

    if ((ret = mbedtls_ssl_setup(&ssl->sslCtx, &ssl->sslConf)) != 0) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ssl_setup() failed, value:-0x%x.", -ret));
        return ret;
    }
    mbedtls_ssl_set_hostname(&ssl->sslCtx, clientPtr->host);
    mbedtls_ssl_set_bio(&ssl->sslCtx, &ssl->netCtx, WsTlsNetSend, WsTlsNetRecv, WsTlsNetRecvTimeout);
    return ret;
}

/*
 * Handshake
 */
static int WsSslHandshake(WsSsl *ssl)
{
    int ret = WEBSOCKET_OK;
    WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("start to handshake..."));

    while ((ret = mbedtls_ssl_handshake(&ssl->sslCtx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("mbedtls_ssl_handshake() failed, ret:-0x%x.", -ret));
            return ret;
        }
    }
    mbedtls_ssl_conf_read_timeout(&ssl->sslConf, 10); /* 10 : 10ms timeout */
    if ((ret = (int)mbedtls_ssl_get_verify_result(&ssl->sslCtx)) != 0) {
#if defined(MBEDTLS_DEBUG_C)
        char vrfy_buf[512]; /* 512 : verify len */
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("svr_cert verification failed\n"));
        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", ret);
#endif
    } else {
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("svr_cert verification ok.\n"));
    }
    return ret;
}

static int WsSslConn(struct WebSocketClient *clientPtr)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
    int value, ret = 0;
    uint32_t flags;
    WsSsl *ssl = NULL;

    clientPtr->ssl = calloc(1, sizeof(WsSsl));
    if (clientPtr->ssl == NULL) {
        WEBSOCKET_DEBUGF(WS_ERR_LEVEL, ("Memory malloc error."));
        ret = WEBSOCKET_NO_MEMORY;
        goto exit;
    }

    ssl = clientPtr->ssl;

    if (clientPtr->serverCert) {
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }

    ret = WsSslInit(ssl);
    if (ret != WEBSOCKET_OK) {
        goto exit;
    }

    ret = WsLoadCertificate(clientPtr, ssl);
    if (ret != WEBSOCKET_OK) {
        goto exit;
    }

    WsConnectToHost(clientPtr);
    if (clientPtr->socket == WS_INVALID_SOCKET) {
        return WEBSOCKET_ERROR_CONNECT_TO_HOST;
    } else {
        WEBSOCKET_DEBUGF(WS_DBG_LEVEL, ("connect to host ok\n"));
        ssl->netCtx.fd = clientPtr->socket;
    }

    ret = WsSslSetup(clientPtr, ssl, authmode);
    if (ret != WEBSOCKET_OK) {
        goto exit;
    }

    ret = WsSslHandshake(ssl);
exit:
    if (ret != 0) {
        WsInformErrorMessage(clientPtr, WEBSOCKET_ERROR_CONNECT_TO_HOST, "Failed connect to host");
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
        WsSslClose(clientPtr);
    }
    return ret;
}

static int WsSslClose(struct WebSocketClient *clientPtr)
{
    if (clientPtr == NULL) {
        return -1;
    }
    
    WsSsl *ssl = clientPtr->ssl;
    if (ssl == NULL) {
        return -1;
    }

    clientPtr->ssl = NULL;
    clientPtr->clientCert = NULL;
    clientPtr->serverCert = NULL;
    clientPtr->clientPk = NULL;

    mbedtls_ssl_close_notify(&ssl->sslCtx);
    WsTlsNetFree(&ssl->netCtx);
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
    mbedtls_pk_free(&ssl->pkey);
    mbedtls_ssl_free(&ssl->sslCtx);
    mbedtls_ssl_config_free(&ssl->sslConf);
    mbedtls_ctr_drbg_free(&ssl->ctrDrbg);
    mbedtls_entropy_free(&ssl->entropy);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_psa_crypto_free();
#endif
    free(ssl);
    return 0;
}
#endif
static void WebsocketClientRelease(struct WebSocketClient *clientPtr)
{
    WsFreeRandom(clientPtr);

    WsMutexLock(clientPtr->workMutex);

    WsDeleteAllFramesInList(clientPtr->sendFrames);
    WsListDeleteClean(&clientPtr->sendFrames);

    WsMutexUnLock(clientPtr->workMutex);

    WsSocketDelete(clientPtr);
}

void WebSocketClientClosePriv(struct WebSocketClient *clientPtr)
{
    WsThread workThread;
    WsThread recvMonitor;

    if (clientPtr == NULL) {
        return;
    }
    workThread = clientPtr->workThread;
    recvMonitor = clientPtr->recvMonitor;

    if (clientPtr->isConnected) {  // connected in loop
        clientPtr->command = COMMAND_INFORM_DISCONNECTED;
    } else if (workThread != NULL) {  // disconnected in loop
        clientPtr->command = COMMAND_END;
    }

	// 唤醒工作线程以便及时处理断连
    osal_event_write(&clientPtr->wakeupEvent, RWS_EVENT_DISCONNECT);

    if (recvMonitor != NULL) {
        osStatus_t st = osThreadJoin(recvMonitor->thread);
        if (st != osOK) {
            osThreadTerminate(recvMonitor->thread);
        }
    }

    if (workThread != NULL) {
        clientPtr->isRunning = false;
        osThreadJoin(workThread->thread);
        osThreadTerminate(workThread->thread);
    }

    if (recvMonitor != NULL) {
        free(recvMonitor->stackMem);
        free(recvMonitor);
        clientPtr->recvMonitor = NULL;
    }

    if (workThread != NULL) {
        free(workThread->stackMem);
        free(workThread);
        clientPtr->workThread = NULL;
    }
    for (int i = 0; i < WS_MAX_CLIENT_NUM; i++) {
        if (g_wsClient[i] == clientPtr) {
            g_wsClient[i] = NULL;
            break;
        }
    }
    free(clientPtr);
}
