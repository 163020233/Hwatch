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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "httpclient.h"
#include "http_opts.h"
#include "http_wrappers.h"
#if CONFIG_USE_LWIP
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#else
#include <netdb.h>
#include <sys/socket.h>
#endif
#include "securec.h"

#if CONFIG_HTTP_SECURE
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

/** @brief   This structure defines the httpclient ssl structure.  */
typedef struct {
    mbedtls_ssl_context sslCtx;       /**< mbedtls ssl context */
    mbedtls_net_context netCtx;       /**< fill in socket id   */
    mbedtls_ssl_config sslConf;       /**< entropy context     */
    mbedtls_entropy_context entropy;  /**< ssl configuration   */
    mbedtls_ctr_drbg_context ctrDrbg; /**< ctr drbg context    */
    mbedtls_x509_crt_profile profile; /**< x509 cacert profile */
    mbedtls_x509_crt cacert;          /**< x509 cacert         */
    mbedtls_x509_crt clicert;         /**< x509 client cacert  */
    mbedtls_pk_context pkey;          /**< pkey context        */
} HttpClientSsl;

#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Conncection wrapper function for HTTP
 *  @param[in] client    pointer to the httpclient.
 *  @param[in] host      host url
 *  @return    0 success other failed
 */
int HttpTcpConnWrapper(HttpClient *client, const char *host)
{
    struct addrinfo hints, *addrList, *cur;
    int ret = 0;
    char port[10] = {0};

    memset_s(&hints, sizeof(hints), 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (sprintf_s(port, sizeof(port), "%d", client->remotePort) < 0) {
        http_err("sprintf error, ret=0x%x\n", ret);
        return HTTP_ENOBUFS;
    }

    ret = getaddrinfo(host, port, &hints, &addrList);
    if (ret != 0) {
        http_err("getaddrinfo != 0, return EDNS,ret=%d,error:%s\n", ret, strerror(errno));
        return HTTP_EDNS;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = HTTP_EDNS;
    for (cur = addrList; cur != NULL; cur = cur->ai_next) {
        client->socket = (int)socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
        if (client->socket < 0) {
            ret = HTTP_ECONN;
            http_err("socket init fail, errno=%d, error:%s\n", errno, strerror(errno));
            continue;
        }

#ifdef SW22_PRODUCT_FPGA
        cur->ai_addrlen =  cur->ai_family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
#endif

        if (connect(client->socket, cur->ai_addr, (int)cur->ai_addrlen) == 0) {
            ret = 0;
            break;
        }
        http_err("socket connect fail,errno=%d, error:%s\n", errno, strerror(errno));
        closesocket(client->socket);
        client->socket = -1;
        ret = HTTP_ECONN;
    }

    freeaddrinfo(addrList);

    return ret;
}

int HttpTcpCloseWrapper(HttpClient *client)
{
    closesocket(client->socket);
    client->socket = -1;
    return 0;
}

int HttpTcpSendWrapper(HttpClient *client, const char *data, int length)
{
    int writtenLen = 0;

    while (writtenLen < length) {
        int ret = send(client->socket, data + writtenLen, length - writtenLen, 0);
        if (ret > 0) {
            writtenLen += ret;
            continue;
        } else if (ret == 0) {
            return writtenLen;
        } else {
            http_err("socket send err ret=%d, errno=%d, reason=%s\n", ret, errno, strerror(errno));
            return HTTP_ESEND; /* Connnection error */
        }
    }

    return writtenLen;
}

int HttpTcpRecvWrapper(HttpClient *client, char *buf, int buflen, int timeoutMs, int *pReadLen)
{
    int ret = 0, selectRet;
    int readLen = 0;
    struct timeval timeout;
    fd_set sets;
    int errRecord;
    int sockfd = client->socket;

    timeout.tv_sec = timeoutMs / 1000; /* 1000 : ms to s */
    timeout.tv_usec = 0;

    while (readLen < buflen) {
        FD_ZERO(&sets);
        FD_SET(sockfd, &sets);

#ifdef SW22_PRODUCT_FPGA
        selectRet = lwip_select(sockfd + 1, &sets, NULL, NULL, &timeout);
#else
        selectRet = select(sockfd + 1, &sets, NULL, NULL, &timeout);
#endif
        errRecord = errno;
        if (selectRet > 0) {
            if (0 == FD_ISSET(sockfd, &sets)) {
                ret = 0;
#ifdef HTTP_DEBUG
                http_debug("select continue");
#endif
                continue;
            }

            ret = recv(sockfd, buf + readLen, buflen - readLen, 0);
            errRecord = errno;
            if (ret < 0) {
                if ((EINTR == errRecord) || (EAGAIN == errRecord) || (EWOULDBLOCK == errRecord) ||
                    (EPROTOTYPE == errRecord) || (EALREADY == errRecord) || (EINPROGRESS == errRecord)) {
#ifdef HTTP_DEBUG
                    http_debug("recv continue %d ret %d", errRecord, ret);
#endif
                    continue;
                }
            } else if (ret == 0) {
#ifdef HTTP_DEBUG
                http_debug("recv return 0 disconnected\n");
#endif
                ret = HTTP_ECLSD;
            }
        } else if (selectRet == 0) {
            http_info("select return 0 may disconnected\n");
            ret = HTTP_ECLSD;
        } else {
#ifdef HTTP_DEBUG
            http_debug("select return %d errno %d", selectRet, errRecord);
#endif
            if (errRecord == EINTR) {
                continue;
            }

            ret = selectRet;
        }

        if (ret > 0) {
            readLen += ret;
        } else if (ret == HTTP_ECLSD) {
            break;
        } else {
            http_err(
                "socket recv error (recv returned %d, errno=%d, reason:%s)\n", ret, errRecord, strerror(errRecord));
            *pReadLen = readLen;
            return HTTP_ECONN;
        }
    }

    *pReadLen = readLen;
    if (ret == HTTP_ECLSD) {
        return ret;
    } else {
        return HTTP_SUCCESS;
    }
}

int HttpTcpSseRecv(HttpClient *client, char *buffer, int buflen, int *bytesRead)
{
    int sockfd = client->socket;
    int errRecord;
    int readLen;

    readLen = recv(sockfd, buffer, buflen - 1, 0);
    if (readLen > 0) {
        buffer[readLen] = '\0';
        *bytesRead = readLen;
    } else if (readLen == 0) {
#ifdef HTTP_DEBUG
        http_debug("sse bytes read == 0\n");
#endif
        *bytesRead = 0;
    } else {
        errRecord = errno;
        if ((EINTR == errRecord) || (EAGAIN == errRecord) || (EWOULDBLOCK == errRecord) || (EPROTOTYPE == errRecord) ||
            (EALREADY == errRecord) || (EINPROGRESS == errRecord)) {
#ifdef HTTP_DEBUG
            http_debug("recv continue %d readLen=%d", errRecord, readLen);
#endif
            return HTTP_EAGAIN;
        }
        return HTTP_ECONN;
    }
    return HTTP_SUCCESS;
}

#if CONFIG_HTTP_SECURE
int HttpSslSseRecv(HttpClient *client, char *buffer, int buflen, int *bytesRead)
{
    int readLen;
    HttpClientSsl *ssl;

    if (client == NULL || buffer == NULL || buflen <= 0) {
        return HTTP_EARG;
    }
    
    ssl = (HttpClientSsl *)client->ssl;
    if (ssl == NULL) {
        return HTTP_EARG;
    }
    mbedtls_ssl_conf_read_timeout(&ssl->sslConf, 0);
    readLen = mbedtls_ssl_read(&ssl->sslCtx, (unsigned char *)buffer, buflen);
    if (readLen == 0) {
#ifdef HTTP_DEBUG
        http_debug("sse bytes read == 0\n");
#endif
        *bytesRead = 0;
        return HTTP_SUCCESS;
    } else if (readLen < 0) {
        if ((MBEDTLS_ERR_SSL_TIMEOUT == readLen) || (MBEDTLS_ERR_SSL_CONN_EOF == readLen) ||
            (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == readLen) || (MBEDTLS_ERR_SSL_NON_FATAL == readLen)) {
            /* read already complete */
            /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
            return HTTP_EAGAIN;
        }

        if (MBEDTLS_ERR_SSL_WANT_READ == readLen || MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET == readLen) {
            return HTTP_EAGAIN;
        }

        if (readLen == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            return HTTP_ECLSD;
        }
    }

    if (readLen > 0) {
        buffer[readLen] = '\0';
        *bytesRead = readLen;
    } else {
        http_err("ssl read error (recv returned 0x%x)", -readLen);
        return HTTP_ECONN;
    }
    return HTTP_SUCCESS;
}

int HttpSslSendWrapper(HttpClient *client, const char *data, size_t length)
{
    size_t writtenLen = 0;

    HttpClientSsl *ssl = (HttpClientSsl *)client->ssl;
    if (ssl == NULL) {
        return HTTP_EARG;
    }

    while (writtenLen < length) {
        int ret = mbedtls_ssl_write(&ssl->sslCtx, (unsigned char *)(data + writtenLen), (length - writtenLen));
        if (ret > 0) {
            writtenLen += (size_t)ret;
            continue;
        } else if (ret == 0) {
            return writtenLen;
        } else {
            http_err("ssl write fail, ret=0x%x\n", -ret);
            return HTTP_ESEND; /* Connnection error */
        }
    }

    return writtenLen;
}

static void HttpClientDebug(void *ctx, int level, const char *file, int line, const char *str)
{
#ifdef HTTP_DEBUG
    http_debug("%s", str);
#endif
}

static int HttpInitSslSession(HttpClientSsl *ssl)
{
    int ret;
#ifdef MBEDTLS_ENTROPY_C
    const char *pers = "https";
#endif
#if defined(MBEDTLS_DEBUG_C) && defined(DEBUG_LEVEL)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init(&ssl->netCtx);
    mbedtls_ssl_init(&ssl->sslCtx);
    mbedtls_ssl_config_init(&ssl->sslConf);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
#endif
    mbedtls_pk_init(&ssl->pkey);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    ret = psa_crypto_init();
    if (ret != HTTP_SUCCESS) {
        http_err("Failed to initialize PSA Crypto implementation: %d\n", (int)ret);
        return HTTP_EUNKNOW;
    }
#endif
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_init(&ssl->ctrDrbg);
    mbedtls_entropy_init(&ssl->entropy);
    if ((ret = mbedtls_ctr_drbg_seed(
             &ssl->ctrDrbg, mbedtls_entropy_func, &ssl->entropy, (const unsigned char *)pers, strlen(pers))) != 0) {
        http_err("mbedtls_ctr_drbg_seed() failed, ret:-0x%x.", -ret);
        return HTTP_EUNKNOW;
    }
#endif
    return HTTP_SUCCESS;
}

static int HttpLoadCerticate(HttpClientSsl *ssl, HttpClient *client)
{
    int ret;
    int value;
    if (client->clientCert != NULL && client->clientPk != NULL) {
#ifdef MBEDTLS_X509_CRT_PARSE_C
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)client->clientCert, client->clientCertLen);
        if (ret < 0) {
            http_err("Loading cli_cert failed! mbedtls_x509_crt_parse returned -0x%x.", -ret);
            return ret;
        }
#endif
        ret = mbedtls_pk_parse_key(&ssl->pkey,
            (const unsigned char *)client->clientPk,
            client->clientPkLen,
            NULL,
            0,
            mbedtls_ctr_drbg_random,
            &ssl->ctrDrbg);
        if (ret != 0) {
            http_err("failed! mbedtls_pk_parse_key returned -0x%x.", -ret);
            mbedtls_x509_crt_free(&ssl->clicert);
            return ret;
        }
    }
    /*
     * Load the trusted CA
     */
    /* cert_len passed in is gotten from sizeof not strlen */
#ifdef MBEDTLS_X509_CRT_PARSE_C
    if (client->serverCert != NULL &&
        ((value = mbedtls_x509_crt_parse(
              &ssl->cacert, (const unsigned char *)client->serverCert, client->serverCertLen)) < 0)) {
        http_err("mbedtls_x509_crt_parse() failed, value:-0x%x.", -value);
        return HTTP_EPROTO;
    }
#endif
    return HTTP_SUCCESS;
}

static int HttpSslStartConInner(HttpClientSsl *ssl, HttpClient *client, const char *host)
{
    if (HttpTcpConnWrapper(client, host) != 0) {
        return HTTP_ECONN;
    }
#ifdef HTTP_DEBUG
    http_debug("Connected %s success...\n", host);
#endif
    ssl->netCtx.fd = client->socket;
    return HTTP_SUCCESS;
}

static int HttpSslSetupStaff(HttpClientSsl *ssl, HttpClient *client, int authmode, const char *host)
{
    int ret;
    if ((ret = mbedtls_ssl_config_defaults(
             &ssl->sslConf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        http_err("mbedtls_ssl_config_defaults() failed, ret:-0x%x.", -ret);
        return HTTP_EUNKNOW;
    }

#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_ssl_conf_authmode(&ssl->sslConf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->sslConf, &ssl->cacert, NULL);

    if (client->clientCert && (ret = mbedtls_ssl_conf_own_cert(&ssl->sslConf, &ssl->clicert, &ssl->pkey)) != 0) {
        http_err(" failed! mbedtls_ssl_conf_own_cert returned %d.", ret);
        return ret;
    }
#endif

    mbedtls_ssl_conf_rng(&ssl->sslConf, mbedtls_ctr_drbg_random, &ssl->ctrDrbg);
    mbedtls_ssl_conf_dbg(&ssl->sslConf, HttpClientDebug, NULL);

    if ((ret = mbedtls_ssl_setup(&ssl->sslCtx, &ssl->sslConf)) != 0) {
        http_err("mbedtls_ssl_setup() failed, ret:-0x%x.", -ret);
        return HTTP_EPROTO;
    }

    mbedtls_ssl_set_bio(&ssl->sslCtx, &ssl->netCtx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);
    mbedtls_ssl_set_hostname(&ssl->sslCtx, host);
    mbedtls_ssl_conf_read_timeout(&ssl->sslConf, 10000); /* 10000 : timeout */
    return HTTP_SUCCESS;
}

static int serverCertificateVerify(HttpClientSsl *ssl)
{
    int ret;
    if ((ret = (int)mbedtls_ssl_get_verify_result(&ssl->sslCtx)) != 0) {
        char vrfyBuf[512] = {0}; /* 512 : buffer length */
        http_err("svr_cert verification failed.\n");
        mbedtls_x509_crt_verify_info(vrfyBuf, sizeof(vrfyBuf), "  ! ", ret);
        return HTTP_ECONN;
    } else {
        http_info("svr_cert verification ok.\n");
    }
    return HTTP_SUCCESS;
}

int HttpSslConnWrapper(HttpClient *client, const char *host)
{
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
    int ret = 0;
    HttpClientSsl *ssl;

    client->ssl = (HttpClientSsl *)calloc(1, sizeof(HttpClientSsl));
    if (client->ssl == NULL) {
        http_err("Memory malloc error.");
        ret = HTTP_ENOBUFS;
        goto exit;
    }
    ssl = (HttpClientSsl *)client->ssl;

    if (client->serverCert != NULL) {
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
    }
    ret = HttpInitSslSession(ssl);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = HttpLoadCerticate(ssl, client);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = HttpSslStartConInner(ssl, client, host);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    ret = HttpSslSetupStaff(ssl, client, authmode, host);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }
    /*
     * Handshake
     */
    while ((ret = mbedtls_ssl_handshake(&ssl->sslCtx)) != 0) {
        if (serverCertificateVerify(ssl) != HTTP_SUCCESS) {
            goto exit;
        }
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            http_err("mbedtls_ssl_handshake() failed, ret:-0x%x.\n", -ret);
            goto exit;
        }
    }
    if (serverCertificateVerify(ssl) != HTTP_SUCCESS) {
        ret = HTTP_ECONN;
        goto exit;
    }
exit:
    if (ret != 0) {
        HttpSslCloseWrapper(client);
    }
    return ret;
}

int HttpSslCloseWrapper(HttpClient *client)
{
    HttpClientSsl *ssl = (HttpClientSsl *)client->ssl;
    if (ssl == NULL) {
        return HTTP_ECONN;
    }

    client->ssl = NULL;
    client->clientCert = NULL;
    client->serverCert = NULL;
    client->clientPk = NULL;
    client->socket = -1;

    mbedtls_ssl_close_notify(&ssl->sslCtx);
    mbedtls_net_free(&ssl->netCtx);
#ifdef MBEDTLS_X509_CRT_PARSE_C
    mbedtls_x509_crt_free(&ssl->cacert);
    mbedtls_x509_crt_free(&ssl->clicert);
#endif
    mbedtls_pk_free(&ssl->pkey);
#ifdef MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_psa_crypto_free();
#endif
    mbedtls_ssl_free(&ssl->sslCtx);
    mbedtls_ssl_config_free(&ssl->sslConf);
#ifdef MBEDTLS_ENTROPY_C
    mbedtls_ctr_drbg_free(&ssl->ctrDrbg);
    mbedtls_entropy_free(&ssl->entropy);
#endif
    free(ssl);
    ssl = NULL;
    return HTTP_SUCCESS;
}

int HttpSslRecvWrapper(HttpClient *client, char *buf, int buflen, int timeoutMs, int *pReadLen)
{
    int ret = 0;
    int readLen = 0;

    HttpClientSsl *ssl = (HttpClientSsl *)client->ssl;
    if (ssl == NULL) {
        return HTTP_ECONN;
    }

    mbedtls_ssl_conf_read_timeout(&ssl->sslConf, timeoutMs);

    while (readLen < buflen) {
        ret = mbedtls_ssl_read(&ssl->sslCtx, (unsigned char *)buf + readLen, buflen - readLen);
        if (ret == 0) {
            ret = HTTP_ECLSD;
        } else if (ret < 0) {
            if ((MBEDTLS_ERR_SSL_TIMEOUT == ret) || (MBEDTLS_ERR_SSL_CONN_EOF == ret) ||
                (MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED == ret) || (MBEDTLS_ERR_SSL_NON_FATAL == ret)) {
                /* read already complete */
                /* if call mbedtls_ssl_read again, it will return 0 (means EOF) */
            }

            if (MBEDTLS_ERR_SSL_WANT_READ == ret || MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET == ret) {
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = HTTP_ECLSD;
            }
        }

        if (ret > 0) {
            readLen += ret;
        } else if (ret == HTTP_ECLSD) {
            break;
        } else {
            http_err("ssl read error (recv returned 0x%x readLen:%d)", -ret, readLen);
            *pReadLen = readLen;
            return HTTP_ECONN;
        }
    }

    *pReadLen = readLen;

    if (ret == HTTP_ECLSD) {
        return ret;
    } else {
        return HTTP_SUCCESS;
    }
}

#endif

#ifdef __cplusplus
}
#endif