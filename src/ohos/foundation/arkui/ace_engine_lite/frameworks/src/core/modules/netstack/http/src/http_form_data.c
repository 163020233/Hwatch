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
#include <string.h>
#include <errno.h>

#include "securec.h"
#include "httpclient.h"
#include "http_opts.h"
#include "http_wrappers.h"
#include "http_form_data.h"

#ifdef CONFIG_HTTP_FORM_DATA

#ifdef __cplusplus
extern "C" {
#endif

static FormDataInfo g_formdataInfo[CLIENT_FORM_DATA_NUM] = {0};

#define FILE_FORMAT_START "\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\n"
#define FILE_FORMAT_CONTENT_TYPE_START \
    "\r\nContent-Disposition: form-data; name=\"%s\"; filename=\"%s\"\r\nContent-Type: %s\r\n\r\n"

#define TEXT_FORMAT "\r\nContent-Disposition: form-data; name=\"%s\"\r\n\r\n%s\r\n"
#define TEXT_CONTENT_TYPE_FORMAT "\r\nContent-Disposition: form-data; name=\"%s\"\r\nContent-Type:%s\r\n\r\n%s\r\n"

static void HttpFormDataClear(HttpFormDataNode *curNode)
{
    HttpFormDataNode *next = NULL;
    while (curNode != NULL) {
        next = curNode->next;
        curNode->next = NULL;
        if (curNode->formatContent != NULL) {
            free(curNode->formatContent);
            curNode->formatContent = NULL;
        }
        free(curNode);
        curNode = next;
    }
}

static FormDataInfo *HttpFoundFormdataInfo(HttpClientData *clientData)
{
    int i;

    for (i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if ((g_formdataInfo[i].clientData == clientData) && (g_formdataInfo[i].isUsed == 1)) {
            break;
        }
    }

    if (i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &g_formdataInfo[i];
}

static FormDataInfo *HttpFoundEmptyFormdataInfo(void)
{
    int i;

    for (i = 0; i < CLIENT_FORM_DATA_NUM; i++) {
        if (g_formdataInfo[i].isUsed == 0) {
            break;
        }
    }

    if (i == CLIENT_FORM_DATA_NUM) {
        return NULL;
    }

    return &g_formdataInfo[i];
}

static int HttpGetFileNameOffset(char *path)
{
    char *ptr = path;
    int offset = 0;
    int i = 0;

    while (*ptr != '\0') {
        i++;
        if (*ptr == '/') {
            offset = i;
        }
        ptr++;
    }
    return offset;
}

void HttpClientClearFormData(HttpClientData *clientData)
{
    if (clientData == NULL) {
        return;
    }
    FormDataInfo *dataInfo = HttpFoundFormdataInfo(clientData);
    if (dataInfo == NULL) {
        return;
    }

    HttpFormDataClear(dataInfo->formDataNode);
    (void)memset_s(dataInfo, sizeof(FormDataInfo), 0, sizeof(FormDataInfo));
}

static int HttpFormdataAddList(HttpClientData *clientData, HttpFormDataNode **preNode, HttpFormDataNode **lastNode)
{
    FormDataInfo *dataInfo = NULL;
    HttpFormDataNode *current = NULL;

    if ((dataInfo = HttpFoundFormdataInfo(clientData)) == NULL) {
        if ((dataInfo = HttpFoundEmptyFormdataInfo()) == NULL) {
            http_err("%s:%d found no clientData info\n", __func__, __LINE__);
            return HTTP_EARG;
        }
    }
    if (dataInfo->isUsed == 0) {
        dataInfo->isUsed = 1;
        dataInfo->clientData = clientData;
        dataInfo->formDataNode = (HttpFormDataNode *)malloc(sizeof(HttpFormDataNode));
        if (dataInfo->formDataNode == NULL) {
            dataInfo->isUsed = 0;
            http_err("%s:%d malloc failed\n", __func__, __LINE__);
            return HTTP_ENOBUFS;
        }

        *preNode = dataInfo->formDataNode;
        *lastNode = dataInfo->formDataNode;
    } else {
        current = dataInfo->formDataNode;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = (HttpFormDataNode *)malloc(sizeof(HttpFormDataNode));
        if (current->next == NULL) {
            http_err("%s:%d malloc failed\n", __func__, __LINE__);
            return HTTP_ENOBUFS;
        }
        *preNode = current;
        *lastNode = current->next;
    }
    return HTTP_SUCCESS;
}

int HttpClientFormdataAddText(HttpClientData *clientData, char *contentType, char *name, char *data, int dataLen)
{
    int bufLen;
    int ret;
    HttpFormDataNode *preNode = NULL;
    HttpFormDataNode *current = NULL;

    if ((name == NULL) || (data == NULL) || (dataLen == 0)) {
        http_err("%s invalid params\n", __func__);
        return HTTP_EARG;
    }

    if (strlen(data) > (uint32_t)dataLen) {
        http_err("%s invalid dataLen %d strlen data %u\n", __func__, dataLen, strlen(data));
        return HTTP_EARG;
    }

    ret = HttpFormdataAddList(clientData, &preNode, &current);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    (void)memset_s(current, sizeof(HttpFormDataNode), 0, sizeof(HttpFormDataNode));
    if (contentType == NULL) {
        bufLen = strlen(TEXT_FORMAT) + strlen(name) + strlen(data) + 1;
    } else {
        bufLen = strlen(TEXT_CONTENT_TYPE_FORMAT) + strlen(name) + strlen(data) + strlen(contentType) + 1;
    }
    current->formatContent = (char *)malloc(bufLen + 1);
    if (current->formatContent == NULL) {
        http_err("%s:%d malloc failed\n", __func__, __LINE__);
        free(current);
        return HTTP_ENOBUFS;
    }
    (void)memset_s(current->formatContent, (bufLen + 1), 0, (bufLen + 1));

    if (contentType == NULL) {
        current->formatLen = sprintf_s(current->formatContent, (bufLen + 1), TEXT_FORMAT, name, data);
    } else {
        current->formatLen =
            sprintf_s(current->formatContent, (bufLen + 1), TEXT_CONTENT_TYPE_FORMAT, name, contentType, data);
    }
    if (current->formatLen < 0) {
        free(current->formatContent);
        current->formatContent = NULL;
        free(current);
        return HTTP_EUNKNOW;
    }
    return HTTP_SUCCESS;
}

#if CONFIG_HTTP_FILE_OPERATE
int HttpClientFormdataAddfile(HttpClientData *clientData, char *name, char *contentType, char *filePath)
{
    int bufLen;
    int ret;
    HttpFormDataNode *preNode = NULL;
    HttpFormDataNode *current = NULL;

    if (clientData == NULL || name == NULL || filePath == NULL) {
        return HTTP_EARG;
    }

    ret = HttpFormdataAddList(clientData, &preNode, &current);
    if (ret != HTTP_SUCCESS) {
        return ret;
    }

    (void)memset_s(current, sizeof(HttpFormDataNode), 0, sizeof(HttpFormDataNode));
    size_t filePathLen = strlen(filePath);
    if (filePathLen > FORM_DATA_PATH_MAXLEN - 1) {
        free(current);
        return HTTP_EARG;
    }
    if (contentType == NULL) {
        bufLen = strlen(FILE_FORMAT_START) + strlen(name) + strlen(filePath) - HttpGetFileNameOffset(filePath) + 1;
    } else {
        bufLen = strlen(FILE_FORMAT_CONTENT_TYPE_START) + strlen(name) + strlen(filePath) -
                 HttpGetFileNameOffset(filePath) + strlen(contentType) + 1;
    }
    current->formatContent = (char *)malloc(bufLen + 1);
    if (current->formatContent == NULL) {
        http_err("formatContent malloc failed\r\n");
        free(current);
        return HTTP_ENOBUFS;
    }
    (void)memset_s(current->formatContent, (bufLen + 1), 0, (bufLen + 1));

    current->isFile = 1;

    if (memcpy_s(current->filePath, FORM_DATA_PATH_MAXLEN, filePath, filePathLen) != 0) {
        http_err("memcpy fail\n");
        free(current->formatContent);
        current->formatContent = NULL;
        free(current);
        return HTTP_ENOBUFS;
    }
    if (contentType == NULL) {
        current->formatLen = sprintf_s(current->formatContent,
            (bufLen + 1),
            FILE_FORMAT_START,
            name,
            (char *)filePath + HttpGetFileNameOffset(filePath));
    } else {
        current->formatLen = sprintf_s(current->formatContent,
            (bufLen + 1),
            FILE_FORMAT_CONTENT_TYPE_START,
            name,
            (char *)filePath + HttpGetFileNameOffset(filePath),
            contentType);
    }
    if (current->formatLen < 0) {
        free(current->formatContent);
        current->formatContent = NULL;
        free(current);
        return HTTP_EUNKNOW;
    }
    current->formatLen = strlen(current->formatContent);
    return HTTP_SUCCESS;
}

static int HttpSendBuffer(HttpClient *client, char *fileData, int readSize)
{
    int ret = HTTP_SUCCESS;
#if CONFIG_HTTP_SECURE
    if (client->isHttp == false) {
        if (HttpSslSendWrapper(client, fileData, readSize) != readSize) {
            ret = HTTP_ESEND;
            return ret;
        }
    } else
#endif
    {
        ret = HttpTcpSendWrapper(client, fileData, readSize);
        if (ret > 0) {
#ifdef HTTP_DEBUG
            http_debug("Written %d bytes, socket = %d\n", ret, client->socket);
#endif
        } else if (ret == 0) {
            http_err("ret == 0,Connection was closed by server\n");
            ret = HTTP_ECLSD;
            return ret;
        } else {
            return ret;
        }
    }
    return ret;
}

static int SendFormDataFile(HttpClient *client, HttpFormDataNode *curNode)
{
    int readSize = 0;
    int ret;
    char *fileData;
    char *resolvePath;

    resolvePath = realpath(curNode->filePath, NULL);
    if (resolvePath == NULL) {
        http_err("File path invalid, %s!", curNode->filePath);
        return HTTP_EUNKNOW;
    }

    fileData = malloc(HTTPCLIENT_MAX_FILE_DATA_LEN);
    if (fileData == NULL) {
        free(resolvePath);
        return HTTP_ENOBUFS;
    }
    (void)memset_s(fileData, HTTPCLIENT_MAX_FILE_DATA_LEN, 0, HTTPCLIENT_MAX_FILE_DATA_LEN);

    FILE *fd = fopen(resolvePath, "rb");
    if (fd == NULL) {
        http_err("%s: open file(%s) failed errno=%d\n", __func__, resolvePath, errno);
        free(fileData);
        free(resolvePath);
        return HTTP_EUNKNOW;
    }

    while ((readSize = (int)fread(fileData, 1, HTTPCLIENT_MAX_FILE_DATA_LEN, fd)) > 0) {
        ret = HttpSendBuffer(client, fileData, readSize);
        if (ret != HTTP_SUCCESS) {
            goto END;
        }
        (void)memset_s(fileData, HTTPCLIENT_MAX_FILE_DATA_LEN, 0, HTTPCLIENT_MAX_FILE_DATA_LEN);
    }

    if (ferror(fd)) {
        http_err("fread failed\n");
        ret = HTTP_EUNKNOW;
        goto END;
    }
    ret = HTTP_SUCCESS;
END:
    (void)fclose(fd);
    free(fileData);
    free(resolvePath);
    return ret;
}
#endif

int HttpClientFormdataLen(HttpClientData *clientData)
{
    int totalLen = 0;
    FormDataInfo *dataInfo = NULL;
    HttpFormDataNode *current = NULL;
    char *resolvePath = NULL;

    dataInfo = HttpFoundFormdataInfo(clientData);
    if ((NULL == dataInfo) || (0 == dataInfo->isUsed) || dataInfo->formDataNode == NULL) {
        return 0;
    }
    current = dataInfo->formDataNode;
    do {
#if CONFIG_HTTP_FILE_OPERATE
        if (current->isFile == 1) {
            int fileSize;
            resolvePath = realpath(current->filePath, NULL);
            if (resolvePath == NULL) {
                http_err("File path invalid, %s!", current->filePath);
                return HTTP_EUNKNOW;
            }
            FILE *fd = fopen(resolvePath, "rb");
            if (fd == NULL) {
                http_err("open file(%s) failed errno=%d\n", resolvePath, errno);
                free(resolvePath);
                return HTTP_EUNKNOW;
            }

            fseek(fd, 0, SEEK_END);
            fileSize = ftell(fd);
            fseek(fd, 0, SEEK_SET);
            fclose(fd);
            if (fileSize > HTTPCLIENT_SEND_BUF_SIZE || fileSize < 0) {
                free(resolvePath);
                return HTTP_EUNKNOW;
            }
            totalLen += fileSize;
            free(resolvePath);
        }
#endif

        totalLen += current->formatLen;
        totalLen += strlen(FORM_DATA_BOUNDARY_START);
        current = current->next;
    } while (current != NULL);

    return totalLen;
}

#if CONFIG_HTTP_SECURE

static int SslSendFormdata(HttpClient *client, HttpClientData *clientData)
{
    int ret;
    FormDataInfo *dataInfo = NULL;
    HttpFormDataNode *current = NULL;

    dataInfo = HttpFoundFormdataInfo(clientData);
    if ((NULL == dataInfo) || (0 == dataInfo->isUsed)) {
        return HTTP_SUCCESS;
    }

    current = dataInfo->formDataNode;

    while (current != NULL) {
        ret = HttpSslSendWrapper(client, FORM_DATA_BOUNDARY_START, strlen(FORM_DATA_BOUNDARY_START));
        if (ret <= 0) {
            return HTTP_ESEND;
        }

        ret = HttpSslSendWrapper(client, current->formatContent, current->formatLen);
        if (ret <= 0) {
            return HTTP_ESEND;
        }
#if CONFIG_HTTP_FILE_OPERATE
        if (current->isFile == 1) {
            ret = SendFormDataFile(client, current);
            if (ret != HTTP_SUCCESS) {
                return ret;
            }
        }
#endif
        current = current->next;
    }

    ret = HttpSslSendWrapper(client, FORM_DATA_BOUNDARY_END, strlen(FORM_DATA_BOUNDARY_END));
    if (ret <= 0) {
        return HTTP_ESEND;
    }

    return HTTP_SUCCESS;
}
#endif

int HttpClientSendFormdata(HttpClient *client, HttpClientData *clientData)
{
    int ret;
    FormDataInfo *dataInfo = NULL;
    HttpFormDataNode *current = NULL;
#if CONFIG_HTTP_SECURE
    if (client->isHttp == false) {
        return SslSendFormdata(client, clientData);
    }
#endif
    dataInfo = HttpFoundFormdataInfo(clientData);
    if ((NULL == dataInfo) || (0 == dataInfo->isUsed)) {
        return HTTP_SUCCESS;
    }

    current = dataInfo->formDataNode;

    while (current != NULL) {
        ret = HttpTcpSendWrapper(client, FORM_DATA_BOUNDARY_START, strlen(FORM_DATA_BOUNDARY_START));
        if (ret <= 0) {
            return HTTP_ESEND;
        }
        ret = HttpTcpSendWrapper(client, current->formatContent, current->formatLen);
        if (ret <= 0) {
            return HTTP_ESEND;
        }
#if CONFIG_HTTP_FILE_OPERATE
        if (current->isFile == 1) {
            ret = SendFormDataFile(client, current);
            if (ret != HTTP_SUCCESS) {
                return ret;
            }
        }
#endif
        current = current->next;
    }

    ret = HttpTcpSendWrapper(client, FORM_DATA_BOUNDARY_END, strlen(FORM_DATA_BOUNDARY_END));
    if (ret <= 0) {
        return HTTP_ESEND;
    }
    return HTTP_SUCCESS;
}

#ifdef __cplusplus
}
#endif

#endif