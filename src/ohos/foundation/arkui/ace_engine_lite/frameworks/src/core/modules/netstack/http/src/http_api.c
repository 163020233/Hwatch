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
#include <stdlib.h>

#include "httpclient.h"
#include "http_wrappers.h"

#ifdef __cplusplus
extern "C" {
#endif

static HTTPC_RESULT HttpClientCommon(HttpClient *client, const char *url, int method, HttpClientData *clientData)
{
    HTTPC_RESULT ret = HTTP_ECONN;

    /* reset httpclient redirect flag */
    clientData->isRedirected = 0;
    ret = HttpClientConn(client, url);
    if (ret == HTTP_SUCCESS) {
        ret = HttpClientSend(client, url, method, clientData);
        if (ret == HTTP_SUCCESS) {
            ret = HttpClientRecvResponse(client, clientData);
        }
    }
    HttpClientClose(client);
    return ret;
}

HTTPC_RESULT HttpClientGetRequest(HttpClient *client, const char *url, HttpClientData *clientData)
{
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    int ret = HttpClientCommon(client, url, HTTP_GET, clientData);
    while ((0 == ret) && (1 == clientData->isRedirected)) {
        ret = HttpClientCommon(client, clientData->redirectUrl, HTTP_GET, clientData);
    }
    if (clientData->redirectUrl != NULL) {
        free(clientData->redirectUrl);
        clientData->redirectUrl = NULL;
    }
    return ret;
}

HTTPC_RESULT HttpClientHeadRequest(HttpClient *client, const char *url, HttpClientData *clientData)
{
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    int ret = HttpClientCommon(client, url, HTTP_HEAD, clientData);
    while ((0 == ret) && (1 == clientData->isRedirected)) {
        ret = HttpClientCommon(client, clientData->redirectUrl, HTTP_HEAD, clientData);
    }

    if (clientData->redirectUrl != NULL) {
        free(clientData->redirectUrl);
        clientData->redirectUrl = NULL;
    }
    return ret;
}

HTTPC_RESULT HttpClientPostRequest(HttpClient *client, const char *url, HttpClientData *clientData)
{
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    int ret = HttpClientCommon(client, url, HTTP_POST, clientData);

    while ((0 == ret) && (1 == clientData->isRedirected)) {
        ret = HttpClientCommon(client, clientData->redirectUrl, HTTP_POST, clientData);
    }
    if (clientData->redirectUrl != NULL) {
        free(clientData->redirectUrl);
        clientData->redirectUrl = NULL;
    }
    return ret;
}

HTTPC_RESULT HttpClientPutRequest(HttpClient *client, const char *url, HttpClientData *clientData)
{
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    int ret = HttpClientCommon(client, url, HTTP_PUT, clientData);

    while ((0 == ret) && (1 == clientData->isRedirected)) {
        ret = HttpClientCommon(client, clientData->redirectUrl, HTTP_PUT, clientData);
    }

    if (clientData->redirectUrl != NULL) {
        free(clientData->redirectUrl);
        clientData->redirectUrl = NULL;
    }

    return ret;
}

HTTPC_RESULT HttpClientDelete(HttpClient *client, const char *url, HttpClientData *clientData)
{
    if (client == NULL || url == NULL || clientData == NULL) {
        return HTTP_EARG;
    }
    return HttpClientCommon(client, url, HTTP_DELETE, clientData);
}

#ifdef __cplusplus
}
#endif