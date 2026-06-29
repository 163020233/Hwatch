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

#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief   http requst type */
typedef enum {
    HTTP_DELETE,
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
    HTTP_PUT
} HTTP_REQUEST_TYPE;

/** @brief   http error code */
typedef enum {
    HTTP_EAGAIN   =  1,  /**< more data to retrieved */
    HTTP_SUCCESS  =  0,  /**< operation success      */
    HTTP_ENOBUFS  = -1,  /**< buffer error           */
    HTTP_EARG     = -2,  /**< illegal argument       */
    HTTP_ENOTSUPP = -3,  /**< not support            */
    HTTP_EDNS     = -4,  /**< DNS fail               */
    HTTP_ECONN    = -5,  /**< connect fail           */
    HTTP_ESEND    = -6,  /**< send packet fail       */
    HTTP_ECLSD    = -7,  /**< connect closed         */
    HTTP_ERECV    = -8,  /**< recv packet fail       */
    HTTP_EPARSE   = -9,  /**< url parse error        */
    HTTP_EPROTO   = -10, /**< protocol error         */
    HTTP_EUNKNOW  = -11, /**< unknown error          */
    HTTP_ETIMEOUT = -12, /**< timeout                */
} HTTPC_RESULT;

typedef struct HttpClient HttpClient;

/** @brief http SSE callback */
typedef void (*HttpSseCallback)(HttpClient *client, const char* event, const char* data, const char* id);

/** @brief   This structure defines the HttpClient structure   */
struct HttpClient {
    int socket;                     /**< socket ID                 */
    int remotePort;                /**< hTTP or HTTPS port        */
    int responseCode;              /**< response code             */
    char *header;                   /**< request custom header     */
    char *authUser;                /**< username for basic authentication         */
    char *authPassword;            /**< password for basic authentication         */
    bool isHttp;                   /**< http connection? if 1, http; if 0, https  */
    HttpSseCallback sseCallback;   /**< http sse callback */
    int timeoutMs;                   /**< http timeout in millseconds */
#if CONFIG_HTTP_SECURE
    const char *serverCert;        /**< server certification      */
    const char *clientCert;        /**< client certification      */
    const char *clientPk;          /**< client private key        */
    int serverCertLen;            /**< server certification lenght, server_cert buffer size  */
    int clientCertLen;            /**< client certification lenght, client_cert buffer size  */
    int clientPkLen;              /**< client private key lenght, client_pk buffer size      */
    void *ssl;                      /**< ssl content               */
#endif
};

/** @brief   This structure defines the HTTP data structure.  */
typedef struct {
    bool isMore;            /**< indicates if more data needs to be retrieved. */
    bool isChunked;         /**< response data is encoded in portions/chunks.*/
    bool isSSE;             /**< indicates if response in SSE mode. */
    int method;             /**< http method. */
    int retrieveLen;        /**< content length to be retrieved. */
    int responseContentLen; /**< response content total length. */
    int contentBlockLen;    /**< the content length of one block. */
    int postBufLen;         /**< post data length. */
    int responseBufLen;     /**< response body buffer length. */
    int headerBufLen;       /**< response head buffer lehgth. */
    char *postContentType;  /**< content type of the post data. */
    char *postBuf;          /**< user data to be posted. */
    char *responseBuf;      /**< buffer to store the response body data. */
    char *headerBuf;        /**< buffer to store the response head data. */
    bool isRedirected;      /**< redirected URL? if 1, has redirect url; if 0, no redirect url */
    char *redirectUrl;      /**< redirect url when got http 3** response code. */
    char *leftBuffer;       /**< buffer to store last recv not report to user */
    int leftBufferLen;      /**< last recv buffer length, not report to user */
} HttpClientData;

/**
 * This function executes a GET request on a given URL. It blocks until completion.
 * @param[in] client             client is a pointer to the #HttpClient.
 * @param[in] url                url is the URL to run the request.
 * @param[in, out] clientData   clientData is a pointer to the #HttpClientData instance to collect the data
 * returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientGetRequest(HttpClient *client, const char *url, HttpClientData *clientData);

/**
 * This function executes a HEAD request on a given URL. It blocks until completion.
 * @param[in] client             client is a pointer to the #HttpClient.
 * @param[in] url                url is the URL to run the request.
 * @param[in, out] clientData   clientData is a pointer to the #HttpClientData instance to collect the data
 * returned by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientHeadRequest(HttpClient *client, const char *url, HttpClientData *clientData);

/**
 * This function executes a POST request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #HttpClient.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] clientData    clientData is a pointer to the #HttpClientData instance to collect the data
 * returned by the request. It also contains the data to be posted.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientPostRequest(HttpClient *client, const char *url, HttpClientData *clientData);

/**
 * This function executes a PUT request on a given URL. It blocks until completion.
 * @param[in] client              client is a pointer to the #HttpClient.
 * @param[in] url                 url is the URL to run the request.
 * @param[in, out] clientData    clientData is a pointer to the #HttpClientData instance to collect the data
 * returned by the request. It also contains the data to be put.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientPutRequest(HttpClient *client, const char *url, HttpClientData *clientData);

/**
 * This function executes a DELETE request on a given URL. It blocks until completion.
 * @param[in] client               client is a pointer to the #HttpClient.
 * @param[in] url                  url is the URL to run the request.
 * @param[in, out] clientData clientData is a pointer to the #HttpClientData instance to collect the data returned
 * by the request.
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientDelete(HttpClient *client, const char *url, HttpClientData *clientData);

/**
 * This function allocates buffer for http header/response
 * @param[in] clientData      pointer to the #HttpClientData.
 * @param[in] headerSize      header buffer size
 * @param[in] respSize        response buffer size
 * @return  HTTP_SUCCESS       success
 * @return  HTTP_EUNKNOW       fail
 */
HTTPC_RESULT HttpClientPrepare(HttpClientData *clientData, int headerSize, int respSize);

/**
 * This function deallocates buffer for http header/response.
 * @param[in] clientData      pointer to the #HttpClientData.
 * @return  HTTP_SUCCESS       success
 * @return  HTTP_EUNKNOW       fail
 */
HTTPC_RESULT HttpClientUnprepare(HttpClientData *clientData);

/**
 * This function reset buffer for  http header/response.
 * @param[in] clientData      pointer to the #HttpClientData.
 * @return           None.
 */
void HttpClientReset(HttpClientData *clientData);

/**
 * This function establish http/https connection.
 * @param[in] client            pointer to the #HttpClient.
 * @param[in] url               remote URL
 * @return           Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientConn(HttpClient *client, const char *url);

/**
 * This function sends HTTP request.
 * @param[in] client            a pointer to the #HttpClient.
 * @param[in] url               remote URL
 * @param[in] method            http request method
 * @param[in] clientData       a pointer to #HttpClientData.
 * @return    Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientSend(HttpClient *client, const char *url, int method, HttpClientData *clientData);

/**
 * This function receives response from remote
 * @param[in]  client               a pointer to #HttpClient.
 * @param[out] clientData          a pointer to #HttpClientData.
 * @return     Please refer to #HTTPC_RESULT.
 */
HTTPC_RESULT HttpClientRecvResponse(HttpClient *client, HttpClientData *clientData);

/**
 * This function close http connection.
 * @param[in] client               client is a pointer to the #HttpClient.
 * @return           None.
 */
void HttpClientClose(HttpClient *client);

/**
 * This function sends HTTP request.
 * @param[in] client            a pointer to the #HttpClient.
 * @param[in] data              post data buffer
 * @param[in] length            buffer length
 * @return   length of data be uploaded to server; value < 0, if errors occurred.
 */
int HttpClientSendData(HttpClient *client, const char *postData, int length);

/**
 * This function receives SSE response from remote.
 * @param[in]  client               a pointer to #HttpClient.
 * @param[in] clientData          a pointer to #HttpClientData.
 * @return     Please refer to #HTTPC_RESULT.
 */
int HttpClientSseRecv(HttpClient *client, HttpClientData *clientData);

/**
 * This function sets a custom header.
 * @param[in] client               client is a pointer to the #HttpClient.
 * @param[in] header               header is a custom header string.
 * @return           None.
 */
void HttpClientSetCustomHeader(HttpClient *client, char *header);

/**
 * This function gets the HTTP response code assigned to the last request.
 * @param[in] client               client is a pointer to the #HttpClient.
 * @return           The HTTP response code of the last request.
 */
int HttpClientGetResponseCode(HttpClient *client);

/**
 * This function retrieves the value of the specified HTTP response header.
 * @param[in] headerBuf headerBuf is the response header buffer.
 * @param[in] name                The name is to find the specified http response header name.
 * @param[in, out] valPos         valPos is the position of header value in headerBuf.
 * @param[in, out] valLen         valLen is header value length.
 * @return           0, The value is got. Others, errors occurred.
 */
int HttpClientGetResponseHeaderValue(char *headerBuf, char *name, int *valPos, int *valLen);

/**
 * This function add text formdata information.
 * @param[in] clientData          clientData is a pointer to the #HttpClientData.
 * @param[in] contentType         contentType is a pointer to the content type string.
 * @param[in] name                 name is a pointer to the name string.
 * @param[in] data                 data is a pointer to the data.
 * @param[in] dataLen             dataLen is the data length.
 * @return           The HTTP response code of the last request.
 */
int HttpClientFormdataAddText(HttpClientData *clientData, char *contentType, char *name, char *data, int dataLen);

/**
 * This function add file formdata information.
 * @param[in] clientData          clientData is a pointer to the #HttpClientData.
 * @param[in] name                 name is a pointer to the name string.
 * @param[in] contentType         contentType is a pointer to the content type string.
 * @param[in] filePath            filePath is a pointer to the file path.
 * @return           The HTTP response code of the last request.
 */
int HttpClientFormdataAddfile(HttpClientData *clientData, char *name, char *contentType, char *filePath);

/**
 * This function clear file formdata information.
 * @param[in] clientData          clientData is a pointer to the #HttpClientData.
 * @return           None.
 */
void HttpClientClearFormData(HttpClientData *clientData);
#ifdef __cplusplus
}
#endif

#endif /* HTTPCLIENT_H */