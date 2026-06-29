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
#include <strings.h>
#include <stdlib.h>

#include "httpclient.h"
#include "http_wrappers.h"
#include "http_form_data.h"
#include "http_opts.h"
#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_EVENT_ID_LENGTH 64
#define HTTP_LINEPTR_LEN_MIN 6
/** @brief http SSE data structure */
typedef struct {
    char *event;
    char *data;
    char eventId[HTTP_EVENT_ID_LENGTH];
} HttpSSEData;

/* base64 encode */
static void HttpClientBase64enc(char *out, const unsigned char *in)
{
    const char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    unsigned int i = 0, x = 0, l = 0;

    for (; *in; in++) {
        x = (x << 8) | *in;                         /* 8: length of byte */
        for (l += 8; l >= 6; l -= 6) {              /* 8, 6: length of byte */
            out[i++] = code[(x >> (l - 6)) & 0x3f]; /* 6, 0x3f: mask */
        }
    }
    if (l > 0) {
        x <<= 6 - l;               /* 6: offset of byte */
        out[i++] = code[x & 0x3f]; /* 0x3f: mask */
    }
    for (; i % 4;) { /* 4: mask */
        out[i++] = '=';
    }
    out[i] = '\0';
}

static int HttpGetUrlScheme(const char *url, char *scheme, size_t maxSchemeLen)
{
    char *hostPtr = NULL;
    char *schemePtr = (char *)url;
    if (url == NULL) {
        http_err("Could not find url");
        return HTTP_EPARSE;
    }

    hostPtr = (char *)strstr(url, "://");
    if (hostPtr == NULL) {
        http_err("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }

    if (maxSchemeLen < (size_t)(hostPtr - schemePtr + 1)) { /* including NULL-terminating char */
        http_err("Scheme str is too small (%u >= %d)", maxSchemeLen, hostPtr - schemePtr + 1);
        return HTTP_EPARSE;
    }

    if (memcpy_s(scheme, maxSchemeLen, schemePtr, hostPtr - schemePtr) != 0) {
        http_err("memcpy fail");
        return HTTP_EPARSE;
    }

    scheme[hostPtr - schemePtr] = '\0';
    return HTTP_SUCCESS;
}

static int HttpGetUrlPort(const char *url, int *port)
{
    char *portPtr = NULL;
    char *hostPtr = NULL;

    if (url == NULL) {
        http_err("Could not find url");
        return HTTP_EPARSE;
    }

    hostPtr = (char *)strstr(url, "://");
    if (hostPtr == NULL) {
        http_err("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }

    hostPtr += 3; /* 3: '://' length */
    portPtr = hostPtr;
    while (*portPtr && *portPtr != ':' && *portPtr != '/' && *portPtr != '?' && *portPtr != '#') {
        portPtr++;
    }
 
    if (*portPtr == ':') {
        uint16_t tport;
        portPtr++;
        if (sscanf_s(portPtr, "%hu", &tport) != 1) {
            http_err("Could not find port");
            return HTTP_EPARSE;
        }
        *port = (int)tport;
    } else {
        *port = 0;
    }
    return HTTP_SUCCESS;
}

static int HttpParseUrlPathInner(const char *hostPtr, const char *pathPtr, char *path, size_t maxPathLen)
{
    char *fragmentPtr = NULL;
    size_t pathLen;

    if (path == NULL || maxPathLen == 0) {
        return HTTP_SUCCESS;
    }
    fragmentPtr = strchr(hostPtr, '#');
    if (fragmentPtr != NULL) {
        pathLen = fragmentPtr - pathPtr;
    } else {
        pathLen = strlen(pathPtr);
    }
    if (maxPathLen < pathLen + 1) { /* including NULL-terminating char */
        http_err("Path str is too small (%u >= %u)", maxPathLen, pathLen + 1);
        return HTTP_EPARSE;
    }
    if (memcpy_s(path, maxPathLen, pathPtr, pathLen) != 0) {
        http_err("memcpy fail\n");
        return HTTP_EPARSE;
    }
    path[pathLen] = '\0';
    return HTTP_SUCCESS;
}

/* parse url according to http[s]://host[:port][/[path]] */
static int HttpClientParseUrl(const char *url, char *host, size_t maxHostLen, char *path, size_t maxPathLen)
{
    char *hostPtr = NULL;
    size_t hostLen = 0;
    char *pathPtr = NULL;
    char *endPtr = NULL;

    hostPtr = (char *)strstr(url, "://");
    if (hostPtr == NULL) {
        http_err("Could not find host");
        return HTTP_EPARSE; /* URL is invalid */
    }
    hostPtr += 3; /* 3: '://' length */
    endPtr = hostPtr;
    while (*endPtr && *endPtr != ':' && *endPtr != '/' && *endPtr != '?' && *endPtr != '#') {
        endPtr++;
    }
    hostLen = endPtr - hostPtr;

    pathPtr = strchr(hostPtr, '/');
    if (pathPtr == NULL) {
        http_err("Could not find '/'");
        return HTTP_EPARSE;
    }

    if (maxHostLen < hostLen + 1) { /* including NULL-terminating char */
        http_err("Host str is too small (%u >= %u)", maxHostLen, hostLen + 1);
        return HTTP_EPARSE;
    }
    if (memcpy_s(host, maxHostLen, hostPtr, hostLen)) {
        http_err("memcpy fail\n");
        return HTTP_EPARSE;
    }
    host[hostLen] = '\0';

    return HttpParseUrlPathInner(hostPtr, pathPtr, path, maxPathLen);
}

static int HttpClientGetInfo(
    HttpClient *client, char *sendBuf, int sendBufSize, int *sendIdx, char *buf) /* 0 on success, err code on failure */
{
    int ret;
    int len = strlen(buf);
    int cpLen;
    int idx = *sendIdx;

    do {
        if ((HTTPCLIENT_SEND_BUF_SIZE - idx) >= len) {
            cpLen = len;
        } else {
            cpLen = HTTPCLIENT_SEND_BUF_SIZE - idx;
        }

        if (memcpy_s(sendBuf + idx, sendBufSize - idx, buf, cpLen) != 0) {
            http_err("send buffer overflow");
            return HTTP_EUNKNOW;
        }
        idx += cpLen;
        len -= cpLen;
        if (idx == HTTPCLIENT_SEND_BUF_SIZE) {
            if (client->isHttp == false) {
                http_err("send buffer overflow");
                return HTTP_EUNKNOW;
            }
            ret = HttpTcpSendWrapper(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE);
            if (ret) {
                return (ret);
            }
            idx = 0;
        }
    } while (len > 0);

    *sendIdx = idx;
    return HTTP_SUCCESS;
}

void HttpClientSetCustomHeader(HttpClient *client, char *header)
{
    if (client == NULL || header == NULL || strlen(header) < 2) { /* 2: \r\n len */
        http_err("header invalid\n");
        return;
    }
    int headerLen = strlen(header);
    if (header[headerLen - 2] == '\r' && header[headerLen - 1] == '\n') { /* 2: \r\n len */
        client->header = header;
        return;
    }
    http_err("set http header fail, header must end with '\\r\\n'\n");
}

int HttpClientBasicAuth(HttpClient *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_BASIC_AUTH_SIZE) {
        return HTTP_EUNKNOW;
    }
    client->authUser = user;
    client->authPassword = password;
    return HTTP_SUCCESS;
}

static int HttpClientSendAuth(HttpClient *client, char *sendBuf, int sendBufSize, int *sendIdx)
{
    char bAuth[(int)((HTTPCLIENT_BASIC_AUTH_SIZE + 3) * 4 / 3 + 3)]; /* 3, 4: auth offset */
    unsigned char base64buff[HTTPCLIENT_BASIC_AUTH_SIZE + 3]; /* 3 : auth offset */

    HttpClientGetInfo(client, sendBuf, sendBufSize, sendIdx, "Authorization: Basic ");
    int ret = sprintf_s((char *)base64buff, sizeof(base64buff), "%s:%s", client->authUser, client->authPassword);
    if (ret < 0) {
        return HTTP_EARG;
    }
    HttpClientBase64enc(bAuth, base64buff);
    bAuth[strlen(bAuth) + 2] = '\0'; /* 2: auth length offset */
    bAuth[strlen(bAuth) + 1] = '\n'; /* 1: auth length offset */
    bAuth[strlen(bAuth)] = '\r';
#ifdef HTTP_DEBUG
    http_debug("bAuth:%s", bAuth);
#endif
    HttpClientGetInfo(client, sendBuf, sendBufSize, sendIdx, bAuth);
    return HTTP_SUCCESS;
}

static int HttpCreateHeaderbuf(char **host, char **path, char **sendBuf, char **buf)
{
    *host = (char *)malloc(HTTPCLIENT_MAX_HOST_LEN);
    if (*host == NULL) {
        http_err("host malloc failed");
        return HTTP_ENOBUFS;
    }
    memset_s(*host, HTTPCLIENT_MAX_HOST_LEN, 0, HTTPCLIENT_MAX_HOST_LEN);

    *path = (char *)malloc(HTTPCLIENT_MAX_URL_LEN);
    if (*path == NULL) {
        http_err("path malloc failed");
        return HTTP_ENOBUFS;
    }
    memset_s(*path, HTTPCLIENT_MAX_URL_LEN, 0, HTTPCLIENT_MAX_URL_LEN);

    *sendBuf = (char *)malloc(HTTPCLIENT_SEND_BUF_SIZE);
    if (*sendBuf == NULL) {
        http_err("sendBuf malloc failed");
        return HTTP_ENOBUFS;
    }
    memset_s(*sendBuf, HTTPCLIENT_SEND_BUF_SIZE, 0, HTTPCLIENT_SEND_BUF_SIZE);

    *buf = (char *)malloc(HTTPCLIENT_SEND_BUF_SIZE);
    if (*buf == NULL) {
        http_err("buf malloc failed");
        return HTTP_ENOBUFS;
    }
    memset_s(*buf, HTTPCLIENT_SEND_BUF_SIZE, 0, HTTPCLIENT_SEND_BUF_SIZE);
    return HTTP_SUCCESS;
}

static void HttpFreeHeaderBuf(char *host, char *path, char *sendBuf, char *buf)
{
    if (host != NULL) {
        free(host);
    }

    if (path != NULL) {
        free(path);
    }

    if (sendBuf != NULL) {
        free(sendBuf);
    }

    if (buf != NULL) {
        free(buf);
    }
}

static void HttpCpyContentInfo(HttpClient *client, HttpClientData *clientData, int *len, char *sendBuf, char *buf)
{
    int ret;
#ifdef CONFIG_HTTP_FORM_DATA
    int formatdataLen = HttpClientFormdataLen(clientData);
#endif
    if (clientData->postBuf != NULL) {
        ret = sprintf_s(buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Length: %d\r\n", clientData->postBufLen);
        if (ret < 0) {
            http_err("content length sprintf fail\n");
        }
        HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, len, buf);
        if (clientData->postContentType != NULL) {
            ret = sprintf_s(buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Type: %s\r\n", clientData->postContentType);
            if (ret < 0) {
                http_err("Content-Type sprintf fail\n");
            }
            HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, len, buf);
        }
    }
#ifdef CONFIG_HTTP_FORM_DATA
    else if (formatdataLen > 0 && (clientData->method == HTTP_POST || clientData->method == HTTP_PUT)) {
        (void)memset_s(buf, HTTPCLIENT_SEND_BUF_SIZE, 0, HTTPCLIENT_SEND_BUF_SIZE);
        ret = sprintf_s(
            buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Type: multipart/form-data; boundary=%s\r\n", FORM_DATA_BOUNDARY);
        if (ret < 0) {
            http_err("sprintf fail\n");
        }
        HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, len, buf);
        formatdataLen += strlen(FORM_DATA_BOUNDARY_END);
        ret = sprintf_s(buf, HTTPCLIENT_SEND_BUF_SIZE, "Content-Length: %d\r\n", formatdataLen);
        if (ret < 0) {
            http_err("sprintf fail\n");
        }
        HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, len, buf);
    }
#endif
}

static char *HttpGetMethodStr(int method)
{
    char *meth = (method == HTTP_GET)      ? "GET"
                 : (method == HTTP_POST)   ? "POST"
                 : (method == HTTP_PUT)    ? "PUT"
                 : (method == HTTP_DELETE) ? "DELETE"
                 : (method == HTTP_HEAD)   ? "HEAD"
                                           : "";
    return meth;
}

static int HttpSendHeaderInner(HttpClient *client, char *sendBuf, int len)
{
    int ret;
#if CONFIG_HTTP_SECURE
    if (client->isHttp == false) {
        if (HttpSslSendWrapper(client, sendBuf, len) != len) {
            http_err("SSL_write failed");
            return HTTP_EUNKNOW;
        }
        return HTTP_SUCCESS;
    }
#endif

    ret = HttpTcpSendWrapper(client, sendBuf, len);
    if (ret > 0) {
#ifdef HTTP_DEBUG
        http_debug("Written %d bytes, socket = %d", ret, client->socket);
#endif
    } else if (ret == 0) {
        http_err("ret == 0,Connection was closed by server");
        return HTTP_ECLSD;
    } else {
        http_err("Connection error (send returned %d)", ret);
        return HTTP_ECONN;
    }
    return HTTP_SUCCESS;
}

static int HttpWriteSendHeader(char *buf, int method, char *path, char *host)
{
    return sprintf_s(buf,
        HTTPCLIENT_SEND_BUF_SIZE,
        "%s %s HTTP/1.1\r\nCache-Control: no-cache\r\nConnection: close\r\nHost: %s\r\n",
        HttpGetMethodStr(method),
        path,
        host); /* Write request */
}

/* send request header */
static int HttpClientSendHeader(HttpClient *client, const char *url, int method, HttpClientData *clientData)
{
    char *host = NULL;
    char *path = NULL;
    int len = 0;
    char *sendBuf = NULL;
    char *buf = NULL;
    int ret;

    ret = HttpCreateHeaderbuf(&host, &path, &sendBuf, &buf);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    ret = HttpClientParseUrl(url, host, HTTPCLIENT_MAX_HOST_LEN, path, HTTPCLIENT_MAX_URL_LEN);
    if (ret != HTTP_SUCCESS) {
        http_err("HttpClientParseUrl returned %d", ret);
        goto exit;
    }

    /* Send request */
    ret = HttpWriteSendHeader(buf, method, path, host);
    if (ret < 0) {
        http_err("sprintf fail, ret=0x%x", ret);
        goto exit;
    }
    ret = HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, &len, buf);
    if (ret != HTTP_SUCCESS) {
        http_err("Could not GetInfo, ret=0x%x", ret);
        goto exit;
    }
    /* Send all headers */
    if (client->authUser) {
        HttpClientSendAuth(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, &len); /* send out Basic Auth header */
    }
    /* Add user header information */
    if (client->header) {
        ret = HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, &len, (char *)client->header);
        if (ret != HTTP_SUCCESS) {
            http_err("Could not GetInfo for user header, ret=0x%x", ret);
            goto exit;
        }
    }

    HttpCpyContentInfo(client, clientData, &len, sendBuf, buf);
    /* Close headers */
    HttpClientGetInfo(client, sendBuf, HTTPCLIENT_SEND_BUF_SIZE, &len, "\r\n");
    ret = HttpSendHeaderInner(client, sendBuf, len);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }
exit:
    HttpFreeHeaderBuf(host, path, sendBuf, buf);
    return ret;
}

static int HttpClientSendUserData(HttpClient *client, HttpClientData *clientData)
{
    int ret = 0;

    if (clientData->postBuf != NULL && clientData->postBufLen > 0) {
#ifdef HTTP_DEBUG
        http_debug("clientData->postBuf:%s", clientData->postBuf);
#endif
#if CONFIG_HTTP_SECURE
        if (client->isHttp == false) {
            if (HttpSslSendWrapper(client, clientData->postBuf, clientData->postBufLen) != clientData->postBufLen) {
                http_err("SSL_write failed");
                return HTTP_EUNKNOW;
            }
        } else
#endif
        {
            ret = HttpTcpSendWrapper(client, clientData->postBuf, clientData->postBufLen);
            if (ret > 0) {
#ifdef HTTP_DEBUG
                http_debug("Written %d bytes", ret);
#endif
            } else if (ret == 0) {
#ifdef HTTP_DEBUG
                http_debug("ret == 0,Connection was closed by server");
#endif
                return HTTP_ECLSD; /* Connection was closed by server */
            } else {
                http_err("Connection error (send returned %d)", ret);
                return HTTP_ECONN;
            }
        }
    } else {
#ifdef CONFIG_HTTP_FORM_DATA
        ret = HttpClientSendFormdata(client, clientData);
        if (ret == HTTP_ENOTSUPP) {
            http_err("post buffer is null\n");
            return HTTP_ECONN;
        } else {
            return ret;
        }
#endif
    }

    return HTTP_SUCCESS;
}

int HttpClientSendData(HttpClient *client, const char *postData, int length)
{
    int ret = 0;
    if (client == NULL || postData == NULL || length <= 0) {
        return HTTP_EARG;
    }

    if (client->isHttp) {
        ret = HttpTcpSendWrapper(client, postData, (int)length);
    } else {
#if CONFIG_HTTP_SECURE
        ret = HttpSslSendWrapper(client, postData, (int)length);
#else
        return HTTP_ENOTSUPP;
#endif
    }
    return ret;
}

static int HttpClientRecvData(HttpClient *client, char *buf, int maxLen, int *pReadLen)
{
    int ret = 0;
    int timeoutMs = HTTPCLIENT_MAX_RECVDATA_WAIT_MS;
    if (client->timeoutMs > 0) {
        timeoutMs = client->timeoutMs;
    }
    if (client->isHttp) {
        ret = HttpTcpRecvWrapper(client, buf, maxLen, timeoutMs, pReadLen);
    } else {
#if CONFIG_HTTP_SECURE
        ret = HttpSslRecvWrapper(client, buf, maxLen, timeoutMs, pReadLen);
#endif
    }
    return ret;
}

static int HttpParseNullContent(HttpClient *client, char *data, int *len, HttpClientData *clientData)
{
    int ret;
    int count = 0;
    int maxLen;

    if (data == NULL) {
        return HTTP_EUNKNOW;
    }

    while (true) {
        if (count + *len < clientData->responseBufLen) {
            memcpy_s(clientData->responseBuf + count, clientData->responseBufLen - count, data, *len);
            count += *len;
        } else {
            memcpy_s(clientData->responseBuf + count,
                clientData->responseBufLen - count,
                data,
                clientData->responseBufLen - count);
            clientData->contentBlockLen = clientData->responseBufLen;
            return HTTP_EAGAIN;
        }

        maxLen = HTTP_MIN(HTTPCLIENT_CHUNK_BUF_SIZE, clientData->responseBufLen - count);
        if (maxLen <= 0) {
            http_err("%s %d error maxLen %d", __func__, __LINE__, maxLen);
            return HTTP_EUNKNOW;
        }
        ret = HttpClientRecvData(client, data, maxLen, len);

        /* Receive data */
#ifdef HTTP_DEBUG
        http_debug("data len=%d count=%d\n", *len, count);
#endif
        if (ret == HTTP_ECONN) {
#ifdef HTTP_DEBUG
            http_debug("ret == HTTP_ECONN");
#endif
            clientData->contentBlockLen = count;
            return ret;
        }

        if (*len == 0) { /* read no more data */
#ifdef HTTP_DEBUG
            http_debug("no more len == 0\n");
#endif
            clientData->isMore = false;
            return HTTP_SUCCESS;
        }
#ifdef HTTP_DEBUG
        http_debug("in loop %s %d ret %d len %d\n", __func__, __LINE__, ret, *len);
#endif
    }
    return HTTP_SUCCESS;
}

static int CheckCrlfString(char *data, const int len)
{
    int crlfPos = 0;
    if (len >= 2) {                            /* 2: \r\n length */
        for (; crlfPos <= len - 2; crlfPos++) { /* 2: \r\n length */
            if (data[crlfPos] == '\r' && data[crlfPos + 1] == '\n') {
                return crlfPos; /* find \r\n after chunk size,return position */
            }
        }
    }
    return -1;
}

static int HttpFoundCrlfPos(HttpClient *client, char *data, int *len, HttpClientData *clientData)
{
    int crlfPos;
    do {
        int ret = 0;
        /* find \r\n after chunk size,return position */
        crlfPos = CheckCrlfString(data, *len);
        if (crlfPos > 0) {
            return crlfPos;
        }
        if (crlfPos == 0 && *len > 2) { /* 2: remove the \r\n */
            if (memmove_s(data, *len, &data[2], *len - 2) != HTTP_SUCCESS) { /* 2: remove the \r\n */
                return HTTP_EPROTO;
            }
            *len -= 2; /* 2: \r\n len */
            continue;
        }

        /* Try to read more */
        if (*len < HTTPCLIENT_CHUNK_BUF_SIZE) {
            int newTrfLen;
            int maxRecv = HTTP_MIN(clientData->responseBufLen, HTTPCLIENT_CHUNK_BUF_SIZE);
            if (maxRecv - *len <= 0) {
                http_err("%s %d error maxLen %d", __func__, __LINE__, maxRecv - *len);
                return HTTP_EUNKNOW;
            }
            ret = HttpClientRecvData(client, data + *len, maxRecv - *len, &newTrfLen);
            *len += newTrfLen;
            if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && newTrfLen == 0)) {
                return ret;
            } else {
#ifdef HTTP_DEBUG
                http_debug("in loop %s line=%d ret=%d,len=%d\n", __func__, __LINE__, ret, *len);
#endif
                continue;
            }
        } else {
            return HTTP_EUNKNOW;
        }
#ifdef HTTP_DEBUG
        http_debug("in loop %s %d len %d ret %d\n", __func__, __LINE__, *len, ret);
#endif
    } while (true);
    return HTTP_EUNKNOW;
}

static int HttpReadChunkHeader(HttpClient *client, char *data, int *len, HttpClientData *clientData)
{
    int readLen = 0;
    int crlfPos;
    int ret;

    if (clientData->isChunked && clientData->retrieveLen <= 0) {
        crlfPos = HttpFoundCrlfPos(client, data, len, clientData);
        if (crlfPos < 0) {
            return crlfPos;
        }
        ret = sscanf_s(data, "%x", &readLen); /* chunk length */
        clientData->retrieveLen = readLen;
        clientData->responseContentLen += clientData->retrieveLen;
        if (ret != 1) {
            http_err("Could not read chunk length");
            return HTTP_EPROTO;
        }
#ifdef HTTP_DEBUG
        http_debug("chunk size=%d,crlfpos=%d\n", readLen, crlfPos);
#endif
        ret = memmove_s(data, *len, &data[crlfPos + 2], *len - (crlfPos + 2)); /* 2: length of \r\n */
        if (ret != HTTP_SUCCESS) {
            return HTTP_EPROTO;
        }
        *len -= (crlfPos + 2); /* 2: length of \r\n */

        if (readLen == 0) {
            /* Last chunk */
            clientData->isMore = false;
#ifdef HTTP_DEBUG
            http_debug("no more (last chunk)\n");
#endif
            return readLen;
        }
    } else {
        readLen = clientData->retrieveLen;
    }
    return readLen;
}

static void HttpSetLeftBuffer(HttpClientData *clientData, int contentBlockLen, char *data, int len)
{
    int leftBufferLen = clientData->responseBufLen - contentBlockLen;
    if (len > leftBufferLen) {
        if (memmove_s(data, len, &data[leftBufferLen], len - leftBufferLen) != 0) {
            http_err("memmove_s fail\n");
        }
        clientData->leftBufferLen = len - leftBufferLen;
        clientData->leftBuffer = data;
    } else {
        clientData->leftBufferLen = 0;
        clientData->leftBuffer = NULL;
    }
}

static int CopyResponseBufInner(
    HttpClient *client, char *data, int *len, int contentLen, HttpClientData *clientData, int *countPtr)
{
    int tempLen = 0;
    int maxLen;
    int readLen = contentLen;
    int contentBlockLen = *countPtr;
    do {
        int ret;
        tempLen = HTTP_MIN(*len, readLen);
        if (contentBlockLen + tempLen < clientData->responseBufLen) {
            memcpy_s(
                clientData->responseBuf + contentBlockLen, clientData->responseBufLen - contentBlockLen, data, tempLen);
            contentBlockLen += tempLen;
            *countPtr = contentBlockLen;
            clientData->retrieveLen -= tempLen;
        } else {
            memcpy_s(clientData->responseBuf + contentBlockLen, clientData->responseBufLen - contentBlockLen,
                data, clientData->responseBufLen - contentBlockLen);
            HttpSetLeftBuffer(clientData, contentBlockLen, data, *len);
            clientData->retrieveLen -= (clientData->responseBufLen - contentBlockLen);
            clientData->contentBlockLen = clientData->responseBufLen;
            return HTTP_EAGAIN;
        }

        if (*len >= readLen) {
#ifdef HTTP_DEBUG
            http_debug("memmove readLen=%d buf len=%d\n", readLen, *len);
#endif
            ret = memmove_s(data, *len, &data[readLen], *len - readLen); /* chunk case, read between two chunks */
            if (ret != HTTP_SUCCESS) {
                return ret;
            }
            *len -= readLen;
            readLen = 0;
            clientData->retrieveLen = 0;
            clientData->leftBufferLen = 0;
            clientData->leftBuffer = NULL;
        } else {
            readLen -= (*len);
        }
        if (readLen > 0) {
            maxLen = HTTP_MIN(HTTP_MIN(HTTPCLIENT_CHUNK_BUF_SIZE, clientData->responseBufLen - contentBlockLen), readLen);
            if (maxLen <= 0) {
                http_err("%s %d error maxLen %d", __func__, __LINE__, maxLen);
                return HTTP_EUNKNOW;
            }
            ret = HttpClientRecvData(client, data, maxLen, len);
            if (ret == HTTP_ECONN || (ret == HTTP_ECLSD && *len == 0)) {
                return ret;
            }
        }
    } while (readLen > 0);
    return HTTP_SUCCESS;
}

static int HttpClientRetrieveContent(HttpClient *client, char *data, int dataLen, HttpClientData *clientData)
{
    int contentBlockLen = 0;
    int readLen = 0;
    int len = dataLen;
    clientData->isMore = true;
    if (clientData->responseContentLen == -1 && clientData->isChunked == false) {
        return HttpParseNullContent(client, data, &len, clientData);
    }

    while (true) {
        readLen = HttpReadChunkHeader(client, data, &len, clientData);
        if (readLen < 0) {
            return readLen;
        } else if (readLen == 0 && clientData->isMore == false) {
            break;
        }
        int ret = CopyResponseBufInner(client, data, &len, readLen, clientData, &contentBlockLen);
        if (ret < 0 || ret == HTTP_EAGAIN) {
            return ret;
        }

        if (!clientData->isChunked) {
            clientData->isMore = false;
            break;
        }
#ifdef HTTP_DEBUG
        http_debug("%s buf len=%d, readLen=%d, blockLen=%d\n", __func__, len, readLen, contentBlockLen);
#endif
        if (len < 2) { /* 2: length of the \r\n */
            int newTrfLen = 0;
            int maxRecv = HTTP_MIN(clientData->responseBufLen - contentBlockLen + 2,
                HTTPCLIENT_CHUNK_BUF_SIZE - len); /* 2: length of the \r\n */
            if (maxRecv <= 0) {
                http_err("%s %d error maxLen %d", __func__, __LINE__, maxRecv);
                return HTTP_EUNKNOW;
            }

            /* Read missing chars to find end of chunk */
            ret = HttpClientRecvData(client, data + len, maxRecv, &newTrfLen); /* get missing \r\n from socket */
            if ((ret == HTTP_ECONN) || (ret == HTTP_ECLSD && newTrfLen == 0)) {
                return ret;
            }
            len += newTrfLen;
        }
        if ((data[0] != '\r') || (data[1] != '\n')) {
            http_err("Format error, not find crlf, %s", data); /* after memmove, the beginning of next chunk */
            return HTTP_EPROTO;
        }
        if (memmove_s(data, len, &data[2], len - 2) != HTTP_SUCCESS) { /* 2: remove the data end \r\n */
            return HTTP_EPROTO;
        }
        len -= 2; /* 2: length of the \r\n */
    }
    clientData->contentBlockLen = contentBlockLen;

    return HTTP_SUCCESS;
}

static int HttpReadResponseHeader(
    HttpClient *client, char *crlfPtr, char *colonPtr, char *data, HttpClientData *clientData)
{
    int keyLen = colonPtr - data;
    int valueLen = crlfPtr - colonPtr - strlen(": ");
    char *keyPtr = data;
    char *valuePtr = colonPtr + strlen(": ");

    if (strncasecmp(keyPtr, "Content-Length", keyLen) == 0) {
        if (sscanf_s(valuePtr, "%d[^\r]", &(clientData->responseContentLen)) < 0) {
            http_err("sscanf_s fail");
            return HTTP_EUNKNOW;
        }
        clientData->retrieveLen = clientData->responseContentLen;
    } else if (strncasecmp(keyPtr, "Transfer-Encoding", keyLen) == 0) {
        if (strncasecmp(valuePtr, "Chunked", valueLen) == 0) {
            clientData->isChunked = true;
            clientData->responseContentLen = 0;
            clientData->retrieveLen = 0;
        }
    } else if (strncasecmp(keyPtr, "Content-Type", keyLen) == 0) {
        if (strncasecmp(valuePtr, "text/event-stream", valueLen) == 0) {
            clientData->isSSE = true;
        } else if (clientData->isSSE) {
            return HTTP_EPROTO;
        }
    } else if ((client->responseCode >= 300 && client->responseCode < 400) && /* 300, 400: response code */
               (strncasecmp(keyPtr, "Location", keyLen) == 0)) {
        if (HTTPCLIENT_MAX_URL_LEN < valueLen + 1) {
            http_err("url is too large (%d >= %d)", valueLen + 1, HTTPCLIENT_MAX_URL_LEN);
            return HTTP_EUNKNOW;
        }
        if (clientData->redirectUrl == NULL) {
            clientData->redirectUrl = (char *)malloc(HTTPCLIENT_MAX_URL_LEN);
        }
        if (clientData->redirectUrl == NULL) {
            http_err("malloc fail\n");
            return HTTP_ENOBUFS;
        }

        if (memset_s(clientData->redirectUrl, HTTPCLIENT_MAX_URL_LEN, 0, HTTPCLIENT_MAX_URL_LEN) != 0) {
            http_err("memset fail\n");
            return HTTP_ENOBUFS;
        }
        if (memcpy_s(clientData->redirectUrl, HTTPCLIENT_MAX_URL_LEN, valuePtr, valueLen) != 0) {
            http_err("memcpy fail\n");
            return HTTP_ENOBUFS;
        }
        clientData->isRedirected = 1;
    }
    return HTTP_SUCCESS;
}

static int HttpClientParseResponseCode(HttpClient *client, char *data, int *len)
{
    int crlfPos;
    char *crlfPtr = strstr(data, "\r\n");
    if (crlfPtr == NULL) {
        http_err("\r\n not found");
        return HTTP_EPROTO;
    }

    crlfPos = crlfPtr - data;

    /* Parse HTTP response */
    if (sscanf_s(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->responseCode)) != 1) {
        /* Cannot match string, error */
        http_err("Not a correct HTTP answer : %s", data);
        return HTTP_EPROTO;
    }

    if ((client->responseCode < 200) || (client->responseCode >= 400)) { /* 200, 400: response code */
#ifdef HTTP_DEBUG
        http_debug("Response code %d", client->responseCode);
#endif
        if (client->responseCode == 416) { /* 416: response code */
            http_err("Requested Range Not Satisfiable");
            return HTTP_EUNKNOW;
        }
    }

    if (memmove_s(data, *len, &data[crlfPos + 2], *len - (crlfPos + 2)) != 0) { /* 2: length of the \r\n */
        http_err("mem move data fail");
        return HTTP_EUNKNOW;
    }                      /* Be sure to move NULL-terminating char as well */
    *len -= (crlfPos + 2); /* 2: length of the \r\n */
    return HTTP_SUCCESS;
}

static int HttpParseHeaderContent(HttpClient *client, HttpClientData *clientData, char *crlfPtr, char *data, int *len)
{
    int crlfPos = crlfPtr - data;
    char *colonPtr = NULL;
    int ret = HTTP_SUCCESS;

    colonPtr = strstr(data, ": ");
    if (colonPtr == NULL) {
        http_err("Could not parse header");
        return HTTP_EUNKNOW;
    }

    ret = HttpReadResponseHeader(client, crlfPtr, colonPtr, data, clientData);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    ret = memmove_s(data, *len, &data[crlfPos + 2], *len - (crlfPos + 2)); /* 2: length of \r\n */
    /* Be sure to move NULL-terminating char as well */
    if (ret != HTTP_SUCCESS) {
        return ret;
    }
    *len -= (crlfPos + 2); /* 2: length of \r\n */
    return HTTP_SUCCESS;
}

static void CopyResponseHeaderBuf(char **headerBuf, int *headerBufLen, char *data, int crlfPos)
{
    char *colonPtr = NULL;
    int ret;
    colonPtr = strstr(data, ": ");
    if (colonPtr == NULL) {
        return;
    }
    if (*headerBufLen >= crlfPos + 2 && *headerBuf != NULL) { /* 2: length of the \r\n */
        /* copy response header to caller buffer */
        ret = memcpy_s(*headerBuf, *headerBufLen, data, crlfPos + 2); /* 2: length of the \r\n */
        if (ret != HTTP_SUCCESS) {
            http_err("memcpy_s fail, ret=0x%x\n", ret);
            return;
        }
        *headerBuf += crlfPos + 2;    /* 2: length of the \r\n */
        *headerBufLen -= crlfPos + 2; /* 2: length of the \r\n */
    }
}

static int HttpClientRetrieveHeaders(HttpClient *client, char *data, int *len, HttpClientData *clientData)
{
    char *crlfPtr = NULL;
    int headerBufLen = clientData->headerBufLen;
    char *headerBuf = clientData->headerBuf;
    int ret;

    (void)memset_s(headerBuf, headerBufLen, 0, headerBufLen);

    /* Now get headers */
    while (true) {
        int readResult;
        crlfPtr = strstr(data, "\r\n");
        if (crlfPtr == NULL) {
            int newTrfLen = 0;
            if (HTTPCLIENT_CHUNK_BUF_SIZE - *len <= 0) {
                http_err("%s %d error maxLen %d", __func__, __LINE__, HTTPCLIENT_CHUNK_BUF_SIZE - *len);
                return HTTP_EUNKNOW;
            }
            readResult = HttpClientRecvData(client, data + *len, HTTPCLIENT_CHUNK_BUF_SIZE - *len, &newTrfLen);
            *len += newTrfLen;
            if ((readResult == HTTP_ECONN) || (readResult == HTTP_ECLSD && newTrfLen == 0)) {
                return readResult;
            } else {
                continue;
            }
        }
        int crlfPos = crlfPtr - data;
        if (crlfPos == 0) {                                  /* End of headers */
            ret = memmove_s(data, *len, &data[2], *len - 2); /* 2: length of \r\n */
            if (ret != HTTP_SUCCESS) {
                return HTTP_ERECV;
            }
            *len -= 2; /* 2: length of \r\n */
            break;
        }
        CopyResponseHeaderBuf(&headerBuf, &headerBufLen, data, crlfPos);

        ret = HttpParseHeaderContent(client, clientData, crlfPtr, data, len);
        if (ret != HTTP_SUCCESS) {
            return ret;
        }
    }
    return HTTP_SUCCESS;
}

static int HttpClientResponseParse(HttpClient *client, char *data, int len, HttpClientData *clientData)
{
    int ret;
    clientData->responseContentLen = -1;
    clientData->isChunked = false;

    ret = HttpClientParseResponseCode(client, data, &len);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    ret = HttpClientRetrieveHeaders(client, data, &len, clientData);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    if (client->responseCode == 204) { /* 204 : http status code, means no content */
        return HTTP_SUCCESS;
    }

    if (clientData->method == HTTP_HEAD) {
        return client->responseCode == 200 ? HTTP_SUCCESS : HTTP_ENOTSUPP; /* 200 : http code */
    }

    return HttpClientRetrieveContent(client, data, len, clientData);
}

static int HttpSetConType(HttpClient *client, const char *url)
{
    int ret = HTTP_ECONN;
    char scheme[8] = {0}; /* 8: max len of http scheme */

    ret = HttpGetUrlScheme(url, scheme, sizeof(scheme));
    if (ret != HTTP_SUCCESS) {
        http_err("HttpGetUrlScheme returned %d", ret);
        return ret;
    }

    // http or https
    if (strcmp(scheme, "https") == 0) {
        client->isHttp = false;
    } else if (strcmp(scheme, "http") == 0) {
        client->isHttp = true;
    }
    return HTTP_SUCCESS;
}

HTTPC_RESULT HttpClientConn(HttpClient *client, const char *url)
{
    int ret = HTTP_ECONN;
    if (client == NULL || url == NULL) {
        return HTTP_EARG;
    }
    char *host = (char *)malloc(HTTPCLIENT_MAX_HOST_LEN);
    if (host == NULL) {
        http_err("host malloc failed");
        ret = HTTP_ENOBUFS;
        goto exit;
    }
    (void)memset_s(host, HTTPCLIENT_MAX_HOST_LEN, 0, HTTPCLIENT_MAX_HOST_LEN);

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    ret = HttpClientParseUrl(url, host, HTTPCLIENT_MAX_HOST_LEN, NULL, 0);
    if (ret != HTTP_SUCCESS) {
        http_err("HttpClientParseUrl returned %d", ret);
        goto exit;
    }

    ret = HttpGetUrlPort(url, &(client->remotePort));
    if (ret != HTTP_SUCCESS) {
        http_err("HttpGetUrlPort returned %d", ret);
        goto exit;
    }

    ret = HttpSetConType(client, url);
    if (ret != HTTP_SUCCESS) {
        goto exit;
    }
    // default http 80 port, https 443 port
    if (client->remotePort == 0) {
        if (client->isHttp) {
            client->remotePort = HTTP_SERVERPORT;
        } else {
            client->remotePort = HTTPS_SERVERPORT;
        }
    }

    client->socket = -1;
    if (client->isHttp) {
        ret = HttpTcpConnWrapper(client, host);
    } else {
#if CONFIG_HTTP_SECURE
        ret = HttpSslConnWrapper(client, host);
#endif
    }

exit:
    if (host != NULL) {
        free(host);
    }
    return (HTTPC_RESULT)ret;
}

HTTPC_RESULT HttpClientSend(HttpClient *client, const char *url, int method, HttpClientData *clientData)
{
    int ret = HTTP_ECONN;
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    if (client->socket < 0) {
        return (HTTPC_RESULT)ret;
    }
    clientData->method = method;

    ret = HttpClientSendHeader(client, url, method, clientData);
    if (ret != 0) {
        return (HTTPC_RESULT)ret;
    }

    if (method == HTTP_POST || method == HTTP_PUT) {
        ret = HttpClientSendUserData(client, clientData);
    }
#ifdef HTTP_DEBUG
    http_debug("HttpClientSend() result:%d, client:%p\r\n", ret, client);
#endif
    return (HTTPC_RESULT)ret;
}

static int HttpCopyLastRecvBuf(HttpClientData *clientData, char *buf, int bufLen)
{
    int recLen = 0;
    if (clientData->leftBuffer != NULL && clientData->leftBufferLen > 0) {
        if (memcpy_s(buf, HTTPCLIENT_CHUNK_BUF_SIZE, clientData->leftBuffer, clientData->leftBufferLen) != 0) {
            http_err("memcpy fail!\n");
            return -1;
        }
        recLen = clientData->leftBufferLen;
        free(clientData->leftBuffer);
        clientData->leftBuffer = NULL;
        clientData->leftBufferLen = 0;
    }
    return recLen;
}

static void HttpFreeRecvBuffer(HttpClientData *clientData, int ret, char *buf)
{
    if (buf != NULL && clientData->leftBuffer == NULL) {
        free(buf);
        buf = NULL;
    }
    if (ret < 0) {
        clientData->isMore = false;
        if (clientData->leftBuffer != NULL) {
            free(clientData->leftBuffer);
            clientData->leftBuffer = NULL;
            clientData->leftBufferLen = 0;
        }
    }
}

HTTPC_RESULT HttpClientRecvResponse(HttpClient *client, HttpClientData *clientData)
{
    int reclen = 0;
    int ret = HTTP_ECONN;
    char *buf = NULL;

    if (client == NULL || clientData == NULL || clientData->headerBuf == NULL || clientData->responseBuf == NULL) {
        return HTTP_EARG;
    }

    if (clientData->headerBufLen < HTTPCLIENT_CHUNK_BUF_SIZE || clientData->responseBufLen < HTTPCLIENT_CHUNK_BUF_SIZE) {
        http_err("Error: header buffer or response buffer should not less than %d!", HTTPCLIENT_CHUNK_BUF_SIZE);
        return HTTP_EARG;
    }

    buf = (char *)malloc(HTTPCLIENT_CHUNK_BUF_SIZE);
    if (buf == NULL) {
        http_err("Malloc failed socket fd %d!", client->socket);
        ret = HTTP_ENOBUFS;
        goto exit;
    }
    (void)memset_s(buf, HTTPCLIENT_CHUNK_BUF_SIZE, 0, HTTPCLIENT_CHUNK_BUF_SIZE);
    reclen = HttpCopyLastRecvBuf(clientData, buf, HTTPCLIENT_CHUNK_BUF_SIZE);
    if (reclen < 0) {
        goto exit;
    }

    if (client->socket < 0) {
        http_err("Invalid socket fd %d!", client->socket);
        goto exit;
    }

    if (clientData->isMore) {
        clientData->responseBuf[0] = '\0';
        ret = HttpClientRetrieveContent(client, buf, reclen, clientData);
    } else {
        ret = HttpClientRecvData(client, buf, HTTPCLIENT_CHUNK_BUF_SIZE, &reclen);
        if (ret != HTTP_SUCCESS && ret != HTTP_ECLSD) {
            goto exit;
        }

        if (reclen > 0) {
#ifdef HTTP_DEBUG
            http_debug("reclen:%d\n", reclen);
#endif
            ret = HttpClientResponseParse(client, buf, reclen, clientData);
        }
    }
#ifdef HTTP_DEBUG
    http_debug("%s() result:%d, client:%p\n", __FUNCTION__, ret, client);
#endif
exit:
    HttpFreeRecvBuffer(clientData, ret, buf);
    return (HTTPC_RESULT)ret;
}


static int HttpCreateSseEvent(char **event, char **eventData, size_t length)
{
    *event = (char *)malloc(length);
    if (*event == NULL) {
        http_err("malloc %u fail\n", length);
        return HTTP_ENOBUFS;
    }
    *eventData = (char *)malloc(length);
    if (*eventData == NULL) {
        http_err("malloc %u fail\n", length);
        free(*event);
        *event = NULL;
        return HTTP_ENOBUFS;
    }
    (void)memset_s(*event, length, 0, length);
    (void)memset_s(*eventData, length, 0, length);
    return HTTP_SUCCESS;
}

static int HttpParseSSELine(char *ptr, HttpSSEData *sseData, int lineLen, int *dataLen)
{
    int ret = HTTP_EPARSE;
    char *dataContent = NULL;

    char *linePtr = (char *)calloc(1, lineLen + 1);
    if (linePtr == NULL) {
        return HTTP_ENOBUFS;
    }

    if (memcpy_s(linePtr, lineLen + 1, ptr, lineLen) != 0) {
        goto EXIT;
    }

    /* parse event and data in one line */
    if (lineLen >= 6 && strncmp(linePtr, "event:", 6) == 0) { /* 6 : length of event: */
        /* event line */
        if (sscanf_s(linePtr, "event: %[^\r\n]", sseData->event, lineLen) < 0) {
            goto EXIT;
        }
    } else if (lineLen >= 5 && strncmp(linePtr, "data:", 5) == 0) { /* 5 : length of data: */
        /* data line */
        dataContent = (char *)calloc(1, lineLen);
        if (dataContent == NULL) {
            ret = HTTP_ENOBUFS;
            goto EXIT;
        }
        if (sscanf_s(linePtr, "data: %[^\r\n]", dataContent, lineLen) < 0) {
            goto EXIT;
        }
        if (*dataLen > 0) {
            sseData->data[*dataLen] = '\n';
            (*dataLen)++;
        }
        if (strcpy_s(sseData->data + *dataLen, lineLen, dataContent) != 0) {
            goto EXIT;
        }
        *dataLen += strlen(dataContent);
    } else if (lineLen >= 3 && strncmp(linePtr, "id:", 3) == 0) { /* 3: length of id: */
        /* event id line */
        if (sscanf_s(linePtr, "id: %[^\r\n]", sseData->eventId, HTTP_EVENT_ID_LENGTH) < 0) {
            goto EXIT;
        }
    }
    ret = HTTP_SUCCESS;
EXIT:
    if (linePtr != NULL) {
        free(linePtr);
    }
    if (dataContent != NULL) {
        free(dataContent);
    }
    return ret;
}

static int HttpParseSseEvent(HttpClient *client, char *buffer, int *length)
{
    int ret = HTTP_EAGAIN;
    int dataLen = 0;
    HttpSSEData sseData = {0};
    size_t leftLength = *length;
    char *ptr = buffer;
    char *bufferEnd = buffer + leftLength;

    if (HttpCreateSseEvent(&sseData.event, &sseData.data, leftLength) != HTTP_SUCCESS) {
        return HTTP_ENOBUFS;
    }

    while (ptr < bufferEnd) {
        /* find line end */
        char *lineEnd = strchr(ptr, '\n');
        if (lineEnd == NULL) {
            ret = HTTP_EAGAIN;
#ifdef HTTP_DEBUG
            http_debug("not find line end\n");
#endif
            goto EXIT;
        }

        size_t lineLen = (uintptr_t)(void *)lineEnd - (uintptr_t)(void *)ptr;
        leftLength -= (lineLen + 1);
        if (lineLen > 0 && ptr[lineLen - 1] == '\r') {
            lineLen--; /* remove \r */
        }

        if (lineLen > 0) {
            ret = HttpParseSSELine(ptr, &sseData, lineLen, &dataLen);
            if (ret != HTTP_SUCCESS) {
                http_err("HttpParseSSELine fail, ret=%d\n", ret);
                goto EXIT;
            }
            ptr = lineEnd + 1;
            continue;
        }

        /* blank line means event ends, call callback to user */
        if (dataLen > 0 && client->sseCallback != NULL) {
            sseData.data[dataLen] = '\0';
            client->sseCallback(client, sseData.event, sseData.data, sseData.eventId);
        }
        ret = HTTP_SUCCESS;
        if (leftLength > 0) { /* recv buffer length is more than needed */
#ifdef HTTP_DEBUG
            http_debug("left buffer length =%d, buffeLen=%d\n", leftLength, *length);
#endif
            if (memmove_s(buffer, *length, buffer + (*length - leftLength), leftLength) != 0) {
                ret = HTTP_EPARSE;
                goto EXIT;
            }
        }
        *length = leftLength;
        goto EXIT;
    }
    ret = HTTP_EAGAIN;
EXIT:
    free(sseData.event);
    free(sseData.data);
    return ret;
}

static int HttpSSEParseHeader(HttpClient *client, HttpClientData *clientData, int *bytesRead)
{
    int ret = HttpClientParseResponseCode(client, clientData->responseBuf, bytesRead);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    ret = HttpClientRetrieveHeaders(client, clientData->responseBuf, bytesRead, clientData);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }
    return ret;
}

static int HttpClientSseRecvInner(HttpClient *client, HttpClientData *clientData)
{
    int reclen = 0;
    int ret = HTTP_ECONN;
    bool parseHeader = true;
    int bytesRead = 0;
    int timeoutMs = HTTPCLIENT_MAX_RECVDATA_WAIT_MS;
    if (client->timeoutMs > 0) {
        timeoutMs = client->timeoutMs;
    }
    while (bytesRead < clientData->responseBufLen) {
        if (client->isHttp) {
            ret = HttpTcpRecvWrapper(client,
                clientData->responseBuf + bytesRead,
                clientData->responseBufLen - bytesRead, timeoutMs, &reclen);
        } else {
#if CONFIG_HTTP_SECURE
            ret = HttpSslRecvWrapper(client,
                clientData->responseBuf + bytesRead,
                clientData->responseBufLen - bytesRead, timeoutMs, &reclen);
#endif
        }
        if (ret == HTTP_EAGAIN) {
            continue;
        }
        if (ret != HTTP_SUCCESS || reclen == 0) {
            return ret;
        }
        bytesRead += reclen;
#ifdef HTTP_DEBUG
        http_debug("sse reclen=%d,bytesread=%d\n", reclen, bytesRead);
#endif
        if (parseHeader) {
            parseHeader = false;
            ret = HttpSSEParseHeader(client, clientData, &bytesRead);
            if (ret != HTTP_SUCCESS) {
                return ret;
            }
        }
        if (bytesRead == 0) {
            continue;
        }

        ret = HttpParseSseEvent(client, clientData->responseBuf, &bytesRead);
        if (ret == HTTP_EAGAIN || (ret == HTTP_SUCCESS && bytesRead > 0)) {
            continue;
        }
        if (ret != HTTP_SUCCESS) {
            return ret;
        }
        (void)memset_s(clientData->responseBuf, clientData->responseBufLen, 0, clientData->responseBufLen);
    }
    return ret;
}

int HttpClientSseRecv(HttpClient *client, HttpClientData *clientData)
{
    if (client == NULL || clientData == NULL || clientData->headerBuf == NULL || clientData->responseBuf == NULL ||
        client->sseCallback == NULL) {
        return HTTP_EARG;
    }

    if (clientData->responseBufLen < HTTPCLIENT_CHUNK_BUF_SIZE || clientData->headerBufLen < HTTPCLIENT_CHUNK_BUF_SIZE) {
        http_err("Error: response buffer or header buffer should not less than %d!\n", HTTPCLIENT_CHUNK_BUF_SIZE);
        return HTTP_EARG;
    }
    clientData->isSSE = true;
    (void)memset_s(clientData->responseBuf, clientData->responseBufLen, 0, clientData->responseBufLen);
    return HttpClientSseRecvInner(client, clientData);
}

void HttpClientClose(HttpClient *client)
{
    if (client == NULL) {
        return;
    }
    if (client->isHttp) {
        HttpTcpCloseWrapper(client);
    } else {
#if CONFIG_HTTP_SECURE
        HttpSslCloseWrapper(client);
#endif
    }

    client->socket = -1;
#ifdef HTTP_DEBUG
    http_debug("HttpClientClose() client:%p\n", client);
#endif
}

int HttpClientGetResponseCode(HttpClient *client)
{
    if (client == NULL) {
        return HTTP_EARG;
    }
    return client->responseCode;
}

int HttpClientGetResponseHeaderValue(char *headerBuf, char *name, int *valPos, int *valLen)
{
    char *data = headerBuf;
    char *crlfPtr, *colonPtr, *keyPtr, *valuePtr;
    int keyLen, valueLen;

    if (headerBuf == NULL || name == NULL || valPos == NULL || valLen == NULL) {
        return HTTP_EARG;
    }

    while (true) {
        crlfPtr = strstr(data, "\r\n");
        colonPtr = strstr(data, ": ");
        if (crlfPtr != NULL && colonPtr != NULL) {
            keyLen = colonPtr - data;
            valueLen = crlfPtr - colonPtr - strlen(": ");
            keyPtr = data;
            valuePtr = colonPtr + strlen(": ");
#ifdef HTTP_DEBUG
            http_debug("Response header: %.*s: %.*s", keyLen, keyPtr, valueLen, valuePtr);
#endif
            if (strncasecmp(keyPtr, name, keyLen) == 0) {
                *valPos = valuePtr - headerBuf;
                *valLen = valueLen;
                return 0;
            } else {
                data = crlfPtr + 2; /* 2: length of \r\n */
                continue;
            }
        } else {
            return HTTP_EARG;
        }
    }
}

HTTPC_RESULT HttpClientPrepare(HttpClientData *clientData, int headerSize, int respSize)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    if (!clientData)
        return HTTP_EUNKNOW;

    if (headerSize < HTTPCLIENT_CHUNK_BUF_SIZE || respSize < HTTPCLIENT_CHUNK_BUF_SIZE) {
        http_err("Error: header buffer or response buffer should not less than %d!", HTTPCLIENT_CHUNK_BUF_SIZE);
        return HTTP_EARG;
    }

    memset_s(clientData, sizeof(HttpClientData), 0, sizeof(HttpClientData));

    clientData->headerBuf = (char *)malloc(headerSize);
    clientData->responseBuf = (char *)malloc(respSize);

    if (clientData->headerBuf == NULL || clientData->responseBuf == NULL) {
        http_err("httpc_prepare alloc memory failed");
        if (clientData->headerBuf) {
            free(clientData->headerBuf);
            clientData->headerBuf = NULL;
        }

        if (clientData->responseBuf) {
            free(clientData->responseBuf);
            clientData->responseBuf = NULL;
        }
        ret = HTTP_EUNKNOW;
        goto finish;
    }
#ifdef HTTP_DEBUG
    http_debug("httpc_prepare alloc memory");
#endif
    clientData->headerBufLen = headerSize;
    clientData->responseBufLen = respSize;
    clientData->postBufLen = 0;

    clientData->isRedirected = 0;
    clientData->redirectUrl = NULL;

finish:
    return ret;
}

void HttpClientReset(HttpClientData *clientData)
{
    if (clientData == NULL) {
        return;
    }
    char *responseBuf = clientData->responseBuf;
    char *headerBuf = clientData->headerBuf;
    int responseBufLen = clientData->responseBufLen;
    int headerBufLen = clientData->headerBufLen;

    memset_s(clientData, sizeof(HttpClientData), 0, sizeof(HttpClientData));

    clientData->responseBuf = responseBuf;
    clientData->headerBuf = headerBuf;
    clientData->responseBufLen = responseBufLen;
    clientData->headerBufLen = headerBufLen;
}

HTTPC_RESULT HttpClientUnprepare(HttpClientData *clientData)
{
    HTTPC_RESULT ret = HTTP_SUCCESS;

    if (clientData == NULL) {
        ret = HTTP_EUNKNOW;
        goto finish;
    }

    if (clientData->headerBuf) {
        free(clientData->headerBuf);
        clientData->headerBuf = NULL;
    }

    if (clientData->responseBuf) {
        free(clientData->responseBuf);
        clientData->responseBuf = NULL;
    }

    clientData->headerBufLen = 0;
    clientData->responseBufLen = 0;

    clientData->isRedirected = 0;
    if (clientData->redirectUrl) {
        free(clientData->redirectUrl);
        clientData->redirectUrl = NULL;
    }

finish:
    return ret;
}

#ifdef __cplusplus
}
#endif