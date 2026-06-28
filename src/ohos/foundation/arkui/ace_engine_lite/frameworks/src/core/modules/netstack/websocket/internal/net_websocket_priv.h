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

#ifndef __NET_WEBSOCKET_PRIV_H__
#define __NET_WEBSOCKET_PRIV_H__

#include <sys/time.h>

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include <unistd.h>
#include <errno.h>

#include "net_websocket.h"
#include "net_websocket_type.h"
#include "net_websocket_opts.h"
#include "net_websocket_thread.h"
#include "net_websocket_list.h"

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

#ifdef WEBSOCKET_SSL_ENABLE
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#endif

#include "soc_osal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int WsSocket;
#define WS_INVALID_SOCKET -1
#define WS_SOCK_CLOSE(sock) closesocket(sock)

#define O_NONBLOCK 04000

#ifdef WEBSOCKET_SSL_ENABLE
typedef struct WsSslStruct {
    mbedtls_ssl_context sslCtx; /* mbedtls ssl context */
    mbedtls_net_context netCtx; /* Fill in socket id */
    mbedtls_ssl_config sslConf; /* SSL configuration */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctrDrbg;
    mbedtls_x509_crt_profile profile;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
} WsSsl;
#endif

#define WS_MAX_SCHEME_LEN 16

struct WebSocketClient {
    int clientfd;
    int port;
    WsSocket socket;
    osal_event wakeupEvent;   // OS event for waking up work thread
    WsThread recvMonitor;
    char scheme[WS_MAX_SCHEME_LEN];
    char *url;
    char *host;
    char *path;
    char *requestOption;

    char *secWsProtocol;  // "Sec-WebSocket-Protocol" field
    char *secWsAccept;    // "Sec-WebSocket-Accept" field from handshake

    WsThread workThread;
    bool isRunning;

    int command;

    bool isConnected;  // sock connected + handshake done

    /** Pointer to the callback invoked when a connection message is received */
    WebSocketOnOpenCallback onOpen;
    /** Pointer to the callback invoked when a message is received */
    WebSocketOnMessageCallback onMessage;
    /** Pointer to the callback invoked when an error message is received */
    WebSocketOnErrorCallback onError;
    /** Pointer to the callback invoked when a close message is received */
    WebSocketOnCloseCallback onClose;

    uint8_t *received;
    size_t receivedSize;  // size of 'received' memory
    size_t receivedLen;   // length of actualy readed message

    WsList *sendFrames;
    WsList *recvdFrames;

    WsMutex workMutex;
    WsMutex sendMutex;

    mbedtls_entropy_context randomEntropy; /* random entropy */
    mbedtls_ctr_drbg_context randomCtrDrbg;

#ifdef WEBSOCKET_SSL_ENABLE
    const char *serverCert; /**< Server certification. */
    const char *clientCert; /**< Client certification. */
    const char *clientPk;   /**< Client private key. */
    int serverCertLen;      /**< Server certification lenght, server_cert buffer size. */
    int clientCertLen;      /**< Client certification lenght, client_cert buffer size. */
    int clientPkLen;        /**< Client private key lenght, client_pk buffer size. */
    WsSsl *ssl;             /**< Ssl content. */
#endif
    char *recvBuffer;
    int recvBufferSize;

    struct WebSocketErrorResult errorResult;
};

typedef enum WsBinaryCode { WS_BINARY_START, WS_BINARY_CONTINUE, WS_BINARY_END, WS_BINARY } WsBinary;

void WebSocketClientClosePriv(struct WebSocketClient *clientPtr);

struct WebSocketClient *WsGetClient(int fd);

int AllocWsClient(void);

int WsSocketSendTextPriv(struct WebSocketClient *clientPtr, const char *text);

int WsClientSendBinStartPriv(struct WebSocketClient *clientPtr, const char *data, size_t length);

int WsClientIdleDataSend(struct WebSocketClient *clientPtr);

int WsCreateStartWorkThread(struct WebSocketClient *clientPtr);

void WsSocketInformRecvdFrames(struct WebSocketClient *clientPtr);

void WsSocketResizeReceived(struct WebSocketClient *clientPtr, const size_t needSize);

bool WsProcessHandshakeResponce(struct WebSocketClient *clientPtr);

void WsReadHandshakeResponceValue(const char *str, char **value);

void WsSocketIdleRecv(struct WebSocketClient *clientPtr);

int WsClientSendBinPriv(struct WebSocketClient *clientPtr, const char *data, size_t length, WsBinary binType);

#define COMMAND_IDLE -1
#define COMMAND_NONE 0
#define COMMAND_CONNECT_TO_HOST 1
#define COMMAND_SEND_HANDSHAKE 2
#define COMMAND_WAIT_HANDSHAKE_RESPONSE 3
#define COMMAND_INFORM_CONNECTED 4
#define COMMAND_INFORM_DISCONNECTED 5

#define COMMAND_END 9999

#define RWS_EVENT_SEND_READY  0x01   // 有新帧待发送
#define RWS_EVENT_DISCONNECT  0x02   // 断连请求
#define RWS_EVENT_RECV_READY  0x04   // socket 有数据可读（由监控线程触发）
#define RWS_EVENT_ALL         (RWS_EVENT_SEND_READY | RWS_EVENT_DISCONNECT | RWS_EVENT_RECV_READY)

#ifdef __cplusplus
}
#endif

#endif
